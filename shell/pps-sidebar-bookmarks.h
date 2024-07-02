/* pps-sidebar-bookmarks.h
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

#include "pps-sidebar-page.h"
#include <papers-view.h>

G_BEGIN_DECLS

typedef struct _PpsSidebarBookmarks        PpsSidebarBookmarks;
typedef struct _PpsSidebarBookmarksClass   PpsSidebarBookmarksClass;
typedef struct _PpsSidebarBookmarksPrivate PpsSidebarBookmarksPrivate;

#define PPS_TYPE_SIDEBAR_BOOKMARKS              (pps_sidebar_bookmarks_get_type())
#define PPS_SIDEBAR_BOOKMARKS(object)           (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_SIDEBAR_BOOKMARKS, PpsSidebarBookmarks))
#define PPS_SIDEBAR_BOOKMARKS_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_SIDEBAR_BOOKMARKS, PpsSidebarBookmarksClass))
#define PPS_IS_SIDEBAR_BOOKMARKS(object)        (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_SIDEBAR_BOOKMARKS))
#define PPS_IS_SIDEBAR_BOOKMARKS_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_SIDEBAR_BOOKMARKS))
#define PPS_SIDEBAR_BOOKMARKS_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_SIDEBAR_BOOKMARKS, PpsSidebarBookmarksClass))

struct _PpsSidebarBookmarks {
	PpsSidebarPage base_instance;
};

struct _PpsSidebarBookmarksClass {
	PpsSidebarPageClass base_class;

        void (*add_bookmark) (PpsSidebarBookmarks *sidebar_bookmarks);
        void (*activated)    (PpsSidebarBookmarks *sidebar_bookmarks,
                              gint                old_page,
                              gint                page);
};

GType      pps_sidebar_bookmarks_get_type      (void) G_GNUC_CONST;
GtkWidget *pps_sidebar_bookmarks_new           (void);
void       pps_sidebar_bookmarks_set_bookmarks (PpsSidebarBookmarks *sidebar_bookmarks,
					       PpsBookmarks        *bookmarks);
G_END_DECLS
