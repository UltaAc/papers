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

#include "pps-link-dest.h"
#include "pps-macros.h"

G_BEGIN_DECLS

#define PPS_TYPE_LINK_ACTION (pps_link_action_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsLinkAction, pps_link_action, PPS, LINK_ACTION, GObject)

typedef enum {
	PPS_LINK_ACTION_TYPE_GOTO_DEST,
	PPS_LINK_ACTION_TYPE_GOTO_REMOTE,
	PPS_LINK_ACTION_TYPE_EXTERNAL_URI,
	PPS_LINK_ACTION_TYPE_LAUNCH,
	PPS_LINK_ACTION_TYPE_NAMED,
	PPS_LINK_ACTION_TYPE_LAYERS_STATE,
	PPS_LINK_ACTION_TYPE_RESET_FORM
	/* We'll probably fill this in more as we support the other types of
	 * actions */
} PpsLinkActionType;

PPS_PUBLIC
PpsLinkActionType pps_link_action_get_action_type (PpsLinkAction *self);
PPS_PUBLIC
PpsLinkDest *pps_link_action_get_dest (PpsLinkAction *self);
PPS_PUBLIC
const gchar *pps_link_action_get_uri (PpsLinkAction *self);
PPS_PUBLIC
const gchar *pps_link_action_get_filename (PpsLinkAction *self);
PPS_PUBLIC
const gchar *pps_link_action_get_params (PpsLinkAction *self);
PPS_PUBLIC
const gchar *pps_link_action_get_name (PpsLinkAction *self);
PPS_PUBLIC
GList *pps_link_action_get_show_list (PpsLinkAction *self);
PPS_PUBLIC
GList *pps_link_action_get_hide_list (PpsLinkAction *self);
PPS_PUBLIC
GList *pps_link_action_get_toggle_list (PpsLinkAction *self);
PPS_PUBLIC
GList *pps_link_action_get_reset_fields (PpsLinkAction *self);
PPS_PUBLIC
gboolean pps_link_action_get_exclude_reset_fields (PpsLinkAction *self);

PPS_PUBLIC
PpsLinkAction *pps_link_action_new_dest (PpsLinkDest *dest);
PPS_PUBLIC
PpsLinkAction *pps_link_action_new_remote (PpsLinkDest *dest,
                                           const gchar *filename);
PPS_PUBLIC
PpsLinkAction *pps_link_action_new_external_uri (const gchar *uri);
PPS_PUBLIC
PpsLinkAction *pps_link_action_new_launch (const gchar *filename,
                                           const gchar *params);
PPS_PUBLIC
PpsLinkAction *pps_link_action_new_named (const gchar *name);
PPS_PUBLIC
PpsLinkAction *pps_link_action_new_layers_state (GList *show_list,
                                                 GList *hide_list,
                                                 GList *toggle_list);
PPS_PUBLIC
PpsLinkAction *pps_link_action_new_reset_form (GList *fields,
                                               gboolean exclude_fields);

PPS_PUBLIC
gboolean pps_link_action_equal (PpsLinkAction *a,
                                PpsLinkAction *b);

G_END_DECLS
