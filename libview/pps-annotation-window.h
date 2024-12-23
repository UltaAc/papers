/* pps-annotation-window.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2009 Carlos Garcia Campos <carlosgc@gnome.org>
 * Copyright (C) 2007 Iñigo Martinez <inigomartinez@gmail.com>
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

#if !defined(PAPERS_COMPILATION)
#error "This is a private header."
#endif

#include <gtk/gtk.h>

#include <pps-document.h>

#include "pps-annotation.h"

G_BEGIN_DECLS

typedef struct _PpsAnnotationWindow PpsAnnotationWindow;
typedef struct _PpsAnnotationWindowClass PpsAnnotationWindowClass;

#define PPS_TYPE_ANNOTATION_WINDOW (pps_annotation_window_get_type ())
#define PPS_ANNOTATION_WINDOW(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), PPS_TYPE_ANNOTATION_WINDOW, PpsAnnotationWindow))
#define PPS_ANNOTATION_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), PPS_TYPE_ANNOTATION_WINDOW, PpsAnnotationWindowClass))
#define PPS_IS_ANNOTATION_WINDOW(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), PPS_TYPE_ANNOTATION_WINDOW))
#define PPS_IS_ANNOTATION_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PPS_TYPE_ANNOTATION_WINDOW))
#define PPS_ANNOTATION_WINDOW_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS ((object), PPS_TYPE_ANNOTATION_WINDOW, PpsAnnotationWindowClass))

GType pps_annotation_window_get_type (void) G_GNUC_CONST;
GtkWidget *pps_annotation_window_new (PpsAnnotation *annot,
                                      GtkWindow *parent,
                                      PpsDocument *document);
PpsAnnotation *pps_annotation_window_get_annotation (PpsAnnotationWindow *window);
gboolean pps_annotation_window_is_open (PpsAnnotationWindow *window);
void pps_annotation_window_set_enable_spellchecking (PpsAnnotationWindow *window,
                                                     gboolean spellcheck);

G_END_DECLS
