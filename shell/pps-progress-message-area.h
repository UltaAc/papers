/* pps-progress-message-area.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2008 Carlos Garcia Campos <carlosgc@gnome.org>
 * Copyright (C) 2018 Germán Poo-Caamaño <gpoo@gnome.org>
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

#include "pps-message-area.h"

G_BEGIN_DECLS

#define PPS_TYPE_PROGRESS_MESSAGE_AREA (pps_progress_message_area_get_type ())
#define PPS_PROGRESS_MESSAGE_AREA(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PPS_TYPE_PROGRESS_MESSAGE_AREA, PpsProgressMessageArea))
#define PPS_PROGRESS_MESSAGE_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), PPS_TYPE_PROGRESS_MESSAGE_AREA, PpsProgressMessageAreaClass))
#define PPS_IS_PROGRESS_MESSAGE_AREA(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PPS_TYPE_PROGRESS_MESSAGE_AREA))
#define PPS_IS_PROGRESS_MESSAGE_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PPS_TYPE_PROGRESS_MESSAGE_AREA))
#define PPS_PROGRESS_MESSAGE_AREA_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), PPS_TYPE_PROGRESS_MESSAGE_AREA, PpsProgressMessageAreaClass))

typedef struct _PpsProgressMessageArea PpsProgressMessageArea;
typedef struct _PpsProgressMessageAreaClass PpsProgressMessageAreaClass;

struct _PpsProgressMessageArea {
	PpsMessageArea parent_instance;
};

struct _PpsProgressMessageAreaClass {
	PpsMessageAreaClass parent_class;
};

GType pps_progress_message_area_get_type (void) G_GNUC_CONST;
GtkWidget *pps_progress_message_area_new (const gchar *icon_name,
                                          const gchar *text);
void pps_progress_message_area_set_status (PpsProgressMessageArea *area,
                                           const gchar *str);
void pps_progress_message_area_set_fraction (PpsProgressMessageArea *area,
                                             gdouble fraction);

G_END_DECLS
