/* pps-sidebar-annotations.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2010 Carlos Garcia Campos  <carlosgc@gnome.org>
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
#include <adwaita.h>

#include <pps-document.h>
#include "pps-document-annotations.h"
#include "pps-sidebar-annotations.h"
#include "pps-jobs.h"
#include "pps-job-scheduler.h"
#include "pps-document-view.h"

enum {
	ANNOT_ACTIVATED,
	N_SIGNALS
};

struct _PpsSidebarAnnotationsPrivate {
	GtkWidget   *list_view;
	GtkWidget   *stack;

	GtkWidget   *popup;

	GListStore  *model;

	PpsJob      *job;
};

static void pps_sidebar_annotations_load            (PpsSidebarAnnotations   *sidebar_annots);
static void job_finished_callback (PpsJobAnnots          *job,
				   PpsSidebarAnnotations *sidebar_annots);
static guint signals[N_SIGNALS];

G_DEFINE_TYPE_WITH_PRIVATE (PpsSidebarAnnotations, pps_sidebar_annotations, PPS_TYPE_SIDEBAR_PAGE)

#define GET_PRIVATE(t) (pps_sidebar_annotations_get_instance_private (t))

#define ANNOT_ICON_SIZE 16

static void
sidebar_annots_button_press_cb (GtkGestureClick *gesture,
				gint		 n_press,
				gdouble		 x,
				gdouble		 y,
				GtkListItem     *item)
{
	GtkWidget *row = gtk_list_item_get_child (item);
	GtkWidget *sidebar_annots = gtk_widget_get_ancestor (row, PPS_TYPE_SIDEBAR_ANNOTATIONS);
	PpsSidebarAnnotationsPrivate *priv = GET_PRIVATE (PPS_SIDEBAR_ANNOTATIONS (sidebar_annots));
	PpsAnnotation *annotation = PPS_ANNOTATION (gtk_list_item_get_item (item));
	guint button = gtk_gesture_single_get_current_button (GTK_GESTURE_SINGLE (gesture));
	PpsMapping *mapping = g_new (PpsMapping, 1);
	PpsDocumentView *window;
	graphene_point_t sidebar_annots_point;

	g_return_if_fail (annotation);

	mapping->data = annotation;
	pps_annotation_get_area (annotation, &mapping->area);

	switch (button) {
		case GDK_BUTTON_PRIMARY:
			g_signal_emit (sidebar_annots, signals[ANNOT_ACTIVATED], 0, mapping);
			break;
		case GDK_BUTTON_SECONDARY:
			window = PPS_DOCUMENT_VIEW (gtk_widget_get_ancestor (sidebar_annots, PPS_TYPE_DOCUMENT_VIEW));

			if (window)
				pps_document_view_handle_annot_popup (window, annotation);
			else
				g_warn_if_reached();

			if (!gtk_widget_compute_point (row, gtk_widget_get_parent (priv->popup),
						       &GRAPHENE_POINT_INIT(x, y),
						       &sidebar_annots_point))
				g_warn_if_reached ();

			gtk_popover_set_pointing_to (GTK_POPOVER (priv->popup),
				&(const GdkRectangle) { sidebar_annots_point.x, sidebar_annots_point.y, 1, 1 });
			gtk_popover_popup (GTK_POPOVER (priv->popup));
			break;
		default:
			break;
	}
}

static void
job_finished_callback (PpsJobAnnots          *job,
		       PpsSidebarAnnotations *sidebar_annots)
{
	PpsSidebarAnnotationsPrivate *priv = GET_PRIVATE (sidebar_annots);
	GList *l;

	if (!job->annots) {
		adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->stack),
						       "empty");
		g_clear_object (&priv->job);
		return;
	}

	g_list_store_remove_all (priv->model);

	for (l = job->annots; l; l = g_list_next (l)) {
		PpsMappingList *mapping_list;
		GList *ll;

		mapping_list = (PpsMappingList *)l->data;

		for (ll = pps_mapping_list_get_list (mapping_list); ll; ll = g_list_next (ll)) {
			PpsMapping *mapping;
			PpsAnnotation *annotation;

			mapping = ll->data;
			annotation = mapping->data;
			if (!PPS_IS_ANNOTATION_MARKUP (annotation))
				continue;

			g_list_store_append (priv->model, annotation);
		}
	}

	adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->stack),
					       "annot");

	g_clear_object (&priv->job);
}

static void
pps_sidebar_annotations_load (PpsSidebarAnnotations *sidebar_annots)
{
	PpsSidebarAnnotationsPrivate *priv = GET_PRIVATE (sidebar_annots);
	PpsDocumentModel *model = pps_sidebar_page_get_document_model (PPS_SIDEBAR_PAGE (sidebar_annots));
	PpsDocument *document = pps_document_model_get_document (model);

	if (priv->job) {
		g_signal_handlers_disconnect_by_func (priv->job,
						      job_finished_callback,
						      sidebar_annots);
		g_object_unref (priv->job);
	}

	priv->job = pps_job_annots_new (document);
	g_signal_connect (priv->job, "finished",
			  G_CALLBACK (job_finished_callback),
			  sidebar_annots);
	/* The priority doesn't matter for this job */
	pps_job_scheduler_push_job (priv->job, PPS_JOB_PRIORITY_NONE);
}

static void
factory_setup_cb (GtkSignalListItemFactory *self,
                  GObject                  *object,
                  PpsSidebarAnnotations    *sidebar)
{
        GtkListItem *item = GTK_LIST_ITEM (object);
        GtkWidget *widget;
	GtkGesture *gesture;

	widget = GTK_WIDGET (g_object_new (g_type_from_name ("PpsSidebarAnnotationsRow"),
					   NULL));

	gesture = gtk_gesture_click_new ();
	gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture), 0);
	gtk_widget_add_controller (widget, GTK_EVENT_CONTROLLER (gesture));

	g_signal_connect_object (G_OBJECT (gesture), "pressed",
				 (GCallback)sidebar_annots_button_press_cb,
				 item, G_CONNECT_DEFAULT);

        gtk_list_item_set_child (item, widget);
}

static void
factory_bind_cb (GtkSignalListItemFactory *self,
                 GObject                  *object,
                 PpsSidebarAnnotations    *sidebar)
{
        GtkListItem *item = GTK_LIST_ITEM (object);
        GtkWidget *widget = gtk_list_item_get_child (item);
        PpsAnnotation *annotation = PPS_ANNOTATION (gtk_list_item_get_item (item));

	g_object_set (widget,
		      "document", pps_document_model_get_document (pps_sidebar_page_get_document_model (PPS_SIDEBAR_PAGE (sidebar))),
		      "annotation", PPS_ANNOTATION_MARKUP (annotation),
		      NULL);
}

static void
pps_sidebar_annotations_dispose (GObject *object)
{
	PpsSidebarAnnotations *sidebar_annots = PPS_SIDEBAR_ANNOTATIONS (object);
	PpsSidebarAnnotationsPrivate *priv = GET_PRIVATE (sidebar_annots);

	if (priv->job != NULL) {
		g_signal_handlers_disconnect_by_func (priv->job,
						      job_finished_callback,
						      sidebar_annots);
		g_clear_object (&priv->job);
	}

	G_OBJECT_CLASS (pps_sidebar_annotations_parent_class)->dispose (object);
}

static void
pps_sidebar_annotations_document_changed_cb (PpsDocumentModel      *model,
					    GParamSpec           *pspec,
					    PpsSidebarAnnotations *sidebar_annots)
{
	PpsDocument *document = pps_document_model_get_document (model);

	if (!PPS_IS_DOCUMENT_ANNOTATIONS (document))
		return;

	pps_sidebar_annotations_load (sidebar_annots);
}

static gboolean
pps_sidebar_annotations_support_document (PpsSidebarPage *sidebar_page,
					  PpsDocument    *document)
{
	return (PPS_IS_DOCUMENT_ANNOTATIONS (document));
}

static void
pps_sidebar_annotations_constructed (GObject *object)
{
	g_signal_connect (pps_sidebar_page_get_document_model (PPS_SIDEBAR_PAGE (object)),
			  "notify::document",
			  G_CALLBACK (pps_sidebar_annotations_document_changed_cb),
			  PPS_SIDEBAR_ANNOTATIONS (object));
}

static void
pps_sidebar_annotations_init (PpsSidebarAnnotations *pps_annots)
{
	gtk_widget_init_template (GTK_WIDGET (pps_annots));

	g_signal_connect_object (pps_annots, "annot-activated",
				 G_CALLBACK (pps_sidebar_page_navigate_to_view),
				 pps_annots, G_CONNECT_SWAPPED);
}

static void
pps_sidebar_annotations_class_init (PpsSidebarAnnotationsClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
        PpsSidebarPageClass *sidebar_page_class = PPS_SIDEBAR_PAGE_CLASS (klass);

	g_object_class->constructed = pps_sidebar_annotations_constructed;
	g_object_class->dispose = pps_sidebar_annotations_dispose;

	sidebar_page_class->support_document = pps_sidebar_annotations_support_document;

	gtk_widget_class_set_template_from_resource (widget_class,
			"/org/gnome/papers/ui/sidebar-annotations.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, list_view);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, model);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, stack);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, popup);

        gtk_widget_class_bind_template_callback (widget_class, factory_setup_cb);
        gtk_widget_class_bind_template_callback (widget_class, factory_bind_cb);

	signals[ANNOT_ACTIVATED] =
		g_signal_new ("annot-activated",
			      G_TYPE_FROM_CLASS (g_object_class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (PpsSidebarAnnotationsClass, annot_activated),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__POINTER,
			      G_TYPE_NONE, 1,
			      G_TYPE_POINTER);
}

GtkWidget *
pps_sidebar_annotations_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_SIDEBAR_ANNOTATIONS, NULL));
}

void
pps_sidebar_annotations_annot_added (PpsSidebarAnnotations *sidebar_annots,
				    PpsAnnotation         *annot)
{
	pps_sidebar_annotations_load (sidebar_annots);
}

void
pps_sidebar_annotations_annot_removed (PpsSidebarAnnotations *sidebar_annots)
{
	pps_sidebar_annotations_load (sidebar_annots);
}
