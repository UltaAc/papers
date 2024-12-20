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

#define PPS_TYPE_PROGRESS_MESSAGE_AREA (pps_progress_message_area_get_type ())
G_DECLARE_FINAL_TYPE(PpsProgressMessageArea, pps_progress_message_area, PPS, PROGRESS_MESSAGE_AREA, AdwBin)

struct _PpsProgressMessageArea {
	AdwBin parent_instance;
};

GtkWidget *pps_progress_message_area_new (const gchar *icon_name, const gchar *text);
void pps_progress_message_area_set_image_from_icon_name (PpsProgressMessageArea *area,
                                                const gchar *icon_name);
void pps_progress_message_area_set_text (PpsProgressMessageArea *area,
                                const gchar *str);
void pps_progress_message_area_set_secondary_text (PpsProgressMessageArea *area,
                                          const gchar *str);

void pps_progress_message_area_add_button (PpsProgressMessageArea *area,
                                  const gchar *first_button_text,
                                  gint response_id);
GtkInfoBar *pps_progress_message_area_get_info_bar (PpsProgressMessageArea *area);

void pps_progress_message_area_set_status (PpsProgressMessageArea *area,
                                           const gchar *str);
void pps_progress_message_area_set_fraction (PpsProgressMessageArea *area,
                                             gdouble fraction);

G_END_DECLS
