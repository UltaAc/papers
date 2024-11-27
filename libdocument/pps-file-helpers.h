/*
 *  Copyright (C) 2002 Jorn Baayen
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

#include <gio/gio.h>
#include <glib.h>

#include "pps-macros.h"

G_BEGIN_DECLS

typedef enum {
	PPS_COMPRESSION_NONE,
	PPS_COMPRESSION_BZIP2,
	PPS_COMPRESSION_GZIP,
	PPS_COMPRESSION_LZMA
} PpsCompressionType;

void _pps_file_helpers_init (void);

void _pps_file_helpers_shutdown (void);

PPS_PUBLIC
int pps_mkstemp (const char *tmpl,
                 char **file_name,
                 GError **error);
PPS_PUBLIC
GFile *pps_mkstemp_file (const char *tmpl,
                         GError **error);
PPS_PUBLIC
void pps_tmp_filename_unlink (const gchar *filename);
PPS_PUBLIC
void pps_tmp_file_unlink (GFile *file);
PPS_PUBLIC
void pps_tmp_uri_unlink (const gchar *uri);
PPS_PUBLIC
gboolean pps_file_is_temp (GFile *file);
PPS_PUBLIC
gboolean pps_xfer_uri_simple (const char *from,
                              const char *to,
                              GError **error);
PPS_PUBLIC
gboolean pps_file_copy_metadata (const char *from,
                                 const char *to,
                                 GError **error);

PPS_PUBLIC
gchar *pps_file_get_mime_type (const gchar *uri,
                               gboolean fast,
                               GError **error);

PPS_PUBLIC
gchar *pps_file_get_mime_type_from_fd (int fd,
                                       GError **error);

PPS_PUBLIC
gchar *pps_file_uncompress (const gchar *uri,
                            PpsCompressionType type,
                            GError **error);
PPS_PUBLIC
gchar *pps_file_compress (const gchar *uri,
                          PpsCompressionType type,
                          GError **error);

G_END_DECLS
