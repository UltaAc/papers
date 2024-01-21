/* pps-toolbar.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2012-2014 Carlos Garcia Campos <carlosgc@gnome.org>
 * Copyright (C) 2014-2018 Germán Poo-Caamaño <gpoo@gnome.org>
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

#include <glib/gi18n.h>

#include "pps-toolbar.h"

#include "pps-zoom-action.h"
#include "pps-application.h"
#include "pps-page-selector.h"

enum
{
	PROP_0,
	PROP_DOCUMENT_MODEL,
};

typedef struct {
	AdwHeaderBar *header_bar;

	GtkWidget *sidebar_button;
	GtkWidget *page_selector;
	GtkWidget *annots_button;
	GtkWidget *zoom_action;
	GtkWidget *find_button;
	GtkWidget *action_menu_button;

	PpsDocumentModel *model;

} PpsToolbarPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PpsToolbar, pps_toolbar, ADW_TYPE_BIN)

#define GET_PRIVATE(o) pps_toolbar_get_instance_private (o)

static void
pps_toolbar_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
        PpsToolbar *pps_toolbar = PPS_TOOLBAR (object);
	PpsToolbarPrivate *priv = GET_PRIVATE (pps_toolbar);

        switch (prop_id) {
	case PROP_DOCUMENT_MODEL:
		priv->model = g_value_get_object (value);
		break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
}

static void
pps_toolbar_zoom_selector_activated (GtkWidget *zoom_action,
				    PpsToolbar *pps_toolbar)
{
	GtkRoot *window = gtk_widget_get_root (zoom_action);

	g_return_if_fail (PPS_IS_WINDOW (window));

	pps_window_focus_view (PPS_WINDOW (window));
}

static void
pps_toolbar_find_button_sensitive_changed (GtkWidget  *find_button,
					  GParamSpec *pspec,
					  PpsToolbar *pps_toolbar)
{
        if (gtk_widget_is_sensitive (find_button)) {
                gtk_widget_set_tooltip_text (find_button,
                                             _("Find a word or phrase in the document"));
		gtk_button_set_icon_name (GTK_BUTTON (find_button), "edit-find-symbolic");
	} else {
                gtk_widget_set_tooltip_text (find_button,
                                             _("Search not available for this document"));
		gtk_button_set_icon_name (GTK_BUTTON (find_button), "find-unsupported-symbolic");
	}
}

static void
pps_toolbar_constructed (GObject *object)
{
        PpsToolbar      *pps_toolbar = PPS_TOOLBAR (object);
	PpsToolbarPrivate *priv = GET_PRIVATE (pps_toolbar);

        G_OBJECT_CLASS (pps_toolbar_parent_class)->constructed (object);

	pps_page_selector_set_model (PPS_PAGE_SELECTOR (priv->page_selector),
					 priv->model);

	pps_zoom_action_set_model (PPS_ZOOM_ACTION (priv->zoom_action), priv->model);
}

static void
pps_toolbar_class_init (PpsToolbarClass *klass)
{
        GObjectClass *g_object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

        g_object_class->set_property = pps_toolbar_set_property;
        g_object_class->constructed = pps_toolbar_constructed;

	gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/papers/ui/pps-toolbar.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsToolbar, header_bar);
	gtk_widget_class_bind_template_child_private (widget_class, PpsToolbar, sidebar_button);
	gtk_widget_class_bind_template_child_private (widget_class, PpsToolbar, page_selector);
	gtk_widget_class_bind_template_child_private (widget_class, PpsToolbar, annots_button);
	gtk_widget_class_bind_template_child_private (widget_class, PpsToolbar, action_menu_button);
	gtk_widget_class_bind_template_child_private (widget_class, PpsToolbar, find_button);
	gtk_widget_class_bind_template_child_private (widget_class, PpsToolbar, zoom_action);
	gtk_widget_class_bind_template_callback (widget_class, pps_toolbar_find_button_sensitive_changed);
	gtk_widget_class_bind_template_callback (widget_class, pps_toolbar_zoom_selector_activated);

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
pps_toolbar_init (PpsToolbar *pps_toolbar)
{
        /* Ensure GTK+ private types used by the template
         * definition before calling gtk_widget_init_template() */
        g_type_ensure (PPS_TYPE_PAGE_SELECTOR);
        g_type_ensure (PPS_TYPE_ZOOM_ACTION);

	gtk_widget_init_template (GTK_WIDGET (pps_toolbar));
}

GtkWidget *
pps_toolbar_new (void)
{
        return GTK_WIDGET (g_object_new (PPS_TYPE_TOOLBAR, NULL));
}

GtkWidget *
pps_toolbar_get_page_selector (PpsToolbar *pps_toolbar)
{
	PpsToolbarPrivate *priv;

        g_return_val_if_fail (PPS_IS_TOOLBAR (pps_toolbar), NULL);

        priv = GET_PRIVATE (pps_toolbar);

        return priv->page_selector;
}

AdwHeaderBar *
pps_toolbar_get_header_bar (PpsToolbar *pps_toolbar)
{
	PpsToolbarPrivate *priv;

        g_return_val_if_fail (PPS_IS_TOOLBAR (pps_toolbar), NULL);

        priv = GET_PRIVATE (pps_toolbar);

        return priv->header_bar;
}
