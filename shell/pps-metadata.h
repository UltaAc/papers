/* pps-metadata.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2009 Carlos Garcia Campos  <carlosgc@gnome.org>
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

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define PPS_TYPE_METADATA         (pps_metadata_get_type())
#define PPS_METADATA(object)      (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_METADATA, PpsMetadata))
#define PPS_METADATA_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_METADATA, PpsMetadataClass))
#define PPS_IS_METADATA(object)   (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_METADATA))

typedef struct _PpsMetadata      PpsMetadata;
typedef struct _PpsMetadataClass PpsMetadataClass;

GType       pps_metadata_get_type              (void) G_GNUC_CONST;
PpsMetadata *pps_metadata_new                   (GFile       *file);
gboolean    pps_metadata_is_empty              (PpsMetadata  *metadata);

gboolean    pps_metadata_get_string            (PpsMetadata  *metadata,
					       const gchar *key,
					       gchar     **value);
gboolean    pps_metadata_set_string            (PpsMetadata  *metadata,
					       const gchar *key,
					       const gchar *value);
gboolean    pps_metadata_get_int               (PpsMetadata  *metadata,
					       const gchar *key,
					       gint        *value);
gboolean    pps_metadata_set_int               (PpsMetadata  *metadata,
					       const gchar *key,
					       gint         value);
gboolean    pps_metadata_get_double            (PpsMetadata  *metadata,
					       const gchar *key,
					       gdouble     *value);
gboolean    pps_metadata_set_double            (PpsMetadata  *metadata,
					       const gchar *key,
					       gdouble      value);
gboolean    pps_metadata_get_boolean           (PpsMetadata  *metadata,
					       const gchar *key,
					       gboolean    *value);
gboolean    pps_metadata_set_boolean           (PpsMetadata  *metadata,
					       const gchar *key,
					       gboolean     value);
gboolean    pps_metadata_has_key               (PpsMetadata  *metadata,
                                               const gchar *key);

gboolean    pps_is_metadata_supported_for_file (GFile       *file);

G_END_DECLS
