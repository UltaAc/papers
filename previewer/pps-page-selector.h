/*
 *  Copyright (C) 2003, 2004 Marco Pesenti Gritti
 *  Copyright (C) 2003, 2004 Christian Persch
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

#include <papers-view.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PPS_TYPE_PAGE_SELECTOR (pps_page_selector_get_type ())
#define PPS_PAGE_SELECTOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PPS_TYPE_PAGE_SELECTOR, PpsPageSelector))

typedef struct _PpsPageSelector PpsPageSelector;
typedef struct _PpsPageSelectorClass PpsPageSelectorClass;

struct _PpsPageSelectorClass {
	GtkBoxClass parent_class;

	void (*activate_link) (PpsPageSelector *page_action,
	                       PpsLink *link);
};

GType pps_page_selector_get_type (void) G_GNUC_CONST;

void pps_page_selector_set_model (PpsPageSelector *page_selector,
                                  PpsDocumentModel *doc_model);

G_END_DECLS
