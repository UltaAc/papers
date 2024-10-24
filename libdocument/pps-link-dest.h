/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2006 Carlos Garcia Campos <carlosgc@gnome.org>
 *  Copyright (C) 2005 Red Hat, Inc.
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

G_BEGIN_DECLS

#define PPS_TYPE_LINK_DEST (pps_link_dest_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsLinkDest, pps_link_dest, PPS, LINK_DEST, GObject)

typedef enum {
	PPS_LINK_DEST_TYPE_PAGE,
	PPS_LINK_DEST_TYPE_XYZ,
	PPS_LINK_DEST_TYPE_FIT,
	PPS_LINK_DEST_TYPE_FITH,
	PPS_LINK_DEST_TYPE_FITV,
	PPS_LINK_DEST_TYPE_FITR,
	PPS_LINK_DEST_TYPE_NAMED,
	PPS_LINK_DEST_TYPE_PAGE_LABEL,
	PPS_LINK_DEST_TYPE_UNKNOWN
} PpsLinkDestType;

PPS_PUBLIC
PpsLinkDestType pps_link_dest_get_dest_type (PpsLinkDest *self);
PPS_PUBLIC
gint pps_link_dest_get_page (PpsLinkDest *self);
PPS_PUBLIC
gdouble pps_link_dest_get_top (PpsLinkDest *self,
                               gboolean *change_top);
PPS_PUBLIC
gdouble pps_link_dest_get_left (PpsLinkDest *self,
                                gboolean *change_left);
PPS_PUBLIC
gdouble pps_link_dest_get_bottom (PpsLinkDest *self);
PPS_PUBLIC
gdouble pps_link_dest_get_right (PpsLinkDest *self);
PPS_PUBLIC
gdouble pps_link_dest_get_zoom (PpsLinkDest *self,
                                gboolean *change_zoom);
PPS_PUBLIC
const gchar *pps_link_dest_get_named_dest (PpsLinkDest *self);
PPS_PUBLIC
const gchar *pps_link_dest_get_page_label (PpsLinkDest *self);

PPS_PUBLIC
PpsLinkDest *pps_link_dest_new_page (gint page);
PPS_PUBLIC
PpsLinkDest *pps_link_dest_new_xyz (gint page,
                                    gdouble left,
                                    gdouble top,
                                    gdouble zoom,
                                    gboolean change_left,
                                    gboolean change_top,
                                    gboolean change_zoom);
PPS_PUBLIC
PpsLinkDest *pps_link_dest_new_fit (gint page);
PPS_PUBLIC
PpsLinkDest *pps_link_dest_new_fith (gint page,
                                     gdouble top,
                                     gboolean change_top);
PPS_PUBLIC
PpsLinkDest *pps_link_dest_new_fitv (gint page,
                                     gdouble left,
                                     gboolean change_left);
PPS_PUBLIC
PpsLinkDest *pps_link_dest_new_fitr (gint page,
                                     gdouble left,
                                     gdouble bottom,
                                     gdouble right,
                                     gdouble top);
PPS_PUBLIC
PpsLinkDest *pps_link_dest_new_named (const gchar *named_dest);
PPS_PUBLIC
PpsLinkDest *pps_link_dest_new_page_label (const gchar *page_label);

PPS_PUBLIC
gboolean pps_link_dest_equal (PpsLinkDest *a,
                              PpsLinkDest *b);

G_END_DECLS
