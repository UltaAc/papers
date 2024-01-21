/*
 *  Copyright (C) 2009 Carlos Garcia Campos
 *  Copyright (C) 2004 Marco Pesenti Gritti
 *  Copyright © 2021 Christian Persch
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
 *
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "pps-document-info.h"
#include "pps-xmp.h"

G_DEFINE_BOXED_TYPE (PpsDocumentInfo, pps_document_info, pps_document_info_copy, pps_document_info_free)

/**
 * pps_document_info_new:
 *
 * Returns: (transfer full): a new, empty #PpsDocumentInfo
 */
PpsDocumentInfo *
pps_document_info_new (void)
{
        return g_new0 (PpsDocumentInfo, 1);
}

/**
 * pps_document_info_copy:
 * @info: a #PpsDocumentInfo
 *
 * Returns: (transfer full): a copy of @info
 */
PpsDocumentInfo *
pps_document_info_copy (PpsDocumentInfo *info)
{
        PpsDocumentInfo *copy;

        g_return_val_if_fail (info != NULL, NULL);

        copy = pps_document_info_new ();

	copy->title = g_strdup (info->title);
	copy->format = g_strdup (info->format);
	copy->author = g_strdup (info->author);
	copy->subject = g_strdup (info->subject);
	copy->keywords = g_strdup (info->keywords);
	copy->security = g_strdup (info->security);
	copy->creator = g_strdup (info->creator);
	copy->producer = g_strdup (info->producer);
	copy->linearized = g_strdup (info->linearized);

        copy->creation_datetime = g_date_time_ref (info->creation_datetime);
        copy->modified_datetime = g_date_time_ref (info->modified_datetime);

	copy->layout = info->layout;
	copy->mode = info->mode;
	copy->ui_hints = info->ui_hints;
	copy->permissions = info->permissions;
	copy->n_pages = info->n_pages;
	copy->license = pps_document_license_copy (info->license);

	copy->fields_mask = info->fields_mask;

        return copy;
}

/**
 * pps_document_info_free:
 * @info: (transfer full): a #PpsDocumentInfo
 *
 * Frees @info.
 */
void
pps_document_info_free (PpsDocumentInfo *info)
{
        if (info == NULL)
                return;

	g_free (info->title);
	g_free (info->format);
	g_free (info->author);
	g_free (info->subject);
	g_free (info->keywords);
	g_free (info->creator);
	g_free (info->producer);
	g_free (info->linearized);
	g_free (info->security);
	pps_document_license_free (info->license);

        g_clear_pointer (&info->creation_datetime, g_date_time_unref);
        g_clear_pointer (&info->modified_datetime, g_date_time_unref);

        g_free (info);
}

/*
 * pps_document_info_take_created_datetime:
 * @info: a #PpsDocumentInfo
 * @datetime: (transfer full): a #GDateTime
 *
 * Sets the #GDateTime for when the document was created.
 */
void
pps_document_info_take_created_datetime (PpsDocumentInfo *info,
                                        GDateTime      *datetime)
{
	g_return_if_fail (info != NULL);
	g_clear_pointer (&info->creation_datetime, g_date_time_unref);

	info->creation_datetime = datetime;
	info->fields_mask |= PPS_DOCUMENT_INFO_CREATION_DATETIME;
}

/**
 * pps_document_info_get_created_datetime:
 * @info: a #PpsDocumentInfo
 *
 * Returns: (transfer none) (nullable): a #GDateTime for when the document was created
 */
GDateTime *
pps_document_info_get_created_datetime (const PpsDocumentInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);
	g_return_val_if_fail (info->fields_mask & PPS_DOCUMENT_INFO_CREATION_DATETIME, NULL);

        return info->creation_datetime;
}

/*
 * pps_document_info_take_modified_datetime:
 * @info: a #PpsDocumentInfo
 * @datetime: (transfer full): a #GDateTime
 *
 * Sets the #GDateTime for when the document was last modified.
 */
void
pps_document_info_take_modified_datetime (PpsDocumentInfo *info,
                                         GDateTime      *datetime)
{
	g_return_if_fail (info != NULL);

	g_clear_pointer (&info->modified_datetime, g_date_time_unref);
	info->modified_datetime = datetime;
	info->fields_mask |= PPS_DOCUMENT_INFO_MOD_DATETIME;
}

/**
 * pps_document_info_get_modified_datetime:
 * @info: a #PpsDocumentInfo
 *
 * Returns: (transfer none) (nullable): a #GDateTime for when the document was last modified
 */
GDateTime *
pps_document_info_get_modified_datetime (const PpsDocumentInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);
	g_return_val_if_fail (info->fields_mask & PPS_DOCUMENT_INFO_MOD_DATETIME, NULL);

	return info->modified_datetime;
}

/*
 * pps_document_info_set_from_xmp:
 * @info: a #PpsDocumentInfo
 * @xmp: a XMP document
 * @size: the size of @xmp in bytes, or -1 if @xmp is a NUL-terminated string
 *
 * Parses the XMP document and sets @info from it.
 *
 * Returns: %TRUE iff @xmp could be successfully parsed as a XMP document
 */
gboolean
pps_document_info_set_from_xmp (PpsDocumentInfo *info,
                               const char     *xmp,
                               gssize          size)
{
        return pps_xmp_parse (xmp, size != -1 ? size : strlen (xmp), info);
}

/* PpsDocumentLicense */
G_DEFINE_BOXED_TYPE (PpsDocumentLicense, pps_document_license, pps_document_license_copy, pps_document_license_free)

/**
 * pps_document_license_new:
 *
 * Returns: (transfer full): a new, empty #PpsDocumentLicense
 */
PpsDocumentLicense *
pps_document_license_new (void)
{
	return g_new0 (PpsDocumentLicense, 1);
}

/**
 * pps_document_license_copy:
 * @license: (nullable): a #PpsDocumentLicense
 *
 * Returns: (transfer full): a copy of @license, or %NULL
 */
PpsDocumentLicense *
pps_document_license_copy (PpsDocumentLicense *license)
{
	PpsDocumentLicense *new_license;

	if (!license)
		return NULL;

	new_license = pps_document_license_new ();

	if (license->text)
		new_license->text = g_strdup (license->text);
	if (license->uri)
		new_license->uri = g_strdup (license->uri);
	if (license->web_statement)
		new_license->web_statement = g_strdup (license->web_statement);

	return new_license;
}

/**
 * pps_document_license_free:
 * @license: (transfer full): a #PpsDocumentLicense
 *
 * Frees @license.
 */
void
pps_document_license_free (PpsDocumentLicense *license)
{
	if (!license)
		return;

	g_free (license->text);
	g_free (license->uri);
	g_free (license->web_statement);

	g_free (license);
}

/**
 * pps_document_license_get_text:
 * @license: (transfer full): a #PpsDocumentLicense
 *
 * Returns: (transfer none) (nullable): the license text
 */
const gchar *
pps_document_license_get_text (PpsDocumentLicense *license)
{
	return license->text;
}

/**
 * pps_document_license_get_uri:
 * @license: (transfer full): a #PpsDocumentLicense
 *
 * Returns: (transfer none) (nullable): the license URI
 */
const gchar *
pps_document_license_get_uri (PpsDocumentLicense *license)
{
	return license->uri;
}

/**
 * pps_document_license_get_web_statement
 * @license: (transfer full): a #PpsDocumentLicense
 *
 * Returns: (transfer none) (nullable): the license web statement
 */
const gchar *
pps_document_license_get_web_statement (PpsDocumentLicense *license)
{
	return license->web_statement;
}
