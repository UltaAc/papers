/* pps-sidebar-links.h
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

#include "pps-document.h"
#include "pps-link.h"
#include "pps-utils.h"

G_BEGIN_DECLS

typedef struct _PpsSidebarLinks PpsSidebarLinks;
typedef struct _PpsSidebarLinksClass PpsSidebarLinksClass;
typedef struct _PpsSidebarLinksPrivate PpsSidebarLinksPrivate;

#define PPS_TYPE_SIDEBAR_LINKS		   (pps_sidebar_links_get_type())
#define PPS_SIDEBAR_LINKS(object)	   (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_SIDEBAR_LINKS, PpsSidebarLinks))
#define PPS_SIDEBAR_LINKS_CLASS(klass)	   (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_SIDEBAR_LINKS, PpsSidebarLinksClass))
#define PPS_IS_SIDEBAR_LINKS(object)	   (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_SIDEBAR_LINKS))
#define PPS_IS_SIDEBAR_LINKS_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_SIDEBAR_LINKS))
#define PPS_SIDEBAR_LINKS_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_SIDEBAR_LINKS, PpsSidebarLinksClass))

struct _PpsSidebarLinks {
	GtkBox base_instance;

	PpsSidebarLinksPrivate *priv;
};

struct _PpsSidebarLinksClass {
	GtkBoxClass base_class;

	void    (* link_activated) (PpsSidebarLinks *sidebar_links,
				    PpsLink         *link);
};

GType      pps_sidebar_links_get_type       (void);
GtkWidget *pps_sidebar_links_new            (void);

G_END_DECLS
