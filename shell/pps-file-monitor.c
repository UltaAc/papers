/* pps-file-monitor.c
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

#include "config.h"

#include <glib.h>
#include <gio/gio.h>

#include "pps-file-monitor.h"

enum {
	CHANGED,
	N_SIGNALS
};

typedef struct {
	GFileMonitor *monitor;

	guint         timeout_id;
} PpsFileMonitorPrivate;

static void pps_file_monitor_timeout_start (PpsFileMonitor    *pps_monitor);
static void pps_file_monitor_timeout_stop  (PpsFileMonitor    *pps_monitor);
static void pps_file_monitor_changed_cb    (GFileMonitor     *monitor,
					   GFile            *file,
					   GFile            *other_file,
					   GFileMonitorEvent event_type,
					   PpsFileMonitor    *pps_monitor);

G_DEFINE_TYPE_WITH_PRIVATE (PpsFileMonitor, pps_file_monitor, G_TYPE_OBJECT)

#define GET_PRIVATE(o) pps_file_monitor_get_instance_private (o)

static guint signals[N_SIGNALS];

static void
pps_file_monitor_init (PpsFileMonitor *pps_monitor)
{
}

static void
pps_file_monitor_finalize (GObject *object)
{
	PpsFileMonitor *pps_monitor = PPS_FILE_MONITOR (object);
	PpsFileMonitorPrivate *priv = GET_PRIVATE (pps_monitor);

	pps_file_monitor_timeout_stop (pps_monitor);

	if (priv->monitor) {
		g_signal_handlers_disconnect_by_func (priv->monitor,
						      pps_file_monitor_changed_cb,
						      pps_monitor);
		g_clear_object (&priv->monitor);
	}

	G_OBJECT_CLASS (pps_file_monitor_parent_class)->finalize (object);
}

static void
pps_file_monitor_class_init (PpsFileMonitorClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->finalize = pps_file_monitor_finalize;

	/* Signals */
	signals[CHANGED] =
		g_signal_new ("changed",
			      PPS_TYPE_FILE_MONITOR,
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (PpsFileMonitorClass, changed),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);
}

static void
timeout_cb (PpsFileMonitor *pps_monitor)
{
	PpsFileMonitorPrivate *priv = GET_PRIVATE (pps_monitor);

	g_signal_emit (pps_monitor, signals[CHANGED], 0);

	priv->timeout_id = 0;
}

static void
pps_file_monitor_timeout_start (PpsFileMonitor *pps_monitor)
{
	PpsFileMonitorPrivate *priv = GET_PRIVATE (pps_monitor);

	pps_file_monitor_timeout_stop (pps_monitor);

	priv->timeout_id =
		g_timeout_add_once (5000, (GSourceOnceFunc)timeout_cb, pps_monitor);
}

static void
pps_file_monitor_timeout_stop (PpsFileMonitor *pps_monitor)
{
	PpsFileMonitorPrivate *priv = GET_PRIVATE (pps_monitor);

	g_clear_handle_id (&priv->timeout_id, g_source_remove);
}

static void
pps_file_monitor_changed_cb (GFileMonitor     *monitor,
			    GFile            *file,
			    GFile            *other_file,
			    GFileMonitorEvent event_type,
			    PpsFileMonitor    *pps_monitor)
{
	switch (event_type) {
	case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
		pps_file_monitor_timeout_stop (pps_monitor);
		g_signal_emit (pps_monitor, signals[CHANGED], 0);

		break;
	case G_FILE_MONITOR_EVENT_CHANGED:
		pps_file_monitor_timeout_start (pps_monitor);
		break;
	default:
		break;
	}
}

PpsFileMonitor *
pps_file_monitor_new (const gchar *uri)
{
	PpsFileMonitor *pps_monitor;
	PpsFileMonitorPrivate *priv;
	GFile         *file;
	GError        *error = NULL;

	pps_monitor = PPS_FILE_MONITOR (g_object_new (PPS_TYPE_FILE_MONITOR, NULL));
	priv = GET_PRIVATE (pps_monitor);

	file = g_file_new_for_uri (uri);
	priv->monitor = g_file_monitor_file (file, G_FILE_MONITOR_NONE, NULL, &error);
	if (priv->monitor) {
		g_signal_connect (priv->monitor, "changed",
				  G_CALLBACK (pps_file_monitor_changed_cb), pps_monitor);
	} else if (error) {
		g_warning ("%s", error->message);
		g_error_free (error);
	}

	g_object_unref (file);

	return pps_monitor;
}
