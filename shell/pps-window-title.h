/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2005 Red Hat, Inc
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

#include "pps-window.h"
#include "pps-document.h"

G_BEGIN_DECLS

typedef struct _PpsWindowTitle PpsWindowTitle;

PpsWindowTitle *pps_window_title_new	    (PpsWindow *window);
void           pps_window_title_set_document (PpsWindowTitle     *window_title,
					     PpsDocument        *document);
void	       pps_window_title_set_filename (PpsWindowTitle     *window_title,
					     const char        *filename);
void	       pps_window_title_free         (PpsWindowTitle     *window_title);

G_END_DECLS
