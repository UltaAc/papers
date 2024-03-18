/*
 * pps-debug.c
 * This file is part of Papers
 *
 * Copyright (C) 1998, 1999 Alex Roberts, Ppsan Lawrence
 * Copyright (C) 2000, 2001 Chema Celorio, Paolo Maggi
 * Copyright (C) 2002 - 2005 Paolo Maggi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*
 * Modified by the gedit Team, 1998-2005. See the AUTHORS file for a
 * list of people on the gedit Team.
 * See the ChangeLog files for a list of changes.
 *
 * $Id: gedit-debug.c 4809 2006-04-08 14:46:31Z pborelli $
 */

/* Modified by Papers Team */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include "pps-debug.h"

#ifdef PPS_ENABLE_DEBUG
static PpsDebugSection pps_debug = PPS_NO_DEBUG;
static PpsDebugBorders pps_debug_borders = PPS_DEBUG_BORDER_NONE;

void
_pps_debug_init (void)
{
        const GDebugKey keys[] = {
                { "jobs",    PPS_DEBUG_JOBS         },
                { "borders", PPS_DEBUG_SHOW_BORDERS }
        };
        const GDebugKey border_keys[] = {
                { "chars",      PPS_DEBUG_BORDER_CHARS      },
                { "links",      PPS_DEBUG_BORDER_LINKS      },
                { "forms",      PPS_DEBUG_BORDER_FORMS      },
                { "annots",     PPS_DEBUG_BORDER_ANNOTS     },
                { "images",     PPS_DEBUG_BORDER_IMAGES     },
                { "media",      PPS_DEBUG_BORDER_MEDIA      },
                { "selections", PPS_DEBUG_BORDER_SELECTIONS }
        };

        pps_debug = g_parse_debug_string (g_getenv ("PPS_DEBUG"), keys, G_N_ELEMENTS (keys));
        if (pps_debug & PPS_DEBUG_SHOW_BORDERS)
                pps_debug_borders = g_parse_debug_string (g_getenv ("PPS_DEBUG_SHOW_BORDERS"),
                                                         border_keys, G_N_ELEMENTS (border_keys));
}

void
pps_debug_message (PpsDebugSection  section,
		  const gchar    *file,
		  gint            line,
		  const gchar    *function,
		  const gchar    *format, ...)
{
	if (G_UNLIKELY (pps_debug & section)) {
		gchar *msg = NULL;

		if (format) {
			va_list args;

			va_start (args, format);
			msg = g_strdup_vprintf (format, args);
			va_end (args);
		}

		g_print ("%s:%d (%s) %s\n", file, line, function, msg ? msg : "");

		fflush (stdout);

		g_free (msg);
	}
}

PpsDebugBorders
pps_debug_get_debug_borders (void)
{
        return pps_debug_borders;
}

#endif /* PPS_ENABLE_DEBUG */
