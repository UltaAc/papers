/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2004 Red Hat, Inc.
 *  Copyright (C) 2018 Germán Poo-Caamaño <gpoo@gnome.org>
 *
 *  Author:
 *    Jonathan Blandford <jrb@alum.mit.edu>
 *    Germán Poo-Caamaño <gpoo@gnome.org>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "pps-sidebar.h"
#include "pps-sidebar-page.h"

enum
{
	PROP_0,
	PROP_CURRENT_PAGE,
	PROP_DOCUMENT_MODEL,
};

typedef struct {
	GtkWidget *stack;
	GtkWidget *switcher;

	PpsDocumentModel *model;
} PpsSidebarPrivate;

static void pps_sidebar_set_page  (PpsSidebar       *pps_sidebar,
                                   GtkWidget        *main_widget);
static void pps_sidebar_child_change_cb (GObject    *gobject,
					GParamSpec *pspec,
					PpsSidebar  *pps_sidebar);
static void pps_sidebar_buildable_iface_init (GtkBuildableIface *iface);
static GtkBuildableIface *parent_buildable_iface;

G_DEFINE_TYPE_WITH_CODE (PpsSidebar, pps_sidebar, ADW_TYPE_BIN,
                         G_ADD_PRIVATE (PpsSidebar)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                                                pps_sidebar_buildable_iface_init))

#define GET_PRIVATE(o) pps_sidebar_get_instance_private (o)

static void
pps_sidebar_set_property (GObject      *object,
		         guint         prop_id,
		         const GValue *value,
		         GParamSpec   *pspec)
{
	PpsSidebar *sidebar = PPS_SIDEBAR (object);

	switch (prop_id)
	{
	case PROP_CURRENT_PAGE:
		pps_sidebar_set_page (sidebar, g_value_get_object (value));
		break;
	case PROP_DOCUMENT_MODEL:
		pps_sidebar_set_model (sidebar,
			PPS_DOCUMENT_MODEL (g_value_get_object (value)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

/**
 * pps_sidebar_get_current_page:
 * @pps_sidebar: a #PpsSidebar.
 *
 * Returns: (not nullable) (transfer none): The current visible page.
 */
GtkWidget *
pps_sidebar_get_current_page (PpsSidebar *pps_sidebar)
{
	PpsSidebarPrivate *priv = GET_PRIVATE (pps_sidebar);

	return gtk_stack_get_visible_child (GTK_STACK (priv->stack));
}

const char *
pps_sidebar_get_visible_child_name (PpsSidebar *pps_sidebar)
{
	PpsSidebarPrivate *priv = GET_PRIVATE (pps_sidebar);

	return gtk_stack_get_visible_child_name (GTK_STACK (priv->stack));
}

void
pps_sidebar_set_visible_child_name (PpsSidebar *pps_sidebar, const char *name)
{
	PpsSidebarPrivate *priv = GET_PRIVATE (pps_sidebar);
	PpsDocument *document = pps_document_model_get_document (priv->model);
	GtkWidget *page = gtk_stack_get_child_by_name (GTK_STACK (priv->stack), name);

	g_warn_if_fail (page != NULL);

	if (page && pps_sidebar_page_support_document (PPS_SIDEBAR_PAGE (page), document)) {
		gtk_stack_set_visible_child (GTK_STACK (priv->stack), page);
	} else {
		gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), "thumbnails");
	}
}

static void
pps_sidebar_get_property (GObject *object,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *pspec)
{
	PpsSidebar *sidebar = PPS_SIDEBAR (object);

	switch (prop_id)
	{
	case PROP_CURRENT_PAGE:
		g_value_set_object (value, pps_sidebar_get_current_page (sidebar));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_sidebar_dispose (GObject *object)
{
	PpsSidebarPrivate *priv = GET_PRIVATE (PPS_SIDEBAR (object));

	g_clear_object (&priv->model);

	G_OBJECT_CLASS (pps_sidebar_parent_class)->dispose (object);
}

static void
pps_sidebar_class_init (PpsSidebarClass *pps_sidebar_class)
{
        GObjectClass *g_object_class = G_OBJECT_CLASS (pps_sidebar_class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (pps_sidebar_class);

	g_object_class->get_property = pps_sidebar_get_property;
	g_object_class->set_property = pps_sidebar_set_property;
	g_object_class->dispose = pps_sidebar_dispose;

	gtk_widget_class_set_template_from_resource (widget_class,
			"/org/gnome/papers/ui/sidebar.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebar, switcher);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebar, stack);
	gtk_widget_class_bind_template_callback (widget_class, pps_sidebar_child_change_cb);

	g_object_class_install_property (g_object_class,
					 PROP_CURRENT_PAGE,
					 g_param_spec_object ("current-page",
							      "Current page",
							      "The currently visible page",
							      GTK_TYPE_WIDGET,
							      G_PARAM_READWRITE |
							      G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (g_object_class,
					 PROP_DOCUMENT_MODEL,
					 g_param_spec_object ("document-model",
							      "DocumentModel",
							      "The document model",
							      PPS_TYPE_DOCUMENT_MODEL,
							      G_PARAM_WRITABLE |
							      G_PARAM_CONSTRUCT_ONLY |
                                                              G_PARAM_STATIC_STRINGS));
}

static void
pps_sidebar_child_change_cb (GObject    *gobject,
			    GParamSpec *pspec,
			    PpsSidebar  *pps_sidebar)
{
	PpsSidebarPrivate *priv = GET_PRIVATE (pps_sidebar);
	GtkStack *stack = GTK_STACK (priv->stack);
	const gchar *name;

	name = gtk_stack_get_visible_child_name (stack);
	if (name)
		g_object_notify (G_OBJECT (pps_sidebar), "current-page");
}

static void
pps_sidebar_init (PpsSidebar *pps_sidebar)
{
	gtk_widget_init_template (GTK_WIDGET (pps_sidebar));
}

static gboolean
pps_sidebar_current_page_support_document (PpsSidebar  *pps_sidebar,
                                          PpsDocument *document)
{
	GtkWidget *current_page = pps_sidebar_get_current_page (pps_sidebar);

	return pps_sidebar_page_support_document (PPS_SIDEBAR_PAGE (current_page),
						 document);
}

static void
pps_sidebar_document_changed_cb (PpsDocumentModel *model,
				GParamSpec      *pspec,
				PpsSidebar       *pps_sidebar)
{
	PpsSidebarPrivate *priv = GET_PRIVATE (pps_sidebar);
	PpsDocument *document = pps_document_model_get_document (model);
	GListModel *list = G_LIST_MODEL (gtk_stack_get_pages (GTK_STACK (priv->stack)));
	PpsSidebarPage *first_supported_page = NULL, *sidebar_page;
	guint i = 0;
	GtkStackPage *page;
	gboolean supported;

	while ((page = g_list_model_get_item (list, i)) != NULL) {
		sidebar_page = PPS_SIDEBAR_PAGE (gtk_stack_page_get_child (page));

		supported = pps_sidebar_page_support_document (sidebar_page, document);
		gtk_stack_page_set_visible (page, supported);

		if (supported && !first_supported_page)
			first_supported_page = sidebar_page;

		i++;
	}

	if (first_supported_page != NULL) {
		if (!pps_sidebar_current_page_support_document (pps_sidebar, document)) {
			pps_sidebar_set_page (pps_sidebar, GTK_WIDGET (first_supported_page));
		}
	} else {
		gtk_widget_set_visible (GTK_WIDGET (pps_sidebar), FALSE);
	}
}

static GObject *
pps_sidebar_buildable_get_internal_child (GtkBuildable *buildable,
                             GtkBuilder   *builder,
                             const char   *childname)
{
        PpsSidebar *sidebar = PPS_SIDEBAR (buildable);
	PpsSidebarPrivate *priv = GET_PRIVATE (sidebar);

        if (g_strcmp0 (childname, "stack") == 0)
                return G_OBJECT (priv->stack);

        return parent_buildable_iface->get_internal_child (buildable, builder, childname);
}

static void
pps_sidebar_buildable_iface_init (GtkBuildableIface *iface)
{
        parent_buildable_iface = g_type_interface_peek_parent (iface);

        iface->get_internal_child = pps_sidebar_buildable_get_internal_child;
}

/* Public functions */

GtkWidget *
pps_sidebar_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_SIDEBAR, NULL));
}

void
pps_sidebar_set_model (PpsSidebar       *pps_sidebar,
		      PpsDocumentModel *model)
{
	PpsSidebarPrivate *priv;

	g_return_if_fail (PPS_IS_SIDEBAR (pps_sidebar));
	g_return_if_fail (PPS_IS_DOCUMENT_MODEL (model));

	priv = GET_PRIVATE (pps_sidebar);

	if (model == priv->model)
		return;

	if (priv->model) {
		g_signal_handlers_disconnect_by_func (priv->model,
			G_CALLBACK (pps_sidebar_document_changed_cb), pps_sidebar);
		g_object_unref (priv->model);
	}

	priv->model = g_object_ref (model);
	g_signal_connect (model, "notify::document",
			  G_CALLBACK (pps_sidebar_document_changed_cb),
			  pps_sidebar);
}

void
pps_sidebar_set_page (PpsSidebar   *pps_sidebar,
		     GtkWidget   *main_widget)
{
	PpsSidebarPrivate *priv;

	g_return_if_fail (PPS_IS_SIDEBAR (pps_sidebar));
	g_return_if_fail (GTK_IS_WIDGET (main_widget));

	priv = GET_PRIVATE (pps_sidebar);

	gtk_stack_set_visible_child (GTK_STACK (priv->stack),
				     main_widget);
	g_object_notify (G_OBJECT (pps_sidebar), "current-page");
}
