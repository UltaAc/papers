/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2018 Germán Poo-Caamaño <gpoo@gnome.org>
 *  Copyright (C) 2008 Carlos Garcia Campos <carlosgc@gnome.org>
 *  Copyright (C) 2005 Red Hat, Inc
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
#include <gtk/gtk.h>
#include <gio/gio.h>

#include "pps-keyring.h"
#include "pps-password-view.h"

/* Define a maximum width in case there is a file with a very long name */
#define MAX_WIDHT_LABEL 64
/* Define a maximum width for password entry */
#define MAX_WIDHT_PASSWORD_ENTRY 32

enum {
	UNLOCK,
	CANCELLED,
	LAST_SIGNAL
};

enum {
	PROP_FILENAME = 1,
	PROP_ASK_PASSWORD,
	N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

typedef struct {
	GtkWidget    *password_entry;

	gchar        *password;
	GPasswordSave password_save;

	char         *filename;
} PpsPasswordViewPrivate;

static guint password_view_signals [LAST_SIGNAL] = { 0 };

#define GET_PRIVATE(o) pps_password_view_get_instance_private (o)

G_DEFINE_TYPE_WITH_PRIVATE (PpsPasswordView, pps_password_view, ADW_TYPE_BIN)

static void pps_password_view_clicked_cb (GtkWidget      *button,
					 PpsPasswordView *password_view);
static void
pps_password_view_set_property (GObject      *object,
					                      guint         prop_id,
					                      const GValue *value,
					                      GParamSpec   *pspec);
static void
pps_password_view_set_filename (PpsPasswordView *password_view,
                                const char     *filename);
static void pps_password_view_ask_password  (PpsPasswordView *password_view, gboolean error);

static void
pps_password_view_finalize (GObject *object)
{
	PpsPasswordView *password_view = PPS_PASSWORD_VIEW (object);
	PpsPasswordViewPrivate *priv = GET_PRIVATE (password_view);

	g_clear_pointer (&priv->password, g_free);
	g_clear_pointer (&priv->filename, g_free);

	G_OBJECT_CLASS (pps_password_view_parent_class)->finalize (object);
}

static void
pps_password_view_class_init (PpsPasswordViewClass *class)
{
	GObjectClass *g_object_class;
	GtkWidgetClass *widget_class;

	g_object_class = G_OBJECT_CLASS (class);
	widget_class = GTK_WIDGET_CLASS (class);

	g_object_class->set_property = pps_password_view_set_property;

	obj_properties[PROP_FILENAME] =
		g_param_spec_string ("filename",
		                     "Filename",
		                     "Name of the document to unlock.",
		                     NULL,
		                     G_PARAM_WRITABLE);

	obj_properties[PROP_ASK_PASSWORD] =
		g_param_spec_boolean ("ask-password",
		                      "Ask Password",
		                      "Triggers a password prompt, optionally erroneous if set",
		                      FALSE,
		                      G_PARAM_WRITABLE);

	g_object_class_install_properties (g_object_class, N_PROPERTIES, obj_properties);

	password_view_signals[UNLOCK] =
		g_signal_new ("unlock",
			      G_TYPE_FROM_CLASS (g_object_class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      0,
			      NULL, NULL,
			      NULL,
			      G_TYPE_NONE,
			      2, G_TYPE_STRING, G_TYPE_PASSWORD_SAVE);

	password_view_signals[CANCELLED] =
		g_signal_new ("cancelled",
			      G_TYPE_FROM_CLASS (g_object_class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      0,
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);

	gtk_widget_class_set_template_from_resource (widget_class,
				"/org/gnome/papers/ui/password-view.ui");
	gtk_widget_class_bind_template_callback (widget_class,
						 pps_password_view_clicked_cb);

	g_object_class->finalize = pps_password_view_finalize;
}

static void
pps_password_view_set_property (GObject      *object,
					                      guint         prop_id,
					                      const GValue *value,
					                      GParamSpec   *pspec)
{
	PpsPasswordView *view = PPS_PASSWORD_VIEW (object);

	switch (prop_id) {
		case PROP_FILENAME:
			pps_password_view_set_filename (view, g_value_get_string(value));
			break;

		case PROP_ASK_PASSWORD:
			pps_password_view_ask_password(view, g_value_get_boolean(value));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_password_view_clicked_cb (GtkWidget      *button,
			     PpsPasswordView *password_view)
{
	pps_password_view_ask_password (password_view, FALSE);
}

static void
pps_password_view_init (PpsPasswordView *password_view)
{
	PpsPasswordViewPrivate *priv = GET_PRIVATE (password_view);

	priv->password_save = G_PASSWORD_SAVE_NEVER;

	gtk_widget_init_template (GTK_WIDGET (password_view));
}

static void
pps_password_view_set_filename (PpsPasswordView *password_view,
                                const char     *filename)
{
	PpsPasswordViewPrivate *priv = GET_PRIVATE (password_view);

	g_return_if_fail (PPS_IS_PASSWORD_VIEW (password_view));
	g_return_if_fail (filename != NULL);

	if (g_strcmp0 (priv->filename, filename) == 0)
		return;

	g_free (priv->filename);
	priv->filename = g_strdup (filename);
}

static void
pps_password_dialog_got_response (AdwMessageDialog *dialog,
                                 GAsyncResult     *result,
                                 PpsPasswordView   *password_view)
{
	PpsPasswordViewPrivate *priv = GET_PRIVATE (password_view);
	const char *response = adw_message_dialog_choose_finish (dialog, result);

	gtk_widget_set_sensitive (GTK_WIDGET (password_view), TRUE);

	if (g_strcmp0 (response, "unlock") == 0) {
		priv->password =
			g_strdup (gtk_editable_get_text (GTK_EDITABLE (priv->password_entry)));

		g_signal_emit (password_view, password_view_signals[UNLOCK], 0, g_strdup (priv->password), priv->password_save);
	} else {
		g_signal_emit (password_view, password_view_signals[CANCELLED], 0);
	}
}

static void
pps_password_dialog_remember_button_toggled (GtkCheckButton *button,
					    PpsPasswordView  *password_view)
{
	PpsPasswordViewPrivate *priv = GET_PRIVATE (password_view);

	if (gtk_check_button_get_active (button)) {
		gpointer data;

		data = g_object_get_data (G_OBJECT (button), "password-save");
		priv->password_save = GPOINTER_TO_INT (data);
	}
}

static void
pps_password_dialog_entry_changed_cb (GtkEditable      *editable,
                                     AdwMessageDialog *dialog)
{
	const char *text;

	text = gtk_editable_get_text (GTK_EDITABLE (editable));

	adw_message_dialog_set_response_enabled (dialog, "unlock",
						 (text != NULL && *text != '\0'));
}

static void
pps_password_view_ask_password (PpsPasswordView *password_view, gboolean error)
{
	AdwMessageDialog *dialog;
	GtkWidget *message_area;
	GtkWidget *password_entry;
	gchar     *text;
	PpsPasswordViewPrivate *priv = GET_PRIVATE (password_view);
	GtkWindow *parent_window;

        text = g_markup_printf_escaped (_("The document “%s” is locked and requires a password before it can be opened"),
                                        priv->filename);

	parent_window = GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (password_view)));

	dialog = ADW_MESSAGE_DIALOG (adw_message_dialog_new (parent_window,
							     _("Password Required"),
							     NULL));
	adw_message_dialog_set_body_use_markup (dialog, true);
	adw_message_dialog_set_body (dialog, text);
	g_free (text);

	adw_message_dialog_add_responses (dialog,
					  "cancel", _("_Cancel"),
					  "unlock", _("_Unlock"),
					  NULL);
	adw_message_dialog_set_close_response (dialog, "cancel");
	adw_message_dialog_set_default_response (dialog, "unlock");
	adw_message_dialog_set_response_enabled (dialog, "unlock", FALSE);

	message_area = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
	adw_message_dialog_set_extra_child (dialog, message_area);

	password_entry = gtk_password_entry_new ();
	gtk_password_entry_set_show_peek_icon (GTK_PASSWORD_ENTRY (password_entry), TRUE);
	g_object_set (G_OBJECT (password_entry),
		      "placeholder-text", _("Password"),
		      NULL);
	g_object_set (G_OBJECT (password_entry), "activates-default", TRUE, NULL);
	g_object_set (G_OBJECT (password_entry), "width-chars", 32, NULL);
	g_signal_connect (password_entry, "changed",
			  G_CALLBACK (pps_password_dialog_entry_changed_cb),
			  dialog);

	gtk_box_append (GTK_BOX (message_area), password_entry);

	if (error) {
		GtkWidget* error_message = gtk_label_new (_("Invalid password"));

		gtk_box_append (GTK_BOX (message_area), error_message);

		gtk_widget_add_css_class (error_message, "error");
		gtk_widget_add_css_class (password_entry, "error");
	}

	priv->password_entry = password_entry;

	if (pps_keyring_is_available ()) {
		GtkWidget  *choice;
		GtkWidget  *remember_box;
		GtkWidget  *group;

		remember_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
		gtk_box_append (GTK_BOX (message_area), remember_box);
		gtk_widget_set_halign (remember_box, GTK_ALIGN_CENTER);

		choice = gtk_check_button_new_with_mnemonic (_("Forget password _immediately"));
		gtk_check_button_set_active (GTK_CHECK_BUTTON (choice),
					      priv->password_save == G_PASSWORD_SAVE_NEVER);
		g_object_set_data (G_OBJECT (choice), "password-save",
				   GINT_TO_POINTER (G_PASSWORD_SAVE_NEVER));
		g_signal_connect (choice, "toggled",
				  G_CALLBACK (pps_password_dialog_remember_button_toggled),
				  password_view);
		gtk_box_append (GTK_BOX (remember_box), choice);

		group = choice;
		choice = gtk_check_button_new_with_mnemonic (_("Remember password until you _log out"));
		gtk_check_button_set_group (GTK_CHECK_BUTTON (choice), GTK_CHECK_BUTTON (group));
		gtk_check_button_set_active (GTK_CHECK_BUTTON (choice),
					      priv->password_save == G_PASSWORD_SAVE_FOR_SESSION);
		g_object_set_data (G_OBJECT (choice), "password-save",
				   GINT_TO_POINTER (G_PASSWORD_SAVE_FOR_SESSION));
		g_signal_connect (choice, "toggled",
				  G_CALLBACK (pps_password_dialog_remember_button_toggled),
				  password_view);
		gtk_box_append (GTK_BOX (remember_box), choice);

		group = choice;
		choice = gtk_check_button_new_with_mnemonic (_("Remember _forever"));
		gtk_check_button_set_group (GTK_CHECK_BUTTON (choice), GTK_CHECK_BUTTON (group));
		gtk_check_button_set_active (GTK_CHECK_BUTTON (choice),
					      priv->password_save == G_PASSWORD_SAVE_PERMANENTLY);
		g_object_set_data (G_OBJECT (choice), "password-save",
				   GINT_TO_POINTER (G_PASSWORD_SAVE_PERMANENTLY));
		g_signal_connect (choice, "toggled",
				  G_CALLBACK (pps_password_dialog_remember_button_toggled),
				  password_view);
		gtk_box_append (GTK_BOX (remember_box), choice);
	}

	adw_message_dialog_choose (dialog, NULL,
				   (GAsyncReadyCallback) pps_password_dialog_got_response,
				   password_view);
}

PpsPasswordView *
pps_password_view_new (void)
{
	return g_object_new (PPS_TYPE_PASSWORD_VIEW, NULL);
}
