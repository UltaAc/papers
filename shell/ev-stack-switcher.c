/* ev-stack-switcher.c
 *
 * Copyright 2024 Christopher Davis <christopherdavis@gnome.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "ev-stack-switcher.h"

struct _EvStackSwitcher
{
	AdwBin parent_instance;

	GtkBox *box;

	GtkStack *stack;
	GtkSelectionModel *pages;
	GHashTable *buttons;
};

G_DEFINE_FINAL_TYPE (EvStackSwitcher, ev_stack_switcher, ADW_TYPE_BIN)

enum {
	PROP_0,
	PROP_STACK,
	N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
button_active_notify_cb (EvStackSwitcher *self,
                         GParamSpec      *pspec,
                         GtkToggleButton *button)
{
	gboolean active = gtk_toggle_button_get_active (button);
	guint index = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (button), "child-index"));
	gboolean selected = active;

	if (active) {
		gtk_selection_model_select_item (self->pages, index, TRUE);
	} else {
		selected = gtk_selection_model_is_selected (self->pages, index);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), selected);
	}

	gtk_accessible_update_state (GTK_ACCESSIBLE (button),
				     GTK_ACCESSIBLE_STATE_SELECTED, selected,
				     -1);
}

static void
add_child (EvStackSwitcher *self,
           guint            position)
{
	GtkWidget *button;
        GtkStackPage *page;
	g_autofree char *title, *icon_name;
	gboolean visible, selected;

	button = g_object_new (GTK_TYPE_TOGGLE_BUTTON,
			       "accessible-role", GTK_ACCESSIBLE_ROLE_TAB,
			       "hexpand", TRUE,
			       "vexpand", TRUE,
			       "focus-on-click", FALSE,
			       NULL);

	gtk_box_append (self->box, button);

	page = g_list_model_get_item (G_LIST_MODEL (self->pages), position);
	selected = gtk_selection_model_is_selected (self->pages, position);

	gtk_accessible_update_state (GTK_ACCESSIBLE (button),
				     GTK_ACCESSIBLE_STATE_SELECTED, selected,
				     -1);
	gtk_accessible_update_relation (GTK_ACCESSIBLE (button),
					GTK_ACCESSIBLE_RELATION_CONTROLS, page, NULL,
					-1);
	g_object_get (page,
		      "title", &title,
		      "icon-name", &icon_name,
		      "visible", &visible,
		      NULL);

	g_object_set_data (G_OBJECT (button), "child-index", GUINT_TO_POINTER (position));

	g_object_bind_property (page, "icon-name",
				button, "icon-name",
				G_BINDING_SYNC_CREATE);
	g_object_bind_property (page, "title",
				button, "tooltip-text",
				G_BINDING_SYNC_CREATE);
	g_object_bind_property (page, "visible",
				button, "visible",
				G_BINDING_SYNC_CREATE);

	gtk_accessible_update_property (GTK_ACCESSIBLE (button),
					GTK_ACCESSIBLE_PROPERTY_LABEL, title,
					-1);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), selected);

	gtk_widget_set_visible (button, visible);

	g_hash_table_insert (self->buttons, g_object_ref (page), button);

	g_signal_connect_swapped (button, "notify::active",
				  G_CALLBACK (button_active_notify_cb), self);

	g_object_unref (page);
}

static void
populate_switcher (EvStackSwitcher *self)
{
  for (guint i = 0; i < g_list_model_get_n_items (G_LIST_MODEL (self->pages)); i++)
    add_child (self, i);
}

static void
clear_switcher (EvStackSwitcher *self)
{
  GHashTableIter iter;
  GtkWidget *page;
  GtkWidget *button;

  g_hash_table_iter_init (&iter, self->buttons);
  while (g_hash_table_iter_next (&iter, (gpointer *)&page, (gpointer *)&button))
    {
      gtk_widget_unparent (button);
      g_hash_table_iter_remove (&iter);
    }
}

static void
items_changed_cb (GListModel      *model,
                  guint            position,
                  guint            removed,
                  guint            added,
                  EvStackSwitcher *self)
{
  clear_switcher (self);
  populate_switcher (self);
}

static void
selection_changed_cb (GtkSelectionModel *model,
                      guint              position,
                      guint              n_items,
                      EvStackSwitcher   *self)
{
  guint i;

  for (i = position; i < position + n_items; i++)
    {
      GtkStackPage *page;
      GtkWidget *button;
      gboolean selected;

      page = g_list_model_get_item (G_LIST_MODEL (self->pages), i);
      button = g_hash_table_lookup (self->buttons, page);
      if (button)
        {
          selected = gtk_selection_model_is_selected (self->pages, i);
          gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), selected);

          gtk_accessible_update_state (GTK_ACCESSIBLE (button),
                                       GTK_ACCESSIBLE_STATE_SELECTED, selected,
                                       -1);
        }
      g_object_unref (page);
    }
}

static void
disconnect_stack_signals (EvStackSwitcher *self)
{
  g_signal_handlers_disconnect_by_func (self->pages, items_changed_cb, self);
  g_signal_handlers_disconnect_by_func (self->pages, selection_changed_cb, self);
}

static void
connect_stack_signals (EvStackSwitcher *self)
{
  g_signal_connect (self->pages, "items-changed", G_CALLBACK (items_changed_cb), self);
  g_signal_connect (self->pages, "selection-changed", G_CALLBACK (selection_changed_cb), self);
}


EvStackSwitcher *
ev_stack_switcher_new (void)
{
	return g_object_new (EV_TYPE_STACK_SWITCHER, NULL);
}

static void
ev_stack_switcher_finalize (GObject *object)
{
	G_OBJECT_CLASS (ev_stack_switcher_parent_class)->finalize (object);
}

static void
ev_stack_switcher_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
	EvStackSwitcher *self = EV_STACK_SWITCHER (object);

	switch (prop_id)
	  {
	  case PROP_STACK:
		g_value_set_object (value, self->stack);
		break;
	  default:
	    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	  }
}

static void
ev_stack_switcher_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
	EvStackSwitcher *self = EV_STACK_SWITCHER (object);

	switch (prop_id)
	  {
	  case PROP_STACK:
		ev_stack_switcher_set_stack (self, g_value_get_object (value));
		break;
	  default:
	    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	  }
}

static void
ev_stack_switcher_class_init (EvStackSwitcherClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->finalize = ev_stack_switcher_finalize;
	object_class->get_property = ev_stack_switcher_get_property;
	object_class->set_property = ev_stack_switcher_set_property;

	properties [PROP_STACK] =
		g_param_spec_object ("stack", NULL, NULL,
		                     GTK_TYPE_STACK,
		                     (G_PARAM_READWRITE |
		                      G_PARAM_EXPLICIT_NOTIFY |
		                      G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (object_class, PROP_STACK, properties [PROP_STACK]);

	gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_TAB_LIST);
}

static void
ev_stack_switcher_init (EvStackSwitcher *self)
{
	self->buttons = g_hash_table_new_full (g_direct_hash, g_direct_equal,
					       g_object_unref, NULL);
	self->box = GTK_BOX (gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0));

	gtk_widget_add_css_class (GTK_WIDGET (self->box), "toolbar");

	adw_bin_set_child (ADW_BIN (self), GTK_WIDGET (self->box));
}

void
ev_stack_switcher_set_stack (EvStackSwitcher *self,
                             GtkStack        *stack)
{
	g_return_if_fail (EV_IS_STACK_SWITCHER (self));
	g_return_if_fail (GTK_IS_STACK (stack));

	if (self->stack == stack) {
		return;
	} else if (self->stack) {
		disconnect_stack_signals (self);
		clear_switcher (self);
	}

	g_set_object (&self->stack, stack);
	g_set_object (&self->pages, gtk_stack_get_pages (stack));

	populate_switcher (self);
	connect_stack_signals (self);
}
