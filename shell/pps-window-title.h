/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2005 Red Hat, Inc
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

#include "pps-document-view.h"
#include "pps-document.h"

G_BEGIN_DECLS

typedef struct _PpsDocumentViewTitle PpsDocumentViewTitle;

PpsDocumentViewTitle *pps_document_view_title_new	    (PpsDocumentView *window);
void           pps_document_view_title_set_document (PpsDocumentViewTitle     *window_title,
					     PpsDocument        *document);
void	       pps_document_view_title_set_filename (PpsDocumentViewTitle     *window_title,
					     const char        *filename);
void	       pps_document_view_title_free         (PpsDocumentViewTitle     *window_title);

G_END_DECLS
