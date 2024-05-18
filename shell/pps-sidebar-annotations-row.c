/* pps-sidebar-annotations-row.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2024 Pablo Correa Gomez <ablocorrea@hotmail.com>
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

#include <glib/gi18n.h>

#include <pps-annotation.h>

#include "pps-sidebar-annotations-row.h"

enum {
	PROP_0,
	PROP_ANNOT,
};

struct _PpsSidebarAnnotationsRow {
	AdwActionRow parent;
};

typedef struct {
	PpsAnnotation *annot;
} PpsSidebarAnnotationsRowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PpsSidebarAnnotationsRow, pps_sidebar_annotations_row, ADW_TYPE_ACTION_ROW);
#define GET_PRIVATE(t) (pps_sidebar_annotations_row_get_instance_private (t))

static void
set_row_tooltip (PpsSidebarAnnotationsRow *self,
		 PpsAnnotationMarkup      *annot)
{
	AdwActionRow *row = ADW_ACTION_ROW (self);
	const gchar *label, *modified;
	g_autofree gchar *tooltip = NULL;

	label = pps_annotation_markup_get_label (annot);
	modified = pps_annotation_get_modified (PPS_ANNOTATION (annot));
	if (modified)
		tooltip = g_strdup_printf ("<span weight=\"bold\">%s</span>\n%s",
					   label, modified);
	else
		tooltip = g_strdup_printf ("<span weight=\"bold\">%s</span>",
					   label);

	gtk_widget_set_tooltip_markup (GTK_WIDGET (row), tooltip);
}

static void
label_modified_changed_cb (PpsAnnotationMarkup *annot,
			   GParamSpec          *pspec,
			   gpointer            *user_data)
{
	set_row_tooltip (PPS_SIDEBAR_ANNOTATIONS_ROW (user_data), annot);
}

static void
set_row_title (PpsSidebarAnnotationsRow *self,
	       PpsAnnotation            *annot)
{
	AdwActionRow *row = ADW_ACTION_ROW (self);
	const gchar *contents;
	g_autofree gchar *markup = NULL;

	contents = pps_annotation_get_contents (annot);
	if (contents && *contents != '\0') {
		g_autofree gchar *escaped = g_markup_escape_text (contents, -1);
		markup = g_strstrip (g_strdup_printf ("%s", escaped));
	}
	else
		markup = g_strdup_printf ("<i>%s</i>", _("No Comment"));

	adw_preferences_row_set_title (ADW_PREFERENCES_ROW (row), markup);
}

static void
annot_contents_changed_cb (PpsAnnotation *annot,
			   GParamSpec    *pspec,
			   gpointer      *user_data)
{
	set_row_title (PPS_SIDEBAR_ANNOTATIONS_ROW (user_data), annot);
}

static void
pps_sidebar_annotations_row_set_annotation (PpsSidebarAnnotationsRow *self,
					    PpsAnnotation *annot)
{
	PpsSidebarAnnotationsRowPrivate *priv = GET_PRIVATE (self);
	const gchar  *icon_name = NULL;

	g_return_if_fail (PPS_IS_ANNOTATION (annot));
	g_return_if_fail (PPS_IS_ANNOTATION_MARKUP (annot));

	if (priv->annot)
		g_signal_handlers_disconnect_by_data (priv->annot, self);
	g_clear_object (&priv->annot);

	priv->annot = g_object_ref (annot);
	set_row_tooltip (self, PPS_ANNOTATION_MARKUP (annot));
	g_signal_connect (annot, "notify::label",
			  G_CALLBACK (label_modified_changed_cb),
			  self);
	g_signal_connect (annot, "notify::modified",
			  G_CALLBACK (label_modified_changed_cb),
			  self);
	set_row_title (self, annot);
	g_signal_connect (annot, "notify::contents",
			  G_CALLBACK (annot_contents_changed_cb),
			  self);

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

	adw_action_row_add_prefix(ADW_ACTION_ROW (ADW_ACTION_ROW (self)),
				  gtk_image_new_from_icon_name (icon_name));
}

static void
pps_sidebar_annotations_row_dispose (GObject *object)
{
	PpsSidebarAnnotationsRow *self = PPS_SIDEBAR_ANNOTATIONS_ROW (object);
	PpsSidebarAnnotationsRowPrivate *priv = GET_PRIVATE (self);

	g_signal_handlers_disconnect_by_data (priv->annot, self);
	g_clear_object (&priv->annot);

	G_OBJECT_CLASS (pps_sidebar_annotations_row_parent_class)->dispose (object);
}

static void
pps_sidebar_annotations_row_set_property (GObject      *object,
					  guint         prop_id,
					  const GValue *value,
					  GParamSpec   *pspec)
{
	PpsSidebarAnnotationsRow *self = PPS_SIDEBAR_ANNOTATIONS_ROW (object);

	switch (prop_id)
	{
	case PROP_ANNOT:
		pps_sidebar_annotations_row_set_annotation (self,
							    g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_sidebar_annotations_row_class_init (PpsSidebarAnnotationsRowClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->dispose = pps_sidebar_annotations_row_dispose;
	g_object_class->set_property = pps_sidebar_annotations_row_set_property;

	g_object_class_install_property (g_object_class,
					 PROP_ANNOT,
					 g_param_spec_object ("annotation",
							      NULL, NULL,
							      PPS_TYPE_ANNOTATION,
							      G_PARAM_WRITABLE |
							      G_PARAM_CONSTRUCT_ONLY |
							      G_PARAM_STATIC_STRINGS));
}

static void
pps_sidebar_annotations_row_init (PpsSidebarAnnotationsRow *self)
{
}
