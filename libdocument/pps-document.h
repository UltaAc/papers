/*
 *  Copyright (C) 2009 Carlos Garcia Campos
 *  Copyright (C) 2000-2003 Marco Pesenti Gritti
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#if !defined (__PPS_PAPERS_DOCUMENT_H_INSIDE__) && !defined (PAPERS_COMPILATION)
#error "Only <papers-document.h> can be included directly."
#endif

#include <glib-object.h>
#include <glib.h>
#include <gio/gio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cairo.h>

#include "pps-macros.h"
#include "pps-document-info.h"
#include "pps-page.h"
#include "pps-render-context.h"

G_BEGIN_DECLS

#define PPS_TYPE_DOCUMENT            (pps_document_get_type ())

PPS_PUBLIC
G_DECLARE_DERIVABLE_TYPE (PpsDocument, pps_document, PPS, DOCUMENT, GObject)

#define PPS_DOCUMENT_ERROR pps_document_error_quark ()

typedef enum /*< flags >*/ {
        PPS_DOCUMENT_LOAD_FLAG_NONE = 0,
        PPS_DOCUMENT_LOAD_FLAG_NO_CACHE
} PpsDocumentLoadFlags;

typedef enum
{
        PPS_DOCUMENT_ERROR_INVALID,
        PPS_DOCUMENT_ERROR_UNSUPPORTED_CONTENT,
        PPS_DOCUMENT_ERROR_ENCRYPTED
} PpsDocumentError;

typedef struct _PpsPoint PpsPoint;
typedef struct _PpsRectangle PpsRectangle;
typedef struct _PpsMapping PpsMapping;
typedef struct _PpsDocumentBackendInfo PpsDocumentBackendInfo;

struct _PpsPoint {
        double x;
        double y;
};

struct _PpsDocumentBackendInfo
{
	const gchar *name;
	const gchar *version;
};

struct _PpsDocumentClass
{
        GObjectClass base_class;

        /* Virtual Methods  */
        gboolean          (* load)                  (PpsDocument          *document,
						     const char          *uri,
						     GError             **error);
        gboolean          (* save)                  (PpsDocument          *document,
						     const char          *uri,
						     GError             **error);
        gint              (* get_n_pages)           (PpsDocument          *document);
	PpsPage          * (* get_page)              (PpsDocument          *document,
						     gint                 index);
        void              (* get_page_size)         (PpsDocument          *document,
						     PpsPage              *page_index,
						     double              *width,
						     double              *height);
        gchar           * (* get_page_label)        (PpsDocument          *document,
						     PpsPage              *page);
        cairo_surface_t * (* render)                (PpsDocument          *document,
						     PpsRenderContext     *rc);
	GdkPixbuf       * (* get_thumbnail)         (PpsDocument          *document,
						     PpsRenderContext     *rc);
        PpsDocumentInfo  * (* get_info)              (PpsDocument          *document);
        gboolean          (* get_backend_info)      (PpsDocument          *document,
						     PpsDocumentBackendInfo *info);

        /* GIO streams */
        gboolean          (* load_stream)           (PpsDocument          *document,
						     GInputStream        *stream,
						     PpsDocumentLoadFlags  flags,
						     GCancellable        *cancellable,
						     GError             **error);
        gboolean          (* load_gfile)            (PpsDocument          *document,
						     GFile               *file,
						     PpsDocumentLoadFlags  flags,
						     GCancellable        *cancellable,
						     GError             **error);
	cairo_surface_t * (* get_thumbnail_surface) (PpsDocument          *document,
						     PpsRenderContext     *rc);
        gboolean          (* load_fd)               (PpsDocument          *document,
						     int                  fd,
						     PpsDocumentLoadFlags  flags,
						     GCancellable        *cancellable,
						     GError             **error);
};

PPS_PUBLIC
GQuark           pps_document_error_quark          (void);

/* Document mutex */
PPS_PUBLIC
void             pps_document_doc_mutex_lock       (PpsDocument      *document);
PPS_PUBLIC
void             pps_document_doc_mutex_unlock     (PpsDocument      *document);
PPS_PUBLIC
gboolean         pps_document_doc_mutex_trylock    (PpsDocument      *document);

PPS_PUBLIC
PpsDocumentInfo  *pps_document_get_info             (PpsDocument      *document);
PPS_PUBLIC
gboolean         pps_document_get_backend_info     (PpsDocument      *document,
						   PpsDocumentBackendInfo *info);
PPS_PUBLIC
gboolean         pps_document_get_modified         (PpsDocument      *document);
PPS_PUBLIC
void             pps_document_set_modified         (PpsDocument      *document,
						   gboolean         modified);
PPS_PUBLIC
gboolean         pps_document_load                 (PpsDocument      *document,
						   const char      *uri,
						   GError         **error);
PPS_PUBLIC
gboolean         pps_document_load_full            (PpsDocument           *document,
						   const char           *uri,
						   PpsDocumentLoadFlags   flags,
						   GError              **error);
PPS_PUBLIC
gboolean         pps_document_load_fd              (PpsDocument         *document,
                                                   int                 fd,
                                                   PpsDocumentLoadFlags flags,
                                                   GCancellable       *cancellable,
                                                   GError            **error);
PPS_PUBLIC
gboolean         pps_document_save                 (PpsDocument      *document,
						   const char      *uri,
						   GError         **error);
PPS_PUBLIC
gint             pps_document_get_n_pages          (PpsDocument      *document);
PPS_PUBLIC
PpsPage          *pps_document_get_page             (PpsDocument      *document,
						   gint             index);
PPS_PUBLIC
void             pps_document_get_page_size        (PpsDocument      *document,
						   gint             page_index,
						   double          *width,
						   double          *height);
PPS_PUBLIC
gchar           *pps_document_get_page_label       (PpsDocument      *document,
						   gint             page_index);
PPS_PUBLIC
cairo_surface_t *pps_document_render               (PpsDocument      *document,
						   PpsRenderContext *rc);
PPS_PUBLIC
GdkPixbuf       *pps_document_get_thumbnail        (PpsDocument      *document,
						   PpsRenderContext *rc);
PPS_PUBLIC
cairo_surface_t *pps_document_get_thumbnail_surface (PpsDocument      *document,
						    PpsRenderContext *rc);
PPS_PUBLIC
guint64          pps_document_get_size             (PpsDocument      *document);
PPS_PUBLIC
const gchar     *pps_document_get_uri              (PpsDocument      *document);
PPS_PUBLIC
const gchar     *pps_document_get_title            (PpsDocument      *document);
PPS_PUBLIC
gboolean         pps_document_is_page_size_uniform (PpsDocument      *document);
PPS_PUBLIC
void             pps_document_get_max_page_size    (PpsDocument      *document,
						   gdouble         *width,
						   gdouble         *height);
PPS_PUBLIC
void             pps_document_get_min_page_size    (PpsDocument      *document,
						   gdouble         *width,
						   gdouble         *height);
PPS_PUBLIC
gboolean         pps_document_check_dimensions     (PpsDocument      *document);
PPS_PUBLIC
gint             pps_document_get_max_label_len    (PpsDocument      *document);
PPS_PUBLIC
gboolean         pps_document_has_text_page_labels (PpsDocument      *document);
PPS_PUBLIC
gboolean         pps_document_find_page_by_label   (PpsDocument      *document,
						   const gchar     *page_label,
						   gint            *page_index);

PPS_PUBLIC
gint             pps_rect_cmp                      (PpsRectangle     *a,
					           PpsRectangle     *b);

#define PPS_TYPE_RECTANGLE (pps_rectangle_get_type ())
struct _PpsRectangle
{
	gdouble x1;
	gdouble y1;
	gdouble x2;
	gdouble y2;
};

PPS_PUBLIC
GType        pps_rectangle_get_type (void) G_GNUC_CONST;
PPS_PUBLIC
PpsRectangle *pps_rectangle_new      (void);
PPS_PUBLIC
PpsRectangle *pps_rectangle_copy     (PpsRectangle *pps_rect);
PPS_PUBLIC
void         pps_rectangle_free     (PpsRectangle *pps_rect);

struct _PpsMapping {
	PpsRectangle area;
	gpointer    data;
};

PPS_PUBLIC
GType        pps_mapping_get_type (void) G_GNUC_CONST;
PPS_PUBLIC
PpsMapping  *pps_mapping_new      (void);
PPS_PUBLIC
PpsMapping  *pps_mapping_copy     (const PpsMapping *pps_mapping);
PPS_PUBLIC
void         pps_mapping_free     (PpsMapping *pps_mapping);
PPS_PUBLIC
void         pps_mapping_set_area (PpsMapping *pps_mapping, PpsRectangle *area);
PPS_PUBLIC
PpsRectangle*pps_mapping_get_area (PpsMapping *pps_mapping);
PPS_PUBLIC
void         pps_mapping_set_data (PpsMapping *pps_mapping, GObject *data);
PPS_PUBLIC
GObject     *pps_mapping_get_data (const PpsMapping *pps_mapping);


/* backends shall implement this function to be able to be opened by Papers
 */
PPS_PUBLIC
GType pps_backend_query_type (void);

G_END_DECLS
