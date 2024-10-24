/* pps-document-media.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2015 Igalia S.L.
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

#include "pps-document.h"
#include "pps-macros.h"
#include "pps-mapping-list.h"
#include "pps-media.h"

G_BEGIN_DECLS

#define PPS_TYPE_DOCUMENT_MEDIA (pps_document_media_get_type ())

PPS_PUBLIC
G_DECLARE_INTERFACE (PpsDocumentMedia, pps_document_media, PPS, DOCUMENT_MEDIA, GObject)

struct _PpsDocumentMediaInterface {
	GTypeInterface base_iface;

	/* Methods  */
	PpsMappingList *(*get_media_mapping) (PpsDocumentMedia *document_media,
	                                      PpsPage *page);
};

PPS_PUBLIC
PpsMappingList *pps_document_media_get_media_mapping (PpsDocumentMedia *document_media,
                                                      PpsPage *page);

G_END_DECLS
