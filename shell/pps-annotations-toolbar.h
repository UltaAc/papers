/* pps-annotations-toolbar.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2015 Carlos Garcia Campos  <carlosgc@gnome.org>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PPS_TYPE_ANNOTATIONS_TOOLBAR              (pps_annotations_toolbar_get_type())
G_DECLARE_FINAL_TYPE (PpsAnnotationsToolbar, pps_annotations_toolbar, PPS, ANNOTATIONS_TOOLBAR, GtkBox);

GtkWidget *pps_annotations_toolbar_new                (void);
void       pps_annotations_toolbar_add_annot_finished (PpsAnnotationsToolbar *toolbar);

G_END_DECLS
