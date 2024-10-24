/* pps-bookmarks.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2010 Carlos Garcia Campos  <carlosgc@gnome.org>
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

#include "config.h"

#include <string.h>

#include "pps-bookmarks.h"

enum {
	PROP_0,
	PROP_METADATA
};

enum {
	CHANGED,
	N_SIGNALS
};

struct _PpsBookmarks {
	GObject base;

	PpsMetadata *metadata;
	GList *items;
};

G_DEFINE_TYPE (PpsBookmarks, pps_bookmarks, G_TYPE_OBJECT)

static guint signals[N_SIGNALS];

static gint
pps_bookmark_compare (PpsBookmark *a,
                      PpsBookmark *b)
{
	if (a->page < b->page)
		return -1;
	if (a->page > b->page)
		return 1;
	return 0;
}

G_DEFINE_BOXED_TYPE (PpsBookmark, pps_bookmark, pps_bookmark_copy, pps_bookmark_free)

PpsBookmark *
pps_bookmark_new (gint page, const gchar *title)
{
	PpsBookmark *bm = g_slice_new0 (PpsBookmark);

	g_assert (bm != NULL);

	bm->page = page;
	bm->title = g_strdup (title);

	return bm;
}

PpsBookmark *
pps_bookmark_copy (const PpsBookmark *bm)
{
	PpsBookmark *new_bm;

	if (G_UNLIKELY (!bm))
		return NULL;

	new_bm = g_slice_new (PpsBookmark);

	g_assert (new_bm != NULL);

	new_bm->page = bm->page;
	new_bm->title = g_strdup (bm->title);

	return new_bm;
}

void
pps_bookmark_free (PpsBookmark *bm)
{
	if (G_UNLIKELY (!bm))
		return;

	g_free (bm->title);
	g_slice_free (PpsBookmark, bm);
}

guint
pps_bookmark_get_page (const PpsBookmark *bm)
{
	return bm->page;
}

const gchar *
pps_bookmark_get_title (const PpsBookmark *bm)
{
	return bm->title;
}

static void
pps_bookmarks_finalize (GObject *object)
{
	PpsBookmarks *bookmarks = PPS_BOOKMARKS (object);

	if (bookmarks->items) {
		g_list_free_full (bookmarks->items, (GDestroyNotify) pps_bookmark_free);
		bookmarks->items = NULL;
	}

	g_clear_object (&bookmarks->metadata);

	G_OBJECT_CLASS (pps_bookmarks_parent_class)->finalize (object);
}

static void
pps_bookmarks_init (PpsBookmarks *bookmarks)
{
}

static void
pps_bookmarks_set_property (GObject *object,
                            guint prop_id,
                            const GValue *value,
                            GParamSpec *pspec)
{
	PpsBookmarks *bookmarks = PPS_BOOKMARKS (object);

	switch (prop_id) {
	case PROP_METADATA:
		bookmarks->metadata = (PpsMetadata *) g_value_dup_object (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_bookmarks_constructed (GObject *object)
{
	PpsBookmarks *bookmarks = PPS_BOOKMARKS (object);
	const gchar *bm_list_str;
	GVariant *bm_list;
	GVariantIter iter;
	GVariant *child;
	GError *error = NULL;

	if (!pps_metadata_get_string (bookmarks->metadata, "bookmarks", &bm_list_str))
		return;

	if (!bm_list_str || bm_list_str[0] == '\0')
		return;

	bm_list = g_variant_parse ((const GVariantType *) "a(us)",
	                           bm_list_str, NULL, NULL,
	                           &error);
	if (!bm_list) {
		g_warning ("Error getting bookmarks: %s\n", error->message);
		g_error_free (error);

		return;
	}

	g_variant_iter_init (&iter, bm_list);
	while ((child = g_variant_iter_next_value (&iter))) {
		PpsBookmark *bm = g_slice_new (PpsBookmark);

		g_variant_get (child, "(us)", &bm->page, &bm->title);
		if (bm->title && strlen (bm->title) > 0)
			bookmarks->items = g_list_prepend (bookmarks->items, bm);
		g_variant_unref (child);
	}
	g_variant_unref (bm_list);

	bookmarks->items = g_list_reverse (bookmarks->items);
}

static void
pps_bookmarks_class_init (PpsBookmarksClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->set_property = pps_bookmarks_set_property;
	gobject_class->finalize = pps_bookmarks_finalize;
	gobject_class->constructed = pps_bookmarks_constructed;

	g_object_class_install_property (gobject_class,
	                                 PROP_METADATA,
	                                 g_param_spec_object ("metadata",
	                                                      "Metadata",
	                                                      "The document metadata",
	                                                      PPS_TYPE_METADATA,
	                                                      G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE |
	                                                          G_PARAM_STATIC_STRINGS));
	/* Signals */
	signals[CHANGED] =
	    g_signal_new ("changed",
	                  PPS_TYPE_BOOKMARKS,
	                  G_SIGNAL_RUN_LAST,
	                  0,
	                  NULL, NULL,
	                  g_cclosure_marshal_VOID__VOID,
	                  G_TYPE_NONE, 0);
}

PpsBookmarks *
pps_bookmarks_new (PpsMetadata *metadata)
{
	g_return_val_if_fail (PPS_IS_METADATA (metadata), NULL);

	return PPS_BOOKMARKS (g_object_new (PPS_TYPE_BOOKMARKS,
	                                    "metadata", metadata, NULL));
}

static void
pps_bookmarks_save (PpsBookmarks *bookmarks)
{
	GList *l;
	GVariantBuilder builder;
	GVariant *bm_list;
	gchar *bm_list_str;

	if (!bookmarks->items) {
		pps_metadata_set_string (bookmarks->metadata, "bookmarks", "");
		return;
	}

	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);
	for (l = bookmarks->items; l; l = g_list_next (l)) {
		PpsBookmark *bm = (PpsBookmark *) l->data;

		g_variant_builder_add (&builder, "(u&s)", bm->page, bm->title);
	}
	bm_list = g_variant_builder_end (&builder);

	bm_list_str = g_variant_print (bm_list, FALSE);
	g_variant_unref (bm_list);
	pps_metadata_set_string (bookmarks->metadata, "bookmarks", bm_list_str);
	g_free (bm_list_str);
}

/**
 * pps_bookmarks_get_bookmarks:
 * @bookmarks: A #PpsBookmarks
 *
 * Returns: (transfer container) (element-type PpsBookmark): List of #PpsBookmark
 */
GList *
pps_bookmarks_get_bookmarks (PpsBookmarks *bookmarks)
{
	g_return_val_if_fail (PPS_IS_BOOKMARKS (bookmarks), NULL);

	return g_list_copy (bookmarks->items);
}

gboolean
pps_bookmarks_has_bookmarks (PpsBookmarks *bookmarks)
{
	g_return_val_if_fail (PPS_IS_BOOKMARKS (bookmarks), FALSE);

	return bookmarks->items != NULL;
}

void
pps_bookmarks_add (PpsBookmarks *bookmarks,
                   PpsBookmark *bookmark)
{
	PpsBookmark *bm;

	g_return_if_fail (PPS_IS_BOOKMARKS (bookmarks));
	g_return_if_fail (bookmark->title != NULL);

	if (g_list_find_custom (bookmarks->items, bookmark, (GCompareFunc) pps_bookmark_compare))
		return;

	bm = g_slice_new (PpsBookmark);
	*bm = *bookmark;
	bookmarks->items = g_list_append (bookmarks->items, bm);
	g_signal_emit (bookmarks, signals[CHANGED], 0);
	pps_bookmarks_save (bookmarks);
}

void
pps_bookmarks_delete (PpsBookmarks *bookmarks,
                      PpsBookmark *bookmark)
{
	GList *bm_link;

	g_return_if_fail (PPS_IS_BOOKMARKS (bookmarks));

	bm_link = g_list_find_custom (bookmarks->items, bookmark, (GCompareFunc) pps_bookmark_compare);
	if (!bm_link)
		return;

	bookmarks->items = g_list_delete_link (bookmarks->items, bm_link);
	g_signal_emit (bookmarks, signals[CHANGED], 0);
	pps_bookmarks_save (bookmarks);
}

void
pps_bookmarks_update (PpsBookmarks *bookmarks,
                      PpsBookmark *bookmark)
{
	GList *bm_link;
	PpsBookmark *bm;

	g_return_if_fail (PPS_IS_BOOKMARKS (bookmarks));
	g_return_if_fail (bookmark->title != NULL);

	bm_link = g_list_find_custom (bookmarks->items, bookmark, (GCompareFunc) pps_bookmark_compare);
	if (!bm_link)
		return;

	bm = (PpsBookmark *) bm_link->data;

	if (strcmp (bookmark->title, bm->title) == 0)
		return;

	g_free (bm->title);
	*bm = *bookmark;
	g_signal_emit (bookmarks, signals[CHANGED], 0);
	pps_bookmarks_save (bookmarks);
}
