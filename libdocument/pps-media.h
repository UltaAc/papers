/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2015 Igalia S.L.
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

#include <glib-object.h>

#include "pps-macros.h"
#include "pps-page.h"

G_BEGIN_DECLS

#define PPS_TYPE_MEDIA (pps_media_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsMedia, pps_media, PPS, MEDIA, GObject)

struct _PpsMedia {
	GObject base_instance;
};

PPS_PUBLIC
PpsMedia *pps_media_new_for_uri (PpsPage *page,
                                 const gchar *uri);
PPS_PUBLIC
const gchar *pps_media_get_uri (PpsMedia *media);
PPS_PUBLIC
guint pps_media_get_page_index (PpsMedia *media);
PPS_PUBLIC
gboolean pps_media_get_show_controls (PpsMedia *media);
PPS_PUBLIC
void pps_media_set_show_controls (PpsMedia *media,
                                  gboolean show_controls);

G_END_DECLS
