/* pps-file-monitor.h
 *  this file is part of papers, a gnome document viewer
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

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _PpsFileMonitor        PpsFileMonitor;
typedef struct _PpsFileMonitorClass   PpsFileMonitorClass;

#define PPS_TYPE_FILE_MONITOR              (pps_file_monitor_get_type())
#define PPS_FILE_MONITOR(object)           (G_TYPE_CHECK_INSTANCE_CAST((object), PPS_TYPE_FILE_MONITOR, PpsFileMonitor))
#define PPS_FILE_MONITOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), PPS_TYPE_FILE_MONITOR, PpsFileMonitorClass))
#define PPS_IS_FILE_MONITOR(object)        (G_TYPE_CHECK_INSTANCE_TYPE((object), PPS_TYPE_FILE_MONITOR))
#define PPS_IS_FILE_MONITOR_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass), PPS_TYPE_FILE_MONITOR))
#define PPS_FILE_MONITOR_GET_CLASS(object) (G_TYPE_INSTANCE_GET_CLASS((object), PPS_TYPE_FILE_MONITOR, PpsFileMonitorClass))

struct _PpsFileMonitor {
	GObject base_instance;
};

struct _PpsFileMonitorClass {
	GObjectClass base_class;

	/* Signals */
	void (*changed) (PpsFileMonitor *pps_monitor);
};

GType          pps_file_monitor_get_type (void) G_GNUC_CONST;
PpsFileMonitor *pps_file_monitor_new      (const gchar *uri);

G_END_DECLS
