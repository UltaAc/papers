/* pps-bookmarks.h
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

#include <glib-object.h>

#include "pps-metadata.h"

G_BEGIN_DECLS

#define PPS_TYPE_BOOKMARKS         (pps_bookmarks_get_type())
#define PPS_BOOKMARKS(object)      (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_BOOKMARKS, PpsBookmarks))
#define PPS_BOOKMARKS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_BOOKMARKS, PpsBookmarksClass))
#define PPS_IS_BOOKMARKS(object)   (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_BOOKMARKS))

typedef struct _PpsBookmarks      PpsBookmarks;
typedef struct _PpsBookmarksClass PpsBookmarksClass;

typedef struct _PpsBookmark {
        guint  page;
        gchar *title;
} PpsBookmark;

GType        pps_bookmarks_get_type      (void) G_GNUC_CONST;
PpsBookmarks *pps_bookmarks_new           (PpsMetadata *metadata);
GList       *pps_bookmarks_get_bookmarks (PpsBookmarks *bookmarks);
gboolean     pps_bookmarks_has_bookmarks (PpsBookmarks *bookmarks);
void         pps_bookmarks_add           (PpsBookmarks *bookmarks,
                                         PpsBookmark  *bookmark);
void         pps_bookmarks_delete        (PpsBookmarks *bookmarks,
                                         PpsBookmark  *bookmark);
void         pps_bookmarks_update        (PpsBookmarks *bookmarks,
                                         PpsBookmark  *bookmark);

G_END_DECLS
