/* pps-sidebar.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2004 Red Hat, Inc.
 * Copyright (C) 2018 Germán Poo-Caamaño <gpoo@gnome.org>
 *
 * Author:
 *   Jonathan Blandford <jrb@alum.mit.edu>
 *   Germán Poo-Caamaño <gpoo@gnome.org>
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

#include <adwaita.h>

#include "pps-document-model.h"

G_BEGIN_DECLS

#define PPS_TYPE_SIDEBAR		     (pps_sidebar_get_type())
G_DECLARE_FINAL_TYPE (PpsSidebar, pps_sidebar, PPS, SIDEBAR, AdwBin);

struct _PpsSidebar {
	AdwBin base_instance;
};

GtkWidget *pps_sidebar_new       (void);
void       pps_sidebar_set_model (PpsSidebar       *pps_sidebar,
                                 PpsDocumentModel *model);
GtkWidget *pps_sidebar_get_current_page (PpsSidebar *pps_sidebar);
const char *pps_sidebar_get_visible_child_name (PpsSidebar *pps_sidebar);
void	   pps_sidebar_set_visible_child_name (PpsSidebar *pps_sidebar,
					      const char *name);

G_END_DECLS
