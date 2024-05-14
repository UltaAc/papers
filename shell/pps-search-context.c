/* pps-search-context.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2015 Igalia S.L.
 * Copyright (C) 2024 Markus Göllnitz  <camelcasenick@bewares.it>
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

#include "pps-jobs.h"
#include "pps-search-context.h"
#include <papers-view.h>

enum {
        PROP_0,

        PROP_DOCUMENT_MODEL,
	PROP_SEARCH_TERM,

	NUM_PROPERTIES
};

enum {
        RESULT_ACTIVATED,

        STARTED,
	FINISHED,
        CLEARED,

        N_SIGNALS
};

typedef struct {
        PpsDocumentModel *model;
        PpsJob           *job;
        PpsFindOptions    options;
        PpsFindOptions    supported_options;

        guint             pages_searched;

	gchar            *search_term;
} PpsSearchContextPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PpsSearchContext, pps_search_context, G_TYPE_OBJECT)

#define GET_PRIVATE(o) pps_search_context_get_instance_private (o)

static guint signals[N_SIGNALS];

static GParamSpec *props[NUM_PROPERTIES] = { NULL, };

static void
pps_search_context_clear_job (PpsSearchContext *context)
{
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        if (!priv->job)
                return;

        if (!pps_job_is_finished (priv->job))
                pps_job_cancel (priv->job);

        g_signal_handlers_disconnect_matched (priv->job, G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, context);
	g_clear_object (&priv->job);
}

static void
pps_search_context_set_supported_options (PpsSearchContext *context,
                                          PpsFindOptions    options)
{
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        priv->supported_options = options;
}

static void
pps_search_context_setup_document (PpsSearchContext *context,
                                   PpsDocument      *document)
{
        if (!document || !PPS_IS_DOCUMENT_FIND (document)) {
                pps_search_context_set_supported_options (context, PPS_FIND_DEFAULT);
                return;
        }

        pps_search_context_set_supported_options (context, pps_document_find_get_supported_options (PPS_DOCUMENT_FIND (document)));
}

static void
document_changed_cb (PpsDocumentModel *model,
                     GParamSpec       *pspec,
                     PpsSearchContext *context)
{
        pps_search_context_setup_document (context, pps_document_model_get_document (model));
}

static void
find_job_finished_cb (PpsJobFind       *job,
                      PpsSearchContext *context)
{
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        g_signal_emit (context, signals[FINISHED], 0, priv->job);
        pps_search_context_clear_job (context);
}

static void
search_changed_cb (PpsSearchContext *context)
{
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        pps_search_context_clear_job (context);
        priv->pages_searched = 0;

        if (priv->search_term && priv->search_term[0]) {
                PpsDocument *doc = pps_document_model_get_document (priv->model);

                priv->job = pps_job_find_new (doc,
                                             pps_document_model_get_page (priv->model),
                                             pps_document_get_n_pages (doc),
                                             priv->search_term,
					     priv->options);
                g_signal_connect (priv->job, "finished",
                                  G_CALLBACK (find_job_finished_cb),
                                  context);

                g_signal_emit (context, signals[STARTED], 0, priv->job);
                pps_job_scheduler_push_job (priv->job, PPS_JOB_PRIORITY_NONE);
        } else {
                g_signal_emit (context, signals[CLEARED], 0);
        }
}

static void
pps_search_context_dispose (GObject *object)
{
        PpsSearchContext *context = PPS_SEARCH_CONTEXT (object);

        pps_search_context_clear_job (context);

        G_OBJECT_CLASS (pps_search_context_parent_class)->dispose (object);
}


static void
pps_search_context_init (PpsSearchContext *context)
{
}

static void
pps_search_context_set_property (GObject      *object,
				 guint         prop_id,
				 const GValue *value,
				 GParamSpec   *pspec)
{
        PpsSearchContext *context = PPS_SEARCH_CONTEXT (object);
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        switch (prop_id) {
        case PROP_DOCUMENT_MODEL:
                priv->model = g_value_get_object (value);
                break;
        case PROP_SEARCH_TERM:
                pps_search_context_set_search_term (context, g_value_get_string (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
}

static void
pps_search_context_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
        PpsSearchContext *context = PPS_SEARCH_CONTEXT (object);
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        switch (prop_id) {
        case PROP_SEARCH_TERM:
                g_value_set_string (value, priv->search_term);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        }
}

static void
pps_search_context_constructed (GObject *object)
{
	PpsSearchContext *context = PPS_SEARCH_CONTEXT (object);
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        G_OBJECT_CLASS (pps_search_context_parent_class)->constructed (object);

        g_object_add_weak_pointer (G_OBJECT (priv->model),
                                   (gpointer)&priv->model);

        pps_search_context_setup_document (context, pps_document_model_get_document (priv->model));
        g_signal_connect_object (priv->model, "notify::document",
                                 G_CALLBACK (document_changed_cb),
                                 context, G_CONNECT_DEFAULT);
}

static void
pps_search_context_class_init (PpsSearchContextClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

        gobject_class->set_property = pps_search_context_set_property;
        gobject_class->get_property = pps_search_context_get_property;
        gobject_class->dispose = pps_search_context_dispose;
        gobject_class->constructed = pps_search_context_constructed;

	props[PROP_DOCUMENT_MODEL] =
        	g_param_spec_object ("document-model",
				     "DocumentModel",
				     "The document model",
				     PPS_TYPE_DOCUMENT_MODEL,
				     G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

	props[PROP_SEARCH_TERM] =
		g_param_spec_string ("search-term",
				     NULL,
				     "search term for the current context",
				     "",
				     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY);

	g_object_class_install_properties (gobject_class, NUM_PROPERTIES, props);

        /* Signals */
        signals[RESULT_ACTIVATED] =
                g_signal_new ("result-activated",
                              G_TYPE_FROM_CLASS (gobject_class),
                              G_SIGNAL_RUN_LAST,
                              0, NULL, NULL,
                              g_cclosure_marshal_generic,
                              G_TYPE_NONE, 2,
                              G_TYPE_INT,
                              G_TYPE_INT);
        signals[STARTED] =
                g_signal_new ("started",
                              G_OBJECT_CLASS_TYPE (gobject_class),
                              G_SIGNAL_RUN_LAST,
                              0, NULL, NULL,
                              g_cclosure_marshal_VOID__OBJECT,
                              G_TYPE_NONE, 1,
                              PPS_TYPE_JOB_FIND);
        signals[FINISHED] =
                g_signal_new ("finished",
                              G_OBJECT_CLASS_TYPE (gobject_class),
                              G_SIGNAL_RUN_LAST,
                              0, NULL, NULL,
                              g_cclosure_marshal_VOID__OBJECT,
                              G_TYPE_NONE, 1,
                              PPS_TYPE_JOB_FIND);
        signals[CLEARED] =
                g_signal_new ("cleared",
                              G_OBJECT_CLASS_TYPE (gobject_class),
                              G_SIGNAL_RUN_LAST,
                              0, NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);
}

PpsSearchContext*
pps_search_context_new (PpsDocumentModel *model)
{
        return PPS_SEARCH_CONTEXT (g_object_new (PPS_TYPE_SEARCH_CONTEXT,
						 "search-term", "",
						 "document-model", model,
						 NULL));
}

void
pps_search_context_set_search_term (PpsSearchContext *context,
                                    const gchar      *search_term)
{
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

	if (g_strcmp0(search_term, priv->search_term) == 0)
		return;

	priv->search_term = g_strdup (search_term);
	g_object_notify_by_pspec (G_OBJECT (context), props[PROP_SEARCH_TERM]);
        search_changed_cb (context);
}

const gchar*
pps_search_context_get_search_term (PpsSearchContext *context)
{
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

	return g_strdup (priv->search_term);
}

void
pps_search_context_set_options (PpsSearchContext *context,
                                PpsFindOptions    options)
{
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        if (priv->options == options)
                return;

        priv->options = options;
        search_changed_cb (context);
}

PpsFindOptions
pps_search_context_get_options (PpsSearchContext *context)
{
        PpsSearchContextPrivate *priv = GET_PRIVATE (context);

        return priv->options;
}

void
pps_search_context_restart (PpsSearchContext *context)
{
        search_changed_cb (context);
}

void
pps_search_context_select_result (PpsSearchContext *context,
				  PpsSearchResult  *result)
{
	g_signal_emit (context, signals[RESULT_ACTIVATED], 0,
		       pps_search_result_get_page (result),
		       pps_search_result_get_index (result));
}
