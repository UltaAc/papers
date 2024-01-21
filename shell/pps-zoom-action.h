/* pps-zoom-action.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2012 Carlos Garcia Campos  <carlosgc@gnome.org>
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
#include <papers-document.h>
#include <papers-view.h>

G_BEGIN_DECLS

#define PPS_TYPE_ZOOM_ACTION            (pps_zoom_action_get_type ())
#define PPS_ZOOM_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PPS_TYPE_ZOOM_ACTION, PpsZoomAction))
#define PPS_IS_ZOOM_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PPS_TYPE_ZOOM_ACTION))
#define PPS_ZOOM_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PPS_TYPE_ZOOM_ACTION, PpsZoomActionClass))
#define PPS_IS_ZOOM_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), PPS_TYPE_ZOOM_ACTION))
#define PPS_ZOOM_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PPS_TYPE_ZOOM_ACTION, PpsZoomActionClass))

typedef struct _PpsZoomAction        PpsZoomAction;
typedef struct _PpsZoomActionClass   PpsZoomActionClass;

struct _PpsZoomAction {
        GtkBox parent;
};

struct _PpsZoomActionClass {
        GtkBoxClass parent_class;
};

GType pps_zoom_action_get_type  (void);
void  pps_zoom_action_set_model (PpsZoomAction *zoom_action,
				 PpsDocumentModel *model);

G_END_DECLS
