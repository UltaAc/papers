/* pps-job-scheduler.c
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

#include "pps-debug.h"
#include "pps-job-scheduler.h"

typedef struct _PpsSchedulerJob {
	PpsJob         *job;
	PpsJobPriority  priority;
} PpsSchedulerJob;

static gpointer
pps_jobs_hash_init (gpointer data)
{
	return g_hash_table_new (NULL, NULL);
}

static GHashTable *
pps_jobs_hash (void)
{
	static GOnce once_init = G_ONCE_INIT;
	g_once (&once_init, pps_jobs_hash_init, NULL);
	return once_init.retval;
}

static void
pps_scheduler_job_free (PpsSchedulerJob *job)
{
	if (!job)
		return;

	g_object_unref (job->job);
	g_free (job);
}

static void
pps_scheduler_job_destroy (PpsSchedulerJob *job)
{
	pps_debug_message (DEBUG_JOBS, "%s", PPS_GET_TYPE_NAME (job->job));

	if (pps_job_get_run_mode (job->job) == PPS_JOB_RUN_MAIN_LOOP) {
		g_signal_handlers_disconnect_by_func (job->job,
						      G_CALLBACK (pps_scheduler_job_destroy),
						      job);
	}

	g_hash_table_remove (pps_jobs_hash (), job);
	pps_scheduler_job_free (job);
}

static void
pps_job_thread (PpsSchedulerJob *scheduler_job, gpointer data)
{
	PpsJob *job = scheduler_job->job;

	pps_debug_message (DEBUG_JOBS, "%s", PPS_GET_TYPE_NAME (job));

	if (!g_cancellable_is_cancelled (job->cancellable))
		pps_job_run (job);

	pps_scheduler_job_destroy (scheduler_job);
}

static gint
job_priority_compare (PpsSchedulerJob *a,
		      PpsSchedulerJob *b,
		      gpointer data)
{
	return (gint)a->priority - (gint)b->priority;
}

static gpointer
pps_job_scheduler_init (gpointer data)
{
	/* We limit the thread numbers to 8 since threads above 8 may result
	 * increased latency on some machine. For example, AMD 5950x.
	 */
	GThreadPool *pool = g_thread_pool_new_full ((GFunc)pps_job_thread, NULL,
                       (GDestroyNotify)pps_scheduler_job_destroy,
                       MIN (8, g_get_num_processors()), TRUE, NULL);

	g_thread_pool_set_sort_function(pool, (GCompareDataFunc)job_priority_compare, NULL);

	return pool;
}

static GThreadPool *
pps_thread_pool (void)
{
	static GOnce once_init = G_ONCE_INIT;
	g_once (&once_init, pps_job_scheduler_init, NULL);
	return once_init.retval;
}

static void
pps_job_queue_push (PpsSchedulerJob *job)
{
	pps_debug_message (DEBUG_JOBS, "%s priority %d", PPS_GET_TYPE_NAME (job->job), job->priority);

	g_hash_table_insert (pps_jobs_hash (), job->job, job);
	g_thread_pool_push (pps_thread_pool (), job, NULL);
}

static gboolean
pps_job_idle (PpsJob *job)
{
	pps_debug_message (DEBUG_JOBS, "%s", PPS_GET_TYPE_NAME (job));

	if (g_cancellable_is_cancelled (job->cancellable))
		return G_SOURCE_REMOVE;

	return pps_job_run (job);
}

void
pps_job_scheduler_push_job (PpsJob         *job,
			   PpsJobPriority  priority)
{
	PpsSchedulerJob *s_job;

	pps_debug_message (DEBUG_JOBS, "%s priority %d", PPS_GET_TYPE_NAME (job), priority);

	s_job = g_new0 (PpsSchedulerJob, 1);
	s_job->job = g_object_ref (job);
	s_job->priority = priority;

	switch (pps_job_get_run_mode (job)) {
	case PPS_JOB_RUN_THREAD:
		pps_job_queue_push (s_job);
		break;
	case PPS_JOB_RUN_MAIN_LOOP:
		g_signal_connect_swapped (job, "finished",
					  G_CALLBACK (pps_scheduler_job_destroy),
					  s_job);
		g_signal_connect_swapped (job, "cancelled",
					  G_CALLBACK (pps_scheduler_job_destroy),
					  s_job);
		g_idle_add_full (G_PRIORITY_DEFAULT_IDLE,
				 (GSourceFunc)pps_job_idle,
				 g_object_ref (job),
				 (GDestroyNotify)g_object_unref);
		break;
	default:
		g_assert_not_reached ();
	}
}

void
pps_job_scheduler_update_job (PpsJob         *job,
			     PpsJobPriority  priority)
{
	/* Main loop jobs are scheduled immediately */
	if (pps_job_get_run_mode (job) == PPS_JOB_RUN_MAIN_LOOP)
		return;

	pps_debug_message (DEBUG_JOBS, "%s priority %d", PPS_GET_TYPE_NAME (job), priority);

	if (priority == PPS_JOB_PRIORITY_URGENT)
		g_thread_pool_move_to_front (pps_thread_pool(),
				g_hash_table_lookup (pps_jobs_hash (), job));
}

/**
 * pps_job_scheduler_wait:
 *
 * Synchronously waits until all jobs are done.
 * Remember that main loop is not running already probably.
 */
void
pps_job_scheduler_wait (void)
{
	pps_debug_message (DEBUG_JOBS, "Waiting for empty job list");

	while (g_thread_pool_unprocessed (pps_thread_pool()))
		g_usleep (100);

	pps_debug_message (DEBUG_JOBS, "Job list is empty");
}
