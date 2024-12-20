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

#include "pps-progress-message-area.h"

typedef struct
{
	GtkInfoBar *info_bar;
	GtkWidget *image;
	GtkWidget *label;
	GtkWidget *secondary_label;
	GtkWidget *progress_bar;
	GtkWidget *progress_label;
} PpsProgressMessageAreaPrivate;

enum {
	PROP_0,
	PROP_TEXT,
	PROP_SECONDARY_TEXT,
	PROP_STATUS,
	PROP_FRACTION,
};

static void pps_progress_message_area_set_property (GObject *object,
                                           guint prop_id,
                                           const GValue *value,
                                           GParamSpec *pspec);
static void pps_progress_message_area_get_property (GObject *object,
                                           guint prop_id,
                                           GValue *value,
                                           GParamSpec *pspec);

G_DEFINE_TYPE_WITH_PRIVATE (PpsProgressMessageArea, pps_progress_message_area, ADW_TYPE_BIN)

#define GET_PRIVATE(o) pps_progress_message_area_get_instance_private (o)

static void
pps_progress_message_area_class_init (PpsProgressMessageAreaClass *class)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

	gobject_class->set_property = pps_progress_message_area_set_property;
	gobject_class->get_property = pps_progress_message_area_get_property;

	gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/papers/ui/progress-message-area.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsProgressMessageArea, info_bar);
	gtk_widget_class_bind_template_child_private (widget_class, PpsProgressMessageArea, image);
	gtk_widget_class_bind_template_child_private (widget_class, PpsProgressMessageArea, label);
	gtk_widget_class_bind_template_child_private (widget_class, PpsProgressMessageArea, secondary_label);
	gtk_widget_class_bind_template_child_private (widget_class, PpsProgressMessageArea, progress_bar);
	gtk_widget_class_bind_template_child_private (widget_class, PpsProgressMessageArea, progress_label);

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
	                                 PROP_STATUS,
	                                 g_param_spec_string ("status",
	                                                      "Status",
	                                                      "The status text of the progress area",
	                                                      NULL,
	                                                      G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class,
	                                 PROP_FRACTION,
	                                 g_param_spec_double ("fraction",
	                                                      "Fraction",
	                                                      "The fraction of total work that has been completed",
	                                                      0.0, 1.0, 0.0,
	                                                      G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));
}

static void
pps_progress_message_area_init (PpsProgressMessageArea *area)
{
	gtk_widget_init_template (GTK_WIDGET (area));
}

static void
pps_progress_message_area_set_property (GObject *object,
                               guint prop_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
	PpsProgressMessageArea *area = PPS_PROGRESS_MESSAGE_AREA (object);

	switch (prop_id) {
	case PROP_TEXT:
		pps_progress_message_area_set_text (area, g_value_get_string (value));
		break;
	case PROP_SECONDARY_TEXT:
		pps_progress_message_area_set_secondary_text (area, g_value_get_string (value));
		break;
	case PROP_STATUS:
		pps_progress_message_area_set_status (area, g_value_get_string (value));
		break;
	case PROP_FRACTION:
		pps_progress_message_area_set_fraction (area, g_value_get_double (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
pps_progress_message_area_get_property (GObject *object,
                               guint prop_id,
                               GValue *value,
                               GParamSpec *pspec)
{
	PpsProgressMessageArea *area = PPS_PROGRESS_MESSAGE_AREA (object);
	PpsProgressMessageAreaPrivate *priv = GET_PRIVATE (area);

	switch (prop_id) {
	case PROP_TEXT:
		g_value_set_string (value, gtk_label_get_label (GTK_LABEL (priv->label)));
		break;
	case PROP_SECONDARY_TEXT:
		g_value_set_string (value, gtk_label_get_label (GTK_LABEL (priv->secondary_label)));
		break;
	case PROP_STATUS:
		g_value_set_string (value, gtk_label_get_label (GTK_LABEL (priv->progress_label)));
		break;
	case PROP_FRACTION: {
		gdouble fraction;

		fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (priv->progress_bar));
		g_value_set_double (value, fraction);
		break;
	}
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

void
pps_progress_message_area_add_button (PpsProgressMessageArea *area,
                             const gchar *button_text,
                             gint response_id)
{
	PpsProgressMessageAreaPrivate *priv = GET_PRIVATE (area);

	if (button_text == NULL)
		return;

	gtk_info_bar_add_button (priv->info_bar, button_text, response_id);
}

/**
 * pps_progress_message_area_get_info_bar
 * @area: A #PpsProgressMessageArea
 *
 * Returns: (transfer none) (not nullable): The #GtkInfoBar
 */
GtkInfoBar *
pps_progress_message_area_get_info_bar (PpsProgressMessageArea *area)
{
	PpsProgressMessageAreaPrivate *priv = GET_PRIVATE (area);

	return priv->info_bar;
}

void
pps_progress_message_area_set_image_from_icon_name (PpsProgressMessageArea *area,
                                           const gchar *icon_name)
{
	PpsProgressMessageAreaPrivate *priv = GET_PRIVATE (area);

	g_return_if_fail (PPS_IS_PROGRESS_MESSAGE_AREA (area));
	g_return_if_fail (icon_name != NULL);

	gtk_image_set_from_icon_name (GTK_IMAGE (priv->image),
	                              icon_name);
}

void
pps_progress_message_area_set_text (PpsProgressMessageArea *area,
                           const gchar *str)
{
	PpsProgressMessageAreaPrivate *priv;

	g_return_if_fail (PPS_IS_PROGRESS_MESSAGE_AREA (area));

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
pps_progress_message_area_set_secondary_text (PpsProgressMessageArea *area,
                                     const gchar *str)
{
	PpsProgressMessageAreaPrivate *priv;

	g_return_if_fail (PPS_IS_PROGRESS_MESSAGE_AREA (area));

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

void
pps_progress_message_area_set_status (PpsProgressMessageArea *area,
                                      const gchar *str)
{
	PpsProgressMessageAreaPrivate *priv = GET_PRIVATE (area);
	g_return_if_fail (PPS_IS_PROGRESS_MESSAGE_AREA (area));

	gtk_label_set_text (GTK_LABEL (priv->progress_label), str);

	g_object_notify (G_OBJECT (area), "status");
}

void
pps_progress_message_area_set_fraction (PpsProgressMessageArea *area,
                                        gdouble fraction)
{
	PpsProgressMessageAreaPrivate *priv = GET_PRIVATE (area);
	g_return_if_fail (PPS_IS_PROGRESS_MESSAGE_AREA (area));

	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (priv->progress_bar),
	                               fraction);
	g_object_notify (G_OBJECT (area), "fraction");
}

GtkWidget *
pps_progress_message_area_new (const gchar *icon_name,
                               const gchar *text)
{
	GtkWidget *widget;

	widget = g_object_new (PPS_TYPE_PROGRESS_MESSAGE_AREA,
	                       "text", text,
	                       NULL);

	pps_progress_message_area_set_image_from_icon_name (PPS_PROGRESS_MESSAGE_AREA (widget),
	                                           icon_name);

	return widget;
}
