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
#include "pps-sidebar-page.h"
#include "pps-sidebar-annotations.h"
#include "pps-jobs.h"
#include "pps-job-scheduler.h"
#include "pps-window.h"
#include "pps-utils.h"

enum {
	PROP_0,
	PROP_DOCUMENT_MODEL
};

enum {
	COLUMN_MARKUP,
	COLUMN_ICON,
	COLUMN_ANNOT_MAPPING,
	COLUMN_TOOLTIP,
	N_COLUMNS
};

enum {
	ANNOT_ACTIVATED,
	N_SIGNALS
};

struct _PpsSidebarAnnotationsPrivate {
	PpsDocumentModel  *model;

	GtkWidget   *list_box;
	GtkWidget   *stack;

	GtkWidget   *popup;

	PpsJob       *job;
};

static void pps_sidebar_annotations_page_iface_init (PpsSidebarPageInterface *iface);
static void pps_sidebar_annotations_load            (PpsSidebarAnnotations   *sidebar_annots);
static void job_finished_callback (PpsJobAnnots          *job,
				   PpsSidebarAnnotations *sidebar_annots);
static void pps_sidebar_annotations_set_model (PpsSidebarPage   *sidebar_page,
					      PpsDocumentModel *model);
static guint signals[N_SIGNALS];

G_DEFINE_TYPE_EXTENDED (PpsSidebarAnnotations,
                        pps_sidebar_annotations,
                        GTK_TYPE_BOX,
                        0,
                        G_ADD_PRIVATE (PpsSidebarAnnotations)
                        G_IMPLEMENT_INTERFACE (PPS_TYPE_SIDEBAR_PAGE,
					       pps_sidebar_annotations_page_iface_init))

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

	g_clear_object (&priv->model);
	G_OBJECT_CLASS (pps_sidebar_annotations_parent_class)->dispose (object);
}

static void
pps_sidebar_annotations_init (PpsSidebarAnnotations *pps_annots)
{
	gtk_widget_init_template (GTK_WIDGET (pps_annots));
}

static void
pps_sidebar_annotations_set_property (GObject      *object,
			       guint         prop_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
	PpsSidebarAnnotations *sidebar_annots = PPS_SIDEBAR_ANNOTATIONS (object);

	switch (prop_id)
	{
	case PROP_DOCUMENT_MODEL:
		pps_sidebar_annotations_set_model (PPS_SIDEBAR_PAGE (sidebar_annots),
			PPS_DOCUMENT_MODEL (g_value_get_object (value)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
pps_sidebar_annotations_class_init (PpsSidebarAnnotationsClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	g_object_class->set_property = pps_sidebar_annotations_set_property;
	g_object_class->dispose = pps_sidebar_annotations_dispose;

	gtk_widget_class_set_template_from_resource (widget_class,
			"/org/gnome/papers/ui/sidebar-annotations.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, list_box);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, stack);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAnnotations, popup);

	g_object_class_override_property (g_object_class, PROP_DOCUMENT_MODEL, "document-model");

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
pps_sidebar_annotations_annot_changed (PpsSidebarAnnotations *sidebar_annots,
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
			const gchar  *label;
			const gchar  *modified;
			const gchar  *contents;
			gchar        *markup = NULL;
			gchar        *tooltip = NULL;
			const gchar  *icon_name = NULL;
			GtkWidget    *row;
			GtkEventController *controller;

			annot = ((PpsMapping *)(ll->data))->data;
			if (!PPS_IS_ANNOTATION_MARKUP (annot))
				continue;

			label = pps_annotation_markup_get_label (PPS_ANNOTATION_MARKUP (annot));
			modified = pps_annotation_get_modified (annot);
			contents = pps_annotation_get_contents (annot);

			if (modified)
				tooltip = g_strdup_printf ("<span weight=\"bold\">%s</span>\n%s", label, modified);
			else
				tooltip = g_strdup_printf ("<span weight=\"bold\">%s</span>", label);

			if (contents && *contents != '\0')
				markup = g_strstrip (g_strdup_printf ("%s", contents));
			else
				markup = g_strdup_printf ("<i>%s</i>", _("No Comment"));

			if (PPS_IS_ANNOTATION_TEXT (annot)) {
				icon_name = "annotations-text-symbolic";
			} else if (PPS_IS_ANNOTATION_ATTACHMENT (annot)) {
				icon_name = "mail-attachment-symbolic";
			} else if (PPS_IS_ANNOTATION_TEXT_MARKUP (annot)) {
                                switch (pps_annotation_text_markup_get_markup_type (PPS_ANNOTATION_TEXT_MARKUP (annot))) {
                                case PPS_ANNOTATION_TEXT_MARKUP_HIGHLIGHT:
                                        icon_name = "format-justify-left-symbolic";
                                        break;
                                case PPS_ANNOTATION_TEXT_MARKUP_STRIKE_OUT:
                                        icon_name = "format-text-strikethrough-symbolic";
                                        break;
                                case PPS_ANNOTATION_TEXT_MARKUP_UNDERLINE:
                                        icon_name = "format-text-underline-symbolic";
                                        break;
                                case PPS_ANNOTATION_TEXT_MARKUP_SQUIGGLY:
                                        icon_name = "annotations-squiggly-symbolic";
                                        break;
                                }
                        }

			row = adw_action_row_new ();
			adw_action_row_add_prefix(ADW_ACTION_ROW (row), gtk_image_new_from_icon_name (icon_name));
			adw_preferences_row_set_title (ADW_PREFERENCES_ROW (row), markup);
			gtk_widget_set_tooltip_markup (row, tooltip);
			adw_expander_row_add_row (ADW_EXPANDER_ROW (expander), row);

			controller = GTK_EVENT_CONTROLLER (gtk_gesture_click_new ());
			gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (controller), 0);
			gtk_widget_add_controller (row, controller);

			g_signal_connect (G_OBJECT (controller), "pressed",
					  (GCallback)sidebar_annots_button_press_cb, ll->data);

			g_free (markup);
			g_free (tooltip);
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
	PpsDocument *document = pps_document_model_get_document (priv->model);

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
pps_sidebar_annotations_document_changed_cb (PpsDocumentModel      *model,
					    GParamSpec           *pspec,
					    PpsSidebarAnnotations *sidebar_annots)
{
	PpsDocument *document = pps_document_model_get_document (model);

	if (!PPS_IS_DOCUMENT_ANNOTATIONS (document))
		return;

	pps_sidebar_annotations_load (sidebar_annots);
}

/* PpsSidebarPageIface */
static void
pps_sidebar_annotations_set_model (PpsSidebarPage   *sidebar_page,
				  PpsDocumentModel *model)
{
	g_return_if_fail (PPS_IS_SIDEBAR_ANNOTATIONS (sidebar_page));
	g_return_if_fail (PPS_IS_DOCUMENT_MODEL (model));
	PpsSidebarAnnotationsPrivate *priv = GET_PRIVATE (PPS_SIDEBAR_ANNOTATIONS (sidebar_page));

	if (priv->model == model)
		return;

	if (priv->model) {
		g_signal_handlers_disconnect_by_func (priv->model,
			G_CALLBACK (pps_sidebar_annotations_document_changed_cb), sidebar_page);
		g_object_unref (priv->model);
	}

	priv->model = g_object_ref (model);

	g_signal_connect (model, "notify::document",
			  G_CALLBACK (pps_sidebar_annotations_document_changed_cb),
			  sidebar_page);
}

static gboolean
pps_sidebar_annotations_support_document (PpsSidebarPage *sidebar_page,
					 PpsDocument    *document)
{
	return (PPS_IS_DOCUMENT_ANNOTATIONS (document));
}

static void
pps_sidebar_annotations_page_iface_init (PpsSidebarPageInterface *iface)
{
	iface->support_document = pps_sidebar_annotations_support_document;
	iface->set_model = pps_sidebar_annotations_set_model;
}
