/* pps-signature.c
 * this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2024 Jan-Michael Brummer <jan-michael.brummer1@volkswagen.de>
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

#include "config.h"

#include "pps-document-signatures.h"

typedef struct {
	char *destination_file;
	PpsCertificateInfo *certificate_info;
	char *password;
	int page;
	char *signature;
	char *signature_left;

	PpsRectangle *rect;
	GdkRGBA font_color;
	GdkRGBA border_color;
	GdkRGBA background_color;
	gdouble font_size;
	gdouble left_font_size;
	gdouble border_width;
	char *document_owner_password;
	char *document_user_password;
} PpsSignaturePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PpsSignature, pps_signature, G_TYPE_OBJECT);
#define GET_SIG_PRIVATE(o) pps_signature_get_instance_private (o)

static void
pps_signature_finalize (GObject *object)
{
	PpsSignature *self = PPS_SIGNATURE (object);
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	g_clear_pointer (&priv->certificate_info, pps_certificate_info_free);
	g_clear_pointer (&priv->destination_file, g_free);
	g_clear_pointer (&priv->password, g_free);
	g_clear_pointer (&priv->signature, g_free);
	g_clear_pointer (&priv->signature_left, g_free);
	g_clear_pointer (&priv->rect, pps_rectangle_free);
	g_clear_pointer (&priv->document_owner_password, g_free);
	g_clear_pointer (&priv->document_user_password, g_free);

	G_OBJECT_CLASS (pps_signature_parent_class)->finalize (object);
}

static void
pps_signature_init (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	gdk_rgba_parse (&priv->font_color, "#000000");
	gdk_rgba_parse (&priv->border_color, "#000000");
	gdk_rgba_parse (&priv->background_color, "#F0F0F0");

	priv->font_size = 10.0;
	priv->left_font_size = 20.0;
	priv->border_width = 1.5;
	priv->certificate_info = NULL;
}

static void
pps_signature_class_init (PpsSignatureClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->finalize = pps_signature_finalize;
}

PpsSignature *
pps_signature_new (void)
{
	return PPS_SIGNATURE (g_object_new (PPS_TYPE_SIGNATURE, 0));
}

void
pps_signature_set_certificate_info (PpsSignature             *self,
                                    const PpsCertificateInfo *certificate_info)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	g_clear_object (&priv->certificate_info);
	priv->certificate_info = pps_certificate_info_copy (certificate_info);
}

PpsCertificateInfo *
pps_signature_get_certificate_info (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->certificate_info;
}

void
pps_signature_set_destination_file (PpsSignature *self,
                                    const char   *file)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	g_clear_pointer (&priv->destination_file, g_free);
	priv->destination_file = g_strdup (file);
}

const char *
pps_signature_get_destination_file (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->destination_file;
}

void
pps_signature_set_page (PpsSignature *self,
                        guint         page)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	priv->page = page;
}

gint
pps_signature_get_page (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->page;
}

void
pps_signature_set_rect (PpsSignature       *self,
                        const PpsRectangle *rectangle)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	g_clear_pointer (&priv->rect, pps_rectangle_free);
	priv->rect = pps_rectangle_copy ((PpsRectangle*)rectangle);
}

PpsRectangle *
pps_signature_get_rect (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->rect;
}

void
pps_signature_set_signature (PpsSignature *self,
                             const char   *signature)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	g_clear_pointer (&priv->signature, g_free);
	priv->signature = g_strdup (signature);
}

const char *
pps_signature_get_signature (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->signature;
}

void
pps_signature_set_signature_left (PpsSignature *self,
                                  const char   *signature_left)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	g_clear_pointer (&priv->signature_left, g_free);
	priv->signature_left = g_strdup (signature_left);
}

const char *
pps_signature_get_signature_left (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->signature_left;
}

const char *
pps_signature_get_password (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->password;
}

void
pps_signature_set_font_color (PpsSignature *self,
                              GdkRGBA      *color)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	priv->font_color.red = color->red;
	priv->font_color.green = color->green;
	priv->font_color.blue = color->blue;
	priv->font_color.alpha = color->alpha;
}

void
pps_signature_get_font_color (PpsSignature *self,
                              GdkRGBA      *color)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	color->red = priv->font_color.red;
	color->green = priv->font_color.green;
	color->blue = priv->font_color.blue;
	color->alpha = priv->font_color.alpha;
}

void
pps_signature_set_border_color (PpsSignature *self,
                                GdkRGBA      *color)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	priv->border_color.red = color->red;
	priv->border_color.green = color->green;
	priv->border_color.blue = color->blue;
	priv->border_color.alpha = color->alpha;
}

void
pps_signature_get_border_color (PpsSignature *self,
                                GdkRGBA      *color)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	color->red = priv->border_color.red;
	color->green = priv->border_color.green;
	color->blue = priv->border_color.blue;
	color->alpha = priv->border_color.alpha;
}

void
pps_signature_set_background_color (PpsSignature *self,
                                    GdkRGBA      *color)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	priv->background_color.red = color->red;
	priv->background_color.green = color->green;
	priv->background_color.blue = color->blue;
	priv->background_color.alpha = color->alpha;
}

void
pps_signature_get_background_color (PpsSignature *self,
                                    GdkRGBA      *color)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	color->red = priv->background_color.red;
	color->green = priv->background_color.green;
	color->blue = priv->background_color.blue;
	color->alpha = priv->background_color.alpha;
}

void
pps_signature_set_owner_password (PpsSignature *self,
                                  const char   *password)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	g_clear_pointer (&priv->document_owner_password, g_free);
	priv->document_owner_password = g_strdup (password);
}

const char *
pps_signature_get_owner_password (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->document_owner_password;
}

void
pps_signature_set_user_password (PpsSignature *self,
                                 const char   *password)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	g_clear_pointer (&priv->document_user_password, g_free);
	priv->document_user_password = g_strdup (password);
}

const char *
pps_signature_get_user_password (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->document_user_password;
}

void
pps_signature_set_font_size (PpsSignature *self,
                             gint          size)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	priv->font_size = size;
}

gint
pps_signature_get_font_size (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->font_size;
}

void
pps_signature_set_left_font_size (PpsSignature *self,
                                  gint          size)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	priv->left_font_size = size;
}

gint
pps_signature_get_left_font_size (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->left_font_size;
}

void
pps_signature_set_border_width (PpsSignature *self,
                                gint          width)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	priv->border_width = width;
}

gint
pps_signature_get_border_width (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->border_width;
}

/* Certificate Info */

G_DEFINE_BOXED_TYPE (PpsCertificateInfo, pps_certificate_info, pps_certificate_info_copy, pps_certificate_info_free)

PpsCertificateInfo *
pps_certificate_info_new(const char *id, const char *subject_common_name)
{
	PpsCertificateInfo *info = (PpsCertificateInfo *)g_malloc0(sizeof(PpsCertificateInfo));

	info->id = g_strdup (id);
	info->subject_common_name = g_strdup (subject_common_name);
	return info;
}

PpsCertificateInfo *
pps_certificate_info_copy (const PpsCertificateInfo *info)

{
	PpsCertificateInfo *info_copy = (PpsCertificateInfo *)g_malloc0(sizeof(PpsCertificateInfo));

	info_copy->id = g_strdup (info->id);
	info_copy->subject_common_name = g_strdup (info->subject_common_name);
	return info_copy;
}

void
pps_certificate_info_free (PpsCertificateInfo *info)
{
	g_clear_pointer (&info->id, g_free);
	g_clear_pointer (&info->subject_common_name, g_free);
}

const char *
pps_certificate_info_get_id (const PpsCertificateInfo *info)
{
	return info->id;
}

const char *
pps_certificate_info_get_subject_common_name (const PpsCertificateInfo *info)
{
	return info->subject_common_name;
}

