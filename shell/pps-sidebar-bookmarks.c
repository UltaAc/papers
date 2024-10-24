/* pps-sidebar-bookmarks.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2010 Carlos Garcia Campos  <carlosgc@gnome.org>
 * Copyright (C) 2020 Germán Poo-Caamaño  <gpoo@gnome.org>
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

#include "config.h"

#include <glib/gi18n.h>

#include "pps-sidebar-bookmarks.h"

#include "pps-document-misc.h"
#include "pps-document.h"
#include "pps-utils.h"

enum {
	COLUMN_MARKUP,
	COLUMN_PAGE,
	N_COLUMNS
};

enum {
	ACTIVATED,
	N_SIGNALS
};

static guint signals[N_SIGNALS] = { 0 };

struct _PpsSidebarBookmarksPrivate {
	PpsBookmarks *bookmarks;

	GtkWidget *tree_view;
	GtkWidget *del_button;
	GtkWidget *add_button;

	/* Popup menu */
	GtkWidget *popup;
};

#define GET_PRIVATE(o) pps_sidebar_bookmarks_get_instance_private (o);

static void pps_sidebar_bookmarks_page_changed (PpsSidebarBookmarks *sidebar_bookmarks,
                                                gint old_page,
                                                gint new_page);
static void pps_sidebar_bookmarks_selection_changed (GtkTreeSelection *selection,
                                                     PpsSidebarBookmarks *sidebar_bookmarks);

G_DEFINE_TYPE_WITH_PRIVATE (PpsSidebarBookmarks, pps_sidebar_bookmarks, PPS_TYPE_SIDEBAR_PAGE)

static gint
pps_sidebar_bookmarks_get_selected_page (PpsSidebarBookmarks *sidebar_bookmarks,
                                         GtkTreeSelection *selection)
{
	GtkTreeModel *model;
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		guint page;

		gtk_tree_model_get (model, &iter,
		                    COLUMN_PAGE, &page,
		                    -1);
		return page;
	}

	return -1;
}

static void
pps_bookmarks_popup_cmd_open_bookmark (GSimpleAction *action,
                                       GVariant *parameter,
                                       gpointer sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	PpsDocumentModel *model = pps_sidebar_page_get_document_model (PPS_SIDEBAR_PAGE (sidebar_bookmarks));
	GtkTreeSelection *selection;
	gint page;
	gint old_page = pps_document_model_get_page (model);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tree_view));
	page = pps_sidebar_bookmarks_get_selected_page (sidebar_bookmarks, selection);
	g_signal_emit (sidebar_bookmarks, signals[ACTIVATED], 0, old_page, page);
	pps_document_model_set_page (model, page);
	pps_sidebar_page_navigate_to_view (PPS_SIDEBAR_PAGE (sidebar_bookmarks));
}

static void
pps_bookmarks_popup_cmd_rename_bookmark (GSimpleAction *action,
                                         GVariant *parameter,
                                         gpointer sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	GtkTreeView *tree_view = GTK_TREE_VIEW (priv->tree_view);
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (tree_view);
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		GtkTreePath *path;

		path = gtk_tree_model_get_path (model, &iter);
		gtk_tree_view_set_cursor (tree_view, path,
		                          gtk_tree_view_get_column (tree_view, 0),
		                          TRUE);
		gtk_tree_path_free (path);
	}
}

static void
pps_bookmarks_popup_cmd_delete_bookmark (GSimpleAction *action,
                                         GVariant *parameter,
                                         gpointer sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	GtkTreeSelection *selection;
	gint page;
	PpsBookmark bm;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tree_view));
	page = pps_sidebar_bookmarks_get_selected_page (sidebar_bookmarks, selection);
	bm.page = page;
	bm.title = NULL;
	pps_bookmarks_delete (priv->bookmarks, &bm);
	if (gtk_widget_get_sensitive (priv->del_button))
		gtk_widget_set_sensitive (priv->del_button, FALSE);
}

static GActionGroup *
create_action_group (PpsSidebarBookmarks *sidebar_bookmarks)
{
	const GActionEntry popup_entries[] = {
		{ "open-bookmark", pps_bookmarks_popup_cmd_open_bookmark },
		{ "rename-bookmark", pps_bookmarks_popup_cmd_rename_bookmark },
		{ "delete-bookmark", pps_bookmarks_popup_cmd_delete_bookmark }
	};
	GSimpleActionGroup *group;

	group = g_simple_action_group_new ();
	g_action_map_add_action_entries (G_ACTION_MAP (group),
	                                 popup_entries,
	                                 G_N_ELEMENTS (popup_entries),
	                                 sidebar_bookmarks);

	return G_ACTION_GROUP (group);
}

static gint
compare_bookmarks (PpsBookmark *a,
                   PpsBookmark *b)
{
	if (a->page < b->page)
		return -1;
	if (a->page > b->page)
		return 1;
	return 0;
}

static void
pps_sidebar_bookmarks_update (PpsSidebarBookmarks *sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	GtkListStore *model;
	GList *items, *l;
	GtkTreeIter iter;
	GtkTreeView *tree_view = GTK_TREE_VIEW (priv->tree_view);
	GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);

	model = GTK_LIST_STORE (gtk_tree_view_get_model (tree_view));

	g_signal_handlers_block_by_func (selection,
	                                 pps_sidebar_bookmarks_selection_changed,
	                                 sidebar_bookmarks);
	gtk_list_store_clear (model);
	g_signal_handlers_unblock_by_func (selection,
	                                   pps_sidebar_bookmarks_selection_changed,
	                                   sidebar_bookmarks);

	if (!priv->bookmarks) {
		g_object_set (priv->tree_view, "has-tooltip", FALSE, NULL);
		return;
	}

	items = pps_bookmarks_get_bookmarks (priv->bookmarks);
	items = g_list_sort (items, (GCompareFunc) compare_bookmarks);
	for (l = items; l; l = g_list_next (l)) {
		PpsBookmark *bm = (PpsBookmark *) l->data;

		gtk_list_store_append (model, &iter);
		gtk_list_store_set (model, &iter,
		                    COLUMN_MARKUP, bm->title,
		                    COLUMN_PAGE, bm->page,
		                    -1);
	}
	g_list_free (items);
	g_object_set (priv->tree_view, "has-tooltip", TRUE, NULL);
}

static void
pps_sidebar_bookmarks_changed (PpsBookmarks *bookmarks,
                               PpsSidebarBookmarks *sidebar_bookmarks)
{
	pps_sidebar_bookmarks_update (sidebar_bookmarks);
}

static void
pps_sidebar_bookmarks_selection_changed (GtkTreeSelection *selection,
                                         PpsSidebarBookmarks *sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	PpsDocumentModel *model = pps_sidebar_page_get_document_model (PPS_SIDEBAR_PAGE (sidebar_bookmarks));
	gint page;

	page = pps_sidebar_bookmarks_get_selected_page (sidebar_bookmarks, selection);
	if (page >= 0) {
		gint old_page = pps_document_model_get_page (model);
		g_signal_emit (sidebar_bookmarks, signals[ACTIVATED], 0, old_page, page);
		pps_document_model_set_page (model, page);
		pps_sidebar_page_navigate_to_view (PPS_SIDEBAR_PAGE (sidebar_bookmarks));
		gtk_widget_set_sensitive (priv->del_button, TRUE);
	} else {
		gtk_widget_set_sensitive (priv->del_button, FALSE);
	}
}

static void
pps_sidebar_bookmarks_page_changed (PpsSidebarBookmarks *sidebar_bookmarks,
                                    gint old_page,
                                    gint new_page)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	GtkTreeSelection *selection;
	gint selected_page;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tree_view));
	selected_page = pps_sidebar_bookmarks_get_selected_page (sidebar_bookmarks, selection);

	if (selected_page != new_page)
		gtk_tree_selection_unselect_all (selection);
}

static void
pps_sidebar_bookmarks_del_clicked (GtkWidget *button,
                                   PpsSidebarBookmarks *sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	GtkTreeSelection *selection;
	gint page;
	PpsBookmark bm;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->tree_view));
	page = pps_sidebar_bookmarks_get_selected_page (sidebar_bookmarks, selection);
	if (page < 0)
		return;

	bm.page = page;
	bm.title = NULL;
	pps_bookmarks_delete (priv->bookmarks, &bm);
	if (gtk_widget_get_sensitive (priv->del_button))
		gtk_widget_set_sensitive (priv->del_button, FALSE);
}

static void
pps_sidebar_bookmarks_bookmark_renamed (GtkCellRendererText *renderer,
                                        const gchar *path_string,
                                        const gchar *new_text,
                                        PpsSidebarBookmarks *sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
	GtkTreeModel *model;
	GtkTreeIter iter;
	guint page;
	PpsBookmark bm;

	if (!new_text || new_text[0] == '\0')
		return;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (priv->tree_view));
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter,
	                    COLUMN_PAGE, &page,
	                    -1);
	gtk_tree_path_free (path);

	bm.page = page;
	bm.title = g_markup_escape_text (new_text, -1);
	pps_bookmarks_update (priv->bookmarks, &bm);
}

static gboolean
pps_sidebar_bookmarks_query_tooltip (GtkWidget *widget,
                                     gint x,
                                     gint y,
                                     gboolean keyboard_tip,
                                     GtkTooltip *tooltip,
                                     PpsSidebarBookmarks *sidebar_bookmarks)
{

	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	PpsDocumentModel *document_model;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path = NULL;
	PpsDocument *document;
	guint page;
	gchar *page_label;
	gchar *text;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW (priv->tree_view));
	if (!gtk_tree_view_get_tooltip_context (GTK_TREE_VIEW (priv->tree_view),
	                                        x, y, keyboard_tip,
	                                        &model, &path, &iter))
		return FALSE;

	gtk_tree_model_get (model, &iter,
	                    COLUMN_PAGE, &page,
	                    -1);

	document_model = pps_sidebar_page_get_document_model (PPS_SIDEBAR_PAGE (sidebar_bookmarks));
	document = pps_document_model_get_document (document_model);
	page_label = pps_document_get_page_label (document, page);
	text = g_strdup_printf (_ ("Page %s"), page_label);
	gtk_tooltip_set_text (tooltip, text);
	g_free (text);
	g_free (page_label);

	gtk_tree_view_set_tooltip_row (GTK_TREE_VIEW (priv->tree_view),
	                               tooltip, path);
	gtk_tree_path_free (path);

	return TRUE;
}

static void
pps_sidebar_bookmarks_button_press_cb (GtkGestureClick *self,
                                       gint n_press,
                                       gdouble x,
                                       gdouble y,
                                       PpsSidebarBookmarks *sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);
	GtkTreeView *tree_view = GTK_TREE_VIEW (priv->tree_view);
	GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
	GdkEvent *event = gtk_event_controller_get_current_event (GTK_EVENT_CONTROLLER (self));
	GtkTreePath *path;

	if (!gdk_event_triggers_context_menu (event))
		return;

	if (!gtk_tree_view_get_path_at_pos (tree_view, x, y,
	                                    &path, NULL, NULL, NULL))
		return;

	g_signal_handlers_block_by_func (selection,
	                                 pps_sidebar_bookmarks_selection_changed,
	                                 sidebar_bookmarks);
	gtk_tree_view_set_cursor (tree_view, path, NULL, FALSE);
	g_signal_handlers_unblock_by_func (selection,
	                                   pps_sidebar_bookmarks_selection_changed,
	                                   sidebar_bookmarks);
	gtk_tree_path_free (path);

	if (!gtk_widget_get_sensitive (priv->del_button))
		gtk_widget_set_sensitive (priv->del_button, TRUE);

	gtk_popover_set_pointing_to (GTK_POPOVER (priv->popup), &(const GdkRectangle) { x, y, 1, 1 });
	gtk_popover_popup (GTK_POPOVER (priv->popup));
}

static gboolean
pps_sidebar_bookmarks_support_document (PpsSidebarPage *sidebar_page,
                                        PpsDocument *document)
{
	return TRUE;
}

static void
pps_sidebar_bookmarks_dispose (GObject *object)
{
	PpsSidebarBookmarks *sidebar_bookmarks = PPS_SIDEBAR_BOOKMARKS (object);
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);

	g_clear_object (&priv->bookmarks);

	G_OBJECT_CLASS (pps_sidebar_bookmarks_parent_class)->dispose (object);
}

static void
pps_sidebar_bookmarks_constructed (GObject *object)
{
	g_signal_connect_swapped (pps_sidebar_page_get_document_model (PPS_SIDEBAR_PAGE (object)),
	                          "page-changed",
	                          G_CALLBACK (pps_sidebar_bookmarks_page_changed),
	                          PPS_SIDEBAR_BOOKMARKS (object));
}

static void
pps_sidebar_bookmarks_init (PpsSidebarBookmarks *sidebar_bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);

	gtk_widget_init_template (GTK_WIDGET (sidebar_bookmarks));

	gtk_widget_insert_action_group (priv->popup,
	                                "bookmarks",
	                                create_action_group (sidebar_bookmarks));
}

static void
pps_sidebar_bookmarks_class_init (PpsSidebarBookmarksClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	PpsSidebarPageClass *sidebar_page_class = PPS_SIDEBAR_PAGE_CLASS (klass);

	g_object_class->constructed = pps_sidebar_bookmarks_constructed;
	g_object_class->dispose = pps_sidebar_bookmarks_dispose;

	sidebar_page_class->support_document = pps_sidebar_bookmarks_support_document;

	gtk_widget_class_set_template_from_resource (widget_class,
	                                             "/org/gnome/papers/ui/sidebar-bookmarks.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarBookmarks, tree_view);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarBookmarks, del_button);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarBookmarks, add_button);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarBookmarks, popup);

	gtk_widget_class_bind_template_callback (widget_class, pps_sidebar_bookmarks_del_clicked);
	gtk_widget_class_bind_template_callback (widget_class, pps_sidebar_bookmarks_bookmark_renamed);
	gtk_widget_class_bind_template_callback (widget_class, pps_sidebar_bookmarks_query_tooltip);
	gtk_widget_class_bind_template_callback (widget_class, pps_sidebar_bookmarks_selection_changed);
	gtk_widget_class_bind_template_callback (widget_class, pps_sidebar_bookmarks_query_tooltip);
	gtk_widget_class_bind_template_callback (widget_class, pps_sidebar_bookmarks_button_press_cb);

	/* Signals */
	signals[ACTIVATED] =
	    g_signal_new ("bookmark-activated",
	                  PPS_TYPE_SIDEBAR_BOOKMARKS,
	                  G_SIGNAL_RUN_LAST,
	                  G_STRUCT_OFFSET (PpsSidebarBookmarksClass, activated),
	                  NULL, NULL,
	                  NULL,
	                  G_TYPE_NONE, 2,
	                  G_TYPE_INT, G_TYPE_INT);
}

GtkWidget *
pps_sidebar_bookmarks_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_SIDEBAR_BOOKMARKS, NULL));
}

void
pps_sidebar_bookmarks_set_bookmarks (PpsSidebarBookmarks *sidebar_bookmarks,
                                     PpsBookmarks *bookmarks)
{
	PpsSidebarBookmarksPrivate *priv = GET_PRIVATE (sidebar_bookmarks);

	g_return_if_fail (PPS_IS_BOOKMARKS (bookmarks));

	if (priv->bookmarks == bookmarks)
		return;

	g_set_object (&priv->bookmarks, bookmarks);

	g_signal_connect (priv->bookmarks, "changed",
	                  G_CALLBACK (pps_sidebar_bookmarks_changed),
	                  sidebar_bookmarks);

	gtk_widget_set_sensitive (priv->add_button, TRUE);
	pps_sidebar_bookmarks_update (sidebar_bookmarks);
}
