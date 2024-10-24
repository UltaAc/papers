/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2004 Red Hat, Inc
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

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum {
	PPS_VIEW_CURSOR_NORMAL,
	PPS_VIEW_CURSOR_IBEAM,
	PPS_VIEW_CURSOR_LINK,
	PPS_VIEW_CURSOR_WAIT,
	PPS_VIEW_CURSOR_HIDDEN,
	PPS_VIEW_CURSOR_DRAG,
	PPS_VIEW_CURSOR_ADD
} PpsViewCursor;

const gchar *pps_view_cursor_name (PpsViewCursor cursor);

G_END_DECLS
