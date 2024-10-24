/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2004 Martin Kretzschmar
 *
 *  Author:
 *    Martin Kretzschmar <martink@gnome.org>
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

#include "pps-file-exporter.h"
#include "pps-document.h"
#include <config.h>

G_DEFINE_INTERFACE (PpsFileExporter, pps_file_exporter, 0)

static void
pps_file_exporter_default_init (PpsFileExporterInterface *klass)
{
}

void
pps_file_exporter_begin (PpsFileExporter *exporter,
                         PpsFileExporterContext *fc)
{
	PpsFileExporterInterface *iface = PPS_FILE_EXPORTER_GET_IFACE (exporter);

	iface->begin (exporter, fc);
}

void
pps_file_exporter_begin_page (PpsFileExporter *exporter)
{
	PpsFileExporterInterface *iface = PPS_FILE_EXPORTER_GET_IFACE (exporter);

	if (iface->begin_page)
		iface->begin_page (exporter);
}

void
pps_file_exporter_do_page (PpsFileExporter *exporter,
                           PpsRenderContext *rc)
{
	PpsFileExporterInterface *iface = PPS_FILE_EXPORTER_GET_IFACE (exporter);

	iface->do_page (exporter, rc);
}

void
pps_file_exporter_end_page (PpsFileExporter *exporter)
{
	PpsFileExporterInterface *iface = PPS_FILE_EXPORTER_GET_IFACE (exporter);

	if (iface->end_page)
		iface->end_page (exporter);
}

void
pps_file_exporter_end (PpsFileExporter *exporter)
{
	PpsFileExporterInterface *iface = PPS_FILE_EXPORTER_GET_IFACE (exporter);

	iface->end (exporter);
}

PpsFileExporterCapabilities
pps_file_exporter_get_capabilities (PpsFileExporter *exporter)
{
	PpsFileExporterInterface *iface = PPS_FILE_EXPORTER_GET_IFACE (exporter);

	return iface->get_capabilities (exporter);
}
