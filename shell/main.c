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

int
main (int argc, char *argv[])
{
        EvApplication  *application;
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

        if (!ev_init ())
                return 1;

        application = ev_application_new ();
        if (!g_application_register (G_APPLICATION (application), NULL, &error)) {
                g_printerr ("Failed to register: %s\n", error->message);
                g_error_free (error);
                status = 1;
                goto done;
        }

	/* Change directory so we don't prevent unmounting in case the initial cwd
	 * is on an external device (see bug #575436)
	 */
	g_chdir (g_get_home_dir ());

	status = g_application_run (G_APPLICATION (application), argc, argv);

    done:
	ev_job_scheduler_wait ();
	ev_shutdown ();

        g_object_unref (application);
	return status;
}
