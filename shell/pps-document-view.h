/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2004 Martin Kretzschmar
 *
 *  Author:
 *    Martin Kretzschmar <martink@gnome.org>
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

#include <glib.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "pps-link.h"
#include "pps-metadata.h"
#include "pps-annotation.h"
#include "pps-document-model.h"

G_BEGIN_DECLS

#define PPS_TYPE_DOCUMENT_VIEW		(pps_document_view_get_type())
G_DECLARE_FINAL_TYPE (PpsDocumentView, pps_document_view, PPS, DOCUMENT_VIEW, AdwBreakpointBin)

struct _PpsDocumentView {
	AdwBreakpointBin base_instance;
};

PpsDocumentView       *pps_document_view_new                            (void);
const char     *pps_document_view_get_uri                        (PpsDocumentView       *pps_doc_view);
void		pps_document_view_open_document (PpsDocumentView  *pps_doc_view,
						 PpsDocument *document,
						 PpsMetadata *metadata,
						 PpsLinkDest *dest);
gboolean	pps_document_view_is_empty	                 (PpsDocumentView       *pps_doc_view);
void		pps_document_view_print_range                    (PpsDocumentView       *pps_doc_view,
                                                          int             first_page,
                                                          int		 last_page);
const gchar    *pps_document_view_get_dbus_object_path           (PpsDocumentView       *pps_doc_view);
void            pps_document_view_focus_view                     (PpsDocumentView       *pps_doc_view);
AdwHeaderBar   *pps_document_view_get_header_bar                 (PpsDocumentView       *pps_doc_view);
void            pps_document_view_handle_annot_popup             (PpsDocumentView       *pps_doc_view,
                                                          PpsAnnotation   *annot);
PpsMetadata     *pps_document_view_get_metadata			 (PpsDocumentView	 *pps_doc_view);
void		pps_document_view_set_document 		(PpsDocumentView *pps_doc_view,
							 PpsDocument *document);
void		pps_document_view_reload_document 	(PpsDocumentView *pps_doc_view,
							 PpsDocument *document);
gboolean	pps_document_view_close_handled 	(PpsDocumentView *pps_doc_view);
void		pps_document_view_set_filenames		(PpsDocumentView *pps_doc_view,
							 const gchar      *display_name,
							 const gchar      *edit_name);
PpsDocumentModel *pps_document_view_get_model 		(PpsDocumentView *pps_doc_view);
void		pps_document_view_set_fullscreen_mode 	(PpsDocumentView *pps_doc_view,
				       			 gboolean         fullscreened);
void		pps_document_view_set_inverted_colors 	(PpsDocumentView *pps_doc_view,
				       			 gboolean         inverted);

G_END_DECLS
