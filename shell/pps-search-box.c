/* pps-search-box.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2015 Igalia S.L.
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
#include "pps-search-box.h"

#include <glib/gi18n.h>

typedef struct {
	PpsSearchContext *context;

        GtkWidget       *entry;
        GtkWidget       *next_button;
        GtkWidget       *prev_button;

	gulong search_term_signal;
} PpsSearchBoxPrivate;

static void pps_search_box_buildable_iface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_WITH_CODE (PpsSearchBox, pps_search_box, ADW_TYPE_BIN,
                         G_ADD_PRIVATE (PpsSearchBox)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                                                pps_search_box_buildable_iface_init))

#define GET_PRIVATE(o) pps_search_box_get_instance_private(o)

#define FIND_PAGE_RATE_REFRESH 100

static void
find_job_finished_cb (PpsSearchContext *search_context,
		      PpsJobFind       *job,
		      gint             first_match_page,
                      PpsSearchBox     *box)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);
	gboolean has_results;

	has_results = g_list_model_get_n_items (pps_search_context_get_result_model (search_context)) != 0;

        gtk_widget_set_sensitive (priv->next_button, has_results);
        gtk_widget_set_sensitive (priv->prev_button, has_results);

        if (!has_results) {
                gtk_widget_add_css_class (priv->entry, "error");
        }
}

static void
search_changed_cb (PpsSearchBox    *box)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);
	const gchar *search_term;

        gtk_widget_set_sensitive (priv->next_button, FALSE);
        gtk_widget_set_sensitive (priv->prev_button, FALSE);

	gtk_widget_remove_css_class(priv->entry, "error");

	g_return_if_fail (priv->context != NULL);

	search_term = pps_search_context_get_search_term (priv->context);

	g_signal_handler_block (priv->entry, priv->search_term_signal);
	if (!g_str_equal (gtk_editable_get_text (GTK_EDITABLE (priv->entry)), search_term))
		gtk_editable_set_text (GTK_EDITABLE (priv->entry), search_term);
	g_signal_handler_unblock (priv->entry, priv->search_term_signal);
}

static void
search_term_changed_cb (GtkSearchEntry *entry,
			PpsSearchBox   *box)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

	g_return_if_fail (priv->context != NULL);

	pps_search_context_set_search_term (priv->context, gtk_editable_get_text (GTK_EDITABLE (entry)));
}


static void
whole_words_only_toggled_cb (GSimpleAction *action,
			     GVariant      *state,
			     gpointer       user_data)
{
	PpsSearchBox *box = PPS_SEARCH_BOX (user_data);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);
        PpsFindOptions options = pps_search_context_get_options (priv->context);
	gboolean active = g_variant_get_boolean (state);
	g_simple_action_set_state (action, state);

        if (active)
                options |= PPS_FIND_WHOLE_WORDS_ONLY;
        else
                options &= ~PPS_FIND_WHOLE_WORDS_ONLY;
        pps_search_context_set_options (priv->context, options);
}

static void
case_sensitive_toggled_cb (GSimpleAction *action,
			   GVariant      *state,
			   gpointer       user_data)
{
	PpsSearchBox *box = PPS_SEARCH_BOX (user_data);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);
        PpsFindOptions options = pps_search_context_get_options (priv->context);
	gboolean active = g_variant_get_boolean (state);
	g_simple_action_set_state (action, state);

        if (active)
                options |= PPS_FIND_CASE_SENSITIVE;
        else
                options &= ~PPS_FIND_CASE_SENSITIVE;
        pps_search_context_set_options (priv->context, options);
}

static void
entry_activate_cb (GtkEntry    *entry,
                   PpsSearchBox *box)
{
	gtk_widget_activate_action (GTK_WIDGET (box), "doc.find-next", NULL);
}

static void
entry_next_match_cb (GtkSearchEntry *entry,
                     PpsSearchBox *box)
{
	gtk_widget_activate_action (GTK_WIDGET (box), "doc.find-next", NULL);
}

static void
entry_previous_match_cb (GtkSearchEntry *entry,
                         PpsSearchBox *box)
{
	gtk_widget_activate_action (GTK_WIDGET (box), "doc.find-previous", NULL);
}

static gboolean
pps_search_box_grab_focus (GtkWidget *widget)
{
        PpsSearchBox *box = PPS_SEARCH_BOX (widget);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        return gtk_widget_grab_focus (priv->entry);
}

static void
pps_search_box_dispose (GObject *object)
{
	PpsSearchBox *box = PPS_SEARCH_BOX (object);
	PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

	g_clear_object (&priv->context);

	G_OBJECT_CLASS (pps_search_box_parent_class)->dispose (object);
}

static void
pps_search_box_class_init (PpsSearchBoxClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	GObjectClass   *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = pps_search_box_dispose;

        widget_class->grab_focus = pps_search_box_grab_focus;

	gtk_widget_class_set_template_from_resource (widget_class,
		"/org/gnome/papers/ui/search-box.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsSearchBox, entry);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSearchBox, prev_button);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSearchBox, next_button);

	gtk_widget_class_bind_template_callback (widget_class, case_sensitive_toggled_cb);
	gtk_widget_class_bind_template_callback (widget_class, whole_words_only_toggled_cb);

	gtk_widget_class_bind_template_callback (widget_class, entry_activate_cb);
	gtk_widget_class_bind_template_callback (widget_class, entry_next_match_cb);
	gtk_widget_class_bind_template_callback (widget_class, entry_previous_match_cb);
}

const GActionEntry actions[] = {
	{ "whole-words-only", NULL, NULL, "false", whole_words_only_toggled_cb },
	{ "case-sensitive", NULL, NULL, "false", case_sensitive_toggled_cb },
};

static void
pps_search_box_init (PpsSearchBox *box)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

	GSimpleActionGroup *group;

	gtk_widget_init_template (GTK_WIDGET (box));

	group = g_simple_action_group_new ();
	g_action_map_add_action_entries (G_ACTION_MAP (group), actions,
					 G_N_ELEMENTS (actions), box);

	gtk_widget_insert_action_group (GTK_WIDGET (box), "search", G_ACTION_GROUP (group));

	priv->search_term_signal = g_signal_connect_object (priv->entry, "search-changed",
							    G_CALLBACK (search_term_changed_cb),
							    box, G_CONNECT_DEFAULT);
}

GtkWidget *
pps_search_box_new ()
{
        return GTK_WIDGET (g_object_new (PPS_TYPE_SEARCH_BOX, NULL));
}

/**
 * pps_search_box_get_entry:
 * @box: The instance of the #PpsSearchBox.
 *
 * Returns: (not nullable) (transfer none): The internal #GtkSearchEntry.
 */
GtkSearchEntry *
pps_search_box_get_entry (PpsSearchBox *box)
{
        PpsSearchBoxPrivate *priv;

        g_return_val_if_fail (PPS_IS_SEARCH_BOX (box), NULL);

	priv = GET_PRIVATE (box);

        return GTK_SEARCH_ENTRY (priv->entry);
}

void
pps_search_box_set_search_context (PpsSearchBox     *box,
				   PpsSearchContext *context)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        g_return_if_fail (PPS_IS_SEARCH_BOX (box));
	g_return_if_fail (PPS_IS_SEARCH_CONTEXT (context));

	if (priv->context != NULL) {
		g_signal_handlers_disconnect_by_func (priv->context, search_changed_cb, box);
		g_signal_handlers_disconnect_by_func (priv->context, find_job_finished_cb, box);
	}

	priv->context = g_object_ref (context);

	g_signal_connect_object (priv->context, "started",
				 G_CALLBACK (search_changed_cb),
				 box, G_CONNECT_SWAPPED);
	g_signal_connect_object (priv->context, "cleared",
				 G_CALLBACK (search_changed_cb),
				 box, G_CONNECT_SWAPPED);
	g_signal_connect_object (priv->context, "notify::search-term",
				 G_CALLBACK (search_changed_cb),
				 box, G_CONNECT_SWAPPED);

	g_signal_connect_object (priv->context, "finished",
				 G_CALLBACK (find_job_finished_cb),
				 box, G_CONNECT_DEFAULT);
}


static GtkBuildableIface *parent_buildable_iface;

static GObject *
pps_search_box_buildable_get_internal_child (GtkBuildable *buildable,
                             GtkBuilder   *builder,
                             const char   *childname)
{
        PpsSearchBox *box = PPS_SEARCH_BOX (buildable);

        if (strcmp (childname, "entry") == 0)
                return G_OBJECT (pps_search_box_get_entry (box));

        return parent_buildable_iface->get_internal_child (buildable, builder, childname);
}

static void
pps_search_box_buildable_iface_init (GtkBuildableIface *iface)
{
        parent_buildable_iface = g_type_interface_peek_parent (iface);

        iface->get_internal_child = pps_search_box_buildable_get_internal_child;
}
