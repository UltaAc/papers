/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2006 Carlos Garcia Campos <carlosgc@gnome.org>
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

#if !defined(__PPS_PAPERS_DOCUMENT_H_INSIDE__) && !defined(PAPERS_COMPILATION)
#error "Only <papers-document.h> can be included directly."
#endif

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib-object.h>

#include "pps-macros.h"

G_BEGIN_DECLS

#define PPS_TYPE_IMAGE (pps_image_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsImage, pps_image, PPS, IMAGE, GObject)

struct _PpsImage {
	GObject base_instance;
};

PPS_PUBLIC
PpsImage *pps_image_new (gint page,
                         gint img_id);
PPS_PUBLIC
PpsImage *pps_image_new_from_pixbuf (GdkPixbuf *pixbuf);

PPS_PUBLIC
gint pps_image_get_id (PpsImage *image);
PPS_PUBLIC
gint pps_image_get_page (PpsImage *image);
PPS_PUBLIC
GdkPixbuf *pps_image_get_pixbuf (PpsImage *image);
PPS_PUBLIC
const gchar *pps_image_save_tmp (PpsImage *image,
                                 GdkPixbuf *pixbuf);
PPS_PUBLIC
const gchar *pps_image_get_tmp_uri (PpsImage *image);

G_END_DECLS
