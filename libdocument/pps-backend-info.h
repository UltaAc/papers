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

#pragma once

#if !defined(PAPERS_COMPILATION)
#error "This is a private header."
#endif

#include <glib.h>

G_BEGIN_DECLS

typedef struct _PpsBackendInfo PpsBackendInfo;

struct _PpsBackendInfo {
	gchar *type_desc;
	gchar **mime_types;

	gchar *module_name;
	gboolean resident;
};

void _pps_backend_info_free (PpsBackendInfo *info);

GList *_pps_backend_info_load_from_dir (const char *path);

G_END_DECLS
