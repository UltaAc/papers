/* pps-mapping.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2009 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include "pps-document.h"
#include "pps-macros.h"

G_BEGIN_DECLS

typedef struct _PpsMappingList PpsMappingList;

#define PPS_TYPE_MAPPING_LIST (pps_mapping_list_get_type ())
PPS_PUBLIC
GType pps_mapping_list_get_type (void) G_GNUC_CONST;

PPS_PUBLIC
PpsMappingList *pps_mapping_list_new (guint page,
                                      GList *list,
                                      GDestroyNotify data_destroy_func);
PPS_PUBLIC
PpsMappingList *pps_mapping_list_ref (PpsMappingList *mapping_list);
PPS_PUBLIC
void pps_mapping_list_unref (PpsMappingList *mapping_list);

PPS_PUBLIC
guint pps_mapping_list_get_page (PpsMappingList *mapping_list);
PPS_PUBLIC
GList *pps_mapping_list_get_list (PpsMappingList *mapping_list);
PPS_PUBLIC
void pps_mapping_list_remove (PpsMappingList *mapping_list,
                              PpsMapping *mapping);
PPS_PUBLIC
PpsMapping *pps_mapping_list_find (PpsMappingList *mapping_list,
                                   gconstpointer data);
PPS_PUBLIC
PpsMapping *pps_mapping_list_find_custom (PpsMappingList *mapping_list,
                                          gconstpointer data,
                                          GCompareFunc func);
PPS_PUBLIC
PpsMapping *pps_mapping_list_get (PpsMappingList *mapping_list,
                                  gdouble x,
                                  gdouble y);
PPS_PUBLIC
gpointer pps_mapping_list_get_data (PpsMappingList *mapping_list,
                                    gdouble x,
                                    gdouble y);
PPS_PUBLIC
PpsMapping *pps_mapping_list_nth (PpsMappingList *mapping_list,
                                  guint n);
PPS_PUBLIC
guint pps_mapping_list_length (PpsMappingList *mapping_list);

G_END_DECLS
