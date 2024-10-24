/* pps-document-forms.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2007 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include "pps-document-forms.h"
#include <config.h>

G_DEFINE_INTERFACE (PpsDocumentForms, pps_document_forms, 0)

static void
pps_document_forms_default_init (PpsDocumentFormsInterface *klass)
{
}

PpsMappingList *
pps_document_forms_get_form_fields (PpsDocumentForms *document_forms,
                                    PpsPage *page)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	return iface->get_form_fields (document_forms, page);
}

gboolean
pps_document_forms_document_is_modified (PpsDocumentForms *document_forms)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	return (iface->document_is_modified) ? iface->document_is_modified (document_forms) : FALSE;
}

void
pps_document_forms_reset_form (PpsDocumentForms *document_forms,
                               PpsLinkAction *action)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	if (iface->reset_form)
		iface->reset_form (document_forms, action);
}

gchar *
pps_document_forms_form_field_text_get_text (PpsDocumentForms *document_forms,
                                             PpsFormField *field)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	return iface->form_field_text_get_text (document_forms, field);
}

void
pps_document_forms_form_field_text_set_text (PpsDocumentForms *document_forms,
                                             PpsFormField *field,
                                             const gchar *text)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	iface->form_field_text_set_text (document_forms, field, text);
}

gboolean
pps_document_forms_form_field_button_get_state (PpsDocumentForms *document_forms,
                                                PpsFormField *field)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	return iface->form_field_button_get_state (document_forms, field);
}

void
pps_document_forms_form_field_button_set_state (PpsDocumentForms *document_forms,
                                                PpsFormField *field,
                                                gboolean state)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	iface->form_field_button_set_state (document_forms, field, state);
}

gchar *
pps_document_forms_form_field_choice_get_item (PpsDocumentForms *document_forms,
                                               PpsFormField *field,
                                               gint index)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	return iface->form_field_choice_get_item (document_forms, field, index);
}

gint
pps_document_forms_form_field_choice_get_n_items (PpsDocumentForms *document_forms,
                                                  PpsFormField *field)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	return iface->form_field_choice_get_n_items (document_forms, field);
}

gboolean
pps_document_forms_form_field_choice_is_item_selected (PpsDocumentForms *document_forms,
                                                       PpsFormField *field,
                                                       gint index)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	return iface->form_field_choice_is_item_selected (document_forms, field, index);
}

void
pps_document_forms_form_field_choice_select_item (PpsDocumentForms *document_forms,
                                                  PpsFormField *field,
                                                  gint index)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	iface->form_field_choice_select_item (document_forms, field, index);
}

void
pps_document_forms_form_field_choice_toggle_item (PpsDocumentForms *document_forms,
                                                  PpsFormField *field,
                                                  gint index)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	iface->form_field_choice_toggle_item (document_forms, field, index);
}

void
pps_document_forms_form_field_choice_unselect_all (PpsDocumentForms *document_forms,
                                                   PpsFormField *field)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	iface->form_field_choice_unselect_all (document_forms, field);
}

void
pps_document_forms_form_field_choice_set_text (PpsDocumentForms *document_forms,
                                               PpsFormField *field,
                                               const gchar *text)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	iface->form_field_choice_set_text (document_forms, field, text);
}

gchar *
pps_document_forms_form_field_choice_get_text (PpsDocumentForms *document_forms,
                                               PpsFormField *field)
{
	PpsDocumentFormsInterface *iface = PPS_DOCUMENT_FORMS_GET_IFACE (document_forms);

	return iface->form_field_choice_get_text (document_forms, field);
}
