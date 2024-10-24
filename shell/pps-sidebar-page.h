/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/*
 *  Copyright (C) 2005 Marco Pesenti Gritti
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

#include <adwaita.h>
#include <glib-object.h>
#include <glib.h>

#include "pps-document.h"
#include <papers-view.h>

G_BEGIN_DECLS

#define PPS_TYPE_SIDEBAR_PAGE (pps_sidebar_page_get_type ())
G_DECLARE_DERIVABLE_TYPE (PpsSidebarPage, pps_sidebar_page, PPS, SIDEBAR_PAGE, AdwBin)

struct _PpsSidebarPageClass {
	AdwBinClass parent_class;

	/* Methods  */
	gboolean (*support_document) (PpsSidebarPage *sidebar_page,
	                              PpsDocument *document);
};

gboolean pps_sidebar_page_support_document (PpsSidebarPage *sidebar_page,
                                            PpsDocument *document);
PpsDocumentModel *pps_sidebar_page_get_document_model (PpsSidebarPage *sidebar_page);
void pps_sidebar_page_navigate_to_view (PpsSidebarPage *sidebar_page);

G_END_DECLS
