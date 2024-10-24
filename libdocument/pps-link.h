/* this file is part of papers, a gnome document viewer
 *
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

#include "pps-document.h"
#include "pps-link-action.h"
#include "pps-macros.h"

G_BEGIN_DECLS

#define PPS_TYPE_LINK (pps_link_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsLink, pps_link, PPS, LINK, GObject)

PPS_PUBLIC
PpsLink *pps_link_new (const gchar *title,
                       PpsLinkAction *action);

PPS_PUBLIC
const gchar *pps_link_get_title (PpsLink *self);
PPS_PUBLIC
PpsLinkAction *pps_link_get_action (PpsLink *self);

G_END_DECLS
