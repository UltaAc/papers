/* this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2008 Carlos Garcia Campos <carlosgc@gnome.org>
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

#if !defined(__PPS_PAPERS_DOCUMENT_H_INSIDE__) && !defined(PAPERS_COMPILATION)
#error "Only <papers-document.h> can be included directly."
#endif

#include <gio/gio.h>
#include <glib-object.h>

#include "pps-macros.h"

G_BEGIN_DECLS

#define PPS_TYPE_LAYER (pps_layer_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsLayer, pps_layer, PPS, LAYER, GObject)

struct _PpsLayer {
	GObject base_instance;
};

PPS_PUBLIC
PpsLayer *pps_layer_new (gint rb_group);
PPS_PUBLIC
gint pps_layer_get_rb_group (PpsLayer *layer);
PPS_PUBLIC
void pps_layer_set_children (PpsLayer *pps_layer,
                             GListModel *children);
PPS_PUBLIC
GListModel *pps_layer_get_children (PpsLayer *pps_layer);

G_END_DECLS
