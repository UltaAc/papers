/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2004 Red Hat, Inc
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

#if !defined (__PPS_PAPERS_VIEW_H_INSIDE__) && !defined (PAPERS_COMPILATION)
#error "Only <papers-view.h> can be included directly."
#endif

#include <gtk/gtk.h>

#include <papers-document.h>

#include "context/pps-document-model.h"
#include "context/pps-search-context.h"
#include "pps-jobs.h"

G_BEGIN_DECLS

#define PPS_TYPE_VIEW            (pps_view_get_type ())
PPS_PUBLIC
G_DECLARE_DERIVABLE_TYPE (PpsView, pps_view, PPS, VIEW, GtkWidget)

PPS_PUBLIC
PpsView *	pps_view_new		    (void);
PPS_PUBLIC
void		pps_view_set_model	    (PpsView          *view,
					     PpsDocumentModel *model);
PPS_PUBLIC
gboolean        pps_view_is_loading          (PpsView          *view);
PPS_PUBLIC
void            pps_view_reload              (PpsView          *view);
PPS_PUBLIC
void            pps_view_set_page_cache_size (PpsView          *view,
					     gsize            cache_size);

PPS_PUBLIC
void            pps_view_set_allow_links_change_zoom (PpsView  *view,
                                                     gboolean allowed);
PPS_PUBLIC
gboolean        pps_view_get_allow_links_change_zoom (PpsView  *view);

/* Clipboard */
PPS_PUBLIC
void		pps_view_copy		  (PpsView         *view);
PPS_PUBLIC
void            pps_view_copy_link_address (PpsView         *view,
					   PpsLinkAction   *action);
PPS_PUBLIC
void		pps_view_select_all	  (PpsView         *view);
PPS_PUBLIC
gboolean        pps_view_has_selection	  (PpsView         *view);
PPS_PUBLIC
char *   pps_view_get_selected_text (PpsView  *view);

/* Page size */
PPS_PUBLIC
gboolean	pps_view_can_zoom_in       (PpsView         *view);
PPS_PUBLIC
void		pps_view_zoom_in		  (PpsView         *view);
PPS_PUBLIC
gboolean        pps_view_can_zoom_out      (PpsView         *view);
PPS_PUBLIC
void		pps_view_zoom_out	  (PpsView         *view);

/* Find */
PPS_PUBLIC
void            pps_view_set_search_context        (PpsView   *view,
						    PpsSearchContext *context);
PPS_PUBLIC
void            pps_view_find_restart              (PpsView         *view,
                                                   gint            page);
PPS_PUBLIC
void            pps_view_find_next                 (PpsView         *view);
PPS_PUBLIC
void            pps_view_find_previous             (PpsView         *view);
PPS_PUBLIC
void            pps_view_find_set_result           (PpsView         *view,
						   gint            page,
						   gint            result);
PPS_PUBLIC
void     	pps_view_find_set_highlight_search (PpsView         *view,
						   gboolean        value);

/* Navigation */
PPS_PUBLIC
void	       pps_view_handle_link        (PpsView         *view,
					   PpsLink         *link);
PPS_PUBLIC
gboolean       pps_view_next_page	  (PpsView         *view);
PPS_PUBLIC
gboolean       pps_view_previous_page	  (PpsView         *view);

PPS_PUBLIC
gboolean       pps_view_get_page_extents   (PpsView       *view,
                                           gint          page,
                                           GdkRectangle *page_area,
                                           GtkBorder    *border);
PPS_PUBLIC
gboolean       pps_view_get_page_extents_for_border (PpsView       *view,
                                                    gint          page,
                                                    GtkBorder    *border,
                                                    GdkRectangle *page_area);

/* Annotations */
PPS_PUBLIC
void           pps_view_focus_annotation      (PpsView          *view,
					       const PpsMapping *annot_mapping);
PPS_PUBLIC
void           pps_view_begin_add_text_annotation  (PpsView *view);
PPS_PUBLIC
void           pps_view_cancel_add_text_annotation (PpsView *view);
PPS_PUBLIC
void           pps_view_remove_annotation     (PpsView          *view,
					      PpsAnnotation    *annot);
PPS_PUBLIC
gboolean       pps_view_add_text_markup_annotation_for_selected_text (PpsView  *view);
PPS_PUBLIC
void           pps_view_set_annotation_color (PpsView  *view, GdkRGBA *color);
PPS_PUBLIC
void           pps_view_set_enable_spellchecking (PpsView *view,
                                                 gboolean spellcheck);
PPS_PUBLIC
gboolean       pps_view_get_enable_spellchecking (PpsView *view);

/* Caret navigation */
PPS_PUBLIC
gboolean       pps_view_supports_caret_navigation    (PpsView  *view);
PPS_PUBLIC
gboolean       pps_view_is_caret_navigation_enabled  (PpsView  *view);
PPS_PUBLIC
void           pps_view_set_caret_navigation_enabled (PpsView  *view,
                                                     gboolean enabled);
PPS_PUBLIC
void           pps_view_set_caret_cursor_position    (PpsView  *view,
                                                     guint    page,
                                                     guint    offset);
PPS_PUBLIC
gboolean       pps_view_current_event_is_type        (PpsView *view,
						     GdkEventType type);

typedef void (*PpsUserRectangleCallback)(PpsRectangle *rect);

PPS_PUBLIC
void           pps_view_start_signature_rect     (PpsView *view);

PPS_PUBLIC
void           pps_view_cancel_signature_rect    (PpsView *view);

G_END_DECLS
