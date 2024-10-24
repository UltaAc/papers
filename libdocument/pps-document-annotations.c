/* pps-document-annotations.c
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

#include "pps-document-annotations.h"

G_DEFINE_INTERFACE (PpsDocumentAnnotations, pps_document_annotations, 0)

static void
pps_document_annotations_default_init (PpsDocumentAnnotationsInterface *klass)
{
}

PpsMappingList *
pps_document_annotations_get_annotations (PpsDocumentAnnotations *document_annots,
                                          PpsPage *page)
{
	PpsDocumentAnnotationsInterface *iface = PPS_DOCUMENT_ANNOTATIONS_GET_IFACE (document_annots);

	return iface->get_annotations (document_annots, page);
}

gboolean
pps_document_annotations_document_is_modified (PpsDocumentAnnotations *document_annots)
{
	PpsDocumentAnnotationsInterface *iface = PPS_DOCUMENT_ANNOTATIONS_GET_IFACE (document_annots);

	return (iface->document_is_modified) ? iface->document_is_modified (document_annots) : FALSE;
}

void
pps_document_annotations_save_annotation (PpsDocumentAnnotations *document_annots,
                                          PpsAnnotation *annot,
                                          PpsAnnotationsSaveMask mask)
{
	PpsDocumentAnnotationsInterface *iface = PPS_DOCUMENT_ANNOTATIONS_GET_IFACE (document_annots);

	iface->save_annotation (document_annots, annot, mask);
}

void
pps_document_annotations_add_annotation (PpsDocumentAnnotations *document_annots,
                                         PpsAnnotation *annot)
{
	PpsDocumentAnnotationsInterface *iface = PPS_DOCUMENT_ANNOTATIONS_GET_IFACE (document_annots);

	if (iface->add_annotation)
		iface->add_annotation (document_annots, annot);
}

gboolean
pps_document_annotations_can_add_annotation (PpsDocumentAnnotations *document_annots)
{
	PpsDocumentAnnotationsInterface *iface = PPS_DOCUMENT_ANNOTATIONS_GET_IFACE (document_annots);

	return iface->add_annotation != NULL;
}

void
pps_document_annotations_remove_annotation (PpsDocumentAnnotations *document_annots,
                                            PpsAnnotation *annot)
{
	PpsDocumentAnnotationsInterface *iface = PPS_DOCUMENT_ANNOTATIONS_GET_IFACE (document_annots);

	if (iface->remove_annotation)
		iface->remove_annotation (document_annots, annot);
}

gboolean
pps_document_annotations_can_remove_annotation (PpsDocumentAnnotations *document_annots)
{
	PpsDocumentAnnotationsInterface *iface = PPS_DOCUMENT_ANNOTATIONS_GET_IFACE (document_annots);

	return iface->remove_annotation != NULL;
}

PpsAnnotationsOverMarkup
pps_document_annotations_over_markup (PpsDocumentAnnotations *document_annots,
                                      PpsAnnotation *annot,
                                      gdouble x,
                                      gdouble y)
{
	PpsDocumentAnnotationsInterface *iface = PPS_DOCUMENT_ANNOTATIONS_GET_IFACE (document_annots);

	if (iface->over_markup)
		return iface->over_markup (document_annots, annot, x, y);

	return PPS_ANNOTATION_OVER_MARKUP_NOT_IMPLEMENTED;
}
