/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; c-indent-level: 8 -*- */
/* this file is part of papers, a gnome document viewer
 *
 *  Copyright (C) 2004 Red Hat, Inc.
 *  Copyright (C) 2004, 2005 Anders Carlsson <andersca@gnome.org>
 *
 *  Authors:
 *    Jonathan Blandford <jrb@alum.mit.edu>
 *    Anders Carlsson <andersca@gnome.org>
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

#include <string.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include <cairo-gobject.h>

#include "pps-document-misc.h"
#include "pps-job-scheduler.h"
#include "pps-sidebar.h"
#include "pps-sidebar-page.h"
#include "pps-sidebar-thumbnails.h"
#include "pps-utils.h"
#include "pps-window.h"

#define THUMBNAIL_WIDTH 100

typedef struct _PpsThumbsSize
{
	gint width;
	gint height;
} PpsThumbsSize;

typedef struct _PpsThumbsSizeCache {
	gboolean uniform;
	gint uniform_width;
	gint uniform_height;
	PpsThumbsSize *sizes;
} PpsThumbsSizeCache;

struct _PpsSidebarThumbnailsPrivate {
	GtkWidget *swindow;
	GtkWidget *icon_view;
	GtkAdjustment *vadjustment;
	GtkListStore *list_store;
	GHashTable *loading_icons;
	PpsDocument *document;
	PpsDocumentModel *model;
	PpsThumbsSizeCache *size_cache;

	gint n_pages, pages_done;

	int rotation;
	gboolean inverted_colors;
	gboolean blank_first_dual_mode; /* flag for when we're using a blank first thumbnail
					 * for dual mode with !odd_left preference. Issue #30 */
	/* Visible pages */
	gint start_page, end_page;
};

enum {
	COLUMN_PAGE_STRING,
	COLUMN_SURFACE,
	COLUMN_THUMBNAIL_SET,
	COLUMN_JOB,
	NUM_COLUMNS
};

enum {
	PROP_0,
	PROP_DOCUMENT_MODEL,
};

static void         pps_sidebar_thumbnails_clear_model      (PpsSidebarThumbnails     *sidebar);
static gboolean     pps_sidebar_thumbnails_support_document (PpsSidebarPage           *sidebar_page,
							    PpsDocument              *document);
static void         pps_sidebar_thumbnails_set_model	   (PpsSidebarPage   *sidebar_page,
							    PpsDocumentModel *model);
static void         pps_sidebar_thumbnails_page_iface_init  (PpsSidebarPageInterface  *iface);
static void         pps_sidebar_thumbnails_set_current_page (PpsSidebarThumbnails *sidebar,
							    gint     page);
static void         thumbnail_job_completed_callback       (PpsJobThumbnailCairo     *job,
							    PpsSidebarThumbnails     *sidebar_thumbnails);
static void         pps_sidebar_thumbnails_reload           (PpsSidebarThumbnails     *sidebar_thumbnails);
static void         adjustment_changed_cb                  (PpsSidebarThumbnails     *sidebar_thumbnails);
static void         check_toggle_blank_first_dual_mode     (PpsSidebarThumbnails     *sidebar_thumbnails);

G_DEFINE_TYPE_EXTENDED (PpsSidebarThumbnails,
                        pps_sidebar_thumbnails,
                        GTK_TYPE_BOX,
                        0,
                        G_ADD_PRIVATE (PpsSidebarThumbnails)
                        G_IMPLEMENT_INTERFACE (PPS_TYPE_SIDEBAR_PAGE,
					       pps_sidebar_thumbnails_page_iface_init))

/* Thumbnails dimensions cache */
#define PPS_THUMBNAILS_SIZE_CACHE_KEY "pps-thumbnails-size-cache"

static void
get_thumbnail_size_for_page (PpsDocument *document,
			     guint       page,
			     gint       *width,
			     gint       *height)
{
	gdouble scale;
	gdouble w, h;

	pps_document_get_page_size (document, page, &w, &h);
	scale = (gdouble)THUMBNAIL_WIDTH / w;

	*width = MAX ((gint)(w * scale + 0.5), 1);
	*height = MAX ((gint)(h * scale + 0.5), 1);
}

static PpsThumbsSizeCache *
pps_thumbnails_size_cache_new (PpsDocument *document)
{
	PpsThumbsSizeCache *cache;
	gint               i, n_pages;
	PpsThumbsSize      *thumb_size;

	cache = g_new0 (PpsThumbsSizeCache, 1);

	if (pps_document_is_page_size_uniform (document)) {
		cache->uniform = TRUE;
		get_thumbnail_size_for_page (document, 0,
					     &cache->uniform_width,
					     &cache->uniform_height);
		return cache;
	}

	n_pages = pps_document_get_n_pages (document);
	cache->sizes = g_new0 (PpsThumbsSize, n_pages);

	for (i = 0; i < n_pages; i++) {
		thumb_size = &(cache->sizes[i]);
		get_thumbnail_size_for_page (document, i,
					     &thumb_size->width,
					     &thumb_size->height);
	}

	return cache;
}

static void
pps_thumbnails_size_cache_get_size (PpsThumbsSizeCache *cache,
				   gint               page,
				   gint               rotation,
				   gint              *width,
				   gint              *height)
{
	gint w, h;

	if (cache->uniform) {
		w = cache->uniform_width;
		h = cache->uniform_height;
	} else {
		PpsThumbsSize *thumb_size;

		thumb_size = &(cache->sizes[page]);

		w = thumb_size->width;
		h = thumb_size->height;
	}

	if (rotation == 0 || rotation == 180) {
		if (width) *width = w;
		if (height) *height = h;
	} else {
		if (width) *width = h;
		if (height) *height = w;
	}
}

static void
pps_thumbnails_size_cache_free (PpsThumbsSizeCache *cache)
{
	g_clear_pointer (&cache->sizes, g_free);
	g_free (cache);
}

static PpsThumbsSizeCache *
pps_thumbnails_size_cache_get (PpsDocument *document)
{
	PpsThumbsSizeCache *cache;

	cache = g_object_get_data (G_OBJECT (document), PPS_THUMBNAILS_SIZE_CACHE_KEY);
	if (!cache) {
		cache = pps_thumbnails_size_cache_new (document);
		g_object_set_data_full (G_OBJECT (document),
					PPS_THUMBNAILS_SIZE_CACHE_KEY,
					cache,
					(GDestroyNotify)pps_thumbnails_size_cache_free);
	}

	return cache;
}

static gboolean
pps_sidebar_thumbnails_page_is_in_visible_range (PpsSidebarThumbnails *sidebar,
                                                guint                page)
{
        GtkTreePath *path;
        GtkTreePath *start, *end;
        gboolean     retval;
        GList *selection;

	if (!sidebar->priv->icon_view)
		return FALSE;

        selection = gtk_icon_view_get_selected_items (GTK_ICON_VIEW (sidebar->priv->icon_view));
        if (!selection)
                return FALSE;

        path = (GtkTreePath *)selection->data;

        /* We don't handle or expect multiple selection. */
        g_assert (selection->next == NULL);
        g_list_free (selection);

        if (!gtk_icon_view_get_visible_range (GTK_ICON_VIEW (sidebar->priv->icon_view), &start, &end)) {
                gtk_tree_path_free (path);
                return FALSE;
        }

        retval = gtk_tree_path_compare (path, start) >= 0 && gtk_tree_path_compare (path, end) <= 0;
        gtk_tree_path_free (path);
        gtk_tree_path_free (start);
        gtk_tree_path_free (end);

        return retval;
}

static void
pps_sidebar_thumbnails_dispose (GObject *object)
{
	PpsSidebarThumbnails *sidebar_thumbnails = PPS_SIDEBAR_THUMBNAILS (object);

	g_clear_pointer (&sidebar_thumbnails->priv->loading_icons,
			 g_hash_table_destroy);

	G_OBJECT_CLASS (pps_sidebar_thumbnails_parent_class)->dispose (object);
}

static void
pps_sidebar_thumbnails_set_property (GObject      *object,
			       guint         prop_id,
			       const GValue *value,
			       GParamSpec   *pspec)
{
	PpsSidebarThumbnails *sidebar_thumbnails = PPS_SIDEBAR_THUMBNAILS (object);

	switch (prop_id)
	{
	case PROP_DOCUMENT_MODEL:
		pps_sidebar_thumbnails_set_model (PPS_SIDEBAR_PAGE (sidebar_thumbnails),
			PPS_DOCUMENT_MODEL (g_value_get_object (value)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
pps_sidebar_thumbnails_map (GtkWidget *widget)
{
	PpsSidebarThumbnails *sidebar;

	sidebar = PPS_SIDEBAR_THUMBNAILS (widget);

	GTK_WIDGET_CLASS (pps_sidebar_thumbnails_parent_class)->map (widget);

	adjustment_changed_cb (sidebar);
}

static void
pps_sidebar_check_reset_current_page (PpsSidebarThumbnails *sidebar)
{
	guint page;

	if (!sidebar->priv->model)
		return;

	page = pps_document_model_get_page (sidebar->priv->model);
	if (!pps_sidebar_thumbnails_page_is_in_visible_range (sidebar, page))
		pps_sidebar_thumbnails_set_current_page (sidebar, page);
}

GtkWidget *
pps_sidebar_thumbnails_new (void)
{
	return GTK_WIDGET (g_object_new (PPS_TYPE_SIDEBAR_THUMBNAILS, NULL));
}

static cairo_surface_t *
pps_sidebar_thumbnails_get_loading_icon (PpsSidebarThumbnails *sidebar_thumbnails,
					gint                 width,
					gint                 height)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
        cairo_surface_t *icon;
	gchar           *key;

	key = g_strdup_printf ("%dx%d", width, height);
	icon = g_hash_table_lookup (priv->loading_icons, key);
	if (!icon) {
		gboolean inverted_colors;
                gint device_scale = 1;

                device_scale = gtk_widget_get_scale_factor (GTK_WIDGET (sidebar_thumbnails));

		inverted_colors = pps_document_model_get_inverted_colors (priv->model);
                icon = pps_document_misc_render_loading_thumbnail_surface (GTK_WIDGET (sidebar_thumbnails),
                                                                          width * device_scale,
                                                                          height * device_scale,
                                                                          inverted_colors);
		g_hash_table_insert (priv->loading_icons, key, icon);
	} else {
		g_free (key);
	}

	return icon;
}

static void
cancel_running_jobs (PpsSidebarThumbnails *sidebar_thumbnails,
		     gint                 start_page,
		     gint                 end_page)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
	GtkTreePath *path;
	GtkTreeIter iter;
	gboolean result;

	g_assert (start_page <= end_page);

	path = gtk_tree_path_new_from_indices (start_page, -1);
	for (result = gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->list_store), &iter, path);
	     result && start_page <= end_page;
	     result = gtk_tree_model_iter_next (GTK_TREE_MODEL (priv->list_store), &iter), start_page ++) {
		PpsJobThumbnailCairo *job;
		gboolean thumbnail_set;

		gtk_tree_model_get (GTK_TREE_MODEL (priv->list_store),
				    &iter,
				    COLUMN_JOB, &job,
				    COLUMN_THUMBNAIL_SET, &thumbnail_set,
				    -1);

		if (thumbnail_set) {
			g_assert (job == NULL);
			continue;
		}

		if (job) {
			g_signal_handlers_disconnect_by_func (job, thumbnail_job_completed_callback, sidebar_thumbnails);
			pps_job_cancel (PPS_JOB (job));
			g_object_unref (job);
		}

		gtk_list_store_set (priv->list_store, &iter,
				    COLUMN_JOB, NULL,
				    COLUMN_THUMBNAIL_SET, FALSE,
				    -1);
	}
	gtk_tree_path_free (path);
}

static void
get_size_for_page (PpsSidebarThumbnails *sidebar_thumbnails,
                   gint                 page,
                   gint                *width_return,
                   gint                *height_return)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
        gdouble width, height;
        gint thumbnail_height, device_scale;

        device_scale = gtk_widget_get_scale_factor (GTK_WIDGET (sidebar_thumbnails));
        pps_document_get_page_size (priv->document, page, &width, &height);
        thumbnail_height = (int)(THUMBNAIL_WIDTH * height / width + 0.5);

        if (priv->rotation == 90 || priv->rotation == 270) {
                *width_return = thumbnail_height * device_scale;
                *height_return = THUMBNAIL_WIDTH * device_scale;
        } else {
                *width_return = THUMBNAIL_WIDTH * device_scale;
                *height_return = thumbnail_height * device_scale;
        }
}

static void
add_range (PpsSidebarThumbnails *sidebar_thumbnails,
	   gint                 start_page,
	   gint                 end_page)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
	GtkTreePath *path;
	GtkTreeIter iter;
	gboolean result;
	gint page = start_page;

	g_assert (start_page <= end_page);

	if (priv->blank_first_dual_mode)
		page--;

	path = gtk_tree_path_new_from_indices (start_page, -1);
	for (result = gtk_tree_model_get_iter (GTK_TREE_MODEL (priv->list_store), &iter, path);
	     result && page <= end_page;
	     result = gtk_tree_model_iter_next (GTK_TREE_MODEL (priv->list_store), &iter), page ++) {
		PpsJob *job;
		gboolean thumbnail_set;

		gtk_tree_model_get (GTK_TREE_MODEL (priv->list_store), &iter,
				    COLUMN_JOB, &job,
				    COLUMN_THUMBNAIL_SET, &thumbnail_set,
				    -1);

		if (job == NULL && !thumbnail_set) {
			gint thumbnail_width, thumbnail_height;
			get_size_for_page (sidebar_thumbnails, page, &thumbnail_width, &thumbnail_height);

			job = pps_job_thumbnail_cairo_new_with_target_size (priv->document,
									   page, priv->rotation,
									   thumbnail_width,
									   thumbnail_height);
			g_object_set_data_full (G_OBJECT (job), "tree_iter",
						gtk_tree_iter_copy (&iter),
						(GDestroyNotify) gtk_tree_iter_free);
			g_signal_connect (job, "finished",
					  G_CALLBACK (thumbnail_job_completed_callback),
					  sidebar_thumbnails);
			gtk_list_store_set (priv->list_store, &iter,
					    COLUMN_JOB, job,
					    -1);
			pps_job_scheduler_push_job (PPS_JOB (job), PPS_JOB_PRIORITY_HIGH);

			/* The queue and the list own a ref to the job now */
			g_object_unref (job);
		} else if (job) {
			g_object_unref (job);
		}
	}
	gtk_tree_path_free (path);
}

/* This modifies start */
static void
update_visible_range (PpsSidebarThumbnails *sidebar_thumbnails,
		      gint                 start_page,
		      gint                 end_page)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
	int old_start_page, old_end_page;
	int n_pages_in_visible_range;

	/* Preload before and after current visible scrolling range, the same amount of
	 * thumbs in it, to help prevent thumbnail creation happening in the user's sight.
	 * https://bugzilla.gnome.org/show_bug.cgi?id=342110#c15 */
	n_pages_in_visible_range = (end_page - start_page) + 1;
	start_page = MAX (0, start_page - n_pages_in_visible_range);
	end_page = MIN (priv->n_pages - 1, end_page + n_pages_in_visible_range);

	old_start_page = priv->start_page;
	old_end_page = priv->end_page;

	if (start_page == old_start_page &&
	    end_page == old_end_page)
		return;

	/* Clear the areas we no longer display */
	if (old_start_page >= 0 && old_start_page < start_page)
		cancel_running_jobs (sidebar_thumbnails, old_start_page, MIN (start_page - 1, old_end_page));

	if (old_end_page > 0 && old_end_page > end_page)
		cancel_running_jobs (sidebar_thumbnails, MAX (end_page + 1, old_start_page), old_end_page);

	add_range (sidebar_thumbnails, start_page, end_page);

	priv->start_page = start_page;
	priv->end_page = end_page;
}

static void
adjustment_changed_cb (PpsSidebarThumbnails *sidebar_thumbnails)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
	GtkTreePath *path = NULL;
	GtkTreePath *path2 = NULL;
	gdouble page_size;

	/* Widget is not currently visible */
	if (!gtk_widget_get_mapped (GTK_WIDGET (sidebar_thumbnails)))
		return;

	page_size = gtk_adjustment_get_page_size (priv->vadjustment);

	if (page_size == 0)
		return;

	if (priv->icon_view) {
		if (! gtk_widget_get_realized (priv->icon_view))
			return;
		if (! gtk_icon_view_get_visible_range (GTK_ICON_VIEW (priv->icon_view), &path, &path2))
			return;
	} else {
		return;
	}

	if (path && path2) {
		update_visible_range (sidebar_thumbnails,
				      gtk_tree_path_get_indices (path)[0],
				      gtk_tree_path_get_indices (path2)[0]);
	}

	gtk_tree_path_free (path);
	gtk_tree_path_free (path2);
}

static GdkTexture *
gdk_texture_new_for_surface (cairo_surface_t *surface)
{
  GdkTexture *texture;
  GBytes *bytes;

  g_return_val_if_fail (cairo_surface_get_type (surface) == CAIRO_SURFACE_TYPE_IMAGE, NULL);
  g_return_val_if_fail (cairo_image_surface_get_width (surface) > 0, NULL);
  g_return_val_if_fail (cairo_image_surface_get_height (surface) > 0, NULL);

  bytes = g_bytes_new_with_free_func (cairo_image_surface_get_data (surface),
                                      cairo_image_surface_get_height (surface)
                                      * cairo_image_surface_get_stride (surface),
                                      (GDestroyNotify) cairo_surface_destroy,
                                      cairo_surface_reference (surface));

  texture = gdk_memory_texture_new (cairo_image_surface_get_width (surface),
                                    cairo_image_surface_get_height (surface),
                                    GDK_MEMORY_DEFAULT,
                                    bytes,
                                    cairo_image_surface_get_stride (surface));

  g_bytes_unref (bytes);

  return texture;
}

static void
pps_sidebar_thumbnails_fill_model (PpsSidebarThumbnails *sidebar_thumbnails)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
	GtkTreeIter iter;
	int i;
	gint prpps_width = -1;
	gint prpps_height = -1;

	for (i = 0; i < sidebar_thumbnails->priv->n_pages; i++) {
		gchar     *page_label;
		gchar     *page_string;
		cairo_surface_t *loading_icon = NULL;
		GdkTexture *texture;
		gint       width, height;

		page_label = pps_document_get_page_label (priv->document, i);
		page_string = g_markup_printf_escaped ("<i>%s</i>", page_label);
		pps_thumbnails_size_cache_get_size (sidebar_thumbnails->priv->size_cache, i,
						  sidebar_thumbnails->priv->rotation,
						  &width, &height);
		if (!loading_icon || (width != prpps_width && height != prpps_height)) {
			loading_icon =
				pps_sidebar_thumbnails_get_loading_icon (sidebar_thumbnails,
									width, height);
		}

		prpps_width = width;
		prpps_height = height;

		texture = gdk_texture_new_for_surface (loading_icon);

		gtk_list_store_append (priv->list_store, &iter);
		gtk_list_store_set (priv->list_store, &iter,
				    COLUMN_PAGE_STRING, page_string,
				    COLUMN_SURFACE, texture,
				    COLUMN_THUMBNAIL_SET, FALSE,
				    -1);
		g_free (page_label);
		g_free (page_string);
		cairo_surface_destroy (loading_icon);
	}
}

static void
pps_sidebar_icon_selection_changed (GtkIconView         *icon_view,
				   PpsSidebarThumbnails *pps_sidebar_thumbnails)
{
	PpsSidebarThumbnailsPrivate *priv = pps_sidebar_thumbnails->priv;
	GtkTreePath *path;
	GList *selected;
	int page;

	selected = gtk_icon_view_get_selected_items (icon_view);
	if (selected == NULL)
		return;

	/* We don't handle or expect multiple selection. */
	g_assert (selected->next == NULL);

	path = selected->data;
	page = gtk_tree_path_get_indices (path)[0];

	if (priv->blank_first_dual_mode) {
		if (page == 0) {
			gtk_icon_view_unselect_path (icon_view, path);
			gtk_tree_path_free (path);
			g_list_free (selected);
			return;
		}
		page--;

	}

	gtk_tree_path_free (path);
	g_list_free (selected);

	pps_document_model_set_page (priv->model, page);
}

static void
pps_sidebar_thumbnails_device_scale_factor_changed_cb (PpsSidebarThumbnails *sidebar_thumbnails,
                                                      GParamSpec          *pspec)

{
        pps_sidebar_thumbnails_reload (sidebar_thumbnails);
}

static void
pps_sidebar_thumbnails_row_changed (GtkTreeModel *model,
                                   GtkTreePath  *path,
                                   GtkTreeIter  *iter,
                                   gpointer      data)
{
	guint signal_id;

	signal_id = GPOINTER_TO_UINT (data);

	/* PREVENT GtkIconView "row-changed" handler to be reached, as it will
	 * perform a full invalidate and relayout of all items, See bug:
	 * https://bugzilla.gnome.org/show_bug.cgi?id=691448#c9 */
	g_signal_stop_emission (model, signal_id, 0);
}

static void
pps_sidebar_thumbnails_init (PpsSidebarThumbnails *pps_sidebar_thumbnails)
{
	PpsSidebarThumbnailsPrivate *priv;
	guint signal_id;

	priv = pps_sidebar_thumbnails->priv = pps_sidebar_thumbnails_get_instance_private (pps_sidebar_thumbnails);

	gtk_widget_init_template (GTK_WIDGET (pps_sidebar_thumbnails));

	signal_id = g_signal_lookup ("row-changed", GTK_TYPE_TREE_MODEL);
	g_signal_connect (GTK_TREE_MODEL (priv->list_store), "row-changed",
			  G_CALLBACK (pps_sidebar_thumbnails_row_changed),
			  GUINT_TO_POINTER (signal_id));


	priv->vadjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (priv->swindow));
	g_signal_connect_data (priv->vadjustment, "value-changed",
			       G_CALLBACK (adjustment_changed_cb),
			       pps_sidebar_thumbnails, NULL,
			       G_CONNECT_SWAPPED | G_CONNECT_AFTER);

	g_signal_connect (pps_sidebar_thumbnails, "notify::scale-factor",
			  G_CALLBACK (pps_sidebar_thumbnails_device_scale_factor_changed_cb), NULL);
}

static void
pps_sidebar_thumbnails_set_current_page (PpsSidebarThumbnails *sidebar,
					gint                 page)
{
	GtkTreePath *path;

	if (sidebar->priv->blank_first_dual_mode)
		page++;

	path = gtk_tree_path_new_from_indices (page, -1);

	if (sidebar->priv->icon_view) {

		g_signal_handlers_block_by_func
			(sidebar->priv->icon_view,
			 G_CALLBACK (pps_sidebar_icon_selection_changed), sidebar);

		gtk_icon_view_select_path (GTK_ICON_VIEW (sidebar->priv->icon_view), path);

		g_signal_handlers_unblock_by_func
			(sidebar->priv->icon_view,
			 G_CALLBACK (pps_sidebar_icon_selection_changed), sidebar);

		gtk_icon_view_scroll_to_path (GTK_ICON_VIEW (sidebar->priv->icon_view), path, FALSE, 0.0, 0.0);
	}

	gtk_tree_path_free (path);
}

static void
page_changed_cb (PpsSidebarThumbnails *sidebar,
		 gint                 old_page,
		 gint                 new_page)
{
	pps_sidebar_thumbnails_set_current_page (sidebar, new_page);
}

static void
pps_sidebar_thumbnails_reload (PpsSidebarThumbnails *sidebar_thumbnails)
{
	PpsDocumentModel *model;

	if (sidebar_thumbnails->priv->loading_icons)
		g_hash_table_remove_all (sidebar_thumbnails->priv->loading_icons);

	if (sidebar_thumbnails->priv->document == NULL ||
	    sidebar_thumbnails->priv->n_pages <= 0)
		return;

	model = sidebar_thumbnails->priv->model;

	pps_sidebar_thumbnails_clear_model (sidebar_thumbnails);
	pps_sidebar_thumbnails_fill_model (sidebar_thumbnails);

	/* Trigger a redraw */
	sidebar_thumbnails->priv->start_page = -1;
	sidebar_thumbnails->priv->end_page = -1;
	pps_sidebar_thumbnails_set_current_page (sidebar_thumbnails,
						pps_document_model_get_page (model));
	g_idle_add_once ((GSourceOnceFunc)adjustment_changed_cb, sidebar_thumbnails);
}

static void
pps_sidebar_thumbnails_rotation_changed_cb (PpsDocumentModel     *model,
					   GParamSpec          *pspec,
					   PpsSidebarThumbnails *sidebar_thumbnails)
{
	gint rotation = pps_document_model_get_rotation (model);

	sidebar_thumbnails->priv->rotation = rotation;
	pps_sidebar_thumbnails_reload (sidebar_thumbnails);
}

static void
pps_sidebar_thumbnails_inverted_colors_changed_cb (PpsDocumentModel     *model,
						  GParamSpec          *pspec,
						  PpsSidebarThumbnails *sidebar_thumbnails)
{
	gboolean inverted_colors = pps_document_model_get_inverted_colors (model);

	sidebar_thumbnails->priv->inverted_colors = inverted_colors;
	pps_sidebar_thumbnails_reload (sidebar_thumbnails);
}

static void
thumbnail_job_completed_callback (PpsJobThumbnailCairo *job,
				  PpsSidebarThumbnails *sidebar_thumbnails)
{
        GtkWidget                  *widget = GTK_WIDGET (sidebar_thumbnails);
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
	GtkTreeIter                *iter;
        cairo_surface_t            *surface;
	GdkTexture                 *texture;
        gint                        device_scale;

        if (pps_job_is_failed (PPS_JOB (job)))
          return;

        device_scale = gtk_widget_get_scale_factor (widget);
        cairo_surface_set_device_scale (job->thumbnail_surface, device_scale, device_scale);

        surface = pps_document_misc_render_thumbnail_surface_with_frame (widget,
                                                                        job->thumbnail_surface,
                                                                        -1, -1);

	iter = (GtkTreeIter *) g_object_get_data (G_OBJECT (job), "tree_iter");
	if (priv->inverted_colors)
		pps_document_misc_invert_surface (surface);

	texture = gdk_texture_new_for_surface (surface);

	gtk_list_store_set (priv->list_store,
			    iter,
			    COLUMN_SURFACE, texture,
			    COLUMN_THUMBNAIL_SET, TRUE,
			    COLUMN_JOB, NULL,
			    -1);
        cairo_surface_destroy (surface);

	gtk_widget_queue_draw (priv->icon_view);
}

static void
pps_sidebar_thumbnails_document_changed_cb (PpsDocumentModel     *model,
					   GParamSpec          *pspec,
					   PpsSidebarThumbnails *sidebar_thumbnails)
{
	PpsDocument *document = pps_document_model_get_document (model);
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;

	if (pps_document_get_n_pages (document) <= 0 ||
	    !pps_document_check_dimensions (document)) {
		return;
	}

	priv->size_cache = pps_thumbnails_size_cache_get (document);
	priv->document = document;
	priv->n_pages = pps_document_get_n_pages (document);
	priv->rotation = pps_document_model_get_rotation (model);
	priv->inverted_colors = pps_document_model_get_inverted_colors (model);
	if (priv->loading_icons) {
                g_hash_table_remove_all (priv->loading_icons);
	} else {
                priv->loading_icons = g_hash_table_new_full (g_str_hash,
                                                             g_str_equal,
                                                             (GDestroyNotify)g_free,
                                                             (GDestroyNotify)cairo_surface_destroy);
	}

	pps_sidebar_thumbnails_clear_model (sidebar_thumbnails);
	pps_sidebar_thumbnails_fill_model (sidebar_thumbnails);

	/* Connect to the signal and trigger a fake callback */
	g_signal_connect_swapped (priv->model, "page-changed",
				  G_CALLBACK (page_changed_cb),
				  sidebar_thumbnails);
	g_signal_connect (priv->model, "notify::rotation",
			  G_CALLBACK (pps_sidebar_thumbnails_rotation_changed_cb),
			  sidebar_thumbnails);
	g_signal_connect (priv->model, "notify::inverted-colors",
			  G_CALLBACK (pps_sidebar_thumbnails_inverted_colors_changed_cb),
			  sidebar_thumbnails);
	g_signal_connect_data (priv->model, "notify::page-layout",
			       G_CALLBACK (check_toggle_blank_first_dual_mode),
			       sidebar_thumbnails,
			       NULL, G_CONNECT_SWAPPED | G_CONNECT_AFTER);
	g_signal_connect_data (priv->model, "notify::dual-odd-left",
			       G_CALLBACK (check_toggle_blank_first_dual_mode),
			       sidebar_thumbnails,
			       NULL, G_CONNECT_SWAPPED | G_CONNECT_AFTER);
	sidebar_thumbnails->priv->start_page = -1;
	sidebar_thumbnails->priv->end_page = -1;
	pps_sidebar_thumbnails_set_current_page (sidebar_thumbnails,
						pps_document_model_get_page (model));
	check_toggle_blank_first_dual_mode (sidebar_thumbnails);
	adjustment_changed_cb (sidebar_thumbnails);
}

static void
pps_sidebar_thumbnails_set_model (PpsSidebarPage   *sidebar_page,
				 PpsDocumentModel *model)
{
	PpsSidebarThumbnails *sidebar_thumbnails = PPS_SIDEBAR_THUMBNAILS (sidebar_page);
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;

	if (priv->model == model)
		return;

	priv->model = model;
	g_signal_connect (model, "notify::document",
			  G_CALLBACK (pps_sidebar_thumbnails_document_changed_cb),
			  sidebar_page);
}

static gboolean
pps_sidebar_thumbnails_clear_job (GtkTreeModel *model,
			         GtkTreePath *path,
			         GtkTreeIter *iter,
				 gpointer data)
{
	PpsJob *job;

	gtk_tree_model_get (model, iter, COLUMN_JOB, &job, -1);

	if (job != NULL) {
		pps_job_cancel (job);
		g_signal_handlers_disconnect_by_func (job, thumbnail_job_completed_callback, data);
		g_object_unref (job);
	}

	return FALSE;
}

static void
pps_sidebar_thumbnails_clear_model (PpsSidebarThumbnails *sidebar_thumbnails)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;

	gtk_tree_model_foreach (GTK_TREE_MODEL (priv->list_store), pps_sidebar_thumbnails_clear_job, sidebar_thumbnails);
	gtk_list_store_clear (priv->list_store);
}

static gboolean
pps_sidebar_thumbnails_support_document (PpsSidebarPage   *sidebar_page,
				        PpsDocument *document)
{
	return TRUE;
}

static void
pps_sidebar_thumbnails_page_iface_init (PpsSidebarPageInterface *iface)
{
	iface->support_document = pps_sidebar_thumbnails_support_document;
	iface->set_model = pps_sidebar_thumbnails_set_model;
}

static gboolean
iter_is_blank_thumbnail (GtkTreeModel *tree_model,
			 GtkTreeIter  *iter)
{
	GdkTexture *texture = NULL;
	PpsJob *job = NULL;
	gboolean thumbnail_set = FALSE;

	gtk_tree_model_get (tree_model, iter,
			    COLUMN_SURFACE, &texture,
			    COLUMN_THUMBNAIL_SET, &thumbnail_set,
			    COLUMN_JOB, &job, -1);

	/* The blank thumbnail item can be distinguished among all
	 * other items in the GtkIconView as it's the only one which
	 * has the COLUMN_SURFACE as NULL while COLUMN_THUMBNAIL_SET
	 * is set to TRUE. */
	return texture == NULL && job == NULL && thumbnail_set;
}

/* Returns the total horizontal(left+right) width of thumbnail frames.
 * As it was added in pps_document_misc_render_thumbnail_frame() */
static gint
pps_sidebar_thumbnails_frame_horizontal_width (PpsSidebarThumbnails *sidebar)
{
        GtkWidget *widget;
        GtkStyleContext *context;
        GtkBorder border = {0, };
        gint offset;

        widget = GTK_WIDGET (sidebar);
        context = gtk_widget_get_style_context (widget);

        gtk_style_context_save (context);

        gtk_style_context_add_class (context, "page-thumbnail");
        gtk_style_context_get_border (context, &border);
        offset = border.left + border.right;

        gtk_style_context_restore (context);

        return offset;
}

static void
pps_sidebar_thumbnails_get_column_widths (PpsSidebarThumbnails *sidebar,
					 gint *one_column_width,
					 gint *two_columns_width)
{
	PpsSidebarThumbnailsPrivate *priv;
	GtkIconView *icon_view;
	gint margin, column_spacing, item_padding, thumbnail_width;
	static gint frame_horizontal_width;

	priv = sidebar->priv;
	icon_view = GTK_ICON_VIEW (priv->icon_view);

	pps_thumbnails_size_cache_get_size (priv->size_cache, 0,
					   priv->rotation,
					   &thumbnail_width, NULL);

	margin = gtk_icon_view_get_margin (icon_view);
	column_spacing = gtk_icon_view_get_column_spacing (icon_view);
	item_padding = gtk_icon_view_get_item_padding (icon_view);
	frame_horizontal_width = pps_sidebar_thumbnails_frame_horizontal_width (sidebar);

	if (one_column_width) {
		*one_column_width = 2 * margin +
				    2 * item_padding +
				    1 * frame_horizontal_width +
				    1 * thumbnail_width +
				    column_spacing;
	}
	if (two_columns_width) {
		*two_columns_width = 2 * margin +
				     4 * item_padding +
				     2 * frame_horizontal_width +
				     2 * thumbnail_width +
				     column_spacing;
	}
}

static void
pps_sidebar_thumbnails_set_sidebar_width (PpsSidebarThumbnails *sidebar,
					 gint sidebar_width)
{
	g_return_if_fail (sidebar != NULL);

	if (sidebar_width <= 0)
		return;

	gtk_widget_set_size_request (GTK_WIDGET (sidebar), sidebar_width, -1);

	g_idle_add_once ((GSourceOnceFunc)pps_sidebar_check_reset_current_page, sidebar);
}

/* If thumbnail sidebar is currently being displayed then
 * it resizes it to be of one column width layout */
static void
pps_sidebar_thumbnails_to_one_column (PpsSidebarThumbnails *sidebar)
{
	gint one_column_width;

	pps_sidebar_thumbnails_get_column_widths (sidebar, &one_column_width,
						 NULL);
	pps_sidebar_thumbnails_set_sidebar_width (sidebar, one_column_width);
}

/* If thumbnail sidebar is currently being displayed then
 * it resizes it to be of two columns width layout */
static void
pps_sidebar_thumbnails_to_two_columns (PpsSidebarThumbnails *sidebar)
{
	gint two_columns_width;

	pps_sidebar_thumbnails_get_column_widths (sidebar, NULL,
						 &two_columns_width);
	pps_sidebar_thumbnails_set_sidebar_width (sidebar, two_columns_width);
}

/* This function checks whether the conditions to insert a blank first item
 * in dual mode are met and activates/deactivates the mode accordingly (that
 * is setting priv->blank_first_dual_mode on/off).
 *
 * Aditionally, we make sure that the minimum size of the sidebar fits the
 * current mode.
 */
static void
check_toggle_blank_first_dual_mode (PpsSidebarThumbnails *sidebar_thumbnails)
{
	PpsSidebarThumbnailsPrivate *priv = sidebar_thumbnails->priv;
	GtkTreeModel *tree_model;
	GtkTreeIter first;
	gboolean odd_pages_left, dual_mode;

	dual_mode = pps_document_model_get_page_layout (priv->model) == PPS_PAGE_LAYOUT_DUAL;
	odd_pages_left = pps_document_model_get_dual_page_odd_pages_left (priv->model);

	if (dual_mode && !odd_pages_left) {
		tree_model = GTK_TREE_MODEL (priv->list_store);
		if (!gtk_tree_model_get_iter_first (tree_model, &first))
			return;

		priv->blank_first_dual_mode = TRUE;
		if (!iter_is_blank_thumbnail (tree_model, &first))
			gtk_list_store_insert_with_values (priv->list_store, &first, 0,
							   COLUMN_SURFACE, NULL,
							   COLUMN_THUMBNAIL_SET, TRUE,
							   COLUMN_JOB, NULL,
							   -1);
	} else {
		tree_model = GTK_TREE_MODEL (priv->list_store);
		if (!gtk_tree_model_get_iter_first (tree_model, &first))
			return;

		priv->blank_first_dual_mode = FALSE;
		if (iter_is_blank_thumbnail (tree_model, &first))
			gtk_list_store_remove (priv->list_store, &first);
	}

	if (dual_mode)
		pps_sidebar_thumbnails_to_two_columns (sidebar_thumbnails);
	else
		pps_sidebar_thumbnails_to_one_column (sidebar_thumbnails);
	pps_sidebar_check_reset_current_page (sidebar_thumbnails);
}

static void
pps_sidebar_thumbnails_class_init (PpsSidebarThumbnailsClass *pps_sidebar_thumbnails_class)
{
	GObjectClass *g_object_class;
	GtkWidgetClass *widget_class;

	g_object_class = G_OBJECT_CLASS (pps_sidebar_thumbnails_class);
	widget_class = GTK_WIDGET_CLASS (pps_sidebar_thumbnails_class);

	g_object_class->dispose = pps_sidebar_thumbnails_dispose;
	g_object_class->set_property = pps_sidebar_thumbnails_set_property;
	widget_class->map = pps_sidebar_thumbnails_map;

	gtk_widget_class_set_css_name (widget_class, "evsidebarthumbnails");

	gtk_widget_class_set_template_from_resource (widget_class,
				"/org/gnome/papers/ui/sidebar-thumbnails.ui");
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarThumbnails, icon_view);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarThumbnails, list_store);
	gtk_widget_class_bind_template_child_private (widget_class, PpsSidebarThumbnails, swindow);

	gtk_widget_class_bind_template_callback (widget_class, pps_sidebar_icon_selection_changed);

	g_object_class_override_property (g_object_class, PROP_DOCUMENT_MODEL, "document-model");
}
