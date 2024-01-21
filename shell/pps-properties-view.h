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

#include <gtk/gtk.h>

#include <papers-document.h>

G_BEGIN_DECLS

typedef struct _PpsPropertiesView PpsPropertiesView;
typedef struct _PpsPropertiesViewClass PpsPropertiesViewClass;
typedef struct _PpsPropertiesViewPrivate PpsPropertiesViewPrivate;

#define PPS_TYPE_PROPERTIES			(pps_properties_view_get_type())
#define PPS_PROPERTIES_VIEW(object)	        (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_PROPERTIES, PpsPropertiesView))
#define PPS_PROPERTIES_VIEW_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_PROPERTIES, PpsPropertiesViewClass))
#define PPS_IS_PROPERTIES_VIEW(object)		(G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_PROPERTIES))
#define PPS_IS_PROPERTIES_VIEW_CLASS(klass)   	(G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_PROPERTIES))
#define PPS_PROPERTIES_VIEW_GET_CLASS(object) 	(G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_PROPERTIES, PpsPropertiesViewClass))

GType		pps_properties_view_get_type		(void);
void		pps_properties_view_register_type	(GTypeModule *module);

GtkWidget      *pps_properties_view_new			(PpsDocument           *document);
void		pps_properties_view_set_info		(PpsPropertiesView     *properties,
							 const PpsDocumentInfo *info);



G_END_DECLS
