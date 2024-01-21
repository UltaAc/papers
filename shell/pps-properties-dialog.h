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

typedef struct _PpsPropertiesDialog PpsPropertiesDialog;
typedef struct _PpsPropertiesDialogClass PpsPropertiesDialogClass;
typedef struct _PpsPropertiesDialogPrivate PpsPropertiesDialogPrivate;

#define PPS_TYPE_PROPERTIES_DIALOG		(pps_properties_dialog_get_type())
#define PPS_PROPERTIES_DIALOG(object)	        (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_PROPERTIES_DIALOG, PpsPropertiesDialog))
#define PPS_PROPERTIES_DIALOG_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_PROPERTIES_DIALOG, PpsPropertiesDialogClass))
#define PPS_IS_PROPERTIES_DIALOG(object)		(G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_PROPERTIES_DIALOG))
#define PPS_IS_PROPERTIES_DIALOG_CLASS(klass)  	(G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_PROPERTIES_DIALOG))
#define PPS_PROPERTIES_DIALOG_GET_CLASS(object)	(G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_PROPERTIES_DIALOG, PpsPropertiesDialogClass))

GType		 pps_properties_dialog_get_type     (void);
GtkWidget	*pps_properties_dialog_new          (void);
void	         pps_properties_dialog_set_document (PpsPropertiesDialog *properties,
					  	    PpsDocument         *document);

G_END_DECLS
