/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2007 Carlos Garcia Campos <carlosgc@gnome.org>
 *
 * Papers is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Papers is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <config.h>

#include <config.h>
#include <glib/gi18n-lib.h>
#include <libspectre/spectre.h>
#include <stdlib.h>

#include "pps-spectre.h"

#include "pps-document-misc.h"
#include "pps-file-exporter.h"

struct _PSDocument {
	PpsDocument object;

	SpectreDocument *doc;
	SpectreExporter *exporter;
};

struct _PSDocumentClass {
	PpsDocumentClass parent_class;
};

static void ps_document_file_exporter_iface_init (PpsFileExporterInterface *iface);

G_DEFINE_TYPE_WITH_CODE (PSDocument, ps_document, PPS_TYPE_DOCUMENT, G_IMPLEMENT_INTERFACE (PPS_TYPE_FILE_EXPORTER, ps_document_file_exporter_iface_init))

/* PSDocument */
static void
ps_document_init (PSDocument *ps_document)
{
}

static void
ps_document_dispose (GObject *object)
{
	PSDocument *ps = PS_DOCUMENT (object);

	if (ps->doc) {
		spectre_document_free (ps->doc);
		ps->doc = NULL;
	}

	if (ps->exporter) {
		spectre_exporter_free (ps->exporter);
		ps->exporter = NULL;
	}

	G_OBJECT_CLASS (ps_document_parent_class)->dispose (object);
}

/* PpsDocumentIface */
static gboolean
ps_document_load (PpsDocument *document,
                  const char *uri,
                  GError **error)
{
	PSDocument *ps = PS_DOCUMENT (document);
	gchar *filename;

	filename = g_filename_from_uri (uri, NULL, error);
	if (!filename)
		return FALSE;

	ps->doc = spectre_document_new ();

	spectre_document_load (ps->doc, filename);
	if (spectre_document_status (ps->doc)) {
		gchar *filename_dsp;

		filename_dsp = g_filename_display_name (filename);
		g_set_error (error,
		             G_FILE_ERROR,
		             G_FILE_ERROR_FAILED,
		             _ ("Failed to load document “%s”"),
		             filename_dsp);
		g_free (filename_dsp);
		g_free (filename);

		return FALSE;
	}

	g_free (filename);

	return TRUE;
}

static gboolean
ps_document_save (PpsDocument *document,
                  const char *uri,
                  GError **error)
{
	PSDocument *ps = PS_DOCUMENT (document);
	gchar *filename;

	filename = g_filename_from_uri (uri, NULL, error);
	if (!filename)
		return FALSE;

	spectre_document_save (ps->doc, filename);
	if (spectre_document_status (ps->doc)) {
		gchar *filename_dsp;

		filename_dsp = g_filename_display_name (filename);
		g_set_error (error,
		             G_FILE_ERROR,
		             G_FILE_ERROR_FAILED,
		             _ ("Failed to save document “%s”"),
		             filename_dsp);
		g_free (filename_dsp);
		g_free (filename);

		return FALSE;
	}

	g_free (filename);

	return TRUE;
}

static int
ps_document_get_n_pages (PpsDocument *document)
{
	PSDocument *ps = PS_DOCUMENT (document);

	return spectre_document_get_n_pages (ps->doc);
}

static PpsPage *
ps_document_get_page (PpsDocument *document,
                      gint index)
{
	PSDocument *ps = PS_DOCUMENT (document);
	SpectrePage *ps_page;
	PpsPage *page;

	ps_page = spectre_document_get_page (ps->doc, index);
	page = pps_page_new (index);
	page->backend_page = (PpsBackendPage) ps_page;
	page->backend_destroy_func = (PpsBackendPageDestroyFunc) spectre_page_free;

	return page;
}

static gint
get_page_rotation (SpectrePage *page)
{
	switch (spectre_page_get_orientation (page)) {
	default:
	case SPECTRE_ORIENTATION_PORTRAIT:
		return 0;
	case SPECTRE_ORIENTATION_LANDSCAPE:
		return 90;
	case SPECTRE_ORIENTATION_REVERSE_PORTRAIT:
		return 180;
	case SPECTRE_ORIENTATION_REVERSE_LANDSCAPE:
		return 270;
	}

	return 0;
}

static void
ps_document_get_page_size (PpsDocument *document,
                           PpsPage *page,
                           double *width,
                           double *height)
{
	SpectrePage *ps_page;
	gdouble page_width, page_height;
	gint pwidth, pheight;
	gint rotate;

	ps_page = (SpectrePage *) page->backend_page;

	spectre_page_get_size (ps_page, &pwidth, &pheight);

	rotate = get_page_rotation (ps_page);
	if (rotate == 90 || rotate == 270) {
		page_height = pwidth;
		page_width = pheight;
	} else {
		page_width = pwidth;
		page_height = pheight;
	}

	if (width) {
		*width = page_width;
	}

	if (height) {
		*height = page_height;
	}
}

static char *
ps_document_get_page_label (PpsDocument *document,
                            PpsPage *page)
{
	const gchar *label = spectre_page_get_label ((SpectrePage *) page->backend_page);
	gchar *utf8;

	if (!label)
		return NULL;

	if (g_utf8_validate (label, -1, NULL))
		return g_strdup (label);

	/* Try with latin1 and ASCII encondings */
	utf8 = g_convert (label, -1, "utf-8", "latin1", NULL, NULL, NULL);
	if (!utf8)
		utf8 = g_convert (label, -1, "utf-8", "ASCII", NULL, NULL, NULL);

	return utf8;
}

static PpsDocumentInfo *
ps_document_get_info (PpsDocument *document)
{
	PSDocument *ps = PS_DOCUMENT (document);
	PpsDocumentInfo *info;
	const gchar *creator;
	SpectrePage *ps_page;
	gint width, height;

	info = pps_document_info_new ();
	info->fields_mask |= PPS_DOCUMENT_INFO_TITLE |
	                     PPS_DOCUMENT_INFO_FORMAT |
	                     PPS_DOCUMENT_INFO_CREATOR |
	                     PPS_DOCUMENT_INFO_N_PAGES |
	                     PPS_DOCUMENT_INFO_PAPER_SIZE;

	creator = spectre_document_get_creator (ps->doc);

	ps_page = spectre_document_get_page (ps->doc, 0);
	spectre_page_get_size (ps_page, &width, &height);
	spectre_page_free (ps_page);

	info->title = g_strdup (spectre_document_get_title (ps->doc));
	info->format = g_strdup (spectre_document_get_format (ps->doc));
	info->creator = g_strdup (creator ? creator : spectre_document_get_for (ps->doc));
	info->n_pages = spectre_document_get_n_pages (ps->doc);
	info->paper_width = width / 72.0f * 25.4f;
	info->paper_height = height / 72.0f * 25.4f;

	return info;
}

static gboolean
ps_document_get_backend_info (PpsDocument *document,
                              PpsDocumentBackendInfo *info)
{
	info->name = "libspectre";
	info->version = SPECTRE_VERSION_STRING;

	return TRUE;
}

static cairo_surface_t *
ps_document_render (PpsDocument *document,
                    PpsRenderContext *rc)
{
	SpectrePage *ps_page;
	SpectreRenderContext *src;
	gint width_points;
	gint height_points;
	gint width, height;
	gint swidth, sheight;
	guchar *data = NULL;
	gint stride;
	gint rotation;
	cairo_surface_t *surface;
	static const cairo_user_data_key_t key;

	ps_page = (SpectrePage *) rc->page->backend_page;

	spectre_page_get_size (ps_page, &width_points, &height_points);

	pps_render_context_compute_transformed_size (rc, width_points, height_points,
	                                             &width, &height);

	rotation = (rc->rotation + get_page_rotation (ps_page)) % 360;

	if (rotation == 90 || rotation == 270) {
		swidth = height;
		sheight = width;
	} else {
		swidth = width;
		sheight = height;
	}

	src = spectre_render_context_new ();
	spectre_render_context_set_scale (src,
	                                  (gdouble) swidth / width_points,
	                                  (gdouble) sheight / height_points);
	spectre_render_context_set_rotation (src, rotation);
	spectre_page_render (ps_page, src, &data, &stride);
	spectre_render_context_free (src);

	if (!data) {
		return NULL;
	}

	if (spectre_page_status (ps_page)) {
		g_warning ("%s", spectre_status_to_string (spectre_page_status (ps_page)));
		g_free (data);

		return NULL;
	}

	surface = cairo_image_surface_create_for_data (data,
	                                               CAIRO_FORMAT_RGB24,
	                                               width, height,
	                                               stride);
	cairo_surface_set_user_data (surface, &key,
	                             data, (cairo_destroy_func_t) g_free);
	return surface;
}

static void
ps_document_class_init (PSDocumentClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	PpsDocumentClass *pps_document_class = PPS_DOCUMENT_CLASS (klass);

	object_class->dispose = ps_document_dispose;

	pps_document_class->load = ps_document_load;
	pps_document_class->save = ps_document_save;
	pps_document_class->get_n_pages = ps_document_get_n_pages;
	pps_document_class->get_page = ps_document_get_page;
	pps_document_class->get_page_size = ps_document_get_page_size;
	pps_document_class->get_page_label = ps_document_get_page_label;
	pps_document_class->get_info = ps_document_get_info;
	pps_document_class->get_backend_info = ps_document_get_backend_info;
	pps_document_class->render = ps_document_render;
}

/* PpsFileExporterIface */
static void
ps_document_file_exporter_begin (PpsFileExporter *exporter,
                                 PpsFileExporterContext *fc)
{
	PSDocument *ps = PS_DOCUMENT (exporter);

	if (ps->exporter)
		spectre_exporter_free (ps->exporter);

	switch (fc->format) {
	case PPS_FILE_FORMAT_PS:
		ps->exporter =
		    spectre_exporter_new (ps->doc,
		                          SPECTRE_EXPORTER_FORMAT_PS);
		break;
	case PPS_FILE_FORMAT_PDF:
		ps->exporter =
		    spectre_exporter_new (ps->doc,
		                          SPECTRE_EXPORTER_FORMAT_PDF);
		break;
	default:
		g_assert_not_reached ();
	}

	spectre_exporter_begin (ps->exporter, fc->filename);
}

static void
ps_document_file_exporter_do_page (PpsFileExporter *exporter,
                                   PpsRenderContext *rc)
{
	PSDocument *ps = PS_DOCUMENT (exporter);

	spectre_exporter_do_page (ps->exporter, rc->page->index);
}

static void
ps_document_file_exporter_end (PpsFileExporter *exporter)
{
	PSDocument *ps = PS_DOCUMENT (exporter);

	spectre_exporter_end (ps->exporter);
}

static PpsFileExporterCapabilities
ps_document_file_exporter_get_capabilities (PpsFileExporter *exporter)
{
	return PPS_FILE_EXPORTER_CAN_PAGE_SET |
	       PPS_FILE_EXPORTER_CAN_COPIES |
	       PPS_FILE_EXPORTER_CAN_COLLATE |
	       PPS_FILE_EXPORTER_CAN_REVERSE |
	       PPS_FILE_EXPORTER_CAN_GENERATE_PS |
	       PPS_FILE_EXPORTER_CAN_GENERATE_PDF;
}

static void
ps_document_file_exporter_iface_init (PpsFileExporterInterface *iface)
{
	iface->begin = ps_document_file_exporter_begin;
	iface->do_page = ps_document_file_exporter_do_page;
	iface->end = ps_document_file_exporter_end;
	iface->get_capabilities = ps_document_file_exporter_get_capabilities;
}

GType
pps_backend_query_type (void)
{
	return PS_TYPE_DOCUMENT;
}
