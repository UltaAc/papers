/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2009 Juanjo Marín <juanj.marin@juntadeandalucia.es>
 *  Copyright (C) 2008 Carlos Garcia Campos
 *  Copyright (C) 2004 Martin Kretzschmar
 *  Copyright (C) 2004 Red Hat, Inc.
 *  Copyright (C) 2000, 2001, 2002, 2003, 2004 Marco Pesenti Gritti
 *  Copyright © 2003, 2004, 2005, 2009, 2012 Christian Persch
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <adwaita.h>

#include <papers-view.h>
#include "pps-find-sidebar.h"
#include "pps-annotations-toolbar.h"
#include "pps-application.h"
#include "pps-file-monitor.h"
#include "pps-history.h"
#include "pps-message-area.h"
#include "pps-metadata.h"
#include "pps-page-selector.h"
#include "pps-sidebar-annotations.h"
#include "pps-sidebar-bookmarks.h"
#include "pps-sidebar.h"
#include "pps-utils.h"
#include "pps-keyring.h"
#include "pps-view-presentation.h"
#include "pps-window.h"
#include "pps-window-title.h"
#include "pps-progress-message-area.h"
#include "pps-annotation-properties-dialog.h"
#include "pps-bookmarks.h"
#include "pps-search-box.h"

#ifdef ENABLE_DBUS
#include "pps-gdbus-generated.h"
#endif /* ENABLE_DBUS */

#define MOUSE_BACK_BUTTON 8
#define MOUSE_FORWARD_BUTTON 9

typedef enum {
	PPS_SAVE_DOCUMENT,
	PPS_SAVE_ATTACHMENT,
	PPS_SAVE_IMAGE
} PpsSaveType;

typedef enum {
	PPS_WINDOW_ACTION_RELOAD,
	PPS_WINDOW_ACTION_CLOSE
} PpsWindowAction;

typedef struct {
	/* UI */
	GtkWidget *stack;
	GtkWidget *toolbar_view;
	GtkWidget *sidebar;
	GtkWidget *search_box;
	GtkWidget *search_bar;
	GtkWidget *scrolled_window;
	GtkWidget *view;
	GtkWidget *loading_message;
	GtkWidget *presentation_view;
	GtkWidget *message_area;
	GtkWidget *password_view;
	GtkWidget *sidebar_links;
	GtkWidget *find_sidebar;
	GtkWidget *sidebar_layers;
	GtkWidget *sidebar_annots;
	GtkWidget *sidebar_bookmarks;
	GtkWidget *annots_toolbar_revealer;
	GtkWidget *annots_toolbar;
	GtkWidget *error_page;
	GtkWidget *page_selector;
	GtkWidget *header_bar;

	AdwOverlaySplitView *split_view;

	/* Settings */
	GSettings *settings;
	GSettings *default_settings;
	GSettings *lockdown_settings;

	/* Progress Messages */
	guint progress_idle;
	GCancellable *progress_cancellable;

	/* Loading message */
	guint loading_message_timeout;

	/* Dialogs */
	GtkWindow *properties;
	GtkWindow *print_dialog;

	/* Menu button */
	GtkWidget    *action_menu_button;

	/* Popup view */
	GtkWidget    *view_popup;
	PpsLink       *link;
	PpsImage      *image;
	PpsAnnotation *annot;

	/* Popup attachment */
	GMenuModel   *attachment_popup_menu;
	GtkWidget    *attachment_popup;
	GList        *attach_list;

	/* Document */
	PpsDocumentModel *model;
	char *uri;
	gint64 uri_mtime;
	char *local_uri;
	char *display_name;
	char *edit_name;
	gboolean in_reload;
	PpsFileMonitor *monitor;
	guint setup_document_idle;

	PpsDocument *document;
	PpsHistory *history;
	PpsWindowTitle *title;
	PpsMetadata *metadata;
	PpsBookmarks *bookmarks;

	/* Has the document been modified? */
	gboolean is_modified;
	gulong   modified_handler_id;

	/* Load params */
	PpsLinkDest       *dest;
	PpsWindowRunMode   window_mode;

	PpsJob            *load_job;
	PpsJob            *reload_job;
	PpsJob            *save_job;
	gboolean          close_after_save;

	/* Printing */
	GQueue           *print_queue;
	GtkPrintSettings *print_settings;
	GtkPageSetup     *print_page_setup;
	gboolean          close_after_print;

#ifdef ENABLE_DBUS
	/* DBus */
	PpsPapersWindow *skeleton;
	gchar          *dbus_object_path;
#endif

        guint presentation_mode_inhibit_id;

	/* Caret navigation */
	GtkWidget *ask_caret_navigation_check;

	gboolean password_view_cancelled;
} PpsWindowPrivate;

#define GET_PRIVATE(o) pps_window_get_instance_private (o)

#define PPS_WINDOW_IS_PRESENTATION(priv) (priv->presentation_view != NULL)

#define GS_LOCKDOWN_SCHEMA_NAME  "org.gnome.desktop.lockdown"
#define GS_LOCKDOWN_SAVE         "disable-save-to-disk"
#define GS_LOCKDOWN_PRINT        "disable-printing"
#define GS_LOCKDOWN_PRINT_SETUP  "disable-print-setup"

#ifdef ENABLE_DBUS
#define PPS_WINDOW_DBUS_OBJECT_PATH "/org/gnome/Papers" OBJECT_PROFILE "/Window/%d"
#endif

#define GS_SCHEMA_NAME           "org.gnome.Papers"
#define GS_OVERRIDE_RESTRICTIONS "override-restrictions"
#define GS_PAGE_CACHE_SIZE       "page-cache-size"
#define GS_AUTO_RELOAD           "auto-reload"
#define GS_LAST_DOCUMENT_DIRECTORY "document-directory"
#define GS_LAST_PICTURES_DIRECTORY "pictures-directory"
#define GS_ALLOW_LINKS_CHANGE_ZOOM "allow-links-change-zoom"

#define PPS_PRINT_SETTINGS_FILE  "print-settings"
#define PPS_PRINT_SETTINGS_GROUP "Print Settings"
#define PPS_PAGE_SETUP_GROUP     "Page Setup"

static const gchar *document_print_settings[] = {
	GTK_PRINT_SETTINGS_COLLATE,
	GTK_PRINT_SETTINGS_REVERSE,
	GTK_PRINT_SETTINGS_NUMBER_UP,
	GTK_PRINT_SETTINGS_SCALE,
	GTK_PRINT_SETTINGS_PRINT_PAGES,
	GTK_PRINT_SETTINGS_PAGE_RANGES,
	GTK_PRINT_SETTINGS_PAGE_SET,
	GTK_PRINT_SETTINGS_OUTPUT_URI
};

static void	pps_window_update_actions_sensitivity    (PpsWindow         *pps_window);
static void	pps_window_set_mode			(PpsWindow         *window,
							 PpsWindowRunMode   mode);
static void	pps_window_load_job_cb  			(PpsJob            *job,
							 gpointer          data);
static gboolean pps_window_check_document_modified 	(PpsWindow         *pps_window,
							 PpsWindowAction    command);
static void     pps_window_reload_document               (PpsWindow         *window,
							 PpsLinkDest *dest);
static void	pps_window_document_modified_cb		(PpsDocument *document,
                                                         GParamSpec *pspec,
                                                         PpsWindow   *pps_window);
static void     pps_window_reload_job_cb                 (PpsJob            *job,
							 PpsWindow         *window);
static void     pps_window_save_job_cb                   (PpsJob            *save,
							 PpsWindow         *window);
static void     pps_window_run_fullscreen                (PpsWindow         *window);
static void     pps_window_stop_fullscreen               (PpsWindow         *window,
							 gboolean          unfullscreen_window);
static void     pps_window_run_presentation              (PpsWindow         *window);
static void     pps_window_stop_presentation             (PpsWindow         *window,
							 gboolean          unfullscreen_window);
static void     pps_window_popup_cmd_open_link           (GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void     pps_window_popup_cmd_open_link_new_window(GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void     pps_window_popup_cmd_copy_link_address   (GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void     pps_window_popup_cmd_save_image_as       (GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void     pps_window_popup_cmd_copy_image          (GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void     pps_window_popup_cmd_annot_properties    (GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void     pps_window_popup_cmd_remove_annotation   (GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void	pps_window_popup_cmd_open_attachment     (GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void	pps_window_popup_cmd_save_attachment_as  (GSimpleAction    *action,
							 GVariant         *parameter,
							 gpointer          user_data);
static void	view_handle_link_cb 			(PpsView           *view,
							 PpsLink           *link,
							 PpsLink           *backlink,
							 PpsWindow         *window);
static void	bookmark_activated_cb 		        (PpsSidebarBookmarks *sidebar_bookmarks,
							 gint              old_page,
							 gint              page,
							 PpsWindow         *window);
static void	scroll_history_cb                       (PpsView           *view,
							 GtkScrollType     scroll,
							 gboolean          horizontal,
							 PpsWindow         *window);
static void	scroll_child_history_cb                 (GtkScrolledWindow *scrolled_window,
							 GtkScrollType      scroll,
							 gboolean           horizontal,
							 PpsWindow          *window);
static void     activate_link_cb                        (GObject          *object,
							 PpsLink           *link,
							 PpsWindow         *window);
static void     view_external_link_cb                   (PpsWindow         *window,
							 PpsLinkAction     *action);
static void     pps_window_load_file_remote              (PpsWindow         *pps_window,
							 GFile            *source_file);
#ifdef ENABLE_DBUS
static void	pps_window_emit_closed			(PpsWindow         *window);
static void 	pps_window_emit_doc_loaded		(PpsWindow	  *window);
#endif

static void     pps_window_show_find_bar                 (PpsWindow         *pps_window);
static void     pps_window_close_find_bar                (PpsWindow         *pps_window);
static void     pps_window_begin_add_annot               (PpsWindow         *pps_window,
							 PpsAnnotationType  annot_type);
static void	pps_window_cancel_add_annot		(PpsWindow *window);
static void     pps_window_cmd_toggle_edit_annots 	(GSimpleAction *action,
							 GVariant      *state,
							 gpointer       user_data);

G_DEFINE_TYPE_WITH_PRIVATE (PpsWindow, pps_window, ADW_TYPE_APPLICATION_WINDOW)

static gboolean
pps_window_is_start_view (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	return priv->window_mode == PPS_WINDOW_MODE_START_VIEW;
}

static void
pps_window_set_action_enabled (PpsWindow   *pps_window,
			      const char *name,
			      gboolean    enabled)
{
	GAction *action;

	action = g_action_map_lookup_action (G_ACTION_MAP (pps_window), name);
	g_simple_action_set_enabled (G_SIMPLE_ACTION (action), enabled);
}

static void
pps_window_update_actions_sensitivity (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsDocument *document = priv->document;
	PpsView     *view = PPS_VIEW (priv->view);
	const PpsDocumentInfo *info = NULL;
	gboolean has_document = FALSE;
	gboolean ok_to_print = TRUE;
	gboolean ok_to_copy = TRUE;
	gboolean has_properties = TRUE;
	gboolean override_restrictions = TRUE;
	gboolean can_get_text = FALSE;
	gboolean can_find = FALSE;
	gboolean can_annotate = FALSE;
	gboolean presentation_mode;
	gboolean start_view_mode;
	gboolean dual_mode = FALSE;
	gboolean has_pages = FALSE;
	int      n_pages = 0, page = -1;

	if (document) {
		has_document = TRUE;
		info = pps_document_get_info (document);
		page = pps_document_model_get_page (priv->model);
		n_pages = pps_document_get_n_pages (priv->document);
		has_pages = n_pages > 0;
		dual_mode = pps_document_model_get_page_layout (priv->model) == PPS_PAGE_LAYOUT_DUAL;
	}

	if (!info || info->fields_mask == 0) {
		has_properties = FALSE;
	}

	if (has_document && PPS_IS_SELECTION (document)) {
		can_get_text = TRUE;
	}

	if (has_pages && PPS_IS_DOCUMENT_FIND (document)) {
		can_find = TRUE;
	}

	if (has_document && PPS_IS_DOCUMENT_ANNOTATIONS (document)) {
		can_annotate = pps_document_annotations_can_add_annotation (PPS_DOCUMENT_ANNOTATIONS (document));
	}

	if (has_document && priv->settings) {
		override_restrictions =
			g_settings_get_boolean (priv->settings,
						GS_OVERRIDE_RESTRICTIONS);
	}

	if (!override_restrictions && info && info->fields_mask & PPS_DOCUMENT_INFO_PERMISSIONS) {
		ok_to_print = (info->permissions & PPS_DOCUMENT_PERMISSIONS_OK_TO_PRINT);
		ok_to_copy = (info->permissions & PPS_DOCUMENT_PERMISSIONS_OK_TO_COPY);
	}

	if (has_document && !pps_print_operation_exists_for_document(document))
		ok_to_print = FALSE;

	if (has_document && priv->lockdown_settings &&
	    g_settings_get_boolean (priv->lockdown_settings, GS_LOCKDOWN_SAVE)) {
		ok_to_copy = FALSE;
	}

	if (has_document && priv->lockdown_settings &&
	    g_settings_get_boolean (priv->lockdown_settings, GS_LOCKDOWN_PRINT)) {
		ok_to_print = FALSE;
	}

	/* Get modes */
	presentation_mode = PPS_WINDOW_IS_PRESENTATION (priv);
	start_view_mode = pps_window_is_start_view (pps_window);

	/* File menu */
	pps_window_set_action_enabled (pps_window, "open-copy", has_document);
	pps_window_set_action_enabled (pps_window, "save-as", has_document &&
				      ok_to_copy && !start_view_mode);
	pps_window_set_action_enabled (pps_window, "print", has_pages &&
				      ok_to_print && !start_view_mode);
	pps_window_set_action_enabled (pps_window, "show-properties",
				      has_document && has_properties &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "fullscreen",
				      has_document && !start_view_mode);
	pps_window_set_action_enabled (pps_window, "presentation",
				      has_document && !start_view_mode);

        /* Edit menu */
	pps_window_set_action_enabled (pps_window, "select-all", has_pages &&
				      can_get_text && !start_view_mode);
	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.find",
				       can_find && !start_view_mode);
	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.toggle-find",
				       can_find && !start_view_mode);
	pps_window_set_action_enabled (pps_window, "add-annotation", can_annotate &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "highlight-annotation", can_annotate &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "toggle-edit-annots", can_annotate &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "rotate-left", has_pages &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "rotate-right", has_pages &&
				      !start_view_mode);

        /* View menu */
	pps_window_set_action_enabled (pps_window, "continuous", has_pages &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "dual-page", has_pages &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "rtl", has_pages &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "reload", has_pages &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "inverted-colors",
				      has_pages && !start_view_mode);
	pps_window_set_action_enabled (pps_window, "enable-spellchecking", FALSE);

	/* Bookmarks menu */
	pps_window_set_action_enabled (pps_window, "add-bookmark",
				      has_pages && priv->bookmarks &&
				      !start_view_mode);

	/* Other actions that must be disabled in start view, in
	 * case they have a shortcut or gesture associated
	 */
	pps_window_set_action_enabled (pps_window, "show-side-pane", !start_view_mode);
	pps_window_set_action_enabled (pps_window, "sizing-mode", !start_view_mode);
	pps_window_set_action_enabled (pps_window, "zoom", !start_view_mode);
	pps_window_set_action_enabled (pps_window, "escape", !start_view_mode);

	/* Don't enable popup actions here because the page can change while a
	 * popup is visible due to kinetic scrolling. The 'popup' functions
	 * will enable appropriate actions when the popup is shown. */
	if (start_view_mode) {
		pps_window_set_action_enabled (pps_window, "open-link", FALSE);
		pps_window_set_action_enabled (pps_window, "open-link-new-window", FALSE);
		pps_window_set_action_enabled (pps_window, "go-to-link", FALSE);
		pps_window_set_action_enabled (pps_window, "copy-link-address", FALSE);
		pps_window_set_action_enabled (pps_window, "save-image", FALSE);
		pps_window_set_action_enabled (pps_window, "copy-image", FALSE);
		pps_window_set_action_enabled (pps_window, "open-attachment", FALSE);
		pps_window_set_action_enabled (pps_window, "save-attachment", FALSE);
		pps_window_set_action_enabled (pps_window, "annot-properties", FALSE);
		pps_window_set_action_enabled (pps_window, "remove-annot", FALSE);
	}

	pps_window_set_action_enabled (pps_window, "copy",
					has_pages &&
					pps_view_has_selection (view) &&
					!start_view_mode);
	pps_window_set_action_enabled (pps_window, "dual-odd-left", dual_mode &&
				      has_pages && !start_view_mode);

	pps_window_set_action_enabled (pps_window, "zoom-in",
				      has_pages &&
				      pps_view_can_zoom_in (view) &&
				      !presentation_mode &&
				      !start_view_mode);
	pps_window_set_action_enabled (pps_window, "zoom-out",
				      has_pages &&
				      pps_view_can_zoom_out (view) &&
				      !presentation_mode &&
				      !start_view_mode);

        /* Go menu */
	if (has_pages) {
		pps_window_set_action_enabled (pps_window, "go-previous-page", page > 0);
		pps_window_set_action_enabled (pps_window, "go-next-page", page < n_pages - 1);
		pps_window_set_action_enabled (pps_window, "go-first-page", page > 0);
		pps_window_set_action_enabled (pps_window, "go-last-page", page < n_pages - 1);
		pps_window_set_action_enabled (pps_window, "select-page", TRUE);
	} else {
  		pps_window_set_action_enabled (pps_window, "go-first-page", FALSE);
		pps_window_set_action_enabled (pps_window, "go-previous-page", FALSE);
		pps_window_set_action_enabled (pps_window, "go-next-page", FALSE);
		pps_window_set_action_enabled (pps_window, "go-last-page", FALSE);
		pps_window_set_action_enabled (pps_window, "select-page", FALSE);
	}

	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.go-back-history",
				      !pps_history_is_frozen (priv->history) &&
				      pps_history_can_go_back (priv->history) &&
				      !start_view_mode);
	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.go-forward-history",
				      !pps_history_is_frozen (priv->history) &&
				      pps_history_can_go_forward (priv->history) &&
				      !start_view_mode);

	pps_window_set_action_enabled (pps_window, "caret-navigation",
				      has_pages &&
				      pps_view_supports_caret_navigation (view) &&
				      !presentation_mode &&
				      !start_view_mode);
}


static void
update_sizing_buttons (PpsWindow *window)
{
	GAction     *action;
	const gchar *mode = NULL;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	action = g_action_map_lookup_action (G_ACTION_MAP (window), "sizing-mode");

	switch (pps_document_model_get_sizing_mode (priv->model)) {
	case PPS_SIZING_FIT_PAGE:
		mode = "fit-page";
		break;
	case PPS_SIZING_FIT_WIDTH:
		mode = "fit-width";
		break;
	case PPS_SIZING_AUTOMATIC:
		mode = "automatic";
		break;
	case PPS_SIZING_FREE:
		mode = "free";
		break;
	}

	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_string (mode));
}

/**
 * pps_window_is_empty:
 * @pps_window: The instance of the #PpsWindow.
 *
 * It does look if there is any document loaded or if there is any job to load
 * a document.
 *
 * Returns: %TRUE if there isn't any document loaded or any any documente to be
 *          loaded, %FALSE in other case.
 */
gboolean
pps_window_is_empty (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	g_return_val_if_fail (PPS_IS_WINDOW (pps_window), FALSE);

	return (priv->document == NULL) &&
		(priv->load_job == NULL);
}


static void
pps_window_set_message_area (PpsWindow  *window,
			    GtkWidget *area)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (priv->message_area == area)
		return;

	if (priv->message_area)
		adw_toolbar_view_remove (ADW_TOOLBAR_VIEW (priv->toolbar_view), priv->message_area);
	priv->message_area = area;

	if (!area)
		return;

	adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW (priv->toolbar_view), priv->message_area);
}

static void
pps_window_message_area_response_cb (PpsMessageArea *area,
				    gint           response_id,
				    PpsWindow      *window)
{
	pps_window_set_message_area (window, NULL);
}

G_GNUC_PRINTF (3, 4) static void
pps_window_error_message (PpsWindow    *window,
			 GError      *error,
			 const gchar *format,
			 ...)
{
	GtkWidget *area;
	va_list    args;
	gchar     *msg = NULL;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (priv->message_area)
		return;

	va_start (args, format);
	msg = g_strdup_vprintf (format, args);
	va_end (args);

	area = pps_message_area_new (GTK_MESSAGE_ERROR,
				    msg,
				    NULL);
	g_free (msg);

	if (error)
		pps_message_area_set_secondary_text (PPS_MESSAGE_AREA (area), error->message);
	g_signal_connect (pps_message_area_get_info_bar (PPS_MESSAGE_AREA (area)), "response",
			  G_CALLBACK (pps_window_message_area_response_cb),
			  window);
	gtk_widget_set_visible (area, TRUE);
	pps_window_set_message_area (window, area);
}

G_GNUC_PRINTF (2, 3) static void
pps_window_warning_message (PpsWindow    *window,
			   const gchar *format,
			   ...)
{
	GtkWidget *area;
	va_list    args;
	gchar     *msg = NULL;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (priv->message_area)
		return;

	va_start (args, format);
	msg = g_strdup_vprintf (format, args);
	va_end (args);

	area = pps_message_area_new (GTK_MESSAGE_WARNING,
				    msg,
				    NULL);
	g_free (msg);

	g_signal_connect (pps_message_area_get_info_bar (PPS_MESSAGE_AREA (area)), "response",
			  G_CALLBACK (pps_window_message_area_response_cb),
			  window);
	gtk_widget_set_visible (area, TRUE);
	pps_window_set_message_area (window, area);
}

static gboolean
show_loading_message_cb (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	priv->loading_message_timeout = 0;
	gtk_widget_set_visible (priv->loading_message, TRUE);

	return G_SOURCE_REMOVE;
}

static void
pps_window_show_loading_message (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (priv->loading_message_timeout)
		return;
	priv->loading_message_timeout =
		g_timeout_add_seconds_full (G_PRIORITY_LOW, 3, (GSourceFunc)show_loading_message_cb, window, NULL);
}

static void
pps_window_hide_loading_message (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	g_clear_handle_id (&priv->loading_message_timeout, g_source_remove);

	gtk_widget_set_visible (priv->loading_message, FALSE);
}

static const gchar *
find_link_in_outlines (PpsOutlines *outlines, PpsLink *link)
{
	PpsLink *outlines_link;
	PpsLinkAction *a, *b;
	const gchar *link_title = NULL;
	GListModel *children;

	outlines_link = pps_outlines_get_link (outlines);

	if (outlines_link) {
		a = pps_link_get_action (link);
		b = pps_link_get_action (outlines_link);

		if (a && b && pps_link_action_equal (a, b)) {
			link_title = pps_link_get_title (link);
		}
	}

	children = pps_outlines_get_children (outlines);

	if (children) {
		int n = 0;
		PpsOutlines *child;

		while ((child = g_list_model_get_item (children, n++)) != NULL) {
			link_title = find_link_in_outlines (child, link);

			if (link_title)
				break;
		}
	}

	return link_title;
}

static const gchar *
pps_window_find_title_for_link (PpsWindow *window,
			       PpsLink   *link)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (PPS_IS_DOCUMENT_LINKS (priv->document) &&
	    pps_document_links_has_document_links (PPS_DOCUMENT_LINKS (priv->document))) {
		GListModel *model;
		const gchar *link_title = NULL;

		g_object_get (G_OBJECT (priv->sidebar_links),
			      "model", &model,
			      NULL);
		if (model) {
			int n = g_list_model_get_n_items (model);

			for (int i = 0; i < n; i++) {
				link_title = find_link_in_outlines (g_list_model_get_item (model, i), link);

				if (link_title)
					break;
			}

			g_object_unref (model);
		}

		return link_title;
	}

	return NULL;
}

static void
view_handle_link_cb (PpsView *view, PpsLink *link, PpsLink *backlink,
		     PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	PpsLink *new_link = NULL;

	if (!pps_link_get_title (link)) {
		const gchar *link_title;

		link_title = pps_window_find_title_for_link (window, link);
		if (link_title) {
			new_link = pps_link_new (link_title, pps_link_get_action (link));
		} else {
			PpsLinkAction *action;
			PpsLinkDest   *dest;
			gchar        *page_label;
			gchar        *title;

			action = pps_link_get_action (link);
			dest = pps_link_action_get_dest (action);
			page_label = pps_document_links_get_dest_page_label (PPS_DOCUMENT_LINKS (priv->document), dest);
			if (!page_label)
				return;

			title = g_strdup_printf (_("Page %s"), page_label);
			g_free (page_label);

			new_link = pps_link_new (title, action);
			g_free (title);
		}
	}

	pps_history_add_link (priv->history, backlink);

	pps_history_add_link (priv->history, new_link ? new_link : link);
	if (new_link)
		g_object_unref (new_link);
}

static void
bookmark_activated_cb (PpsSidebarBookmarks *sidebar_bookmarks,
		       gint                old_page,
		       gint                page,
		       PpsWindow           *window)
{
	PpsWindowPrivate *priv = pps_window_get_instance_private (window);

	pps_history_add_page (priv->history, old_page);
	pps_history_add_page (priv->history, page);
}

static void
scroll_history_cb (PpsView        *view,
		   GtkScrollType  scroll,
		   gboolean       horizontal,
		   PpsWindow      *window)
{
	PpsWindowPrivate *priv = pps_window_get_instance_private (window);
	gint old_page = -1;
	gint new_page = -1;

	if (!priv->document)
		return;

	switch (scroll) {
		case GTK_SCROLL_START:
			old_page = pps_document_model_get_page (priv->model);
			new_page = 0;
			break;
		case GTK_SCROLL_END:
			old_page = pps_document_model_get_page (priv->model);
			new_page = pps_document_get_n_pages (priv->document) - 1;
			break;
		default:
			break;
	}

	if (old_page >= 0 && new_page >= 0) {
		pps_history_add_page (priv->history, old_page);
		pps_history_add_page (priv->history, new_page);
	}
}

static void
scroll_child_history_cb (GtkScrolledWindow *scrolled_window,
			 GtkScrollType      scroll,
			 gboolean           horizontal,
			 PpsWindow          *window)
{
	PpsWindowPrivate *priv = pps_window_get_instance_private (window);

	if (!priv->document)
		return;

	if (pps_document_model_get_continuous (priv->model) && !horizontal) {
		gint old_page = -1;
		gint new_page = -1;
		switch (scroll) {
		case GTK_SCROLL_START:
			old_page = pps_document_model_get_page (priv->model);
			new_page = 0;
			break;
		case GTK_SCROLL_END:
			old_page = pps_document_model_get_page (priv->model);
			new_page = pps_document_get_n_pages (priv->document) - 1;
			break;
		default:
			break;
		}
		if (old_page >= 0 && new_page >= 0) {
			pps_history_add_page (priv->history, old_page);
			pps_history_add_page (priv->history, new_page);

		}
	}
}

static gboolean
scrolled_window_focus_in_cb (GtkEventControllerFocus    *self,
			     PpsWindow                   *window)
{
	pps_window_focus_view (window);
	return GDK_EVENT_STOP;
}

static void
view_selection_changed_cb (PpsView   *view,
			   PpsWindow *window)
{
	pps_window_set_action_enabled (window, "copy",
					pps_view_has_selection (view));
}

static void
view_layers_changed_cb (PpsView   *view,
			PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	/* FIXME: We use a indirect way to call a method to rust object.
	 *        We should use a method after rust port of PpsWindow.
	 */
	g_signal_emit_by_name (priv->sidebar_layers, "update-visibility", NULL);
}

static void
view_is_loading_changed_cb (PpsView     *view,
			    GParamSpec *spec,
			    PpsWindow   *window)
{
	if (pps_view_is_loading (view))
		pps_window_show_loading_message (window);
	else
		pps_window_hide_loading_message (window);
}

static void
view_caret_cursor_moved_cb (PpsView   *view,
			    guint     page,
			    guint     offset,
			    PpsWindow *window)
{
	GVariant *position;
	gchar    *caret_position;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->metadata)
		return;

	position = g_variant_new ("(uu)", page, offset);
	caret_position = g_variant_print (position, FALSE);
	g_variant_unref (position);

	pps_metadata_set_string (priv->metadata, "caret-position", caret_position);
	g_free (caret_position);
}

static void
page_changed_cb (PpsWindow        *pps_window,
			   gint             old_page,
			   gint             new_page,
			   PpsDocumentModel *model)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_window_update_actions_sensitivity (pps_window);

	if (priv->metadata && !pps_window_is_empty (pps_window))
		pps_metadata_set_int (priv->metadata, "page", new_page);
}

static void
update_document_mode (PpsWindow *window, PpsDocumentMode mode)
{
	if (mode == PPS_DOCUMENT_MODE_PRESENTATION) {
		pps_window_run_presentation (window);
	}
	else if (mode == PPS_DOCUMENT_MODE_FULL_SCREEN) {
		pps_window_run_fullscreen (window);
	}
}

static void
pps_window_init_metadata_with_default_values (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GSettings  *settings = priv->default_settings;
	PpsMetadata *metadata = priv->metadata;

	/* Sidebar */
	if (!pps_metadata_has_key (metadata, "sidebar_visibility")) {
		pps_metadata_set_boolean (metadata, "sidebar_visibility",
					 g_settings_get_boolean (settings, "show-sidebar"));
	}
	if (!pps_metadata_has_key (metadata, "sidebar_page")) {
		gchar *sidebar_page_id = g_settings_get_string (settings, "sidebar-page");

		pps_metadata_set_string (metadata, "sidebar_page", sidebar_page_id);
		g_free (sidebar_page_id);
	}

	/* Document model */
	if (!pps_metadata_has_key (metadata, "continuous")) {
		pps_metadata_set_boolean (metadata, "continuous",
					 g_settings_get_boolean (settings, "continuous"));
	}
	if (!pps_metadata_has_key (metadata, "dual-page")) {
		pps_metadata_set_boolean (metadata, "dual-page",
					 g_settings_get_boolean (settings, "dual-page"));
	}
	if (!pps_metadata_has_key (metadata, "dual-page-odd-left")) {
		pps_metadata_set_boolean (metadata, "dual-page-odd-left",
					 g_settings_get_boolean (settings, "dual-page-odd-left"));
	}
	if (!pps_metadata_has_key (metadata, "rtl")) {
		pps_metadata_set_boolean (metadata, "rtl",
					 gtk_widget_get_default_direction () == GTK_TEXT_DIR_RTL ? TRUE : FALSE);
	}
	if (!pps_metadata_has_key (metadata, "inverted-colors")) {
		pps_metadata_set_boolean (metadata, "inverted-colors",
					 g_settings_get_boolean (settings, "inverted-colors"));
	}
	if (!pps_metadata_has_key (metadata, "sizing_mode")) {
		PpsSizingMode mode = g_settings_get_enum (settings, "sizing-mode");
		GEnumValue *enum_value = g_enum_get_value (g_type_class_peek (PPS_TYPE_SIZING_MODE), mode);

		pps_metadata_set_string (metadata, "sizing_mode", enum_value->value_nick);
	}

	if (!pps_metadata_has_key (metadata, "zoom")) {
		pps_metadata_set_double (metadata, "zoom",
					g_settings_get_double (settings, "zoom"));
	}

	if (!pps_metadata_has_key (metadata, "fullscreen")) {
		pps_metadata_set_boolean (metadata, "fullscreen",
					 g_settings_get_boolean (settings, "fullscreen"));
	}

	if (!pps_metadata_has_key (metadata, "window-maximized")) {
		pps_metadata_set_boolean (metadata, "window-maximized",
					  g_settings_get_boolean (settings, "window-maximized"));
	}
}

static void
setup_sidebar_from_metadata (PpsWindow *window)
{
	gboolean show_sidebar;
	const gchar *page_id;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->metadata)
		return;

	if (pps_metadata_get_boolean (priv->metadata, "sidebar_visibility", &show_sidebar))
		adw_overlay_split_view_set_show_sidebar (priv->split_view, show_sidebar);

	if (pps_metadata_get_string (priv->metadata, "sidebar_page", &page_id))
		pps_sidebar_set_visible_child_name (PPS_SIDEBAR (priv->sidebar), page_id);
}

static void
setup_model_from_metadata (PpsWindow *window)
{
	gint     page;
	const gchar   *sizing_mode;
	gdouble  zoom;
	gint     rotation;
	gboolean inverted_colors = FALSE;
	gboolean continuous = FALSE;
	gboolean dual_page = FALSE;
	gboolean dual_page_odd_left = FALSE;
	gboolean rtl = FALSE;
	gboolean fullscreen = FALSE;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->metadata)
		return;

	/* Current page */
	if (!priv->dest &&
	    pps_metadata_get_int (priv->metadata, "page", &page)) {
		pps_document_model_set_page (priv->model, page);
	}

	/* Sizing mode */
	if (pps_metadata_get_string (priv->metadata, "sizing_mode", &sizing_mode)) {
		GEnumValue *enum_value;

		enum_value = g_enum_get_value_by_nick
			(g_type_class_peek (PPS_TYPE_SIZING_MODE), sizing_mode);
		pps_document_model_set_sizing_mode (priv->model, enum_value->value);
	}

	/* Zoom */
	if (pps_document_model_get_sizing_mode (priv->model) == PPS_SIZING_FREE) {
		if (pps_metadata_get_double (priv->metadata, "zoom", &zoom)) {
			zoom *= pps_document_misc_get_widget_dpi  (GTK_WIDGET (window)) / 72.0;
			pps_document_model_set_scale (priv->model, zoom);
		}
	}

	/* Rotation */
	if (pps_metadata_get_int (priv->metadata, "rotation", &rotation)) {
		switch (rotation) {
		case 90:
			rotation = 90;
			break;
		case 180:
			rotation = 180;
			break;
		case 270:
			rotation = 270;
			break;
		default:
			rotation = 0;
			break;
		}
		pps_document_model_set_rotation (priv->model, rotation);
	}

	/* Inverted Colors */
	if (pps_metadata_get_boolean (priv->metadata, "inverted-colors", &inverted_colors)) {
		pps_document_model_set_inverted_colors (priv->model, inverted_colors);
	}

	/* Continuous */
	if (pps_metadata_get_boolean (priv->metadata, "continuous", &continuous)) {
		pps_document_model_set_continuous (priv->model, continuous);
	}

	/* Dual page */
	if (pps_metadata_get_boolean (priv->metadata, "dual-page", &dual_page)) {
		pps_document_model_set_page_layout (priv->model,
			dual_page ? PPS_PAGE_LAYOUT_DUAL : PPS_PAGE_LAYOUT_SINGLE);
	}

	/* Dual page odd pages left */
	if (pps_metadata_get_boolean (priv->metadata, "dual-page-odd-left", &dual_page_odd_left)) {
		pps_document_model_set_dual_page_odd_pages_left (priv->model, dual_page_odd_left);
	}

	/* Right to left document */
	if (pps_metadata_get_boolean (priv->metadata, "rtl", &rtl)) {
		pps_document_model_set_rtl (priv->model, rtl);
	}

	/* Fullscreen */
	if (pps_metadata_get_boolean (priv->metadata, "fullscreen", &fullscreen)) {
		if (fullscreen)
			pps_window_run_fullscreen (window);
	}
}

static void
monitor_get_dimensions (PpsWindow *pps_window,
                       gint     *width,
                       gint     *height)
{
	GdkDisplay  *display;
	GtkNative   *native;
	GdkSurface  *surface;
	GdkMonitor  *monitor;
	GdkRectangle geometry;

	*width = 0;
	*height = 0;

	display = gtk_widget_get_display (GTK_WIDGET (pps_window));
	native = gtk_widget_get_native (GTK_WIDGET (pps_window));

	if (native) {
		surface = gtk_native_get_surface (native);
		monitor = gdk_display_get_monitor_at_surface (display,
							     surface);
		gdk_monitor_get_geometry (monitor, &geometry);
		*width = geometry.width;
		*height = geometry.height;
	}
}

static void
setup_document_from_metadata (PpsWindow *window)
{
	gint    page, n_pages;
	gint    width;
	gint    height;
	gdouble width_ratio;
	gdouble height_ratio;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->metadata)
		return;

	setup_sidebar_from_metadata (window);

	/* Make sure to not open a document on the last page,
	 * since closing it on the last page most likely means the
	 * user was finished reading the document. In that case, reopening should
	 * show the first page. */
	page = pps_document_model_get_page (priv->model);
	n_pages = pps_document_get_n_pages (priv->document);
	if (page == n_pages - 1)
		pps_document_model_set_page (priv->model, 0);

	if (pps_metadata_get_int (priv->metadata, "window_width", &width) &&
	    pps_metadata_get_int (priv->metadata, "window_height", &height))
		return; /* size was already set in setup_size_from_metadata */

	/* Following code is intended to be executed first time a document is opened
	 * in Papers, that's why is located *after* the previous return that exits
	 * when papers metadata for window_width{height} already exists. */
	if (n_pages == 1)
		pps_document_model_set_page_layout (priv->model, PPS_PAGE_LAYOUT_SINGLE);
	else if (n_pages == 2)
		pps_document_model_set_dual_page_odd_pages_left (priv->model, TRUE);

	g_settings_get (priv->default_settings, "window-ratio", "(dd)", &width_ratio, &height_ratio);
	if (width_ratio > 0. && height_ratio > 0.) {
		gdouble    document_width;
		gdouble    document_height;
		gint       request_width;
		gint       request_height;
		gint       monitor_width;
		gint       monitor_height;

		pps_document_get_max_page_size (priv->document,
					       &document_width, &document_height);

		request_width = (gint)(width_ratio * document_width + 0.5);
		request_height = (gint)(height_ratio * document_height + 0.5);

		monitor_get_dimensions (window, &monitor_width, &monitor_height);
		if (monitor_width > 0 && monitor_height > 0) {
			request_width = MIN (request_width, monitor_width);
			request_height = MIN (request_height, monitor_height);
		}

		if (request_width > 0 && request_height > 0) {
			gtk_window_set_default_size (GTK_WINDOW (window),
						     request_width,
						     request_height);
		}
	}
}

static void
setup_size_from_metadata (PpsWindow *window)
{
	gint     width;
	gint     height;
	gboolean maximized;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->metadata)
		return;

        if (pps_metadata_get_int (priv->metadata, "window_width", &width) &&
	    pps_metadata_get_int (priv->metadata, "window_height", &height)) {
		gtk_window_set_default_size (GTK_WINDOW (window), width, height);
	}

	if (pps_metadata_get_boolean (priv->metadata, "window-maximized", &maximized)) {
		if (maximized) {
			gtk_window_maximize (GTK_WINDOW (window));
		} else {
			gtk_window_unmaximize (GTK_WINDOW (window));
		}
	}
}

static void
setup_view_from_metadata (PpsWindow *window)
{
	gboolean presentation;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->metadata)
		return;

	/* Presentation */
	if (pps_metadata_get_boolean (priv->metadata, "presentation", &presentation)) {
		if (presentation)
			pps_window_run_presentation (window);
	}

	/* Caret navigation mode */
	if (pps_view_supports_caret_navigation (PPS_VIEW (priv->view))) {
		gboolean caret_navigation;
		const gchar   *caret_position;

		if (pps_metadata_get_string (priv->metadata, "caret-position", &caret_position)) {
			GVariant *position;

			position = g_variant_parse (G_VARIANT_TYPE ("(uu)"), caret_position, NULL, NULL, NULL);
			if (position) {
				guint page, offset;

				g_variant_get (position, "(uu)", &page, &offset);
				g_variant_unref (position);

				pps_view_set_caret_cursor_position (PPS_VIEW (priv->view),
								   page, offset);
			}
		}

		if (pps_metadata_get_boolean (priv->metadata, "caret-navigation", &caret_navigation))
			pps_view_set_caret_navigation_enabled (PPS_VIEW (priv->view), caret_navigation);
	}
}

static void
page_cache_size_changed (GSettings *settings,
			 gchar     *key,
			 PpsWindow  *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	guint page_cache_mb;

	page_cache_mb = g_settings_get_uint (settings, GS_PAGE_CACHE_SIZE);
	pps_view_set_page_cache_size (PPS_VIEW (priv->view),
				     (gsize) page_cache_mb * 1024 * 1024);
}

static void
allow_links_change_zoom_changed (GSettings *settings,
			 gchar     *key,
			 PpsWindow  *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean allow_links_change_zoom = g_settings_get_boolean (settings, GS_ALLOW_LINKS_CHANGE_ZOOM);

	pps_view_set_allow_links_change_zoom (PPS_VIEW (priv->view), allow_links_change_zoom);
}

static void
pps_window_setup_default (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsDocumentModel *model = priv->model;
	GSettings       *settings = priv->default_settings;

	/* Sidebar */
	adw_overlay_split_view_set_show_sidebar (priv->split_view,
						 g_settings_get_boolean (settings, "show-sidebar"));
	g_signal_emit_by_name (priv->split_view, "notify::show-sidebar", NULL);

	/* Document model */
	pps_document_model_set_continuous (model, g_settings_get_boolean (settings, "continuous"));
	pps_document_model_set_page_layout (model, g_settings_get_boolean (settings, "dual-page") ?
			PPS_PAGE_LAYOUT_DUAL : PPS_PAGE_LAYOUT_SINGLE);
	pps_document_model_set_dual_page_odd_pages_left (model, g_settings_get_boolean (settings, "dual-page-odd-left"));
	pps_document_model_set_rtl (model, gtk_widget_get_default_direction () == GTK_TEXT_DIR_RTL ? TRUE : FALSE);
	pps_document_model_set_inverted_colors (model, g_settings_get_boolean (settings, "inverted-colors"));
	pps_document_model_set_sizing_mode (model, g_settings_get_enum (settings, "sizing-mode"));
	if (pps_document_model_get_sizing_mode (model) == PPS_SIZING_FREE)
		pps_document_model_set_scale (model, g_settings_get_double (settings, "zoom"));

	g_simple_action_set_state (
		G_SIMPLE_ACTION (g_action_map_lookup_action (G_ACTION_MAP (pps_window),
		                                             "enable-spellchecking")),
		g_variant_new_boolean (FALSE)
	);
	pps_view_set_enable_spellchecking (PPS_VIEW (priv->view),
		g_settings_get_boolean (settings, "enable-spellchecking"));

	pps_window_set_mode (pps_window, PPS_WINDOW_MODE_START_VIEW);

}

static void
override_restrictions_changed (GSettings *settings,
			       gchar     *key,
			       PpsWindow  *pps_window)
{
	pps_window_update_actions_sensitivity (pps_window);
}

#ifdef HAVE_DESKTOP_SCHEMAS
static void
lockdown_changed (GSettings   *lockdown,
		  const gchar *key,
		  PpsWindow    *pps_window)
{
	pps_window_update_actions_sensitivity (pps_window);
}
#endif

static GSettings *
pps_window_ensure_settings (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

        if (priv->settings != NULL)
                return priv->settings;

        priv->settings = g_settings_new (GS_SCHEMA_NAME);

        g_signal_connect (priv->settings,
                          "changed::"GS_OVERRIDE_RESTRICTIONS,
                          G_CALLBACK (override_restrictions_changed),
                          pps_window);
        g_signal_connect (priv->settings,
			  "changed::"GS_PAGE_CACHE_SIZE,
			  G_CALLBACK (page_cache_size_changed),
			  pps_window);
        g_signal_connect (priv->settings,
			  "changed::"GS_ALLOW_LINKS_CHANGE_ZOOM,
			  G_CALLBACK (allow_links_change_zoom_changed),
			  pps_window);

        return priv->settings;
}

static void
pps_window_setup_document (PpsWindow *pps_window)
{
	const PpsDocumentInfo *info;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsDocument *document = priv->document;

	priv->setup_document_idle = 0;

	pps_window_set_mode (pps_window, PPS_WINDOW_MODE_NORMAL);
	pps_window_title_set_document (priv->title, document);
	pps_window_title_set_filename (priv->title,
				      priv->display_name);

        pps_window_ensure_settings (pps_window);

#ifdef HAVE_DESKTOP_SCHEMAS
	if (!priv->lockdown_settings) {
		priv->lockdown_settings = g_settings_new (GS_LOCKDOWN_SCHEMA_NAME);
		g_signal_connect (priv->lockdown_settings,
				  "changed",
				  G_CALLBACK (lockdown_changed),
				  pps_window);
	}
#endif

	pps_window_update_actions_sensitivity (pps_window);

	if (priv->properties) {
		g_object_set (priv->properties, "document", priv->document, NULL);
	}

	info = pps_document_get_info (document);
	update_document_mode (pps_window, info->mode);

	if (PPS_WINDOW_IS_PRESENTATION (priv))
		gtk_widget_grab_focus (priv->presentation_view);
	else
		gtk_widget_grab_focus (priv->view);
}

static void
pps_window_set_document_metadata (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	const PpsDocumentInfo *info;

	if (!priv->metadata)
		return;

	info = pps_document_get_info (priv->document);
	if (info->fields_mask & PPS_DOCUMENT_INFO_TITLE && info->title && info->title[0] != '\0')
		pps_metadata_set_string (priv->metadata, "title", info->title);
	else
		pps_metadata_set_string (priv->metadata, "title", "");

	if (info->fields_mask & PPS_DOCUMENT_INFO_AUTHOR && info->author && info->author[0] != '\0')
		pps_metadata_set_string (priv->metadata, "author", info->author);
	else
		pps_metadata_set_string (priv->metadata, "author", "");
}

static void
pps_window_set_document (PpsWindow *pps_window, PpsDocument *document)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (priv->document == document)
		return;

	if (priv->document)
		g_object_unref (priv->document);
	priv->document = g_object_ref (document);

	pps_window_set_message_area (pps_window, NULL);

	pps_window_set_document_metadata (pps_window);

	if (pps_document_get_n_pages (document) <= 0) {
		pps_window_warning_message (pps_window, "%s",
					   _("The document contains no pages"));
	} else if (!pps_document_check_dimensions (document)) {
		pps_window_warning_message (pps_window, "%s",
					   _("The document contains only empty pages"));
	}

	pps_window_set_mode(pps_window, PPS_WINDOW_MODE_NORMAL);

	pps_window_update_actions_sensitivity (pps_window);

	if (PPS_WINDOW_IS_PRESENTATION (priv)) {
		gint current_page;

		current_page = pps_view_presentation_get_current_page (
			PPS_VIEW_PRESENTATION (priv->presentation_view));

		/* Update the model with the current presentation page */
		pps_document_model_set_page (priv->model, current_page);
		pps_window_run_presentation (pps_window);
	}

	priv->is_modified = FALSE;
	priv->modified_handler_id = g_signal_connect (document, "notify::modified", G_CALLBACK (pps_window_document_modified_cb), pps_window);

	g_clear_handle_id (&priv->setup_document_idle, g_source_remove);

	priv->setup_document_idle = g_idle_add_once ((GSourceOnceFunc)pps_window_setup_document, pps_window);
}


static void
pps_window_file_changed (PpsWindow *pps_window,
			gpointer  user_data)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (priv->settings &&
	    g_settings_get_boolean (priv->settings, GS_AUTO_RELOAD))
		pps_window_reload_document (pps_window, NULL);
}

static void
pps_window_password_view_unlock (PpsWindow *pps_window, gchar* password, GPasswordSave flags)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	g_assert (priv->load_job);

	pps_job_load_set_password (PPS_JOB_LOAD (priv->load_job), password);
	PPS_JOB_LOAD (priv->load_job)->password_save = flags;
	pps_job_scheduler_push_job (priv->load_job, PPS_JOB_PRIORITY_NONE);
	priv->password_view_cancelled = FALSE;
}

static void
pps_window_clear_load_job (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (priv->load_job != NULL) {
		if (!pps_job_is_finished (priv->load_job))
			pps_job_cancel (priv->load_job);

		g_signal_handlers_disconnect_by_func (priv->load_job, pps_window_load_job_cb, pps_window);
		g_clear_object (&priv->load_job);
	}
}

static void
pps_window_password_view_cancelled (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	priv->password_view_cancelled = TRUE;
	if (pps_window_is_start_view (pps_window)) {
		pps_window_clear_load_job (pps_window);
	}
}

static void
pps_window_clear_reload_job (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (priv->reload_job != NULL) {
		if (!pps_job_is_finished (priv->reload_job))
			pps_job_cancel (priv->reload_job);

		g_signal_handlers_disconnect_by_func (priv->reload_job, pps_window_reload_job_cb, pps_window);
		g_clear_object (&priv->reload_job);
	}
}

static void
pps_window_clear_local_uri (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (priv->local_uri) {
		pps_tmp_uri_unlink (priv->local_uri);
		g_clear_pointer (&priv->local_uri, g_free);
	}
}

static void
pps_window_handle_link (PpsWindow *pps_window,
		       PpsLinkDest *dest)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (dest) {
		PpsLink *link;
		PpsLinkAction *link_action;

		link_action = pps_link_action_new_dest (dest);
		link = pps_link_new (NULL, link_action);
		pps_view_handle_link (PPS_VIEW (priv->view), link);
		g_object_unref (link_action);
		g_object_unref (link);
	}
}

/* This callback will executed when load job will be finished.
 *
 * Since the flow of the error dialog is very confusing, we assume that both
 * document and uri will go away after this function is called, and thus we need
 * to ref/dup them.  Additionally, it needs to clear
 * priv->password_{uri,document}, and thus people who call this
 * function should _not_ necessarily expect those to exist after being
 * called. */
static void
pps_window_load_job_cb (PpsJob *job,
		       gpointer data)
{
	PpsWindow *pps_window = PPS_WINDOW (data);
	PpsJobLoad *job_load = PPS_JOB_LOAD (job);
	g_autoptr (PpsDocument) document = pps_job_load_get_loaded_document (job_load);
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GError *error = NULL;

	g_assert (job_load->uri);

	pps_window_hide_loading_message (pps_window);

	/* Success! */
	if (pps_job_is_succeeded (job, &error)) {
		pps_document_model_set_document (priv->model, g_steal_pointer (&document));

#ifdef ENABLE_DBUS
		pps_window_emit_doc_loaded (pps_window);
#endif
		setup_document_from_metadata (pps_window);
		setup_view_from_metadata (pps_window);

		gtk_recent_manager_add_item (gtk_recent_manager_get_default (), priv->uri);

		if (job_load->password) {
			pps_keyring_save_password (priv->uri,
						  job_load->password,
						  job_load->password_save);
		}

		pps_window_handle_link (pps_window, priv->dest);
		g_clear_object (&priv->dest);

		switch (priv->window_mode) {
		        case PPS_WINDOW_MODE_FULLSCREEN:
				pps_window_run_fullscreen (pps_window);
				break;
		        case PPS_WINDOW_MODE_PRESENTATION:
				pps_window_run_presentation (pps_window);
				break;
		        default:
				break;
		}
		/* Create a monitor for the document */
		priv->monitor = pps_file_monitor_new (priv->uri);
		g_signal_connect_swapped (priv->monitor, "changed",
					  G_CALLBACK (pps_window_file_changed),
					  pps_window);
		pps_window_clear_load_job (pps_window);
		return;
	}

	if (g_error_matches (error, PPS_DOCUMENT_ERROR, PPS_DOCUMENT_ERROR_ENCRYPTED) &&
	    PPS_IS_DOCUMENT_SECURITY (document)) {
		gchar *password;

		setup_view_from_metadata (pps_window);

		/* First look whether password is in keyring */
		password = pps_keyring_lookup_password (priv->uri);
		if (password) {
			if (job_load->password && strcmp (password, job_load->password) == 0) {
				/* Password in keyring is wrong */
				pps_job_load_set_password (job_load, NULL);
				/* FIXME: delete password from keyring? */
			} else {
				pps_job_load_set_password (job_load, password);
				pps_job_scheduler_push_job (job, PPS_JOB_PRIORITY_NONE);
				g_free (password);
				return;
			}

			g_free (password);
		}

		/* We need to ask the user for a password */
		gboolean wrong_password = job_load->password != NULL;
		pps_job_load_set_password (job_load, NULL);

		g_object_set(G_OBJECT (priv->password_view), "filename", priv->display_name, NULL);

		pps_window_set_mode (pps_window, PPS_WINDOW_MODE_PASSWORD_VIEW);

		g_object_set (G_OBJECT (priv->password_view), "ask-password", wrong_password, NULL);
	} else {
		adw_status_page_set_description (ADW_STATUS_PAGE (priv->error_page), error->message);
		pps_window_set_mode (pps_window, PPS_WINDOW_MODE_ERROR_VIEW);

		pps_window_clear_local_uri (pps_window);
		pps_window_clear_load_job (pps_window);
	}
}

static void
pps_window_reload_job_cb (PpsJob    *job,
			 PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (!pps_job_is_succeeded (job, NULL)) {
		pps_window_clear_reload_job (pps_window);
		priv->in_reload = FALSE;
		g_clear_object (&priv->dest);

		return;
	}

	pps_document_model_set_document (priv->model,
					pps_job_load_get_loaded_document (PPS_JOB_LOAD (job)));
	if (priv->dest) {
		pps_window_handle_link (pps_window, priv->dest);
		g_clear_object (&priv->dest);
	}

	/* Restart the search after reloading */
	if (gtk_search_bar_get_search_mode (GTK_SEARCH_BAR (priv->search_bar)))
		pps_search_box_restart (PPS_SEARCH_BOX (priv->search_box));

	pps_window_clear_reload_job (pps_window);
	priv->in_reload = FALSE;
}

/**
 * pps_window_get_uri:
 * @pps_window: The instance of the #PpsWindow.
 *
 * It returns the uri of the document showed in the #PpsWindow.
 *
 * Returns: the uri of the document showed in the #PpsWindow.
 */
const char *
pps_window_get_uri (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	return priv->uri;
}

/**
 * pps_window_close_dialogs:
 * @pps_window: The window where dialogs will be closed.
 *
 * It looks for password, print and properties dialogs and closes them and
 * frees them from memory. If there is any print job it does free it too.
 */
static void
pps_window_close_dialogs (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	g_clear_pointer (&priv->print_dialog, gtk_window_destroy);
	g_clear_pointer (&priv->properties, gtk_window_destroy);
}


static void
pps_window_clear_progress_idle (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	g_clear_handle_id (&priv->progress_idle, g_source_remove);
}

static void
reset_progress_idle (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	priv->progress_idle = 0;
}

static void
pps_window_show_progress_message (PpsWindow   *pps_window,
				 guint       interval,
				 GSourceFunc function)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	g_clear_handle_id (&priv->progress_idle, g_source_remove);

	priv->progress_idle =
		g_timeout_add_seconds_full (G_PRIORITY_DEFAULT,
					    interval, function,
					    pps_window,
					    (GDestroyNotify)reset_progress_idle);
}

static void
pps_window_reset_progress_cancellable (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (priv->progress_cancellable)
		g_cancellable_reset (priv->progress_cancellable);
	else
		priv->progress_cancellable = g_cancellable_new ();
}

static void
pps_window_progress_response_cb (PpsProgressMessageArea *area,
				gint                   response,
				PpsWindow              *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (response == GTK_RESPONSE_CANCEL)
		g_cancellable_cancel (priv->progress_cancellable);
	pps_window_set_message_area (pps_window, NULL);
}

static gboolean
show_loading_progress (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GtkWidget *area;
	gchar     *text;
	gchar     *display_name;

	if (priv->message_area)
		return G_SOURCE_REMOVE;

	text = g_uri_unescape_string (priv->uri, NULL);
	display_name = g_markup_escape_text (text, -1);
	g_free (text);
	text = g_strdup_printf (_("Loading document from “%s”"),
				display_name);

	area = pps_progress_message_area_new ("document-open-symbolic",
					     text,
					     _("C_ancel"),
					     GTK_RESPONSE_CANCEL,
					     NULL);
	g_signal_connect (pps_message_area_get_info_bar (PPS_MESSAGE_AREA (area)), "response",
			  G_CALLBACK (pps_window_progress_response_cb),
			  pps_window);

	pps_window_set_message_area (pps_window, area);

	g_free (text);
	g_free (display_name);

	return G_SOURCE_REMOVE;
}

static void
pps_window_load_remote_failed (PpsWindow *pps_window,
			      GError   *error)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gchar *text;
	gchar *display_name;

	pps_window_hide_loading_message (pps_window);
	priv->in_reload = FALSE;

	text = g_uri_unescape_string (priv->local_uri, NULL);
	display_name = g_markup_escape_text (text, -1);
	g_free (text);
	pps_window_error_message (pps_window, error,
				 _("Unable to open document “%s”."),
				 display_name);
	g_free (display_name);
	g_clear_pointer (&priv->local_uri, g_free);
	priv->uri_mtime = 0;
}

static void
set_uri_mtime (GFile        *source,
	       GAsyncResult *async_result,
	       PpsWindow     *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFileInfo *info;
	GError *error = NULL;

	info = g_file_query_info_finish (source, async_result, &error);

	if (error) {
		priv->uri_mtime = 0;
		g_error_free (error);
	} else {
		g_autoptr(GDateTime) dt = g_file_info_get_modification_date_time (info);
		if (dt != NULL)
			priv->uri_mtime = g_date_time_to_unix (dt);
		else
			priv->uri_mtime = 0;

		g_object_unref (info);
	}

	g_object_unref (source);
}

static void
mount_volume_ready_cb (GFile        *source,
		       GAsyncResult *async_result,
		       PpsWindow     *pps_window)
{
	GError *error = NULL;

	g_file_mount_enclosing_volume_finish (source, async_result, &error);

	if (error) {
		pps_window_load_remote_failed (pps_window, error);
		g_object_unref (source);
		g_error_free (error);
	} else {
		/* Volume successfully mounted,
		   try opening the file again */
		pps_window_load_file_remote (pps_window, source);
	}
}

static void
window_open_file_copy_ready_cb (GFile        *source,
				GAsyncResult *async_result,
				PpsWindow     *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GError *error = NULL;

	pps_window_clear_progress_idle (pps_window);
	pps_window_set_message_area (pps_window, NULL);

	g_file_copy_finish (source, async_result, &error);
	if (!error) {
		pps_job_scheduler_push_job (priv->load_job, PPS_JOB_PRIORITY_NONE);
		g_file_query_info_async (source,
					 G_FILE_ATTRIBUTE_TIME_MODIFIED,
					 0, G_PRIORITY_DEFAULT,
					 NULL,
					 (GAsyncReadyCallback)set_uri_mtime,
					 pps_window);
		return;
	}

	if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_MOUNTED)) {
		GMountOperation *operation;

		operation = gtk_mount_operation_new (GTK_WINDOW (pps_window));
		g_file_mount_enclosing_volume (source,
					       G_MOUNT_MOUNT_NONE,
					       operation, NULL,
					       (GAsyncReadyCallback)mount_volume_ready_cb,
					       pps_window);
		g_object_unref (operation);
	} else if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
		pps_window_clear_load_job (pps_window);
		pps_window_clear_local_uri (pps_window);
		g_clear_pointer (&priv->uri, g_free);
		g_clear_pointer (&priv->display_name, g_free);
		g_clear_pointer (&priv->edit_name, g_free);
		g_object_unref (source);

		pps_window_hide_loading_message (pps_window);
	} else {
		pps_window_load_remote_failed (pps_window, error);
		g_object_unref (source);
	}

	g_error_free (error);
}

static void
window_open_file_copy_progress_cb (goffset   n_bytes,
				   goffset   total_bytes,
				   PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gchar *status;
	gdouble fraction;

	if (!priv->message_area)
		return;

	if (total_bytes <= 0)
		return;

	fraction = n_bytes / (gdouble)total_bytes;
	status = g_strdup_printf (_("Downloading document (%d%%)"),
				  (gint)(fraction * 100));

	pps_progress_message_area_set_status (PPS_PROGRESS_MESSAGE_AREA (priv->message_area),
					     status);
	pps_progress_message_area_set_fraction (PPS_PROGRESS_MESSAGE_AREA (priv->message_area),
					       fraction);
	g_free (status);
}

static void
pps_window_load_file_remote (PpsWindow *pps_window,
			    GFile    *source_file)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFile *target_file;

	if (!priv->local_uri) {
		char *base_name, *template;
                GFile *tmp_file;
                GError *err = NULL;

		/* We'd like to keep extension of source uri since
		 * it helps to resolve some mime types, say cbz.
                 */
		base_name = priv->edit_name;
                template = g_strdup_printf ("document.XXXXXX-%s", base_name);

                tmp_file = pps_mkstemp_file (template, &err);
		g_free (template);
                if (tmp_file == NULL) {
                        pps_window_error_message (pps_window, err,
                                                 "%s", _("Failed to load remote file."));
                        g_error_free (err);
                        return;
                }

		priv->local_uri = g_file_get_uri (tmp_file);
		g_object_unref (tmp_file);

		pps_job_load_set_uri (PPS_JOB_LOAD (priv->load_job),
				     priv->local_uri);
	}

	pps_window_reset_progress_cancellable (pps_window);

	target_file = g_file_new_for_uri (priv->local_uri);
	g_file_copy_async (source_file, target_file,
			   G_FILE_COPY_OVERWRITE,
			   G_PRIORITY_DEFAULT,
			   priv->progress_cancellable,
			   (GFileProgressCallback)window_open_file_copy_progress_cb,
			   pps_window,
			   (GAsyncReadyCallback)window_open_file_copy_ready_cb,
			   pps_window);
	g_object_unref (target_file);

	pps_window_show_progress_message (pps_window, 1,
					 (GSourceFunc)show_loading_progress);
}

static void
set_filenames (PpsWindow *pps_window, GFile *f)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFileInfo       *info;
	GError          *error = NULL;

	g_clear_pointer (&priv->display_name, g_free);
	g_clear_pointer (&priv->edit_name, g_free);

	info = g_file_query_info (f,
				  G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME ","
				  G_FILE_ATTRIBUTE_STANDARD_EDIT_NAME,
			          G_FILE_QUERY_INFO_NONE, NULL, &error);
	if (info) {
		priv->display_name = g_strdup (g_file_info_get_display_name (info));
		priv->edit_name = g_strdup (g_file_info_get_edit_name (info));
		g_object_unref (info);
	} else {
		g_warning ("%s: %s", G_STRFUNC, error->message);
		g_error_free (error);
	}

	if (!priv->display_name)
		priv->display_name = g_file_get_basename (f);
	if (!priv->edit_name)
		priv->edit_name = g_file_get_basename (f);
}

static void
open_uri_check_local_cb (GObject      *object,
                         GAsyncResult *res,
                         gpointer      user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFile *source_file = G_FILE (object);
	g_autoptr (GFileInputStream) source_stream = NULL;

	source_stream = g_file_read_finish (source_file, res, NULL);

	if (source_stream && !g_seekable_can_seek (G_SEEKABLE (source_stream))) {
		pps_window_load_file_remote (pps_window, source_file);
	} else {
		pps_window_show_loading_message (pps_window);
		g_object_unref (source_file);
		pps_job_scheduler_push_job (priv->load_job, PPS_JOB_PRIORITY_NONE);
	}
}

/**
 * pps_window_open_uri:
 * @pps_window: a `PpsWindow`
 * @uri: uri to open
 * @dest: (nullable): destination to point to
 * @mode: open mode
 */
void
pps_window_open_uri (PpsWindow       *pps_window,
		    const char     *uri,
		    PpsLinkDest     *dest,
		    PpsWindowRunMode mode)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFile *source_file;
	g_autofree char *path = NULL;

	priv->in_reload = FALSE;

	if (priv->uri &&
	    g_ascii_strcasecmp (priv->uri, uri) == 0 &&
	    !priv->password_view_cancelled) {
		if (pps_window_check_document_modified (pps_window, PPS_WINDOW_ACTION_RELOAD))
			return;
		pps_window_reload_document (pps_window, dest);
		return;
	}

	g_clear_object (&priv->monitor);

	pps_window_close_dialogs (pps_window);
	pps_window_clear_load_job (pps_window);
	pps_window_clear_local_uri (pps_window);

	pps_window_set_mode (pps_window, mode);

	source_file = g_file_new_for_uri (uri);

	g_clear_pointer (&priv->uri, g_free);
	path = g_file_get_path (source_file);
	/* Try to use FUSE-backed files if possible to avoid downloading */
	if (path)
		priv->uri = g_filename_to_uri (path, NULL, NULL);
	else
		priv->uri = g_strdup (uri);

	g_clear_object (&priv->metadata);
	g_clear_object (&priv->bookmarks);

	if (pps_is_metadata_supported_for_file (source_file)) {
		priv->metadata = pps_metadata_new (source_file);
		pps_window_init_metadata_with_default_values (pps_window);
		if (priv->metadata) {
			priv->bookmarks = pps_bookmarks_new (priv->metadata);
			pps_sidebar_bookmarks_set_bookmarks (PPS_SIDEBAR_BOOKMARKS (priv->sidebar_bookmarks),
							    priv->bookmarks);
		}
	}

	g_clear_object (&priv->dest);
	priv->dest = dest ? g_object_ref (dest) : NULL;

	set_filenames (pps_window, source_file);
	setup_size_from_metadata (pps_window);
	setup_model_from_metadata (pps_window);

	priv->load_job = pps_job_load_new ();
	pps_job_load_set_uri (PPS_JOB_LOAD (priv->load_job), priv->uri);

	g_signal_connect (priv->load_job,
			  "finished",
			  G_CALLBACK (pps_window_load_job_cb),
			  pps_window);

	if (path == NULL && !priv->local_uri) {
		pps_window_load_file_remote (pps_window, source_file);
	} else {
		/* source_file is probably local, but make sure it's seekable
		 * before loading it directly.
		 */
		g_file_read_async (source_file,
				   G_PRIORITY_DEFAULT, NULL,
				   open_uri_check_local_cb, pps_window);
	}
}

void
pps_window_open_document (PpsWindow       *pps_window,
			 PpsDocument     *document,
			 PpsLinkDest     *dest,
			 PpsWindowRunMode mode)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (document == priv->document)
		return;

	pps_window_close_dialogs (pps_window);
	pps_window_clear_load_job (pps_window);
	pps_window_clear_local_uri (pps_window);

	g_clear_object (&priv->monitor);

	if (priv->uri)
		g_free (priv->uri);
	priv->uri = g_strdup (pps_document_get_uri (document));

	setup_size_from_metadata (pps_window);
	setup_model_from_metadata (pps_window);

	pps_document_model_set_document (priv->model, document);

	setup_document_from_metadata (pps_window);
	setup_view_from_metadata (pps_window);

	if (dest) {
		PpsLink *link;
		PpsLinkAction *link_action;

		link_action = pps_link_action_new_dest (dest);
		link = pps_link_new (NULL, link_action);
		pps_view_handle_link (PPS_VIEW (priv->view), link);
		g_object_unref (link_action);
		g_object_unref (link);
	}

	switch (mode) {
	case PPS_WINDOW_MODE_FULLSCREEN:
		pps_window_run_fullscreen (pps_window);
		break;
	case PPS_WINDOW_MODE_PRESENTATION:
		pps_window_run_presentation (pps_window);
		break;
	default:
		break;
	}

	/* Create a monitor for the document */
	priv->monitor = pps_file_monitor_new (priv->uri);
	g_signal_connect_swapped (priv->monitor, "changed",
				  G_CALLBACK (pps_window_file_changed),
				  pps_window);
}

static void
pps_window_reload_local (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	const gchar *uri;

	uri = priv->local_uri ? priv->local_uri : priv->uri;
	priv->reload_job = pps_job_load_new ();
	pps_job_load_set_uri (PPS_JOB_LOAD (priv->reload_job), uri);

	g_signal_connect (priv->reload_job, "finished",
			  G_CALLBACK (pps_window_reload_job_cb),
			  pps_window);
	pps_job_scheduler_push_job (priv->reload_job, PPS_JOB_PRIORITY_NONE);
}

static gboolean
show_reloading_progress (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GtkWidget *area;
	gchar     *text;

	if (priv->message_area)
		return G_SOURCE_REMOVE;

	text = g_strdup_printf (_("Reloading document from %s"),
				priv->uri);

	area = pps_progress_message_area_new ("view-refresh-symbolic",
					     text,
					     _("C_ancel"),
					     GTK_RESPONSE_CANCEL,
					     NULL);
	g_signal_connect (pps_message_area_get_info_bar (PPS_MESSAGE_AREA (area)), "response",
			  G_CALLBACK (pps_window_progress_response_cb),
			  pps_window);

	gtk_widget_set_visible (area, TRUE);
	pps_window_set_message_area (pps_window, area);
	g_free (text);

	return G_SOURCE_REMOVE;
}

static void
reload_remote_copy_ready_cb (GFile        *remote,
			     GAsyncResult *async_result,
			     PpsWindow     *pps_window)
{
	GError *error = NULL;

	pps_window_clear_progress_idle (pps_window);

	g_file_copy_finish (remote, async_result, &error);
	if (error) {
		if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
			pps_window_error_message (pps_window, error,
						 "%s", _("Failed to reload document."));
		g_error_free (error);
	} else {
		pps_window_reload_local (pps_window);
	}

	g_object_unref (remote);
}

static void
reload_remote_copy_progress_cb (goffset   n_bytes,
				goffset   total_bytes,
				PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gchar *status;
	gdouble fraction;

	if (!priv->message_area)
		return;

	if (total_bytes <= 0)
		return;

	fraction = n_bytes / (gdouble)total_bytes;
	status = g_strdup_printf (_("Downloading document (%d%%)"),
				  (gint)(fraction * 100));

	pps_progress_message_area_set_status (PPS_PROGRESS_MESSAGE_AREA (priv->message_area),
					     status);
	pps_progress_message_area_set_fraction (PPS_PROGRESS_MESSAGE_AREA (priv->message_area),
					       fraction);
	g_free (status);
}

static void
query_remote_uri_mtime_cb (GFile        *remote,
			   GAsyncResult *async_result,
			   PpsWindow     *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFileInfo *info;
	gint64     mtime;
	GError    *error = NULL;
	g_autoptr(GDateTime) dt = NULL;

	info = g_file_query_info_finish (remote, async_result, &error);
	if (error) {
		g_error_free (error);
		g_object_unref (remote);
		pps_window_reload_local (pps_window);

		return;
	}

	dt = g_file_info_get_modification_date_time (info);
	if (dt == NULL) {
		g_object_unref (remote);
		pps_window_reload_local (pps_window);
		g_object_unref (info);

		return;
	}

	 mtime = g_date_time_to_unix (dt);

	if (priv->uri_mtime != mtime) {
		GFile *target_file;

		/* Remote file has changed */
		priv->uri_mtime = mtime;

		pps_window_reset_progress_cancellable (pps_window);

		target_file = g_file_new_for_uri (priv->local_uri);
		g_file_copy_async (remote, target_file,
				   G_FILE_COPY_OVERWRITE,
				   G_PRIORITY_DEFAULT,
				   priv->progress_cancellable,
				   (GFileProgressCallback)reload_remote_copy_progress_cb,
				   pps_window,
				   (GAsyncReadyCallback)reload_remote_copy_ready_cb,
				   pps_window);
		g_object_unref (target_file);
		pps_window_show_progress_message (pps_window, 1,
						 (GSourceFunc)show_reloading_progress);
	} else {
		g_object_unref (remote);
		pps_window_reload_local (pps_window);
	}

	g_object_unref (info);
}

static void
pps_window_reload_remote (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFile *remote;

	remote = g_file_new_for_uri (priv->uri);
	/* Reload the remote uri only if it has changed */
	g_file_query_info_async (remote,
				 G_FILE_ATTRIBUTE_TIME_MODIFIED,
				 0, G_PRIORITY_DEFAULT,
				 NULL,
				 (GAsyncReadyCallback)query_remote_uri_mtime_cb,
				 pps_window);
}

static void
pps_window_reload_document (PpsWindow *pps_window,
			   PpsLinkDest *dest)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_window_clear_reload_job (pps_window);
	priv->in_reload = TRUE;

	if (priv->dest)
		g_object_unref (priv->dest);
	priv->dest = dest ? g_object_ref (dest) : NULL;

	if (priv->local_uri) {
		pps_window_reload_remote (pps_window);
	} else {
		pps_window_reload_local (pps_window);
	}
}

static const gchar *
get_settings_key_for_directory (GUserDirectory directory)
{
        switch (directory) {
                case G_USER_DIRECTORY_PICTURES:
                        return GS_LAST_PICTURES_DIRECTORY;
                case G_USER_DIRECTORY_DOCUMENTS:
                default:
                        return GS_LAST_DOCUMENT_DIRECTORY;
        }
}

static void
pps_window_file_dialog_restore_folder (PpsWindow       *window,
				       GtkFileDialog   *dialog,
				       GUserDirectory   directory)
{
        const gchar *dir;
        gchar *folder_path;
        GFile *folder;

        g_settings_get (pps_window_ensure_settings (window),
                        get_settings_key_for_directory (directory),
                        "ms", &folder_path);

        dir = g_get_user_special_dir (directory);
        folder_path = folder_path ?: g_strdup (dir) ?: g_strdup (g_get_home_dir ());

        folder = g_file_new_for_path (folder_path);
	gtk_file_dialog_set_initial_folder (dialog, folder);

        g_free (folder_path);
        g_object_unref (folder);
}

static void
pps_window_file_dialog_save_folder (PpsWindow       *window,
				    GFile           *file,
				    GUserDirectory   directory)
{
        gchar *path = NULL;
        GFile *folder = NULL;

	if (file)
		folder = g_file_get_parent (file);


	/* Store 'nothing' if the folder is the default one */
        if (folder && g_strcmp0 (g_file_get_path (folder), g_get_user_special_dir (directory)) != 0)
                path = g_file_get_path (folder);

	g_clear_object (&folder);

        g_settings_set (pps_window_ensure_settings (window),
                        get_settings_key_for_directory (directory),
                        "ms", path);
        g_free (path);
}

static void
file_open_dialog_response_cb (GtkFileDialog	*dialog,
			      GAsyncResult	*result,
			      PpsWindow		*pps_window)
{
	GListModel *files = gtk_file_dialog_open_multiple_finish (dialog, result, NULL);

	if (files) {
		pps_application_open_uri_list (PPS_APP, files);
	}

	if (g_list_model_get_n_items (files))
		pps_window_file_dialog_save_folder (pps_window,
						     g_list_model_get_item (files, 0),
						     G_USER_DIRECTORY_DOCUMENTS);
}

static void
pps_window_cmd_file_open (GSimpleAction *action,
			 GVariant      *parameter,
			 gpointer       user_data)
{
	PpsWindow  *window = user_data;

	GtkFileDialog *dialog = gtk_file_dialog_new ();

	gtk_file_dialog_set_modal (dialog, TRUE);
	pps_document_factory_add_filters (dialog, NULL);
        pps_window_file_dialog_restore_folder (window, dialog,
                                               G_USER_DIRECTORY_DOCUMENTS);

	gtk_file_dialog_open_multiple (dialog, GTK_WINDOW (window), NULL,
		(GAsyncReadyCallback)file_open_dialog_response_cb, window);
}

static void
pps_window_open_copy_at_dest (PpsWindow   *window,
			     PpsLinkDest *dest)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	PpsWindow *new_window = PPS_WINDOW (pps_window_new ());
	PpsWindowPrivate *new_priv = GET_PRIVATE (new_window);

	if (priv->metadata)
		new_priv->metadata = g_object_ref (priv->metadata);
	new_priv->display_name = g_strdup (priv->display_name);
	new_priv->edit_name = g_strdup (priv->edit_name);
	pps_window_open_document (new_window,
				 priv->document,
				 dest, 0);
	gtk_widget_set_visible (new_priv->sidebar,
				gtk_widget_is_visible (priv->sidebar));
	adw_overlay_split_view_set_show_sidebar (new_priv->split_view,
						 adw_overlay_split_view_get_show_sidebar (priv->split_view));

	gtk_window_present (GTK_WINDOW (new_window));
}

static void
pps_window_cmd_file_open_copy (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	PpsWindow *window = user_data;

	pps_window_open_copy_at_dest (window, NULL);
}

static void
show_saving_progress (GFile *dst)
{
	PpsWindow  *pps_window;
	GtkWidget *area;
	gchar     *text;
	gchar     *uri;
	PpsSaveType save_type;

	pps_window = PPS_WINDOW (g_object_get_data (G_OBJECT (dst), "pps-window"));
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	priv->progress_idle = 0;

	if (priv->message_area)
		return;

	save_type = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (dst), "save-type"));
	uri = g_file_get_uri (dst);
	switch (save_type) {
	case PPS_SAVE_DOCUMENT:
		text = g_strdup_printf (_("Saving document to %s"), uri);
		break;
	case PPS_SAVE_ATTACHMENT:
		text = g_strdup_printf (_("Saving attachment to %s"), uri);
		break;
	case PPS_SAVE_IMAGE:
		text = g_strdup_printf (_("Saving image to %s"), uri);
		break;
	default:
		g_assert_not_reached ();
	}
	g_free (uri);
	area = pps_progress_message_area_new ("document-save-symbolic",
					     text,
					     _("C_ancel"),
					     GTK_RESPONSE_CANCEL,
					     NULL);
	g_signal_connect (pps_message_area_get_info_bar (PPS_MESSAGE_AREA (area)), "response",
			  G_CALLBACK (pps_window_progress_response_cb),
			  pps_window);
	gtk_widget_set_visible (area, TRUE);
	pps_window_set_message_area (pps_window, area);
	g_free (text);
}

static void
window_save_file_copy_ready_cb (GFile        *src,
				GAsyncResult *async_result,
				GFile        *dst)
{
	PpsWindow *pps_window;
	GError   *error = NULL;

	pps_window = PPS_WINDOW (g_object_get_data (G_OBJECT (dst), "pps-window"));
	pps_window_clear_progress_idle (pps_window);

	if (g_file_copy_finish (src, async_result, &error)) {
		pps_tmp_file_unlink (src);
		return;
	}

	if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
		gchar *name;

		name = g_file_get_basename (dst);
		pps_window_error_message (pps_window, error,
					 _("The file could not be saved as “%s”."),
					 name);
		g_free (name);
	}
	pps_tmp_file_unlink (src);
	g_error_free (error);
}

static void
window_save_file_copy_progress_cb (goffset n_bytes,
				   goffset total_bytes,
				   GFile  *dst)
{
	PpsWindow  *pps_window;
	PpsSaveType save_type;
	gchar     *status;
	gdouble    fraction;

	pps_window = PPS_WINDOW (g_object_get_data (G_OBJECT (dst), "pps-window"));
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (!priv->message_area)
		return;

	if (total_bytes <= 0)
		return;

	fraction = n_bytes / (gdouble)total_bytes;
	save_type = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (dst), "save-type"));

	switch (save_type) {
	case PPS_SAVE_DOCUMENT:
		status = g_strdup_printf (_("Uploading document (%d%%)"),
					  (gint)(fraction * 100));
		break;
	case PPS_SAVE_ATTACHMENT:
		status = g_strdup_printf (_("Uploading attachment (%d%%)"),
					  (gint)(fraction * 100));
		break;
	case PPS_SAVE_IMAGE:
		status = g_strdup_printf (_("Uploading image (%d%%)"),
					  (gint)(fraction * 100));
		break;
	default:
		g_assert_not_reached ();
	}

	pps_progress_message_area_set_status (PPS_PROGRESS_MESSAGE_AREA (priv->message_area),
					     status);
	pps_progress_message_area_set_fraction (PPS_PROGRESS_MESSAGE_AREA (priv->message_area),
					       fraction);

	g_free (status);
}

static void
pps_window_save_remote (PpsWindow  *pps_window,
		       PpsSaveType save_type,
		       GFile     *src,
		       GFile     *dst)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_window_reset_progress_cancellable (pps_window);
	g_object_set_data (G_OBJECT (dst), "pps-window", pps_window);
	g_object_set_data (G_OBJECT (dst), "save-type", GINT_TO_POINTER (save_type));
	g_file_copy_async (src, dst,
			   G_FILE_COPY_OVERWRITE,
			   G_PRIORITY_DEFAULT,
			   priv->progress_cancellable,
			   (GFileProgressCallback)window_save_file_copy_progress_cb,
			   dst,
			   (GAsyncReadyCallback)window_save_file_copy_ready_cb,
			   dst);
	priv->progress_idle =
		g_timeout_add_once (1000,
				    (GSourceOnceFunc)show_saving_progress,
				    dst);
}

static void
pps_window_clear_save_job (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (priv->save_job != NULL) {
		if (!pps_job_is_finished (priv->save_job))
			pps_job_cancel (priv->save_job);

		g_signal_handlers_disconnect_by_func (priv->save_job,
						      pps_window_save_job_cb,
						      pps_window);
		g_clear_object (&priv->save_job);
	}
}

static void
pps_window_save_job_cb (PpsJob     *job,
		       PpsWindow  *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GError *error;
	if (!pps_job_is_succeeded (job, &error)) {
		priv->close_after_save = FALSE;
		pps_window_error_message (window, error,
					 _("The file could not be saved as “%s”."),
					 PPS_JOB_SAVE (job)->uri);
	} else {
		gtk_recent_manager_add_item (gtk_recent_manager_get_default (),
					     PPS_JOB_SAVE (job)->uri);
	}

	pps_window_clear_save_job (window);

	if (priv->close_after_save)
		g_idle_add_once ((GSourceOnceFunc)gtk_window_destroy, window);
}

static void
file_save_dialog_response_cb (GtkFileDialog	*dialog,
			      GAsyncResult	*result,
			      PpsWindow		*pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFile *file;
	gchar *uri;

	file = gtk_file_dialog_save_finish (dialog, result, NULL);

	if (!file) {
		priv->close_after_save = FALSE;
		return;
	}

        pps_window_file_dialog_save_folder (pps_window, file,
                                            G_USER_DIRECTORY_DOCUMENTS);

	uri = g_file_get_uri (file);
	g_clear_object (&file);

	/* FIXME: remote copy should be done here rather than in the save job,
	 * so that we can track progress and cancel the operation
	 */

	pps_window_clear_save_job (pps_window);
	priv->save_job = pps_job_save_new (priv->document,
						     uri, priv->uri);
	g_signal_connect (priv->save_job, "finished",
			  G_CALLBACK (pps_window_save_job_cb),
			  pps_window);
	/* The priority doesn't matter for this job */
	pps_job_scheduler_push_job (priv->save_job, PPS_JOB_PRIORITY_NONE);

	g_free (uri);
}

static void
pps_window_save_as (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GtkFileDialog *dialog;
	gchar *base_name, *dir_name, *var_tmp_dir, *tmp_dir;
	GFile *file, *parent, *dest_file;
	const gchar *default_dir, *dest_dir, *documents_dir;

	dialog = gtk_file_dialog_new ();

	gtk_file_dialog_set_title (dialog, _("Save As…"));
	pps_document_factory_add_filters (dialog, priv->document);

	file = g_file_new_for_uri (priv->uri);
	base_name = priv->edit_name;
	parent = g_file_get_parent (file);
	dir_name = g_file_get_path (parent);
	g_object_unref (parent);

	gtk_file_dialog_set_initial_name (dialog, base_name);

	documents_dir = g_get_user_special_dir (G_USER_DIRECTORY_DOCUMENTS);
	default_dir = g_file_test (documents_dir, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR) ?
	              documents_dir : g_get_home_dir ();

	tmp_dir = g_build_filename ("tmp", NULL);
	var_tmp_dir = g_build_filename ("var", "tmp", NULL);
	dest_dir = dir_name && !g_str_has_prefix (dir_name, g_get_tmp_dir ()) &&
			    !g_str_has_prefix (dir_name, tmp_dir) &&
	                    !g_str_has_prefix (dir_name, var_tmp_dir) ?
	                    dir_name : default_dir;

	dest_file = g_file_new_for_path (dest_dir);
	gtk_file_dialog_set_initial_folder (dialog, dest_file);

	g_object_unref (file);
	g_object_unref (dest_file);
	g_free (tmp_dir);
	g_free (var_tmp_dir);
	g_free (dir_name);

	gtk_file_dialog_set_modal (dialog, TRUE);

	gtk_file_dialog_save (dialog, GTK_WINDOW (pps_window), NULL,
			(GAsyncReadyCallback)file_save_dialog_response_cb, pps_window);
}

static void
pps_window_cmd_save_as (GSimpleAction *action,
		       GVariant      *parameter,
		       gpointer       user_data)
{
	PpsWindow *window = user_data;

	pps_window_save_as (window);
}

static gchar *
print_settings_filename (gboolean create)
{
	if (create) {
		g_autofree gchar *dot_dir = g_build_filename (g_get_user_config_dir (),"papers", NULL);
		g_mkdir_with_parents (dot_dir, 0700);
	}

	return g_build_filename (g_get_user_config_dir (), "papers", PPS_PRINT_SETTINGS_FILE, NULL);
}

static GKeyFile *
get_print_settings_file (void)
{
	GKeyFile *print_settings_file;
	gchar    *filename;
        GError *error = NULL;

	print_settings_file = g_key_file_new ();

	filename = print_settings_filename (FALSE);
        if (!g_key_file_load_from_file (print_settings_file,
                                        filename,
                                        G_KEY_FILE_KEEP_COMMENTS |
                                        G_KEY_FILE_KEEP_TRANSLATIONS,
                                        &error)) {

                /* Don't warn if the file simply doesn't exist */
                if (!g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
			g_warning ("%s", error->message);

                g_error_free (error);
	}

	g_free (filename);

	return print_settings_file;
}

static void
save_print_setting_file (GKeyFile *key_file)
{
	gchar  *filename;
	GError *error = NULL;

	filename = print_settings_filename (TRUE);
	g_key_file_save_to_file (key_file, filename, &error);
	if (error) {
		g_warning ("Failed to save print settings: %s", error->message);
		g_error_free (error);
	}
	g_free (filename);
}

static void
pps_window_save_print_settings (PpsWindow         *window,
			       GtkPrintSettings *print_settings)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GKeyFile *key_file;
	gint      i;

	key_file = get_print_settings_file ();
	gtk_print_settings_to_key_file (print_settings, key_file, PPS_PRINT_SETTINGS_GROUP);

	/* Always Remove n_copies from global settings */
	g_key_file_remove_key (key_file, PPS_PRINT_SETTINGS_GROUP, GTK_PRINT_SETTINGS_N_COPIES, NULL);

	/* Save print settings that are specific to the document */
	for (i = 0; i < G_N_ELEMENTS (document_print_settings); i++) {
		/* Remove it from global settings */
		g_key_file_remove_key (key_file, PPS_PRINT_SETTINGS_GROUP,
				       document_print_settings[i], NULL);

		if (priv->metadata) {
			const gchar *value;

			value = gtk_print_settings_get (print_settings,
							document_print_settings[i]);
			pps_metadata_set_string (priv->metadata,
						document_print_settings[i], value);
		}
	}

	save_print_setting_file (key_file);
	g_key_file_free (key_file);
}

static void
pps_window_save_print_page_setup (PpsWindow     *window,
				 GtkPageSetup *page_setup)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GKeyFile *key_file;

	key_file = get_print_settings_file ();
	gtk_page_setup_to_key_file (page_setup, key_file, PPS_PAGE_SETUP_GROUP);

	/* Do not save document settings in global file */
	g_key_file_remove_key (key_file, PPS_PAGE_SETUP_GROUP,
			       "page-setup-orientation", NULL);
	g_key_file_remove_key (key_file, PPS_PAGE_SETUP_GROUP,
			       "page-setup-margin-top", NULL);
	g_key_file_remove_key (key_file, PPS_PAGE_SETUP_GROUP,
			       "page-setup-margin-bottom", NULL);
	g_key_file_remove_key (key_file, PPS_PAGE_SETUP_GROUP,
			       "page-setup-margin-left", NULL);
	g_key_file_remove_key (key_file, PPS_PAGE_SETUP_GROUP,
			       "page-setup-margin-right", NULL);

	save_print_setting_file (key_file);
	g_key_file_free (key_file);

	if (!priv->metadata)
		return;

	/* Save page setup options that are specific to the document */
	pps_metadata_set_int (priv->metadata, "page-setup-orientation",
			     gtk_page_setup_get_orientation (page_setup));
	pps_metadata_set_double (priv->metadata, "page-setup-margin-top",
				gtk_page_setup_get_top_margin (page_setup, GTK_UNIT_MM));
	pps_metadata_set_double (priv->metadata, "page-setup-margin-bottom",
				gtk_page_setup_get_bottom_margin (page_setup, GTK_UNIT_MM));
	pps_metadata_set_double (priv->metadata, "page-setup-margin-left",
				gtk_page_setup_get_left_margin (page_setup, GTK_UNIT_MM));
	pps_metadata_set_double (priv->metadata, "page-setup-margin-right",
				gtk_page_setup_get_right_margin (page_setup, GTK_UNIT_MM));
}

static void
pps_window_load_print_settings_from_metadata (PpsWindow         *window,
					     GtkPrintSettings *print_settings)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	gint i;

	if (!priv->metadata)
		return;

	/* Load print setting that are specific to the document */
	for (i = 0; i < G_N_ELEMENTS (document_print_settings); i++) {
		const gchar *value = NULL;

		pps_metadata_get_string (priv->metadata,
					document_print_settings[i], &value);
		gtk_print_settings_set (print_settings,
					document_print_settings[i], value);
	}
}

static void
pps_window_load_print_page_setup_from_metadata (PpsWindow     *window,
					       GtkPageSetup *page_setup)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	gint          int_value;
	gdouble       double_value;
	GtkPaperSize *paper_size = gtk_page_setup_get_paper_size (page_setup);

	/* Load page setup options that are specific to the document */
	if (priv->metadata &&
	    pps_metadata_get_int (priv->metadata, "page-setup-orientation", &int_value)) {
		gtk_page_setup_set_orientation (page_setup, int_value);
	} else {
		gtk_page_setup_set_orientation (page_setup, GTK_PAGE_ORIENTATION_PORTRAIT);
	}

	if (priv->metadata &&
	    pps_metadata_get_double (priv->metadata, "page-setup-margin-top", &double_value)) {
		gtk_page_setup_set_top_margin (page_setup, double_value, GTK_UNIT_MM);
	} else {
		gtk_page_setup_set_top_margin (page_setup,
					       gtk_paper_size_get_default_top_margin (paper_size, GTK_UNIT_MM),
					       GTK_UNIT_MM);
	}

	if (priv->metadata &&
	    pps_metadata_get_double (priv->metadata, "page-setup-margin-bottom", &double_value)) {
		gtk_page_setup_set_bottom_margin (page_setup, double_value, GTK_UNIT_MM);
	} else {
		gtk_page_setup_set_bottom_margin (page_setup,
						  gtk_paper_size_get_default_bottom_margin (paper_size, GTK_UNIT_MM),
						  GTK_UNIT_MM);
	}

	if (priv->metadata &&
	    pps_metadata_get_double (priv->metadata, "page-setup-margin-left", &double_value)) {
		gtk_page_setup_set_left_margin (page_setup, double_value, GTK_UNIT_MM);
	} else {
		gtk_page_setup_set_left_margin (page_setup,
						gtk_paper_size_get_default_left_margin (paper_size, GTK_UNIT_MM),
						GTK_UNIT_MM);
	}

	if (priv->metadata &&
	    pps_metadata_get_double (priv->metadata, "page-setup-margin-right", &double_value)) {
		gtk_page_setup_set_right_margin (page_setup, double_value, GTK_UNIT_MM);
	} else {
		gtk_page_setup_set_right_margin (page_setup,
						 gtk_paper_size_get_default_right_margin (paper_size, GTK_UNIT_MM),
						 GTK_UNIT_MM);
	}
}

static GtkPrintSettings *
get_print_settings (GKeyFile *key_file)
{
	GtkPrintSettings *print_settings;

	print_settings = g_key_file_has_group (key_file, PPS_PRINT_SETTINGS_GROUP) ?
		gtk_print_settings_new_from_key_file (key_file, PPS_PRINT_SETTINGS_GROUP, NULL) :
		gtk_print_settings_new ();

	return print_settings ? print_settings : gtk_print_settings_new ();
}

static GtkPageSetup *
get_print_page_setup (GKeyFile *key_file)
{
	GtkPageSetup *page_setup;

	page_setup = g_key_file_has_group (key_file, PPS_PAGE_SETUP_GROUP) ?
		gtk_page_setup_new_from_key_file (key_file, PPS_PAGE_SETUP_GROUP, NULL) :
		gtk_page_setup_new ();

	return page_setup ? page_setup : gtk_page_setup_new ();
}

static void
pps_window_print_cancel (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsPrintOperation *op;

	if (!priv->print_queue)
		return;

	while ((op = g_queue_peek_tail (priv->print_queue))) {
		pps_print_operation_cancel (op);
	}
}

static void
pps_window_print_update_pending_jobs_message (PpsWindow *pps_window,
					     gint      n_jobs)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gchar *text = NULL;

	if (!PPS_IS_PROGRESS_MESSAGE_AREA (priv->message_area) ||
	    !priv->print_queue)
		return;

	if (n_jobs == 0) {
		pps_window_set_message_area (pps_window, NULL);
		return;
	}

	if (n_jobs > 1) {
		text = g_strdup_printf (ngettext ("%d pending job in queue",
						  "%d pending jobs in queue",
						  n_jobs - 1), n_jobs - 1);
	}

	pps_message_area_set_secondary_text (PPS_MESSAGE_AREA (priv->message_area),
					    text);
	g_free (text);
}

static void
pps_window_print_operation_done (PpsPrintOperation       *op,
				GtkPrintOperationResult result,
				PpsWindow               *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gint n_jobs;

	switch (result) {
	case GTK_PRINT_OPERATION_RESULT_APPLY: {
		GtkPrintSettings *print_settings;

		print_settings = pps_print_operation_get_print_settings (op);
		pps_window_save_print_settings (pps_window, print_settings);

		if (pps_print_operation_get_embed_page_setup (op)) {
			GtkPageSetup *page_setup;

			page_setup = pps_print_operation_get_default_page_setup (op);
			pps_window_save_print_page_setup (pps_window, page_setup);
		}
	}

		break;
	case GTK_PRINT_OPERATION_RESULT_ERROR: {
		GtkWidget *dialog;
		GError    *error = NULL;


		pps_print_operation_get_error (op, &error);

		/* The message area is already used by
		 * the printing progress, so it's better to
		 * use a popup dialog in this case
		 */
		dialog = gtk_message_dialog_new (GTK_WINDOW (pps_window),
						 GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_ERROR,
						 GTK_BUTTONS_CLOSE,
						 "%s", _("Failed to print document"));
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
							  "%s", error->message);
		g_signal_connect (dialog, "response",
				  G_CALLBACK (gtk_window_destroy),
				  NULL);
		gtk_widget_set_visible (dialog, TRUE);

		g_error_free (error);
	}
		break;
	case GTK_PRINT_OPERATION_RESULT_CANCEL:
	default:
		break;
	}

	g_queue_remove (priv->print_queue, op);
	g_object_unref (op);
	n_jobs = g_queue_get_length (priv->print_queue);
	pps_window_print_update_pending_jobs_message (pps_window, n_jobs);

	if (n_jobs == 0 && priv->close_after_print)
		g_idle_add_once ((GSourceOnceFunc)gtk_window_destroy,
				 pps_window);
}

static void
pps_window_print_progress_response_cb (PpsProgressMessageArea *area,
				      gint                   response,
				      PpsWindow              *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (response == GTK_RESPONSE_CANCEL) {
		PpsPrintOperation *op;

		op = g_queue_peek_tail (priv->print_queue);
		pps_print_operation_cancel (op);
	} else {
		gtk_widget_set_visible (GTK_WIDGET (area), FALSE);
	}
}

static void
pps_window_print_operation_status_changed (PpsPrintOperation *op,
					  PpsWindow         *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	const gchar *status;
	gdouble      fraction;

	status = pps_print_operation_get_status (op);
	fraction = pps_print_operation_get_progress (op);

	if (!priv->message_area) {
		GtkWidget   *area;
		const gchar *job_name;
		gchar       *text;

		job_name = pps_print_operation_get_job_name (op);
		text = g_strdup_printf (_("Printing job “%s”"), job_name);

		area = pps_progress_message_area_new ("document-print-symbolic",
						     text,
						     _("C_ancel"),
						     GTK_RESPONSE_CANCEL,
						     NULL);
		pps_window_print_update_pending_jobs_message (pps_window, 1);
		g_signal_connect (pps_message_area_get_info_bar (PPS_MESSAGE_AREA (area)), "response",
				  G_CALLBACK (pps_window_print_progress_response_cb),
				  pps_window);

		gtk_widget_set_visible (area, TRUE);
		pps_window_set_message_area (pps_window, area);
		g_free (text);
	}

	pps_progress_message_area_set_status (PPS_PROGRESS_MESSAGE_AREA (priv->message_area),
					     status);
	pps_progress_message_area_set_fraction (PPS_PROGRESS_MESSAGE_AREA (priv->message_area),
					       fraction);
}

static void
pps_window_print_operation_begin_print (PpsPrintOperation *op,
				       PpsWindow         *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (!priv->print_queue)
		priv->print_queue = g_queue_new ();

	g_queue_push_head (priv->print_queue, op);
	pps_window_print_update_pending_jobs_message (pps_window,
						     g_queue_get_length (priv->print_queue));
}

void
pps_window_print_range (PpsWindow *pps_window,
		       gint      first_page,
		       gint      last_page)
{
	PpsPrintOperation *op;
	GKeyFile         *print_settings_file;
	GtkPrintSettings *print_settings;
	GtkPageSetup     *print_page_setup;
	gint              current_page;
	gint              document_last_page;
	gboolean          embed_page_setup;
	gchar            *output_basename;
	gchar            *dot;
	PpsWindowPrivate *priv;

	g_return_if_fail (PPS_IS_WINDOW (pps_window));
	priv = GET_PRIVATE (pps_window);
	g_return_if_fail (priv->document != NULL);

	if (!priv->print_queue)
		priv->print_queue = g_queue_new ();

	op = pps_print_operation_new (priv->document);
	if (!op) {
		g_warning ("%s", "Printing is not supported for document\n");
		return;
	}

	g_signal_connect (op, "begin_print",
			  G_CALLBACK (pps_window_print_operation_begin_print),
			  (gpointer)pps_window);
	g_signal_connect (op, "status_changed",
			  G_CALLBACK (pps_window_print_operation_status_changed),
			  (gpointer)pps_window);
	g_signal_connect (op, "done",
			  G_CALLBACK (pps_window_print_operation_done),
			  (gpointer)pps_window);

	current_page = pps_document_model_get_page (priv->model);
	document_last_page = pps_document_get_n_pages (priv->document);

	print_settings_file = get_print_settings_file ();

	print_settings = get_print_settings (print_settings_file);
	pps_window_load_print_settings_from_metadata (pps_window, print_settings);

	print_page_setup = get_print_page_setup (print_settings_file);
	pps_window_load_print_page_setup_from_metadata (pps_window, print_page_setup);

	if (first_page != 1 || last_page != document_last_page) {
		GtkPageRange range;

		/* Ranges in GtkPrint are 0 - N */
		range.start = first_page - 1;
		range.end = last_page - 1;

		gtk_print_settings_set_print_pages (print_settings,
						    GTK_PRINT_PAGES_RANGES);
		gtk_print_settings_set_page_ranges (print_settings,
						    &range, 1);
	}

	output_basename = g_strdup (priv->edit_name);
	dot = g_strrstr (output_basename, ".");
	if (dot)
		dot[0] = '\0';

	/* Set output basename for printing to file */
	gtk_print_settings_set (print_settings,
			        GTK_PRINT_SETTINGS_OUTPUT_BASENAME,
			        output_basename);
	g_free (output_basename);

	pps_print_operation_set_job_name (op, gtk_window_get_title (GTK_WINDOW (pps_window)));
	pps_print_operation_set_current_page (op, current_page);
	pps_print_operation_set_print_settings (op, print_settings);
	pps_print_operation_set_default_page_setup (op, print_page_setup);
	embed_page_setup = priv->lockdown_settings ?
		!g_settings_get_boolean (priv->lockdown_settings,
					 GS_LOCKDOWN_PRINT_SETUP) :
		TRUE;
	pps_print_operation_set_embed_page_setup (op, embed_page_setup);

	g_object_unref (print_settings);
	g_object_unref (print_page_setup);
	g_key_file_free (print_settings_file);

	pps_print_operation_run (op, GTK_WINDOW (pps_window));
}

static void
pps_window_print (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_window_print_range (window, 1,
			       pps_document_get_n_pages (priv->document));
}

static void
pps_window_cmd_file_print (GSimpleAction *action,
			  GVariant      *state,
			  gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	gtk_menu_button_popdown (GTK_MENU_BUTTON (priv->action_menu_button));
	pps_window_print (pps_window);
}

static void
pps_window_cmd_file_properties (GSimpleAction *action,
			       GVariant      *state,
			       gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (priv->properties == NULL) {
		priv->properties = g_object_new (g_type_from_name ("PpsPropertiesWindow"),
						 "document", priv->document, NULL);
		g_object_add_weak_pointer (G_OBJECT (priv->properties),
					   (gpointer) &(priv->properties));
		gtk_window_set_transient_for (GTK_WINDOW (priv->properties),
					      GTK_WINDOW (pps_window));
	}

	gtk_window_present (GTK_WINDOW (priv->properties));
}

static void
document_modified_reload_dialog_response (GtkDialog *dialog,
					  gint	     response,
					  PpsWindow  *pps_window)
{
	gtk_window_destroy (GTK_WINDOW (dialog));

	if (response == GTK_RESPONSE_YES)
	        pps_window_reload_document (pps_window, NULL);
}

static void
document_modified_confirmation_dialog_response (GtkDialog *dialog,
						gint       response,
						PpsWindow  *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gtk_window_destroy (GTK_WINDOW (dialog));

	switch (response) {
	case GTK_RESPONSE_YES:
		priv->close_after_save = TRUE;
		pps_window_save_as (pps_window);
		break;
	case GTK_RESPONSE_NO:
		gtk_window_destroy (GTK_WINDOW (pps_window));
		break;
	case GTK_RESPONSE_CANCEL:
	default:
		priv->close_after_save = FALSE;
	}
}

static gboolean
pps_window_check_document_modified (PpsWindow      *pps_window,
				   PpsWindowAction command)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsDocument  *document = priv->document;
	GtkWidget   *dialog;
	gchar       *text, *markup;
	const gchar *secondary_text, *secondary_text_command;

	if (!document)
		return FALSE;

	if (PPS_IS_DOCUMENT_FORMS (document) &&
	    pps_document_forms_document_is_modified (PPS_DOCUMENT_FORMS (document))) {
		secondary_text = _("Document contains form fields that have been filled out.");
	} else if (PPS_IS_DOCUMENT_ANNOTATIONS (document) &&
		   pps_document_annotations_document_is_modified (PPS_DOCUMENT_ANNOTATIONS (document))) {
		secondary_text = _("Document contains new or modified annotations.");
	} else {
		return FALSE;
	}

	dialog = gtk_message_dialog_new (GTK_WINDOW (pps_window),
					 GTK_DIALOG_MODAL,
					 GTK_MESSAGE_QUESTION,
					 GTK_BUTTONS_NONE,
					 NULL);

	if (command == PPS_WINDOW_ACTION_RELOAD) {
		text = g_markup_printf_escaped (_("Reload document “%s”?"),
						gtk_window_get_title (GTK_WINDOW (pps_window)));
		secondary_text_command = _("If you reload the document, changes will be permanently lost.");
		gtk_dialog_add_buttons (GTK_DIALOG (dialog),
					_("_No"),
					GTK_RESPONSE_NO,
					_("_Reload"),
					GTK_RESPONSE_YES,
					NULL);
		g_signal_connect (dialog, "response",
				  G_CALLBACK (document_modified_reload_dialog_response),
				  pps_window);
	} else {
		text = g_markup_printf_escaped (_("Save a copy of document “%s” before closing?"),
                                                gtk_window_get_title (GTK_WINDOW (pps_window)));
		secondary_text_command = _("If you don’t save a copy, changes will be permanently lost.");
		gtk_dialog_add_buttons (GTK_DIALOG (dialog),
				_("Close _without Saving"),
				GTK_RESPONSE_NO,
				_("C_ancel"),
				GTK_RESPONSE_CANCEL,
				_("Save a _Copy"),
				GTK_RESPONSE_YES,
				NULL);
		g_signal_connect (dialog, "response",
			  G_CALLBACK (document_modified_confirmation_dialog_response),
			  pps_window);

	}
	markup = g_strdup_printf ("<b>%s</b>", text);
	g_free (text);

	gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), markup);
	g_free (markup);

	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
						  "%s %s", secondary_text, secondary_text_command);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);

	gtk_widget_set_visible (dialog, TRUE);

	return TRUE;
}

static void
print_jobs_confirmation_dialog_response (GtkDialog *dialog,
					 gint       response,
					 PpsWindow  *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	gtk_window_destroy (GTK_WINDOW (dialog));

	switch (response) {
	case GTK_RESPONSE_YES:
		if (!priv->print_queue ||
		    g_queue_is_empty (priv->print_queue))
			gtk_window_destroy (GTK_WINDOW (pps_window));
		else
			priv->close_after_print = TRUE;
		break;
	case GTK_RESPONSE_NO:
		priv->close_after_print = TRUE;
		if (priv->print_queue &&
		    !g_queue_is_empty (priv->print_queue)) {
			gtk_widget_set_sensitive (GTK_WIDGET (pps_window), FALSE);
			pps_window_print_cancel (pps_window);
		} else {
			gtk_window_destroy (GTK_WINDOW (pps_window));
		}
		break;
	case GTK_RESPONSE_CANCEL:
	default:
		priv->close_after_print = FALSE;
	}
}

static gboolean
pps_window_check_print_queue (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GtkWidget *dialog;
	gchar     *text, *markup;
	gint       n_print_jobs;

	n_print_jobs = priv->print_queue ?
		g_queue_get_length (priv->print_queue) : 0;

	if (n_print_jobs == 0)
		return FALSE;

	dialog = gtk_message_dialog_new (GTK_WINDOW (pps_window),
					 GTK_DIALOG_MODAL,
					 GTK_MESSAGE_QUESTION,
					 GTK_BUTTONS_NONE,
					 NULL);
	if (n_print_jobs == 1) {
		PpsPrintOperation *op;
		const gchar      *job_name;

		op = g_queue_peek_tail (priv->print_queue);
		job_name = pps_print_operation_get_job_name (op);

		text = g_strdup_printf (_("Wait until print job “%s” finishes before closing?"),
					job_name);
	} else {
		/* TRANS: the singular form is not really used as n_print_jobs > 1
 			  but some languages distinguish between different plurals forms,
			  so the ngettext is needed. */
		text = g_strdup_printf (ngettext("There is %d print job active. "
						 "Wait until print finishes before closing?",
						 "There are %d print jobs active. "
						 "Wait until print finishes before closing?",
						 n_print_jobs),
					n_print_jobs);
	}

	markup = g_strdup_printf ("<b>%s</b>", text);
	g_free (text);

	gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), markup);
	g_free (markup);

	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s",
						  _("If you close the window, pending print "
						    "jobs will not be printed."));

	gtk_dialog_add_buttons (GTK_DIALOG (dialog),
				_("Cancel _print and Close"),
				GTK_RESPONSE_NO,
				_("_Cancel"),
				GTK_RESPONSE_CANCEL,
				_("Close _after Printing"),
				GTK_RESPONSE_YES,
				NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);

	g_signal_connect (dialog, "response",
			  G_CALLBACK (print_jobs_confirmation_dialog_response),
			  pps_window);
	gtk_widget_set_visible (dialog, TRUE);

	return TRUE;
}

static void
pps_window_save_settings (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsView          *pps_view = PPS_VIEW (priv->view);
	PpsDocumentModel *model = priv->model;
	GSettings       *settings = priv->default_settings;
	PpsSizingMode     sizing_mode;

	g_settings_set_boolean (settings, "continuous",
				pps_document_model_get_continuous (model));
	g_settings_set_boolean (settings, "dual-page",
		                pps_document_model_get_page_layout (model) == PPS_PAGE_LAYOUT_DUAL);
	g_settings_set_boolean (settings, "dual-page-odd-left",
				pps_document_model_get_dual_page_odd_pages_left (model));
	g_settings_set_boolean (settings, "fullscreen",
				gtk_window_is_fullscreen (GTK_WINDOW (pps_window)) &&
				!PPS_WINDOW_IS_PRESENTATION (priv));
	g_settings_set_boolean (settings, "inverted-colors",
				pps_document_model_get_inverted_colors (model));
	sizing_mode = pps_document_model_get_sizing_mode (model);
	g_settings_set_enum (settings, "sizing-mode", sizing_mode);
	if (sizing_mode == PPS_SIZING_FREE) {
		gdouble zoom = pps_document_model_get_scale (model);

		zoom *= 72.0 / pps_document_misc_get_widget_dpi (GTK_WIDGET (pps_window));
		g_settings_set_double (settings, "zoom", zoom);
	}
	g_settings_set_boolean (settings, "show-sidebar",
				gtk_widget_get_visible (priv->sidebar));
	g_settings_set_string (settings, "sidebar-page",
				pps_sidebar_get_visible_child_name (PPS_SIDEBAR (priv->sidebar)));
	g_settings_set_boolean (settings, "enable-spellchecking",
				pps_view_get_enable_spellchecking (pps_view));
	g_settings_apply (settings);
}

static gboolean
pps_window_close_handled (GtkWindow *self)
{
	PpsWindow *pps_window = PPS_WINDOW (self);
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (PPS_WINDOW_IS_PRESENTATION (priv)) {
		gint current_page;

		/* Save current page */
		current_page = pps_view_presentation_get_current_page (
			PPS_VIEW_PRESENTATION (priv->presentation_view));

		pps_document_model_set_page (priv->model, current_page);
	}

	g_clear_signal_handler (&priv->modified_handler_id, priv->document);

	if (pps_window_check_document_modified (pps_window, PPS_WINDOW_ACTION_CLOSE))
		return TRUE;

	if (pps_window_check_print_queue (pps_window))
		return TRUE;

	if (!pps_window_is_start_view (pps_window))
		pps_window_save_settings (pps_window);

	return FALSE;
}

static void
pps_window_cmd_file_close_window (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	GtkWindow *window = user_data;

	gtk_window_close (window);
}

static void
pps_window_focus_page_selector (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	g_return_if_fail (PPS_IS_WINDOW (window));

	gtk_widget_grab_focus (priv->page_selector);
}

static void
pps_window_cmd_focus_page_selector (GSimpleAction *action,
				   GVariant      *parameter,
				   gpointer       user_data)
{
	pps_window_focus_page_selector (PPS_WINDOW (user_data));
}

static void
pps_window_cmd_continuous (GSimpleAction *action,
			  GVariant      *state,
			  gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_window_stop_presentation (window, TRUE);
	pps_document_model_set_continuous (priv->model, g_variant_get_boolean (state));
	g_simple_action_set_state (action, state);
}


static void
pps_window_cmd_dual (GSimpleAction *action,
		    GVariant      *state,
		    gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	PpsDocument *document = priv->document;
	gboolean has_pages = FALSE;
	gboolean dual_page;
	gboolean start_view_mode;

	dual_page = g_variant_get_boolean (state);

	pps_window_stop_presentation (window, TRUE);
	pps_document_model_set_page_layout (priv->model,
			dual_page ? PPS_PAGE_LAYOUT_DUAL : PPS_PAGE_LAYOUT_SINGLE);
	g_simple_action_set_state (action, state);

	start_view_mode = pps_window_is_start_view (window);

	if (document)
		has_pages = pps_document_get_n_pages (priv->document) > 0;

	pps_window_set_action_enabled (window, "dual-odd-left", dual_page &&
				      has_pages && !start_view_mode);
}

static void
pps_window_cmd_dual_odd_pages_left (GSimpleAction *action,
				   GVariant      *state,
				   gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_document_model_set_dual_page_odd_pages_left (priv->model,
							g_variant_get_boolean (state));
	g_simple_action_set_state (action, state);
}

static void
pps_window_cmd_rtl (GSimpleAction *action,
                   GVariant      *state,
                   gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_document_model_set_rtl (priv->model,
	                           g_variant_get_boolean (state));
	g_simple_action_set_state (action, state);
}

static void
pps_window_change_sizing_mode_action_state (GSimpleAction *action,
					   GVariant      *state,
					   gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	const gchar *mode;

	mode = g_variant_get_string (state, NULL);

	if (g_str_equal (mode, "fit-page"))
		pps_document_model_set_sizing_mode (priv->model, PPS_SIZING_FIT_PAGE);
	else if (g_str_equal (mode, "fit-width"))
		pps_document_model_set_sizing_mode (priv->model, PPS_SIZING_FIT_WIDTH);
	else if (g_str_equal (mode, "automatic"))
		pps_document_model_set_sizing_mode (priv->model, PPS_SIZING_AUTOMATIC);
	else if (g_str_equal (mode, "free"))
		pps_document_model_set_sizing_mode (priv->model, PPS_SIZING_FREE);
	else
		g_assert_not_reached ();

	g_simple_action_set_state (action, state);
}

static void
pps_window_cmd_view_zoom (GSimpleAction *action,
			 GVariant      *parameter,
			 gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gdouble zoom = g_variant_get_double (parameter);

	pps_document_model_set_sizing_mode (priv->model, PPS_SIZING_FREE);
	pps_document_model_set_scale (priv->model,
				     zoom * pps_document_misc_get_widget_dpi (GTK_WIDGET (pps_window)) / 72.0);
}

static void
pps_window_cmd_edit_select_all (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_view_select_all (PPS_VIEW (priv->view));
}

static void
pps_window_cmd_toggle_find (GSimpleAction *action,
			   GVariant      *state,
			   gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean show = g_variant_get_boolean (state);

	if (show)
		pps_window_show_find_bar (pps_window);
	else
		pps_window_close_find_bar (pps_window);
	pps_view_find_set_highlight_search (PPS_VIEW (priv->view), show);

	g_simple_action_set_state (action, state);
}

static void
pps_window_find_restart (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gint page;

	page = pps_document_model_get_page (priv->model);
	pps_view_find_restart (PPS_VIEW (priv->view), page);
	pps_find_sidebar_restart (PPS_FIND_SIDEBAR (priv->find_sidebar), page);
}

static void
pps_window_cmd_find (GSimpleAction *action,
		    GVariant      *parameter,
		    gpointer       user_data)
{
	PpsView *view;
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	g_autofree gchar *selected_text = NULL;

	view = PPS_VIEW (priv->view);
        selected_text = pps_view_get_selected_text (view);

        if (selected_text != NULL && g_strcmp0(selected_text, "") != 0) {
		GtkSearchEntry *entry = pps_search_box_get_entry (PPS_SEARCH_BOX (priv->search_box));
		gtk_editable_set_text (GTK_EDITABLE (entry), selected_text);
		gtk_editable_select_region (GTK_EDITABLE (entry), 0, -1);
		pps_window_show_find_bar (pps_window);
		pps_window_find_restart (pps_window);
	} else {
		gtk_widget_activate_action (GTK_WIDGET (pps_window), "doc.toggle-find", NULL);
	}
}

static void
pps_window_cmd_find_next (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_view_find_next (PPS_VIEW (priv->view));
	pps_find_sidebar_next (PPS_FIND_SIDEBAR (priv->find_sidebar));
}

static void
pps_window_cmd_find_previous (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_view_find_previous (PPS_VIEW (priv->view));
	pps_find_sidebar_previous (PPS_FIND_SIDEBAR (priv->find_sidebar));
}

static void
pps_window_cmd_edit_copy (GSimpleAction *action,
			 GVariant      *parameter,
			 gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_view_copy (PPS_VIEW (priv->view));
}

static void
pps_window_update_fullscreen_action (PpsWindow *window,
				    gboolean  fullscreen)
{
	GAction *action;

	action = g_action_map_lookup_action (G_ACTION_MAP (window), "fullscreen");
	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (fullscreen));
}

static void
pps_window_run_fullscreen (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	gboolean fullscreen_window = TRUE;
	gboolean maximized = FALSE;

	if (PPS_WINDOW_IS_PRESENTATION (priv)) {
		pps_window_stop_presentation (window, FALSE);
		fullscreen_window = FALSE;
	}

	if (fullscreen_window && gtk_window_is_fullscreen (GTK_WINDOW (window)))
		return;

	pps_window_update_fullscreen_action (window, TRUE);

	adw_header_bar_set_show_end_title_buttons (ADW_HEADER_BAR (priv->header_bar), FALSE);


	if (fullscreen_window)
		gtk_window_fullscreen (GTK_WINDOW (window));
	gtk_widget_grab_focus (priv->view);

	if (priv->metadata && !pps_window_is_empty (window)) {
		pps_metadata_get_boolean (priv->metadata, "window-maximized", &maximized);
		if (!maximized)
			pps_metadata_set_boolean (priv->metadata, "fullscreen", TRUE);
	}
}

static void
pps_window_stop_fullscreen (PpsWindow *window,
			   gboolean  unfullscreen_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!gtk_window_is_fullscreen (GTK_WINDOW (window)))
		return;

	pps_window_update_fullscreen_action (window, FALSE);

	adw_header_bar_set_show_end_title_buttons (ADW_HEADER_BAR (priv->header_bar), TRUE);

	if (unfullscreen_window)
		gtk_window_unfullscreen (GTK_WINDOW (window));

	if (priv->metadata && !pps_window_is_empty (window))
		pps_metadata_set_boolean (priv->metadata, "fullscreen", FALSE);
}

static void
pps_window_cmd_view_fullscreen (GSimpleAction *action,
			       GVariant      *state,
			       gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	gtk_menu_button_popdown (GTK_MENU_BUTTON (priv->action_menu_button));

	if (g_variant_get_boolean (state)) {
		pps_window_run_fullscreen (window);
	} else {
		pps_window_stop_fullscreen (window, TRUE);
	}

	g_simple_action_set_state (action, state);
}

static void
pps_window_inhibit_screensaver (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

        if (priv->presentation_mode_inhibit_id != 0)
                return;

        priv->presentation_mode_inhibit_id =
                gtk_application_inhibit (GTK_APPLICATION (g_application_get_default ()),
                                         GTK_WINDOW (window),
                                         GTK_APPLICATION_INHIBIT_IDLE,
                                         _("Running in presentation mode"));
}


static void
pps_window_uninhibit_screensaver (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

        if (priv->presentation_mode_inhibit_id == 0)
                return;

        gtk_application_uninhibit (GTK_APPLICATION (g_application_get_default ()),
                                   priv->presentation_mode_inhibit_id);
        priv->presentation_mode_inhibit_id = 0;
}

static void
pps_window_view_presentation_finished (PpsWindow *window)
{
	pps_window_stop_presentation (window, TRUE);
}

static void
pps_window_run_presentation (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GAction  *action;
	GVariant *annot_state;
	guint     current_page;
	guint     rotation;
	gboolean  inverted_colors;
	GtkEventController *controller;

	if (PPS_WINDOW_IS_PRESENTATION (priv))
		return;

	pps_window_close_find_bar (window);

	/* We do not want to show the annotation toolbar during
	 * the presentation mode. But we would like to restore it
	 * afterwards
	 */
	action = g_action_map_lookup_action (G_ACTION_MAP (window),
					     "toggle-edit-annots");
	annot_state = g_variant_new_boolean (FALSE);
	pps_window_cmd_toggle_edit_annots (G_SIMPLE_ACTION (action),
					  annot_state,
					  window);

	if (gtk_window_is_fullscreen (GTK_WINDOW (window)))
		pps_window_stop_fullscreen (window, FALSE);

	current_page = pps_document_model_get_page (priv->model);
	rotation = pps_document_model_get_rotation (priv->model);
	inverted_colors = pps_document_model_get_inverted_colors (priv->model);
	priv->presentation_view = GTK_WIDGET (pps_view_presentation_new (priv->document,
									current_page,
									rotation,
									inverted_colors));

	adw_view_stack_add (ADW_VIEW_STACK (priv->stack), priv->presentation_view);

	g_signal_connect_swapped (priv->presentation_view, "finished",
				  G_CALLBACK (pps_window_view_presentation_finished),
				  window);
	g_signal_connect_swapped (priv->presentation_view, "external-link",
				  G_CALLBACK (view_external_link_cb),
				  window);
	controller = GTK_EVENT_CONTROLLER (gtk_event_controller_focus_new ());
	g_signal_connect_swapped (controller, "enter",
				  G_CALLBACK (pps_window_inhibit_screensaver),
				  window);
	g_signal_connect_swapped (controller, "leave",
				  G_CALLBACK (pps_window_uninhibit_screensaver),
				  window);
	gtk_widget_add_controller (priv->presentation_view, controller);

	gtk_widget_set_hexpand (GTK_WIDGET (priv->presentation_view), TRUE);
	gtk_widget_set_vexpand (GTK_WIDGET (priv->presentation_view), TRUE);

	pps_window_set_mode (window, PPS_WINDOW_MODE_PRESENTATION);

	gtk_widget_grab_focus (priv->presentation_view);
	gtk_window_fullscreen (GTK_WINDOW (window));

        pps_window_inhibit_screensaver (window);

	if (priv->metadata && !pps_window_is_empty (window))
		pps_metadata_set_boolean (priv->metadata, "presentation", TRUE);
}

static void
pps_window_stop_presentation (PpsWindow *window,
			     gboolean  unfullscreen_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	guint current_page;
	guint rotation;

	if (!PPS_WINDOW_IS_PRESENTATION (priv))
		return;

	current_page = pps_view_presentation_get_current_page (PPS_VIEW_PRESENTATION (priv->presentation_view));
	pps_document_model_set_page (priv->model, current_page);
	rotation = pps_view_presentation_get_rotation (PPS_VIEW_PRESENTATION (priv->presentation_view));
	pps_document_model_set_rotation (priv->model, rotation);

	pps_window_set_mode (window, PPS_WINDOW_MODE_NORMAL);
	adw_view_stack_remove (ADW_VIEW_STACK (priv->stack), priv->presentation_view);
	priv->presentation_view = NULL;

	if (unfullscreen_window)
		gtk_window_unfullscreen (GTK_WINDOW (window));

	gtk_widget_grab_focus (priv->view);

        pps_window_uninhibit_screensaver (window);

	if (priv->metadata && !pps_window_is_empty (window))
		pps_metadata_set_boolean (priv->metadata, "presentation", FALSE);
}

static void
pps_window_cmd_view_presentation (GSimpleAction *action,
				 GVariant      *state,
				 gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!PPS_WINDOW_IS_PRESENTATION (priv))
		pps_window_run_presentation (window);
	/* We don't exit presentation when action is toggled because it conflicts with some
	 * remote controls. The behaviour is also consistent with libreoffice and other
	 * presentation tools. See https://bugzilla.gnome.org/show_bug.cgi?id=556162
	 */
}

static void
pps_window_set_mode (PpsWindow         *window,
		    PpsWindowRunMode   mode)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (priv->window_mode == mode)
		return;

	priv->window_mode = mode;

	switch (mode) {
		case PPS_WINDOW_MODE_NORMAL:
			adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->stack), "document");
			break;
		case PPS_WINDOW_MODE_PASSWORD_VIEW:
			adw_view_stack_set_visible_child (ADW_VIEW_STACK (priv->stack), priv->password_view);
			break;
		case PPS_WINDOW_MODE_START_VIEW:
			adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->stack), "start");
			break;
		case PPS_WINDOW_MODE_ERROR_VIEW:
			adw_view_stack_set_visible_child_name (ADW_VIEW_STACK (priv->stack), "error");
			break;
		case PPS_WINDOW_MODE_PRESENTATION:
			g_return_if_fail (priv->presentation_view != NULL);
			adw_view_stack_set_visible_child (ADW_VIEW_STACK (priv->stack), priv->presentation_view);
			break;
	        default:
			g_assert_not_reached ();
	}

	pps_window_update_actions_sensitivity (window);
}

static void
pps_window_cmd_edit_rotate_left (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gint rotation;

	if (PPS_WINDOW_IS_PRESENTATION (priv)) {
		rotation = pps_view_presentation_get_rotation (PPS_VIEW_PRESENTATION (priv->presentation_view));
		pps_view_presentation_set_rotation (PPS_VIEW_PRESENTATION (priv->presentation_view),
						   rotation - 90);
	} else {
		rotation = pps_document_model_get_rotation (priv->model);

		pps_document_model_set_rotation (priv->model, rotation - 90);
	}
}

static void
pps_window_cmd_edit_rotate_right (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gint rotation;

	if (PPS_WINDOW_IS_PRESENTATION (priv)) {
		rotation = pps_view_presentation_get_rotation (PPS_VIEW_PRESENTATION (priv->presentation_view));
		pps_view_presentation_set_rotation (PPS_VIEW_PRESENTATION (priv->presentation_view),
						   rotation + 90);
	} else {
		rotation = pps_document_model_get_rotation (priv->model);

		pps_document_model_set_rotation (priv->model, rotation + 90);
	}
}

static void
pps_window_cmd_view_inverted_colors (GSimpleAction *action,
				    GVariant      *state,
				    gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_document_model_set_inverted_colors (priv->model,
					       g_variant_get_boolean (state));
	g_simple_action_set_state (action, state);
}

static void
pps_window_cmd_view_enable_spellchecking (GSimpleAction *action,
				    GVariant      *state,
				    gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_view_set_enable_spellchecking (PPS_VIEW (priv->view),
	g_variant_get_boolean (state));
	g_simple_action_set_state (action, state);
}

static void
pps_window_cmd_view_zoom_in (GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_document_model_set_sizing_mode (priv->model, PPS_SIZING_FREE);
	pps_view_zoom_in (PPS_VIEW (priv->view));
}

static void
pps_window_cmd_view_zoom_out (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_document_model_set_sizing_mode (priv->model, PPS_SIZING_FREE);
	pps_view_zoom_out (PPS_VIEW (priv->view));
}

static void
pps_window_cmd_go_back_history (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gint old_page = pps_document_model_get_page (priv->model);

	if (old_page >= 0)
		pps_history_add_page (priv->history, old_page);

	pps_history_go_back (priv->history);
}

static void
pps_window_cmd_go_forward_history (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_history_go_forward (priv->history);
}

static void
pps_window_cmd_go_previous_page (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_previous_page (PPS_VIEW (priv->view));
}

static void
pps_window_cmd_go_next_page (GSimpleAction *action,
					GVariant      *parameter,
					gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_next_page (PPS_VIEW (priv->view));
}

static void
pps_window_cmd_go_first_page (GSimpleAction *action,
					 GVariant      *parameter,
					 gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	gint old_page = pps_document_model_get_page (priv->model);
	gint new_page = 0;
	pps_document_model_set_page (priv->model, 0);
	if (old_page >= 0) {
		pps_history_add_page (priv->history, old_page);
		pps_history_add_page (priv->history, new_page);
	}
}

static void
pps_window_cmd_go_last_page (GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	gint old_page = pps_document_model_get_page (priv->model);
	gint new_page = pps_document_get_n_pages (priv->document) - 1;

	pps_document_model_set_page (priv->model,
				    pps_document_get_n_pages (priv->document) - 1);
	if (old_page >= 0 && new_page >= 0) {
		pps_history_add_page (priv->history, old_page);
		pps_history_add_page (priv->history, new_page);
	}

}

static void
pps_window_cmd_go_forward (GSimpleAction *action,
			  GVariant      *parameter,
			  gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	int n_pages, current_page;

	n_pages = pps_document_get_n_pages (priv->document);
	current_page = pps_document_model_get_page (priv->model);

	if (current_page + 10 < n_pages) {
		pps_document_model_set_page (priv->model, current_page + 10);
	}
}

static void
pps_window_cmd_go_backwards (GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	int current_page;

	current_page = pps_document_model_get_page (priv->model);

	if (current_page - 10 >= 0) {
		pps_document_model_set_page (priv->model, current_page - 10);
	}
}

static void
pps_window_cmd_bookmarks_add (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	PpsBookmark bm;
	gchar     *page_label;

	bm.page = pps_document_model_get_page (priv->model);
	page_label = pps_document_get_page_label (priv->document, bm.page);
	bm.title = g_strdup_printf (_("Page %s"), page_label);
	g_free (page_label);

	/* PpsBookmarks takes ownership of bookmark */
	pps_bookmarks_add (priv->bookmarks, &bm);
}

static void
pps_window_cmd_bookmarks_delete (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	PpsBookmark bm;

	bm.page = pps_document_model_get_page (priv->model);
	bm.title = NULL;

	pps_bookmarks_delete (priv->bookmarks, &bm);
}

static void
pps_window_cmd_view_reload (GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data)
{
	PpsWindow *pps_window = user_data;

	if (pps_window_check_document_modified (pps_window, PPS_WINDOW_ACTION_RELOAD))
		return;

	pps_window_reload_document (pps_window, NULL);
}

static void
pps_window_cmd_escape (GSimpleAction *action,
		      GVariant      *parameter,
		      gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (gtk_search_bar_get_search_mode (GTK_SEARCH_BAR (priv->search_bar)))
		pps_window_close_find_bar (window);
	else if (PPS_WINDOW_IS_PRESENTATION (priv))
		pps_window_stop_presentation (window, TRUE);
	else if (gtk_window_is_fullscreen (GTK_WINDOW (window)))
		pps_window_stop_fullscreen (window, TRUE);
	else {
		/* Cancel any annotation in progress and untoggle the
		 * toolbar button. */
		pps_window_cancel_add_annot (window);
		pps_annotations_toolbar_add_annot_finished (PPS_ANNOTATIONS_TOOLBAR (priv->annots_toolbar));
		gtk_widget_grab_focus (priv->view);
	}
}

static void
save_sizing_mode (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	PpsSizingMode mode;
	GEnumValue *enum_value;

	if (!priv->metadata || pps_window_is_empty (window))
		return;

	mode = pps_document_model_get_sizing_mode (priv->model);
	enum_value = g_enum_get_value (g_type_class_peek (PPS_TYPE_SIZING_MODE), mode);
	pps_metadata_set_string (priv->metadata, "sizing_mode",
				enum_value->value_nick);
}

static void
document_changed_cb (PpsDocumentModel *model,
			       GParamSpec      *pspec,
			       PpsWindow        *pps_window)
{
	pps_window_set_document (pps_window,
				pps_document_model_get_document (model));
}

static void
pps_window_document_modified_cb (PpsDocument *document,
                                GParamSpec *pspec,
                                PpsWindow   *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	AdwWindowTitle *window_title = ADW_WINDOW_TITLE (
		adw_header_bar_get_title_widget (ADW_HEADER_BAR (priv->header_bar)));
	const gchar *title = adw_window_title_get_title (window_title);
	gchar *new_title;

	if (priv->is_modified)
		return;

	priv->is_modified = TRUE;
	if (gtk_widget_get_direction (GTK_WIDGET (pps_window)) == GTK_TEXT_DIR_RTL)
		new_title = g_strconcat ("• ", title, NULL);
	else
		new_title = g_strconcat (title, " •", NULL);

	if (new_title) {
		adw_window_title_set_title (window_title, new_title);
		g_free (new_title);
	}
}


static void
sizing_mode_changed_cb (PpsDocumentModel *model,
				  GParamSpec      *pspec,
		 		  PpsWindow        *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsSizingMode sizing_mode = pps_document_model_get_sizing_mode (model);

	g_object_set (priv->scrolled_window,
		      "hscrollbar-policy",
		      sizing_mode == PPS_SIZING_FREE ?
		      GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER,
		      "vscrollbar-policy", GTK_POLICY_AUTOMATIC,
		      NULL);

	update_sizing_buttons (pps_window);
	save_sizing_mode (pps_window);
}


static void
zoom_changed_cb (PpsDocumentModel *model, GParamSpec *pspec, PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

        pps_window_update_actions_sensitivity (pps_window);

	if (!priv->metadata)
		return;

	if (pps_document_model_get_sizing_mode (model) == PPS_SIZING_FREE && !pps_window_is_empty (pps_window)) {
		gdouble zoom;

		zoom = pps_document_model_get_scale (model);
		zoom *= 72.0 / pps_document_misc_get_widget_dpi (GTK_WIDGET (pps_window));
		pps_metadata_set_double (priv->metadata, "zoom", zoom);
	}
}

static void
continuous_changed_cb (PpsDocumentModel *model,
				 GParamSpec      *pspec,
				 PpsWindow        *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean continuous;
	GAction *action;

	continuous = pps_document_model_get_continuous (model);

	action = g_action_map_lookup_action (G_ACTION_MAP (pps_window), "continuous");
	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (continuous));

	if (priv->metadata && !pps_window_is_empty (pps_window))
		pps_metadata_set_boolean (priv->metadata, "continuous", continuous);
}

static void
rotation_changed_cb (PpsDocumentModel *model,
			       GParamSpec      *pspec,
			       PpsWindow        *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	gint rotation = pps_document_model_get_rotation (model);

	if (priv->metadata && !pps_window_is_empty (window))
		pps_metadata_set_int (priv->metadata, "rotation",
				     rotation);
}

static void
inverted_colors_changed_cb (PpsDocumentModel *model,
			              GParamSpec      *pspec,
			              PpsWindow        *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	gboolean inverted_colors = pps_document_model_get_inverted_colors (model);
	GdkDisplay *display = gtk_widget_get_display (GTK_WIDGET (window));
	AdwStyleManager *manager = adw_style_manager_get_for_display (display);
	GAction *action;

	action = g_action_map_lookup_action (G_ACTION_MAP (window), "inverted-colors");
	g_simple_action_set_state (G_SIMPLE_ACTION (action),
				   g_variant_new_boolean (inverted_colors));

	if (inverted_colors)
		adw_style_manager_set_color_scheme (manager, ADW_COLOR_SCHEME_FORCE_DARK);
	else
		adw_style_manager_set_color_scheme (manager, ADW_COLOR_SCHEME_DEFAULT);

	if (priv->metadata && !pps_window_is_empty (window))
		pps_metadata_set_boolean (priv->metadata, "inverted-colors",
					 inverted_colors);
}

static void
page_layout_changed_cb (PpsDocumentModel *model,
			GParamSpec      *pspec,
			PpsWindow        *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean dual_page;
	GAction *action;

	dual_page = pps_document_model_get_page_layout (model) == PPS_PAGE_LAYOUT_DUAL;

	action = g_action_map_lookup_action (G_ACTION_MAP (pps_window), "dual-page");
	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (dual_page));

	if (priv->metadata && !pps_window_is_empty (pps_window))
		pps_metadata_set_boolean (priv->metadata, "dual-page", dual_page);
}

static void
dual_mode_odd_pages_left_changed_cb (PpsDocumentModel *model,
					       GParamSpec      *pspec,
					       PpsWindow        *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean odd_left;
	GAction *action;

	odd_left = pps_document_model_get_dual_page_odd_pages_left (model);

	action = g_action_map_lookup_action (G_ACTION_MAP (pps_window), "dual-odd-left");
	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (odd_left));

	if (priv->metadata && !pps_window_is_empty (pps_window))
		pps_metadata_set_boolean (priv->metadata, "dual-page-odd-left",
					 odd_left);
}

static void
direction_changed_cb (PpsDocumentModel *model,
                          GParamSpec      *pspec,
                          PpsWindow        *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean rtl;
	GAction *action;

	rtl = pps_document_model_get_rtl (model);

	action = g_action_map_lookup_action (G_ACTION_MAP (pps_window), "rtl");
	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (rtl));

	if (priv->metadata && !pps_window_is_empty (pps_window))
		pps_metadata_set_boolean (priv->metadata, "rtl",
					 rtl);
}

static void
pps_window_view_cmd_toggle_sidebar (GSimpleAction *action,
				   GVariant      *state,
				   gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean show_side_pane;

	if (PPS_WINDOW_IS_PRESENTATION (priv))
		return;

	show_side_pane = g_variant_get_boolean (state);
	g_simple_action_set_state (action, g_variant_new_boolean (show_side_pane));
	adw_overlay_split_view_set_show_sidebar (priv->split_view, show_side_pane);
}

static void
sidebar_current_page_changed_cb (PpsSidebar  *pps_sidebar,
					   GParamSpec *pspec,
					   PpsWindow   *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsSidebar *sidebar = PPS_SIDEBAR (priv->sidebar);

	if (priv->metadata && !pps_window_is_empty (pps_window)) {
		pps_metadata_set_string (priv->metadata,
					"sidebar_page",
					pps_sidebar_get_visible_child_name (sidebar));
	}
}

static void
sidebar_visibility_changed_cb (AdwOverlaySplitView *split_view,
                               GParamSpec          *pspec,
                               PpsWindow            *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (!PPS_WINDOW_IS_PRESENTATION (priv)) {
		gboolean visible = adw_overlay_split_view_get_show_sidebar (split_view);

		g_action_group_change_action_state (G_ACTION_GROUP (pps_window), "show-side-pane",
						    g_variant_new_boolean (visible));

		if (priv->metadata)
			pps_metadata_set_boolean (priv->metadata, "sidebar_visibility",
						 visible);
		if (!visible)
			gtk_widget_grab_focus (priv->view);
	}
}

static void
zoom_selector_activated (GtkWidget *zoom_action,
			 PpsWindow *window)
{
	pps_window_focus_view (window);
}

static void
find_button_sensitive_changed (GtkWidget  *find_button,
			       GParamSpec *pspec,
			       PpsWindow  *window)
{
        if (gtk_widget_is_sensitive (find_button)) {
                gtk_widget_set_tooltip_text (find_button,
                                             _("Search"));
		gtk_button_set_icon_name (GTK_BUTTON (find_button), "edit-find-symbolic");
	} else {
                gtk_widget_set_tooltip_text (find_button,
                                             _("Search Unavailable"));
		gtk_button_set_icon_name (GTK_BUTTON (find_button), "find-unsupported-symbolic");
	}
}

static void
view_menu_link_popup (PpsWindow *pps_window,
		      PpsLink   *link)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean  show_external = FALSE;
	gboolean  show_internal = FALSE;

	g_clear_object (&priv->link);
	if (link) {
		PpsLinkAction *pps_action;

		priv->link = g_object_ref (link);

		pps_action = pps_link_get_action (link);
		if (pps_action) {
			switch (pps_link_action_get_action_type (pps_action)) {
		                case PPS_LINK_ACTION_TYPE_GOTO_DEST:
		                case PPS_LINK_ACTION_TYPE_GOTO_REMOTE:
					show_internal = TRUE;
					break;
		                case PPS_LINK_ACTION_TYPE_EXTERNAL_URI:
		                case PPS_LINK_ACTION_TYPE_LAUNCH:
					show_external = TRUE;
					break;
		                default:
					break;
			}
		}
	}

	pps_window_set_action_enabled (pps_window, "open-link", show_external);
	pps_window_set_action_enabled (pps_window, "copy-link-address", show_external);
	pps_window_set_action_enabled (pps_window, "go-to-link", show_internal);

	pps_window_set_action_enabled (pps_window, "open-link-new-window", show_internal);
}

static void
view_menu_image_popup (PpsWindow  *pps_window,
		       PpsImage   *image)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean show_image = FALSE;

	g_clear_object (&priv->image);
	if (image) {
		priv->image = g_object_ref (image);
		show_image = TRUE;
	}

	pps_window_set_action_enabled (pps_window, "save-image", show_image);
	pps_window_set_action_enabled (pps_window, "copy-image", show_image);
}

static void
view_menu_annot_popup (PpsWindow     *pps_window,
		       PpsAnnotation *annot)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	gboolean show_annot_props = FALSE;
	gboolean show_attachment = FALSE;
	gboolean can_remove_annots = FALSE;

	g_clear_object (&priv->annot);
	if (annot) {
		priv->annot = g_object_ref (annot);

		show_annot_props = PPS_IS_ANNOTATION_MARKUP (annot);

		if (PPS_IS_ANNOTATION_ATTACHMENT (annot)) {
			PpsAttachment *attachment;

			attachment = pps_annotation_attachment_get_attachment (PPS_ANNOTATION_ATTACHMENT (annot));
			if (attachment) {
				show_attachment = TRUE;

				g_list_free_full (priv->attach_list,
						  g_object_unref);
				priv->attach_list = NULL;
				priv->attach_list =
					g_list_prepend (priv->attach_list,
							g_object_ref (attachment));
			}
		}
	}

	if (PPS_IS_DOCUMENT_ANNOTATIONS (priv->document))
		can_remove_annots = pps_document_annotations_can_remove_annotation (PPS_DOCUMENT_ANNOTATIONS (priv->document));

	pps_window_set_action_enabled (pps_window, "annot-properties", show_annot_props);
	pps_window_set_action_enabled (pps_window, "remove-annot", annot != NULL && can_remove_annots);
	pps_window_set_action_enabled (pps_window, "open-attachment", show_attachment);
	pps_window_set_action_enabled (pps_window, "save-attachment", show_attachment);
}

static void
view_popup_hide_cb (GtkWidget *popup,
		    PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsDocument *document = priv->document;
	gboolean can_annotate;

	can_annotate = PPS_IS_DOCUMENT_ANNOTATIONS (document) &&
		pps_document_annotations_can_add_annotation (PPS_DOCUMENT_ANNOTATIONS (document));

	pps_window_set_action_enabled (pps_window, "highlight-annotation", can_annotate);
}

static void
view_menu_popup_cb (PpsView   *view,
		    GList    *items,
		    double    x,
		    double    y,
		    PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	PpsDocument *document = priv->document;
	GList   *l;
	gboolean has_link = FALSE;
	gboolean has_image = FALSE;
	gboolean has_annot = FALSE;
	gboolean can_annotate;
	graphene_point_t window_point;

	for (l = items; l; l = g_list_next (l)) {
		if (PPS_IS_LINK (l->data)) {
			view_menu_link_popup (pps_window, PPS_LINK (l->data));
			has_link = TRUE;
		} else if (PPS_IS_IMAGE (l->data)) {
			view_menu_image_popup (pps_window, PPS_IMAGE (l->data));
			has_image = TRUE;
		} else if (PPS_IS_ANNOTATION (l->data)) {
			view_menu_annot_popup (pps_window, PPS_ANNOTATION (l->data));
			has_annot = TRUE;
		}
	}

	if (!has_link)
		view_menu_link_popup (pps_window, NULL);
	if (!has_image)
		view_menu_image_popup (pps_window, NULL);
	if (!has_annot)
		view_menu_annot_popup (pps_window, NULL);

	can_annotate = PPS_IS_DOCUMENT_ANNOTATIONS (document) &&
		pps_document_annotations_can_add_annotation (PPS_DOCUMENT_ANNOTATIONS (document)) &&
		!has_annot && pps_view_has_selection (view);

	pps_window_set_action_enabled (pps_window, "highlight-annotation", can_annotate);

	if (!gtk_widget_compute_point (GTK_WIDGET (view),
				      gtk_widget_get_parent (priv->view_popup),
				      &GRAPHENE_POINT_INIT(x, y), &window_point))
		g_warn_if_reached ();

	gtk_popover_set_pointing_to (GTK_POPOVER (priv->view_popup),
				&(const GdkRectangle) { window_point.x, window_point.y, 1, 1 });
	gtk_popover_popup (GTK_POPOVER (priv->view_popup));
}

static gboolean
attachment_bar_menu_popup_cb (GtkWidget        *attachbar,
			      graphene_point_t *point,
			      GList            *attach_list,
			      PpsWindow        *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	graphene_point_t new_point;

	g_assert (attach_list != NULL);

	pps_window_set_action_enabled (pps_window, "open-attachment", TRUE);

	pps_window_set_action_enabled (pps_window, "save-attachment", TRUE);

	g_list_free_full (priv->attach_list, g_object_unref);
	priv->attach_list = attach_list;

	if (!gtk_widget_compute_point (GTK_WIDGET (attachbar),
				       gtk_widget_get_parent (priv->attachment_popup),
				       point,
				       &new_point))
		return FALSE;

	gtk_popover_set_pointing_to (GTK_POPOVER (priv->attachment_popup),
				&(const GdkRectangle) { new_point.x, new_point.y, 1, 1 });
	gtk_popover_popup (GTK_POPOVER (priv->attachment_popup));

	return TRUE;
}

static gboolean
save_attachment_to_target_file (PpsAttachment *attachment,
                                GFile        *target_file,
                                gboolean      is_dir,
                                gboolean      is_native,
                                PpsWindow     *pps_window)
{
	GFile  *save_to = NULL;
	GError *error = NULL;

	if (is_native) {
		if (is_dir) {
			save_to = g_file_get_child (target_file,
                            /* FIXMEchpe: file name encoding! */
						    pps_attachment_get_name (attachment));
		} else {
			save_to = g_object_ref (target_file);
		}
	} else {
		save_to = pps_mkstemp_file ("saveattachment.XXXXXX", &error);
	}

        if (save_to)
                pps_attachment_save (attachment, save_to, &error);

	if (error) {
		pps_window_error_message (pps_window, error,
					 "%s", _("The attachment could not be saved."));
		g_error_free (error);
		g_object_unref (save_to);

		return FALSE;
	}

	if (!is_native) {
		GFile *dest_file;

		if (is_dir) {
			dest_file = g_file_get_child (target_file,
						      pps_attachment_get_name (attachment));
		} else {
			dest_file = g_object_ref (target_file);
		}

		pps_window_save_remote (pps_window, PPS_SAVE_ATTACHMENT,
				       save_to, dest_file);

		g_object_unref (dest_file);
	}

	g_object_unref (save_to);
	return TRUE;
}


static gboolean
attachment_bar_save_attachment_cb (GtkWidget     *attachbar,
                                   PpsAttachment *attachment,
                                   const char    *uri,
                                   PpsWindow     *pps_window)
{
	GFile    *target_file;
	gboolean  is_native;
	gboolean  success;

	target_file = g_file_new_for_uri (uri);
	is_native = g_file_is_native (target_file);

	success = save_attachment_to_target_file (attachment,
	                                          target_file,
	                                          FALSE,
	                                          is_native,
	                                          pps_window);

	g_object_unref (target_file);
	return success;
}

static void
find_sidebar_result_activated_cb (PpsFindSidebar *find_sidebar,
				  gint           page,
				  gint           result,
				  PpsWindow      *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_find_set_result (PPS_VIEW (priv->view), page, result);
}

static void
search_entry_stop_search_cb (GtkSearchEntry *entry,
			     PpsWindow       *pps_window)
{
	pps_window_close_find_bar (pps_window);
}

static void
search_started_cb (PpsSearchBox *search_box,
		   PpsJobFind   *job,
		   PpsWindow    *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (!priv->document || !PPS_IS_DOCUMENT_FIND (priv->document))
		return;

	pps_view_find_started (PPS_VIEW (priv->view), job);
	pps_find_sidebar_start (PPS_FIND_SIDEBAR (priv->find_sidebar), job);
}

static void
search_cleared_cb (PpsSearchBox *search_box,
		   PpsWindow    *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	pps_window_update_actions_sensitivity (pps_window);
	pps_find_sidebar_clear (PPS_FIND_SIDEBAR (priv->find_sidebar));

	pps_view_find_cancel (PPS_VIEW (priv->view));
	gtk_widget_queue_draw (GTK_WIDGET (priv->view));
}

void
pps_window_handle_annot_popup (PpsWindow     *pps_window,
			      PpsAnnotation *annot)
{
	view_menu_annot_popup (pps_window, annot);
}

static void
pps_window_show_find_bar (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (adw_overlay_split_view_get_sidebar (priv->split_view) == priv->find_sidebar) {
		gtk_widget_grab_focus (priv->search_box);
		return;
	}

	if (priv->document == NULL || !PPS_IS_DOCUMENT_FIND (priv->document)) {
		g_error ("Find action should be insensitive since document doesn't support find");
		return;
	}

	if (PPS_WINDOW_IS_PRESENTATION (priv))
		return;

	pps_history_freeze (priv->history);

	adw_overlay_split_view_set_sidebar (priv->split_view, priv->find_sidebar);

	gtk_search_bar_set_search_mode (GTK_SEARCH_BAR (priv->search_bar), TRUE);
	gtk_widget_grab_focus (priv->search_box);
	g_action_group_change_action_state (G_ACTION_GROUP (pps_window), "show-side-pane",
						g_variant_new_boolean (TRUE));
	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.find-next", TRUE);
	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.find-previous", TRUE);
}

static void
pps_window_close_find_bar (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (adw_overlay_split_view_get_sidebar (priv->split_view) != priv->find_sidebar)
		return;

	adw_overlay_split_view_set_sidebar (priv->split_view, priv->sidebar);

	gtk_search_bar_set_search_mode (GTK_SEARCH_BAR (priv->search_bar), FALSE);
	gtk_widget_grab_focus (priv->view);

	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.find-next", FALSE);
	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.find-previous", FALSE);

	pps_history_thaw (priv->history);
}

static gboolean
pps_window_drag_data_received (GtkDropTarget* self,
			      const GValue* value,
			      gdouble x,
			      gdouble y,
			      gpointer user_data)
{
	GdkFileList *file_list = g_value_get_boxed(value);
	GSList *list = gdk_file_list_get_files(file_list);
	GListStore *uri_list = g_list_store_new (G_TYPE_FILE);

	for (GSList *l = list; l != NULL; l = l->next)
	{
		GFile *file = l->data;

		g_list_store_append (uri_list, file);
	}

	pps_application_open_uri_list (PPS_APP, G_LIST_MODEL (uri_list));

	g_object_unref (uri_list);

	return TRUE;
}

static void
pps_window_set_caret_navigation_enabled (PpsWindow *window,
					gboolean enabled)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GAction *action;

	if (priv->metadata)
		pps_metadata_set_boolean (priv->metadata, "caret-navigation", enabled);

	pps_view_set_caret_navigation_enabled (PPS_VIEW (priv->view), enabled);

	action = g_action_map_lookup_action (G_ACTION_MAP (window), "caret-navigation");
	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (enabled));
}

static void
pps_window_caret_navigation_message_area_response_cb (PpsMessageArea *area,
						     gint           response_id,
						     PpsWindow      *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	/* Turn the caret navigation mode on */
	if (response_id == GTK_RESPONSE_YES)
		pps_window_set_caret_navigation_enabled (window, TRUE);

	/* Turn the confirmation dialog off if the user has requested not to show it again */
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->ask_caret_navigation_check))) {
		g_settings_set_boolean (pps_window_ensure_settings (window), "show-caret-navigation-message", FALSE);
		g_settings_apply (priv->settings);
	}

	priv->ask_caret_navigation_check = NULL;
	pps_window_set_message_area (window, NULL);
	gtk_widget_grab_focus (priv->view);
}

static void
pps_window_cmd_view_toggle_caret_navigation (GSimpleAction *action,
					    GVariant      *state,
					    gpointer       user_data)
{
	PpsWindow  *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GtkWidget *message_area;
	GtkWidget *box;
	GtkWidget *hbox;
	gboolean   enabled;

	/* Don't ask for user confirmation to turn the caret navigation off when it is active,
	 * or to turn it on when the confirmation dialog is not to be shown per settings */
	enabled = pps_view_is_caret_navigation_enabled (PPS_VIEW (priv->view));
	if (enabled || !g_settings_get_boolean (pps_window_ensure_settings (window), "show-caret-navigation-message")) {
		pps_window_set_caret_navigation_enabled (window, !enabled);
		return;
	}

	/* Ask for user confirmation to turn the caret navigation mode on */
	if (priv->message_area)
		return;

	message_area = pps_message_area_new (GTK_MESSAGE_QUESTION,
					    _("Enable caret navigation?"),
					    _("_Enable"), GTK_RESPONSE_YES,
					    NULL);
	pps_message_area_set_secondary_text (PPS_MESSAGE_AREA (message_area),
					    _("Pressing F7 turns the caret navigation on or off. "
					      "This feature places a moveable cursor in text pages, "
					      "allowing you to move around and select text with your keyboard. "
					      "Do you want to enable the caret navigation?"));

	priv->ask_caret_navigation_check = gtk_check_button_new_with_label (_("Don’t show this message again"));
	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_box_prepend (GTK_BOX (hbox), priv->ask_caret_navigation_check);

	box = _pps_message_area_get_main_box (PPS_MESSAGE_AREA (message_area));
	gtk_box_prepend (GTK_BOX (box), hbox);
	g_signal_connect (pps_message_area_get_info_bar (PPS_MESSAGE_AREA (message_area)), "response",
			  G_CALLBACK (pps_window_caret_navigation_message_area_response_cb),
			  window);

	gtk_widget_set_visible (message_area, TRUE);
	pps_window_set_message_area (window, message_area);
}

static void
pps_window_cmd_add_highlight_annotation (GSimpleAction *action,
                                        GVariant      *state,
                                        gpointer       user_data)
{
	PpsWindow *pps_window = user_data;

	pps_window_begin_add_annot (pps_window, PPS_ANNOTATION_TYPE_TEXT_MARKUP);
}

static void
pps_window_cmd_add_annotation (GSimpleAction *action,
			      GVariant      *state,
			      gpointer       user_data)
{
	PpsWindow *pps_window = user_data;

	pps_window_begin_add_annot (pps_window, PPS_ANNOTATION_TYPE_TEXT);
}

static void
pps_window_cmd_toggle_edit_annots (GSimpleAction *action,
				  GVariant      *state,
				  gpointer       user_data)
{
	PpsWindow *pps_window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	gtk_revealer_set_reveal_child (GTK_REVEALER (priv->annots_toolbar_revealer),
				       g_variant_get_boolean (state));
	g_simple_action_set_state (action, state);
}

static void
pps_window_dispose (GObject *object)
{
	PpsWindow *window = PPS_WINDOW (object);
	PpsWindowPrivate *priv = GET_PRIVATE (window);

#ifdef ENABLE_DBUS
	if (priv->skeleton != NULL) {
                pps_window_emit_closed (window);

                g_dbus_interface_skeleton_unexport (G_DBUS_INTERFACE_SKELETON (priv->skeleton));
		g_clear_object (&priv->skeleton);
		g_clear_pointer (&priv->dbus_object_path, g_free);
	}
#endif /* ENABLE_DBUS */

	g_clear_object (&priv->bookmarks);
	g_clear_object (&priv->metadata);

	g_clear_handle_id (&priv->setup_document_idle, g_source_remove);
	g_clear_handle_id (&priv->loading_message_timeout, g_source_remove);

	g_clear_object (&priv->monitor);
	g_clear_pointer (&priv->title, pps_window_title_free);

	g_clear_object (&priv->attachment_popup_menu);

	g_clear_object (&priv->settings);
	if (priv->default_settings) {
		g_settings_apply (priv->default_settings);
		g_clear_object (&priv->default_settings);
	}

	/* FIXME: crash when closing papers window */
	/* g_clear_object (&priv->lockdown_settings); */

	if (priv->model) {
		g_signal_handlers_disconnect_by_func (priv->model,
						      page_changed_cb,
						      window);
		g_clear_object (&priv->model);
	}

	g_clear_object (&priv->document);

	pps_window_clear_load_job (window);
	pps_window_clear_reload_job (window);
	pps_window_clear_save_job (window);
	pps_window_clear_local_uri (window);
	pps_window_clear_progress_idle (window);
	g_clear_object (&priv->progress_cancellable);

	pps_window_close_dialogs (window);

	g_clear_object (&priv->link);
	g_clear_object (&priv->image);
	g_clear_object (&priv->annot);

	if (priv->attach_list) {
		g_list_free_full (priv->attach_list, g_object_unref);
		priv->attach_list = NULL;
	}

	g_clear_pointer (&priv->uri, g_free);

	g_clear_pointer (&priv->display_name, g_free);
	g_clear_pointer (&priv->edit_name, g_free);

	g_clear_object (&priv->dest);
	g_clear_object (&priv->history);

	g_clear_pointer (&priv->print_queue, g_queue_free);

	G_OBJECT_CLASS (pps_window_parent_class)->dispose (object);
}

static void
pps_window_button_pressed (GtkGestureClick* self,
			      gint n_press,
			      gdouble x,
			      gdouble y,
			      gpointer user_data)
{
        PpsWindow *window = PPS_WINDOW (user_data);
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GdkEvent *event = gtk_event_controller_get_current_event (GTK_EVENT_CONTROLLER (self));

	if (gdk_event_get_event_type (event) == GDK_TOUCH_BEGIN)
		return;

        switch (gdk_button_event_get_button(event)) {
        case MOUSE_BACK_BUTTON: {
		gint old_page = pps_document_model_get_page (priv->model);

		if (old_page >= 0)
			pps_history_add_page (priv->history, old_page);

		pps_history_go_back (priv->history);
                break;
	}
	case MOUSE_FORWARD_BUTTON:
		pps_history_go_forward (priv->history);
		break;
        default:
		break;
	}
}

static const GActionEntry actions[] = {
	{ "open", pps_window_cmd_file_open },
	{ "continuous", NULL, NULL, "true", pps_window_cmd_continuous },
	{ "show-properties", pps_window_cmd_file_properties },
	{ "open-copy", pps_window_cmd_file_open_copy },
	{ "save-as", pps_window_cmd_save_as },
	{ "print", pps_window_cmd_file_print },
	{ "copy", pps_window_cmd_edit_copy },
	{ "select-all", pps_window_cmd_edit_select_all },
	{ "go-previous-page", pps_window_cmd_go_previous_page },
	{ "go-next-page", pps_window_cmd_go_next_page },
	{ "go-first-page", pps_window_cmd_go_first_page },
	{ "go-last-page", pps_window_cmd_go_last_page },
	{ "select-page", pps_window_cmd_focus_page_selector },
	{ "dual-page", NULL, NULL, "false", pps_window_cmd_dual },
	{ "dual-odd-left", NULL, NULL, "false", pps_window_cmd_dual_odd_pages_left },
	{ "rtl", NULL, NULL, "false", pps_window_cmd_rtl },
	{ "show-side-pane", NULL, NULL, "false", pps_window_view_cmd_toggle_sidebar },
	{ "inverted-colors", NULL, NULL, "false", pps_window_cmd_view_inverted_colors },
	{ "enable-spellchecking", NULL, NULL, "false", pps_window_cmd_view_enable_spellchecking },
	{ "fullscreen", NULL, NULL, "false", pps_window_cmd_view_fullscreen },
	{ "presentation", pps_window_cmd_view_presentation },
	{ "rotate-left", pps_window_cmd_edit_rotate_left },
	{ "rotate-right", pps_window_cmd_edit_rotate_right },
	{ "zoom-in", pps_window_cmd_view_zoom_in },
	{ "zoom-out", pps_window_cmd_view_zoom_out },
	{ "reload", pps_window_cmd_view_reload },
	{ "add-bookmark", pps_window_cmd_bookmarks_add },
	{ "delete-bookmark", pps_window_cmd_bookmarks_delete },
	{ "close", pps_window_cmd_file_close_window },
	{ "sizing-mode", NULL, "s", "'free'", pps_window_change_sizing_mode_action_state },
	{ "zoom", pps_window_cmd_view_zoom, "d" },
	{ "escape", pps_window_cmd_escape },
	{ "caret-navigation", NULL, NULL, "false", pps_window_cmd_view_toggle_caret_navigation },
	{ "add-annotation", NULL, NULL, "false", pps_window_cmd_add_annotation },
	{ "highlight-annotation", pps_window_cmd_add_highlight_annotation },
	{ "toggle-edit-annots", NULL, NULL, "false", pps_window_cmd_toggle_edit_annots },
	/* Popups specific items */
	{ "open-link", pps_window_popup_cmd_open_link },
	{ "open-link-new-window", pps_window_popup_cmd_open_link_new_window },
	{ "go-to-link", pps_window_popup_cmd_open_link },
	{ "copy-link-address", pps_window_popup_cmd_copy_link_address },
	{ "save-image", pps_window_popup_cmd_save_image_as },
	{ "copy-image", pps_window_popup_cmd_copy_image },
	{ "open-attachment", pps_window_popup_cmd_open_attachment },
	{ "save-attachment", pps_window_popup_cmd_save_attachment_as },
	{ "annot-properties", pps_window_popup_cmd_annot_properties },
	{ "remove-annot", pps_window_popup_cmd_remove_annotation }
};

static const GActionEntry doc_actions[] = {
	{ "go-forward", pps_window_cmd_go_forward },
	{ "go-backwards", pps_window_cmd_go_backwards },
	{ "go-back-history", pps_window_cmd_go_back_history },
	{ "go-forward-history", pps_window_cmd_go_forward_history },
	{ "find", pps_window_cmd_find },
	{ "toggle-find", NULL, NULL, "false", pps_window_cmd_toggle_find },
	{ "find-next", pps_window_cmd_find_next },
	{ "find-previous", pps_window_cmd_find_previous },
};

static void
sidebar_links_link_activated_cb (void *sidebar_links, PpsLink *link, PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_handle_link (PPS_VIEW (priv->view), link);
}


static void
activate_link_cb (GObject *object, PpsLink *link, PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_handle_link (PPS_VIEW (priv->view), link);
	gtk_widget_grab_focus (priv->view);
}

static void
history_changed_cb (PpsHistory *history,
                    PpsWindow  *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	gtk_widget_action_set_enabled (GTK_WIDGET (window), "doc.go-back-history",
				      pps_history_can_go_back (priv->history));
	gtk_widget_action_set_enabled (GTK_WIDGET (window), "doc.go-forward-history",
				      pps_history_can_go_forward (priv->history));
}

static void
sidebar_layers_visibility_changed (PpsSidebar       *layers,
				   PpsWindow        *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_reload (PPS_VIEW (priv->view));
}

static void
sidebar_annots_annot_activated_cb (PpsSidebarAnnotations *sidebar_annots,
				   PpsMapping            *annot_mapping,
				   PpsWindow             *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_focus_annotation (PPS_VIEW (priv->view), annot_mapping);
}

static void
pps_window_begin_add_annot (PpsWindow        *window,
			   PpsAnnotationType annot_type)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (annot_type == PPS_ANNOTATION_TYPE_TEXT_MARKUP &&
	    pps_view_has_selection (PPS_VIEW (priv->view))) {
		pps_view_add_text_markup_annotation_for_selected_text (PPS_VIEW (priv->view));
		return;
	}

	pps_view_begin_add_annotation (PPS_VIEW (priv->view), annot_type);
}

static void
view_annot_added (PpsView       *view,
		  PpsAnnotation *annot,
		  PpsWindow     *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_sidebar_annotations_annot_added (PPS_SIDEBAR_ANNOTATIONS (priv->sidebar_annots),
					    annot);
	pps_annotations_toolbar_add_annot_finished (PPS_ANNOTATIONS_TOOLBAR (priv->annots_toolbar));
}

static void
view_annot_cancel_add (PpsView   *view,
		       PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	pps_annotations_toolbar_add_annot_finished (PPS_ANNOTATIONS_TOOLBAR (priv->annots_toolbar));
}

static void
view_annot_changed (PpsView       *view,
		    PpsAnnotation *annot,
		    PpsWindow     *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_sidebar_annotations_annot_changed (PPS_SIDEBAR_ANNOTATIONS (priv->sidebar_annots),
					      annot);
}

static void
view_annot_removed (PpsView       *view,
		    PpsAnnotation *annot,
		    PpsWindow     *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_sidebar_annotations_annot_removed (PPS_SIDEBAR_ANNOTATIONS (priv->sidebar_annots));
}

static void
pps_window_cancel_add_annot(PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_cancel_add_annotation (PPS_VIEW (priv->view));
}

static void
window_maximized_changed (GObject    *object,
			    GParamSpec *pspec,
			    PpsWindow   *pps_window)
{
	GtkWindow *window = GTK_WINDOW (pps_window);
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	if (!gtk_window_is_fullscreen (window)
		&& !pps_window_is_empty (pps_window)
		&& priv->metadata)
		pps_metadata_set_boolean (priv->metadata, "window-maximized",
				gtk_window_is_maximized (window));
}

static void
window_size_changed_cb (GObject		*object,
			GParamSpec	*pspec,
			PpsWindow 	*window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	gdouble document_width, document_height;
	gint window_width, window_height;

	if (!priv->metadata)
		return;

	if (!gtk_window_is_fullscreen (GTK_WINDOW (window)) &&
	    !gtk_window_is_maximized (GTK_WINDOW (window))) {
		if (priv->document) {
			pps_document_get_max_page_size (priv->document,
						       &document_width, &document_height);
			gtk_window_get_default_size (GTK_WINDOW (window), &window_width, &window_height);
			g_settings_set (priv->default_settings, "window-ratio", "(dd)",
					(double)window_width / document_width,
					(double)window_height / document_height);

			pps_metadata_set_int (priv->metadata, "window_width",window_width);
			pps_metadata_set_int (priv->metadata, "window_height", window_height);
		}
	}
}

static gchar *
get_uri (const char *filename, PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	gchar *ret;

	if (g_path_is_absolute (filename)) {
		ret =  g_strdup (filename);
	} else {
		GFile *base_file, *file;
		gchar *dir;

		dir = g_path_get_dirname (priv->uri);
		base_file = g_file_new_for_uri (dir);
		file = g_file_resolve_relative_path (base_file, filename);
		ret = g_file_get_uri (file);

		g_free (dir);
		g_object_unref (base_file);
		g_object_unref (file);
	}

	return ret;
}

static gboolean
file_is_pdf (const char *uri)
{
	GFile *file;
	GFileInfo *file_info;
	gboolean ret = FALSE;

	file = g_file_new_for_uri (uri);
	file_info = g_file_query_info (file,
				       G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
				       G_FILE_QUERY_INFO_NONE, NULL, NULL);
	if (file_info != NULL) {
		const gchar *content_type;
		content_type = g_file_info_get_content_type (file_info);
		if (content_type) {
			gchar *mime_type;
			mime_type = g_content_type_get_mime_type (content_type);
			if (g_ascii_strcasecmp (mime_type, "application/pdf") == 0)
				ret = TRUE;
			g_free (mime_type);
		}
		g_object_unref (file_info);
	}
	g_object_unref (file);

	return ret;
}

static void
launch_action (PpsWindow *pps_window, PpsLinkAction *action)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	const char *filename = pps_link_action_get_filename (action);
	gchar *uri;

	if (filename == NULL)
		return;

	uri = get_uri (filename, pps_window);

	if (!file_is_pdf (uri)) {
		pps_window_warning_message (pps_window,
			_("Security alert: this document has been prevented from opening the file “%s”"),
			filename);
		return;
	}
	/* We are asked to open a PDF file, from a click event, proceed with that - Issue #48
	 * This spawns new Papers process or if already opened presents its window */
	pps_application_open_uri_at_dest (PPS_APP, uri,
					 pps_link_action_get_dest (action),
					 priv->window_mode);
	g_free (uri);

}

static void
launch_external_uri (PpsWindow *window, PpsLinkAction *action)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	const gchar *uri = pps_link_action_get_uri (action);
	GError *error = NULL;
	gboolean ret;
	GdkAppLaunchContext *context;
	GdkDisplay *display;
	GFile *file;
	gchar *uri_scheme;

	display = gtk_widget_get_display (GTK_WIDGET (window));
	context = gdk_display_get_app_launch_context (display);
	file = g_file_new_for_uri (uri);
	uri_scheme = g_file_get_uri_scheme (file);
	g_object_unref (file);

	if (uri_scheme == NULL) {
		gchar *new_uri;

		/* Not a valid uri, assume http if it starts with www */
		if (g_str_has_prefix (uri, "www.")) {
			new_uri = g_strdup_printf ("http://%s", uri);
		} else {
			GFile *parent;

			file = g_file_new_for_uri (priv->uri);
			parent = g_file_get_parent (file);
			g_object_unref (file);
			if (parent) {
				gchar *parent_uri = g_file_get_uri (parent);

				new_uri = g_build_filename (parent_uri, uri, NULL);
				g_free (parent_uri);
				g_object_unref (parent);
			} else {
				new_uri = g_strdup_printf ("file:///%s", uri);
			}
		}
		ret = g_app_info_launch_default_for_uri (new_uri, G_APP_LAUNCH_CONTEXT (context), &error);
		g_free (new_uri);
	} else {
		ret = g_app_info_launch_default_for_uri (uri, G_APP_LAUNCH_CONTEXT (context), &error);
	}

  	if (ret == FALSE) {
		pps_window_error_message (window, error,
					 "%s", _("Unable to open external link"));
		g_error_free (error);
	}

        g_object_unref (context);
}

static void
open_remote_link (PpsWindow *window, PpsLinkAction *action)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	gchar *uri;
	gchar *dir;

	dir = g_path_get_dirname (priv->uri);

	uri = g_build_filename (dir, pps_link_action_get_filename (action),
				NULL);
	g_free (dir);

	pps_application_open_uri_at_dest (PPS_APP, uri,
					 pps_link_action_get_dest (action),
					 PPS_WINDOW_MODE_NORMAL);

	g_free (uri);
}

static void
do_action_named (PpsWindow *window, PpsLinkAction *action)
{
	const gchar *name = pps_link_action_get_name (action);

	if (g_ascii_strcasecmp (name, "FirstPage") == 0) {
		g_action_group_activate_action (G_ACTION_GROUP (window), "go-first-page", NULL);
	} else if (g_ascii_strcasecmp (name, "PrevPage") == 0) {
		g_action_group_activate_action (G_ACTION_GROUP (window), "go-previous-page", NULL);
	} else if (g_ascii_strcasecmp (name, "NextPage") == 0) {
		g_action_group_activate_action (G_ACTION_GROUP (window), "go-next-page", NULL);
	} else if (g_ascii_strcasecmp (name, "LastPage") == 0) {
		g_action_group_activate_action (G_ACTION_GROUP (window), "go-last-page", NULL);
	} else if (g_ascii_strcasecmp (name, "GoToPage") == 0) {
		g_action_group_activate_action (G_ACTION_GROUP (window), "select-page", NULL);
	} else if (g_ascii_strcasecmp (name, "Find") == 0) {
		gtk_widget_activate_action (GTK_WIDGET (window), "doc.find", NULL);
	} else if (g_ascii_strcasecmp (name, "Close") == 0) {
		gtk_window_close (GTK_WINDOW (window));
	} else if (g_ascii_strcasecmp (name, "Print") == 0) {
		g_action_group_activate_action (G_ACTION_GROUP (window), "print", NULL);
	} else if (g_ascii_strcasecmp (name, "SaveAs") == 0) {
		g_action_group_activate_action (G_ACTION_GROUP (window), "save-as", NULL);
	} else {
		g_warning ("Unimplemented named action: %s, please post a "
		           "bug report in Papers Gitlab "
		           "(https://gitlab.gnome.org/GNOME/Incubator/papers/issues) "
			   "with a testcase.",
			   name);
	}
}

static void
reset_form (PpsWindow *window, PpsLinkAction *action)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	PpsDocument      *document = priv->document;

	if (PPS_IS_DOCUMENT_FORMS (document)) {
		pps_document_forms_reset_form (PPS_DOCUMENT_FORMS (document), action);
		pps_view_reload (PPS_VIEW (priv->view));
	}
}

static void
view_external_link_cb (PpsWindow *window, PpsLinkAction *action)
{
	switch (pps_link_action_get_action_type (action)) {
	        case PPS_LINK_ACTION_TYPE_GOTO_DEST: {
			PpsLinkDest *dest;

			dest = pps_link_action_get_dest (action);
			if (!dest)
				return;

			pps_window_open_copy_at_dest (window, dest);
		}
			break;
	        case PPS_LINK_ACTION_TYPE_EXTERNAL_URI:
			launch_external_uri (window, action);
			break;
	        case PPS_LINK_ACTION_TYPE_LAUNCH:
			launch_action (window, action);
			break;
	        case PPS_LINK_ACTION_TYPE_GOTO_REMOTE:
			open_remote_link (window, action);
			break;
	        case PPS_LINK_ACTION_TYPE_NAMED:
			do_action_named (window, action);
			break;
	        case PPS_LINK_ACTION_TYPE_RESET_FORM:
			reset_form (window, action);
			break;
	        default:
			g_assert_not_reached ();
	}
}

static void
pps_window_popup_cmd_open_link (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_handle_link (PPS_VIEW (priv->view), priv->link);
}

static void
pps_window_popup_cmd_open_link_new_window (GSimpleAction *action,
					  GVariant      *parameter,
					  gpointer       user_data)
{
	PpsLinkAction *pps_action = NULL;
	PpsLinkDest   *dest;
	PpsWindow     *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_action = pps_link_get_action (priv->link);
	if (!pps_action)
		return;

	dest = pps_link_action_get_dest (pps_action);
	if (!dest)
		return;

	pps_window_open_copy_at_dest (window, dest);
}

static void
pps_window_popup_cmd_copy_link_address (GSimpleAction *action,
				       GVariant      *parameter,
				       gpointer       user_data)
{
	PpsLinkAction *pps_action;
	PpsWindow     *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_action = pps_link_get_action (priv->link);
	if (!pps_action)
		return;

	pps_view_copy_link_address (PPS_VIEW (priv->view),
				   pps_action);
}

static GFile *
create_file_from_uri_for_format (const gchar     *uri,
				 GdkPixbufFormat *format)
{
	GFile  *target_file;
	gchar **extensions;
	gchar  *uri_extension;
	gint    i;

	extensions = gdk_pixbuf_format_get_extensions (format);
	for (i = 0; extensions[i]; i++) {
		if (g_str_has_suffix (uri, extensions[i])) {
			g_strfreev (extensions);
			return g_file_new_for_uri (uri);
		}
	}

	uri_extension = g_strconcat (uri, ".", extensions[0], NULL);
	target_file = g_file_new_for_uri (uri_extension);
	g_free (uri_extension);
	g_strfreev (extensions);

	return target_file;
}

static void
image_save_dialog_response_cb (GtkFileDialog     *dialog,
			       GAsyncResult      *result,
			       PpsWindow         *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFile           *target_file;
	gboolean         is_native;
	GError          *error = NULL;
	GdkPixbuf       *pixbuf;
	gchar           *uri;
	gchar           *filename;
	gchar           *file_format;
	GdkPixbufFormat *format = NULL;
	GFile		*file;

	file = gtk_file_dialog_save_finish (dialog, result, NULL);

	if (!file)
		return;

	pps_window_file_dialog_save_folder (pps_window, file,
					     G_USER_DIRECTORY_PICTURES);

	uri = g_file_get_uri (file);
	g_clear_object (&file);

	format = get_gdk_pixbuf_format_by_extension (uri);

	if (format == NULL && g_strrstr (uri, ".") == NULL) {
		/* no extension found and no extension provided within uri */
		format = get_gdk_pixbuf_format_by_extension (".png");
		if (format == NULL) {
			/* no .png support, try .jpeg */
			format = get_gdk_pixbuf_format_by_extension (".jpeg");
		}
	}

	if (format == NULL) {
		pps_window_error_message (pps_window, NULL,
					 "%s",
					 _("Couldn’t find appropriate format to save image"));
		g_free (uri);
		return;
	}

	target_file = create_file_from_uri_for_format (uri, format);
	g_free (uri);

	is_native = g_file_is_native (target_file);
	if (is_native) {
		filename = g_file_get_path (target_file);
	} else {
                /* Create a temporary local file to save to */
                if (pps_mkstemp ("saveimage.XXXXXX", &filename, &error) == -1)
                        goto has_error;
	}

	pps_document_doc_mutex_lock ();
	pixbuf = pps_document_images_get_image (PPS_DOCUMENT_IMAGES (priv->document),
					       priv->image);
	pps_document_doc_mutex_unlock ();

	file_format = gdk_pixbuf_format_get_name (format);
	gdk_pixbuf_save (pixbuf, filename, file_format, &error, NULL);
	g_free (file_format);
	g_object_unref (pixbuf);

    has_error:
	if (error) {
		pps_window_error_message (pps_window, error,
					 "%s", _("The image could not be saved."));
		g_error_free (error);
		g_free (filename);
		g_object_unref (target_file);
		return;
	}

	if (!is_native) {
		GFile *source_file;

		source_file = g_file_new_for_path (filename);

		pps_window_save_remote (pps_window, PPS_SAVE_IMAGE,
				       source_file, target_file);
		g_object_unref (source_file);
	}

	g_free (filename);
	g_object_unref (target_file);
}

static void
pps_window_popup_cmd_save_image_as (GSimpleAction *action,
				   GVariant      *parameter,
				   gpointer       user_data)
{
	PpsWindow  *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);
	GtkFileDialog *dialog;
	g_autoptr(GDateTime) now = NULL;
	g_autofree gchar *initial_name =  NULL;

	if (!priv->image)
		return;

	dialog = gtk_file_dialog_new ();

	gtk_file_dialog_set_title (dialog, _("Save Image"));
	gtk_file_dialog_set_modal (dialog, TRUE);

	now = g_date_time_new_now_local ();

	/* We simply give user a default name here. The extension is not hardcoded
	 * and we will detect the target file extension to determine the format.
	 * We will fallback to png or jpeg when no extension is specified.
	 */
	initial_name = g_date_time_format (now, "%c.png");
	gtk_file_dialog_set_initial_name (dialog, initial_name);

	pps_window_file_dialog_restore_folder (window, dialog,
					       G_USER_DIRECTORY_PICTURES);

	gtk_file_dialog_save (dialog, GTK_WINDOW (window), NULL,
			      (GAsyncReadyCallback)image_save_dialog_response_cb,
			      window);
}


static void
pps_window_popup_cmd_copy_image (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	GdkClipboard *clipboard;
	GdkPixbuf    *pixbuf;
	PpsWindow     *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->image)
		return;

	clipboard = gtk_widget_get_clipboard (GTK_WIDGET (window));
	pps_document_doc_mutex_lock ();
	pixbuf = pps_document_images_get_image (PPS_DOCUMENT_IMAGES (priv->document),
					       priv->image);
	pps_document_doc_mutex_unlock ();

	gdk_clipboard_set_texture (clipboard,
			gdk_texture_new_for_pixbuf (pixbuf));
	g_object_unref (pixbuf);
}

static void
pps_window_popup_cmd_annot_properties_response_cb (GtkDialog	*self,
						  gint		 response_id,
						  gpointer	 user_data)
{
	PpsAnnotationPropertiesDialog *dialog = PPS_ANNOTATION_PROPERTIES_DIALOG (self);
	PpsWindow                     *window = user_data;
	PpsWindowPrivate              *priv = GET_PRIVATE (window);

	GdkRGBA                       rgba;
	gdouble                       opacity;
	const gchar                  *author;
	gboolean                      popup_is_open;
	PpsAnnotation                 *annot = priv->annot;
	PpsAnnotationsSaveMask         mask = PPS_ANNOTATIONS_SAVE_NONE;

	if (response_id != GTK_RESPONSE_APPLY) {
		gtk_window_destroy (GTK_WINDOW (dialog));
		return;
	}

	/* Set annotations changes */
	author = pps_annotation_properties_dialog_get_author (dialog);
	if (pps_annotation_markup_set_label (PPS_ANNOTATION_MARKUP (annot), author))
		mask |= PPS_ANNOTATIONS_SAVE_LABEL;

	pps_annotation_properties_dialog_get_rgba (dialog, &rgba);
	if (pps_annotation_set_rgba (annot, &rgba))
		mask |= PPS_ANNOTATIONS_SAVE_COLOR;

	opacity = pps_annotation_properties_dialog_get_opacity (dialog);
	if (pps_annotation_markup_set_opacity (PPS_ANNOTATION_MARKUP (annot), opacity))
		mask |= PPS_ANNOTATIONS_SAVE_OPACITY;

	popup_is_open = pps_annotation_properties_dialog_get_popup_is_open (dialog);
	if (pps_annotation_markup_set_popup_is_open (PPS_ANNOTATION_MARKUP (annot), popup_is_open))
		mask |= PPS_ANNOTATIONS_SAVE_POPUP_IS_OPEN;

	if (PPS_IS_ANNOTATION_TEXT (annot)) {
		PpsAnnotationTextIcon icon;

		icon = pps_annotation_properties_dialog_get_text_icon (dialog);
		if (pps_annotation_text_set_icon (PPS_ANNOTATION_TEXT (annot), icon))
			mask |= PPS_ANNOTATIONS_SAVE_TEXT_ICON;
	}

	if (PPS_IS_ANNOTATION_TEXT_MARKUP (annot)) {
		PpsAnnotationTextMarkupType markup_type;

		markup_type = pps_annotation_properties_dialog_get_text_markup_type (dialog);
		if (pps_annotation_text_markup_set_markup_type (PPS_ANNOTATION_TEXT_MARKUP (annot), markup_type))
			mask |= PPS_ANNOTATIONS_SAVE_TEXT_MARKUP_TYPE;
	}

	if (mask != PPS_ANNOTATIONS_SAVE_NONE) {
		pps_document_doc_mutex_lock ();
		pps_document_annotations_save_annotation (PPS_DOCUMENT_ANNOTATIONS (priv->document),
							 priv->annot,
							 mask);
		pps_document_doc_mutex_unlock ();

		/* FIXME: update annot region only */
		pps_view_reload (PPS_VIEW (priv->view));

		pps_sidebar_annotations_annot_changed (PPS_SIDEBAR_ANNOTATIONS (priv->sidebar_annots), annot);
	}

	gtk_window_destroy (GTK_WINDOW (dialog));
}

static void
pps_window_popup_cmd_annot_properties (GSimpleAction *action,
				      GVariant      *parameter,
				      gpointer       user_data)
{
	PpsWindow                     *window = user_data;
	PpsWindowPrivate              *priv = GET_PRIVATE (window);
	PpsAnnotationPropertiesDialog *dialog;
	PpsAnnotation                 *annot = priv->annot;

	if (!annot)
		return;

	dialog = PPS_ANNOTATION_PROPERTIES_DIALOG (pps_annotation_properties_dialog_new_with_annotation (priv->annot));
	gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);

	g_signal_connect (dialog, "response",
				G_CALLBACK (pps_window_popup_cmd_annot_properties_response_cb),
				window);
	gtk_widget_set_visible (GTK_WIDGET (dialog), TRUE);
}

static void
pps_window_popup_cmd_remove_annotation (GSimpleAction *action,
				       GVariant      *parameter,
				       gpointer       user_data)
{
	PpsWindow *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	pps_view_remove_annotation (PPS_VIEW (priv->view),
				   priv->annot);
}

static void
pps_window_popup_cmd_open_attachment (GSimpleAction *action,
				     GVariant      *parameter,
				     gpointer       user_data)
{
	GList     *l;
	GdkDisplay *display;
	PpsWindow  *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->attach_list)
		return;

	display = gtk_widget_get_display (GTK_WIDGET (window));

	for (l = priv->attach_list; l && l->data; l = g_list_next (l)) {
		PpsAttachment *attachment;
		GError       *error = NULL;
		GdkAppLaunchContext *context = gdk_display_get_app_launch_context (display);

		attachment = (PpsAttachment *) l->data;

		pps_attachment_open (attachment, G_APP_LAUNCH_CONTEXT (context), &error);

		if (error) {
			pps_window_error_message (window, error,
						 "%s", _("Unable to open attachment"));
			g_error_free (error);
		}

		g_clear_object (&context);
	}
}

static void
attachment_save_dialog_response_cb (GtkFileDialog     *dialog,
				    GAsyncResult      *result,
				    PpsWindow         *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	GFile                *target_file;
	GList                *l;
	gboolean              is_dir;
	gboolean              is_native;

	is_dir = g_list_length (priv->attach_list) != 1;

	if (is_dir) {
		target_file = gtk_file_dialog_select_folder_finish (dialog, result, NULL);
	} else {
		target_file = gtk_file_dialog_save_finish (dialog, result, NULL);
	}

	if (!target_file)
		return;

	pps_window_file_dialog_save_folder (pps_window, target_file, G_USER_DIRECTORY_DOCUMENTS);

	is_native = g_file_is_native (target_file);

	for (l = priv->attach_list; l && l->data; l = g_list_next (l)) {
		PpsAttachment *attachment;

		attachment = (PpsAttachment *) l->data;

		save_attachment_to_target_file (attachment,
		                                target_file,
		                                is_dir,
		                                is_native,
		                                pps_window);
	}

	g_object_unref (target_file);
}

static void
pps_window_popup_cmd_save_attachment_as (GSimpleAction *action,
					GVariant      *parameter,
					gpointer       user_data)
{
	GtkFileDialog *dialog;
	PpsAttachment *attachment = NULL;
	PpsWindow     *window = user_data;
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (!priv->attach_list)
		return;

	if (g_list_length (priv->attach_list) == 1)
		attachment = (PpsAttachment *) priv->attach_list->data;

	dialog = gtk_file_dialog_new ();

	gtk_file_dialog_set_title (dialog, _("Save Attachment"));
	gtk_file_dialog_set_modal (dialog, TRUE);

	if (attachment)
		gtk_file_dialog_set_initial_name (dialog, pps_attachment_get_name (attachment));

        pps_window_file_dialog_restore_folder (window, dialog,
                                               G_USER_DIRECTORY_DOCUMENTS);


	if (attachment) {
		gtk_file_dialog_save (dialog, GTK_WINDOW (window), NULL,
			      (GAsyncReadyCallback)attachment_save_dialog_response_cb,
			      window);
	} else {
		gtk_file_dialog_select_folder (dialog, GTK_WINDOW (window), NULL,
			      (GAsyncReadyCallback)attachment_save_dialog_response_cb,
			      window);
	}
}

#ifdef ENABLE_DBUS
static void
pps_window_emit_closed (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

	if (priv->skeleton == NULL)
		return;

        pps_papers_window_emit_closed (priv->skeleton);

	/* If this is the last window call g_dbus_connection_flush_sync()
	 * to make sure the signal is emitted.
	 */
	if (pps_application_get_n_windows (PPS_APP) == 1)
		g_dbus_connection_flush_sync (g_application_get_dbus_connection (g_application_get_default ()), NULL, NULL);
}

static void
pps_window_emit_doc_loaded (PpsWindow *window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (window);

        if (priv->skeleton == NULL)
                return;

        pps_papers_window_emit_document_loaded (priv->skeleton, priv->uri);
}
#endif /* ENABLE_DBUS */

static void
pps_window_init (PpsWindow *pps_window)
{
	GtkEventController *controller;
	GSimpleActionGroup *group;
	guint page_cache_mb;
	gboolean allow_links_change_zoom;
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

#ifdef ENABLE_DBUS
	GError *error = NULL;
	GDBusConnection *connection;
	static gint window_id = 0;
#endif

	/* for drop target support */
	g_type_ensure (GDK_TYPE_FILE_LIST);

	g_type_ensure (PPS_TYPE_VIEW);
	g_type_ensure (PPS_TYPE_SIDEBAR);
	g_type_ensure (PPS_TYPE_SEARCH_BOX);
	g_type_ensure (PPS_TYPE_FIND_SIDEBAR);
	g_type_ensure (PPS_TYPE_SIDEBAR_BOOKMARKS);
	g_type_ensure (PPS_TYPE_PAGE_SELECTOR);
	g_type_ensure (PPS_TYPE_SIDEBAR_ANNOTATIONS);
	g_type_ensure (PPS_TYPE_ANNOTATIONS_TOOLBAR);
	gtk_widget_init_template (GTK_WIDGET (pps_window));

#ifdef ENABLE_DBUS
	connection = g_application_get_dbus_connection (g_application_get_default ());
        if (connection) {
                PpsPapersWindow *skeleton;

		priv->dbus_object_path = g_strdup_printf (PPS_WINDOW_DBUS_OBJECT_PATH, window_id++);

                skeleton = pps_papers_window_skeleton_new ();
                if (g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (skeleton),
                                                      connection,
                                                      priv->dbus_object_path,
                                                      &error)) {
                        priv->skeleton = skeleton;
                } else {
                        g_printerr ("Failed to register bus object %s: %s\n",
				    priv->dbus_object_path, error->message);
			g_clear_pointer (&error, g_error_free);
			g_clear_pointer (&priv->dbus_object_path, g_free);
			g_clear_object (&priv->skeleton);
                }
        }
#endif /* ENABLE_DBUS */

        priv->presentation_mode_inhibit_id = 0;
	priv->title = pps_window_title_new (pps_window);
	priv->history = pps_history_new (priv->model);

	g_signal_connect (priv->history, "activate-link",
			  G_CALLBACK (activate_link_cb),
			  pps_window);
        g_signal_connect (priv->history, "changed",
                          G_CALLBACK (history_changed_cb),
                          pps_window);

	g_action_map_add_action_entries (G_ACTION_MAP (pps_window),
					 actions, G_N_ELEMENTS (actions),
					 pps_window);
	group = g_simple_action_group_new ();
	g_action_map_add_action_entries (G_ACTION_MAP (group),
					 doc_actions,
					 G_N_ELEMENTS (doc_actions),
					 pps_window);
	gtk_widget_insert_action_group (GTK_WIDGET (pps_window),
					"doc", G_ACTION_GROUP (group));
	// These are only enabled once the search has started
	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.find-next", FALSE);
	gtk_widget_action_set_enabled (GTK_WIDGET (pps_window), "doc.find-previous", FALSE);

	pps_page_selector_set_model (PPS_PAGE_SELECTOR (priv->page_selector),
		priv->model);

	g_signal_connect (priv->page_selector,
			  "activate-link",
			  G_CALLBACK (activate_link_cb),
			  pps_window);

	page_cache_mb = g_settings_get_uint (pps_window_ensure_settings (pps_window),
					     GS_PAGE_CACHE_SIZE);
	pps_view_set_page_cache_size (PPS_VIEW (priv->view),
				     (gsize) page_cache_mb * 1024 * 1024);
	allow_links_change_zoom = g_settings_get_boolean (pps_window_ensure_settings (pps_window),
				     GS_ALLOW_LINKS_CHANGE_ZOOM);
	pps_view_set_allow_links_change_zoom (PPS_VIEW (priv->view),
				     allow_links_change_zoom);
	pps_view_set_model (PPS_VIEW (priv->view), priv->model);


	priv->password_view_cancelled = FALSE;
	g_signal_connect_swapped (priv->view, "external-link",
				  G_CALLBACK (view_external_link_cb),
				  pps_window);
	g_signal_connect_object (priv->view, "handle-link",
			         G_CALLBACK (view_handle_link_cb),
			         pps_window, 0);
	g_signal_connect_object (priv->view, "popup",
				 G_CALLBACK (view_menu_popup_cb),
				 pps_window, 0);
	g_signal_connect_object (priv->view, "selection-changed",
				 G_CALLBACK (view_selection_changed_cb),
				 pps_window, 0);
	g_signal_connect_object (priv->sidebar_bookmarks, "bookmark-activated",
				 G_CALLBACK (bookmark_activated_cb),
				 pps_window, 0);
	g_signal_connect_object (priv->view, "scroll",
				 G_CALLBACK (scroll_history_cb),
				 pps_window, 0);
	g_signal_connect_object (priv->scrolled_window, "scroll-child",
				 G_CALLBACK (scroll_child_history_cb),
				 pps_window, 0);
	controller = gtk_event_controller_focus_new ();
	g_signal_connect_object (controller, "enter",
				 G_CALLBACK (scrolled_window_focus_in_cb),
				 pps_window, 0);
	gtk_widget_add_controller (priv->scrolled_window, controller);

	g_signal_connect_object (priv->view, "annot-added",
				 G_CALLBACK (view_annot_added),
				 pps_window, 0);
	g_signal_connect_object (priv->view, "annot-cancel-add",
				 G_CALLBACK (view_annot_cancel_add),
				 pps_window, 0);
	g_signal_connect_object (priv->view, "annot-changed",
				 G_CALLBACK (view_annot_changed),
				 pps_window, 0);
	g_signal_connect_object (priv->view, "annot-removed",
				 G_CALLBACK (view_annot_removed),
				 pps_window, 0);
	g_signal_connect_object (priv->view, "layers-changed",
				 G_CALLBACK (view_layers_changed_cb),
				 pps_window, 0);
	g_signal_connect_object (priv->view, "notify::is-loading",
				 G_CALLBACK (view_is_loading_changed_cb),
				 pps_window, 0);
	g_signal_connect_object (priv->view, "cursor-moved",
				 G_CALLBACK (view_caret_cursor_moved_cb),
				 pps_window, 0);

	priv->default_settings = g_settings_new (GS_SCHEMA_NAME".Default");
	g_settings_delay (priv->default_settings);
	pps_window_setup_default (pps_window);

	sizing_mode_changed_cb (priv->model, NULL, pps_window);
	pps_window_update_actions_sensitivity (pps_window);

	if (g_strcmp0 (PROFILE, "") != 0)
		gtk_widget_add_css_class (GTK_WIDGET (pps_window), "devel");
}

static void
pps_window_class_init (PpsWindowClass *pps_window_class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (pps_window_class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (pps_window_class);
	GtkWindowClass *window_class = GTK_WINDOW_CLASS (pps_window_class);

	g_object_class->dispose = pps_window_dispose;

	window_class->close_request = pps_window_close_handled;

	gtk_widget_class_set_template_from_resource (widget_class,
		"/org/gnome/papers/ui/window.ui");
	gtk_widget_class_bind_template_child_private(widget_class, PpsWindow, toolbar_view);
	gtk_widget_class_bind_template_child_private(widget_class, PpsWindow, sidebar);
	gtk_widget_class_bind_template_child_private(widget_class, PpsWindow, split_view);
	gtk_widget_class_bind_template_child_private(widget_class, PpsWindow, scrolled_window);
	gtk_widget_class_bind_template_child_private(widget_class, PpsWindow, loading_message);
	gtk_widget_class_bind_template_child_private(widget_class, PpsWindow, password_view);

	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, annots_toolbar_revealer);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, annots_toolbar);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, find_sidebar);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, search_bar);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, search_box);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, model);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, view);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, stack);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, error_page);

	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, page_selector);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, header_bar);

	/* sidebar */
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, sidebar_links);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, sidebar_annots);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, sidebar_bookmarks);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, sidebar_layers);

	/* menu button */
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, action_menu_button);

	/* popup menu */
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, view_popup);
	gtk_widget_class_bind_template_child_private (widget_class, PpsWindow, attachment_popup);

	/* bind signal callback */
	gtk_widget_class_bind_template_callback (widget_class, find_sidebar_result_activated_cb);
	gtk_widget_class_bind_template_callback (widget_class, activate_link_cb);
	gtk_widget_class_bind_template_callback (widget_class, pps_window_begin_add_annot);
	gtk_widget_class_bind_template_callback (widget_class, pps_window_cancel_add_annot);
	gtk_widget_class_bind_template_callback (widget_class, window_maximized_changed);
	gtk_widget_class_bind_template_callback (widget_class, window_size_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, sidebar_visibility_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, sidebar_current_page_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, pps_window_button_pressed);
	gtk_widget_class_bind_template_callback (widget_class, pps_window_drag_data_received);
	gtk_widget_class_bind_template_callback (widget_class, view_popup_hide_cb);
	gtk_widget_class_bind_template_callback (widget_class, zoom_selector_activated);
	gtk_widget_class_bind_template_callback (widget_class, find_button_sensitive_changed);
	gtk_widget_class_bind_template_callback (widget_class, pps_spinner_map_cb);
	gtk_widget_class_bind_template_callback (widget_class, pps_spinner_unmap_cb);

	/* search box */
	gtk_widget_class_bind_template_callback (widget_class, search_started_cb);
	gtk_widget_class_bind_template_callback (widget_class, search_cleared_cb);
	gtk_widget_class_bind_template_callback (widget_class, search_entry_stop_search_cb);

	/* model */
	gtk_widget_class_bind_template_callback (widget_class, page_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, document_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, zoom_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, sizing_mode_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, rotation_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, continuous_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, page_layout_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, dual_mode_odd_pages_left_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, direction_changed_cb);
	gtk_widget_class_bind_template_callback (widget_class, inverted_colors_changed_cb);

	/* sidebar */
	gtk_widget_class_bind_template_callback (widget_class, sidebar_links_link_activated_cb);
	gtk_widget_class_bind_template_callback (widget_class, sidebar_annots_annot_activated_cb);
	gtk_widget_class_bind_template_callback (widget_class, attachment_bar_menu_popup_cb);
	gtk_widget_class_bind_template_callback (widget_class, attachment_bar_save_attachment_cb);
	gtk_widget_class_bind_template_callback (widget_class, sidebar_layers_visibility_changed);

	/* password view */
	gtk_widget_class_bind_template_callback (widget_class, pps_window_password_view_unlock);
	gtk_widget_class_bind_template_callback (widget_class, pps_window_password_view_cancelled);
}

/**
 * pps_window_new:
 *
 * Creates a #GtkWidget that represents the window.
 *
 * Returns: the #GtkWidget that represents the window.
 */
PpsWindow *
pps_window_new (void)
{
	return g_object_new (PPS_TYPE_WINDOW,
			     "application", g_application_get_default (),
			     "show-menubar", FALSE,
			     NULL);
}

const gchar *
pps_window_get_dbus_object_path (PpsWindow *pps_window)
{
#ifdef ENABLE_DBUS
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	return priv->dbus_object_path;
#else
	return NULL;
#endif
}


/**
 * pps_window_get_header_bar:
 * @pps_window: the #PpsWindow
 *
 * Returns: (transfer none): the #AdwHeaderBar that represents the toolbar of the window.
 */
AdwHeaderBar *
pps_window_get_header_bar (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);

	g_return_val_if_fail (PPS_WINDOW (pps_window), NULL);

	return ADW_HEADER_BAR (priv->header_bar);
}

void
pps_window_focus_view (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv;

	g_return_if_fail (PPS_WINDOW (pps_window));

	priv = GET_PRIVATE (pps_window);

	gtk_widget_grab_focus (priv->view);
}


/**
 * pps_window_get_metadata:
 * @pps_window: the #PpsWindow
 *
 * Returns: (transfer none): the #PpsMetadata of the document shown in the window.
 */
PpsMetadata *
pps_window_get_metadata (PpsWindow *pps_window)
{
	PpsWindowPrivate *priv = GET_PRIVATE (pps_window);
	g_return_val_if_fail (PPS_WINDOW (pps_window), NULL);

	return priv->metadata;
}
