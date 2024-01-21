/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2005 Red Hat, Inc
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

#include "pps-document.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _PpsPropertiesFonts PpsPropertiesFonts;
typedef struct _PpsPropertiesFontsClass PpsPropertiesFontsClass;
typedef struct _PpsPropertiesFontsPrivate PpsPropertiesFontsPrivate;

#define PPS_TYPE_PROPERTIES_FONTS		(pps_properties_fonts_get_type())
#define PPS_PROPERTIES_FONTS(object)	        (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_PROPERTIES_FONTS, PpsPropertiesFonts))
#define PPS_PROPERTIES_FONTS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_PROPERTIES_FONTS, PpsPropertiesFontsClass))
#define PPS_IS_PROPERTIES(object)		(G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_PROPERTIES_FONTS))
#define PPS_IS_PROPERTIES_CLASS(klass)   	(G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_PROPERTIES_FONTS))
#define PPS_PROPERTIES_FONTS_GET_CLASS(object)	(G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_PROPERTIES_FONTS, PpsPropertiesFontsClass))

GType	           pps_properties_fonts_get_type     (void);
GtkWidget         *pps_properties_fonts_new          (void);
void		   pps_properties_fonts_set_document (PpsPropertiesFonts *properties,
					  	     PpsDocument        *document);

G_END_DECLS
