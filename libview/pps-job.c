/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2008 Carlos Garcia Campos <carlosgc@gnome.org>
 *  Copyright (C) 2005 Red Hat, Inc
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

#include <config.h>

#include "pps-job.h"
#include "pps-debug.h"

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif
#define G_LOG_DOMAIN "PpsJobs"

enum {
	PROP_0,
	PROP_DOCUMENT,
};

enum {
	CANCELLED,
	FINISHED,
	LAST_SIGNAL
};

static guint job_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_ABSTRACT_TYPE (PpsJob, pps_job, G_TYPE_OBJECT)

static void
pps_job_init (PpsJob *job)
{
	job->cancellable = g_cancellable_new ();
}

static void
pps_job_dispose (GObject *object)
{
	PpsJob *job = PPS_JOB (object);

	g_debug ("disposing %s (%p)", PPS_GET_TYPE_NAME(job), job);

	g_clear_object (&job->document);
	g_clear_object (&job->cancellable);
	g_clear_error (&job->error);

	(* G_OBJECT_CLASS (pps_job_parent_class)->dispose) (object);
}

static void
pps_job_set_property (GObject      *object,
		     guint         prop_id,
		     const GValue *value,
		     GParamSpec   *pspec)
{
	PpsJob *job = PPS_JOB (object);

	switch (prop_id) {
	case PROP_DOCUMENT:
		job->document = g_value_dup_object (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_job_class_init (PpsJobClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->dispose = pps_job_dispose;
	g_object_class->set_property = pps_job_set_property;

	job_signals[CANCELLED] =
		g_signal_new ("cancelled",
			      PPS_TYPE_JOB,
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (PpsJobClass, cancelled),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);
	job_signals [FINISHED] =
		g_signal_new ("finished",
			      PPS_TYPE_JOB,
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (PpsJobClass, finished),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);

	g_object_class_install_property (g_object_class,
					 PROP_DOCUMENT,
					 g_param_spec_object ("document",
							      "Document",
							      "The document",
							      PPS_TYPE_DOCUMENT,
							      G_PARAM_WRITABLE |
							      G_PARAM_CONSTRUCT_ONLY |
							      G_PARAM_STATIC_STRINGS));
}

static gboolean
emit_finished (PpsJob *job)
{
	g_debug ("emit finished for %s (%p)", PPS_GET_TYPE_NAME (job), job);

	job->idle_finished_id = 0;

	if (job->cancelled)
		g_debug ("%s (%p) job was cancelled, do not emit finished", PPS_GET_TYPE_NAME (job), job);
	else
		g_signal_emit (job, job_signals[FINISHED], 0);

	return G_SOURCE_REMOVE;
}

static void
pps_job_emit_finished (PpsJob *job)
{
	if (g_cancellable_is_cancelled (job->cancellable)) {
		g_debug ("%s (%p) job was cancelled, returning", PPS_GET_TYPE_NAME (job), job);
		return;
	}

	job->finished = TRUE;

	job->idle_finished_id =
		g_idle_add_full (G_PRIORITY_DEFAULT_IDLE,
				 (GSourceFunc)emit_finished,
				 g_object_ref (job),
				 (GDestroyNotify)g_object_unref);
}

gboolean
pps_job_run (PpsJob *job)
{
	PpsJobClass *class = PPS_JOB_GET_CLASS (job);
	gboolean ret;

	PPS_PROFILER_START (PPS_GET_TYPE_NAME (job), g_strdup("running"));
	ret = class->run (job);
	PPS_PROFILER_STOP ();
	return ret;
}

void
pps_job_cancel (PpsJob *job)
{
	if (job->cancelled)
		return;

	g_debug ("job %s (%p) cancelled", PPS_GET_TYPE_NAME (job), job);

	/* This should never be called from a thread */
	job->cancelled = TRUE;
	g_cancellable_cancel (job->cancellable);

        if (job->finished && job->idle_finished_id == 0)
                return;

	g_signal_emit (job, job_signals[CANCELLED], 0);
}

void
pps_job_failed (PpsJob       *job,
	       GQuark       domain,
	       gint         code,
	       const gchar *format,
	       ...)
{
	va_list args;
	gchar  *message;

	if (job->failed || job->finished)
		return;

	g_debug ("job %s (%p) failed", PPS_GET_TYPE_NAME (job), job);

	job->failed = TRUE;

	va_start (args, format);
	message = g_strdup_vprintf (format, args);
	va_end (args);

	job->error = g_error_new_literal (domain, code, message);
	g_free (message);

	pps_job_emit_finished (job);
}

/**
 * pps_job_failed_from_error: (rename-to pps_job_failed)
 * @job: an #PpsJob
 * @error: a #GError
 */
void
pps_job_failed_from_error (PpsJob  *job,
			  GError *error)
{
	if (job->failed || job->finished)
		return;

	g_debug ("job %s (%p) failed", PPS_GET_TYPE_NAME (job), job);

	job->failed = TRUE;
	job->error = g_error_copy (error);

	pps_job_emit_finished (job);
}

void
pps_job_succeeded (PpsJob *job)
{
	if (job->finished)
		return;

	g_debug ("job %s (%p) succeeded", PPS_GET_TYPE_NAME (job), job);

	job->failed = FALSE;
	pps_job_emit_finished (job);
}

gboolean
pps_job_is_finished (PpsJob *job)
{
	return job->finished;
}

/**
 * pps_job_is_succeeded:
 * @job: a #PpsJob
 * @error: (nullable): (transfer full): the error to set if the job failed
 *
 * Returns: whether the job succeed
 */
gboolean
pps_job_is_succeeded (PpsJob   *job, GError **error)
{
	g_return_val_if_fail (job != NULL, FALSE);

	if (job->failed && error)
		*error = g_error_copy (job->error);

	return !job->failed;
}

/**
 * pps_job_get_document:
 * @job: an #PpsJob
 *
 * Returns: (transfer none): The #PpsDocument of this job.
 */
PpsDocument *
pps_job_get_document (PpsJob	 *job)
{
	g_return_val_if_fail (PPS_IS_JOB (job), NULL);

	return job->document;
}
