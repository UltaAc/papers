/* pps-sidebar-layers.h
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

#include <gtk/gtk.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _PpsSidebarLayers        PpsSidebarLayers;
typedef struct _PpsSidebarLayersClass   PpsSidebarLayersClass;
typedef struct _PpsSidebarLayersPrivate PpsSidebarLayersPrivate;

#define PPS_TYPE_SIDEBAR_LAYERS              (pps_sidebar_layers_get_type())
#define PPS_SIDEBAR_LAYERS(object)           (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_SIDEBAR_LAYERS, PpsSidebarLayers))
#define PPS_SIDEBAR_LAYERS_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_SIDEBAR_LAYERS, PpsSidebarLayersClass))
#define PPS_IS_SIDEBAR_LAYERS(object)        (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_SIDEBAR_LAYERS))
#define PPS_IS_SIDEBAR_LAYERS_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_SIDEBAR_LAYERS))
#define PPS_SIDEBAR_LAYERS_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_SIDEBAR_LAYERS, PpsSidebarLayersClass))

struct _PpsSidebarLayers {
	GtkBox base_instance;

	PpsSidebarLayersPrivate *priv;
};

struct _PpsSidebarLayersClass {
	GtkBoxClass base_class;

	/* Signals */
	void (* layers_visibility_changed) (PpsSidebarLayers *pps_layers);
};

GType      pps_sidebar_layers_get_type            (void) G_GNUC_CONST;
GtkWidget *pps_sidebar_layers_new                 (void);
void       pps_sidebar_layers_update_layers_state (PpsSidebarLayers *sidebar_layers);

G_END_DECLS
