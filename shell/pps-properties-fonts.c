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
#include "pps-job-scheduler.h"
#include "pps-jobs.h"
#include "pps-properties-fonts.h"

struct _PpsPropertiesFonts {
	GtkBox base_instance;

	GtkTreeView *fonts_treeview;
	GtkWidget *fonts_summary;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	PpsJob     *fonts_job;

	PpsDocument *document;
};

struct _PpsPropertiesFontsClass {
	GtkBoxClass base_class;
};

static void
job_fonts_finished_cb (PpsJob *job, PpsPropertiesFonts *properties);

G_DEFINE_TYPE (PpsPropertiesFonts, pps_properties_fonts, GTK_TYPE_BOX)

static void
pps_properties_fonts_dispose (GObject *object)
{
	PpsPropertiesFonts *properties = PPS_PROPERTIES_FONTS (object);

	if (properties->fonts_job) {
		g_signal_handlers_disconnect_by_func (properties->fonts_job,
						      job_fonts_finished_cb,
						      properties);
		pps_job_cancel (properties->fonts_job);

		g_clear_object (&properties->fonts_job);
	}

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
	gtk_widget_class_bind_template_child (widget_class, PpsPropertiesFonts, column);
	gtk_widget_class_bind_template_child (widget_class, PpsPropertiesFonts, renderer);
	gtk_widget_class_bind_template_child (widget_class,
					      PpsPropertiesFonts, fonts_treeview);
	gtk_widget_class_bind_template_child (widget_class,
					      PpsPropertiesFonts, fonts_summary);
}

static void
font_cell_data_func (GtkTreeViewColumn *col, GtkCellRenderer *renderer,
		     GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
	char *name;
	char *details;
	char *markup;

	gtk_tree_model_get (model, iter,
			    PPS_DOCUMENT_FONTS_COLUMN_NAME, &name,
			    PPS_DOCUMENT_FONTS_COLUMN_DETAILS, &details,
			    -1);

	if (details) {
		markup = g_strdup_printf ("<b><big>Font: %s</big></b>\n<small>%s</small>",
					  name, details);
	} else {
		markup = g_strdup_printf ("<b><big>%s</big></b>", name);
	}

	g_object_set (renderer, "markup", markup, NULL, NULL);

	g_free (markup);
	g_free (details);
	g_free (name);
}

static void
pps_properties_fonts_init (PpsPropertiesFonts *properties)
{
	GtkWidget *widget = GTK_WIDGET (properties);

	gtk_widget_init_template (widget);
	gtk_tree_view_column_set_cell_data_func (properties->column,
						 properties->renderer,
						 font_cell_data_func,
						 NULL, NULL);
}

static void
job_fonts_finished_cb (PpsJob *job, PpsPropertiesFonts *properties)
{
	PpsDocumentFonts *document_fonts = PPS_DOCUMENT_FONTS (properties->document);
	GtkTreeModel *model;
	const gchar     *font_summary;

	g_signal_handlers_disconnect_by_func (job, job_fonts_finished_cb, properties);
	g_clear_object (&properties->fonts_job);

	model = gtk_tree_view_get_model (properties->fonts_treeview);
	pps_document_fonts_fill_model (document_fonts, model);

	font_summary = pps_document_fonts_get_fonts_summary (document_fonts);
	if (font_summary) {
		gtk_label_set_text (GTK_LABEL (properties->fonts_summary),
				    font_summary);
		/* show the label only when fonts are scanned, so the label
		 * does not take space while it is loading */
		gtk_widget_set_visible (properties->fonts_summary, TRUE);
	}
}

void
pps_properties_fonts_set_document (PpsPropertiesFonts *properties,
				  PpsDocument        *document)
{
	GtkTreeView *tree_view = properties->fonts_treeview;
	GtkListStore *list_store;

	properties->document = document;

	list_store = gtk_list_store_new (PPS_DOCUMENT_FONTS_COLUMN_NUM_COLUMNS,
					 G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model (tree_view, GTK_TREE_MODEL (list_store));

	properties->fonts_job = pps_job_fonts_new (properties->document);
	g_signal_connect (properties->fonts_job, "finished",
			  G_CALLBACK (job_fonts_finished_cb),
			  properties);
	pps_job_scheduler_push_job (properties->fonts_job, PPS_JOB_PRIORITY_NONE);
}

GtkWidget *
pps_properties_fonts_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_PROPERTIES_FONTS, NULL));
}
