/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2004 Martin Kretzschmar
 *
 *  Author:
 *    Martin Kretzschmar <martink@gnome.org>
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

#include <glib.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "pps-link.h"
#include "pps-history.h"
#include "pps-document-model.h"
#include "pps-metadata.h"

G_BEGIN_DECLS

typedef enum {
	PPS_WINDOW_MODE_NORMAL,
	PPS_WINDOW_MODE_FULLSCREEN,
	PPS_WINDOW_MODE_PRESENTATION,
	PPS_WINDOW_MODE_START_VIEW,
	PPS_WINDOW_MODE_ERROR_VIEW,
	PPS_WINDOW_MODE_PASSWORD_VIEW,
} PpsWindowRunMode;

typedef struct {
	gint start;
	gint end;
} PpsPrintRange;

typedef enum {
	PPS_PRINT_PAGE_SET_ALL,
	PPS_PRINT_PAGE_SET_EVEN,
	PPS_PRINT_PAGE_SET_ODD
} PpsPrintPageSet;

#define PPS_TYPE_WINDOW			(pps_window_get_type())
G_DECLARE_FINAL_TYPE (PpsWindow, pps_window, PPS, WINDOW, AdwApplicationWindow)

struct _PpsWindow {
	AdwApplicationWindow base_instance;
};

PpsWindow       *pps_window_new                            (void);
const char     *pps_window_get_uri                        (PpsWindow       *pps_window);
void		pps_window_open_uri	                 (PpsWindow       *pps_window,
                                                          const char     *uri,
                                                          PpsLinkDest     *dest,
                                                          PpsWindowRunMode mode,
                                                          const gchar    *search_string);
void		pps_window_open_document                  (PpsWindow       *pps_window,
                                                          PpsDocument     *document,
                                                          PpsLinkDest     *dest,
                                                          PpsWindowRunMode mode,
                                                          const gchar    *search_string);
gboolean	pps_window_is_empty	                 (PpsWindow       *pps_window);
void		pps_window_print_range                    (PpsWindow       *pps_window,
                                                          int             first_page,
                                                          int		 last_page);
const gchar    *pps_window_get_dbus_object_path           (PpsWindow       *pps_window);
void            pps_window_focus_view                     (PpsWindow       *pps_window);
AdwHeaderBar   *pps_window_get_header_bar                 (PpsWindow       *pps_window);
void            pps_window_handle_annot_popup             (PpsWindow       *pps_window,
                                                          PpsAnnotation   *annot);
PpsMetadata     *pps_window_get_metadata			 (PpsWindow	 *pps_window);

G_END_DECLS
