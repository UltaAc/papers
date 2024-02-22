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

enum {
        STARTED,
        FINISHED,
        CLEARED,

        NEXT,
        PREVIOUS,

        LAST_SIGNAL
};

enum
{
        PROP_0,

        PROP_DOCUMENT_MODEL,
        PROP_OPTIONS
};

typedef struct {
        PpsDocumentModel *model;
        PpsJob           *job;
        PpsFindOptions    options;
        PpsFindOptions    supported_options;

        GtkWidget       *entry;
        GtkWidget       *next_button;
        GtkWidget       *prpps_button;

        guint            pages_searched;
} PpsSearchBoxPrivate;

static void pps_search_box_buildable_iface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_WITH_CODE (PpsSearchBox, pps_search_box, ADW_TYPE_BIN,
                         G_ADD_PRIVATE (PpsSearchBox)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                                                pps_search_box_buildable_iface_init))

#define GET_PRIVATE(o) pps_search_box_get_instance_private(o)

static guint signals[LAST_SIGNAL] = { 0 };

#define FIND_PAGE_RATE_REFRESH 100

static void
pps_search_box_clear_job (PpsSearchBox *box)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        if (!priv->job)
                return;

        if (!pps_job_is_finished (priv->job))
                pps_job_cancel (priv->job);

        g_signal_handlers_disconnect_matched (priv->job, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, box);
	g_clear_object (&priv->job);
}

static void
find_job_finished_cb (PpsJobFind   *job,
                      PpsSearchBox *box)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);
	gboolean has_results;

        g_signal_emit (box, signals[FINISHED], 0);
        pps_search_box_clear_job (box);

	has_results = pps_job_find_has_results (job);

        gtk_widget_set_sensitive (priv->next_button, has_results);
        gtk_widget_set_sensitive (priv->prpps_button, has_results);

        if (!has_results) {
                gtk_widget_add_css_class (priv->entry, "error");
        }
}

static void
search_changed_cb (GtkSearchEntry *entry,
                   PpsSearchBox    *box)
{
        const char *search_string;
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        pps_search_box_clear_job (box);
        priv->pages_searched = 0;

        gtk_widget_set_sensitive (priv->next_button, FALSE);
        gtk_widget_set_sensitive (priv->prpps_button, FALSE);

	gtk_widget_remove_css_class(priv->entry, "error");

        search_string = gtk_editable_get_text (GTK_EDITABLE (entry));

        if (search_string && search_string[0]) {
                PpsDocument *doc = pps_document_model_get_document (priv->model);

                priv->job = pps_job_find_new (doc,
                                             pps_document_model_get_page (priv->model),
                                             pps_document_get_n_pages (doc),
                                             search_string,
					     priv->options);
                g_signal_connect (priv->job, "finished",
                                  G_CALLBACK (find_job_finished_cb),
                                  box);

                g_signal_emit (box, signals[STARTED], 0, priv->job);
                pps_job_scheduler_push_job (priv->job, PPS_JOB_PRIORITY_NONE);
        } else {
                g_signal_emit (box, signals[CLEARED], 0);
        }
}

static void
previous_clicked_cb (GtkButton   *button,
                     PpsSearchBox *box)
{
        g_signal_emit (box, signals[PREVIOUS], 0);
}

static void
next_clicked_cb (GtkButton   *button,
                 PpsSearchBox *box)
{
        g_signal_emit (box, signals[NEXT], 0);
}

static void
pps_search_box_set_supported_options (PpsSearchBox  *box,
                                     PpsFindOptions options)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        priv->supported_options = options;
}

static void
pps_search_box_setup_document (PpsSearchBox *box,
                              PpsDocument  *document)
{
        if (!document || !PPS_IS_DOCUMENT_FIND (document)) {
                pps_search_box_set_supported_options (box, PPS_FIND_DEFAULT);
                gtk_widget_set_sensitive (GTK_WIDGET (box), FALSE);
                return;
        }

        pps_search_box_set_supported_options (box, pps_document_find_get_supported_options (PPS_DOCUMENT_FIND (document)));
        gtk_widget_set_sensitive (GTK_WIDGET (box), pps_document_get_n_pages (document) > 0);
}

static void
document_changed_cb (PpsDocumentModel *model,
                     GParamSpec      *pspec,
                     PpsSearchBox     *box)
{
        pps_search_box_setup_document (box, pps_document_model_get_document (model));
}

static void
pps_search_box_set_options (PpsSearchBox  *box,
                           PpsFindOptions options)
{
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        if (priv->options == options)
                return;

        priv->options = options;
        search_changed_cb (GTK_SEARCH_ENTRY (priv->entry), box);
}


static void
whole_words_only_toggled_cb (GSimpleAction *action,
			     GVariant      *state,
			     gpointer       user_data)
{
	PpsSearchBox *box = PPS_SEARCH_BOX (user_data);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);
        PpsFindOptions options = priv->options;
	gboolean active = g_variant_get_boolean (state);
	g_simple_action_set_state (action, state);

        if (active)
                options |= PPS_FIND_WHOLE_WORDS_ONLY;
        else
                options &= ~PPS_FIND_WHOLE_WORDS_ONLY;
        pps_search_box_set_options (box, options);
}

static void
case_sensitive_toggled_cb (GSimpleAction *action,
			   GVariant      *state,
			   gpointer       user_data)
{
	PpsSearchBox *box = PPS_SEARCH_BOX (user_data);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);
        PpsFindOptions options = priv->options;
	gboolean active = g_variant_get_boolean (state);
	g_simple_action_set_state (action, state);

        if (active)
                options |= PPS_FIND_CASE_SENSITIVE;
        else
                options &= ~PPS_FIND_CASE_SENSITIVE;
        pps_search_box_set_options (box, options);
}

static void
entry_activate_cb (GtkEntry    *entry,
                   PpsSearchBox *box)
{
        g_signal_emit (box, signals[NEXT], 0);
}

static void
entry_next_match_cb (GtkSearchEntry *entry,
                     PpsSearchBox *box)
{
        g_signal_emit (box, signals[NEXT], 0);
}

static void
entry_previous_match_cb (GtkSearchEntry *entry,
                         PpsSearchBox *box)
{
        g_signal_emit (box, signals[PREVIOUS], 0);
}

static void
pps_search_box_finalize (GObject *object)
{
        PpsSearchBox *box = PPS_SEARCH_BOX (object);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        if (priv->model) {
                g_object_remove_weak_pointer (G_OBJECT (priv->model),
                                              (gpointer)&priv->model);
        }

        G_OBJECT_CLASS (pps_search_box_parent_class)->finalize (object);
}

static void
pps_search_box_dispose (GObject *object)
{
        PpsSearchBox *box = PPS_SEARCH_BOX (object);

        pps_search_box_clear_job (box);

        G_OBJECT_CLASS (pps_search_box_parent_class)->dispose (object);
}

static void
pps_search_box_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
        PpsSearchBox *box = PPS_SEARCH_BOX (object);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        switch (prop_id) {
        case PROP_DOCUMENT_MODEL:
                priv->model = g_value_get_object (value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
}

static void
pps_search_box_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
        PpsSearchBox *box = PPS_SEARCH_BOX (object);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        switch (prop_id) {
        case PROP_OPTIONS:
                g_value_set_flags (value, priv->options);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
}

static void
pps_search_box_constructed (GObject *object)
{
        PpsSearchBox *box = PPS_SEARCH_BOX (object);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        G_OBJECT_CLASS (pps_search_box_parent_class)->constructed (object);

        g_object_add_weak_pointer (G_OBJECT (priv->model),
                                   (gpointer)&priv->model);

        pps_search_box_setup_document (box, pps_document_model_get_document (priv->model));
        g_signal_connect_object (priv->model, "notify::document",
                                 G_CALLBACK (document_changed_cb),
                                 box, 0);
}

static gboolean
pps_search_box_grab_focus (GtkWidget *widget)
{
        PpsSearchBox *box = PPS_SEARCH_BOX (widget);
        PpsSearchBoxPrivate *priv = GET_PRIVATE (box);

        return gtk_widget_grab_focus (priv->entry);
}

static void
pps_search_box_class_init (PpsSearchBoxClass *klass)
{
        GObjectClass   *object_class = G_OBJECT_CLASS (klass);
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

        object_class->finalize = pps_search_box_finalize;
        object_class->dispose = pps_search_box_dispose;
        object_class->constructed = pps_search_box_constructed;
        object_class->set_property = pps_search_box_set_property;
        object_class->get_property = pps_search_box_get_property;

        widget_class->grab_focus = pps_search_box_grab_focus;

	gtk_widget_class_set_template_from_resource (widget_class,
		"/org/gnome/papers/ui/search-box.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsSearchBox, entry);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSearchBox, prpps_button);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSearchBox, next_button);

	gtk_widget_class_bind_template_callback (widget_class, case_sensitive_toggled_cb);
	gtk_widget_class_bind_template_callback (widget_class, whole_words_only_toggled_cb);

	gtk_widget_class_bind_template_callback (widget_class, search_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, entry_activate_cb);
	gtk_widget_class_bind_template_callback (widget_class, entry_next_match_cb);
	gtk_widget_class_bind_template_callback (widget_class, entry_previous_match_cb);
	gtk_widget_class_bind_template_callback (widget_class, previous_clicked_cb);
	gtk_widget_class_bind_template_callback (widget_class, next_clicked_cb);

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
                                         PROP_OPTIONS,
                                         g_param_spec_flags ("options",
                                                             "Search options",
                                                             "The search options",
                                                             PPS_TYPE_FIND_OPTIONS,
                                                             PPS_FIND_DEFAULT,
                                                             G_PARAM_READABLE |
                                                             G_PARAM_STATIC_STRINGS));

        signals[STARTED] =
                g_signal_new ("started",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_LAST,
                              0, NULL, NULL,
                              g_cclosure_marshal_VOID__OBJECT,
                              G_TYPE_NONE, 1,
                              PPS_TYPE_JOB_FIND);
        signals[FINISHED] =
                g_signal_new ("finished",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_LAST,
                              0, NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);
        signals[CLEARED] =
                g_signal_new ("cleared",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_LAST,
                              0, NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);
        signals[NEXT] =
                g_signal_new ("next",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                              0, NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);
        signals[PREVIOUS] =
                g_signal_new ("previous",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                              0, NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);


        gtk_widget_class_add_binding_signal (widget_class, GDK_KEY_Return, GDK_SHIFT_MASK,
                                      "previous", NULL);
        gtk_widget_class_add_binding_signal (widget_class, GDK_KEY_ISO_Enter, GDK_SHIFT_MASK,
                                      "previous", NULL);
        gtk_widget_class_add_binding_signal (widget_class, GDK_KEY_KP_Enter, GDK_SHIFT_MASK,
                                      "previous", NULL);
        gtk_widget_class_add_binding_signal (widget_class, GDK_KEY_Up, GDK_CONTROL_MASK,
                                      "previous", NULL);
        gtk_widget_class_add_binding_signal (widget_class, GDK_KEY_Down, GDK_CONTROL_MASK,
                                      "next", NULL);
}

const GActionEntry actions[] = {
	{ "whole-words-only", NULL, NULL, "false", whole_words_only_toggled_cb },
	{ "case-sensitive", NULL, NULL, "false", case_sensitive_toggled_cb },
};

static void
pps_search_box_init (PpsSearchBox *box)
{
	GSimpleActionGroup *group;

	gtk_widget_init_template (GTK_WIDGET (box));

	group = g_simple_action_group_new ();
	g_action_map_add_action_entries (G_ACTION_MAP (group), actions,
					 G_N_ELEMENTS (actions), box);

	gtk_widget_insert_action_group (GTK_WIDGET (box), "search", G_ACTION_GROUP (group));
}

GtkWidget *
pps_search_box_new (PpsDocumentModel *model)
{
        g_return_val_if_fail (PPS_IS_DOCUMENT_MODEL (model), NULL);

        return GTK_WIDGET (g_object_new (PPS_TYPE_SEARCH_BOX,
                                         "document-model", model,
                                         NULL));
}

GtkSearchEntry *
pps_search_box_get_entry (PpsSearchBox *box)
{
        PpsSearchBoxPrivate *priv;

        g_return_val_if_fail (PPS_IS_SEARCH_BOX (box), NULL);

	priv = GET_PRIVATE (box);

        return GTK_SEARCH_ENTRY (priv->entry);
}

gboolean
pps_search_box_has_results (PpsSearchBox *box)
{
        PpsSearchBoxPrivate *priv;

        g_return_val_if_fail (PPS_IS_SEARCH_BOX (box), FALSE);

	priv = GET_PRIVATE (box);

        return gtk_widget_get_sensitive (priv->next_button);
}

void
pps_search_box_restart (PpsSearchBox *box)
{
        PpsSearchBoxPrivate *priv;

        g_return_if_fail (PPS_IS_SEARCH_BOX (box));

	priv = GET_PRIVATE (box);

        search_changed_cb (GTK_SEARCH_ENTRY (priv->entry), box);
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
