/*
 *  Copyright (C) 2004 Marco Pesenti Gritti
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

#include <stdlib.h>
#include <string.h>

#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "ev-application.h"
#include "ev-debug.h"
#include "ev-init.h"
#include "ev-file-helpers.h"
#include "ev-metadata.h"

#ifdef G_OS_WIN32
#include <io.h>
#include <conio.h>
#if !(_WIN32_WINNT >= 0x0500)
#error "_WIN32_WINNT must be defined >= 0x0500"
#endif
#include <windows.h>
#endif

static gchar   *ev_page_label;
static gchar   *ev_find_string;
static gint     ev_page_index = 0;
static gchar   *ev_named_dest;
static gboolean new_window = FALSE;
static gboolean fullscreen_mode = FALSE;
static gboolean presentation_mode = FALSE;
static const char **file_arguments = NULL;


static gboolean
option_version_cb (const gchar *option_name,
                   const gchar *value,
                   gpointer     data,
                   GError     **error)
{
  g_print ("%s %s\n", _("GNOME Document Viewer"), VERSION);

  exit (0);
  return FALSE;
}

static const GOptionEntry goption_options[] =
{
	{ "new-window", 'o', 0, G_OPTION_ARG_NONE, &new_window, N_("Open a new window."), NULL },
	{ "page-label", 'p', 0, G_OPTION_ARG_STRING, &ev_page_label, N_("The page label of the document to display."), N_("PAGE")},
	{ "page-index", 'i', 0, G_OPTION_ARG_INT, &ev_page_index, N_("The page number of the document to display."), N_("NUMBER")},
	{ "named-dest", 'n', 0, G_OPTION_ARG_STRING, &ev_named_dest, N_("Named destination to display."), N_("DEST")},
	{ "fullscreen", 'f', 0, G_OPTION_ARG_NONE, &fullscreen_mode, N_("Run evince in fullscreen mode"), NULL },
	{ "presentation", 's', 0, G_OPTION_ARG_NONE, &presentation_mode, N_("Run evince in presentation mode"), NULL },
	{ "find", 'l', 0, G_OPTION_ARG_STRING, &ev_find_string, N_("The word or phrase to find in the document"), N_("STRING")},
	{ "version", 0, G_OPTION_FLAG_NO_ARG | G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_CALLBACK, option_version_cb, NULL, NULL },
	{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &file_arguments, NULL, N_("[FILE…]") },
	{ NULL }
};

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

static void
load_files (const char **files)
{
	GdkDisplay *display = gdk_display_get_default();
	EvWindowRunMode  mode = EV_WINDOW_MODE_NORMAL;
	gint             i;
	EvLinkDest      *global_dest = NULL;

	if (!files || new_window) {
		if (!ev_application_has_window (EV_APP) || new_window)
			ev_application_open_recent_view (EV_APP, display);
		return;
	}

	if (ev_page_label)
		global_dest = ev_link_dest_new_page_label (ev_page_label);
	else if (ev_page_index)
		global_dest = ev_link_dest_new_page (MAX (0, ev_page_index - 1));
	else if (ev_named_dest)
		global_dest = ev_link_dest_new_named (ev_named_dest);

	if (fullscreen_mode)
		mode = EV_WINDOW_MODE_FULLSCREEN;
	else if (presentation_mode)
		mode = EV_WINDOW_MODE_PRESENTATION;

	for (i = 0; files[i]; i++) {
		const gchar *filename;
		gchar       *uri;
		gchar       *label;
		GFile       *file;
		EvLinkDest  *dest = NULL;
		const gchar *app_uri;

		filename = files[i];
		label = get_label_from_filename (filename);
		if (label) {
			*label = 0;
			label++;
			dest = ev_link_dest_new_page_label (label);
		} else if (global_dest) {
			dest = g_object_ref (global_dest);
		}

		file = g_file_new_for_commandline_arg (filename);
		uri = g_file_get_uri (file);
		g_object_unref (file);

		app_uri = ev_application_get_uri (EV_APP);
		if (app_uri && strcmp (app_uri, uri) == 0) {
			g_free (uri);
			continue;
		}



		ev_application_open_uri_at_dest (EV_APP, uri, display, dest,
						 mode, ev_find_string,
						 GDK_CURRENT_TIME);

		if (dest)
			g_object_unref (dest);
		g_free (uri);
        }
}

int
main (int argc, char *argv[])
{
        EvApplication  *application;
	GOptionContext *context;
	GError         *error = NULL;
        int             status;

#ifdef G_OS_WIN32

    if (fileno (stdout) != -1 &&
 	  _get_osfhandle (fileno (stdout)) != -1)
	{
	  /* stdout is fine, presumably redirected to a file or pipe */
	}
    else
    {
	  typedef BOOL (* WINAPI AttachConsole_t) (DWORD);

	  AttachConsole_t p_AttachConsole =
	    (AttachConsole_t) GetProcAddress (GetModuleHandle ("kernel32.dll"), "AttachConsole");

	  if (p_AttachConsole != NULL && p_AttachConsole (ATTACH_PARENT_PROCESS))
      {
	      freopen ("CONOUT$", "w", stdout);
	      dup2 (fileno (stdout), 1);
	      freopen ("CONOUT$", "w", stderr);
	      dup2 (fileno (stderr), 2);

      }
	}
#endif

	/* Initialize the i18n stuff */
	bindtextdomain (GETTEXT_PACKAGE, ev_get_locale_dir());
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	context = g_option_context_new (N_("GNOME Document Viewer"));
	g_option_context_set_translation_domain(context, GETTEXT_PACKAGE);
	g_option_context_add_main_entries (context, goption_options, GETTEXT_PACKAGE);

	if (!g_option_context_parse (context, &argc, &argv, &error)) {
		g_printerr ("Cannot parse arguments: %s\n", error->message);
		g_error_free (error);
		g_option_context_free (context);

		return 1;
	}
	g_option_context_free (context);

        if (!ev_init ())
                return 1;

        application = ev_application_new ();
        if (!g_application_register (G_APPLICATION (application), NULL, &error)) {
                g_printerr ("Failed to register: %s\n", error->message);
                g_error_free (error);
                status = 1;
                goto done;
        }

	load_files (file_arguments);

	/* Change directory so we don't prevent unmounting in case the initial cwd
	 * is on an external device (see bug #575436)
	 */
	g_chdir (g_get_home_dir ());

	status = g_application_run (G_APPLICATION (application), 0, NULL);

    done:
	ev_job_scheduler_wait ();
	ev_shutdown ();

        g_object_unref (application);
	return status;
}
