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
#include <adwaita.h>

#include "pps-document-fonts.h"
#include "pps-job-scheduler.h"
#include "pps-jobs.h"
#include "pps-properties-fonts.h"

typedef struct {
	GtkWidget *list_box;
	GtkWidget *fonts_summary;
	PpsJob    *fonts_job;

	GListModel* model;

	PpsDocument *document;
} PpsPropertiesFontsPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PpsPropertiesFonts, pps_properties_fonts, ADW_TYPE_BIN)

#define GET_PRIVATE(o) pps_properties_fonts_get_instance_private (o)

static void
job_fonts_finished_cb (PpsJob *job, PpsPropertiesFonts *properties);

static void
pps_properties_fonts_dispose (GObject *object)
{
	PpsPropertiesFonts *properties = PPS_PROPERTIES_FONTS (object);
	PpsPropertiesFontsPrivate *priv = GET_PRIVATE (properties);

	if (priv->fonts_job) {
		g_signal_handlers_disconnect_by_func (priv->fonts_job,
						      job_fonts_finished_cb,
						      properties);
		pps_job_cancel (priv->fonts_job);

		g_clear_object (&priv->fonts_job);
	}

	g_clear_object (&priv->document);
	g_clear_object (&priv->model);

	G_OBJECT_CLASS (pps_properties_fonts_parent_class)->dispose (object);
}

static void
pps_properties_fonts_class_init (PpsPropertiesFontsClass *properties_class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (properties_class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (properties_class);

	g_object_class->dispose = pps_properties_fonts_dispose;

	gtk_widget_class_set_template_from_resource (widget_class,
				"/org/gnome/papers/ui/properties-fonts.ui");
	gtk_widget_class_bind_template_child_private (widget_class,
						      PpsPropertiesFonts, list_box);
	gtk_widget_class_bind_template_child_private (widget_class,
						      PpsPropertiesFonts, fonts_summary);
}

static void
pps_properties_fonts_init (PpsPropertiesFonts *properties)
{
	GtkWidget *widget = GTK_WIDGET (properties);

	gtk_widget_init_template (widget);
}

static GtkWidget *
font_create_row_func (gpointer item, gpointer user_data)
{
	PpsFontDescription *font_description = PPS_FONT_DESCRIPTION (item);
	GtkWidget *row = adw_action_row_new ();

	g_object_bind_property (font_description, "name", row, "title",
				G_BINDING_SYNC_CREATE);
	g_object_bind_property (font_description, "details", row, "subtitle",
				G_BINDING_SYNC_CREATE);

	return row;
}

static void
job_fonts_finished_cb (PpsJob *job, PpsPropertiesFonts *properties)
{
	PpsPropertiesFontsPrivate *priv = GET_PRIVATE (properties);
	PpsDocumentFonts *document_fonts = PPS_DOCUMENT_FONTS (priv->document);
	const gchar     *font_summary;

	g_signal_handlers_disconnect_by_func (job, job_fonts_finished_cb, properties);
	g_clear_object (&priv->fonts_job);

	priv->model = pps_document_fonts_get_model (document_fonts);
	gtk_list_box_bind_model (GTK_LIST_BOX (priv->list_box),
				 priv->model, font_create_row_func,
				 NULL, NULL);
	font_summary = pps_document_fonts_get_fonts_summary (document_fonts);
	gtk_label_set_text (GTK_LABEL (priv->fonts_summary), font_summary);
}

/**
 * pps_properties_fonts_set_document:
 * @properties: a #PpsPropertiesFonts
 * @document: (transfer full): a #PpsDocument which implements the
 * #PpsDocumentFonts interface
 *
 * Sets the document on the properties to the fonts can be scanned.
 */
void
pps_properties_fonts_set_document (PpsPropertiesFonts *properties,
				   PpsDocument        *document)
{
	PpsPropertiesFontsPrivate *priv = GET_PRIVATE (properties);

	if (document == priv->document)
		return;

	g_clear_object (&priv->document);
	priv->document = g_object_ref (document);

	priv->fonts_job = pps_job_fonts_new (priv->document);
	g_signal_connect (priv->fonts_job, "finished",
			  G_CALLBACK (job_fonts_finished_cb),
			  properties);
	pps_job_scheduler_push_job (priv->fonts_job, PPS_JOB_PRIORITY_NONE);
}

GtkWidget *
pps_properties_fonts_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_PROPERTIES_FONTS, NULL));
}
