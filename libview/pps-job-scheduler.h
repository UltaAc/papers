/* pps-job-scheduler.h
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

#if !defined(__PPS_PAPERS_VIEW_H_INSIDE__) && !defined(PAPERS_COMPILATION)
#error "Only <papers-view.h> can be included directly."
#endif

#include <glib.h>

#include "pps-jobs.h"
#include <papers-document.h>

G_BEGIN_DECLS

typedef enum {
	PPS_JOB_PRIORITY_URGENT, /* Rendering current page range */
	PPS_JOB_PRIORITY_HIGH,   /* Rendering current thumbnail range */
	PPS_JOB_PRIORITY_LOW,    /* Rendering pages not in current range */
	PPS_JOB_PRIORITY_NONE,   /* Any other job: load, save, print, ... */
	PPS_JOB_N_PRIORITIES
} PpsJobPriority;

PPS_PUBLIC
void pps_job_scheduler_push_job (PpsJob *job,
                                 PpsJobPriority priority);
PPS_PUBLIC
void pps_job_scheduler_update_job (PpsJob *job,
                                   PpsJobPriority priority);
PPS_PUBLIC
void pps_job_scheduler_wait (void);

G_END_DECLS
