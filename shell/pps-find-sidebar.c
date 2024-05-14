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

        PpsJobFind *job;
} PpsFindSidebarPrivate;

enum {
        TEXT_COLUMN,
        PAGE_LABEL_COLUMN,
        PAGE_COLUMN,
        RESULT_COLUMN,

        N_COLUMNS
};

G_DEFINE_TYPE_WITH_PRIVATE (PpsFindSidebar, pps_find_sidebar, GTK_TYPE_STACK)

#define GET_PRIVATE(o) pps_find_sidebar_get_instance_private (o)

static void
pps_find_sidebar_cancel (PpsFindSidebar *sidebar)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);

        g_clear_object (&priv->job);
}

static void
pps_find_sidebar_dispose (GObject *object)
{
        PpsFindSidebar *sidebar = PPS_FIND_SIDEBAR (object);

        pps_find_sidebar_cancel (sidebar);

        G_OBJECT_CLASS (pps_find_sidebar_parent_class)->dispose (object);
}

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
        GObjectClass *g_object_class = G_OBJECT_CLASS (find_sidebar_class);

        g_object_class->dispose = pps_find_sidebar_dispose;

        gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (find_sidebar_class),
                                                     "/org/gnome/papers/ui/find-sidebar.ui");

        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (find_sidebar_class),
                                                      PpsFindSidebar, list_view);

        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (find_sidebar_class),
                                                      PpsFindSidebar, selection);

        gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (find_sidebar_class),
                                                      PpsFindSidebar, model);

        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), pps_spinner_map_cb);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), pps_spinner_unmap_cb);

        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), factory_setup_cb);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), factory_bind_cb);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), factory_unbind_cb);

        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (find_sidebar_class), selection_changed_cb);
}

static void
pps_find_sidebar_reset_model (PpsFindSidebar *sidebar)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);

        g_list_store_remove_all (priv->model);
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
        gint                   index = 0;
        gint                   i;

        if (!priv->job)
                return;

        for (i = 0; i < page; i++)
                index += pps_job_find_get_n_main_results (priv->job, i);

        gtk_list_view_scroll_to (list_view, index, GTK_LIST_SCROLL_SELECT, NULL);
}

static gchar *
sanitized_substring (const gchar  *text,
                     gint          start,
                     gint          end)
{
        const gchar *p;
        const gchar *start_ptr;
        const gchar *end_ptr;
        guint        len = 0;
        gchar       *retval;

        if (end - start <= 0)
                return NULL;

        start_ptr = g_utf8_offset_to_pointer (text, start);
        end_ptr = g_utf8_offset_to_pointer (start_ptr, end - start);

        retval = g_malloc (end_ptr - start_ptr + 1);
        p = start_ptr;

        while (p != end_ptr) {
                const gchar *next;

                next = g_utf8_next_char (p);

                if (next != end_ptr) {
                        GUnicodeBreakType break_type;

                        break_type = g_unichar_break_type (g_utf8_get_char (p));
                        if (break_type == G_UNICODE_BREAK_HYPHEN && *next == '\n') {
                                p = g_utf8_next_char (next);
                                continue;
                        }
                }

                if (*p != '\n') {
                        strncpy (retval + len, p, next - p);
                        len += next - p;
                } else {
                        *(retval + len) = ' ';
                        len++;
                }

                p = next;
        }

        if (len == 0) {
                g_free (retval);

                return NULL;
        }

        retval[len] = 0;

        return retval;
}

static gchar *
get_surrounding_text_markup (const gchar  *text,
                             const gchar  *find_text,
                             gboolean      case_sensitive,
                             PangoLogAttr *log_attrs,
                             gint          log_attrs_length,
                             gint          offset,
                             gboolean      has_nextline,
                             gboolean      hyphen_was_ignored)
{
        gint   iter;
        gchar *prec = NULL;
        gchar *succ = NULL;
        gchar *match = NULL;
        gchar *markup;
        gint   max_chars;

        iter = MAX (0, offset - 1);
        while (!log_attrs[iter].is_word_start && iter > 0)
                iter--;

        prec = sanitized_substring (text, iter, offset);

        iter = offset;
        offset += g_utf8_strlen (find_text, -1);

        if (has_nextline || g_utf8_offset_to_pointer (text, offset-1)[0] == '\n') {
                if (has_nextline) {
                        offset += 1; /* for newline */
                        if (hyphen_was_ignored)
                                offset += 1; /* for hyphen */
                }
                match = sanitized_substring (text, iter, offset);
        } else if (!case_sensitive)
                match = g_utf8_substring (text, iter, offset);

        iter = MIN (log_attrs_length, offset + 1);
        max_chars = MIN (log_attrs_length - 1, iter + 100);
        while (TRUE) {
                gint word = iter;

                while (!log_attrs[word].is_word_end && word < max_chars)
                        word++;

                if (word > max_chars)
                        break;

                iter = word + 1;
        }

        succ = sanitized_substring (text, offset, iter);

        markup = g_markup_printf_escaped ("%s<span weight=\"bold\">%s</span>%s",
                                          prec ? prec : "", match ? match : find_text, succ ? succ : "");
        g_free (prec);
        g_free (succ);
        g_free (match);

        return markup;
}

static gchar *
get_page_text (PpsDocument   *document,
               PpsPage       *page,
               PpsRectangle **areas,
               guint        *n_areas)
{
        gchar   *text;
        gboolean success;

        pps_document_doc_mutex_lock ();
        text = pps_document_text_get_text (PPS_DOCUMENT_TEXT (document), page);
        success = pps_document_text_get_text_layout (PPS_DOCUMENT_TEXT (document), page, areas, n_areas);
        pps_document_doc_mutex_unlock ();

        if (!success) {
                g_free (text);
                return NULL;
        }

        return text;
}

static gint
get_match_offset (PpsRectangle *areas,
                  guint        n_areas,
                  PpsFindRectangle *match,
                  gint         offset)
{
        gdouble x, y;
        gint i;

        x = match->x1;
        y = (match->y1 + match->y2) / 2;

        i = offset;

        do {
                PpsRectangle *area = areas + i;
                gdouble area_y = (area->y1 + area->y2) / 2;
                gdouble area_x = (area->x1 + area->x2) / 2;

                if (x >= area->x1 && x < area->x2 &&
                    y >= area->y1 && y <= area->y2 &&
                    area_x >= match->x1 && area_x <= match->x2 &&
                    area_y >= match->y1 && area_y <= match->y2) {
                        return i;
                }

                i = (i + 1) % n_areas;
        } while (i != offset);

        return -1;
}

static void
process_matches_idle (PpsFindSidebar *sidebar)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);
        PpsDocument           *document;
        gint                   first_match_page = -1;
        g_autoptr (GPtrArray)  results_array = g_ptr_array_new ();
        PpsSearchResult      **results;
        gsize                  n_results;

        g_return_if_fail (PPS_IS_JOB (priv->job));

        if (!pps_job_find_has_results (priv->job)) {
                if (pps_job_is_finished (PPS_JOB (priv->job)))
                        g_clear_object (&priv->job);
                return;
        }

        document = pps_job_get_document (PPS_JOB (priv->job));

        for (guint current_page = 0; current_page < pps_document_get_n_pages (document); current_page++) {
                GList        *matches, *l;
                PpsPage      *page;
                guint         index;
                gchar        *page_label;
                gchar        *page_text;
                PpsRectangle *areas = NULL;
                guint         n_areas;
                PangoLogAttr *text_log_attrs;
                gulong        text_log_attrs_length;
                gint          offset;

                matches = priv->job->pages[current_page];
                if (!matches)
                        continue;

                page = pps_document_get_page (document, current_page);
                page_label = pps_document_get_page_label (document, current_page);
                page_text = get_page_text (document, page, &areas, &n_areas);
                g_object_unref (page);
                if (!page_text)
                        continue;

                text_log_attrs_length = g_utf8_strlen (page_text, -1);
                text_log_attrs = g_new0 (PangoLogAttr, text_log_attrs_length + 1);
                pango_get_log_attrs (page_text, -1, -1, NULL, text_log_attrs, text_log_attrs_length + 1);

                if (first_match_page == -1 && current_page >= priv->job->start_page)
                        first_match_page = current_page;

                offset = 0;

                for (l = matches, index = 0; l; l = g_list_next (l), index++) {
                        PpsFindRectangle *match = (PpsFindRectangle *)l->data;
                        PpsSearchResult  *result;
                        gchar            *markup;
                        gint              new_offset;

                        if (l->prev && ((PpsFindRectangle *)l->prev->data)->next_line)
                                continue; /* Skip as this is second part of a multi-line match */

                        new_offset = get_match_offset (areas, n_areas, match, offset);
                        if (new_offset == -1) {
                                /* It may happen that a text match has no corresponding text area available,
                                 * (due to limitations/bugs of Poppler's TextPage->getSelectionWords() used by
                                 * poppler-glib poppler_page_get_text_layout_for_area() function) so in that
                                 * case we just show matched text because we cannot retrieve surrounding text.
                                 * Issue #1943 and related #1545 */
                                markup = g_strdup_printf ("<b>%s</b>", priv->job->text);
                        } else {
                                offset = new_offset;
                                markup = get_surrounding_text_markup (page_text,
                                                                      priv->job->text,
                                                                      priv->job->options & PPS_FIND_CASE_SENSITIVE,
                                                                      text_log_attrs,
                                                                      text_log_attrs_length,
                                                                      offset,
                                                                      match->next_line,
                                                                      match->after_hyphen);
                        }

                        result = pps_search_result_new (g_strdup (markup),
                                                        g_strdup (page_label),
                                                        current_page,
                                                        index);
                        g_ptr_array_add (results_array, result);

                        g_free (markup);
                }

                g_free (page_label);
                g_free (page_text);
                g_free (text_log_attrs);
                g_free (areas);
        }

        results = (PpsSearchResult**) g_ptr_array_steal (results_array, &n_results);
        if (n_results > 0)
                g_list_store_splice (priv->model, 0, 0, (gpointer*) results, (guint) n_results);

        if (first_match_page != -1)
                pps_find_sidebar_highlight_first_match_of_page (sidebar, first_match_page);
}

static void
find_job_finished_cb (PpsJobFind     *job,
                      PpsFindSidebar *sidebar)
{
        g_idle_add_once ((GSourceOnceFunc)process_matches_idle, sidebar);

        if (pps_job_find_has_results (job)) {
                gtk_stack_set_visible_child_name (GTK_STACK (sidebar), "results");
        } else {
                gtk_stack_set_visible_child_name (GTK_STACK (sidebar), "no-results");
        }
}

static void
find_job_cancelled_cb (PpsJob         *job,
                       PpsFindSidebar *sidebar)
{
        pps_find_sidebar_cancel (sidebar);
}

void
pps_find_sidebar_start (PpsFindSidebar *sidebar,
                       PpsJobFind     *job)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);

        if (priv->job == job)
                return;

        pps_find_sidebar_clear (sidebar);
        priv->job = g_object_ref (job);
        g_signal_connect_object (job, "finished",
                                 G_CALLBACK (find_job_finished_cb),
                                 sidebar, 0);
        g_signal_connect_object (job, "cancelled",
                                 G_CALLBACK (find_job_cancelled_cb),
                                 sidebar, 0);

        gtk_stack_set_visible_child_name (GTK_STACK (sidebar), "loading");
}

void
pps_find_sidebar_restart (PpsFindSidebar *sidebar,
                         gint           page)
{
        PpsFindSidebarPrivate *priv = GET_PRIVATE (sidebar);
        gint                   first_match_page = -1;
        gint                   i;

        if (!priv->job)
                return;

        for (i = 0; i < priv->job->n_pages; i++) {
                int index;

                index = page + i;

                if (index >= priv->job->n_pages)
                        index -= priv->job->n_pages;

                if (priv->job->pages[index]) {
                        first_match_page = index;
                        break;
                }
        }

        if (first_match_page != -1)
                pps_find_sidebar_highlight_first_match_of_page (sidebar, first_match_page);
}

void
pps_find_sidebar_clear (PpsFindSidebar *sidebar)
{
        pps_find_sidebar_cancel (sidebar);

        /* It seems it's more efficient to set a new model in the tree view instead of
         * clearing the model that would emit row-deleted signal for every row in the model
         */
        pps_find_sidebar_reset_model (sidebar);

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

	g_signal_connect_object (priv->context, "started",
				 G_CALLBACK (pps_find_sidebar_start),
				 sidebar, G_CONNECT_SWAPPED);
	g_signal_connect_object (priv->context, "cleared",
				 G_CALLBACK (pps_find_sidebar_clear),
				 sidebar, G_CONNECT_SWAPPED);
}
