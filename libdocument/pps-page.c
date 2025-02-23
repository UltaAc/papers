/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2008 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include <config.h>

#include "pps-page.h"

G_DEFINE_TYPE (PpsPage, pps_page, G_TYPE_OBJECT)

static void
pps_page_init (PpsPage *page)
{
}

static void
pps_page_finalize (GObject *object)
{
	PpsPage *page = PPS_PAGE (object);

	if (page->backend_destroy_func) {
		page->backend_destroy_func (page->backend_page);
		page->backend_destroy_func = NULL;
	}
	page->backend_page = NULL;

	(*G_OBJECT_CLASS (pps_page_parent_class)->finalize) (object);
}

static void
pps_page_class_init (PpsPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = pps_page_finalize;
}

PpsPage *
pps_page_new (gint index)
{
	PpsPage *page;

	page = PPS_PAGE (g_object_new (PPS_TYPE_PAGE, NULL));
	page->index = index;

	return page;
}
