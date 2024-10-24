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

#include "pps-view-cursor.h"

static const gchar *cursors[] = {
	[PPS_VIEW_CURSOR_NORMAL] = NULL,
	[PPS_VIEW_CURSOR_IBEAM] = "text",
	[PPS_VIEW_CURSOR_LINK] = "pointer",
	[PPS_VIEW_CURSOR_WAIT] = "wait",
	[PPS_VIEW_CURSOR_HIDDEN] = "none",
	[PPS_VIEW_CURSOR_DRAG] = "grabbing",
	[PPS_VIEW_CURSOR_ADD] = "crosshair",
};

const gchar *
pps_view_cursor_name (PpsViewCursor cursor)
{
	if (cursor < G_N_ELEMENTS (cursors))
		return cursors[cursor];

	return NULL;
}
