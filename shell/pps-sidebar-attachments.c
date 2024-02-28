/* pps-sidebar-attachments.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2006 Carlos Garcia Campos
 *
 * Author:
 *   Carlos Garcia Campos <carlosgc@gnome.org>
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

#include <string.h>

#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include "pps-document-attachments.h"
#include "pps-document-misc.h"
#include "pps-jobs.h"
#include "pps-job-scheduler.h"
#include "pps-file-helpers.h"
#include "pps-sidebar-attachments.h"
#include "pps-sidebar-page.h"
#include "pps-shell-marshal.h"

enum {
	PROP_0,
	PROP_DOCUMENT_MODEL,
};

enum {
	SIGNAL_POPUP_MENU,
	SIGNAL_SAVE_ATTACHMENT,
	N_SIGNALS
};

static guint signals[N_SIGNALS];

struct _PpsSidebarAttachmentsPrivate {
	GtkGridView *grid_view;
	GListStore  *model;
};

#define GET_PRIVATE(t) (pps_sidebar_attachments_get_instance_private (t))

static void pps_sidebar_attachments_page_iface_init (PpsSidebarPageInterface *iface);
static void pps_sidebar_attachments_set_model (PpsSidebarPage   *page,
					      PpsDocumentModel *model);

static GdkContentProvider *pps_sidebar_attachments_drag_prepare (PpsSidebarAttachments *pps_attachbar,
								 double                x,
								 double                y,
								 gpointer	       user_data);

static void secondary_button_clicked_cb (PpsSidebarAttachments *pps_attachbar,
					 gint                  n_press,
					 gdouble               x,
					 gdouble               y,
					 gpointer              user_data);

G_DEFINE_TYPE_EXTENDED (PpsSidebarAttachments,
                        pps_sidebar_attachments,
                        GTK_TYPE_BOX,
                        0,
                        G_ADD_PRIVATE (PpsSidebarAttachments)
                        G_IMPLEMENT_INTERFACE (PPS_TYPE_SIDEBAR_PAGE,
					       pps_sidebar_attachments_page_iface_init))

static void
grid_view_factory_setup (PpsSidebarAttachments *pps_attachbar,
			 GObject               *object,
			 gpointer              user_data)
{
	GtkWidget *box, *image, *label;
	GtkDragSource *drag_source;
	GtkGesture *secondary_click;

	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);

	image = gtk_image_new ();
	gtk_image_set_pixel_size (GTK_IMAGE (image), 60);
	gtk_widget_set_margin_top (image, 6);
	gtk_widget_set_margin_bottom (image, 6);
	gtk_widget_set_margin_start (image, 6);
	gtk_widget_set_margin_end (image, 6);

	label = gtk_label_new ("");
	gtk_label_set_wrap (GTK_LABEL (label), TRUE);
	gtk_label_set_wrap_mode (GTK_LABEL (label), PANGO_WRAP_CHAR);

	gtk_box_append (GTK_BOX (box), image);
	gtk_box_append (GTK_BOX (box), label);

	drag_source = gtk_drag_source_new ();
	g_signal_connect_swapped (drag_source, "prepare", G_CALLBACK (pps_sidebar_attachments_drag_prepare), pps_attachbar);
	gtk_widget_add_controller (box, GTK_EVENT_CONTROLLER (drag_source));

	secondary_click = gtk_gesture_click_new ();
	gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (secondary_click), GDK_BUTTON_SECONDARY);
	g_signal_connect_swapped (secondary_click, "pressed", G_CALLBACK (secondary_button_clicked_cb), pps_attachbar);
	gtk_widget_add_controller (box, GTK_EVENT_CONTROLLER (secondary_click));

	gtk_list_item_set_child (GTK_LIST_ITEM (object), box);
}

static void
grid_view_factory_bind (PpsSidebarAttachments *pps_attachbar,
			GObject               *object,
			gpointer              user_data)
{
	PpsAttachment *attachment;
	GtkWidget     *box, *image, *label;
	const gchar   *name, *mime_type;
	GIcon         *icon;
	const gchar   *description;
	gchar *markup;

	box = gtk_list_item_get_child (GTK_LIST_ITEM (object));

	attachment = gtk_list_item_get_item (GTK_LIST_ITEM (object) );
	name = pps_attachment_get_name (attachment);
	mime_type = pps_attachment_get_mime_type (attachment);

	description = pps_attachment_get_description (attachment);
	if (description) {
		markup = g_markup_printf_escaped ("%s", description);
		gtk_widget_set_tooltip_markup (box, markup);
		g_free (markup);
	}

	image = gtk_widget_get_first_child ( box);
	icon = g_content_type_get_symbolic_icon (mime_type);
	gtk_image_set_from_gicon (GTK_IMAGE (image), icon);

	label = gtk_widget_get_last_child (box);
	gtk_label_set_text (GTK_LABEL (label), name);
}

static void
secondary_button_clicked_cb (PpsSidebarAttachments *pps_attachbar,
			     gint                  n_press,
			     gdouble               x,
			     gdouble               y,
			     gpointer              user_data)
{
	PpsSidebarAttachmentsPrivate *priv = GET_PRIVATE (pps_attachbar);
	GtkSelectionModel *selection;
	PpsAttachment     *attachment;
	GtkBitsetIter     iter;
	GtkBitset         *bitset;
	guint             value;
	GList             *attach_list = NULL;
	graphene_point_t  point, new_point;

	selection = gtk_grid_view_get_model (priv->grid_view);
	bitset = gtk_selection_model_get_selection (selection);

	if (!gtk_bitset_iter_init_first (&iter, bitset, &value))
		return;

	attachment = g_list_model_get_item (G_LIST_MODEL (selection), value);
	if (!attachment)
		return;

	attach_list =  g_list_prepend (attach_list, attachment);

	while (gtk_bitset_iter_next ( &iter, &value)) {
		attachment = g_list_model_get_item (G_LIST_MODEL (selection), value);
		if (attachment)
			attach_list = g_list_prepend (attach_list, attachment);
	}

	if (!attach_list)
		return;

	point = GRAPHENE_POINT_INIT (x, y);
	if (!gtk_widget_compute_point (gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (user_data)),
				       GTK_WIDGET (pps_attachbar),
				       &point,
				       &new_point))
		new_point = GRAPHENE_POINT_INIT_ZERO;

	g_signal_emit (pps_attachbar, signals[SIGNAL_POPUP_MENU], 0, &new_point, attach_list);
}

static void
grid_view_item_activated_cb (PpsSidebarAttachments *pps_attachbar,
			     guint                 position,
			     gpointer	           user_data)
{
	PpsSidebarAttachmentsPrivate *priv = GET_PRIVATE (pps_attachbar);
	PpsAttachment *attachment;
	GError *error = NULL;
	GdkDisplay *display = gtk_widget_get_display (GTK_WIDGET (pps_attachbar));
	GdkAppLaunchContext *context = gdk_display_get_app_launch_context (display);

	GtkSelectionModel *model = gtk_grid_view_get_model (priv->grid_view);

	attachment = g_list_model_get_item (G_LIST_MODEL (model), position);
	if (!attachment)
		return;

	pps_attachment_open (attachment, G_APP_LAUNCH_CONTEXT (context), &error);

	if (error) {
		g_warning ("%s", error->message);
		g_error_free (error);
	}

	g_object_unref (attachment);
	g_clear_object (&context);
}

static PpsAttachment *
pps_sidebar_attachments_get_selected_attachment (PpsSidebarAttachments *pps_attachbar)
{
	PpsSidebarAttachmentsPrivate *priv = GET_PRIVATE (pps_attachbar);
	PpsAttachment *attachment;
	GtkSelectionModel *selection;
	GtkBitset *bitset;
	GtkBitsetIter iter;
	guint value;


	selection = gtk_grid_view_get_model (priv->grid_view);
	bitset = gtk_selection_model_get_selection (selection);

	if (!gtk_bitset_iter_init_first (&iter, bitset, &value))
		return NULL;

	attachment = g_list_model_get_item (G_LIST_MODEL (selection), value);
	if (attachment)
		return attachment;


	while (gtk_bitset_iter_next ( &iter, &value)) {
		attachment = g_list_model_get_item (G_LIST_MODEL (selection), value);
		if (attachment)
			return attachment;
	}

	return NULL;
}


static GdkContentProvider *
pps_sidebar_attachments_drag_prepare (PpsSidebarAttachments *pps_attachbar,
				      double                x,
				      double                y,
				      gpointer	            user_data)
{
	g_autoptr (PpsAttachment) attachment = NULL;
	g_autoptr (GFile)     file = NULL;
	g_autoptr (GError)    error = NULL;
	gchar                *template;
	gchar		     *tempdir;

	attachment = pps_sidebar_attachments_get_selected_attachment (pps_attachbar);

        if (!attachment)
                return NULL;

	tempdir = pps_mkdtemp ("attachments.XXXXXX", &error);
	if (!tempdir) {
		g_warning ("%s", error->message);
		return NULL;
	}

	/* FIXMEchpe: convert to filename encoding first! */
	template = g_build_filename (tempdir, pps_attachment_get_name (attachment), NULL);
	file = g_file_new_for_path (template);
	g_free (template);
	g_free (tempdir);

	if (file == NULL || !pps_attachment_save (attachment, file, &error)) {
		g_warning ("%s", error->message);
		return NULL;
	}

	return gdk_content_provider_new_typed (G_TYPE_FILE, file);
}

static void
pps_sidebar_attachments_set_property (GObject      *object,
			       guint         prop_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
	PpsSidebarAttachments *sidebar_attachments = PPS_SIDEBAR_ATTACHMENTS (object);

	switch (prop_id)
	{
	case PROP_DOCUMENT_MODEL:
		pps_sidebar_attachments_set_model (PPS_SIDEBAR_PAGE (sidebar_attachments),
			PPS_DOCUMENT_MODEL (g_value_get_object (value)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
pps_sidebar_attachments_class_init (PpsSidebarAttachmentsClass *pps_attachbar_class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (pps_attachbar_class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (pps_attachbar_class);

	gtk_widget_class_set_template_from_resource (widget_class,
			"/org/gnome/papers/ui/sidebar-attachments.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAttachments, model);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarAttachments, grid_view);

	gtk_widget_class_bind_template_callback (widget_class, grid_view_item_activated_cb);
	gtk_widget_class_bind_template_callback (widget_class, grid_view_factory_bind);
	gtk_widget_class_bind_template_callback (widget_class, grid_view_factory_setup);

	g_object_class->set_property = pps_sidebar_attachments_set_property;

	/* Signals */
	signals[SIGNAL_POPUP_MENU] =
		g_signal_new ("popup",
			      G_TYPE_FROM_CLASS (g_object_class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (PpsSidebarAttachmentsClass, popup_menu),
			      NULL, NULL,
			      pps_shell_marshal_VOID__POINTER_POINTER,
			      G_TYPE_NONE, 2,
			      G_TYPE_POINTER,
			      G_TYPE_POINTER);

	signals[SIGNAL_SAVE_ATTACHMENT] =
		g_signal_new ("save-attachment",
			      G_TYPE_FROM_CLASS (g_object_class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (PpsSidebarAttachmentsClass, save_attachment),
			      NULL, NULL,
			      g_cclosure_marshal_generic,
			      G_TYPE_BOOLEAN, 2,
			      G_TYPE_OBJECT,
		              G_TYPE_STRING);

	g_object_class_override_property (g_object_class, PROP_DOCUMENT_MODEL, "document-model");
}

static void
pps_sidebar_attachments_init (PpsSidebarAttachments *pps_attachbar)
{
	gtk_widget_init_template (GTK_WIDGET (pps_attachbar));
}

GtkWidget *
pps_sidebar_attachments_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_SIDEBAR_ATTACHMENTS, NULL));
}

static void
job_finished_callback (PpsJobAttachments     *job,
		       PpsSidebarAttachments *pps_attachbar)
{
	PpsSidebarAttachmentsPrivate *priv = GET_PRIVATE (pps_attachbar);
	GList *l;

	g_list_store_remove_all (priv->model);

        for (l = job->attachments; l && l->data; l = g_list_next (l))
		g_list_store_append (priv->model, l->data);

	g_object_unref (job);
}


static void
pps_sidebar_attachments_document_changed_cb (PpsDocumentModel      *model,
					    GParamSpec           *pspec,
					    PpsSidebarAttachments *pps_attachbar)
{
	PpsDocument *document = pps_document_model_get_document (model);
	PpsSidebarAttachmentsPrivate *priv = GET_PRIVATE (pps_attachbar);
	PpsJob *job;

	if (!PPS_IS_DOCUMENT_ATTACHMENTS (document))
		return;

	g_list_store_remove_all (priv->model);

	if (!pps_document_attachments_has_attachments (PPS_DOCUMENT_ATTACHMENTS (document)))
		return;

	job = pps_job_attachments_new (document);
	g_signal_connect (job, "finished",
			  G_CALLBACK (job_finished_callback),
			  pps_attachbar);
	g_signal_connect (job, "cancelled",
			  G_CALLBACK (g_object_unref),
			  NULL);
	/* The priority doesn't matter for this job */
	pps_job_scheduler_push_job (job, PPS_JOB_PRIORITY_NONE);
}

static void
pps_sidebar_attachments_set_model (PpsSidebarPage   *page,
				  PpsDocumentModel *model)
{
	g_signal_connect (model, "notify::document",
			  G_CALLBACK (pps_sidebar_attachments_document_changed_cb),
			  page);
}

static gboolean
pps_sidebar_attachments_support_document (PpsSidebarPage   *sidebar_page,
					 PpsDocument      *document)
{
	return (PPS_IS_DOCUMENT_ATTACHMENTS (document) &&
		pps_document_attachments_has_attachments (PPS_DOCUMENT_ATTACHMENTS (document)));
}

static void
pps_sidebar_attachments_page_iface_init (PpsSidebarPageInterface *iface)
{
	iface->support_document = pps_sidebar_attachments_support_document;
	iface->set_model = pps_sidebar_attachments_set_model;
}
