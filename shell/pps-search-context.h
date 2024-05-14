/* pps-search-context.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2015 Igalia S.L.
 * Copyright (C) 2024 Markus Göllnitz  <camelcasenick@bewares.it>
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

#include "pps-document-model.h"
#include "pps-metadata.h"

G_BEGIN_DECLS

#define PPS_TYPE_SEARCH_CONTEXT    (pps_search_context_get_type())
G_DECLARE_FINAL_TYPE (PpsSearchContext, pps_search_context, PPS, SEARCH_CONTEXT, GObject)

struct _PpsSearchContext {
        GObject parent_instance;
};

struct _PpsSearchContextClass {
	GObjectClass parent_class;
};

GType             pps_search_context_get_type    (void);
PpsSearchContext *pps_search_context_new         (PpsDocumentModel *model);
const gchar*      pps_search_context_get_search_term (PpsSearchContext *context);
void              pps_search_context_set_search_term (PpsSearchContext *context,
						      const gchar      *search_term);
PpsFindOptions    pps_search_context_get_options (PpsSearchContext *context);
void              pps_search_context_set_options (PpsSearchContext *context,
					          PpsFindOptions    options);
void              pps_search_context_restart     (PpsSearchContext *context);

G_END_DECLS
