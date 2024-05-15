/* this file is part of papers, a gnome document viewer
 *
 * Copyright © 2009 Christian Persch
 *
 * Papers is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * Papers is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <config.h>

#include <glib.h>
#include <glib/gi18n-lib.h>
#ifdef G_OS_WIN32
#include <windows.h>
#endif

#include <exempi/xmp.h>

#include "pps-document-factory.h"
#include "pps-file-helpers.h"
#include "pps-init.h"

static int pps_init_count;

#ifdef G_OS_WIN32

static HMODULE evdocument_dll = NULL;
static gchar *locale_dir = NULL;

#ifdef DLL_EXPORT
BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
BOOL WINAPI
DllMain (HINSTANCE hinstDLL,
	 DWORD     fdwReason,
	 LPVOID    lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		evdocument_dll = hinstDLL;

	return TRUE;
}
#endif

static const gchar *
_pps_win32_get_locale_dir (HMODULE module)
{
	if (locale_dir)
		return locale_dir;

	gchar *install_dir = NULL, *utf8_locale_dir;

	if (evdocument_dll != NULL)
		install_dir =
		g_win32_get_package_installation_directory_of_module (module);

	if (install_dir) {
		utf8_locale_dir = g_build_filename (install_dir,
			"share", "locale", NULL);

		locale_dir = g_win32_locale_filename_from_utf8 (utf8_locale_dir);

		g_free (install_dir);
		g_free (utf8_locale_dir);
	}

	if (!locale_dir)
		locale_dir = g_strdup ("");

	return locale_dir;
}

#endif

/**
 * pps_get_locale_dir:
 *
 * Returns: (type filename): The locale directory.
 */
const gchar *
pps_get_locale_dir (void)
{
#ifdef G_OS_WIN32
	return _pps_win32_get_locale_dir (evdocument_dll);
#else
	return PPS_LOCALEDIR;
#endif
}

/**
 * pps_init:
 *
 * Initializes the papers document library, and binds the papers
 * gettext domain.
 *
 * You must call this before calling any other function in the papers
 * document library.
 *
 * Returns: %TRUE if any backends were found; %FALSE otherwise
 */
gboolean
pps_init (void)
{
        static gboolean have_backends;

        if (pps_init_count++ > 0)
                return have_backends;

	/* set up translation catalog */
	bindtextdomain (GETTEXT_PACKAGE, pps_get_locale_dir ());
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	xmp_init ();
        gdk_pixbuf_init_modules (EXTRA_GDK_PIXBUF_LOADERS_DIR, NULL);
        _pps_file_helpers_init ();
        have_backends = _pps_document_factory_init ();

        return have_backends;
}

/**
 * pps_shutdown:
 *
 * Shuts the papers document library down.
 */
void
pps_shutdown (void)
{
        g_assert (_pps_is_initialized ());

        if (--pps_init_count > 0)
                return;

#ifdef G_OS_WIN32
	if (locale_dir != NULL)
		g_free(locale_dir);
#endif

	xmp_terminate ();
        _pps_document_factory_shutdown ();
        _pps_file_helpers_shutdown ();
}

/*
 * _pps_is_initialized:
 *
 * Returns: %TRUE if the papers document library has been initialized
 */
gboolean
_pps_is_initialized (void)
{
        return pps_init_count > 0;
}
