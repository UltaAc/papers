/*
 *  Copyright (C) 2009 Carlos Garcia Campos
 *  Copyright (C) 2004 Marco Pesenti Gritti
 *  Copyright © 2018 Christian Persch
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

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "pps-document.h"
#include "pps-document-misc.h"

enum {
	PROP_0,
	PROP_MODIFIED
};

typedef struct _PpsPageSize
{
	gdouble width;
	gdouble height;
} PpsPageSize;

struct _PpsDocumentPrivate
{
	gchar          *uri;
	guint64         file_size;

	gboolean        cache_loaded;
	gboolean        modified;

	gboolean        uniform;
	gdouble         uniform_width;
	gdouble         uniform_height;

	gdouble         max_width;
	gdouble         max_height;
	gdouble         min_width;
	gdouble         min_height;
	gint            max_label;

	gchar         **page_labels;
	PpsPageSize     *page_sizes;
};

static guint64         _pps_document_get_size        (const char *uri);
static gchar          *_pps_document_get_page_label  (PpsDocument *document,
						     PpsPage     *page);

static GMutex pps_doc_mutex;
static GMutex pps_fc_mutex;

typedef struct _PpsDocumentPrivate PpsDocumentPrivate;

#define GET_PRIVATE(o) pps_document_get_instance_private (o)

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (PpsDocument, pps_document, G_TYPE_OBJECT)

GQuark
pps_document_error_quark (void)
{
  static GQuark q = 0;
  if (q == 0)
    q = g_quark_from_static_string ("pps-document-error-quark");

  return q;
}

static PpsPage *
pps_document_impl_get_page (PpsDocument *document,
			   gint        index)
{
	return pps_page_new (index);
}

static PpsDocumentInfo *
pps_document_impl_get_info (PpsDocument *document)
{
	return pps_document_info_new ();
}

static void
pps_document_finalize (GObject *object)
{
	PpsDocument *document = PPS_DOCUMENT (object);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	g_clear_pointer (&priv->uri, g_free);
	g_clear_pointer (&priv->page_sizes, g_free);
	g_clear_pointer (&priv->page_labels, g_strfreev);

	G_OBJECT_CLASS (pps_document_parent_class)->finalize (object);
}

static void
pps_document_set_property (GObject      *object,
			  guint         prop_id,
			  const GValue *value,
			  GParamSpec   *pspec)
{
	switch (prop_id) {
	case PROP_MODIFIED:
		pps_document_set_modified (PPS_DOCUMENT (object),
					  g_value_get_boolean (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
pps_document_get_property (GObject     *object,
			  guint        prop_id,
			  GValue      *value,
			  GParamSpec  *pspec)
{
	PpsDocument *document = PPS_DOCUMENT (object);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	switch (prop_id) {
	case PROP_MODIFIED:
		g_value_set_boolean (value, priv->modified);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
pps_document_init (PpsDocument *document)
{
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	/* Assume all pages are the same size until proven otherwise */
	priv->uniform = TRUE;
}

static void
pps_document_class_init (PpsDocumentClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	klass->get_page = pps_document_impl_get_page;
	klass->get_info = pps_document_impl_get_info;
	klass->get_backend_info = NULL;

	g_object_class->get_property = pps_document_get_property;
	g_object_class->set_property = pps_document_set_property;
	g_object_class->finalize = pps_document_finalize;

	g_object_class_install_property (g_object_class,
					 PROP_MODIFIED,
					 g_param_spec_boolean ("modified",
							       "Is modified",
							       "Whether the document has been modified",
							       FALSE,
							       G_PARAM_READWRITE |
							       G_PARAM_STATIC_STRINGS));
}

/**
 * pps_document_get_modified:
 * @document: an #PpsDocument
 *
 * Returns: %TRUE iff the document has been modified.
 *
 * You can monitor changes to the modification state by connecting to the
 * notify::modified signal on @document.
 *
 * Since: 3.28
 */
gboolean
pps_document_get_modified (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), FALSE);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	return priv->modified;
}

/**
 * pps_document_set_modified:
 * @document: an #PpsDocument
 * @modified: a boolean value to set the document as modified or not.
 *
 * Set the @document modification state as @modified.
 *
 * Since: 3.28
 */
void
pps_document_set_modified (PpsDocument *document,
			  gboolean    modified)
{
	g_return_if_fail (PPS_IS_DOCUMENT (document));
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	if (priv->modified != modified) {
		priv->modified = modified;
		g_object_notify (G_OBJECT (document), "modified");
	}
}

void
pps_document_doc_mutex_lock (void)
{
	g_mutex_lock (&pps_doc_mutex);
}

void
pps_document_doc_mutex_unlock (void)
{
	g_mutex_unlock (&pps_doc_mutex);
}

gboolean
pps_document_doc_mutex_trylock (void)
{
	return g_mutex_trylock (&pps_doc_mutex);
}

void
pps_document_fc_mutex_lock (void)
{
	g_mutex_lock (&pps_fc_mutex);
}

void
pps_document_fc_mutex_unlock (void)
{
	g_mutex_unlock (&pps_fc_mutex);
}

gboolean
pps_document_fc_mutex_trylock (void)
{
	return g_mutex_trylock (&pps_fc_mutex);
}

static void
pps_document_setup_cache (PpsDocument *document)
{
        PpsDocumentPrivate *priv = GET_PRIVATE (document);
        gboolean custom_page_labels = FALSE;
	gint n_pages = pps_document_get_n_pages (document);
        gint i;

	/* ensure that no component relies on a loaded
	 * cache while it is (re-)generated
	 */
	priv->cache_loaded = FALSE;

        for (i = 0; i < n_pages; i++) {
                PpsPage     *page = pps_document_get_page (document, i);
                gdouble      page_width = 0;
                gdouble      page_height = 0;
                PpsPageSize *page_size;
                gchar       *page_label;

                pps_document_get_page_size (document, i, &page_width, &page_height);

                if (i == 0) {
                        priv->uniform_width = page_width;
                        priv->uniform_height = page_height;
                        priv->max_width = priv->uniform_width;
                        priv->max_height = priv->uniform_height;
                        priv->min_width = priv->uniform_width;
                        priv->min_height = priv->uniform_height;
                } else if (priv->uniform &&
                            (priv->uniform_width != page_width ||
                            priv->uniform_height != page_height)) {
                        /* It's a different page size.  Backfill the array. */
                        int j;

                        priv->page_sizes = g_new0 (PpsPageSize, n_pages);

                        for (j = 0; j < i; j++) {
                                page_size = &(priv->page_sizes[j]);
                                page_size->width = priv->uniform_width;
                                page_size->height = priv->uniform_height;
                        }
                        priv->uniform = FALSE;
                }
                if (!priv->uniform) {
                        page_size = &(priv->page_sizes[i]);

                        page_size->width = page_width;
                        page_size->height = page_height;

                        if (page_width > priv->max_width)
                                priv->max_width = page_width;
                        if (page_width < priv->min_width)
                                priv->min_width = page_width;

                        if (page_height > priv->max_height)
                                priv->max_height = page_height;
                        if (page_height < priv->min_height)
                                priv->min_height = page_height;
                }

                page_label = _pps_document_get_page_label (document, page);
                if (page_label) {
                        if (!priv->page_labels)
                                priv->page_labels = g_new0 (gchar *, n_pages + 1);

                        if (!custom_page_labels) {
                                gchar *real_page_label;

                                real_page_label = g_strdup_printf ("%d", i + 1);
                                custom_page_labels = g_strcmp0 (real_page_label, page_label) != 0;
                                g_free (real_page_label);
                        }

                        priv->page_labels[i] = page_label;
                        priv->max_label = MAX (priv->max_label,
                                                g_utf8_strlen (page_label, 256));
                }

                g_object_unref (page);
        }

	if (!custom_page_labels)
		g_clear_pointer (&priv->page_labels, g_strfreev);

        /* Cache some info about the document to avoid
         * going to the backends since it requires locks
         */
	priv->cache_loaded = TRUE;
}

static void
pps_document_ensure_cache (PpsDocument *document)
{
        PpsDocumentPrivate *priv = GET_PRIVATE (document);

	if (!priv->cache_loaded) {
		g_mutex_lock (&pps_doc_mutex);
		pps_document_setup_cache (document);
		g_mutex_unlock (&pps_doc_mutex);
	}
}

/**
 * pps_document_load_full:
 * @document: a #PpsDocument
 * @uri: the document's URI
 * @flags: flags from #PpsDocumentLoadFlags
 * @error: a #GError location to store an error, or %NULL
 *
 * Loads @document from @uri.
 *
 * On failure, %FALSE is returned and @error is filled in.
 * If the document is encrypted, PPS_DEFINE_ERROR_ENCRYPTED is returned.
 * If the backend cannot load the specific document, PPS_DOCUMENT_ERROR_INVALID
 * is returned. Other errors are possible too, depending on the backend
 * used to load the document and the URI, e.g. #GIOError, #GFileError, and
 * #GConvertError.
 *
 * Returns: %TRUE on success, or %FALSE on failure.
 */
gboolean
pps_document_load_full (PpsDocument           *document,
		       const char           *uri,
		       PpsDocumentLoadFlags   flags,
		       GError              **error)
{
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);
	gboolean retval;
	GError *err = NULL;
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	retval = klass->load (document, uri, &err);
	if (!retval) {
		if (err) {
			g_propagate_error (error, err);
		} else {
			g_warning ("%s::PpsDocument::load returned FALSE but did not fill in @error; fix the backend!\n",
				   G_OBJECT_TYPE_NAME (document));

			/* So upper layers don't crash */
			g_set_error_literal (error,
					     PPS_DOCUMENT_ERROR,
					     PPS_DOCUMENT_ERROR_INVALID,
					     "Internal error in backend");
		}
	} else {
		if (!(flags & PPS_DOCUMENT_LOAD_FLAG_NO_CACHE))
			pps_document_setup_cache (document);
		priv->uri = g_strdup (uri);
		priv->file_size = _pps_document_get_size (uri);
        }

	return retval;
}

/**
 * pps_document_load:
 * @document: a #PpsDocument
 * @uri: the document's URI
 * @error: a #GError location to store an error, or %NULL
 *
 * Loads @document from @uri.
 *
 * On failure, %FALSE is returned and @error is filled in.
 * If the document is encrypted, PPS_DEFINE_ERROR_ENCRYPTED is returned.
 * If the backend cannot load the specific document, PPS_DOCUMENT_ERROR_INVALID
 * is returned. If the backend does not support the format for the document's
 * contents, PPS_DOCUMENT_ERROR_UNSUPPORTED_CONTENT is returned. Other errors
 * are possible too, depending on the backend used to load the document and
 * the URI, e.g. #GIOError, #GFileError, and #GConvertError.
 *
 * Returns: %TRUE on success, or %FALSE on failure.
 */
gboolean
pps_document_load (PpsDocument  *document,
		  const char  *uri,
		  GError     **error)
{
	return pps_document_load_full (document, uri,
				      PPS_DOCUMENT_LOAD_FLAG_NONE, error);
}

/**
 * pps_document_load_fd:
 * @document: a #PpsDocument
 * @fd: a file descriptor
 * @flags: flags from #PpsDocumentLoadFlags
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @error: (allow-none): a #GError location to store an error, or %NULL
 *
 * Synchronously loads the document from @fd, which must refer to
 * a regular file.
 *
 * Note that this function takes ownership of @fd; you must not ever
 * operate on it again. It will be closed automatically if the document
 * is destroyed, or if this function returns %NULL.
 *
 * See pps_document_load() for more information.
 *
 * Returns: %TRUE if loading succeeded, or %FALSE on error with @error filled in
 *
 * Since: 42.0
 */
gboolean
pps_document_load_fd (PpsDocument         *document,
                     int                 fd,
                     PpsDocumentLoadFlags flags,
                     GCancellable       *cancellable,
                     GError            **error)
{
        PpsDocumentClass *klass;
        struct stat statbuf;
        int fd_flags;

        g_return_val_if_fail (PPS_IS_DOCUMENT (document), FALSE);
        g_return_val_if_fail (fd != -1, FALSE);
        g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), FALSE);
        g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

        klass = PPS_DOCUMENT_GET_CLASS (document);
        if (!klass->load_fd) {
                g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                                     "Backend does not support loading from file descriptor");
                close (fd);
                return FALSE;
        }

        if (fstat(fd, &statbuf) == -1 ||
            (fd_flags = fcntl (fd, F_GETFL, &flags)) == -1) {
                int errsv = errno;
                g_set_error_literal (error, G_FILE_ERROR,
                                     g_file_error_from_errno (errsv),
                                     g_strerror (errsv));
                close (fd);
                return FALSE;
        }

        if (!S_ISREG(statbuf.st_mode)) {
                g_set_error_literal (error, G_FILE_ERROR, G_FILE_ERROR_BADF,
                                     "Not a regular file.");
                close (fd);
                return FALSE;
        }

        switch (fd_flags & O_ACCMODE) {
        case O_RDONLY:
        case O_RDWR:
                break;
        case O_WRONLY:
        default:
                g_set_error_literal (error, G_FILE_ERROR, G_FILE_ERROR_BADF,
                                     "Not a readable file descriptor.");
                close (fd);
                return FALSE;
        }

        if (!klass->load_fd (document, fd, flags, cancellable, error))
                return FALSE;

        if (!(flags & PPS_DOCUMENT_LOAD_FLAG_NO_CACHE))
                pps_document_setup_cache (document);

        return TRUE;
}

/**
 * pps_document_save:
 * @document: a #PpsDocument
 * @uri: the target URI
 * @error: a #GError location to store an error, or %NULL
 *
 * Saves @document to @uri.
 *
 * Returns: %TRUE on success, or %FALSE on error with @error filled in
 */
gboolean
pps_document_save (PpsDocument  *document,
		  const char  *uri,
		  GError     **error)
{
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);

	return klass->save (document, uri, error);
}

/**
 * pps_document_get_page:
 * @document: a #PpsDocument
 * @index: index of page
 *
 * Returns: (transfer full): Newly created #PpsPage for the given index.
 */
PpsPage *
pps_document_get_page (PpsDocument *document,
		      gint        index)
{
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);

	return klass->get_page (document, index);
}

static guint64
_pps_document_get_size (const char  *uri)
{
	GFile  *file = g_file_new_for_uri (uri);
	guint64 size = 0;

	GFileInfo *info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_SIZE,
                                             G_FILE_QUERY_INFO_NONE, NULL, NULL);
	if (info) {
		size = g_file_info_get_size (info);

		g_object_unref (info);
	}

	g_object_unref (file);

	return size;
}

gint
pps_document_get_n_pages (PpsDocument  *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), 0);

	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);

	return klass->get_n_pages (document);
}

/**
 * pps_document_get_page_size:
 * @document: a #PpsDocument
 * @page_index: index of page
 * @width: (out) (allow-none): return location for the width of the page, or %NULL
 * @height: (out) (allow-none): return location for the height of the page, or %NULL
 */
void
pps_document_get_page_size (PpsDocument *document,
			    gint         page_index,
			    double      *width,
			    double      *height)
{
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);
	PpsDocumentPrivate *priv;

	g_return_if_fail (PPS_IS_DOCUMENT (document));
	priv = GET_PRIVATE (document);
	g_return_if_fail (page_index >= 0 || page_index < pps_document_get_n_pages (document));

	if (priv->cache_loaded) {
		if (width)
			*width = priv->uniform ?
				priv->uniform_width :
				priv->page_sizes[page_index].width;
		if (height)
			*height = priv->uniform ?
				priv->uniform_height :
				priv->page_sizes[page_index].height;
	} else {
		PpsPage *page;

		g_mutex_lock (&pps_doc_mutex);
		page = pps_document_get_page (document, page_index);
		klass->get_page_size (document, page, width, height);
		g_object_unref (page);
		g_mutex_unlock (&pps_doc_mutex);
	}
}

static gchar *
_pps_document_get_page_label (PpsDocument *document,
			     PpsPage     *page)
{
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);

	return klass->get_page_label ?
		klass->get_page_label (document, page) : NULL;
}

gchar *
pps_document_get_page_label (PpsDocument *document,
			    gint        page_index)
{
	PpsDocumentPrivate *priv;
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), NULL);
	priv = GET_PRIVATE (document);
	g_return_val_if_fail (page_index >= 0 || page_index < pps_document_get_n_pages (document), NULL);

	if (!priv->cache_loaded) {
		PpsPage *page;
		gchar *page_label;

		g_mutex_lock (&pps_doc_mutex);
		page = pps_document_get_page (document, page_index);
		page_label = _pps_document_get_page_label (document, page);
		g_object_unref (page);
		g_mutex_unlock (&pps_doc_mutex);

		return page_label ? page_label : g_strdup_printf ("%d", page_index + 1);
	}

	return (priv->page_labels && priv->page_labels[page_index]) ?
		g_strdup (priv->page_labels[page_index]) :
		g_strdup_printf ("%d", page_index + 1);
}

/**
 * pps_document_get_info:
 * @document: a #PpsDocument
 *
 * Returns the #PpsDocumentInfo for the document.
 *
 * Returns: (transfer none): a #PpsDocumentInfo
 */
PpsDocumentInfo *
pps_document_get_info (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), NULL);
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);

	return klass->get_info (document);
}

gboolean
pps_document_get_backend_info (PpsDocument *document, PpsDocumentBackendInfo *info)
{
	PpsDocumentClass *klass;

	g_return_val_if_fail (PPS_IS_DOCUMENT (document), FALSE);

	klass = PPS_DOCUMENT_GET_CLASS (document);
	if (klass->get_backend_info == NULL)
		return FALSE;

	return klass->get_backend_info (document, info);
}

cairo_surface_t *
pps_document_render (PpsDocument      *document,
		    PpsRenderContext *rc)
{
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);

	return klass->render (document, rc);
}

static GdkPixbuf *
_pps_document_get_thumbnail (PpsDocument      *document,
			    PpsRenderContext *rc)
{
	cairo_surface_t *surface;
	GdkPixbuf       *pixbuf = NULL;

	surface = pps_document_render (document, rc);
	if (surface != NULL) {
		pixbuf = pps_document_misc_pixbuf_from_surface (surface);
		cairo_surface_destroy (surface);
	}

	return pixbuf;
}

/**
 * pps_document_get_thumbnail:
 * @document: an #PpsDocument
 * @rc: an #PpsRenderContext
 *
 * Returns: (transfer full): a #GdkPixbuf
 */
GdkPixbuf *
pps_document_get_thumbnail (PpsDocument      *document,
			   PpsRenderContext *rc)
{
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);

	if (klass->get_thumbnail)
		return klass->get_thumbnail (document, rc);

	return _pps_document_get_thumbnail (document, rc);
}

/**
 * pps_document_get_thumbnail_surface:
 * @document: an #PpsDocument
 * @rc: an #PpsRenderContext
 *
 * Returns: (transfer full): a #cairo_surface_t
 *
 * Since: 3.14
 */
cairo_surface_t *
pps_document_get_thumbnail_surface (PpsDocument      *document,
				   PpsRenderContext *rc)
{
	PpsDocumentClass *klass = PPS_DOCUMENT_GET_CLASS (document);

	if (klass->get_thumbnail_surface)
		return klass->get_thumbnail_surface (document, rc);

	return pps_document_render (document, rc);
}


const gchar *
pps_document_get_uri (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), NULL);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	return priv->uri;
}

const gchar *
pps_document_get_title (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), NULL);
	PpsDocumentInfo *info = pps_document_get_info (document);

	return (info->fields_mask & PPS_DOCUMENT_INFO_TITLE) ?
		info->title : NULL;
}

gboolean
pps_document_is_page_size_uniform (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), TRUE);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	pps_document_ensure_cache (document);

	return priv->uniform;
}

/**
 * pps_document_get_max_page_size:
 * @document: an #PpsDocument
 * @width: (out): max page width
 * @height: (out): max page height
 *
 */
void
pps_document_get_max_page_size (PpsDocument *document,
			       gdouble    *width,
			       gdouble    *height)
{
	g_return_if_fail (PPS_IS_DOCUMENT (document));
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	pps_document_ensure_cache (document);

	if (width)
		*width = priv->max_width;
	if (height)
		*height = priv->max_height;
}

/**
 * pps_document_get_min_page_size:
 * @document: an #PpsDocument
 * @width: (out): min page width
 * @height: (out): min page height
 *
 */
void
pps_document_get_min_page_size (PpsDocument *document,
			       gdouble    *width,
			       gdouble    *height)
{
	g_return_if_fail (PPS_IS_DOCUMENT (document));
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	pps_document_ensure_cache (document);

	if (width)
		*width = priv->min_width;
	if (height)
		*height = priv->min_height;
}

gboolean
pps_document_check_dimensions (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), FALSE);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	pps_document_ensure_cache (document);

	return (priv->max_width > 0 && priv->max_height > 0);
}

guint64
pps_document_get_size (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), 0);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	return priv->file_size;
}

gint
pps_document_get_max_label_len (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), -1);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	pps_document_ensure_cache (document);

	return priv->max_label;
}

gboolean
pps_document_has_text_page_labels (PpsDocument *document)
{
	g_return_val_if_fail (PPS_IS_DOCUMENT (document), FALSE);
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	pps_document_ensure_cache (document);

	return priv->page_labels != NULL;
}


/**
 * pps_document_find_page_by_label:
 * @document: a #PpsDocument
 * @page_label: the page label
 * @page_index: (out): the output page index
 *
 * Returns: %TRUE iff the page index is found
 *
 */
gboolean
pps_document_find_page_by_label (PpsDocument  *document,
				const gchar *page_label,
				gint        *page_index)
{
	gint i, page, n_pages;
	glong value;
	gchar *endptr = NULL;
	PpsDocumentPrivate *priv = GET_PRIVATE (document);

	g_return_val_if_fail (PPS_IS_DOCUMENT (document), FALSE);
	g_return_val_if_fail (page_label != NULL, FALSE);
	g_return_val_if_fail (page_index != NULL, FALSE);

	pps_document_ensure_cache (document);

	n_pages = pps_document_get_n_pages (document);

        /* First, look for a literal label match */
	for (i = 0; priv->page_labels && i < n_pages; i ++) {
		if (priv->page_labels[i] != NULL &&
		    ! strcmp (page_label, priv->page_labels[i])) {
			*page_index = i;
			return TRUE;
		}
	}

	/* Second, look for a match with case insensitively */
	for (i = 0; priv->page_labels && i < n_pages; i++) {
		if (priv->page_labels[i] != NULL &&
		    ! strcasecmp (page_label, priv->page_labels[i])) {
			*page_index = i;
			return TRUE;
		}
	}

	/* Next, parse the label, and see if the number fits */
	value = strtol (page_label, &endptr, 10);
	if (endptr[0] == '\0') {
		/* Page number is an integer */
		page = MIN (G_MAXINT, value);

		/* convert from a page label to a page offset */
		page --;
		if (page >= 0 && page < n_pages) {
			*page_index = page;
			return TRUE;
		}
	}

	return FALSE;
}

/* PpsRectangle */
G_DEFINE_BOXED_TYPE (PpsRectangle, pps_rectangle, pps_rectangle_copy, pps_rectangle_free)

PpsRectangle *
pps_rectangle_new (void)
{
	return g_new0 (PpsRectangle, 1);
}

PpsRectangle *
pps_rectangle_copy (PpsRectangle *rectangle)
{
	PpsRectangle *new_rectangle;

	g_return_val_if_fail (rectangle != NULL, NULL);

	new_rectangle = g_new (PpsRectangle, 1);
	*new_rectangle = *rectangle;

	return new_rectangle;
}

void
pps_rectangle_free (PpsRectangle *rectangle)
{
	g_free (rectangle);
}

/* Compares two rects.  returns 0 if they're equal */
#define EPSILON 0.0000001

gint
pps_rect_cmp (PpsRectangle *a,
	     PpsRectangle *b)
{
	if (a == b)
		return 0;
	if (a == NULL || b == NULL)
		return 1;

	return ! ((ABS (a->x1 - b->x1) < EPSILON) &&
		  (ABS (a->y1 - b->y1) < EPSILON) &&
		  (ABS (a->x2 - b->x2) < EPSILON) &&
		  (ABS (a->y2 - b->y2) < EPSILON));
}
