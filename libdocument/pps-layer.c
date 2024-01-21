/* this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2008 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include "pps-layer.h"

struct _PpsLayerPrivate {
	gboolean is_parent;
	gint     rb_group;
};

typedef struct _PpsLayerPrivate PpsLayerPrivate;

#define GET_PRIVATE(o) pps_layer_get_instance_private (o);

G_DEFINE_TYPE_WITH_PRIVATE (PpsLayer, pps_layer, G_TYPE_OBJECT)

static void
pps_layer_class_init (PpsLayerClass *klass)
{
}

static void
pps_layer_init (PpsLayer *layer)
{
}

PpsLayer *
pps_layer_new (gboolean is_parent,
	      gint     rb_group)
{
	PpsLayer *layer;
	PpsLayerPrivate *priv;

	layer = PPS_LAYER (g_object_new (PPS_TYPE_LAYER, NULL));
	priv = GET_PRIVATE (layer);
	priv->is_parent = is_parent;
	priv->rb_group = rb_group;

	return layer;
}

gboolean
pps_layer_is_parent (PpsLayer *layer)
{
	g_return_val_if_fail (PPS_IS_LAYER (layer), FALSE);
	PpsLayerPrivate *priv = GET_PRIVATE (layer);

	return priv->is_parent;
}

gint
pps_layer_get_rb_group (PpsLayer *layer)
{
	g_return_val_if_fail (PPS_IS_LAYER (layer), 0);
	PpsLayerPrivate *priv = GET_PRIVATE (layer);

	return priv->rb_group;
}
