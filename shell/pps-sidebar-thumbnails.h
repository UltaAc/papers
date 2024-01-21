/* pps-sidebar-thumbnails.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2004 Red Hat, Inc.
 *
 * Author:
 *   Jonathan Blandford <jrb@alum.mit.edu>
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

G_BEGIN_DECLS

typedef struct _PpsSidebarThumbnails PpsSidebarThumbnails;
typedef struct _PpsSidebarThumbnailsClass PpsSidebarThumbnailsClass;
typedef struct _PpsSidebarThumbnailsPrivate PpsSidebarThumbnailsPrivate;

#define PPS_TYPE_SIDEBAR_THUMBNAILS		(pps_sidebar_thumbnails_get_type())
#define PPS_SIDEBAR_THUMBNAILS(object)		(G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_SIDEBAR_THUMBNAILS, PpsSidebarThumbnails))
#define PPS_SIDEBAR_THUMBNAILS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_SIDEBAR_THUMBNAILS, PpsSidebarThumbnailsClass))
#define PPS_IS_SIDEBAR_THUMBNAILS(object)	(G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_SIDEBAR_THUMBNAILS))
#define PPS_IS_SIDEBAR_THUMBNAILS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_SIDEBAR_THUMBNAILS))
#define PPS_SIDEBAR_THUMBNAILS_GET_CLASS(object)	(G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_SIDEBAR_THUMBNAILS, PpsSidebarThumbnailsClass))

struct _PpsSidebarThumbnails {
	GtkBox base_instance;

	PpsSidebarThumbnailsPrivate *priv;
};

struct _PpsSidebarThumbnailsClass {
	GtkBoxClass base_class;
};

GType      pps_sidebar_thumbnails_get_type     (void) G_GNUC_CONST;
GtkWidget *pps_sidebar_thumbnails_new          (void);

G_END_DECLS
