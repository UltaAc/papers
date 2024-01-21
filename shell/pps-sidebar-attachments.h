/* pps-sidebar-attachments.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2006 Carlos Garcia Campos
 *
 * Author:
 *   Carlos Garcia Campos <carlosgc@gnome.org>
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

#include "pps-attachment.h"
#include "pps-document.h"

G_BEGIN_DECLS

typedef struct _PpsSidebarAttachmentsPrivate PpsSidebarAttachmentsPrivate;

#define PPS_TYPE_SIDEBAR_ATTACHMENTS              (pps_sidebar_attachments_get_type())
G_DECLARE_DERIVABLE_TYPE (PpsSidebarAttachments, pps_sidebar_attachments, PPS, SIDEBAR_ATTACHMENTS, GtkBox);

struct _PpsSidebarAttachmentsClass {
	GtkBoxClass base_class;

	/* Signals */
	void (*popup_menu)      (PpsSidebarAttachments *pps_attachbar,
			         PpsAttachment         *attachment);
	void (*save_attachment) (PpsSidebarAttachments *pps_attachbar,
			         PpsAttachment         *attachment,
	                         const char          *uri);
};

GtkWidget *pps_sidebar_attachments_new          (void);

G_END_DECLS
