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

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "pps-signature.h"
#include "pps-document-signatures.h"
#include "pps-document-type-builtins.h"

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

	char                 *signer_name;
	PpsSignatureStatus    signature_status;
	PpsCertificateStatus  certificate_status;
	GDateTime            *signature_time;
} PpsSignaturePrivate;

enum {
	PROP_0,
	PROP_SIGNER_NAME,
	PROP_SIGNATURE_STATUS,
	PROP_CERTIFICATE_STATUS,
	PROP_SIGN_TIME
};

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
	g_clear_pointer (&priv->signer_name, g_free);
	g_clear_pointer (&priv->signature_time, g_date_time_unref);

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
pps_signature_set_property (GObject      *object,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *param_spec)
{
	PpsSignature *self = PPS_SIGNATURE (object);
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	switch (property_id) {
		case PROP_SIGNER_NAME:
			priv->signer_name = g_value_dup_string (value);
			break;

		case PROP_SIGNATURE_STATUS:
			priv->signature_status = g_value_get_enum (value);
			break;

		case PROP_CERTIFICATE_STATUS:
			priv->certificate_status = g_value_get_enum (value);
			break;

		case PROP_SIGN_TIME:
			priv->signature_time = g_value_get_boxed (value);
			if (priv->signature_time)
				g_date_time_ref (priv->signature_time);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object,
							   property_id,
							   param_spec);
			break;
	}
}

static void
pps_signature_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *param_spec)
{
	PpsSignature *self = PPS_SIGNATURE (object);
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	switch (property_id) {
		case PROP_SIGNER_NAME:
			g_value_set_string (value, priv->signer_name);
			break;

		case PROP_SIGNATURE_STATUS:
			g_value_set_enum (value, priv->signature_status);
			break;

		case PROP_CERTIFICATE_STATUS:
			g_value_set_enum (value, priv->certificate_status);
			break;

		case PROP_SIGN_TIME:
			g_value_set_pointer (value, priv->signature_time);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object,
							   property_id,
							   param_spec);
			break;
	}
}

static void
pps_signature_class_init (PpsSignatureClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->finalize = pps_signature_finalize;
	g_object_class->set_property = pps_signature_set_property;
	g_object_class->get_property = pps_signature_get_property;

	/* Properties */
	g_object_class_install_property (g_object_class,
					 PROP_SIGNER_NAME,
					 g_param_spec_string ("signer-name",
							      "Name",
							      "The name of the entity that signed",
							      NULL,
							      G_PARAM_READWRITE |
							      G_PARAM_CONSTRUCT_ONLY |
							      G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (g_object_class,
					 PROP_SIGNATURE_STATUS,
					 g_param_spec_enum ("signature-status",
							   "SignatureStatus",
							   "Status of the signature",
							   PPS_TYPE_SIGNATURE_STATUS,
							   PPS_SIGNATURE_STATUS_INVALID,
							   G_PARAM_READWRITE |
							   G_PARAM_CONSTRUCT_ONLY |
							   G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (g_object_class,
					 PROP_CERTIFICATE_STATUS,
					 g_param_spec_enum ("certificate-status",
							   "CertificateStatus",
							   "Status of the certificate",
							   PPS_TYPE_CERTIFICATE_STATUS,
							   PPS_CERTIFICATE_STATUS_NOT_VERIFIED,
							   G_PARAM_READWRITE |
							   G_PARAM_CONSTRUCT_ONLY |
							   G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (g_object_class,
					 PROP_SIGN_TIME,
					 g_param_spec_boxed ("signature-time",
							     "SignatureTime",
							     "The time associated with the signature",
							     G_TYPE_DATE_TIME,
							     G_PARAM_READWRITE |
							     G_PARAM_CONSTRUCT_ONLY |
							     G_PARAM_STATIC_STRINGS));

	g_object_class->finalize = pps_signature_finalize;
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


/**
 * pps_signature_new:
 * @signer_name: name of signer who signed this document
 * @signature_status: status of the signature
 * @certificate_status: status of the certificate of the signature
 * @signature_time: time of signing of the document
 *
 * Returns: Newly allocated #PpsSignature object initialized with given parameters
 */
PpsSignature *
pps_signature_new (const gchar          *signer_name,
                   PpsSignatureStatus    signature_status,
                   PpsCertificateStatus  certificate_status,
                   GDateTime            *signature_time)
{
	PpsSignature *signature = g_object_new (PPS_TYPE_SIGNATURE,
					       "signer-name", signer_name,
					       "signature-status", signature_status,
					       "certificate-status", certificate_status,
					       "signature-time", signature_time,
					       NULL);

	return signature;
}

/**
 * pps_signature_get_certificate_status:
 * @self: a #PpsSignature
 *
 * Returns: a #PpsCertificateStatus
 */
PpsCertificateStatus
pps_signature_get_certificate_status (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->certificate_status;
}

/**
 * pps_signature_get_signature_status:
 * @self: a #PpsSignature
 *
 * Returns: a #PpsSignatureStatus
 */
PpsSignatureStatus
pps_signature_get_signature_status (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->signature_status;
}

/**
 * pps_signature_certificate_status_str:
 * @status: a #PpsCertificateStatus
 *
 * Converts certificate status to string.
 *
 * Returns: certificate status as string
 */
char *
pps_signature_certificate_status_str (PpsCertificateStatus status)
{
	switch (status) {
	case PPS_CERTIFICATE_STATUS_TRUSTED:
		return g_strdup (_("Trusted"));
	case PPS_CERTIFICATE_STATUS_UNTRUSTED_ISSUER:
		return g_strdup (_("Untrusted issuer"));
	case PPS_CERTIFICATE_STATUS_UNKNOWN_ISSUER:
		return g_strdup (_("Unknown issuer"));
	case PPS_CERTIFICATE_STATUS_REVOKED:
		return g_strdup (_("Revoked"));
	case PPS_CERTIFICATE_STATUS_EXPIRED:
		return g_strdup (_("Expired"));
	case PPS_CERTIFICATE_STATUS_NOT_VERIFIED:
		return g_strdup (_("Not verified"));
	case PPS_CERTIFICATE_STATUS_GENERIC_ERROR:
	default:
		return g_strdup (_("Generic error"));
	}

	return g_strdup (_("Unknown status"));
}

/**
 * pps_signature_signatures_status_str:
 * @status: a #PpsSignatureStatus
 *
 * Converts signature status to string.
 *
 * Returns: signature status as string
 */
char *
pps_signature_signature_status_str (PpsSignatureStatus status)
{
	switch (status) {
	case PPS_SIGNATURE_STATUS_VALID:
		return g_strdup (_("Valid"));
	case PPS_SIGNATURE_STATUS_INVALID:
		return g_strdup (_("Invalid"));
	case PPS_SIGNATURE_STATUS_DIGEST_MISMATCH:
		return g_strdup (_("Digest mismatch"));
	case PPS_SIGNATURE_STATUS_DECODING_ERROR:
		return g_strdup (_("Decoding error"));
	case PPS_SIGNATURE_STATUS_GENERIC_ERROR:
	default:
		return g_strdup (_("Generic error"));
	}

	return g_strdup (_("Unknown status"));
}

/**
 * pps_signature_get_signature_time:
 * @self: a #PpsSignature
 *
 * Get signature time
 *
 * Returns: signature time
 */
GDateTime *
pps_signature_get_signature_time (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->signature_time;
}

/**
 * pps_signature_is_valid:
 * @self: a #PpsSignature
 *
 * Checks whether signature is valid
 *
 * Returns: signature valid status
 */
gboolean
pps_signature_is_valid (PpsSignature *self)
{
	PpsSignaturePrivate *priv = GET_SIG_PRIVATE (self);

	return priv->certificate_status == PPS_CERTIFICATE_STATUS_TRUSTED && priv->signature_status == PPS_SIGNATURE_STATUS_VALID;
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

