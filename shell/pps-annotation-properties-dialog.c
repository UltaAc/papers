/* pps-annotation-properties-dialog.c
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

#include <config.h>

#include <glib/gi18n.h>

#include "pps-annotation-properties-dialog.h"
#include <adwaita.h>

enum {
	PROP_0,
	PROP_ANNOT_TYPE
};

struct _PpsAnnotationPropertiesDialog {
	AdwAlertDialog        base_instance;

	PpsAnnotationType annot_type;
	PpsAnnotation    *annot;

	AdwPreferencesGroup *group;

	AdwEntryRow      *author;
	GtkColorDialogButton     *color;
	AdwSpinRow       *opacity;
	AdwSwitchRow     *popup_state;

	/* Text Annotations */
	AdwComboRow     *icon;

        /* Text Markup Annotations */
        AdwComboRow     *markup_type;
};

struct _PpsAnnotationPropertiesDialogClass {
	AdwAlertDialogClass base_class;
};

G_DEFINE_TYPE (PpsAnnotationPropertiesDialog, pps_annotation_properties_dialog, ADW_TYPE_ALERT_DIALOG)

static void
pps_annotation_properties_dialog_dispose (GObject *object)
{
	PpsAnnotationPropertiesDialog *dialog = PPS_ANNOTATION_PROPERTIES_DIALOG (object);

	g_clear_object (&dialog->annot);

	G_OBJECT_CLASS (pps_annotation_properties_dialog_parent_class)->dispose (object);
}

static void
pps_annotation_properties_dialog_set_property (GObject      *object,
					      guint         prop_id,
					      const GValue *value,
					      GParamSpec   *pspec)
{
	PpsAnnotationPropertiesDialog *dialog = PPS_ANNOTATION_PROPERTIES_DIALOG (object);

	switch (prop_id) {
	case PROP_ANNOT_TYPE:
		dialog->annot_type = g_value_get_enum (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_properties_dialog_constructed (GObject *object)
{
	PpsAnnotationPropertiesDialog *dialog = PPS_ANNOTATION_PROPERTIES_DIALOG (object);

	switch (dialog->annot_type) {
	case PPS_ANNOTATION_TYPE_TEXT:
		gtk_widget_set_visible (GTK_WIDGET (dialog->icon), TRUE);
		break;
	case PPS_ANNOTATION_TYPE_ATTACHMENT:
		/* TODO */
                break;
        case PPS_ANNOTATION_TYPE_TEXT_MARKUP:
		gtk_widget_set_visible (GTK_WIDGET (dialog->markup_type), TRUE);
                break;
	default:
		break;
	}
	G_OBJECT_CLASS (pps_annotation_properties_dialog_parent_class)->constructed (object);
}

static void
pps_annotation_properties_dialog_init (PpsAnnotationPropertiesDialog *annot_dialog)
{
	gtk_widget_init_template (GTK_WIDGET (annot_dialog));

	gtk_editable_set_text (GTK_EDITABLE (annot_dialog->author), g_get_real_name());
}

static void
pps_annotation_properties_dialog_class_init (PpsAnnotationPropertiesDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	object_class->dispose = pps_annotation_properties_dialog_dispose;
	object_class->constructed = pps_annotation_properties_dialog_constructed;
	object_class->set_property = pps_annotation_properties_dialog_set_property;

	gtk_widget_class_set_template_from_resource (widget_class,
                                                     "/org/gnome/papers/ui/annotation-properties-dialog.ui");


	gtk_widget_class_bind_template_child (widget_class, PpsAnnotationPropertiesDialog, group);
	gtk_widget_class_bind_template_child (widget_class, PpsAnnotationPropertiesDialog, author);
	gtk_widget_class_bind_template_child (widget_class, PpsAnnotationPropertiesDialog, color);
	gtk_widget_class_bind_template_child (widget_class, PpsAnnotationPropertiesDialog, opacity);
	gtk_widget_class_bind_template_child (widget_class, PpsAnnotationPropertiesDialog, popup_state);
	gtk_widget_class_bind_template_child (widget_class, PpsAnnotationPropertiesDialog, icon);
	gtk_widget_class_bind_template_child (widget_class, PpsAnnotationPropertiesDialog, markup_type);

	g_object_class_install_property (object_class,
					 PROP_ANNOT_TYPE,
					 g_param_spec_enum ("annot-type",
							    "AnnotType",
							    "The type of annotation",
							    PPS_TYPE_ANNOTATION_TYPE,
							    PPS_ANNOTATION_TYPE_TEXT,
							    G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY |
                                                            G_PARAM_STATIC_STRINGS));
}

GtkWidget *
pps_annotation_properties_dialog_new (PpsAnnotation *annot)
{
	PpsAnnotationPropertiesDialog *dialog;
	const gchar                  *label;
	gdouble                       opacity;
	gboolean                      is_open;
	GdkRGBA                       rgba;

	dialog = g_object_new (PPS_TYPE_ANNOTATION_PROPERTIES_DIALOG,
			       "annot-type", pps_annotation_get_annotation_type (annot),
			       NULL);
	dialog->annot = g_object_ref (annot);

	label = pps_annotation_markup_get_label (PPS_ANNOTATION_MARKUP (annot));
	if (label)
		gtk_editable_set_text (GTK_EDITABLE (dialog->author), label);

	pps_annotation_get_rgba (annot, &rgba);
	gtk_color_dialog_button_set_rgba (dialog->color, &rgba);

	opacity = pps_annotation_markup_get_opacity (PPS_ANNOTATION_MARKUP (annot));
	adw_spin_row_set_value (dialog->opacity, opacity * 100);

	is_open = pps_annotation_markup_get_popup_is_open (PPS_ANNOTATION_MARKUP (annot));
	adw_switch_row_set_active (dialog->popup_state, is_open);

	if (PPS_IS_ANNOTATION_TEXT (annot)) {
		PpsAnnotationText *annot_text = PPS_ANNOTATION_TEXT (annot);

		adw_combo_row_set_selected (dialog->icon,
					    pps_annotation_text_get_icon (annot_text));
	} else if (PPS_IS_ANNOTATION_TEXT_MARKUP (annot)) {
                PpsAnnotationTextMarkup *annot_markup = PPS_ANNOTATION_TEXT_MARKUP (annot);

		adw_combo_row_set_selected (dialog->markup_type,
					    pps_annotation_text_markup_get_markup_type (annot_markup));
        }

	return GTK_WIDGET (dialog);
}

const gchar *
pps_annotation_properties_dialog_get_author (PpsAnnotationPropertiesDialog *dialog)
{
	return gtk_editable_get_text (GTK_EDITABLE (dialog->author));
}

void
pps_annotation_properties_dialog_get_rgba (PpsAnnotationPropertiesDialog *dialog,
					  GdkRGBA                      *rgba)
{
	*rgba = *gtk_color_dialog_button_get_rgba (GTK_COLOR_DIALOG_BUTTON (dialog->color));
}

gdouble
pps_annotation_properties_dialog_get_opacity (PpsAnnotationPropertiesDialog *dialog)
{
	return adw_spin_row_get_value (dialog->opacity) / 100;
}

gboolean
pps_annotation_properties_dialog_get_popup_is_open (PpsAnnotationPropertiesDialog *dialog)
{
	return adw_switch_row_get_active (dialog->popup_state);
}

PpsAnnotationTextIcon
pps_annotation_properties_dialog_get_text_icon (PpsAnnotationPropertiesDialog *dialog)
{
	return adw_combo_row_get_selected (dialog->icon);
}

PpsAnnotationTextMarkupType
pps_annotation_properties_dialog_get_text_markup_type (PpsAnnotationPropertiesDialog *dialog)
{
	return adw_combo_row_get_selected (dialog->markup_type);
}
