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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "pps-document-fonts.h"
#include "pps-properties-dialog.h"
#include "pps-properties-fonts.h"
#include "pps-properties-view.h"
#include "pps-properties-license.h"

struct _PpsPropertiesDialog {
	GtkDialog base_instance;

	PpsDocument *document;
	GtkWidget *notebook;
	GtkWidget *general_page;
	GtkWidget *fonts_page;
	GtkWidget *license_page;
};

struct _PpsPropertiesDialogClass {
	GtkDialogClass base_class;
};

G_DEFINE_TYPE (PpsPropertiesDialog, pps_properties_dialog, GTK_TYPE_DIALOG)

static void
pps_properties_dialog_class_init (PpsPropertiesDialogClass *properties_class)
{
}

static void
pps_properties_dialog_init (PpsPropertiesDialog *properties)
{
	GtkBox *content_area;

	content_area = GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (properties)));

	gtk_window_set_title (GTK_WINDOW (properties), _("Properties"));
	gtk_window_set_destroy_with_parent (GTK_WINDOW (properties), TRUE);

	properties->notebook = gtk_notebook_new ();
	gtk_notebook_set_show_border (GTK_NOTEBOOK (properties->notebook), FALSE);
	gtk_box_prepend (content_area, properties->notebook);
}

void
pps_properties_dialog_set_document (PpsPropertiesDialog *properties,
			           PpsDocument         *document)
{
	GtkWidget *label;
	const PpsDocumentInfo *info;

	properties->document = document;

	info = pps_document_get_info (document);

	if (properties->general_page == NULL) {
		label = gtk_label_new (_("General"));
		properties->general_page = pps_properties_view_new (document);
		gtk_notebook_append_page (GTK_NOTEBOOK (properties->notebook),
					  properties->general_page, label);
	}
	pps_properties_view_set_info (PPS_PROPERTIES_VIEW (properties->general_page), info);

	if (PPS_IS_DOCUMENT_FONTS (document)) {
		if (properties->fonts_page == NULL) {
			label = gtk_label_new (_("Fonts"));
			properties->fonts_page = pps_properties_fonts_new ();
			gtk_notebook_append_page (GTK_NOTEBOOK (properties->notebook),
						  properties->fonts_page, label);
		}

		pps_properties_fonts_set_document
			(PPS_PROPERTIES_FONTS (properties->fonts_page), document);
	}

	if (info->fields_mask & PPS_DOCUMENT_INFO_LICENSE && info->license) {
		if (properties->license_page == NULL) {
			label = gtk_label_new (_("Document License"));
			properties->license_page = pps_properties_license_new ();
			gtk_notebook_append_page (GTK_NOTEBOOK (properties->notebook),
						  properties->license_page, label);
		}

		pps_properties_license_set_license
			(PPS_PROPERTIES_LICENSE (properties->license_page), info->license);
	}
}

GtkWidget *
pps_properties_dialog_new (void)
{
	PpsPropertiesDialog *properties;

	properties = g_object_new (PPS_TYPE_PROPERTIES_DIALOG,
				   "use-header-bar", TRUE, NULL);

	return GTK_WIDGET (properties);
}
