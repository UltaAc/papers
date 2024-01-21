/* pps-annotation.h
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

#if !defined (__PPS_PAPERS_DOCUMENT_H_INSIDE__) && !defined (PAPERS_COMPILATION)
#error "Only <papers-document.h> can be included directly."
#endif

#include <glib-object.h>
#include <gdk/gdk.h>

#include "pps-document.h"
#include "pps-attachment.h"
#include "pps-macros.h"

G_BEGIN_DECLS

/* PpsAnnotation */
#define PPS_TYPE_ANNOTATION                      (pps_annotation_get_type())
#define PPS_ANNOTATION(object)                   (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_ANNOTATION, PpsAnnotation))
#define PPS_ANNOTATION_CLASS(klass)              (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_ANNOTATION, PpsAnnotationClass))
#define PPS_IS_ANNOTATION(object)                (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_ANNOTATION))
#define PPS_IS_ANNOTATION_CLASS(klass)           (G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_ANNOTATION))
#define PPS_ANNOTATION_GET_CLASS(object)         (G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_ANNOTATION, PpsAnnotationClass))

/* PpsAnnotationMarkup */
#define PPS_TYPE_ANNOTATION_MARKUP               (pps_annotation_markup_get_type ())

PPS_PUBLIC
G_DECLARE_INTERFACE (PpsAnnotationMarkup, pps_annotation_markup, PPS, ANNOTATION_MARKUP, GObject)

/* PpsAnnotationText */
#define PPS_TYPE_ANNOTATION_TEXT                 (pps_annotation_text_get_type())
#define PPS_ANNOTATION_TEXT(object)              (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_ANNOTATION_TEXT, PpsAnnotationText))
#define PPS_ANNOTATION_TEXT_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_ANNOTATION_TEXT, PpsAnnotationTextClass))
#define PPS_IS_ANNOTATION_TEXT(object)           (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_ANNOTATION_TEXT))
#define PPS_IS_ANNOTATION_TEXT_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_ANNOTATION_TEXT))
#define PPS_ANNOTATION_TEXT_GET_CLASS(object)    (G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_ANNOTATION_TEXT, PpsAnnotationTextClass))

/* PpsAnnotationAttachment */
#define PPS_TYPE_ANNOTATION_ATTACHMENT              (pps_annotation_attachment_get_type())
#define PPS_ANNOTATION_ATTACHMENT(object)           (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_ANNOTATION_ATTACHMENT, PpsAnnotationAttachment))
#define PPS_ANNOTATION_ATTACHMENT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_ANNOTATION_ATTACHMENT, PpsAnnotationAttachmentClass))
#define PPS_IS_ANNOTATION_ATTACHMENT(object)        (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_ANNOTATION_ATTACHMENT))
#define PPS_IS_ANNOTATION_ATTACHMENT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_ANNOTATION_ATTACHMENT))
#define PPS_ANNOTATION_ATTACHMENT_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_ANNOTATION_ATTACHMENT, PpsAnnotationAttachmentClass))

/* PpsAnnotationTextMarkup */
#define PPS_TYPE_ANNOTATION_TEXT_MARKUP              (pps_annotation_text_markup_get_type ())
#define PPS_ANNOTATION_TEXT_MARKUP(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), PPS_TYPE_ANNOTATION_TEXT_MARKUP, PpsAnnotationTextMarkup))
#define PPS_ANNOTATION_TEXT_MARKUP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), PPS_TYPE_ANNOTATION_TEXT_MARKUP, PpsAnnotationTextMarkupClass))
#define PPS_IS_ANNOTATION_TEXT_MARKUP(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), PPS_TYPE_ANNOTATION_TEXT_MARKUP))
#define PPS_IS_ANNOTATION_TEXT_MARKUP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), PPS_TYPE_ANNOTATION_TEXT_MARKUP))
#define PPS_ANNOTATION_TEXT_MARKUP_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS ((object), PPS_TYPE_ANNOTATION_TEXT_MARKUP, PpsAnnotationTextMarkupClass))

typedef struct _PpsAnnotation                PpsAnnotation;
typedef struct _PpsAnnotationClass           PpsAnnotationClass;

typedef struct _PpsAnnotationText            PpsAnnotationText;
typedef struct _PpsAnnotationTextClass       PpsAnnotationTextClass;

typedef struct _PpsAnnotationAttachment      PpsAnnotationAttachment;
typedef struct _PpsAnnotationAttachmentClass PpsAnnotationAttachmentClass;

typedef struct _PpsAnnotationTextMarkup      PpsAnnotationTextMarkup;
typedef struct _PpsAnnotationTextMarkupClass PpsAnnotationTextMarkupClass;

typedef enum {
	PPS_ANNOTATION_TYPE_UNKNOWN,
	PPS_ANNOTATION_TYPE_TEXT,
	PPS_ANNOTATION_TYPE_ATTACHMENT,
	PPS_ANNOTATION_TYPE_TEXT_MARKUP
} PpsAnnotationType;

typedef enum {
	PPS_ANNOTATION_TEXT_ICON_NOTE,
	PPS_ANNOTATION_TEXT_ICON_COMMENT,
	PPS_ANNOTATION_TEXT_ICON_KEY,
	PPS_ANNOTATION_TEXT_ICON_HELP,
	PPS_ANNOTATION_TEXT_ICON_NEW_PARAGRAPH,
	PPS_ANNOTATION_TEXT_ICON_PARAGRAPH,
	PPS_ANNOTATION_TEXT_ICON_INSERT,
	PPS_ANNOTATION_TEXT_ICON_CROSS,
	PPS_ANNOTATION_TEXT_ICON_CIRCLE,
	PPS_ANNOTATION_TEXT_ICON_UNKNOWN
} PpsAnnotationTextIcon;

typedef enum {
        PPS_ANNOTATION_TEXT_MARKUP_HIGHLIGHT,
        PPS_ANNOTATION_TEXT_MARKUP_STRIKE_OUT,
        PPS_ANNOTATION_TEXT_MARKUP_UNDERLINE,
        PPS_ANNOTATION_TEXT_MARKUP_SQUIGGLY
} PpsAnnotationTextMarkupType;

#define PPS_ANNOTATION_DEFAULT_COLOR ((const GdkRGBA) { 1., 1., 0, 1.});

/* PpsAnnotation */
PPS_PUBLIC
GType                pps_annotation_get_type                  (void) G_GNUC_CONST;
PPS_PUBLIC
PpsAnnotationType     pps_annotation_get_annotation_type       (PpsAnnotation           *annot);
PPS_PUBLIC
PpsPage              *pps_annotation_get_page                  (PpsAnnotation           *annot);
PPS_PUBLIC
guint                pps_annotation_get_page_index            (PpsAnnotation           *annot);
PPS_PUBLIC
gboolean             pps_annotation_equal                     (PpsAnnotation           *annot,
							      PpsAnnotation           *other);
PPS_PUBLIC
const gchar         *pps_annotation_get_contents              (PpsAnnotation           *annot);
PPS_PUBLIC
gboolean             pps_annotation_set_contents              (PpsAnnotation           *annot,
							      const gchar            *contents);
PPS_PUBLIC
const gchar         *pps_annotation_get_name                  (PpsAnnotation           *annot);
PPS_PUBLIC
gboolean             pps_annotation_set_name                  (PpsAnnotation           *annot,
							      const gchar            *name);
PPS_PUBLIC
const gchar         *pps_annotation_get_modified              (PpsAnnotation           *annot);
PPS_PUBLIC
gboolean             pps_annotation_set_modified              (PpsAnnotation           *annot,
							      const gchar            *modified);
PPS_PUBLIC
gboolean             pps_annotation_set_modified_from_time_t  (PpsAnnotation           *annot,
							      time_t                  utime);
PPS_PUBLIC
void                 pps_annotation_get_rgba                  (PpsAnnotation           *annot,
                                                              GdkRGBA                *rgba);
PPS_PUBLIC
gboolean             pps_annotation_set_rgba                  (PpsAnnotation           *annot,
                                                              const GdkRGBA          *rgba);
PPS_PUBLIC
void                 pps_annotation_get_area                  (PpsAnnotation           *annot,
                                                              PpsRectangle            *area);
PPS_PUBLIC
gboolean             pps_annotation_set_area                  (PpsAnnotation           *annot,
                                                              const PpsRectangle      *area);

/* PpsAnnotationMarkup */
PPS_PUBLIC
const gchar         *pps_annotation_markup_get_label          (PpsAnnotationMarkup     *markup);
PPS_PUBLIC
gboolean             pps_annotation_markup_set_label          (PpsAnnotationMarkup     *markup,
							      const gchar            *label);
PPS_PUBLIC
gdouble              pps_annotation_markup_get_opacity        (PpsAnnotationMarkup     *markup);
PPS_PUBLIC
gboolean             pps_annotation_markup_set_opacity        (PpsAnnotationMarkup     *markup,
							      gdouble                 opacity);
PPS_PUBLIC
gboolean             pps_annotation_markup_can_have_popup     (PpsAnnotationMarkup     *markup);
PPS_PUBLIC
gboolean             pps_annotation_markup_has_popup          (PpsAnnotationMarkup     *markup);
PPS_PUBLIC
gboolean             pps_annotation_markup_set_has_popup      (PpsAnnotationMarkup     *markup,
							      gboolean                has_popup);
PPS_PUBLIC
void                 pps_annotation_markup_get_rectangle      (PpsAnnotationMarkup     *markup,
							      PpsRectangle            *pps_rect);
PPS_PUBLIC
gboolean             pps_annotation_markup_set_rectangle      (PpsAnnotationMarkup     *markup,
							      const PpsRectangle      *pps_rect);
PPS_PUBLIC
gboolean             pps_annotation_markup_get_popup_is_open  (PpsAnnotationMarkup     *markup);
PPS_PUBLIC
gboolean             pps_annotation_markup_set_popup_is_open  (PpsAnnotationMarkup     *markup,
							      gboolean                is_open);

/* PpsAnnotationText */
PPS_PUBLIC
GType                pps_annotation_text_get_type             (void) G_GNUC_CONST;
PPS_PUBLIC
PpsAnnotation        *pps_annotation_text_new                  (PpsPage                 *page);
PPS_PUBLIC
PpsAnnotationTextIcon pps_annotation_text_get_icon             (PpsAnnotationText       *text);
PPS_PUBLIC
gboolean             pps_annotation_text_set_icon             (PpsAnnotationText       *text,
							      PpsAnnotationTextIcon    icon);
PPS_PUBLIC
gboolean             pps_annotation_text_get_is_open          (PpsAnnotationText       *text);
PPS_PUBLIC
gboolean             pps_annotation_text_set_is_open          (PpsAnnotationText       *text,
							      gboolean                is_open);

/* PpsAnnotationAttachment */
PPS_PUBLIC
GType                pps_annotation_attachment_get_type       (void) G_GNUC_CONST;
PPS_PUBLIC
PpsAnnotation        *pps_annotation_attachment_new            (PpsPage                 *page,
							      PpsAttachment           *attachment);
PPS_PUBLIC
PpsAttachment        *pps_annotation_attachment_get_attachment (PpsAnnotationAttachment *annot);
PPS_PUBLIC
gboolean             pps_annotation_attachment_set_attachment (PpsAnnotationAttachment *annot,
							      PpsAttachment           *attachment);

/* PpsAnnotationTextMarkup */
PPS_PUBLIC
GType                      pps_annotation_text_markup_get_type        (void) G_GNUC_CONST;
PPS_PUBLIC
PpsAnnotation              *pps_annotation_text_markup_highlight_new   (PpsPage                    *page);
PPS_PUBLIC
PpsAnnotation              *pps_annotation_text_markup_strike_out_new  (PpsPage                    *page);
PPS_PUBLIC
PpsAnnotation              *pps_annotation_text_markup_underline_new   (PpsPage                    *page);
PPS_PUBLIC
PpsAnnotation              *pps_annotation_text_markup_squiggly_new    (PpsPage                    *page);
PPS_PUBLIC
PpsAnnotationTextMarkupType pps_annotation_text_markup_get_markup_type (PpsAnnotationTextMarkup    *annot);
PPS_PUBLIC
gboolean                   pps_annotation_text_markup_set_markup_type (PpsAnnotationTextMarkup    *annot,
                                                                      PpsAnnotationTextMarkupType markup_type);

G_END_DECLS
