/* pps-document-media.c
 *  this file is part of papers, a gnome document_links viewer
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

#include "pps-document-media.h"
#include <config.h>

G_DEFINE_INTERFACE (PpsDocumentMedia, pps_document_media, 0)

static void
pps_document_media_default_init (PpsDocumentMediaInterface *klass)
{
}

PpsMappingList *
pps_document_media_get_media_mapping (PpsDocumentMedia *document_media,
                                      PpsPage *page)
{
	PpsDocumentMediaInterface *iface = PPS_DOCUMENT_MEDIA_GET_IFACE (document_media);

	return iface->get_media_mapping (document_media, page);
}
