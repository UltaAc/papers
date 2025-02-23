/*
 * Copyright (C) 2006 Pauli Virtanen <pav@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#include "djvu-document.h"
#include "pps-document-links.h"

#include <glib.h>

GListModel *djvu_links_get_links_model (PpsDocumentLinks *document_links);
PpsMappingList *djvu_links_get_links (PpsDocumentLinks *document_links,
                                      gint page,
                                      double scale_factor);
PpsLinkDest *djvu_links_find_link_dest (PpsDocumentLinks *document_links,
                                        const gchar *link_name);
gint djvu_links_find_link_page (PpsDocumentLinks *document_links,
                                const gchar *link_name);
gboolean djvu_links_has_document_links (PpsDocumentLinks *document_links);
