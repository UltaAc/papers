/* pps-document-links.h
 *  this file is part of papers, a gnome document_links viewer
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

#include "config.h"

#include "pps-document-links.h"

G_DEFINE_INTERFACE (PpsDocumentLinks, pps_document_links, 0)

static void
pps_document_links_default_init (PpsDocumentLinksInterface *klass)
{
}

gboolean
pps_document_links_has_document_links (PpsDocumentLinks *document_links)
{
	PpsDocumentLinksInterface *iface = PPS_DOCUMENT_LINKS_GET_IFACE (document_links);
	gboolean retval;

	retval = iface->has_document_links (document_links);

	return retval;
}

/**
 * pps_document_links_get_links_model:
 * @document_links: an #PpsDocumentLinks
 *
 * Returns: (transfer full): a #GListModel
 */
GListModel *
pps_document_links_get_links_model (PpsDocumentLinks *document_links)
{
	PpsDocumentLinksInterface *iface = PPS_DOCUMENT_LINKS_GET_IFACE (document_links);

	return iface->get_links_model (document_links);
}

PpsMappingList *
pps_document_links_get_links (PpsDocumentLinks *document_links,
			     PpsPage          *page)
{
	PpsDocumentLinksInterface *iface = PPS_DOCUMENT_LINKS_GET_IFACE (document_links);

	return iface->get_links (document_links, page);
}

/**
 * pps_document_links_find_link_dest:
 * @document_links: an #PpsDocumentLinks
 * @link_name: the link name
 *
 * Returns: (transfer full): an #PpsLinkDest
 */
PpsLinkDest *
pps_document_links_find_link_dest (PpsDocumentLinks *document_links,
				  const gchar     *link_name)
{
	PpsDocumentLinksInterface *iface = PPS_DOCUMENT_LINKS_GET_IFACE (document_links);
	PpsLinkDest *retval;

	pps_document_doc_mutex_lock ();
	retval = iface->find_link_dest (document_links, link_name);
	pps_document_doc_mutex_unlock ();

	return retval;
}

gint
pps_document_links_find_link_page (PpsDocumentLinks *document_links,
				  const gchar     *link_name)
{
	PpsDocumentLinksInterface *iface = PPS_DOCUMENT_LINKS_GET_IFACE (document_links);
	gint retval;

	pps_document_doc_mutex_lock ();
	retval = iface->find_link_page (document_links, link_name);
	pps_document_doc_mutex_unlock ();

	return retval;
}

/* Helper functions */
gint
pps_document_links_get_dest_page (PpsDocumentLinks *document_links,
				 PpsLinkDest      *dest)
{
	gint page = -1;

	switch (pps_link_dest_get_dest_type (dest)) {
	case PPS_LINK_DEST_TYPE_NAMED: {
		page = pps_document_links_find_link_page (document_links,
							 pps_link_dest_get_named_dest (dest));
	}
		break;
	case PPS_LINK_DEST_TYPE_PAGE_LABEL:
		pps_document_find_page_by_label (PPS_DOCUMENT (document_links),
						pps_link_dest_get_page_label (dest),
						&page);
		break;
	default:
		page = pps_link_dest_get_page (dest);
	}

	return page;
}

gchar *
pps_document_links_get_dest_page_label (PpsDocumentLinks *document_links,
				       PpsLinkDest      *dest)
{
	gchar *label = NULL;

	if (pps_link_dest_get_dest_type (dest) == PPS_LINK_DEST_TYPE_PAGE_LABEL) {
		label = g_strdup (pps_link_dest_get_page_label (dest));
	} else {
		gint page;

		page = pps_document_links_get_dest_page (document_links, dest);
		if (page != -1)
			label = pps_document_get_page_label (PPS_DOCUMENT (document_links),
							    page);
	}

	return label;
}

static PpsLinkDest *
get_link_dest (PpsLink *link)
{
	PpsLinkAction *action;

	action = pps_link_get_action (link);
	if (!action)
		return NULL;

	if (pps_link_action_get_action_type (action) !=
	    PPS_LINK_ACTION_TYPE_GOTO_DEST)
		return NULL;

	return pps_link_action_get_dest (action);
}

gint
pps_document_links_get_link_page (PpsDocumentLinks *document_links,
				 PpsLink          *link)
{
	PpsLinkDest *dest;

	dest = get_link_dest (link);

	return dest ? pps_document_links_get_dest_page (document_links, dest) : -1;
}

gchar *
pps_document_links_get_link_page_label (PpsDocumentLinks *document_links,
				       PpsLink          *link)
{
	PpsLinkDest *dest;

	dest = get_link_dest (link);

	return dest ? pps_document_links_get_dest_page_label (document_links, dest) : NULL;
}
