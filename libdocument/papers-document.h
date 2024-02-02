/*
 * Copyright © 2009 Christian Persch
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#define __PPS_PAPERS_DOCUMENT_H_INSIDE__

#ifndef I_KNOW_THE_PAPERS_LIBS_ARE_UNSTABLE_AND_HAVE_TALKED_WITH_THE_AUTHORS
#error You have to define I_KNOW_THE_PAPERS_LIBS_ARE_UNSTABLE_AND_HAVE_TALKED_WITH_THE_AUTHORS. And please! Contact the authors
#endif

#include <libdocument/pps-annotation.h>
#include <libdocument/pps-attachment.h>
#include <libdocument/pps-document-annotations.h>
#include <libdocument/pps-document-attachments.h>
#include <libdocument/pps-document-factory.h>
#include <libdocument/pps-document-find.h>
#include <libdocument/pps-document-fonts.h>
#include <libdocument/pps-document-forms.h>
#include <libdocument/pps-document-images.h>
#include <libdocument/pps-document-info.h>
#include <libdocument/pps-document-layers.h>
#include <libdocument/pps-document-links.h>
#include <libdocument/pps-document-media.h>
#include <libdocument/pps-document-misc.h>
#include <libdocument/pps-document-print.h>
#include <libdocument/pps-document-security.h>
#include <libdocument/pps-document-text.h>
#include <libdocument/pps-document-transition.h>
#include <libdocument/pps-document-type-builtins.h>
#include <libdocument/pps-document.h>
#include <libdocument/pps-file-exporter.h>
#include <libdocument/pps-file-helpers.h>
#include <libdocument/pps-font-description.h>
#include <libdocument/pps-form-field.h>
#include <libdocument/pps-image.h>
#include <libdocument/pps-init.h>
#include <libdocument/pps-layer.h>
#include <libdocument/pps-link-action.h>
#include <libdocument/pps-link-dest.h>
#include <libdocument/pps-link.h>
#include <libdocument/pps-macros.h>
#include <libdocument/pps-mapping-list.h>
#include <libdocument/pps-media.h>
#include <libdocument/pps-page.h>
#include <libdocument/pps-render-context.h>
#include <libdocument/pps-selection.h>
#include <libdocument/pps-transition-effect.h>
#include <libdocument/pps-version.h>

#undef __PPS_PAPERS_DOCUMENT_H_INSIDE__
