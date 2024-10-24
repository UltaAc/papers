/*
 *  Copyright (C) 2005, Red Hat, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#pragma once

#if !defined(__PPS_PAPERS_DOCUMENT_H_INSIDE__) && !defined(PAPERS_COMPILATION)
#error "Only <papers-document.h> can be included directly."
#endif

#include <gtk/gtk.h>

#include "pps-document.h"
#include "pps-macros.h"

G_BEGIN_DECLS

gboolean _pps_document_factory_init (void);
void _pps_document_factory_shutdown (void);

PPS_PUBLIC
PpsDocument *pps_document_factory_get_document (const char *uri, GError **error);
PPS_PUBLIC
PpsDocument *pps_document_factory_get_document_full (const char *uri,
                                                     PpsDocumentLoadFlags flags,
                                                     GError **error);
PPS_PUBLIC
PpsDocument *pps_document_factory_get_document_for_fd (int fd,
                                                       const char *mime_type,
                                                       PpsDocumentLoadFlags flags,
                                                       GCancellable *cancellable,
                                                       GError **error);

PPS_PUBLIC
void pps_document_factory_add_filters (GtkFileDialog *dialog, PpsDocument *document);

G_END_DECLS
