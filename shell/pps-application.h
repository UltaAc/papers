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

#pragma once

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>
#include <gtk/gtk.h>

#include "pps-window.h"

G_BEGIN_DECLS

#define PPS_TYPE_APPLICATION			(pps_application_get_type ())
G_DECLARE_FINAL_TYPE (PpsApplication, pps_application, PPS, APPLICATION, AdwApplication)

#define PPS_APP					((PpsApplication *) g_application_get_default ())

PpsApplication    *pps_application_new                 (void);

void              pps_application_open_start_view     (PpsApplication   *application);
void              pps_application_open_uri_at_dest    (PpsApplication   *application,
						      const char      *uri,
						      PpsLinkDest      *dest,
						      PpsWindowRunMode  mode,
						      const gchar     *search_string);
void	          pps_application_open_uri_list       (PpsApplication   *application,
						      GListModel      *files);
gboolean	  pps_application_has_window	     (PpsApplication   *application);
guint             pps_application_get_n_windows       (PpsApplication   *application);

const gchar      *pps_application_get_dot_dir         (PpsApplication   *application,
                                                      gboolean         create);

G_END_DECLS
