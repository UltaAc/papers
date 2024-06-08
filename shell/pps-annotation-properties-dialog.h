/* pps-annotation-properties-dialog.h
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

#pragma once

#include <gtk/gtk.h>
#include <glib-object.h>

#include <papers-document.h>

G_BEGIN_DECLS

#define PPS_TYPE_ANNOTATION_PROPERTIES_DIALOG         (pps_annotation_properties_dialog_get_type())
#define PPS_ANNOTATION_PROPERTIES_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), PPS_TYPE_ANNOTATION_PROPERTIES_DIALOG, PpsAnnotationPropertiesDialog))
#define PPS_ANNOTATION_PROPERTIES_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PPS_TYPE_ANNOTATION_PROPERTIES_DIALOG, PpsAnnotationPropertiesDialogClass))
#define PPS_IS_ANNOTATION_PROPERTIES_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), PPS_TYPE_ANNOTATION_PROPERTIES_DIALOG))
#define PPS_IS_ANNOTATION_PROPERTIES_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), PPS_TYPE_ANNOTATION_PROPERTIES_DIALOG))
#define PPS_ANNOTATION_PROPERTIES_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), PPS_TYPE_ANNOTATION_PROPERTIES_DIALOG, PpsAnnotationPropertiesDialogClass))

typedef struct _PpsAnnotationPropertiesDialog      PpsAnnotationPropertiesDialog;
typedef struct _PpsAnnotationPropertiesDialogClass PpsAnnotationPropertiesDialogClass;

GType                      pps_annotation_properties_dialog_get_type             (void) G_GNUC_CONST;
GtkWidget                 *pps_annotation_properties_dialog_new                  (PpsAnnotation                 *annot);

const gchar               *pps_annotation_properties_dialog_get_author           (PpsAnnotationPropertiesDialog *dialog);
void                       pps_annotation_properties_dialog_get_rgba             (PpsAnnotationPropertiesDialog *dialog,
                                                                                 GdkRGBA                      *rgba);
gdouble                    pps_annotation_properties_dialog_get_opacity          (PpsAnnotationPropertiesDialog *dialog);
gboolean                   pps_annotation_properties_dialog_get_popup_is_open    (PpsAnnotationPropertiesDialog *dialog);
PpsAnnotationTextIcon       pps_annotation_properties_dialog_get_text_icon        (PpsAnnotationPropertiesDialog *dialog);
PpsAnnotationTextMarkupType pps_annotation_properties_dialog_get_text_markup_type (PpsAnnotationPropertiesDialog *dialog);

G_END_DECLS
