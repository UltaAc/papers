/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2024 Qiu Wenbo
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
#include <papers-document.h>

G_BEGIN_DECLS

#define PPS_TYPE_OUTLINES (pps_outlines_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsOutlines, pps_outlines, PPS, OUTLINES, GObject);

struct _PpsOutlines {
	GObject parent;
};

PPS_PUBLIC
PpsOutlines *pps_outlines_new (void);

PPS_PUBLIC
void pps_outlines_set_markup (PpsOutlines *pps_outlines, const gchar *markup);
PPS_PUBLIC
void pps_outlines_set_label (PpsOutlines *pps_outlines, const gchar *label);
PPS_PUBLIC
void pps_outlines_set_expand (PpsOutlines *pps_outlines, gboolean expand);
PPS_PUBLIC
void pps_outlines_set_link (PpsOutlines *pps_outlines, PpsLink *link);
PPS_PUBLIC
void pps_outlines_set_children (PpsOutlines *pps_outlines, GListModel *children);

PPS_PUBLIC
PpsLink *pps_outlines_get_link (PpsOutlines *pps_outlines);
PPS_PUBLIC
GListModel *pps_outlines_get_children (PpsOutlines *pps_outlines);
PPS_PUBLIC
gboolean pps_outlines_get_expand (PpsOutlines *pps_outlines);

G_END_DECLS
