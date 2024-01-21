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
	GSList        *job_link;
} PpsSchedulerJob;

G_LOCK_DEFINE_STATIC(job_list);
static GSList *job_list = NULL;

static PpsJob *running_job = NULL;

static gpointer pps_job_thread_proxy               (gpointer        data);
static void     pps_scheduler_thread_job_cancelled (PpsSchedulerJob *job,
						   GCancellable   *cancellable);

/* PpsJobQueue */
static GQueue queue_urgent = G_QUEUE_INIT;
static GQueue queue_high = G_QUEUE_INIT;
static GQueue queue_low = G_QUEUE_INIT;
static GQueue queue_none = G_QUEUE_INIT;
static GCond job_queue_cond;
static GMutex job_queue_mutex;

static GQueue *job_queue[PPS_JOB_N_PRIORITIES] = {
	&queue_urgent,
	&queue_high,
	&queue_low,
	&queue_none
};

static void
pps_job_queue_push (PpsSchedulerJob *job,
		   PpsJobPriority   priority)
{
	pps_debug_message (DEBUG_JOBS, "%s priority %d", PPS_GET_TYPE_NAME (job->job), priority);

	g_mutex_lock (&job_queue_mutex);

	g_queue_push_tail (job_queue[priority], job);
	g_cond_broadcast (&job_queue_cond);

	g_mutex_unlock (&job_queue_mutex);
}

static PpsSchedulerJob *
pps_job_queue_get_next_unlocked (void)
{
	gint i;
	PpsSchedulerJob *job = NULL;

	for (i = PPS_JOB_PRIORITY_URGENT; i < PPS_JOB_N_PRIORITIES; i++) {
		job = (PpsSchedulerJob *) g_queue_pop_head (job_queue[i]);
		if (job)
			break;
	}

	pps_debug_message (DEBUG_JOBS, "%s", job ? PPS_GET_TYPE_NAME (job->job) : "No jobs in queue");

	return job;
}

static gpointer
pps_job_scheduler_init (gpointer data)
{
	g_thread_new ("PpsJobScheduler", pps_job_thread_proxy, NULL);

	return NULL;
}

static void
pps_scheduler_job_list_add (PpsSchedulerJob *job)
{
	pps_debug_message (DEBUG_JOBS, "%s", PPS_GET_TYPE_NAME (job->job));

	G_LOCK (job_list);

	job_list = g_slist_prepend (job_list, job);
	job->job_link = job_list;

	G_UNLOCK (job_list);
}

static void
pps_scheduler_job_list_remove (PpsSchedulerJob *job)
{
	pps_debug_message (DEBUG_JOBS, "%s", PPS_GET_TYPE_NAME (job->job));

	G_LOCK (job_list);

	job_list = g_slist_delete_link (job_list, job->job_link);

	G_UNLOCK (job_list);
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

	if (job->job->run_mode == PPS_JOB_RUN_MAIN_LOOP) {
		g_signal_handlers_disconnect_by_func (job->job,
						      G_CALLBACK (pps_scheduler_job_destroy),
						      job);
	} else {
		g_signal_handlers_disconnect_by_func (job->job->cancellable,
						      G_CALLBACK (pps_scheduler_thread_job_cancelled),
						      job);
	}

	pps_scheduler_job_list_remove (job);
	pps_scheduler_job_free (job);
}

static void
pps_scheduler_thread_job_cancelled (PpsSchedulerJob *job,
				   GCancellable   *cancellable)
{
	GList   *list;

	pps_debug_message (DEBUG_JOBS, "%s", PPS_GET_TYPE_NAME (job->job));

	g_mutex_lock (&job_queue_mutex);

	/* If the job is not still running,
	 * remove it from the job queue and job list.
	 * If the job is currently running, it will be
	 * destroyed as soon as it finishes.
	 */
	list = g_queue_find (job_queue[job->priority], job);
	if (list) {
		g_queue_delete_link (job_queue[job->priority], list);
		g_mutex_unlock (&job_queue_mutex);
		pps_scheduler_job_destroy (job);
	} else {
		g_mutex_unlock (&job_queue_mutex);
	}
}

static void
pps_job_thread (PpsJob *job)
{
	gboolean result;

	pps_debug_message (DEBUG_JOBS, "%s", PPS_GET_TYPE_NAME (job));

	do {
		if (g_cancellable_is_cancelled (job->cancellable))
			result = FALSE;
		else {
                        g_atomic_pointer_set (&running_job, job);
			result = pps_job_run (job);
                }
	} while (result);

        g_atomic_pointer_set (&running_job, NULL);
}

static gboolean
pps_job_idle (PpsJob *job)
{
	pps_debug_message (DEBUG_JOBS, "%s", PPS_GET_TYPE_NAME (job));

	if (g_cancellable_is_cancelled (job->cancellable))
		return G_SOURCE_REMOVE;

	return pps_job_run (job);
}

static gpointer
pps_job_thread_proxy (gpointer data)
{
	while (TRUE) {
		PpsSchedulerJob *job;

		g_mutex_lock (&job_queue_mutex);
		job = pps_job_queue_get_next_unlocked ();
		if (!job) {
			g_cond_wait (&job_queue_cond, &job_queue_mutex);
			g_mutex_unlock (&job_queue_mutex);
			continue;
		}
		g_mutex_unlock (&job_queue_mutex);

		pps_job_thread (job->job);
		pps_scheduler_job_destroy (job);
	}

	return NULL;
}

void
pps_job_scheduler_push_job (PpsJob         *job,
			   PpsJobPriority  priority)
{
	static GOnce once_init = G_ONCE_INIT;
	PpsSchedulerJob *s_job;

	g_once (&once_init, pps_job_scheduler_init, NULL);

	pps_debug_message (DEBUG_JOBS, "%s priority %d", PPS_GET_TYPE_NAME (job), priority);

	s_job = g_new0 (PpsSchedulerJob, 1);
	s_job->job = g_object_ref (job);
	s_job->priority = priority;

	pps_scheduler_job_list_add (s_job);

	switch (pps_job_get_run_mode (job)) {
	case PPS_JOB_RUN_THREAD:
		g_signal_connect_swapped (job->cancellable, "cancelled",
					  G_CALLBACK (pps_scheduler_thread_job_cancelled),
					  s_job);
		pps_job_queue_push (s_job, priority);
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
	GSList         *l;
	PpsSchedulerJob *s_job = NULL;
	gboolean        need_resort = FALSE;

	/* Main loop jobs are scheduled immediately */
	if (pps_job_get_run_mode (job) == PPS_JOB_RUN_MAIN_LOOP)
		return;

	pps_debug_message (DEBUG_JOBS, "%s priority %d", PPS_GET_TYPE_NAME (job), priority);

	G_LOCK (job_list);

	for (l = job_list; l; l = l->next) {
		s_job = (PpsSchedulerJob *)l->data;

		if (s_job->job == job) {
			need_resort = (s_job->priority != priority);
			break;
		}
	}

	G_UNLOCK (job_list);

	if (need_resort) {
		GList *list;

		g_mutex_lock (&job_queue_mutex);

		list = g_queue_find (job_queue[s_job->priority], s_job);
		if (list) {
			pps_debug_message (DEBUG_JOBS, "Moving job %s from priority %d to %d",
					  PPS_GET_TYPE_NAME (job), s_job->priority, priority);
			g_queue_delete_link (job_queue[s_job->priority], list);
			g_queue_push_tail (job_queue[priority], s_job);
			g_cond_broadcast (&job_queue_cond);
		}

		g_mutex_unlock (&job_queue_mutex);
	}
}

/**
 * pps_job_scheduler_get_running_thread_job:
 *
 * Returns: (transfer none): an #PpsJob
 */
PpsJob *
pps_job_scheduler_get_running_thread_job (void)
{
        return g_atomic_pointer_get (&running_job);
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

	while (job_list != NULL)
		g_usleep (100);

	pps_debug_message (DEBUG_JOBS, "Job list is empty");
}
