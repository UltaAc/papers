/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2009 Juanjo Marín <juanj.marin@juntadeandalucia.es>
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

#include "config.h"

#include <string.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "pps-properties-license.h"


struct _PpsPropertiesLicense {
	AdwBin base_instance;

	GtkWidget *license;
	GtkWidget *uri;
	GtkWidget *web_statement;

	GtkWidget *license_box;
	GtkWidget *uri_box;
	GtkWidget *web_statement_box;
};

struct _PpsPropertiesLicenseClass {
	AdwBinClass base_class;
};

G_DEFINE_TYPE (PpsPropertiesLicense, pps_properties_license, ADW_TYPE_BIN)


static void
set_uri_to_label (GtkLabel    *label,
		  const gchar *uri)
{
	gchar     *checked_uri;
	gchar     *markup;

	checked_uri = g_uri_parse_scheme (uri);
	if (checked_uri) {
		markup = g_markup_printf_escaped ("<a href=\"%s\">%s</a>", uri, uri);
		gtk_label_set_markup (label, markup);
		g_free (markup);
		g_free (checked_uri);
	} else {
		gtk_label_set_text (label, uri);
	}
}

void
pps_properties_license_set_license (PpsPropertiesLicense *properties,
				   PpsDocumentLicense   *license)
{
	const gchar *text = pps_document_license_get_text (license);
	const gchar *uri = pps_document_license_get_uri (license);
	const gchar *web_statement = pps_document_license_get_web_statement (license);

	gtk_widget_set_visible (properties->license_box, !!text);
	gtk_widget_set_visible (properties->uri_box, !!uri);
	gtk_widget_set_visible (properties->license_box, !!web_statement);

	if (text) {
		gtk_label_set_text (GTK_LABEL (properties->license), text);
	}

	if (uri) {
		set_uri_to_label (GTK_LABEL (properties->uri), uri);
	}

	if (web_statement) {
		set_uri_to_label (GTK_LABEL (properties->web_statement), web_statement);
	}
}

static void
pps_properties_license_init (PpsPropertiesLicense *properties_license)
{
	gtk_widget_init_template (GTK_WIDGET (properties_license));
}

static void
pps_properties_license_class_init (PpsPropertiesLicenseClass *properties_license_class)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (properties_license_class);

	gtk_widget_class_set_template_from_resource (widget_class,
		"/org/gnome/papers/ui/properties-license.ui");

	gtk_widget_class_bind_template_child (widget_class, PpsPropertiesLicense, license);
	gtk_widget_class_bind_template_child (widget_class, PpsPropertiesLicense, uri);
	gtk_widget_class_bind_template_child (widget_class, PpsPropertiesLicense, web_statement);

	gtk_widget_class_bind_template_child (widget_class, PpsPropertiesLicense, license_box);
	gtk_widget_class_bind_template_child (widget_class, PpsPropertiesLicense, uri_box);
	gtk_widget_class_bind_template_child (widget_class, PpsPropertiesLicense, web_statement_box);
}

GtkWidget *
pps_properties_license_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_PROPERTIES_LICENSE, NULL));
}
