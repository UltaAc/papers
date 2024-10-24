/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2005 Jonathan Blandford <jrb@gnome.org>
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

#if !defined(__PPS_PAPERS_DOCUMENT_H_INSIDE__) && !defined(PAPERS_COMPILATION)
#error "Only <papers-document.h> can be included directly."
#endif

#include <glib-object.h>

#include "pps-macros.h"
#include "pps-page.h"

G_BEGIN_DECLS

#define PPS_TYPE_RENDER_CONTEXT (pps_render_context_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsRenderContext, pps_render_context, PPS, RENDER_CONTEXT, GObject)

struct _PpsRenderContext {
	GObject parent;

	PpsPage *page;
	gint rotation;
	gdouble scale;
	gint target_width;
	gint target_height;
};

PPS_PUBLIC
PpsRenderContext *pps_render_context_new (PpsPage *page,
                                          gint rotation,
                                          gdouble scale);
PPS_PUBLIC
void pps_render_context_set_page (PpsRenderContext *rc,
                                  PpsPage *page);
PPS_PUBLIC
void pps_render_context_set_rotation (PpsRenderContext *rc,
                                      gint rotation);
PPS_PUBLIC
void pps_render_context_set_scale (PpsRenderContext *rc,
                                   gdouble scale);
PPS_PUBLIC
void pps_render_context_set_target_size (PpsRenderContext *rc,
                                         int target_width,
                                         int target_height);
PPS_PUBLIC
void pps_render_context_compute_scaled_size (PpsRenderContext *rc,
                                             double width_points,
                                             double height_points,
                                             int *scaled_width,
                                             int *scaled_height);
PPS_PUBLIC
void pps_render_context_compute_transformed_size (PpsRenderContext *rc,
                                                  double width_points,
                                                  double height_points,
                                                  int *transformed_width,
                                                  int *transformed_height);
PPS_PUBLIC
void pps_render_context_compute_scales (PpsRenderContext *rc,
                                        double width_points,
                                        double height_points,
                                        double *scale_x,
                                        double *scale_y);

G_END_DECLS
