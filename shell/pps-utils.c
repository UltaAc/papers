/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/*
 *  Copyright (C) 2004 Anders Carlsson <andersca@gnome.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <config.h>

#include "pps-utils.h"

#include <string.h>
#include <glib/gi18n.h>

/* Simple function to output the contents of a region.  Used solely for testing
 * the region code.
 */
void
pps_print_region_contents (cairo_region_t *region)
{
	gint n_rectangles, i;

	if (region == NULL) {
		g_print ("<empty region>\n");
		return;
	}

	g_print ("<region %p>\n", region);
	n_rectangles = cairo_region_num_rectangles (region);
	for (i = 0; i < n_rectangles; i++) {
		GdkRectangle rect;

		cairo_region_get_rectangle (region, i, &rect);
		g_print ("\t(%d %d, %d %d) [%dx%d]\n",
			 rect.x,
			 rect.y,
			 rect.x + rect.width,
			 rect.y + rect.height,
			 rect.width,
			 rect.height);
	}
}

GdkPixbufFormat*
get_gdk_pixbuf_format_by_extension (const gchar *uri)
{
	GSList *pixbuf_formats = NULL;
	GSList *iter;
	int i;

	pixbuf_formats = gdk_pixbuf_get_formats ();

	for (iter = pixbuf_formats; iter; iter = iter->next) {
		gchar **extension_list;
		GdkPixbufFormat *format = iter->data;

		if (gdk_pixbuf_format_is_disabled (format) ||
	    	    !gdk_pixbuf_format_is_writable (format))
		            continue;

	        extension_list = gdk_pixbuf_format_get_extensions (format);

		for (i = 0; extension_list[i] != 0; i++) {
			if (g_str_has_suffix (uri, extension_list[i])) {
			    	g_slist_free (pixbuf_formats);
				g_strfreev (extension_list);
				return format;
			}
		}
		g_strfreev (extension_list);
	}

	g_slist_free (pixbuf_formats);
	return NULL;
}

/*
 * Replace all occurrences of substr in str with repl
 *
 * @param str a string
 * @param substr some string to replace
 * @param repl a replacement string
 *
 * @return a newly allocated string with the substr replaced by repl; free with g_free
 */
gchar*
pps_str_replace (const char *str, const char *substr, const char *repl)
{
	GString		*gstr;
	const char	*cur;

	if (str == NULL || substr == NULL || repl == NULL)
		return NULL;

	gstr = g_string_sized_new (2 * strlen (str));

	for (cur = str; *cur; ++cur) {
		if (g_str_has_prefix (cur, substr)) {
			g_string_append (gstr, repl);
			cur += strlen (substr) - 1;
		} else
			g_string_append_c (gstr, *cur);
	}

	return g_string_free (gstr, FALSE);
}

void
pps_spinner_map_cb (GtkSpinner *spinner) {
	gtk_spinner_set_spinning(spinner, TRUE);
}

void
pps_spinner_unmap_cb (GtkSpinner *spinner) {
	gtk_spinner_set_spinning(spinner, FALSE);
}
