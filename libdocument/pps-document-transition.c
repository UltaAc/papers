/* pps-document-transition.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2006 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include "pps-document-transition.h"
#include <config.h>

G_DEFINE_INTERFACE (PpsDocumentTransition, pps_document_transition, 0)

static void
pps_document_transition_default_init (PpsDocumentTransitionInterface *klass)
{
}

gdouble
pps_document_transition_get_page_duration (PpsDocumentTransition *document_trans,
                                           gint page)
{
	PpsDocumentTransitionInterface *iface = PPS_DOCUMENT_TRANSITION_GET_IFACE (document_trans);

	if (iface->get_page_duration)
		return iface->get_page_duration (document_trans, page);

	return -1;
}

/**
 * pps_document_transition_get_effect:
 * @document_trans: an #PpsDocumentTransition
 * @page: a page index
 *
 * Returns: (transfer full): an #PpsTransitionEffect
 */
PpsTransitionEffect *
pps_document_transition_get_effect (PpsDocumentTransition *document_trans,
                                    gint page)
{
	PpsDocumentTransitionInterface *iface = PPS_DOCUMENT_TRANSITION_GET_IFACE (document_trans);
	PpsTransitionEffect *effect = NULL;

	if (iface->get_effect)
		effect = iface->get_effect (document_trans, page);

	if (!effect)
		return pps_transition_effect_new (PPS_TRANSITION_EFFECT_REPLACE, NULL);

	return effect;
}
