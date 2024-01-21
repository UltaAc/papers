/*
 *  Copyright (C) 2005 Marco Pesenti Gritti
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA..
 *
 */

#pragma once

#include <glib-object.h>
#include <papers-document.h>
#include <papers-view.h>

G_BEGIN_DECLS

#define PPS_TYPE_HISTORY            (pps_history_get_type ())
#define PPS_HISTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PPS_TYPE_HISTORY, PpsHistory))
#define PPS_HISTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PPS_TYPE_HISTORY, PpsHistoryClass))
#define PPS_IS_HISTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PPS_TYPE_HISTORY))
#define PPS_IS_HISTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), PPS_TYPE_HISTORY))
#define PPS_HISTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PPS_TYPE_HISTORY, PpsHistoryClass))

typedef struct _PpsHistory		PpsHistory;
typedef struct _PpsHistoryClass		PpsHistoryClass;

struct _PpsHistory
{
	GObject parent;
};

struct _PpsHistoryClass
{
	GObjectClass parent_class;

	void (* changed)       (PpsHistory *history);
        void (* activate_link) (PpsHistory *history,
                                PpsLink    *link);
};

GType           pps_history_get_type         (void);
PpsHistory      *pps_history_new              (PpsDocumentModel *model);
void            pps_history_add_link         (PpsHistory       *history,
                                             PpsLink          *link);
void            pps_history_add_page         (PpsHistory       *history,
                                             gint            page);
gboolean        pps_history_can_go_back      (PpsHistory       *history);
void            pps_history_go_back          (PpsHistory       *history);
gboolean        pps_history_can_go_forward   (PpsHistory       *history);
void            pps_history_go_forward       (PpsHistory       *history);
gboolean        pps_history_go_to_link       (PpsHistory       *history,
                                             PpsLink          *link);
GList          *pps_history_get_back_list    (PpsHistory       *history);
GList          *pps_history_get_forward_list (PpsHistory       *history);

void            pps_history_freeze           (PpsHistory       *history);
void            pps_history_thaw             (PpsHistory       *history);
gboolean        pps_history_is_frozen        (PpsHistory       *history);

G_END_DECLS
