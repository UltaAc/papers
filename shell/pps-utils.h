/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/*
 *  Copyright (C) 2004 Anders Carlsson <andersca@gnome.org>
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

#pragma once

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <pps-document.h>

#include "pps-window.h"

G_BEGIN_DECLS

void			pps_print_region_contents (cairo_region_t *region);

GdkPixbufFormat* 	get_gdk_pixbuf_format_by_extension (const gchar *uri);
gchar*                  pps_str_replace (const char *str, const char *substr, const char *repl);
void                    pps_spawn (const char      *uri,
				   PpsLinkDest     *dest,
				   PpsWindowRunMode mode);


/*
 * Temporary hack around https://gitlab.gnome.org/GNOME/gtk/-/issues/1025
 */
void                    pps_spinner_map_cb (GtkSpinner *spinner);
void                    pps_spinner_unmap_cb (GtkSpinner *spinner);

G_END_DECLS
