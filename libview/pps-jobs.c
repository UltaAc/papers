/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2008 Carlos Garcia Campos <carlosgc@gnome.org>
 *  Copyright (C) 2005 Red Hat, Inc
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

#include "cairo.h"
#include "pps-jobs.h"
#include "pps-document-links.h"
#include "pps-document-images.h"
#include "pps-document-forms.h"
#include "pps-file-exporter.h"
#include "pps-document-factory.h"
#include "pps-document-misc.h"
#include "pps-file-helpers.h"
#include "pps-document-fonts.h"
#include "pps-document-security.h"
#include "pps-document-find.h"
#include "pps-document-layers.h"
#include "pps-document-print.h"
#include "pps-document-annotations.h"
#include "pps-document-attachments.h"
#include "pps-document-media.h"
#include "pps-document-text.h"
#include "pps-outlines.h"
#include "pps-debug.h"

#include <errno.h>
#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>
#include <unistd.h>
#include <fcntl.h>

static void pps_job_init                       (PpsJob                    *job);
static void pps_job_class_init                 (PpsJobClass               *class);
static void pps_job_links_init                 (PpsJobLinks               *job);
static void pps_job_links_class_init           (PpsJobLinksClass          *class);
static void pps_job_attachments_init           (PpsJobAttachments         *job);
static void pps_job_attachments_class_init     (PpsJobAttachmentsClass    *class);
static void pps_job_annots_init                (PpsJobAnnots              *job);
static void pps_job_annots_class_init          (PpsJobAnnotsClass         *class);
static void pps_job_render_texture_init        (PpsJobRenderTexture         *job);
static void pps_job_render_texture_class_init  (PpsJobRenderTextureClass    *class);
static void pps_job_page_data_init             (PpsJobPageData            *job);
static void pps_job_page_data_class_init       (PpsJobPageDataClass       *class);
static void pps_job_thumbnail_texture_init       (PpsJobThumbnailTexture      *job);
static void pps_job_thumbnail_texture_class_init (PpsJobThumbnailTextureClass *class);
static void pps_job_load_init                  (PpsJobLoad                *job);
static void pps_job_load_class_init            (PpsJobLoadClass           *class);
static void pps_job_save_init                  (PpsJobSave                *job);
static void pps_job_save_class_init            (PpsJobSaveClass           *class);
static void pps_job_find_init                  (PpsJobFind                *job);
static void pps_job_find_class_init            (PpsJobFindClass           *class);
static void pps_job_layers_init                (PpsJobLayers              *job);
static void pps_job_layers_class_init          (PpsJobLayersClass         *class);
static void pps_job_export_init                (PpsJobExport              *job);
static void pps_job_export_class_init          (PpsJobExportClass         *class);
static void pps_job_print_init                 (PpsJobPrint               *job);
static void pps_job_print_class_init           (PpsJobPrintClass          *class);

enum {
	PROP_0,
	PROP_DOCUMENT,
};

enum {
	CANCELLED,
	FINISHED,
	LAST_SIGNAL
};

enum {
	FIND_UPDATED,
	FIND_LAST_SIGNAL
};

static guint job_signals[LAST_SIGNAL] = { 0 };
static guint job_find_signals[FIND_LAST_SIGNAL] = { 0 };

G_DEFINE_ABSTRACT_TYPE (PpsJob, pps_job, G_TYPE_OBJECT)
G_DEFINE_TYPE (PpsJobLinks, pps_job_links, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobAttachments, pps_job_attachments, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobAnnots, pps_job_annots, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobRenderTexture, pps_job_render_texture, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobPageData, pps_job_page_data, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobThumbnailTexture, pps_job_thumbnail_texture, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobFonts, pps_job_fonts, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobLoad, pps_job_load, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobSave, pps_job_save, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobFind, pps_job_find, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobLayers, pps_job_layers, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobExport, pps_job_export, PPS_TYPE_JOB)
G_DEFINE_TYPE (PpsJobPrint, pps_job_print, PPS_TYPE_JOB)

/* PpsJob */
static void
pps_job_init (PpsJob *job)
{
	job->cancellable = g_cancellable_new ();
}

static void
pps_job_dispose (GObject *object)
{
	PpsJob *job = PPS_JOB (object);

	pps_debug_message (DEBUG_JOBS, "disposing %s (%p)", PPS_GET_TYPE_NAME(job), job);

	g_clear_object (&job->document);
	g_clear_object (&job->cancellable);
	g_clear_error (&job->error);

	(* G_OBJECT_CLASS (pps_job_parent_class)->dispose) (object);
}

static void
pps_job_set_property (GObject      *object,
		     guint         prop_id,
		     const GValue *value,
		     GParamSpec   *pspec)
{
	PpsJob *job = PPS_JOB (object);

	switch (prop_id) {
	case PROP_DOCUMENT:
		job->document = g_value_dup_object (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_job_class_init (PpsJobClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->dispose = pps_job_dispose;
	g_object_class->set_property = pps_job_set_property;

	job_signals[CANCELLED] =
		g_signal_new ("cancelled",
			      PPS_TYPE_JOB,
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (PpsJobClass, cancelled),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);
	job_signals [FINISHED] =
		g_signal_new ("finished",
			      PPS_TYPE_JOB,
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (PpsJobClass, finished),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);

	g_object_class_install_property (g_object_class,
					 PROP_DOCUMENT,
					 g_param_spec_object ("document",
							      "Document",
							      "The document",
							      PPS_TYPE_DOCUMENT,
							      G_PARAM_WRITABLE |
							      G_PARAM_CONSTRUCT_ONLY |
							      G_PARAM_STATIC_STRINGS));
}

static gboolean
emit_finished (PpsJob *job)
{
	pps_debug_message (DEBUG_JOBS, "%s (%p)", PPS_GET_TYPE_NAME (job), job);

	job->idle_finished_id = 0;

	if (job->cancelled)
		pps_debug_message (DEBUG_JOBS, "%s (%p) job was cancelled, do not emit finished", PPS_GET_TYPE_NAME (job), job);
	else
		g_signal_emit (job, job_signals[FINISHED], 0);

	return G_SOURCE_REMOVE;
}

static void
pps_job_emit_finished (PpsJob *job)
{
	pps_debug_message (DEBUG_JOBS, "%s (%p)", PPS_GET_TYPE_NAME (job), job);

	if (g_cancellable_is_cancelled (job->cancellable)) {
		pps_debug_message (DEBUG_JOBS, "%s (%p) job was cancelled, returning", PPS_GET_TYPE_NAME (job), job);
		return;
	}

	job->finished = TRUE;

	job->idle_finished_id =
		g_idle_add_full (G_PRIORITY_DEFAULT_IDLE,
				 (GSourceFunc)emit_finished,
				 g_object_ref (job),
				 (GDestroyNotify)g_object_unref);
}

gboolean
pps_job_run (PpsJob *job)
{
	PpsJobClass *class = PPS_JOB_GET_CLASS (job);
	gboolean ret;

	PPS_PROFILER_START (PPS_GET_TYPE_NAME (job), g_strdup("running"));
	ret = class->run (job);
	PPS_PROFILER_STOP ();
	return ret;
}

void
pps_job_cancel (PpsJob *job)
{
	if (job->cancelled)
		return;

	pps_debug_message (DEBUG_JOBS, "job %s (%p) cancelled", PPS_GET_TYPE_NAME (job), job);

	/* This should never be called from a thread */
	job->cancelled = TRUE;
	g_cancellable_cancel (job->cancellable);

        if (job->finished && job->idle_finished_id == 0)
                return;

	g_signal_emit (job, job_signals[CANCELLED], 0);
}

void
pps_job_failed (PpsJob       *job,
	       GQuark       domain,
	       gint         code,
	       const gchar *format,
	       ...)
{
	va_list args;
	gchar  *message;

	if (job->failed || job->finished)
		return;

	pps_debug_message (DEBUG_JOBS, "job %s (%p) failed", PPS_GET_TYPE_NAME (job), job);

	job->failed = TRUE;

	va_start (args, format);
	message = g_strdup_vprintf (format, args);
	va_end (args);

	job->error = g_error_new_literal (domain, code, message);
	g_free (message);

	pps_job_emit_finished (job);
}

/**
 * pps_job_failed_from_error: (rename-to pps_job_failed)
 * @job: an #PpsJob
 * @error: a #GError
 */
void
pps_job_failed_from_error (PpsJob  *job,
			  GError *error)
{
	if (job->failed || job->finished)
		return;

	pps_debug_message (DEBUG_JOBS, "job %s (%p) failed", PPS_GET_TYPE_NAME (job), job);

	job->failed = TRUE;
	job->error = g_error_copy (error);

	pps_job_emit_finished (job);
}

void
pps_job_succeeded (PpsJob *job)
{
	if (job->finished)
		return;

	pps_debug_message (DEBUG_JOBS, "job %s (%p) succeeded", PPS_GET_TYPE_NAME (job), job);

	job->failed = FALSE;
	pps_job_emit_finished (job);
}

gboolean
pps_job_is_finished (PpsJob *job)
{
	return job->finished;
}

/**
 * pps_job_is_failed:
 * @job: a #PpsJob
 * @error (nullable): (transfer none): the error to set if the job failed
 *
 * Returns: whether the job failed
 */
gboolean
pps_job_is_failed (PpsJob   *job,
		  GError **error)
{
	g_return_val_if_fail (job != NULL, FALSE);

	if (job->failed && error)
		*error = job->error;

	return job->failed;
}

/**
 * pps_job_get_document:
 * @job: an #PpsJob
 *
 * Returns: (transfer none): The #PpsDocument of this job.
 */
PpsDocument *
pps_job_get_document (PpsJob	 *job)
{
	g_return_val_if_fail (PPS_IS_JOB (job), NULL);

	return job->document;
}

/* PpsJobLinks */
static void
pps_job_links_init (PpsJobLinks *job)
{
}

static void
pps_job_links_dispose (GObject *object)
{
	PpsJobLinks *job;

	job = PPS_JOB_LINKS (object);

	g_clear_object (&job->model);

	(* G_OBJECT_CLASS (pps_job_links_parent_class)->dispose) (object);
}

static void
fill_page_labels (GListModel *model, PpsJob *job)
{
	PpsDocumentLinks *document_links;
	PpsLink          *link;
	gchar            *page_label;
	GListModel	 *children;
	guint items = g_list_model_get_n_items (model);

	for (int i = 0; i < items; i++) {
		PpsOutlines *outlines = g_list_model_get_item(model, i);

		g_object_get (outlines, "link", &link, "children", &children, NULL);


		if (!link)
			continue;;

		document_links = PPS_DOCUMENT_LINKS (job->document);
		page_label = pps_document_links_get_link_page_label (document_links, link);
		if (!page_label)
			continue;

		g_object_set (outlines, "label", page_label, NULL);

		g_free (page_label);
		g_object_unref (link);

		if (children) {
			g_assert (G_IS_LIST_MODEL (children));
			fill_page_labels (children, job);
		}
	}
}

static gboolean
pps_job_links_run (PpsJob *job)
{
	PpsJobLinks *job_links = PPS_JOB_LINKS (job);

	pps_debug_message (DEBUG_JOBS, NULL);

	pps_document_doc_mutex_lock ();
	job_links->model = pps_document_links_get_links_model (PPS_DOCUMENT_LINKS (job->document));
	pps_document_doc_mutex_unlock ();

	fill_page_labels (job_links->model, job);

	pps_job_succeeded (job);

	return FALSE;
}

static void
pps_job_links_class_init (PpsJobLinksClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_links_dispose;
	job_class->run = pps_job_links_run;
}

PpsJob *
pps_job_links_new (PpsDocument *document)
{
	PpsJob *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_LINKS,
			    "document", document,
			    NULL);

	return job;
}

/**
 * pps_job_links_get_model:
 * @job: #PpsJobLinks
 *
 * Get a #GtkTreeModel loaded with the links
 *
 * Return value: (transfer none): The #GtkTreeModel loaded
 *
 * Since: 3.6
 */
GListModel *
pps_job_links_get_model (PpsJobLinks *job)
{
	return job->model;
}

/* PpsJobAttachments */
static void
pps_job_attachments_init (PpsJobAttachments *job)
{
}

static void
pps_job_attachments_dispose (GObject *object)
{
	PpsJobAttachments *job = PPS_JOB_ATTACHMENTS (object);

	g_list_free_full (g_steal_pointer (&job->attachments), g_object_unref);

	(* G_OBJECT_CLASS (pps_job_attachments_parent_class)->dispose) (object);
}

static gboolean
pps_job_attachments_run (PpsJob *job)
{
	PpsJobAttachments *job_attachments = PPS_JOB_ATTACHMENTS (job);

	pps_debug_message (DEBUG_JOBS, NULL);

	pps_document_doc_mutex_lock ();
	job_attachments->attachments =
		pps_document_attachments_get_attachments (PPS_DOCUMENT_ATTACHMENTS (job->document));
	pps_document_doc_mutex_unlock ();

	pps_job_succeeded (job);

	return FALSE;
}

/**
 * pps_job_attachments_get_attachments:
 * @job_attachments: a #PpsJobAttachments
 *
 * Returns: (nullable) (transfer none) (element-type PpsAttachment): a list of #PpsAttachment objects
 */
GList *
pps_job_attachments_get_attachments (PpsJobAttachments *job_attachments)
{
	return job_attachments->attachments;
}

static void
pps_job_attachments_class_init (PpsJobAttachmentsClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_attachments_dispose;
	job_class->run = pps_job_attachments_run;
}

PpsJob *
pps_job_attachments_new (PpsDocument *document)
{
	PpsJob *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_ATTACHMENTS,
			    "document", document,
			    NULL);

	return job;
}

/* PpsJobAnnots */
static void
pps_job_annots_init (PpsJobAnnots *job)
{
}

static void
pps_job_annots_dispose (GObject *object)
{
	PpsJobAnnots *job = PPS_JOB_ANNOTS (object);

	g_list_free_full (g_steal_pointer (&job->annots), (GDestroyNotify) pps_mapping_list_unref);

	G_OBJECT_CLASS (pps_job_annots_parent_class)->dispose (object);
}

static gboolean
pps_job_annots_run (PpsJob *job)
{
	PpsJobAnnots *job_annots = PPS_JOB_ANNOTS (job);
	gint         i;

	pps_debug_message (DEBUG_JOBS, NULL);

	pps_document_doc_mutex_lock ();
	for (i = 0; i < pps_document_get_n_pages (job->document); i++) {
		PpsMappingList *mapping_list;
		PpsPage        *page;

		page = pps_document_get_page (job->document, i);
		mapping_list = pps_document_annotations_get_annotations (PPS_DOCUMENT_ANNOTATIONS (job->document),
									page);
		g_object_unref (page);

		if (mapping_list)
			job_annots->annots = g_list_prepend (job_annots->annots, mapping_list);
	}
	pps_document_doc_mutex_unlock ();

	job_annots->annots = g_list_reverse (job_annots->annots);

	pps_job_succeeded (job);

	return FALSE;
}

static void
pps_job_annots_class_init (PpsJobAnnotsClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_annots_dispose;
	job_class->run = pps_job_annots_run;
}

PpsJob *
pps_job_annots_new (PpsDocument *document)
{
	PpsJob *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_ANNOTS,
			    "document", document,
			    NULL);


	return job;
}

/* PpsJobRenderTexture */
static void
pps_job_render_texture_init (PpsJobRenderTexture *job)
{
}

static void
pps_job_render_texture_dispose (GObject *object)
{
	PpsJobRenderTexture *job = PPS_JOB_RENDER_TEXTURE (object);

	pps_debug_message (DEBUG_JOBS, "disposing job render: %d (%p)", job->page, job);

	g_clear_object (&job->texture);
	g_clear_object (&job->selection);
	g_clear_pointer (&job->selection_region, cairo_region_destroy);

	(* G_OBJECT_CLASS (pps_job_render_texture_parent_class)->dispose) (object);
}

static GdkTexture *
gdk_texture_new_for_surface (cairo_surface_t *surface)
{
	GdkTexture *texture;
	GBytes *bytes;

	g_return_val_if_fail (surface != NULL, NULL);
	g_return_val_if_fail (cairo_surface_get_type (surface) == CAIRO_SURFACE_TYPE_IMAGE, NULL);
	g_return_val_if_fail (cairo_image_surface_get_width (surface) > 0, NULL);
	g_return_val_if_fail (cairo_image_surface_get_height (surface) > 0, NULL);

	bytes = g_bytes_new_with_free_func (cairo_image_surface_get_data (surface),
					    cairo_image_surface_get_height (surface) * cairo_image_surface_get_stride (surface),
					    (GDestroyNotify)cairo_surface_destroy,
					    cairo_surface_reference (surface));

	texture = gdk_memory_texture_new (cairo_image_surface_get_width (surface),
					  cairo_image_surface_get_height (surface),
					  GDK_MEMORY_DEFAULT,
					  bytes,
					  cairo_image_surface_get_stride (surface));

	g_bytes_unref (bytes);

	return texture;
}

PpsJob *
pps_job_render_texture_new (PpsDocument   *document,
			 gint          page,
			 gint          rotation,
			 gdouble       scale,
			 gint          width,
			 gint          height)
{
	PpsJobRenderTexture *job;

	pps_debug_message (DEBUG_JOBS, "page: %d", page);

	job = g_object_new (PPS_TYPE_JOB_RENDER_TEXTURE,
			    "document", document,
			    NULL);

	job->page = page;
	job->rotation = rotation;
	job->scale = scale;
	job->target_width = width;
	job->target_height = height;

	return PPS_JOB (job);
}

static gboolean
pps_job_render_texture_run (PpsJob *job)
{
	PpsJobRenderTexture     *job_render = PPS_JOB_RENDER_TEXTURE (job);
	PpsPage          *pps_page;
	PpsRenderContext *rc;
	cairo_surface_t *surface, *selection = NULL;

	pps_debug_message (DEBUG_JOBS, "page: %d (%p)", job_render->page, job);

	pps_document_doc_mutex_lock ();
	pps_document_fc_mutex_lock ();

	PPS_PROFILER_START (PPS_GET_TYPE_NAME (job), g_strdup_printf("page: %d", job_render->page));

	pps_page = pps_document_get_page (job->document, job_render->page);
	rc = pps_render_context_new (pps_page, job_render->rotation, job_render->scale);
	pps_render_context_set_target_size (rc,
					   job_render->target_width, job_render->target_height);
	g_object_unref (pps_page);

	surface = pps_document_render (job->document, rc);

	if (surface == NULL ||
	    cairo_surface_status (surface) != CAIRO_STATUS_SUCCESS) {
		pps_document_fc_mutex_unlock ();
		pps_document_doc_mutex_unlock ();
		g_object_unref (rc);

                if (surface != NULL) {
                        cairo_status_t status = cairo_surface_status (surface);
                        pps_job_failed (job,
                                       PPS_DOCUMENT_ERROR,
                                       PPS_DOCUMENT_ERROR_INVALID,
                                       _("Failed to render page %d: %s"),
                                       job_render->page,
                                       cairo_status_to_string (status));
                } else {
                        pps_job_failed (job,
                                       PPS_DOCUMENT_ERROR,
                                       PPS_DOCUMENT_ERROR_INVALID,
                                       _("Failed to render page %d"),
                                       job_render->page);
                }

		job_render->texture = NULL;
		return FALSE;
	}

	job_render->texture = gdk_texture_new_for_surface (surface);
	cairo_surface_destroy (surface);

	/* If job was cancelled during the page rendering,
	 * we return now, so that the thread is finished ASAP
	 */
	if (g_cancellable_is_cancelled (job->cancellable)) {
		PPS_PROFILER_STOP ();
		pps_document_fc_mutex_unlock ();
		pps_document_doc_mutex_unlock ();
		g_object_unref (rc);
		return FALSE;
	}

	if (job_render->include_selection && PPS_IS_SELECTION (job->document)) {
		pps_selection_render_selection (PPS_SELECTION (job->document),
					       rc,
					       &selection,
					       &(job_render->selection_points),
					       NULL,
					       job_render->selection_style,
					       &(job_render->text), &(job_render->base));
		job_render->selection_region =
			pps_selection_get_selection_region (PPS_SELECTION (job->document),
							   rc,
							   job_render->selection_style,
							   &(job_render->selection_points));

		if (selection != NULL) {
			job_render->selection = gdk_texture_new_for_surface (selection);
			cairo_surface_destroy (selection);
		}
	}

	g_object_unref (rc);

	PPS_PROFILER_STOP ();
	pps_document_fc_mutex_unlock ();
	pps_document_doc_mutex_unlock ();

	pps_job_succeeded (job);

	return FALSE;
}

static void
pps_job_render_texture_class_init (PpsJobRenderTextureClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_render_texture_dispose;
	job_class->run = pps_job_render_texture_run;
}

void
pps_job_render_texture_set_selection_info (PpsJobRenderTexture *job,
					PpsRectangle      *selection_points,
					PpsSelectionStyle  selection_style,
					GdkRGBA          *text,
					GdkRGBA          *base)
{
	job->include_selection = TRUE;

	job->selection_points = *selection_points;
	job->selection_style = selection_style;
	job->text = *text;
	job->base = *base;
}

/* PpsJobPageData */
static void
pps_job_page_data_init (PpsJobPageData *job)
{
}

static gboolean
pps_job_page_data_run (PpsJob *job)
{
	PpsJobPageData *job_pd = PPS_JOB_PAGE_DATA (job);
	PpsPage        *pps_page;

	pps_debug_message (DEBUG_JOBS, "page: %d (%p)", job_pd->page, job);

	pps_document_doc_mutex_lock ();
	pps_page = pps_document_get_page (job->document, job_pd->page);

	if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_TEXT_MAPPING) && PPS_IS_DOCUMENT_TEXT (job->document))
		job_pd->text_mapping =
			pps_document_text_get_text_mapping (PPS_DOCUMENT_TEXT (job->document), pps_page);
	if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_TEXT) && PPS_IS_DOCUMENT_TEXT (job->document))
		job_pd->text =
			pps_document_text_get_text (PPS_DOCUMENT_TEXT (job->document), pps_page);
	if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_TEXT_LAYOUT) && PPS_IS_DOCUMENT_TEXT (job->document))
		pps_document_text_get_text_layout (PPS_DOCUMENT_TEXT (job->document),
						  pps_page,
						  &(job_pd->text_layout),
						  &(job_pd->text_layout_length));
	if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_TEXT_ATTRS) && PPS_IS_DOCUMENT_TEXT (job->document))
		job_pd ->text_attrs =
			pps_document_text_get_text_attrs (PPS_DOCUMENT_TEXT (job->document),
							 pps_page);
        if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_TEXT_LOG_ATTRS) && job_pd->text) {
                job_pd->text_log_attrs_length = g_utf8_strlen (job_pd->text, -1);
                job_pd->text_log_attrs = g_new0 (PangoLogAttr, job_pd->text_log_attrs_length + 1);

                /* FIXME: We need API to get the language of the document */
                pango_get_log_attrs (job_pd->text, -1, -1, NULL, job_pd->text_log_attrs, job_pd->text_log_attrs_length + 1);
        }
	if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_LINKS) && PPS_IS_DOCUMENT_LINKS (job->document))
		job_pd->link_mapping =
			pps_document_links_get_links (PPS_DOCUMENT_LINKS (job->document), pps_page);
	if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_FORMS) && PPS_IS_DOCUMENT_FORMS (job->document))
		job_pd->form_field_mapping =
			pps_document_forms_get_form_fields (PPS_DOCUMENT_FORMS (job->document),
							   pps_page);
	if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_IMAGES) && PPS_IS_DOCUMENT_IMAGES (job->document))
		job_pd->image_mapping =
			pps_document_images_get_image_mapping (PPS_DOCUMENT_IMAGES (job->document),
							      pps_page);
	if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_ANNOTS) && PPS_IS_DOCUMENT_ANNOTATIONS (job->document))
		job_pd->annot_mapping =
			pps_document_annotations_get_annotations (PPS_DOCUMENT_ANNOTATIONS (job->document),
								 pps_page);
        if ((job_pd->flags & PPS_PAGE_DATA_INCLUDE_MEDIA) && PPS_IS_DOCUMENT_MEDIA (job->document))
                job_pd->media_mapping =
                        pps_document_media_get_media_mapping (PPS_DOCUMENT_MEDIA (job->document),
                                                             pps_page);
	g_object_unref (pps_page);
	pps_document_doc_mutex_unlock ();

	pps_job_succeeded (job);

	return FALSE;
}

static void
pps_job_page_data_class_init (PpsJobPageDataClass *class)
{
	PpsJobClass *job_class = PPS_JOB_CLASS (class);

	job_class->run = pps_job_page_data_run;
}

PpsJob *
pps_job_page_data_new (PpsDocument        *document,
		      gint               page,
		      PpsJobPageDataFlags flags)
{
	PpsJobPageData *job;

	pps_debug_message (DEBUG_JOBS, "%d", page);

	job = g_object_new (PPS_TYPE_JOB_PAGE_DATA,
			    "document", document,
			    NULL);

	job->page = page;
	job->flags = flags;

	return PPS_JOB (job);
}

/* PpsJobThumbnailTexture */
static void
pps_job_thumbnail_texture_init (PpsJobThumbnailTexture *job)
{
}

static void
pps_job_thumbnail_texture_dispose (GObject *object)
{
	PpsJobThumbnailTexture *job = PPS_JOB_THUMBNAIL_TEXTURE (object);

	pps_debug_message (DEBUG_JOBS, "disposing job thumbnail: page: %d (%p)", job->page, job);

	g_clear_object (&job->thumbnail_texture);

	G_OBJECT_CLASS (pps_job_thumbnail_texture_parent_class)->dispose (object);
}

static gboolean
pps_job_thumbnail_texture_run (PpsJob *job)
{
	PpsJobThumbnailTexture  *job_thumb = PPS_JOB_THUMBNAIL_TEXTURE(job);
	PpsRenderContext *rc;
	PpsPage          *page;
	cairo_surface_t *surface;

	pps_debug_message (DEBUG_JOBS, "%d (%p)", job_thumb->page, job);

	pps_document_doc_mutex_lock ();

	PPS_PROFILER_START (PPS_GET_TYPE_NAME (job), g_strdup_printf("page: %d", job_thumb->page));
	page = pps_document_get_page (job->document, job_thumb->page);
	rc = pps_render_context_new (page, job_thumb->rotation, job_thumb->scale);
	pps_render_context_set_target_size (rc,
					   job_thumb->target_width, job_thumb->target_height);
	g_object_unref (page);

	surface = pps_document_get_thumbnail_surface (job->document, rc);

	job_thumb->thumbnail_texture = gdk_texture_new_for_surface (surface);
	cairo_surface_destroy(surface);
	g_object_unref (rc);
	PPS_PROFILER_STOP ();
	pps_document_doc_mutex_unlock ();

	if (job_thumb->thumbnail_texture == NULL) {
		pps_job_failed (job,
			       PPS_DOCUMENT_ERROR,
			       PPS_DOCUMENT_ERROR_INVALID,
			       _("Failed to create thumbnail for page %d"),
			       job_thumb->page);
	} else {
		pps_job_succeeded (job);
	}

	return FALSE;
}

static void
pps_job_thumbnail_texture_class_init (PpsJobThumbnailTextureClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_thumbnail_texture_dispose;
	job_class->run = pps_job_thumbnail_texture_run;
}

PpsJob *
pps_job_thumbnail_texture_new (PpsDocument *document,
		      gint        page,
		      gint        rotation,
		      gdouble     scale)
{
	PpsJobThumbnailTexture *job;

	pps_debug_message (DEBUG_JOBS, "%d", page);

	job = g_object_new (PPS_TYPE_JOB_THUMBNAIL_TEXTURE,
			    "document", document,
			    NULL);

	job->page = page;
	job->rotation = rotation;
	job->scale = scale;
        job->target_width = -1;
        job->target_height = -1;

	return PPS_JOB (job);
}

PpsJob *
pps_job_thumbnail_texture_new_with_target_size (PpsDocument *document,
                                       gint        page,
                                       gint        rotation,
                                       gint        target_width,
                                       gint        target_height)
{
        PpsJob *job = pps_job_thumbnail_texture_new (document, page, rotation, 1.);
        PpsJobThumbnailTexture  *job_thumb = PPS_JOB_THUMBNAIL_TEXTURE(job);

        job_thumb->target_width = target_width;
        job_thumb->target_height = target_height;

        return job;
}

/**
 * pps_job_thumbnail_texture_get_texture:
 * @job: an #PpsJobThumbnailTexture job
 *
 * This is similar to pps_job_find_get_n_results() but it takes
 * care to treat any multi-line matches as being only one result.
 *
 * Returns: (nullable) (transfer none): total number of match results in @page
 */
GdkTexture *
pps_job_thumbnail_texture_get_texture (PpsJobThumbnailTexture *job)
{
	g_return_val_if_fail (PPS_IS_JOB_THUMBNAIL_TEXTURE (job), NULL);

	return job->thumbnail_texture;
}

/* PpsJobFonts */
static void
pps_job_fonts_init (PpsJobFonts *job)
{
}

static gboolean
pps_job_fonts_run (PpsJob *job)
{
	PpsDocument *document = pps_job_get_document (job);

	pps_debug_message (DEBUG_JOBS, NULL);

	pps_document_doc_mutex_lock ();
	pps_document_fc_mutex_lock ();

	pps_document_fonts_scan (PPS_DOCUMENT_FONTS (document));

	pps_document_fc_mutex_unlock ();
	pps_document_doc_mutex_unlock ();

	pps_job_succeeded (job);

	return FALSE;
}

static void
pps_job_fonts_class_init (PpsJobFontsClass *class)
{
	PpsJobClass *job_class = PPS_JOB_CLASS (class);

	job_class->run = pps_job_fonts_run;
}

PpsJob *
pps_job_fonts_new (PpsDocument *document)
{
	PpsJobFonts *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_FONTS,
			    "document", document,
			    NULL);

	return PPS_JOB (job);
}

/**
 * PpsJobLoad:
 *
 * A job class to load an #PpsDocument
 *
 * Supports loading in different forms depending on the options
 * being set.
 *
 */

static void
pps_job_load_init (PpsJobLoad *job)
{
	job->flags = PPS_DOCUMENT_LOAD_FLAG_NONE;
	job->uri = NULL;
	job->fd = -1;
	job->mime_type = NULL;
}

static void
pps_job_load_dispose (GObject *object)
{
	PpsJobLoad *job = PPS_JOB_LOAD (object);

	if (job->fd != -1) {
		close (job->fd);
		job->fd = -1;
	}

	g_clear_pointer (&job->mime_type, g_free);
	g_clear_pointer (&job->uri, g_free);
	g_clear_pointer (&job->password, g_free);
	g_clear_object (&job->loaded_document);

	G_OBJECT_CLASS (pps_job_load_parent_class)->dispose (object);
}

static int
pps_dupfd (int fd,
	  GError **error)
{
	int new_fd;

	new_fd = fcntl (fd, F_DUPFD_CLOEXEC, 3);
	if (new_fd == -1) {
		int errsv = errno;
		g_set_error_literal (error, G_FILE_ERROR, g_file_error_from_errno (errsv),
				     g_strerror (errsv));
	}

	return new_fd;
}

static gboolean
pps_job_load_run (PpsJob *job)
{
	PpsJobLoad *job_load = PPS_JOB_LOAD (job);
	GError    *error = NULL;

	pps_debug_message (DEBUG_JOBS, NULL);

	if (job_load->uri == NULL && job_load->fd == -1) {
		g_set_error_literal (&error, G_FILE_ERROR, G_FILE_ERROR_BADF,
				     "Either the URI or the FD must be set!");
		pps_job_failed_from_error (job, error);
		g_error_free (error);
		return FALSE;
	}

	pps_document_fc_mutex_lock ();

	/* This job may already have a document even if the job didn't complete
	   because, e.g., a password is required - if so, just reload rather than
	   creating a new instance */
	if (job_load->loaded_document) {
		PpsDocument *loaded_doc = job_load->loaded_document;
		const gchar *uncompressed_uri;

		if (job_load->password) {
			pps_document_security_set_password (PPS_DOCUMENT_SECURITY (loaded_doc),
							   job_load->password);
		}

		job->failed = FALSE;
		job->finished = FALSE;
		g_clear_error (&job->error);

		if (job_load->uri) {
			uncompressed_uri = g_object_get_data (G_OBJECT (loaded_doc),
							      "uri-uncompressed");
			pps_document_load_full (loaded_doc,
					       uncompressed_uri ? uncompressed_uri : job_load->uri,
					       job_load->flags,
					       &error);
		} else {
			/* We need to dup the FD since we may need to pass it again
			 * if the document is reloaded, as pps_document calls
			 * consume it.
			 */
			int fd = pps_dupfd (job_load->fd, &error);
			if (fd != -1)
				pps_document_load_fd (loaded_doc,
						     fd,
						     job_load->flags,
						     job->cancellable,
						     &error);
		}
	} else {
		if (job_load->uri) {
			job_load->loaded_document =
				pps_document_factory_get_document_full (job_load->uri,
								       job_load->flags,
								       &error);
		} else {
			/* We need to dup the FD since we may need to pass it again
			 * if the document is reloaded, as pps_document calls
			 * consume it.
			 */
			int fd = pps_dupfd (job_load->fd, &error);
			if (fd != -1)
				job_load->loaded_document =
					pps_document_factory_get_document_for_fd (fd,
										 job_load->mime_type,
										 job_load->flags,
										 job->cancellable,
										 &error);
		}
	}

	pps_document_fc_mutex_unlock ();

	if (error) {
		pps_job_failed_from_error (job, error);
		g_error_free (error);
	} else {
		pps_job_succeeded (job);
	}

	return FALSE;
}

static void
pps_job_load_class_init (PpsJobLoadClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_load_dispose;
	job_class->run = pps_job_load_run;
}

PpsJob *
pps_job_load_new (void)
{
	PpsJobLoad *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_LOAD, NULL);

	return PPS_JOB (job);
}

/**
 * pps_job_load_set_uri:
 * @job: an #PpsJobLoad
 * @uri: an uri representing a file
 *
 * It is an error to call this function if the file descriptor
 * is set for the job.
 *
 */
void
pps_job_load_set_uri (PpsJobLoad   *job,
		     const gchar *uri)
{
	g_return_if_fail (PPS_IS_JOB_LOAD (job));
	g_return_if_fail (uri != NULL);
	g_return_if_fail (job->fd == -1);

	pps_debug_message (DEBUG_JOBS, "%s", uri);

	g_free (job->uri);
	job->uri = g_strdup (uri);
}

/**
 * pps_job_load_set_fd:
 * @job: an #PpsJobLoad
 * @fd: a file descriptor
 * @mime_type: the mime type of the file descriptor
 * @error: (nullable): a location to store a #GError, or %NULL
 *
 * Sets @fd as the file descriptor in @job. If duplicating @fd fails,
 * returns %FALSE with @error filled in.
 *
 * It is an error to call this function if uri is set for the job
 *
 * Returns: %TRUE if the file descriptor could be set
 *
 * Since: 46.0
 */
gboolean
pps_job_load_set_fd (PpsJobLoad   *job,
		    int          fd,
		    const char  *mime_type,
		    GError     **error)
{
	g_return_val_if_fail (PPS_IS_JOB_LOAD (job), FALSE);
	g_return_val_if_fail (fd != -1, FALSE);
	g_return_val_if_fail (mime_type != NULL, FALSE);
	g_return_val_if_fail (job->uri == NULL, FALSE);

	pps_debug_message (DEBUG_JOBS, "%d %s", fd, mime_type);

	g_free (job->mime_type);
	job->mime_type = g_strdup (mime_type);

	job->fd = pps_dupfd (fd, error);
	return job->fd != -1;
}

/**
 * pps_job_load_take_fd:
 * @job: an #PpsJobLoad
 * @fd: a file descriptor
 * @mime_type: the mime type of the file descriptor
 *
 * Sets @fd as the file descriptor in @job.
 * Note that @job takes ownership of @fd; you must not do anything
 * with it afterwards.
 *
 * It is an error to call this function if uri is set for the job
 *
 * Since: 46.0
 */
void
pps_job_load_take_fd (PpsJobLoad  *job,
		     int         fd,
		     const char *mime_type)
{
	g_return_if_fail (PPS_IS_JOB_LOAD (job));
	g_return_if_fail (fd != -1);
	g_return_if_fail (mime_type != NULL);
	g_return_if_fail (job->uri == NULL);

	pps_debug_message (DEBUG_JOBS, "%d %s", fd, mime_type);

	g_free (job->mime_type);
	job->mime_type = g_strdup (mime_type);

	job->fd = fd;
}

void
pps_job_load_set_password (PpsJobLoad *job, const gchar *password)
{
	g_return_if_fail (PPS_IS_JOB_LOAD (job));

	pps_debug_message (DEBUG_JOBS, NULL);

	g_free (job->password);
	job->password = g_strdup (password);
}

void
pps_job_load_set_load_flags (PpsJobLoad           *job,
			    PpsDocumentLoadFlags  flags)
{
	g_return_if_fail (PPS_IS_JOB_LOAD (job));

	job->flags = flags;
}

/**
 * pps_job_load_get_loaded_document:
 * @job: an #PpsJobLoad
 *
 * Returns: (nullable) (transfer full): The loaded document, if available
 *
 * Since: 46.0
 */
PpsDocument*
pps_job_load_get_loaded_document (PpsJobLoad *job)
{
	g_return_val_if_fail (PPS_IS_JOB_LOAD (job), NULL);

	if (!job->loaded_document)
		return NULL;

	return g_object_ref (job->loaded_document);
}

/* PpsJobSave */
static void
pps_job_save_init (PpsJobSave *job)
{
}

static void
pps_job_save_dispose (GObject *object)
{
	PpsJobSave *job = PPS_JOB_SAVE (object);

	pps_debug_message (DEBUG_JOBS, "disposing job save: uri: %s", job->uri);

	g_clear_pointer (&job->uri, g_free);
	g_clear_pointer (&job->document_uri, g_free);

	(* G_OBJECT_CLASS (pps_job_save_parent_class)->dispose) (object);
}

static gboolean
pps_job_save_run (PpsJob *job)
{
	PpsJobSave *job_save = PPS_JOB_SAVE (job);
	gint       fd;
	gchar     *tmp_filename = NULL;
	gchar     *local_uri;
	GError    *error = NULL;

	pps_debug_message (DEBUG_JOBS, "uri: %s, document_uri: %s", job_save->uri, job_save->document_uri);

        fd = pps_mkstemp ("saveacopy.XXXXXX", &tmp_filename, &error);
        if (fd == -1) {
                pps_job_failed_from_error (job, error);
                g_error_free (error);

		return FALSE;
	}
	close (fd);

	pps_document_doc_mutex_lock ();

	/* Save document to temp filename */
	local_uri = g_filename_to_uri (tmp_filename, NULL, &error);
        if (local_uri != NULL) {
                pps_document_save (job->document, local_uri, &error);
        }

	pps_document_doc_mutex_unlock ();

	if (error) {
		g_free (local_uri);
		pps_job_failed_from_error (job, error);
		g_error_free (error);

		return FALSE;
	}

	/* If original document was compressed,
	 * compress it again before saving
	 */
	if (g_object_get_data (G_OBJECT (job->document), "uri-uncompressed")) {
		PpsCompressionType ctype = PPS_COMPRESSION_NONE;
		const gchar      *ext;
		gchar            *uri_comp;

		ext = g_strrstr (job_save->document_uri, ".gz");
		if (ext && g_ascii_strcasecmp (ext, ".gz") == 0)
			ctype = PPS_COMPRESSION_GZIP;

		ext = g_strrstr (job_save->document_uri, ".bz2");
		if (ext && g_ascii_strcasecmp (ext, ".bz2") == 0)
			ctype = PPS_COMPRESSION_BZIP2;

		uri_comp = pps_file_compress (local_uri, ctype, &error);
		g_free (local_uri);
		g_unlink (tmp_filename);

		if (!uri_comp || error) {
			local_uri = NULL;
		} else {
			local_uri = uri_comp;
		}
	}

	g_free (tmp_filename);

	if (error) {
		g_free (local_uri);
		pps_job_failed_from_error (job, error);
		g_error_free (error);

		return FALSE;
	}

	if (!local_uri)
		return FALSE;

	pps_xfer_uri_simple (local_uri, job_save->uri, &error);
	pps_tmp_uri_unlink (local_uri);

        /* Copy the metadata from the original file */
        if (!error) {
                /* Ignore errors here. Failure to copy metadata is not a hard error */
                pps_file_copy_metadata (job_save->document_uri, job_save->uri, NULL);
        }

	if (error) {
		pps_job_failed_from_error (job, error);
		g_error_free (error);
	} else {
		pps_job_succeeded (job);
	}

	return FALSE;
}

static void
pps_job_save_class_init (PpsJobSaveClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_save_dispose;
	job_class->run = pps_job_save_run;
}

PpsJob *
pps_job_save_new (PpsDocument  *document,
		 const gchar *uri,
		 const gchar *document_uri)
{
	PpsJobSave *job;

	pps_debug_message (DEBUG_JOBS, "uri: %s, document_uri: %s", uri, document_uri);

	job = g_object_new (PPS_TYPE_JOB_SAVE,
			    "document", document,
			    NULL);

	job->uri = g_strdup (uri);
	job->document_uri = g_strdup (document_uri);

	return PPS_JOB (job);
}

/* PpsJobFind */
static void
pps_job_find_init (PpsJobFind *job)
{
}

static void
pps_job_find_dispose (GObject *object)
{
	PpsJobFind *job = PPS_JOB_FIND (object);

	g_clear_pointer (&job->text, g_free);

	if (job->pages) {
		gint i;

		for (i = 0; i < job->n_pages; i++) {
			g_list_free_full (job->pages[i], (GDestroyNotify)pps_find_rectangle_free);
		}

		g_clear_pointer (&job->pages, g_free);
	}

	(* G_OBJECT_CLASS (pps_job_find_parent_class)->dispose) (object);
}

static gboolean
pps_job_find_run (PpsJob *job)
{
	PpsJobFind      *job_find = PPS_JOB_FIND (job);
	PpsDocumentFind *find = PPS_DOCUMENT_FIND (job->document);
	PpsPage         *pps_page;
	GList           *matches;

	pps_debug_message (DEBUG_JOBS, NULL);

	for (gint current_page = job_find->start_page;
	     (current_page + 1) % job_find->n_pages != job_find->start_page;
	     current_page = (current_page + 1) % job_find->n_pages) {
		if (g_cancellable_is_cancelled (job->cancellable))
			return FALSE;

		pps_document_doc_mutex_lock ();
		pps_page = pps_document_get_page (job->document, current_page);
		matches = pps_document_find_find_text (find, pps_page, job_find->text,
						       job_find->options);
		g_object_unref (pps_page);
		pps_document_doc_mutex_unlock ();

		job_find->has_results |= (matches != NULL);

		job_find->pages[current_page] = matches;
		g_signal_emit (job_find, job_find_signals[FIND_UPDATED], 0, current_page);
	}

	pps_job_succeeded (job);

	return FALSE;
}

static void
pps_job_find_class_init (PpsJobFindClass *class)
{
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);

	job_class->run = pps_job_find_run;
	gobject_class->dispose = pps_job_find_dispose;

	job_find_signals[FIND_UPDATED] =
		g_signal_new ("updated",
			      PPS_TYPE_JOB_FIND,
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (PpsJobFindClass, updated),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__INT,
			      G_TYPE_NONE,
			      1, G_TYPE_INT);
}

PpsJob *
pps_job_find_new (PpsDocument    *document,
		 gint           start_page,
		 gint           n_pages,
		 const gchar   *text,
		 PpsFindOptions  options)
{
	PpsJobFind *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_FIND,
			    "document", document,
			    NULL);

	job->start_page = start_page;
	job->n_pages = n_pages;
	job->pages = g_new0 (GList *, n_pages);
	job->text = g_strdup (text);
	job->has_results = FALSE;
	job->options = options;

	return PPS_JOB (job);
}

/**
 * pps_job_find_get_options:
 * @job:
 *
 * Returns: the job's find options
 *
 * Since: 3.6
 */
PpsFindOptions
pps_job_find_get_options (PpsJobFind *job)
{
        return job->options;
}

/**
 * pps_job_find_get_n_main_results:
 * @job: an #PpsJobFind job
 * @page: number of the page we want to count its match results.
 *
 * This is similar to pps_job_find_get_n_results() but it takes
 * care to treat any multi-line matches as being only one result.
 *
 * Returns: total number of match results in @page
 */
gint
pps_job_find_get_n_main_results (PpsJobFind *job,
				gint       page)
{
	GList *l;
	int n = 0;

	for (l = job->pages[page]; l; l = l->next) {
		if ( !((PpsFindRectangle *) l->data)->next_line )
			n++;
	}

	return n;
}

gboolean
pps_job_find_has_results (PpsJobFind *job)
{
	return job->has_results;
}

/**
 * pps_job_find_get_results: (skip)
 * @job: an #PpsJobFind
 *
 * Returns: a #GList of #GList<!-- -->s containing #PpsFindRectangle<!-- -->s
 */
GList **
pps_job_find_get_results (PpsJobFind *job)
{
	return job->pages;
}

/* PpsJobLayers */
static void
pps_job_layers_init (PpsJobLayers *job)
{
}

static void
pps_job_layers_dispose (GObject *object)
{
	PpsJobLayers *job = PPS_JOB_LAYERS (object);

	g_clear_object (&job->model);

	(* G_OBJECT_CLASS (pps_job_layers_parent_class)->dispose) (object);
}

static gboolean
pps_job_layers_run (PpsJob *job)
{
	PpsJobLayers *job_layers = PPS_JOB_LAYERS (job);

	pps_debug_message (DEBUG_JOBS, NULL);

	pps_document_doc_mutex_lock ();
	job_layers->model = pps_document_layers_get_layers (PPS_DOCUMENT_LAYERS (job->document));
	pps_document_doc_mutex_unlock ();

	pps_job_succeeded (job);

	return FALSE;
}

static void
pps_job_layers_class_init (PpsJobLayersClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_layers_dispose;
	job_class->run = pps_job_layers_run;
}

/**
 * pps_job_layers_get_model:
 * @job_layers: a #PpsJobLayers
 *
 * Returns: (transfer full): A #GListModel contains the result
 */
GListModel *
pps_job_layers_get_model (PpsJobLayers *job_layers)
{
	return g_steal_pointer (&job_layers->model);
}

PpsJob *
pps_job_layers_new (PpsDocument *document)
{
	PpsJob *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_LAYERS,
			    "document", document,
			    NULL);


	return job;
}

/* PpsJobExport */
static void
pps_job_export_init (PpsJobExport *job)
{
	job->page = -1;
}

static void
pps_job_export_dispose (GObject *object)
{
	PpsJobExport *job = PPS_JOB_EXPORT (object);

	g_clear_object (&job->rc);

	(* G_OBJECT_CLASS (pps_job_export_parent_class)->dispose) (object);
}

static gboolean
pps_job_export_run (PpsJob *job)
{
	PpsJobExport *job_export = PPS_JOB_EXPORT (job);
	PpsPage      *pps_page;

	g_assert (job_export->page != -1);

	pps_debug_message (DEBUG_JOBS, NULL);

	pps_document_doc_mutex_lock ();

	pps_page = pps_document_get_page (job->document, job_export->page);
	if (job_export->rc) {
		job->failed = FALSE;
		job->finished = FALSE;
		g_clear_error (&job->error);

		pps_render_context_set_page (job_export->rc, pps_page);
	} else {
		job_export->rc = pps_render_context_new (pps_page, 0, 1.0);
	}
	g_object_unref (pps_page);

	pps_file_exporter_do_page (PPS_FILE_EXPORTER (job->document), job_export->rc);

	pps_document_doc_mutex_unlock ();

	pps_job_succeeded (job);

	return FALSE;
}

static void
pps_job_export_class_init (PpsJobExportClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_export_dispose;
	job_class->run = pps_job_export_run;
}

PpsJob *
pps_job_export_new (PpsDocument *document)
{
	PpsJob *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_EXPORT,
			    "document", document,
			    NULL);


	return job;
}

void
pps_job_export_set_page (PpsJobExport *job,
			gint         page)
{
	job->page = page;
}

/* PpsJobPrint */
static void
pps_job_print_init (PpsJobPrint *job)
{
	job->page = -1;
}

static void
pps_job_print_dispose (GObject *object)
{
	PpsJobPrint *job = PPS_JOB_PRINT (object);

	g_clear_pointer (&job->cr, cairo_destroy);

	(* G_OBJECT_CLASS (pps_job_print_parent_class)->dispose) (object);
}

static gboolean
pps_job_print_run (PpsJob *job)
{
	PpsJobPrint     *job_print = PPS_JOB_PRINT (job);
	PpsPage         *pps_page;
	cairo_status_t  cr_status;

	g_assert (job_print->page != -1);
	g_assert (job_print->cr != NULL);

	pps_debug_message (DEBUG_JOBS, NULL);

	job->failed = FALSE;
	job->finished = FALSE;
	g_clear_error (&job->error);

	pps_document_doc_mutex_lock ();

	pps_page = pps_document_get_page (job->document, job_print->page);
	pps_document_print_print_page (PPS_DOCUMENT_PRINT (job->document),
				      pps_page, job_print->cr);
	g_object_unref (pps_page);

	pps_document_doc_mutex_unlock ();

        if (g_cancellable_is_cancelled (job->cancellable))
                return FALSE;

	cr_status = cairo_status (job_print->cr);
	if (cr_status == CAIRO_STATUS_SUCCESS) {
		pps_job_succeeded (job);
	} else {
		pps_job_failed (job,
			       GTK_PRINT_ERROR,
			       GTK_PRINT_ERROR_GENERAL,
			       _("Failed to print page %d: %s"),
			       job_print->page,
			       cairo_status_to_string (cr_status));
	}

	return FALSE;
}

static void
pps_job_print_class_init (PpsJobPrintClass *class)
{
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	PpsJobClass   *job_class = PPS_JOB_CLASS (class);

	oclass->dispose = pps_job_print_dispose;
	job_class->run = pps_job_print_run;
}

PpsJob *
pps_job_print_new (PpsDocument *document)
{
	PpsJob *job;

	pps_debug_message (DEBUG_JOBS, NULL);

	job = g_object_new (PPS_TYPE_JOB_PRINT,
			    "document", document,
			    NULL);


	return job;
}

void
pps_job_print_set_page (PpsJobPrint *job,
		       gint        page)
{
	job->page = page;
}

void
pps_job_print_set_cairo (PpsJobPrint *job,
			cairo_t    *cr)
{
	if (job->cr == cr)
		return;

	if (job->cr)
		cairo_destroy (job->cr);
	job->cr = cr ? cairo_reference (cr) : NULL;
}
