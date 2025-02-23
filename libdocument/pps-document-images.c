/* pps-document-images.c
 *  this file is part of papers, a gnome document_links viewer
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

#include "pps-document-images.h"
#include <config.h>

G_DEFINE_INTERFACE (PpsDocumentImages, pps_document_images, 0)

static void
pps_document_images_default_init (PpsDocumentImagesInterface *klass)
{
}

PpsMappingList *
pps_document_images_get_image_mapping (PpsDocumentImages *document_images,
                                       PpsPage *page)
{
	PpsDocumentImagesInterface *iface = PPS_DOCUMENT_IMAGES_GET_IFACE (document_images);

	return iface->get_image_mapping (document_images, page);
}

/**
 * pps_document_images_get_image:
 * @document_images: an #PpsDocumentImages
 * @image: an #PpsImage
 *
 * Returns: (transfer full): a #GdkPixbuf
 */
GdkPixbuf *
pps_document_images_get_image (PpsDocumentImages *document_images,
                               PpsImage *image)
{
	PpsDocumentImagesInterface *iface = PPS_DOCUMENT_IMAGES_GET_IFACE (document_images);

	return iface->get_image (document_images, image);
}
