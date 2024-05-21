/* pps-sidebar-annotations.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2010 Carlos Garcia Campos  <carlosgc@gnome.org>
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

typedef struct _PpsSidebarAnnotationsPrivate PpsSidebarAnnotationsPrivate;

#define PPS_TYPE_SIDEBAR_ANNOTATIONS              (pps_sidebar_annotations_get_type())
G_DECLARE_DERIVABLE_TYPE (PpsSidebarAnnotations, pps_sidebar_annotations, PPS, SIDEBAR_ANNOTATIONS, GtkBox);

struct _PpsSidebarAnnotationsClass {
	GtkBoxClass base_class;

	void    (* annot_activated)     (PpsSidebarAnnotations *sidebar_annots,
					 PpsMapping            *mapping);
};

GtkWidget *pps_sidebar_annotations_new           (void);
void       pps_sidebar_annotations_annot_added   (PpsSidebarAnnotations *sidebar_annots,
					         PpsAnnotation         *annot);
void       pps_sidebar_annotations_annot_removed (PpsSidebarAnnotations *sidebar_annots);

G_END_DECLS
