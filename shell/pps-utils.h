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

#include "pps-document-view.h"

G_BEGIN_DECLS

GdkPixbufFormat *pps_gdk_pixbuf_format_by_extension (const gchar *uri);
void pps_spawn (const char *uri,
                PpsLinkDest *dest);

G_END_DECLS
