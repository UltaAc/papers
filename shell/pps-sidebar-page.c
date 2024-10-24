/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/*
 *  Copyright (C) 2005 Marco Pesenti Gritti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>

#include "pps-sidebar-page.h"

enum {
	PROP_0,
	PROP_DOCUMENT_MODEL,
	PROP_SIDEBAR,
};

typedef struct
{
	PpsDocumentModel *model;
	GObject *sidebar; // PpsSidebar *sidebar;
} PpsSidebarPagePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (PpsSidebarPage, pps_sidebar_page, ADW_TYPE_BIN)

#define GET_PRIVATE(o) pps_sidebar_page_get_instance_private (o)

gboolean
pps_sidebar_page_support_document (PpsSidebarPage *sidebar_page,
                                   PpsDocument *document)
{
	PpsSidebarPageClass *class = PPS_SIDEBAR_PAGE_GET_CLASS (sidebar_page);

	g_return_val_if_fail (PPS_IS_SIDEBAR_PAGE (sidebar_page), FALSE);
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), FALSE);

	g_return_val_if_fail (class->support_document, FALSE);

	return class->support_document (sidebar_page, document);
}

static void
pps_sidebar_page_set_document_model (PpsSidebarPage *sidebar_page,
                                     PpsDocumentModel *model)
{
	PpsSidebarPagePrivate *priv = GET_PRIVATE (sidebar_page);

	if (priv->model == model)
		return;

	g_set_object (&priv->model, model);
}

static void
pps_sidebar_page_set_sidebar (PpsSidebarPage *sidebar_page,
                              GObject *sidebar) // PpsSidebar
{
	PpsSidebarPagePrivate *priv = GET_PRIVATE (sidebar_page);

	if (priv->sidebar == sidebar)
		return;

	g_set_object (&priv->sidebar, sidebar);
}

/**
 * pps_sidebar_page_get_document_model
 * @sidebar_page: A #PpsSidebarPage
 *
 * Returns: (transfer none) (nullable): The #PpsDocumentModel
 */
PpsDocumentModel *
pps_sidebar_page_get_document_model (PpsSidebarPage *sidebar_page)
{
	PpsSidebarPagePrivate *priv = GET_PRIVATE (sidebar_page);
	return priv->model;
}

static void
pps_sidebar_page_set_property (GObject *object,
                               guint prop_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
	PpsSidebarPage *sidebar_page = PPS_SIDEBAR_PAGE (object);

	switch (prop_id) {
	case PROP_DOCUMENT_MODEL:
		pps_sidebar_page_set_document_model (sidebar_page, g_value_get_object (value));
		break;
	case PROP_SIDEBAR:
		pps_sidebar_page_set_sidebar (sidebar_page, g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_sidebar_page_init (PpsSidebarPage *self)
{
}

static void
pps_sidebar_page_dispose (GObject *object)
{
	PpsSidebarPage *sidebar_page = PPS_SIDEBAR_PAGE (object);
	PpsSidebarPagePrivate *priv = GET_PRIVATE (sidebar_page);

	g_clear_object (&priv->model);
	g_clear_object (&priv->sidebar);

	G_OBJECT_CLASS (pps_sidebar_page_parent_class)->dispose (object);
}

static void
pps_sidebar_page_class_init (PpsSidebarPageClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->dispose = pps_sidebar_page_dispose;
	object_class->set_property = pps_sidebar_page_set_property;

	g_object_class_install_property (object_class,
	                                 PROP_DOCUMENT_MODEL,
	                                 g_param_spec_object ("document-model",
	                                                      "DocumentModel",
	                                                      "The document model",
	                                                      PPS_TYPE_DOCUMENT_MODEL,
	                                                      G_PARAM_WRITABLE |
	                                                          G_PARAM_CONSTRUCT_ONLY |
	                                                          G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (object_class,
	                                 PROP_SIDEBAR,
	                                 g_param_spec_object ("sidebar",
	                                                      "Sidebar",
	                                                      "The sidebar containing this page",
	                                                      G_TYPE_OBJECT, // PPS_TYPE_SIDEBAR
	                                                      G_PARAM_WRITABLE |
	                                                          G_PARAM_CONSTRUCT_ONLY |
	                                                          G_PARAM_STATIC_STRINGS));
}

void
pps_sidebar_page_navigate_to_view (PpsSidebarPage *sidebar_page)
{
	PpsSidebarPagePrivate *priv = GET_PRIVATE (sidebar_page);

	g_signal_emit_by_name (priv->sidebar, "navigated-to-view", NULL);
}
