/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2007 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include "pps-backend-info.h"

#define PPS_BACKENDS_GROUP "Papers Backend"
#define PPS_BACKENDS_EXTENSION ".papers-backend"

void
_pps_backend_info_free (PpsBackendInfo *info)
{
	if (info == NULL)
		return;

	g_free (info->module_name);
	g_free (info->type_desc);
	g_strfreev (info->mime_types);
	g_free (info);
}

/**
 * _pps_backend_info_new_from_file:
 * @path: path to the backends file
 * @error: a location to store a #GError, or %NULL
 *
 * Loads backend information from @path.
 *
 * Returns: a new #PpsBackendInfo, or %NULL on error with @error filled in
 */
static PpsBackendInfo *
_pps_backend_info_new_from_file (const char *file,
                                 GError **error)
{
	PpsBackendInfo *info = NULL;
	GKeyFile *backend_file = NULL;

	backend_file = g_key_file_new ();
	if (!g_key_file_load_from_file (backend_file, file, G_KEY_FILE_NONE, error))
		goto err;

	info = g_new0 (PpsBackendInfo, 1);

	info->module_name = g_key_file_get_string (backend_file, PPS_BACKENDS_GROUP,
	                                           "Module", error);
	if (!info->module_name)
		goto err;

	info->resident = g_key_file_get_boolean (backend_file, PPS_BACKENDS_GROUP,
	                                         "Resident", NULL);

	info->type_desc = g_key_file_get_locale_string (backend_file, PPS_BACKENDS_GROUP,
	                                                "TypeDescription", NULL, error);
	if (!info->type_desc)
		goto err;

	info->mime_types = g_key_file_get_string_list (backend_file, PPS_BACKENDS_GROUP,
	                                               "MimeType", NULL, error);
	if (!info->mime_types)
		goto err;

	g_key_file_free (backend_file);

	return info;

err:
	g_key_file_free (backend_file);
	return NULL;
}

/*
 * _pps_backend_info_load_from_dir:
 * @path: a directory name
 *
 * Load all backend infos from @path.
 *
 * Returns: a newly allocated #GList containing newly allocated
 *   #PpsBackendInfo objects
 */
GList *
_pps_backend_info_load_from_dir (const char *path)
{
	GList *list = NULL;
	GDir *dir;
	const gchar *dirent;
	GError *error = NULL;

	dir = g_dir_open (path, 0, &error);
	if (!dir) {
		g_warning ("%s", error->message);
		g_error_free (error);

		return FALSE;
	}

	while ((dirent = g_dir_read_name (dir))) {
		PpsBackendInfo *info;
		gchar *file;

		if (!g_str_has_suffix (dirent, PPS_BACKENDS_EXTENSION))
			continue;

		file = g_build_filename (path, dirent, NULL);
		info = _pps_backend_info_new_from_file (file, &error);
		if (error != NULL) {
			g_warning ("Failed to load backend info from '%s': %s\n",
			           file, error->message);
			g_clear_error (&error);
		}
		g_free (file);

		if (info == NULL)
			continue;

		list = g_list_prepend (list, info);
	}

	g_dir_close (dir);

	return list;
}
