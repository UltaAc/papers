/* pps-transition-effect.h
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2007 Carlos Garnacho <carlos@imendio.com>
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

G_BEGIN_DECLS

#define PPS_TYPE_TRANSITION_EFFECT (pps_transition_effect_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsTransitionEffect, pps_transition_effect, PPS, TRANSITION_EFFECT, GObject)

typedef enum {
	PPS_TRANSITION_EFFECT_REPLACE,
	PPS_TRANSITION_EFFECT_SPLIT,
	PPS_TRANSITION_EFFECT_BLINDS,
	PPS_TRANSITION_EFFECT_BOX,
	PPS_TRANSITION_EFFECT_WIPE,
	PPS_TRANSITION_EFFECT_DISSOLVE,
	PPS_TRANSITION_EFFECT_GLITTER,
	PPS_TRANSITION_EFFECT_FLY,
	PPS_TRANSITION_EFFECT_PUSH,
	PPS_TRANSITION_EFFECT_COVER,
	PPS_TRANSITION_EFFECT_UNCOVER,
	PPS_TRANSITION_EFFECT_FADE
} PpsTransitionEffectType;

typedef enum {
	PPS_TRANSITION_ALIGNMENT_HORIZONTAL,
	PPS_TRANSITION_ALIGNMENT_VERTICAL
} PpsTransitionEffectAlignment;

typedef enum {
	PPS_TRANSITION_DIRECTION_INWARD,
	PPS_TRANSITION_DIRECTION_OUTWARD
} PpsTransitionEffectDirection;

struct _PpsTransitionEffect {
	GObject parent_instance;
};

PPS_PUBLIC
PpsTransitionEffect *pps_transition_effect_new (PpsTransitionEffectType type,
                                                const gchar *first_property_name,
                                                ...);

G_END_DECLS
