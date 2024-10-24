/* pps-document-attachments.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2009 Carlos Garcia Campos  <carlosgc@gnome.org>
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
#include <glib.h>

#include "pps-macros.h"

G_BEGIN_DECLS

#define PPS_TYPE_DOCUMENT_ATTACHMENTS (pps_document_attachments_get_type ())

PPS_PUBLIC
G_DECLARE_INTERFACE (PpsDocumentAttachments, pps_document_attachments, PPS, DOCUMENT_ATTACHMENTS, GObject)

struct _PpsDocumentAttachmentsInterface {
	GTypeInterface base_iface;

	/* Methods  */
	gboolean (*has_attachments) (PpsDocumentAttachments *document_attachments);
	GList *(*get_attachments) (PpsDocumentAttachments *document_attachments);
};

PPS_PUBLIC
gboolean pps_document_attachments_has_attachments (PpsDocumentAttachments *document_attachments);
PPS_PUBLIC
GList *pps_document_attachments_get_attachments (PpsDocumentAttachments *document_attachments);

G_END_DECLS
