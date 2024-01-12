/*
 *  Copyright (C) 2003, 2004 Marco Pesenti Gritti
 *  Copyright (C) 2003, 2004 Christian Persch
 *  Copyright (C) 2018       Germán Poo-Caamaño
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

#include "config.h"

#include <string.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include <evince-document.h>
#include "ev-page-selector.h"

#define COMPLETION_RESULTS_WIDTH 50

/* Widget we pass back */
static gboolean ev_page_selector_completion_search_is_enabled (EvPageSelector *proxy);

enum
{
	WIDGET_ACTIVATE_LINK,
	WIDGET_N_SIGNALS
};

struct _EvPageSelector
{
	GtkBox parent;

	EvDocument *document;
	EvDocumentModel *doc_model;

	GtkWidget *entry;
	GtkWidget *label;
	gulong signal_id;
	gulong notify_document_signal_id;
	GtkTreeModel *filter_model;
	GtkTreeModel *model;
	GtkEntryCompletion *completion;
};

static guint widget_signals[WIDGET_N_SIGNALS] = {0, };

G_DEFINE_TYPE (EvPageSelector, ev_page_selector, GTK_TYPE_BOX)

static gboolean
show_page_number_in_pages_label (EvPageSelector *page_selector,
                                 gint                page)
{
        gchar   *page_label;
        gboolean retval;

        if (!ev_document_has_text_page_labels (page_selector->document))
                return FALSE;

        page_label = g_strdup_printf ("%d", page + 1);
        retval = g_strcmp0 (page_label, gtk_editable_get_text (GTK_EDITABLE (page_selector->entry))) != 0;
        g_free (page_label);

        return retval;
}

static void
update_pages_label (EvPageSelector *page_selector,
		    gint            page)
{
	char *label_text;
	gint n_pages;

	n_pages = ev_document_get_n_pages (page_selector->document);
        if (show_page_number_in_pages_label (page_selector, page))
                label_text = g_strdup_printf (_("(%d of %d)"), page + 1, n_pages);
        else
                label_text = g_strdup_printf (_("of %d"), n_pages);
	gtk_label_set_text (GTK_LABEL (page_selector->label), label_text);
	g_free (label_text);
}

static void
ev_page_selector_set_current_page (EvPageSelector *page_selector,
				   gint                page)
{
	if (page >= 0) {
		gchar *page_label;

		page_label = ev_document_get_page_label (page_selector->document, page);
		gtk_editable_set_text (GTK_EDITABLE (page_selector->entry), page_label);
		gtk_editable_set_position (GTK_EDITABLE (page_selector->entry), -1);
		g_free (page_label);
	} else {
		gtk_editable_set_text (GTK_EDITABLE (page_selector->entry), "");
	}

	update_pages_label (page_selector, page);
}

static void
ev_page_selector_update_max_width (EvPageSelector *page_selector)
{
        gchar *max_label;
        gint   n_pages;
        gint   max_label_len;
        gchar *max_page_label;
        gchar *max_page_numeric_label;

        n_pages = ev_document_get_n_pages (page_selector->document);

        max_page_label = ev_document_get_page_label (page_selector->document, n_pages - 1);
        max_page_numeric_label = g_strdup_printf ("%d", n_pages);
        if (ev_document_has_text_page_labels (page_selector->document) != 0) {
                max_label = g_strdup_printf (_("(%d of %d)"), n_pages, n_pages);
                /* Do not take into account the parentheses for the size computation */
                max_label_len = g_utf8_strlen (max_label, -1) - 2;
        } else {
                max_label = g_strdup_printf (_("of %d"), n_pages);
                max_label_len = g_utf8_strlen (max_label, -1);
        }
        g_free (max_page_label);

        gtk_label_set_width_chars (GTK_LABEL (page_selector->label), max_label_len);
        g_free (max_label);

        max_label_len = ev_document_get_max_label_len (page_selector->document);
        gtk_editable_set_width_chars (GTK_EDITABLE (page_selector->entry),
                                   CLAMP (max_label_len, strlen (max_page_numeric_label) + 1, 12));
        g_free (max_page_numeric_label);
}

static void
page_changed_cb (EvDocumentModel    *model,
		 gint                old_page,
		 gint                new_page,
		 EvPageSelector *page_selector)
{
	ev_page_selector_set_current_page (page_selector, new_page);
}

static gboolean
page_scroll_cb (GtkEventControllerScroll	*self,
		gdouble				 dx,
		gdouble				 dy,
		gpointer			 user_data)
{
	EvPageSelector *page_selector = EV_PAGE_SELECTOR (user_data);
	EvDocumentModel *model = page_selector->doc_model;
	GdkEvent *event = gtk_event_controller_get_current_event (
			GTK_EVENT_CONTROLLER (self));
	GdkScrollDirection direction = gdk_scroll_event_get_direction (event);
	gint pageno = ev_document_model_get_page (model);

	if ((direction == GDK_SCROLL_DOWN) &&
	    (pageno < ev_document_get_n_pages (page_selector->document) - 1))
		pageno++;
	if ((direction == GDK_SCROLL_UP) && (pageno > 0))
		pageno--;
	ev_document_model_set_page (model, pageno);

	return TRUE;
}

static void
activate_cb (EvPageSelector *page_selector)
{
	EvDocumentModel *model;
	const char *text;
	EvLinkDest *link_dest;
	EvLinkAction *link_action;
	EvLink *link;
	gchar *link_text;
	gchar *new_text;
	gint current_page;

	model = page_selector->doc_model;
	current_page = ev_document_model_get_page (model);

	text = gtk_editable_get_text (GTK_EDITABLE (page_selector->entry));

	/* If we are not in search mode, i.e. we are entering a page number */
	if (!ev_page_selector_completion_search_is_enabled (page_selector)) {
		/* Convert utf8 fullwidth numbers (eg. japanese) to halfwidth - fixes #1518 */
		new_text = g_utf8_normalize (text, -1, G_NORMALIZE_ALL);
		gtk_editable_set_text (GTK_EDITABLE (page_selector->entry), new_text);
		text = gtk_editable_get_text (GTK_EDITABLE (page_selector->entry));
		g_free (new_text);
	}

	link_dest = ev_link_dest_new_page_label (text);
	link_action = ev_link_action_new_dest (link_dest);
	link_text = g_strdup_printf (_("Page %s"), text);
	link = ev_link_new (link_text, link_action);

	g_signal_emit (page_selector, widget_signals[WIDGET_ACTIVATE_LINK], 0, link);

	g_object_unref (link_dest);
	g_object_unref (link_action);
	g_object_unref (link);
	g_free (link_text);

	if (current_page == ev_document_model_get_page (model))
		ev_page_selector_set_current_page (page_selector, current_page);
}

static void
disable_completion_search (EvPageSelector *page_selector)
{
	ev_page_selector_enable_completion_search (page_selector, FALSE);
}

static gboolean
focus_out_cb (EvPageSelector *page_selector)
{
        ev_page_selector_set_current_page (page_selector,
                                                ev_document_model_get_page (page_selector->doc_model));
        g_object_set (page_selector->entry, "xalign", 1.0, NULL);
        ev_page_selector_update_max_width (page_selector);
	g_idle_add_once ((GSourceOnceFunc)disable_completion_search, page_selector);

        return FALSE;
}

static void
ev_page_selector_init (EvPageSelector *page_selector)
{
	gtk_widget_init_template (GTK_WIDGET (page_selector));
}

static void
ev_page_selector_clear_document(EvPageSelector *page_selector)
{
	g_clear_object (&page_selector->document);

	// doc_model is weak pointer, so it might be NULL while we have non-NULL
	// handlers. Clearing the signals in such case is an error. We don't
	// really have to worry about setting the ids to 0, since we're already
	// in finalize
	if (page_selector->doc_model != NULL) {
		g_clear_signal_handler (&page_selector->signal_id,
					page_selector->doc_model);
	}
}

static void
ev_page_selector_set_document (EvPageSelector *page_selector,
                               EvDocument     *document)
{
	if (document == NULL)
		return;

	ev_page_selector_clear_document (page_selector);
	page_selector->document = g_object_ref (document);
	gtk_widget_set_sensitive (GTK_WIDGET (page_selector), ev_document_get_n_pages (document) > 0);

        page_selector->signal_id =
                g_signal_connect (page_selector->doc_model,
                                  "page-changed",
                                  G_CALLBACK (page_changed_cb),
                                  page_selector);

        ev_page_selector_set_current_page (page_selector,
                                                ev_document_model_get_page (page_selector->doc_model));
        ev_page_selector_update_max_width (page_selector);
}

static void
ev_page_selector_document_changed_cb (EvDocumentModel    *model,
				      GParamSpec         *pspec,
				      EvPageSelector     *page_selector)
{
        ev_page_selector_set_document (page_selector, ev_document_model_get_document (model));
}

void
ev_page_selector_set_model (EvPageSelector  *page_selector,
			    EvDocumentModel *model)
{
	g_clear_weak_pointer (&page_selector->doc_model);
	page_selector->doc_model = model;
	g_object_add_weak_pointer (G_OBJECT (model),
				   (gpointer)&page_selector->doc_model);

        ev_page_selector_set_document (page_selector, ev_document_model_get_document (model));

	page_selector->notify_document_signal_id =
		g_signal_connect (model, "notify::document",
				  G_CALLBACK (ev_page_selector_document_changed_cb),
				  page_selector);
}

static void
ev_page_selector_finalize (GObject *object)
{
	EvPageSelector *page_selector = EV_PAGE_SELECTOR (object);

	// doc_model is weak pointer, so it might be NULL while we have non-NULL
	// handlers. Clearing the signals in such case is an error. We don't
	// really have to worry about setting the ids to 0, since we're already
	// in finalize
	if (page_selector->doc_model != NULL) {
		g_clear_signal_handler (&page_selector->notify_document_signal_id,
					page_selector->doc_model);
	}
	ev_page_selector_clear_document (page_selector);
	g_clear_weak_pointer (&page_selector->doc_model);

	g_clear_object (&page_selector->completion);

	G_OBJECT_CLASS (ev_page_selector_parent_class)->finalize (object);
}

static gboolean
ev_page_selector_grab_focus (GtkWidget *proxy)
{
	return gtk_widget_grab_focus (EV_PAGE_SELECTOR (proxy)->entry);
}

static void
ev_page_selector_class_init (EvPageSelectorClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->finalize = ev_page_selector_finalize;
	widget_class->grab_focus = ev_page_selector_grab_focus;

	gtk_widget_class_set_template_from_resource (widget_class,
						     "/org/gnome/evince/ui/ev-page-selector.ui");
	gtk_widget_class_bind_template_child (widget_class, EvPageSelector, entry);
	gtk_widget_class_bind_template_child (widget_class, EvPageSelector, label);

	gtk_widget_class_bind_template_callback (widget_class, page_scroll_cb);
	gtk_widget_class_bind_template_callback (widget_class, activate_cb);
	gtk_widget_class_bind_template_callback (widget_class, focus_out_cb);

	widget_signals[WIDGET_ACTIVATE_LINK] =
		g_signal_new ("activate_link",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (EvPageSelectorClass, activate_link),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE, 1,
			      G_TYPE_OBJECT);

}

static gboolean
match_selected_cb (GtkEntryCompletion *completion,
		   GtkTreeModel       *filter_model,
		   GtkTreeIter        *filter_iter,
		   EvPageSelector *proxy)
{
	EvLink *link;
	GtkTreeIter *iter;

	gtk_tree_model_get (filter_model, filter_iter,
			    0, &iter,
			    -1);
	gtk_tree_model_get (proxy->model, iter,
			    EV_DOCUMENT_LINKS_COLUMN_LINK, &link,
			    -1);

	g_signal_emit (proxy, widget_signals[WIDGET_ACTIVATE_LINK], 0, link);

	if (link)
		g_object_unref (link);

	gtk_tree_iter_free (iter);

	return TRUE;
}


static void
display_completion_text (GtkCellLayout      *cell_layout,
			 GtkCellRenderer    *renderer,
			 GtkTreeModel       *filter_model,
			 GtkTreeIter        *filter_iter,
			 EvPageSelector *proxy)
{
	EvLink *link;
	GtkTreeIter *iter;

	gtk_tree_model_get (filter_model, filter_iter,
			    0, &iter,
			    -1);
	gtk_tree_model_get (proxy->model, iter,
			    EV_DOCUMENT_LINKS_COLUMN_LINK, &link,
			    -1);

	g_object_set (renderer, "text", ev_link_get_title (link), NULL);

	if (link)
		g_object_unref (link);

	gtk_tree_iter_free (iter);
}

static gboolean
match_completion (GtkEntryCompletion *completion,
		  const gchar        *key,
		  GtkTreeIter        *filter_iter,
		  EvPageSelector *proxy)
{
	EvLink *link;
	GtkTreeIter *iter;
	const gchar *text = NULL;

	gtk_tree_model_get (gtk_entry_completion_get_model (completion),
			    filter_iter,
			    0, &iter,
			    -1);
	gtk_tree_model_get (proxy->model, iter,
			    EV_DOCUMENT_LINKS_COLUMN_LINK, &link,
			    -1);


	if (link) {
		text = ev_link_get_title (link);
		g_object_unref (link);
	}

	gtk_tree_iter_free (iter);

	if (text && key) {
		gchar *normalized_text;
		gchar *normalized_key;
		gchar *case_normalized_text;
		gchar *case_normalized_key;
		gboolean retval = FALSE;

		normalized_text = g_utf8_normalize (text, -1, G_NORMALIZE_ALL);
		normalized_key = g_utf8_normalize (key, -1, G_NORMALIZE_ALL);
		case_normalized_text = g_utf8_casefold (normalized_text, -1);
		case_normalized_key = g_utf8_casefold (normalized_key, -1);

		if (strstr (case_normalized_text, case_normalized_key))
			retval = TRUE;

		g_free (normalized_text);
		g_free (normalized_key);
		g_free (case_normalized_text);
		g_free (case_normalized_key);

		return retval;
	}

	return FALSE;
}

/* user data to set on the widget. */
#define EPA_FILTER_MODEL_DATA "epa-filter-model"

static gboolean
build_new_tree_cb (GtkTreeModel *model,
		   GtkTreePath  *path,
		   GtkTreeIter  *iter,
		   gpointer      data)
{
	GtkTreeModel *filter_model = GTK_TREE_MODEL (data);
	EvLink *link;
	EvLinkAction *action;
	EvLinkActionType type;

	gtk_tree_model_get (model, iter,
			    EV_DOCUMENT_LINKS_COLUMN_LINK, &link,
			    -1);

	if (!link)
		return FALSE;

	action = ev_link_get_action (link);
	if (!action) {
		g_object_unref (link);
		return FALSE;
	}

	type = ev_link_action_get_action_type (action);

	if (type == EV_LINK_ACTION_TYPE_GOTO_DEST) {
		GtkTreeIter filter_iter;

		gtk_list_store_append (GTK_LIST_STORE (filter_model), &filter_iter);
		gtk_list_store_set (GTK_LIST_STORE (filter_model), &filter_iter,
				    0, iter,
				    -1);
	}

	g_object_unref (link);

	return FALSE;
}

static GtkTreeModel *
get_filter_model_from_model (GtkTreeModel *model)
{
	GtkTreeModel *filter_model;

	filter_model =
		(GtkTreeModel *) g_object_get_data (G_OBJECT (model), EPA_FILTER_MODEL_DATA);
	if (filter_model == NULL) {
		filter_model = (GtkTreeModel *) gtk_list_store_new (1, GTK_TYPE_TREE_ITER);

		gtk_tree_model_foreach (model,
					build_new_tree_cb,
					filter_model);
		g_object_set_data_full (G_OBJECT (model), EPA_FILTER_MODEL_DATA, filter_model, g_object_unref);
	}

	return filter_model;
}


void
ev_page_selector_update_links_model (EvPageSelector *proxy, GtkTreeModel *model)
{
	GtkTreeModel *filter_model;
	GtkEntryCompletion *completion;
	GtkCellRenderer *renderer;

	if (!model || model == proxy->model)
		return;

	/* Magik */
	proxy->model = model;
	filter_model = get_filter_model_from_model (model);

	completion = gtk_entry_completion_new ();
	g_clear_object (&proxy->completion);
	proxy->completion = completion;
	gtk_entry_completion_set_popup_set_width (completion, FALSE);
	gtk_entry_completion_set_model (completion, filter_model);

	g_signal_connect (completion, "match-selected", G_CALLBACK (match_selected_cb), proxy);
	gtk_entry_completion_set_match_func (completion,
					     (GtkEntryCompletionMatchFunc) match_completion,
					     proxy, NULL);

	/* Set up the layout */
	renderer = (GtkCellRenderer *)
		g_object_new (GTK_TYPE_CELL_RENDERER_TEXT,
			      "ellipsize", PANGO_ELLIPSIZE_END,
			      "width_chars", COMPLETION_RESULTS_WIDTH,
			      NULL);
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (completion), renderer, TRUE);
	gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (completion),
					    renderer,
					    (GtkCellLayoutDataFunc) display_completion_text,
					    proxy, NULL);
	gtk_entry_set_completion (GTK_ENTRY (proxy->entry), completion);
}

void
ev_page_selector_clear (EvPageSelector *proxy)
{
	gtk_editable_set_text (GTK_EDITABLE (proxy->entry), "");
}

/* Sets width of the text entry, width will be restablished
 * to default one on focus_out event. This function is used
 * when searching the Outline, so the user has more space
 * to write the search term. */
void
ev_page_selector_set_temporary_entry_width (EvPageSelector *proxy, gint width)
{
	gtk_editable_set_width_chars (GTK_EDITABLE (proxy->entry), width);
	/* xalign will also be restablished on focus_out */
	g_object_set (proxy->entry, "xalign", 0., NULL);
}

/* Enables or disables the completion search on @proxy according to @enable */
void
ev_page_selector_enable_completion_search (EvPageSelector *proxy, gboolean enable)
{
	GtkEntryCompletion *completion = enable ? proxy->completion : NULL;
	gtk_entry_set_completion (GTK_ENTRY (proxy->entry), completion);
}

/* Returns whether the completion search is enabled in @proxy */
static gboolean
ev_page_selector_completion_search_is_enabled (EvPageSelector *proxy)
{
	return gtk_entry_get_completion (GTK_ENTRY (proxy->entry)) != NULL;
}
