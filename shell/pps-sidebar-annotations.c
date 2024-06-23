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

#include "pps-document-annotations.h"
#include "pps-sidebar-annotations.h"
#include "pps-jobs.h"
#include "pps-job-scheduler.h"
#include "pps-window.h"

enum {
	ANNOT_ACTIVATED,
	N_SIGNALS
};

struct _PpsSidebarAnnotationsPrivate {
	GtkWidget   *list_box;
	GtkWidget   *stack;

	GtkWidget   *popup;

	PpsJob       *job;
};

static void pps_sidebar_annotations_load            (PpsSidebarAnnotations   *sidebar_annots);
static void job_finished_callback (PpsJobAnnots          *job,
				   PpsSidebarAnnotations *sidebar_annots);
static guint signals[N_SIGNALS];

G_DEFINE_TYPE_WITH_PRIVATE (PpsSidebarAnnotations, pps_sidebar_annotations, PPS_TYPE_SIDEBAR_PAGE)

#define GET_PRIVATE(t) (pps_sidebar_annotations_get_instance_private (t))

#define ANNOT_ICON_SIZE 16

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
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, list_box);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, stack);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, popup);

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

static void
sidebar_annots_button_press_cb (GtkGestureClick *self,
				gint		 n_press,
				gdouble		 x,
				gdouble		 y,
				PpsMapping	*mapping)
{
	GtkEventController *controller = GTK_EVENT_CONTROLLER (self);
	GdkEvent *event = gtk_event_controller_get_current_event (controller);
	guint button = gdk_button_event_get_button (event);
	GtkWidget *row = gtk_event_controller_get_widget (controller);
	GtkWidget *sidebar_annots = gtk_widget_get_ancestor (row, PPS_TYPE_SIDEBAR_ANNOTATIONS);
	PpsSidebarAnnotationsPrivate *priv = GET_PRIVATE (PPS_SIDEBAR_ANNOTATIONS (sidebar_annots));
	GtkWindow *window;
	graphene_point_t sidebar_annots_point;

	if (!mapping)
		return;

	switch (button) {
		case GDK_BUTTON_PRIMARY:
			g_signal_emit (sidebar_annots, signals[ANNOT_ACTIVATED], 0, mapping);
			break;
		case GDK_BUTTON_SECONDARY:
			if (!PPS_IS_ANNOTATION (mapping->data))
				return;

			window = GTK_WINDOW (gtk_widget_get_native (GTK_WIDGET (sidebar_annots)));

			pps_window_handle_annot_popup (PPS_WINDOW (window), PPS_ANNOTATION (mapping->data));

			if (!gtk_widget_compute_point (row, gtk_widget_get_parent (priv->popup), &GRAPHENE_POINT_INIT(x, y),
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

	gtk_list_box_remove_all (GTK_LIST_BOX (priv->list_box));

	for (l = job->annots; l; l = g_list_next (l)) {
		PpsMappingList *mapping_list;
		GList         *ll;
		gchar         *page_label;
		gboolean       found = FALSE;
		GtkWidget     *expander;

		mapping_list = (PpsMappingList *)l->data;
		page_label = g_strdup_printf (_("Page %d"),
					      pps_mapping_list_get_page (mapping_list) + 1);

		expander = adw_expander_row_new ();
		adw_preferences_row_set_title (ADW_PREFERENCES_ROW (expander), page_label);
		adw_expander_row_set_expanded (ADW_EXPANDER_ROW (expander), TRUE);
		gtk_list_box_append (GTK_LIST_BOX (priv->list_box), expander);

		g_free (page_label);

		for (ll = pps_mapping_list_get_list (mapping_list); ll; ll = g_list_next (ll)) {
			PpsAnnotation *annot;
			GtkWidget    *row;
			GtkEventController *controller;

			annot = ((PpsMapping *)(ll->data))->data;
			if (!PPS_IS_ANNOTATION_MARKUP (annot))
				continue;

			row = GTK_WIDGET (g_object_new (g_type_from_name ("PpsSidebarAnnotationsRow"),
							"annotation",
							PPS_ANNOTATION_MARKUP (annot),
							NULL));

			adw_expander_row_add_row (ADW_EXPANDER_ROW (expander), row);

			controller = GTK_EVENT_CONTROLLER (gtk_gesture_click_new ());
			gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (controller), 0);
			gtk_widget_add_controller (row, controller);

			g_signal_connect (G_OBJECT (controller), "pressed",
					  (GCallback)sidebar_annots_button_press_cb, ll->data);

			found = TRUE;
		}

		if (!found)
			gtk_list_box_remove (GTK_LIST_BOX (priv->list_box), expander);
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
