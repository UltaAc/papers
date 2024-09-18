/* pps-previewer-window.h:
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2009 Carlos Garcia Campos <carlosgc@gnome.org>
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

#include <adwaita.h>
#include <papers-document.h>
#include <papers-view.h>

G_BEGIN_DECLS

#define PPS_TYPE_PREVIEWER_WINDOW                  (pps_previewer_window_get_type())
G_DECLARE_FINAL_TYPE(PpsPreviewerWindow, pps_previewer_window, PPS, PREVIEWER_WINDOW, AdwApplicationWindow)

struct _PpsPreviewerWindow {
	AdwApplicationWindow base_instance;
};

PpsPreviewerWindow *pps_previewer_window_new            (void);

PpsDocumentModel   *pps_previewer_window_get_document_model (PpsPreviewerWindow *window);

void       pps_previewer_window_set_job                (PpsPreviewerWindow *window,
                                                       PpsJob             *job);
gboolean   pps_previewer_window_set_print_settings     (PpsPreviewerWindow *window,
                                                       const gchar       *print_settings,
                                                       GError           **error);
gboolean   pps_previewer_window_set_print_settings_fd  (PpsPreviewerWindow *window,
                                                       int                fd,
                                                       GError           **error);
void       pps_previewer_window_set_source_file        (PpsPreviewerWindow *window,
                                                       const gchar       *source_file);
gboolean   pps_previewer_window_set_source_fd          (PpsPreviewerWindow *window,
                                                       int                fd,
						       GError           **error);

G_END_DECLS
