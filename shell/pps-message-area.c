/* pps-message-area.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2007 Carlos Garcia Campos <carlosgc@gnome.org>
 * Copyright (C) 2018 Germán Poo-Caamaño <gpoo@gnome.org>
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

#include <config.h>

#include "pps-message-area.h"

typedef struct
{
	GtkInfoBar *info_bar;
	GtkWidget *main_box;
	GtkWidget *image;
	GtkWidget *label;
	GtkWidget *secondary_label;

	guint message_type : 3;
} PpsMessageAreaPrivate;

enum {
	PROP_0,
	PROP_TEXT,
	PROP_SECONDARY_TEXT,
	PROP_IMAGE
};

static void pps_message_area_set_property (GObject *object,
                                           guint prop_id,
                                           const GValue *value,
                                           GParamSpec *pspec);
static void pps_message_area_get_property (GObject *object,
                                           guint prop_id,
                                           GValue *value,
                                           GParamSpec *pspec);
static void pps_message_area_buildable_iface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_WITH_CODE (PpsMessageArea, pps_message_area, ADW_TYPE_BIN, G_ADD_PRIVATE (PpsMessageArea) G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE, pps_message_area_buildable_iface_init))

#define GET_PRIVATE(o) pps_message_area_get_instance_private (o);

static void
pps_message_area_class_init (PpsMessageAreaClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

	gobject_class->set_property = pps_message_area_set_property;
	gobject_class->get_property = pps_message_area_get_property;

	gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/papers/ui/message-area.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsMessageArea, info_bar);
	gtk_widget_class_bind_template_child_private (widget_class, PpsMessageArea, main_box);
	gtk_widget_class_bind_template_child_private (widget_class, PpsMessageArea, image);
	gtk_widget_class_bind_template_child_private (widget_class, PpsMessageArea, label);
	gtk_widget_class_bind_template_child_private (widget_class, PpsMessageArea, secondary_label);

	gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_ALERT);

	g_object_class_install_property (gobject_class,
	                                 PROP_TEXT,
	                                 g_param_spec_string ("text",
	                                                      "Text",
	                                                      "The primary text of the message dialog",
	                                                      NULL,
	                                                      G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class,
	                                 PROP_SECONDARY_TEXT,
	                                 g_param_spec_string ("secondary-text",
	                                                      "Secondary Text",
	                                                      "The secondary text of the message dialog",
	                                                      NULL,
	                                                      G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class,
	                                 PROP_IMAGE,
	                                 g_param_spec_object ("image",
	                                                      "Image",
	                                                      "The image",
	                                                      GTK_TYPE_WIDGET,
	                                                      G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));
}

static void
pps_message_area_init (PpsMessageArea *area)
{
	gtk_widget_init_template (GTK_WIDGET (area));
}

static void
pps_message_area_set_property (GObject *object,
                               guint prop_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
	PpsMessageArea *area = PPS_MESSAGE_AREA (object);

	switch (prop_id) {
	case PROP_TEXT:
		pps_message_area_set_text (area, g_value_get_string (value));
		break;
	case PROP_SECONDARY_TEXT:
		pps_message_area_set_secondary_text (area, g_value_get_string (value));
		break;
	case PROP_IMAGE:
		pps_message_area_set_image (area, (GtkWidget *) g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
pps_message_area_get_property (GObject *object,
                               guint prop_id,
                               GValue *value,
                               GParamSpec *pspec)
{
	PpsMessageArea *area = PPS_MESSAGE_AREA (object);
	PpsMessageAreaPrivate *priv = GET_PRIVATE (area);

	switch (prop_id) {
	case PROP_TEXT:
		g_value_set_string (value, gtk_label_get_label (GTK_LABEL (priv->label)));
		break;
	case PROP_SECONDARY_TEXT:
		g_value_set_string (value, gtk_label_get_label (GTK_LABEL (priv->secondary_label)));
		break;
	case PROP_IMAGE:
		g_value_set_object (value, priv->image);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static GtkBuildableIface *parent_buildable_iface;

static GObject *
pps_message_area_buildable_get_internal_child (GtkBuildable *buildable,
                                               GtkBuilder *builder,
                                               const char *childname)
{
	PpsMessageArea *area = PPS_MESSAGE_AREA (buildable);

	if (g_strcmp0 (childname, "main_box") == 0)
		return G_OBJECT (_pps_message_area_get_main_box (area));

	return parent_buildable_iface->get_internal_child (buildable, builder, childname);
}

static void
pps_message_area_buildable_iface_init (GtkBuildableIface *iface)
{
	parent_buildable_iface = g_type_interface_peek_parent (iface);

	iface->get_internal_child = pps_message_area_buildable_get_internal_child;
}

void
pps_message_area_add_button (PpsMessageArea *area,
                             const gchar *button_text,
                             gint response_id)
{
	PpsMessageAreaPrivate *priv = GET_PRIVATE (area);

	if (button_text == NULL)
		return;

	gtk_info_bar_add_button (priv->info_bar, button_text, response_id);
}

GtkWidget *
_pps_message_area_get_main_box (PpsMessageArea *area)
{
	PpsMessageAreaPrivate *priv;

	priv = GET_PRIVATE (area);
	return priv->main_box;
}

/**
 * pps_message_area_get_info_bar
 * @area: A #PpsMessageArea
 *
 * Returns: (transfer none) (not nullable): The #GtkInfoBar
 */
GtkInfoBar *
pps_message_area_get_info_bar (PpsMessageArea *area)
{
	PpsMessageAreaPrivate *priv = GET_PRIVATE (area);

	return priv->info_bar;
}

void
pps_message_area_set_image (PpsMessageArea *area,
                            GtkWidget *image)
{
	GtkWidget *parent;
	PpsMessageAreaPrivate *priv;

	g_return_if_fail (PPS_IS_MESSAGE_AREA (area));

	priv = GET_PRIVATE (area);
	priv->message_type = GTK_MESSAGE_OTHER;

	parent = gtk_widget_get_parent (priv->image);
	g_assert (GTK_IS_BOX (parent));
	gtk_box_remove (GTK_BOX (parent), image);
	gtk_box_prepend (GTK_BOX (parent), priv->image);
	gtk_box_reorder_child_after (GTK_BOX (parent), image, NULL);

	priv->image = image;

	g_object_notify (G_OBJECT (area), "image");
}

void
pps_message_area_set_image_from_icon_name (PpsMessageArea *area,
                                           const gchar *icon_name)
{
	PpsMessageAreaPrivate *priv;

	g_return_if_fail (PPS_IS_MESSAGE_AREA (area));
	g_return_if_fail (icon_name != NULL);

	priv = GET_PRIVATE (area);

	gtk_image_set_from_icon_name (GTK_IMAGE (priv->image),
	                              icon_name);
}

void
pps_message_area_set_text (PpsMessageArea *area,
                           const gchar *str)
{
	PpsMessageAreaPrivate *priv;

	g_return_if_fail (PPS_IS_MESSAGE_AREA (area));

	priv = GET_PRIVATE (area);

	if (str) {
		gchar *msg, *escaped;

		escaped = g_markup_escape_text (str, -1);
		msg = g_strdup_printf ("<b>%s</b>", escaped);
		gtk_label_set_markup (GTK_LABEL (priv->label), msg);
		g_free (msg);
		g_free (escaped);
	} else {
		gtk_label_set_markup (GTK_LABEL (priv->label), NULL);
	}

	g_object_notify (G_OBJECT (area), "text");
}

void
pps_message_area_set_secondary_text (PpsMessageArea *area,
                                     const gchar *str)
{
	PpsMessageAreaPrivate *priv;

	g_return_if_fail (PPS_IS_MESSAGE_AREA (area));

	priv = GET_PRIVATE (area);

	if (str) {
		gchar *msg;

		msg = g_strdup_printf ("<small>%s</small>", str);
		gtk_label_set_markup (GTK_LABEL (priv->secondary_label), msg);
		g_free (msg);
	} else {
		gtk_label_set_markup (GTK_LABEL (priv->secondary_label), NULL);
	}

	gtk_widget_set_visible (priv->secondary_label, str != NULL);

	g_object_notify (G_OBJECT (area), "secondary-text");
}
