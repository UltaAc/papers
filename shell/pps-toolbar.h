/* pps-toolbar.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2012 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include <gtk/gtk.h>
#include <adwaita.h>
#include "pps-window.h"

G_BEGIN_DECLS

#define PPS_TYPE_TOOLBAR              (pps_toolbar_get_type())
G_DECLARE_FINAL_TYPE (PpsToolbar, pps_toolbar, PPS, TOOLBAR, AdwBin);

struct _PpsToolbar {
        AdwBin base_instance;
};

GtkWidget    *pps_toolbar_new                (void);
GtkWidget    *pps_toolbar_get_page_selector  (PpsToolbar *pps_toolbar);
AdwHeaderBar *pps_toolbar_get_header_bar     (PpsToolbar *pps_toolbar);

G_END_DECLS
