/* pps-find-sidebar.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2024 Markus Göllnitz  <camelcasenick@bewares.it>
 * Copyright (C) 2013 Carlos Garcia Campos  <carlosgc@gnome.org>
 * Copyright (C) 2008 Sergey Pushkin  <pushkinsv@gmail.com>
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

#include "pps-find-sidebar.h"
#include "pps-search-result.h"
#include "pps-utils.h"
#include <string.h>

typedef struct {
	PpsSearchContext *context;

        GtkWidget *list_view;
        GtkSingleSelection *selection;
        GListStore *model;
} PpsFindSidebarPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PpsFindSidebar, pps_find_sidebar, GTK_TYPE_STACK)

#define GET_PRIVATE(o) pps_find_sidebar_get_instance_private (o)

static void
selection_changed_cb (GtkSelectionModel *selection,
                      guint              position,
                      guint              n_items,
                      PpsFindSidebar    *sidebar)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);
        PpsSearchResult *selected_result = PPS_SEARCH_RESULT (gtk_single_selection_get_selected_item (GTK_SINGLE_SELECTION (selection)));

	g_return_if_fail (priv->context != NULL);
	g_return_if_fail (selected_result != NULL);

	pps_search_context_select_result (priv->context, selected_result);
}

static void
factory_setup_cb (GtkSignalListItemFactory *self,
                  GObject                  *object,
                  PpsFindSidebar           *sidebar)
{
        GtkListItem *item = GTK_LIST_ITEM (object);
        GtkWidget   *widget;

        widget = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);

        gtk_list_item_set_child (item, widget);
}

static void
factory_bind_cb (GtkSignalListItemFactory *self,
                 GObject                  *object,
                 PpsFindSidebar           *sidebar)
{
        GtkListItem     *item = GTK_LIST_ITEM (object);
        GtkWidget       *widget = gtk_list_item_get_child (item);
        PpsSearchResult *result = PPS_SEARCH_RESULT (gtk_list_item_get_item (item));
        GtkLabel        *result_label;
        GtkLabel        *page_label;

        result_label = GTK_LABEL (gtk_label_new (pps_search_result_get_markup (result)));
        gtk_label_set_use_markup (result_label, TRUE);
        gtk_label_set_ellipsize (result_label, PANGO_ELLIPSIZE_END);
        gtk_widget_set_hexpand (GTK_WIDGET (result_label), TRUE);
        gtk_widget_set_halign (GTK_WIDGET (result_label), GTK_ALIGN_START);

        page_label = GTK_LABEL (gtk_label_new (pps_search_result_get_label (result)));

        gtk_box_append (GTK_BOX (widget), GTK_WIDGET (result_label));
        gtk_box_append (GTK_BOX (widget), GTK_WIDGET (page_label));
}

static void
factory_unbind_cb (GtkSignalListItemFactory *self,
                   GObject                  *object,
                   PpsFindSidebar           *sidebar)
{
        GtkListItem           *item = GTK_LIST_ITEM (object);
        GtkWidget             *widget = gtk_list_item_get_child (item);

        while (gtk_widget_get_first_child (widget))
                gtk_box_remove (GTK_BOX (widget), gtk_widget_get_first_child (widget));
}

static void
pps_find_sidebar_class_init (PpsFindSidebarClass *find_sidebar_class)
{
        gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (find_sidebar_class),
                                                     "/org/gnome/papers/ui/find-sidebar.ui");

        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (find_sidebar_class),
                                                      PpsFindSidebar, list_view);

        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (find_sidebar_class),
                                                      PpsFindSidebar, selection);

        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), pps_spinner_map_cb);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), pps_spinner_unmap_cb);

        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), factory_setup_cb);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), factory_bind_cb);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), factory_unbind_cb);

        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), selection_changed_cb);
}

static void
pps_find_sidebar_init (PpsFindSidebar *sidebar)
{
        gtk_widget_init_template (GTK_WIDGET (sidebar));
}

GtkWidget *
pps_find_sidebar_new (void)
{
        return g_object_new (PPS_TYPE_FIND_SIDEBAR,
                             "orientation", GTK_ORIENTATION_VERTICAL,
                             NULL);
}

static void
pps_find_sidebar_highlight_first_match_of_page (PpsFindSidebar *sidebar,
                                                gint            page)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);
        GtkListView           *list_view = GTK_LIST_VIEW (priv->list_view);
	GListModel            *result_model = pps_search_context_get_result_model (priv->context);
        guint                  index;

        for (index = 0; index < g_list_model_get_n_items (result_model); index++) {
                PpsSearchResult *result = g_list_model_get_item (result_model, index);

		if (pps_search_result_get_page (result) == page)
			break;
        }

        gtk_list_view_scroll_to (list_view, index, GTK_LIST_SCROLL_SELECT, NULL);
}

static void
find_job_finished_cb (PpsSearchContext *search_context,
		      PpsJobFind       *job,
		      gint              first_match_page,
                      PpsFindSidebar   *sidebar)
{
        if (g_list_model_get_n_items (pps_search_context_get_result_model (search_context)) != 0) {
                gtk_stack_set_visible_child_name (GTK_STACK (sidebar), "results");
        } else {
                gtk_stack_set_visible_child_name (GTK_STACK (sidebar), "no-results");
        }

        if (first_match_page != -1)
                pps_find_sidebar_highlight_first_match_of_page (sidebar, first_match_page);
}

static void
pps_find_sidebar_start (PpsFindSidebar *sidebar)
{
        gtk_stack_set_visible_child_name (GTK_STACK (sidebar), "loading");
}

void
pps_find_sidebar_restart (PpsFindSidebar *sidebar,
                         gint           page)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);
        gint                   first_match_page = -1;
	GListModel            *result_model = pps_search_context_get_result_model (priv->context);

        for (guint i = 0; i < g_list_model_get_n_items (result_model); i++) {
                PpsSearchResult *result = g_list_model_get_item (result_model, i);

		if (pps_search_result_get_page (result) >= page) {
			first_match_page = pps_search_result_get_page (result);
			break;
		}
        }

        if (first_match_page != -1)
                pps_find_sidebar_highlight_first_match_of_page (sidebar, first_match_page);
}

void
pps_find_sidebar_clear (PpsFindSidebar *sidebar)
{
        gtk_stack_set_visible_child_name (GTK_STACK (sidebar), "initial");
}

void
pps_find_sidebar_previous (PpsFindSidebar *sidebar)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);
        GtkListView           *list_view = GTK_LIST_VIEW (priv->list_view);
        guint                  pos;

        pos = gtk_single_selection_get_selected (priv->selection) - 1;
        gtk_list_view_scroll_to (list_view, pos, GTK_LIST_SCROLL_SELECT, NULL);
}

void
pps_find_sidebar_next (PpsFindSidebar *sidebar)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);
        GtkListView           *list_view = GTK_LIST_VIEW (priv->list_view);
        guint                  pos;

        pos = gtk_single_selection_get_selected (priv->selection) + 1;
        gtk_list_view_scroll_to (list_view, pos, GTK_LIST_SCROLL_SELECT, NULL);
}

void
pps_find_sidebar_set_search_context (PpsFindSidebar   *sidebar,
				     PpsSearchContext *context)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);

	g_return_if_fail (PPS_IS_SEARCH_CONTEXT (context));

	if (priv->context != NULL) {
		g_signal_handlers_disconnect_by_func (priv->context, pps_find_sidebar_start, sidebar);
		g_signal_handlers_disconnect_by_func (priv->context, pps_find_sidebar_clear, sidebar);
	}

	priv->context = context;

	gtk_single_selection_set_model (priv->selection, pps_search_context_get_result_model (priv->context));
	g_signal_connect_object (priv->context, "started",
				 G_CALLBACK (pps_find_sidebar_start),
				 sidebar, G_CONNECT_SWAPPED);
	g_signal_connect_object (priv->context, "cleared",
				 G_CALLBACK (pps_find_sidebar_clear),
				 sidebar, G_CONNECT_SWAPPED);
	g_signal_connect_object (priv->context, "finished",
				 G_CALLBACK (find_job_finished_cb),
				 sidebar, G_CONNECT_DEFAULT);
}
