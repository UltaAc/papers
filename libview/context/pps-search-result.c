/* pps-search-result.c
 *  this file is part of papers, a gnome document viewer
 *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pps-search-result.h"
#include <string.h>

typedef struct
{
	gchar *markup;
	gchar *label;
	guint page;
	guint index;
} PpsSearchResultPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PpsSearchResult, pps_search_result, G_TYPE_OBJECT)

#define GET_PRIVATE(o) pps_search_result_get_instance_private (o)

static void
pps_search_result_init (PpsSearchResult *self)
{
}

static void
pps_search_result_finalize (GObject *object)
{
	PpsSearchResult *self = PPS_SEARCH_RESULT (object);
	PpsSearchResultPrivate *private = GET_PRIVATE (self);

	g_clear_pointer (&private->markup, g_free);
	g_clear_pointer (&private->label, g_free);

	G_OBJECT_CLASS (pps_search_result_parent_class)->finalize (object);
}

static void
pps_search_result_class_init (PpsSearchResultClass *result_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (result_class);

	object_class->finalize = pps_search_result_finalize;
}

PpsSearchResult *
pps_search_result_new (gchar *markup,
                       gchar *label,
                       guint page,
                       guint index)
{
	PpsSearchResult *result = g_object_new (PPS_TYPE_SEARCH_RESULT, NULL);
	PpsSearchResultPrivate *private = GET_PRIVATE (result);

	private->markup = markup;
	private->label = label;
	private->page = page;
	private->index = index;

	return result;
}

const gchar *
pps_search_result_get_markup (PpsSearchResult *self)
{
	PpsSearchResultPrivate *private = GET_PRIVATE (self);

	return private->markup;
}

const gchar *
pps_search_result_get_label (PpsSearchResult *self)
{
	PpsSearchResultPrivate *private = GET_PRIVATE (self);

	return private->label;
}

guint
pps_search_result_get_page (PpsSearchResult *self)
{
	PpsSearchResultPrivate *private = GET_PRIVATE (self);

	return private->page;
}

guint
pps_search_result_get_index (PpsSearchResult *self)
{
	PpsSearchResultPrivate *private = GET_PRIVATE (self);

	return private->index;
}
