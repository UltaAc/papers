/* pps-document-links.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2004 Red Hat, Inc.
 *
 * Author:
 *   Jonathan Blandford <jrb@alum.mit.edu>
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
#include <gtk/gtk.h>

#include "pps-document.h"
#include "pps-link.h"
#include "pps-macros.h"
#include "pps-mapping-list.h"

G_BEGIN_DECLS

#define PPS_TYPE_DOCUMENT_LINKS (pps_document_links_get_type ())

PPS_PUBLIC
G_DECLARE_INTERFACE (PpsDocumentLinks, pps_document_links, PPS, DOCUMENT_LINKS, GObject)

enum {
	PPS_DOCUMENT_LINKS_COLUMN_MARKUP,
	PPS_DOCUMENT_LINKS_COLUMN_LINK,
	PPS_DOCUMENT_LINKS_COLUMN_EXPAND,
	PPS_DOCUMENT_LINKS_COLUMN_PAGE_LABEL,
	PPS_DOCUMENT_LINKS_COLUMN_NUM_COLUMNS
};

struct _PpsDocumentLinksInterface {
	GTypeInterface base_iface;

	/* Methods  */
	gboolean (*has_document_links) (PpsDocumentLinks *document_links);
	GListModel *(*get_links_model) (PpsDocumentLinks *document_links);
	PpsMappingList *(*get_links) (PpsDocumentLinks *document_links,
	                              PpsPage *page);
	PpsLinkDest *(*find_link_dest) (PpsDocumentLinks *document_links,
	                                const gchar *link_name);
	gint (*find_link_page) (PpsDocumentLinks *document_links,
	                        const gchar *link_name);
};

PPS_PUBLIC
gboolean pps_document_links_has_document_links (PpsDocumentLinks *document_links);
PPS_PUBLIC
GListModel *pps_document_links_get_links_model (PpsDocumentLinks *document_links);

PPS_PUBLIC
PpsMappingList *pps_document_links_get_links (PpsDocumentLinks *document_links,
                                              PpsPage *page);
PPS_PUBLIC
PpsLinkDest *pps_document_links_find_link_dest (PpsDocumentLinks *document_links,
                                                const gchar *link_name);
PPS_PUBLIC
gint pps_document_links_find_link_page (PpsDocumentLinks *document_links,
                                        const gchar *link_name);
PPS_PUBLIC
gint pps_document_links_get_dest_page (PpsDocumentLinks *document_links,
                                       PpsLinkDest *dest);
PPS_PUBLIC
gchar *pps_document_links_get_dest_page_label (PpsDocumentLinks *document_links,
                                               PpsLinkDest *dest);
PPS_PUBLIC
gint pps_document_links_get_link_page (PpsDocumentLinks *document_links,
                                       PpsLink *link);
PPS_PUBLIC
gchar *pps_document_links_get_link_page_label (PpsDocumentLinks *document_links,
                                               PpsLink *link);

G_END_DECLS
