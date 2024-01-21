/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2022 Qiu Wenbo
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
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PPS_TYPE_THUMBNAIL_ITEM              (pps_thumbnail_item_get_type ())
G_DECLARE_FINAL_TYPE (PpsThumbnailItem, pps_thumbnail_item, PPS, THUMBNAIL_ITEM, GObject);

struct _PpsThumbnailItem
{
        GObject parent;
};

void pps_thumbnail_item_set_primary_text (PpsThumbnailItem *pps_thumbnail_item,
					 const gchar *primary_text);
void pps_thumbnail_item_set_secondary_text (PpsThumbnailItem *pps_thumbnail_item,
					   const gchar *secondary_text);
void pps_thumbnail_item_set_paintable (PpsThumbnailItem *pps_thumbnail_item, GdkPaintable *paintable);

G_END_DECLS
