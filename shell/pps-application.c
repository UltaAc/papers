/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2004 Martin Kretzschmar
 *  Copyright © 2010, 2012 Christian Persch
 *
 *  Author:
 *    Martin Kretzschmar <martink@gnome.org>
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


#include <config.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <adwaita.h>
#include <unistd.h>

#include "pps-application.h"
#include "pps-window.h"
#include "pps-init.h"
#include "pps-job-scheduler.h"

#ifdef ENABLE_DBUS
#include "pps-gdbus-generated.h"
#endif /* ENABLE_DBUS */

struct _PpsApplication {
	AdwApplication base_instance;

#ifdef ENABLE_DBUS
        PpsPapersApplication *skeleton;
#endif
};

struct _PpsApplicationClass {
	AdwApplicationClass base_class;
};

G_DEFINE_TYPE (PpsApplication, pps_application, ADW_TYPE_APPLICATION)

#ifdef ENABLE_DBUS
#define APPLICATION_DBUS_OBJECT_PATH "/org/gnome/Papers/Papers" OBJECT_PROFILE
#define APPLICATION_DBUS_INTERFACE   "org.gnome.Papers.Application"
#endif

static void pps_application_open_uri_in_window (PpsApplication  *application,
					       const char     *uri,
					       PpsWindow       *pps_window,
					       PpsLinkDest     *dest,
					       PpsWindowRunMode mode);

/**
 * pps_application_new:
 *
 * Creates a new #PpsApplication instance.
 *
 * Returns: (transfer full): a newly created #PpsApplication
 */
PpsApplication *
pps_application_new (void)
{
	const GApplicationFlags flags = G_APPLICATION_NON_UNIQUE | G_APPLICATION_HANDLES_COMMAND_LINE;

	return g_object_new (PPS_TYPE_APPLICATION,
			     "application-id", APP_ID,
			     "flags", flags,
			     "resource-base-path", "/org/gnome/papers",
			     NULL);
}

static void
pps_spawn (const char     *uri,
	  PpsLinkDest     *dest,
	  PpsWindowRunMode mode)
{
	GString *cmd;
	gchar *path, *cmdline;
	GAppInfo *app;
	GError  *error = NULL;

	cmd = g_string_new (NULL);

	path = g_find_program_in_path ("papers");

	g_string_append_printf (cmd, " %s", path);
	g_free (path);

	/* Page label */
	if (dest) {
                switch (pps_link_dest_get_dest_type (dest)) {
                case PPS_LINK_DEST_TYPE_PAGE_LABEL:
                        g_string_append_printf (cmd, " --page-label=%s",
                                                pps_link_dest_get_page_label (dest));
                        break;
                case PPS_LINK_DEST_TYPE_PAGE:
                case PPS_LINK_DEST_TYPE_XYZ:
                case PPS_LINK_DEST_TYPE_FIT:
                case PPS_LINK_DEST_TYPE_FITH:
                case PPS_LINK_DEST_TYPE_FITV:
                case PPS_LINK_DEST_TYPE_FITR:
                        g_string_append_printf (cmd, " --page-index=%d",
                                                pps_link_dest_get_page (dest) + 1);
                        break;
                case PPS_LINK_DEST_TYPE_NAMED:
                        g_string_append_printf (cmd, " --named-dest=%s",
                                                pps_link_dest_get_named_dest (dest));
                        break;
                default:
                        break;
                }
	}

	/* Mode */
	switch (mode) {
	case PPS_WINDOW_MODE_FULLSCREEN:
		g_string_append (cmd, " -f");
		break;
	case PPS_WINDOW_MODE_PRESENTATION:
		g_string_append (cmd, " -s");
		break;
	default:
		break;
	}

	cmdline = g_string_free (cmd, FALSE);
	app = g_app_info_create_from_commandline (cmdline, NULL, G_APP_INFO_CREATE_SUPPORTS_URIS, &error);

	if (app != NULL) {
                GList uri_list;
                GList *uris = NULL;
		GdkAppLaunchContext *ctx;

		ctx = gdk_display_get_app_launch_context (gdk_display_get_default ());

                /* Some URIs can be changed when passed through a GFile
                 * (for instance unsupported uris with strange formats like mailto:),
                 * so if you have a textual uri you want to pass in as argument,
                 * consider using g_app_info_launch_uris() instead.
                 * See https://bugzilla.gnome.org/show_bug.cgi?id=644604
                 */
                if (uri) {
                        uri_list.data = (gchar *)uri;
                        uri_list.prev = uri_list.next = NULL;
                        uris = &uri_list;
                }
		g_app_info_launch_uris (app, uris, G_APP_LAUNCH_CONTEXT (ctx), &error);

		g_object_unref (app);
		g_object_unref (ctx);
	}

	if (error != NULL) {
		g_printerr ("Error launching papers %s: %s\n", uri, error->message);
		g_error_free (error);
	}

	g_free (cmdline);
}

static void
pps_application_open_uri_in_window (PpsApplication  *application,
				   const char     *uri,
				   PpsWindow       *pps_window,
				   PpsLinkDest     *dest,
				   PpsWindowRunMode mode)
{
	/* We need to load uri before showing the window, so
	   we can restore window size without flickering */
	pps_window_open_uri (pps_window, uri, dest, mode);

	if (!gtk_widget_get_realized (GTK_WIDGET (pps_window)))
		gtk_widget_realize (GTK_WIDGET (pps_window));

	gtk_window_present (GTK_WINDOW (pps_window));
}

/**
 * pps_application_open_uri_at_dest:
 * @application: The instance of the application.
 * @uri: The uri to be opened.
 * @dest: The #PpsLinkDest of the document.
 * @mode: The run mode of the window.
 */
void
pps_application_open_uri_at_dest (PpsApplication  *application,
				 const char     *uri,
				 PpsLinkDest     *dest,
				 PpsWindowRunMode mode)
{
	PpsWindow *pps_window = NULL;
	GList *l, *windows;
	guint n_windows = 0;

	g_return_if_fail (uri != NULL);

	windows = gtk_application_get_windows (GTK_APPLICATION (application));
	for (l = windows; l != NULL; l = l->next) {
		if (PPS_IS_WINDOW (l->data)) {
			const gchar *window_uri = pps_window_get_uri (PPS_WINDOW (l->data));
			if (pps_window_is_empty (PPS_WINDOW (l->data)) || !g_strcmp0 (window_uri, uri)) {
				pps_window = PPS_WINDOW (l->data);
				break;
			}
			n_windows++;
		}
	}

	if (n_windows > 0 && !pps_window) {
		/* spawn a new papers process */
		pps_spawn (uri, dest, mode);
		return;
	}

	if (!pps_window)
		pps_window = PPS_WINDOW (pps_window_new ());

	pps_application_open_uri_in_window (application, uri, pps_window,
					   dest, mode);
}

/**
 * pps_application_open_recent_view:
 * @application: The instance of the application.
 *
 * Creates a new window showing the recent view
 */
void
pps_application_open_start_view (PpsApplication *application)
{
	GtkWidget *new_window = GTK_WIDGET (pps_window_new ());

	if (!gtk_widget_get_realized (new_window))
		gtk_widget_realize (new_window);

	gtk_window_present (GTK_WINDOW (new_window));
}

#ifdef ENABLE_DBUS
static gboolean
handle_get_window_list_cb (PpsPapersApplication   *object,
                           GDBusMethodInvocation *invocation,
                           PpsApplication         *application)
{
        GList     *windows, *l;
        GPtrArray *paths;

        paths = g_ptr_array_new ();

        windows = gtk_application_get_windows (GTK_APPLICATION (application));
        for (l = windows; l; l = g_list_next (l)) {
                if (!PPS_IS_WINDOW (l->data))
                        continue;

                g_ptr_array_add (paths, (gpointer) pps_window_get_dbus_object_path (PPS_WINDOW (l->data)));
        }

        g_ptr_array_add (paths, NULL);
        pps_papers_application_complete_get_window_list (object, invocation,
                                                        (const char * const *) paths->pdata);

        g_ptr_array_free (paths, TRUE);

        return TRUE;
}
#endif /* ENABLE_DBUS */

void
pps_application_open_uri_list (PpsApplication *application,
			      GListModel    *files)
{
	GFile *file;
	guint pos = 0;
	const char *uri;

	while ((file = g_list_model_get_item (files, pos++)) != NULL) {
		uri = g_file_get_uri(file);
		if (!uri)
			continue;

		pps_application_open_uri_at_dest (application, uri,
						 NULL, PPS_WINDOW_MODE_NORMAL);
	}
}

static void
pps_application_about_activated (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));

        const char *developers[] = {
                "Martin Kretzschmar <m_kretzschmar@gmx.net>",
                "Jonathan Blandford <jrb@gnome.org>",
                "Marco Pesenti Gritti <marco@gnome.org>",
                "Nickolay V. Shmyrev <nshmyrev@yandex.ru>",
                "Bryan Clark <clarkbw@gnome.org>",
                "Carlos Garcia Campos <carlosgc@gnome.org>",
                "Wouter Bolsterlee <wbolster@gnome.org>",
                "Christian Persch <chpe" "\100" "src.gnome.org>",
                "Germán Poo-Caamaño <gpoo" "\100" "gnome.org>",
                "Qiu Wenbo <qiuwenbo" "\100" "gnome.org>",
                "Pablo Correa Gómez <ablocorrea" "\100" "hotmail.com>",
                NULL
        };
        const char *documenters[] = {
                "Nickolay V. Shmyrev <nshmyrev@yandex.ru>",
                "Phil Bull <philbull@gmail.com>",
                "Tiffany Antpolski <tiffany.antopolski@gmail.com>",
                NULL
        };

        const char **p;

        for (p = developers; *p; ++p)
                *p = _(*p);

        for (p = documenters; *p; ++p)
                *p = _(*p);

        adw_show_about_dialog (GTK_WIDGET (window),
			       "application-name", _("Papers"),
                               "version", VERSION,
                               "copyright", _("© 1996–2024 The Papers authors"),
                               "license-type", GTK_LICENSE_GPL_2_0,
                               "website", "https://apps.gnome.org/Papers/",
                               "developers", developers,
                               "documenters", documenters,
                               "translator-credits", _("translator-credits"),
                               "application-icon", APP_ID,
                               NULL);
}

static void
pps_application_help_activated (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));
	GtkUriLauncher *launcher = gtk_uri_launcher_new ("help:papers");

	gtk_uri_launcher_launch(launcher, window, NULL, NULL, NULL);
}

static void
pps_application_quit_activated (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       app)
{
	g_application_quit (G_APPLICATION (app));
}

static void
pps_application_new_window_activated (GSimpleAction *action,
				      GVariant      *parameter,
				      gpointer       app)
{
	/* spawn an empty window */
	pps_spawn (NULL, NULL, PPS_WINDOW_MODE_NORMAL);
}

static void
pps_application_startup (GApplication *gapplication)
{
        const gchar *action_accels[] = {
          "win.open",                   "<Ctrl>O", NULL,
          "win.open-copy",              "<Ctrl><Shift>N", NULL,
          "win.save-as",                "<Ctrl>S", NULL,
          "win.print",                  "<Ctrl>P", NULL,
          "win.show-properties",        "<alt>Return", NULL,
          "win.copy",                   "<Ctrl>C", "<Ctrl>Insert", NULL,
          "win.select-all",             "<Ctrl>A", NULL,
          "win.close",                  "<Ctrl>W", NULL,
          "win.escape",                 "Escape", NULL,
          "win.select-page",            "<Ctrl>L", NULL,
          "win.caret-navigation",       "F7", NULL,
          "win.show-sidebar",           "F9", NULL,
          "win.fullscreen",             "F11", NULL,
          "win.presentation",           "F5", "<Shift>F5", NULL,
          "win.rotate-left",            "<Ctrl>Left", NULL,
          "win.rotate-right",           "<Ctrl>Right", NULL,
          "win.inverted-colors",        "<Ctrl>I", NULL,
          "win.add-highlight-annotation", "<Ctrl>H", NULL,
          "app.help",                   "F1", NULL,
          "app.new",                    "<Ctrl>N", NULL,
          NULL
        };

	static GActionEntry app_entries[] = {
		{"about", pps_application_about_activated, NULL, NULL, NULL},
		{"help", pps_application_help_activated, NULL, NULL, NULL},
		{"quit", pps_application_quit_activated, NULL, NULL, NULL},
		{"new", pps_application_new_window_activated, NULL, NULL, NULL},
	};

        PpsApplication *application = PPS_APPLICATION (gapplication);
        const gchar **it;

        G_APPLICATION_CLASS (pps_application_parent_class)->startup (gapplication);

	pps_init();

	/* Manually set name and icon */
	g_set_application_name (_("Papers"));
	gtk_window_set_default_icon_name (APP_ID);

	g_action_map_add_action_entries (G_ACTION_MAP (application), app_entries,
			G_N_ELEMENTS (app_entries), application);

        for (it = action_accels; it[0]; it += g_strv_length ((gchar **)it) + 1)
                gtk_application_set_accels_for_action (GTK_APPLICATION (application), it[0], &it[1]);
}

static void
pps_application_shutdown (GApplication *gapplication)
{
	pps_shutdown();
	pps_job_scheduler_wait ();

        G_APPLICATION_CLASS (pps_application_parent_class)->shutdown (gapplication);
}

static void
pps_application_activate (GApplication *gapplication)
{
        PpsApplication *application = PPS_APPLICATION (gapplication);
        GList *windows, *l;

        windows = gtk_application_get_windows (GTK_APPLICATION (application));
        for (l = windows; l != NULL; l = l->next) {
                if (!PPS_IS_WINDOW (l->data))
                        continue;

                gtk_window_present (GTK_WINDOW (l->data));
        }
}

#ifdef ENABLE_DBUS
static gboolean
pps_application_dbus_register (GApplication    *gapplication,
                              GDBusConnection *connection,
                              const gchar     *object_path,
                              GError         **error)
{
        PpsApplication *application = PPS_APPLICATION (gapplication);
        PpsPapersApplication *skeleton;

        if (!G_APPLICATION_CLASS (pps_application_parent_class)->dbus_register (gapplication,
                                                                               connection,
                                                                               object_path,
                                                                               error))
                return FALSE;

        skeleton = pps_papers_application_skeleton_new ();
        if (!g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (skeleton),
                                               connection,
                                               APPLICATION_DBUS_OBJECT_PATH,
                                               error)) {
                g_object_unref (skeleton);

                return FALSE;
        }

        application->skeleton = skeleton;
        g_signal_connect (skeleton, "handle-get-window-list",
                          G_CALLBACK (handle_get_window_list_cb),
                          application);
        return TRUE;
}

static void
pps_application_dbus_unregister (GApplication    *gapplication,
                                GDBusConnection *connection,
                                const gchar     *object_path)
{
        PpsApplication *application = PPS_APPLICATION (gapplication);

        if (application->skeleton != NULL) {
                g_dbus_interface_skeleton_unexport (G_DBUS_INTERFACE_SKELETON (application->skeleton));
		g_clear_object (&application->skeleton);
        }

        G_APPLICATION_CLASS (pps_application_parent_class)->dbus_unregister (gapplication,
                                                                            connection,
                                                                            object_path);
}

#endif /* ENABLE_DBUS */
static gchar *
get_label_from_filename (const gchar *filename)
{
	GFile   *file;
	gchar   *label;
	gboolean exists;

	label = g_strrstr (filename, "#");
	if (!label)
		return NULL;

	/* Filename contains a #, check
	 * whether it's part of the path
	 * or a label
	 */
	file = g_file_new_for_commandline_arg (filename);
	exists = g_file_query_exists (file, NULL);
	g_object_unref (file);

	return exists ? NULL : label;
}

static int
pps_application_command_line (GApplication	     *gapplication,
			     GApplicationCommandLine *command_line)
{
	GVariantDict *options = g_application_command_line_get_options_dict (command_line);
	PpsApplication *pps_app = PPS_APPLICATION (gapplication);
	PpsWindowRunMode  mode = PPS_WINDOW_MODE_NORMAL;
	gint             i;
	PpsLinkDest      *global_dest = NULL;
	gint32 page_index = 0;
	gchar *named_dest = NULL, *page_label = NULL;
	g_autofree const gchar **files = NULL;

	if (g_variant_dict_contains (options, "fullscreen"))
		mode = PPS_WINDOW_MODE_FULLSCREEN;
	else if (g_variant_dict_contains (options, "presentation"))
		mode = PPS_WINDOW_MODE_PRESENTATION;

	g_variant_dict_lookup (options, "page-label", "s", &page_label);
	g_variant_dict_lookup (options, "named-dest", "s", &named_dest);
	g_variant_dict_lookup (options, "page-index", "i", &page_index);
	g_variant_dict_lookup (options, G_OPTION_REMAINING, "^a&ay", &files);

	if (!files) {
		pps_application_open_start_view (pps_app);
		return 0;
	}

	if (page_label)
		global_dest = pps_link_dest_new_page_label (page_label);
	else if (page_index)
		global_dest = pps_link_dest_new_page (MAX (0, page_index - 1));
	else if (named_dest)
		global_dest = pps_link_dest_new_named (named_dest);

	for (i = 0; files[i]; i++) {
		const gchar *filename;
		gchar       *uri;
		gchar       *label;
		GFile       *file;
		PpsLinkDest  *dest = NULL;

		filename = files[i];
		label = get_label_from_filename (filename);
		if (label) {
			*label = 0;
			label++;
			dest = pps_link_dest_new_page_label (label);
		} else if (global_dest) {
			dest = g_object_ref (global_dest);
		}

		file = g_file_new_for_commandline_arg (filename);
		uri = g_file_get_uri (file);
		g_object_unref (file);

		pps_application_open_uri_at_dest (pps_app, uri, dest,
						 mode);

		g_clear_object (&dest);
		g_free (uri);
        }

	return 0;
}

static gint
pps_application_handle_local_options (GApplication *gapplication,
				     GVariantDict *options)
{
	/* print the version in local instance rather than sending it to primary */
	if (g_variant_dict_contains(options, "version")) {
		g_print ("%s %s\n", _("Papers"), VERSION);
		return 0;
	}

	return -1;
}

static void
pps_application_open (GApplication	 *application,
		      GFile		**files,
		      gint		  n_files,
		      const gchar	 *hint)
{
	guint pos;
	const char *uri;

	for (pos = 0; pos < n_files; pos++) {
		uri = g_file_get_uri (files[pos]);
		if (!uri)
			continue;

		pps_application_open_uri_at_dest (PPS_APPLICATION (application), uri,
						 NULL, PPS_WINDOW_MODE_NORMAL);
	}
}

static void
pps_application_class_init (PpsApplicationClass *pps_application_class)
{
        GApplicationClass *g_application_class = G_APPLICATION_CLASS (pps_application_class);

        g_application_class->startup = pps_application_startup;
        g_application_class->activate = pps_application_activate;
        g_application_class->shutdown = pps_application_shutdown;
	g_application_class->command_line = pps_application_command_line;
	g_application_class->handle_local_options = pps_application_handle_local_options;
	g_application_class->open = pps_application_open;

#ifdef ENABLE_DBUS
        g_application_class->dbus_register = pps_application_dbus_register;
        g_application_class->dbus_unregister = pps_application_dbus_unregister;
#endif
}

static void
pps_application_init (PpsApplication *pps_application)
{
	static const GOptionEntry option_entries[] =
	{
		{ "page-label", 'p', 0, G_OPTION_ARG_STRING, NULL, N_("The page label of the document to display."), N_("PAGE")},
		{ "page-index", 'i', 0, G_OPTION_ARG_INT, NULL, N_("The page number of the document to display."), N_("NUMBER")},
		{ "named-dest", 'n', 0, G_OPTION_ARG_STRING, NULL, N_("Named destination to display."), N_("DEST")},
		{ "fullscreen", 'f', 0, G_OPTION_ARG_NONE, NULL, N_("Run papers in fullscreen mode."), NULL },
		{ "presentation", 's', 0, G_OPTION_ARG_NONE, NULL, N_("Run papers in presentation mode."), NULL },
		{ "find", 'l', 0, G_OPTION_ARG_STRING, NULL, N_("The word or phrase to find in the document."), N_("STRING")},
		{ "version", 0, 0, G_OPTION_ARG_NONE, NULL, N_("Show the version of the program."), NULL },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, NULL, NULL, N_("[FILE…]") },
		{ NULL }
	};

	g_application_set_option_context_parameter_string (G_APPLICATION (pps_application), N_("Papers"));
	g_application_add_main_option_entries (G_APPLICATION (pps_application), option_entries);
}
