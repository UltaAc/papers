/* pps-sidebar-layers.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2008 Carlos Garcia Campos  <carlosgc@gnome.org>
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

#include <glib/gi18n.h>

#include "pps-document-layers.h"
#include "pps-sidebar-page.h"
#include "pps-jobs.h"
#include "pps-job-scheduler.h"
#include "pps-sidebar-layers.h"

struct _PpsSidebarLayersPrivate {
	GtkTreeView  *tree_view;

	PpsDocument   *document;
	PpsJob        *job;
};

enum {
	PROP_0,
	PROP_DOCUMENT_MODEL,
};

enum {
	LAYERS_VISIBILITY_CHANGED,
	N_SIGNALS
};

static void pps_sidebar_layers_page_iface_init (PpsSidebarPageInterface *iface);
static void job_finished_callback             (PpsJobLayers            *job,
					       PpsSidebarLayers        *sidebar_layers);
static void pps_sidebar_layers_set_model (PpsSidebarPage   *sidebar_page,
					 PpsDocumentModel *model);

static guint signals[N_SIGNALS];

G_DEFINE_TYPE_EXTENDED (PpsSidebarLayers,
                        pps_sidebar_layers,
                        GTK_TYPE_BOX,
                        0,
                        G_ADD_PRIVATE (PpsSidebarLayers)
                        G_IMPLEMENT_INTERFACE (PPS_TYPE_SIDEBAR_PAGE,
					       pps_sidebar_layers_page_iface_init))

static void
pps_sidebar_layers_dispose (GObject *object)
{
	PpsSidebarLayers *sidebar = PPS_SIDEBAR_LAYERS (object);

	if (sidebar->priv->job) {
		g_signal_handlers_disconnect_by_func (sidebar->priv->job,
						      job_finished_callback,
						      sidebar);
		pps_job_cancel (sidebar->priv->job);
		g_clear_object (&sidebar->priv->job);
	}

	g_clear_object (&sidebar->priv->document);

	G_OBJECT_CLASS (pps_sidebar_layers_parent_class)->dispose (object);
}

static void
pps_sidebar_layers_set_property (GObject      *object,
			       guint         prop_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
	PpsSidebarLayers *sidebar_layers = PPS_SIDEBAR_LAYERS (object);

	switch (prop_id)
	{
	case PROP_DOCUMENT_MODEL:
		pps_sidebar_layers_set_model (PPS_SIDEBAR_PAGE (sidebar_layers),
			PPS_DOCUMENT_MODEL (g_value_get_object (value)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static GtkTreeModel *
pps_sidebar_layers_create_loading_model (void)
{
	GtkTreeModel *retval;
	GtkTreeIter   iter;
	gchar        *markup;

	/* Creates a fake model to indicate that we're loading */
	retval = (GtkTreeModel *)gtk_list_store_new (PPS_DOCUMENT_LAYERS_N_COLUMNS,
						     G_TYPE_STRING,
						     G_TYPE_OBJECT,
						     G_TYPE_BOOLEAN,
						     G_TYPE_BOOLEAN,
						     G_TYPE_BOOLEAN,
						     G_TYPE_INT);

	gtk_list_store_append (GTK_LIST_STORE (retval), &iter);
	markup = g_strdup_printf ("<span size=\"larger\" style=\"italic\">%s</span>", _("Loading…"));
	gtk_list_store_set (GTK_LIST_STORE (retval), &iter,
			    PPS_DOCUMENT_LAYERS_COLUMN_TITLE, markup,
			    PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE, FALSE,
			    PPS_DOCUMENT_LAYERS_COLUMN_ENABLED, TRUE,
			    PPS_DOCUMENT_LAYERS_COLUMN_SHOWTOGGLE, FALSE,
			    PPS_DOCUMENT_LAYERS_COLUMN_RBGROUP, -1,
			    PPS_DOCUMENT_LAYERS_COLUMN_LAYER, NULL,
			    -1);
	g_free (markup);

	return retval;
}

static gboolean
update_kids (GtkTreeModel *model,
	     GtkTreePath  *path,
	     GtkTreeIter  *iter,
	     GtkTreeIter  *parent)
{
	if (gtk_tree_store_is_ancestor (GTK_TREE_STORE (model), parent, iter)) {
		gboolean visible;

		gtk_tree_model_get (model, parent,
				    PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE, &visible,
				    -1);
		gtk_tree_store_set (GTK_TREE_STORE (model), iter,
				    PPS_DOCUMENT_LAYERS_COLUMN_ENABLED, visible,
				    -1);
	}

	return FALSE;
}

static gboolean
clear_rb_group (GtkTreeModel *model,
		GtkTreePath  *path,
		GtkTreeIter  *iter,
		gint         *rb_group)
{
	gint group;

	gtk_tree_model_get (model, iter,
			    PPS_DOCUMENT_LAYERS_COLUMN_RBGROUP, &group,
			    -1);

	if (group == *rb_group) {
		gtk_tree_store_set (GTK_TREE_STORE (model), iter,
				    PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE, FALSE,
				    -1);
	}

	return FALSE;
}

static void
pps_sidebar_layers_visibility_toggled (GtkCellRendererToggle *cell,
				      gchar                 *path_str,
				      PpsSidebarLayers       *pps_layers)
{
	GtkTreeModel *model;
	GtkTreePath  *path;
	GtkTreeIter   iter;
	gboolean      visible;
	PpsLayer      *layer;

	model = gtk_tree_view_get_model (pps_layers->priv->tree_view);

	path = gtk_tree_path_new_from_string (path_str);
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter,
			    PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE, &visible,
			    PPS_DOCUMENT_LAYERS_COLUMN_LAYER, &layer,
			    -1);

	visible = !visible;
	if (visible) {
		gint rb_group;

		pps_document_layers_show_layer (PPS_DOCUMENT_LAYERS (pps_layers->priv->document),
					       layer);

		rb_group = pps_layer_get_rb_group (layer);
		if (rb_group) {
			gtk_tree_model_foreach (model,
						(GtkTreeModelForeachFunc)clear_rb_group,
						&rb_group);
		}
	} else {
		pps_document_layers_hide_layer (PPS_DOCUMENT_LAYERS (pps_layers->priv->document),
					       layer);
	}

	gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
			    PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE, visible,
			    -1);

	if (pps_layer_is_parent (layer)) {
		gtk_tree_model_foreach (model,
					(GtkTreeModelForeachFunc)update_kids,
					&iter);
	}

	gtk_tree_path_free (path);

	g_signal_emit (pps_layers, signals[LAYERS_VISIBILITY_CHANGED], 0);
}

static GtkTreeView *
pps_sidebar_layers_create_tree_view (PpsSidebarLayers *pps_layers)
{
	GtkTreeView       *tree_view;
	GtkTreeViewColumn *column;
	GtkCellRenderer   *renderer;

	tree_view = GTK_TREE_VIEW (gtk_tree_view_new ());
	gtk_tree_view_set_headers_visible (tree_view, FALSE);
	gtk_tree_selection_set_mode (gtk_tree_view_get_selection (tree_view),
				     GTK_SELECTION_NONE);


	column = gtk_tree_view_column_new ();

	renderer = gtk_cell_renderer_toggle_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "active", PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE,
					     "activatable", PPS_DOCUMENT_LAYERS_COLUMN_ENABLED,
					     "visible", PPS_DOCUMENT_LAYERS_COLUMN_SHOWTOGGLE,
					     "sensitive", PPS_DOCUMENT_LAYERS_COLUMN_ENABLED,
					     NULL);
	g_object_set (G_OBJECT (renderer),
		      "xpad", 0,
		      "ypad", 0,
		      NULL);

	g_signal_connect (renderer, "toggled",
			  G_CALLBACK (pps_sidebar_layers_visibility_toggled),
			  (gpointer)pps_layers);


	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "markup", PPS_DOCUMENT_LAYERS_COLUMN_TITLE,
					     "sensitive", PPS_DOCUMENT_LAYERS_COLUMN_ENABLED,
					     NULL);
	g_object_set (G_OBJECT (renderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);

	gtk_tree_view_append_column (tree_view, column);

	return tree_view;
}

static void
pps_sidebar_layers_init (PpsSidebarLayers *pps_layers)
{
	GtkWidget    *swindow;
	GtkTreeModel *model;

	pps_layers->priv = pps_sidebar_layers_get_instance_private (pps_layers);

	swindow = gtk_scrolled_window_new ();
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swindow),
					GTK_POLICY_NEVER,
					GTK_POLICY_AUTOMATIC);
	gtk_widget_set_vexpand (swindow, TRUE);
	gtk_widget_set_hexpand (swindow, TRUE);

	/* Data Model */
	model = pps_sidebar_layers_create_loading_model ();

	/* Layers list */
	pps_layers->priv->tree_view = pps_sidebar_layers_create_tree_view (pps_layers);
	gtk_tree_view_set_model (pps_layers->priv->tree_view, model);
	g_object_unref (model);

	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (swindow),
		GTK_WIDGET (pps_layers->priv->tree_view));

        gtk_box_prepend (GTK_BOX (pps_layers), swindow);
}

static void
pps_sidebar_layers_class_init (PpsSidebarLayersClass *pps_layers_class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (pps_layers_class);

	g_object_class->set_property = pps_sidebar_layers_set_property;
	g_object_class->dispose = pps_sidebar_layers_dispose;

	g_object_class_override_property (g_object_class, PROP_DOCUMENT_MODEL, "document-model");

	signals[LAYERS_VISIBILITY_CHANGED] =
		g_signal_new ("layers_visibility_changed",
			      G_TYPE_FROM_CLASS (g_object_class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (PpsSidebarLayersClass, layers_visibility_changed),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0, G_TYPE_NONE);
}

GtkWidget *
pps_sidebar_layers_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_SIDEBAR_LAYERS,
				   "orientation", GTK_ORIENTATION_VERTICAL,
                                         NULL));
}

static void
update_layers_state (GtkTreeModel     *model,
		     GtkTreeIter      *iter,
		     PpsDocumentLayers *document_layers)
{
	PpsLayer    *layer;
	gboolean    visible;
	GtkTreeIter child_iter;

	do {
		gtk_tree_model_get (model, iter,
				    PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE, &visible,
				    PPS_DOCUMENT_LAYERS_COLUMN_LAYER, &layer,
				    -1);
		if (layer) {
			gboolean layer_visible;

			layer_visible = pps_document_layers_layer_is_visible (document_layers, layer);
			if (layer_visible != visible) {
				gtk_tree_store_set (GTK_TREE_STORE (model), iter,
						    PPS_DOCUMENT_LAYERS_COLUMN_VISIBLE, layer_visible,
						    -1);
			}
		}

		if (gtk_tree_model_iter_children (model, &child_iter, iter))
			update_layers_state (model, &child_iter, document_layers);
	} while (gtk_tree_model_iter_next (model, iter));
}

void
pps_sidebar_layers_update_layers_state (PpsSidebarLayers *sidebar_layers)
{
	GtkTreeModel     *model;
	GtkTreeIter       iter;
	PpsDocumentLayers *document_layers;

	document_layers = PPS_DOCUMENT_LAYERS (sidebar_layers->priv->document);
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (sidebar_layers->priv->tree_view));
	if (gtk_tree_model_get_iter_first (model, &iter))
		update_layers_state (model, &iter, document_layers);
}

static void
job_finished_callback (PpsJobLayers     *job,
		       PpsSidebarLayers *sidebar_layers)
{
	PpsSidebarLayersPrivate *priv;

	priv = sidebar_layers->priv;

	gtk_tree_view_set_model (GTK_TREE_VIEW (priv->tree_view), job->model);

	g_clear_object (&priv->job);
}

static void
pps_sidebar_layers_document_changed_cb (PpsDocumentModel *model,
				       GParamSpec      *pspec,
				       PpsSidebarLayers *sidebar_layers)
{
	PpsDocument *document = pps_document_model_get_document (model);
	PpsSidebarLayersPrivate *priv = sidebar_layers->priv;

	if (!PPS_IS_DOCUMENT_LAYERS (document))
		return;

	if (priv->document) {
		gtk_tree_view_set_model (GTK_TREE_VIEW (priv->tree_view), NULL);
		g_object_unref (priv->document);
	}

	priv->document = g_object_ref (document);

	if (priv->job) {
		g_signal_handlers_disconnect_by_func (priv->job,
						      job_finished_callback,
						      sidebar_layers);
		g_object_unref (priv->job);
	}

	priv->job = pps_job_layers_new (document);
	g_signal_connect (priv->job, "finished",
			  G_CALLBACK (job_finished_callback),
			  sidebar_layers);
	/* The priority doesn't matter for this job */
	pps_job_scheduler_push_job (priv->job, PPS_JOB_PRIORITY_NONE);
}

static void
pps_sidebar_layers_set_model (PpsSidebarPage   *sidebar_page,
			     PpsDocumentModel *model)
{
	g_signal_connect (model, "notify::document",
			  G_CALLBACK (pps_sidebar_layers_document_changed_cb),
			  sidebar_page);
}

static gboolean
pps_sidebar_layers_support_document (PpsSidebarPage *sidebar_page,
				    PpsDocument    *document)
{
	return (PPS_IS_DOCUMENT_LAYERS (document) &&
		pps_document_layers_has_layers (PPS_DOCUMENT_LAYERS (document)));
}

static void
pps_sidebar_layers_page_iface_init (PpsSidebarPageInterface *iface)
{
	iface->support_document = pps_sidebar_layers_support_document;
	iface->set_model = pps_sidebar_layers_set_model;
}
