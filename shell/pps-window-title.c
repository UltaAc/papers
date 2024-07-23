/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2005 Red Hat, Inc
 *  Copyright (C) 2018 Germán Poo-Caamaño <gpoo@gnome.org>
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
#include "pps-window-title.h"
#include "pps-utils.h"
#include "pps-document-view.h"

#include <string.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>

/* Known backends (for bad extensions fix) */
#define PPS_BACKEND_PS  "PSDocument"
#define PPS_BACKEND_PDF "PdfDocument"

typedef struct
{
	const gchar *backend;
	const gchar *text;
} BadTitleEntry;

struct _PpsDocumentViewTitle
{
	PpsDocumentView *window;
	PpsDocument *document;
	char *filename;
	char *doc_title;
};

static const BadTitleEntry bad_extensions[] = {
	{ PPS_BACKEND_PS, ".dvi" },
	{ PPS_BACKEND_PDF, ".doc" },
	{ PPS_BACKEND_PDF, ".dvi" },
	{ PPS_BACKEND_PDF, ".indd" },
	{ PPS_BACKEND_PDF, ".rtf" }
};

static const BadTitleEntry bad_prefixes[] = {
	{ PPS_BACKEND_PDF, "Microsoft Word - " },
	{ PPS_BACKEND_PDF, "Microsoft PowerPoint - " }
};

/* Some docs report titles with confusing extensions (ex. .doc for pdf).
	   Erase the confusing extension of the title */
static void
pps_document_view_title_sanitize_title (PpsDocumentViewTitle *window_title, char **title) {
	const gchar *backend;
	int i;

	backend = G_OBJECT_TYPE_NAME (window_title->document);

	for (i = 0; i < G_N_ELEMENTS (bad_extensions); i++) {
		if (g_ascii_strcasecmp (bad_extensions[i].backend, backend) == 0 &&
		    g_str_has_suffix (*title, bad_extensions[i].text)) {
			char *new_title;

			new_title = g_strndup (*title, strlen(*title) - strlen(bad_extensions[i].text));
			g_free (*title);
			*title = new_title;
		}
	}
	for (i = 0; i < G_N_ELEMENTS (bad_prefixes); i++) {
		if (g_ascii_strcasecmp (bad_prefixes[i].backend, backend) == 0 &&
		    g_str_has_prefix (*title, bad_prefixes[i].text)) {
			char *new_title;
			int len = strlen(bad_prefixes[i].text);

			new_title = g_strdup_printf ("%s", (*title) + len);
			g_free (*title);
			*title = new_title;
		}
	}
}

static void
pps_document_view_title_update (PpsDocumentViewTitle *window_title)
{
	GtkWidget *window = GTK_WIDGET (window_title->window);
	AdwHeaderBar *header_bar = pps_document_view_get_header_bar (PPS_DOCUMENT_VIEW (window));
	AdwWindowTitle *title_widget = ADW_WINDOW_TITLE (adw_header_bar_get_title_widget (header_bar));
	GtkNative *native;

	char *title = NULL, *p;
	char *subtitle = NULL, *title_header = NULL;
	gboolean ltr;

	ltr = gtk_widget_get_direction (GTK_WIDGET (window)) == GTK_TEXT_DIR_LTR;

	if (window_title->doc_title && window_title->filename) {
		title_header = window_title->doc_title;
		subtitle = window_title->filename;

		if (ltr)
			title = g_strdup_printf ("%s — %s", subtitle, title_header);
		else
			title = g_strdup_printf ("%s — %s", title_header, subtitle);

                for (p = title; *p; ++p) {
                        /* an '\n' byte is always ASCII, no need for UTF-8 special casing */
                        if (*p == '\n')
                                *p = ' ';
                }
	} else if (window_title->filename) {
		title = g_strdup (window_title->filename);
	} else {
		title = g_strdup (_("Papers"));
	}

	native = gtk_widget_get_native (window);

	if (native)
		gtk_window_set_title (GTK_WINDOW (native), title);

	if (title_header && subtitle) {
		adw_window_title_set_title (title_widget, title_header);
		adw_window_title_set_subtitle (title_widget, subtitle);
	} else if (title) {
		adw_window_title_set_title (title_widget, title);
	}

	g_free (title);
}

PpsDocumentViewTitle *
pps_document_view_title_new (PpsDocumentView *window)
{
	PpsDocumentViewTitle *window_title;

	window_title = g_new0 (PpsDocumentViewTitle, 1);
	window_title->window = window;

        pps_document_view_title_update (window_title);

	return window_title;
}

static void
document_destroyed_cb (PpsDocumentViewTitle *window_title,
                       GObject       *document)
{
        window_title->document = NULL;
        g_clear_pointer (&window_title->doc_title, g_free);
}

void
pps_document_view_title_set_document (PpsDocumentViewTitle *window_title,
			      PpsDocument    *document)
{
        if (window_title->document == document)
                return;

        if (window_title->document)
                g_object_weak_unref (G_OBJECT (window_title->document), (GWeakNotify)document_destroyed_cb, window_title);
	window_title->document = document;
        g_object_weak_ref (G_OBJECT (window_title->document), (GWeakNotify)document_destroyed_cb, window_title);
        g_clear_pointer (&window_title->doc_title, g_free);

	if (window_title->document != NULL) {
		gchar *doc_title;

		doc_title = g_strdup (pps_document_get_title (window_title->document));

		/* Make sure we get a valid title back */
		if (doc_title != NULL) {
			doc_title = g_strstrip (doc_title);

			if (doc_title[0] != '\0' &&
			    g_utf8_validate (doc_title, -1, NULL)) {
				pps_document_view_title_sanitize_title (window_title,
								&doc_title);
				window_title->doc_title = doc_title;
			} else {
                                g_free (doc_title);
                        }
		}
	}

	pps_document_view_title_update (window_title);
}

void
pps_document_view_title_set_filename (PpsDocumentViewTitle *window_title,
			      const char    *filename)
{
        if (g_strcmp0 (filename, window_title->filename) == 0)
                return;

	g_free (window_title->filename);
	window_title->filename = g_strdup (filename);

	pps_document_view_title_update (window_title);
}

void
pps_document_view_title_free (PpsDocumentViewTitle *window_title)
{
        if (window_title->document)
                g_object_weak_unref (G_OBJECT (window_title->document), (GWeakNotify)document_destroyed_cb, window_title);
        g_free (window_title->doc_title);
	g_free (window_title->filename);
	g_free (window_title);
}
