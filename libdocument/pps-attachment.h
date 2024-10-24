/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2006 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include <gio/gio.h>
#include <glib-object.h>

#include "pps-macros.h"

G_BEGIN_DECLS

#define PPS_TYPE_ATTACHMENT (pps_attachment_get_type ())

#define PPS_ATTACHMENT_ERROR (pps_attachment_error_quark ())

PPS_PUBLIC
G_DECLARE_DERIVABLE_TYPE (PpsAttachment, pps_attachment, PPS, ATTACHMENT, GObject);

struct _PpsAttachmentClass {
	GObjectClass base_class;
};

PPS_PUBLIC
GQuark pps_attachment_error_quark (void) G_GNUC_CONST;

PPS_PUBLIC
PpsAttachment *pps_attachment_new (const gchar *name,
                                   const gchar *description,
                                   GDateTime *mtime,
                                   GDateTime *ctime,
                                   gsize size,
                                   gpointer data);

PPS_PUBLIC
const gchar *pps_attachment_get_name (PpsAttachment *attachment);
PPS_PUBLIC
const gchar *pps_attachment_get_description (PpsAttachment *attachment);

PPS_PUBLIC
GDateTime *pps_attachment_get_modification_datetime (PpsAttachment *attachment);
PPS_PUBLIC
GDateTime *pps_attachment_get_creation_datetime (PpsAttachment *attachment);

PPS_PUBLIC
const gchar *pps_attachment_get_mime_type (PpsAttachment *attachment);
PPS_PUBLIC
gboolean pps_attachment_save (PpsAttachment *attachment,
                              GFile *file,
                              GError **error);
PPS_PUBLIC
gboolean pps_attachment_open (PpsAttachment *attachment,
                              GAppLaunchContext *context,
                              GError **error);

G_END_DECLS
