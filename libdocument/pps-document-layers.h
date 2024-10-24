/* pps-document-layers.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2008 Carlos Garcia Campos  <carlosgc@gnome.org>
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

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "pps-layer.h"
#include "pps-macros.h"

G_BEGIN_DECLS

#define PPS_TYPE_DOCUMENT_LAYERS (pps_document_layers_get_type ())

PPS_PUBLIC
G_DECLARE_INTERFACE (PpsDocumentLayers, pps_document_layers, PPS, DOCUMENT_LAYERS, GObject)

enum {
	PPS_DOCUMENT_LAYERS_COLUMN_TITLE,
	PPS_DOCUMENT_LAYERS_COLUMN_LAYER,
	PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE,
	PPS_DOCUMENT_LAYERS_COLUMN_ENABLED,
	PPS_DOCUMENT_LAYERS_COLUMN_SHOWTOGGLE,
	PPS_DOCUMENT_LAYERS_COLUMN_RBGROUP,
	PPS_DOCUMENT_LAYERS_N_COLUMNS
};

struct _PpsDocumentLayersInterface {
	GTypeInterface base_iface;

	/* Methods  */
	gboolean (*has_layers) (PpsDocumentLayers *document_layers);
	GListModel *(*get_layers) (PpsDocumentLayers *document_layers);

	void (*show_layer) (PpsDocumentLayers *document_layers,
	                    PpsLayer *layer);
	void (*hide_layer) (PpsDocumentLayers *document_layers,
	                    PpsLayer *layer);
	gboolean (*layer_is_visible) (PpsDocumentLayers *document_layers,
	                              PpsLayer *layer);
};

PPS_PUBLIC
gboolean pps_document_layers_has_layers (PpsDocumentLayers *document_layers);
PPS_PUBLIC
GListModel *pps_document_layers_get_layers (PpsDocumentLayers *document_layers);
PPS_PUBLIC
void pps_document_layers_show_layer (PpsDocumentLayers *document_layers,
                                     PpsLayer *layer);
PPS_PUBLIC
void pps_document_layers_hide_layer (PpsDocumentLayers *document_layers,
                                     PpsLayer *layer);
PPS_PUBLIC
gboolean pps_document_layers_layer_is_visible (PpsDocumentLayers *document_layers,
                                               PpsLayer *layer);

G_END_DECLS
