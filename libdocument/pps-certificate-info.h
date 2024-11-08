/* pps-certificate-info.h
 * this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2024 Jan-Michael Brummer <jan-michael.brummer1@volkswagen.de>
 * Copyright (C) 2024 Marek Kasik <mkasik@redhat.com>
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

#include "pps-document.h"

G_BEGIN_DECLS

#define PPS_TYPE_CERTIFICATE_INFO (pps_certificate_info_get_type ())

PPS_PUBLIC
G_DECLARE_FINAL_TYPE (PpsCertificateInfo, pps_certificate_info, PPS, CERTIFICATE_INFO, GObject);

struct _PpsCertificateInfo {
	GObject base_instance;
};

typedef enum {
	PPS_CERTIFICATE_STATUS_TRUSTED = 0,
	PPS_CERTIFICATE_STATUS_UNTRUSTED_ISSUER,
	PPS_CERTIFICATE_STATUS_UNKNOWN_ISSUER,
	PPS_CERTIFICATE_STATUS_REVOKED,
	PPS_CERTIFICATE_STATUS_EXPIRED,
	PPS_CERTIFICATE_STATUS_GENERIC_ERROR,
	PPS_CERTIFICATE_STATUS_NOT_VERIFIED
} PpsCertificateStatus;

PPS_PUBLIC
PpsCertificateInfo *pps_certificate_info_new (const char *id,
                                              const char *subject_common_name);
