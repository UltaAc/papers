/*
 * Copyright (C) 2000, 2001 Eazel Inc.
 * Copyright (C) 2003  Andrew Sobala <aes@gnome.org>
 * Copyright (C) 2005  Bastien Nocera <hadess@hadess.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Pps project hereby grant permission for non-gpl compatible GStreamer
 * plugins to be used and distributed together with GStreamer and Pps. This
 * permission are above and beyond the permissions granted by the GPL license
 * Pps is covered by.
 *
 * Monday 7th February 2005: Christian Schaller: Add excemption clause.
 * See license_change file for details.
 *
 */

#include <config.h>

#include <string.h>

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include <nautilus-extension.h>

#include <papers-document.h>
#include "pps-properties.h"

static GType epp_type = 0;
static void property_page_provider_iface_init
	(NautilusPropertiesModelProviderInterface *iface);
static GList *pps_properties_get_models
	(NautilusPropertiesModelProvider *provider, GList *files);

static void
pps_properties_plugin_register_type (GTypeModule *module)
{
	const GTypeInfo info = {
		sizeof (GObjectClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) NULL,
		NULL,
		NULL,
		sizeof (GObject),
		0,
		(GInstanceInitFunc) NULL
	};
	const GInterfaceInfo property_page_provider_iface_info = {
		(GInterfaceInitFunc)property_page_provider_iface_init,
		NULL,
		NULL
	};

	epp_type = g_type_module_register_type (module, G_TYPE_OBJECT,
			"PpsPropertiesPlugin",
			&info, 0);
	g_type_module_add_interface (module,
			epp_type,
			NAUTILUS_TYPE_PROPERTIES_MODEL_PROVIDER,
			&property_page_provider_iface_info);
}

static void
property_page_provider_iface_init (NautilusPropertiesModelProviderInterface *iface)
{
	iface->get_models = pps_properties_get_models;
}

static GListModel *
build_properties (PpsDocument *document)
{
	PpsDocumentInfo *info = pps_document_get_info (document);
	GListStore *model = g_list_store_new (NAUTILUS_TYPE_PROPERTIES_ITEM);
	const char *uri = pps_document_get_uri (document);
	GDateTime *datetime = NULL;
	char *text;

#define SET_PROPERTY(p, value) 	do  {				\
	g_list_store_append (model, 				\
		nautilus_properties_item_new (_(properties_info[p##_PROPERTY].label), value)); \
	} while (0)

#define FIELD_SET_PROPERTY(p, value) 			\
	if (info->fields_mask & PPS_DOCUMENT_INFO_##p) {	\
		SET_PROPERTY (p, value);		\
	}

	FIELD_SET_PROPERTY (TITLE, info->title);
	SET_PROPERTY (URI, uri);
	FIELD_SET_PROPERTY (SUBJECT, info->subject);
	FIELD_SET_PROPERTY (AUTHOR, info->author);
	FIELD_SET_PROPERTY (KEYWORDS, info->keywords);
	FIELD_SET_PROPERTY (PRODUCER, info->producer);
	FIELD_SET_PROPERTY (CREATOR, info->creator);

	datetime = pps_document_info_get_created_datetime(info);
	if (datetime != NULL) {
		text = pps_document_misc_format_datetime(datetime);
		SET_PROPERTY(CREATION_DATE, text);
		g_free(text);
	} else {
		SET_PROPERTY(CREATION_DATE, NULL);
	}
	datetime = pps_document_info_get_modified_datetime(info);
	if (datetime != NULL) {
		text = pps_document_misc_format_datetime(datetime);
		SET_PROPERTY(MOD_DATE, text);
		g_free(text);
	} else {
		SET_PROPERTY(MOD_DATE, NULL);
	}

	FIELD_SET_PROPERTY (FORMAT, info->format);

	if (info->fields_mask & PPS_DOCUMENT_INFO_N_PAGES) {
		text = g_strdup_printf ("%d", info->n_pages);
		SET_PROPERTY (N_PAGES, text);
		g_free (text);
	}
	FIELD_SET_PROPERTY (LINEARIZED, info->linearized);
	FIELD_SET_PROPERTY (SECURITY, info->security);

	if (info->fields_mask & PPS_DOCUMENT_INFO_PAPER_SIZE) {
		text = pps_document_info_regular_paper_size (info);
		SET_PROPERTY (PAPER_SIZE, text);
		g_free (text);
	}

	if (info->fields_mask & PPS_DOCUMENT_INFO_CONTAINS_JS) {
		if (info->contains_js == PPS_DOCUMENT_CONTAINS_JS_YES) {
			text = _("Yes");
		} else if (info->contains_js == PPS_DOCUMENT_CONTAINS_JS_NO) {
			text = _("No");
		} else {
			text = _("Unknown");
		}
		SET_PROPERTY (CONTAINS_JS, text);
	}

	if (pps_document_get_size (document)) {
		text = g_format_size (pps_document_get_size (document));
		SET_PROPERTY (FILE_SIZE, text);
		g_free (text);
	}

	return G_LIST_MODEL (model);
#undef SET_PROPERTY
#undef FIELD_SET_PROPERTY
}

static GList *
pps_properties_get_models (NautilusPropertiesModelProvider *provider,
			 GList *files)
{
	GError *error = NULL;
	PpsDocument *document = NULL;
	GList *models = NULL;
	NautilusFileInfo *file;
	gchar *uri = NULL;
	NautilusPropertiesModel *properties_group;

	/* only add properties page if a single file is selected */
	if (files == NULL || files->next != NULL)
		goto end;
	file = files->data;

	/* okay, make the page */
	uri = nautilus_file_info_get_uri (file);

	document = pps_document_factory_get_document (uri, &error);
	if (!document)
		goto end;

	pps_document_load (document, uri, &error);
	if (error) {
		g_error_free (error);
		goto end;
	}

	properties_group = nautilus_properties_model_new (_("Document Properties"), build_properties (document));

	models = g_list_prepend (models, properties_group);
end:
	g_free (uri);
	g_clear_pointer (&error, g_error_free);
	g_clear_object (&document);

	return models;
}

/* --- extension interface --- */

PPS_PUBLIC
void
nautilus_module_initialize (GTypeModule *module)
{
	pps_properties_plugin_register_type (module);
	pps_init ();
}

PPS_PUBLIC
void
nautilus_module_shutdown (void)
{
        pps_shutdown ();
}

PPS_PUBLIC
void
nautilus_module_list_types (const GType **types,
                            int          *num_types)
{
	static GType type_list[1];

	type_list[0] = epp_type;
	*types = type_list;
	*num_types = G_N_ELEMENTS (type_list);
}
