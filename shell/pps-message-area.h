/* pps-message-area.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2007 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include <adwaita.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PPS_TYPE_MESSAGE_AREA (pps_message_area_get_type ())
#define PPS_MESSAGE_AREA(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PPS_TYPE_MESSAGE_AREA, PpsMessageArea))
#define PPS_MESSAGE_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), PPS_TYPE_MESSAGE_AREA, PpsMessageAreaClass))
#define PPS_IS_MESSAGE_AREA(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PPS_TYPE_MESSAGE_AREA))
#define PPS_IS_MESSAGE_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PPS_TYPE_MESSAGE_AREA))
#define PPS_MESSAGE_AREA_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), PPS_TYPE_MESSAGE_AREA, PpsMessageAreaClass))

typedef struct _PpsMessageArea PpsMessageArea;
typedef struct _PpsMessageAreaClass PpsMessageAreaClass;

struct _PpsMessageArea {
	AdwBin parent_instance;
};

struct _PpsMessageAreaClass {
	AdwBinClass parent_class;
};

GType pps_message_area_get_type (void) G_GNUC_CONST;
GtkWidget *pps_message_area_new (GtkMessageType type,
                                 const gchar *text,
                                 const gchar *first_button_text,
                                 ...);
void pps_message_area_set_image (PpsMessageArea *area,
                                 GtkWidget *image);
void pps_message_area_set_image_from_icon_name (PpsMessageArea *area,
                                                const gchar *icon_name);
void pps_message_area_set_text (PpsMessageArea *area,
                                const gchar *str);
void pps_message_area_set_secondary_text (PpsMessageArea *area,
                                          const gchar *str);

void pps_message_area_add_button (PpsMessageArea *area,
                                  const gchar *first_button_text,
                                  gint response_id);
GtkWidget *_pps_message_area_get_main_box (PpsMessageArea *area);
GtkInfoBar *pps_message_area_get_info_bar (PpsMessageArea *area);

G_END_DECLS
