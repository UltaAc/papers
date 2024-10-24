/* pps-signature-details.c
 * this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2024 Jan-Michael Brummer <jan-michael.brummer1@volkswagen.de>
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

#include "pps-signature-details.h"
#include "pps-job-scheduler.h"
#include "pps-jobs.h"

#include <glib/gi18n.h>

struct _PpsSignatureDetails {
	AdwDialog parent_instance;

	GtkWidget *status_page;
	GtkWidget *listbox;
};

G_DEFINE_FINAL_TYPE (PpsSignatureDetails, pps_signature_details, ADW_TYPE_DIALOG)

static void
pps_signature_details_class_init (PpsSignatureDetailsClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/papers/ui/pps-signature-details.ui");

	gtk_widget_class_bind_template_child (widget_class, PpsSignatureDetails, status_page);
	gtk_widget_class_bind_template_child (widget_class, PpsSignatureDetails, listbox);
}

static void
pps_signature_details_init (PpsSignatureDetails *self)
{
	gtk_widget_init_template (GTK_WIDGET (self));
}

static void
signatures_job_finished_callback (PpsJobSignatures *job,
                                  PpsSignatureDetails *self)
{
	gboolean is_invalid = FALSE;

	for (GList *iter = pps_job_signatures_get_signatures (job); iter && iter->data; iter = iter->next) {
		PpsSignature *signature = PPS_SIGNATURE (iter->data);
		GtkWidget *row = adw_expander_row_new ();
		GtkWidget *date_row, *signature_status_row, *certificate_status_row;
		GtkWidget *image;
		PpsCertificateStatus certificate_status = pps_signature_get_certificate_status (signature);
		PpsSignatureStatus signature_status = pps_signature_get_signature_status (signature);
		g_autofree char *signing_time = g_date_time_format (pps_signature_get_signature_time (signature), "%c");

		if (pps_signature_is_valid (signature)) {
			image = gtk_image_new_from_icon_name ("emblem-ok-symbolic");
			gtk_widget_add_css_class (image, "success");
		} else {
			image = gtk_image_new_from_icon_name ("emblem-important-symbolic");
			gtk_widget_add_css_class (image, "error");
			is_invalid = TRUE;
		}

		adw_expander_row_add_prefix (ADW_EXPANDER_ROW (row), image);

		g_object_bind_property (G_OBJECT (signature), "signer-name", G_OBJECT (row), "title", G_BINDING_SYNC_CREATE | G_BINDING_DEFAULT);

		date_row = adw_action_row_new ();
		adw_preferences_row_set_title (ADW_PREFERENCES_ROW (date_row), _ ("Date"));
		adw_action_row_set_subtitle (ADW_ACTION_ROW (date_row), signing_time);
		adw_expander_row_add_row (ADW_EXPANDER_ROW (row), date_row);

		signature_status_row = adw_action_row_new ();
		adw_preferences_row_set_title (ADW_PREFERENCES_ROW (signature_status_row), _ ("Signature Status"));
		adw_action_row_set_subtitle (ADW_ACTION_ROW (signature_status_row), pps_signature_signature_status_str (signature_status));
		if (signature_status != PPS_SIGNATURE_STATUS_VALID) {
			GtkWidget *status_img = gtk_image_new_from_icon_name ("emblem-important-symbolic");
			gtk_widget_add_css_class (status_img, "error");
			adw_action_row_add_suffix (ADW_ACTION_ROW (signature_status_row), status_img);
		}
		adw_expander_row_add_row (ADW_EXPANDER_ROW (row), signature_status_row);

		certificate_status_row = adw_action_row_new ();
		adw_preferences_row_set_title (ADW_PREFERENCES_ROW (certificate_status_row), _ ("Certificate Status"));
		adw_action_row_set_subtitle (ADW_ACTION_ROW (certificate_status_row), pps_signature_certificate_status_str (certificate_status));
		if (certificate_status != PPS_CERTIFICATE_STATUS_TRUSTED) {
			GtkWidget *status_img = gtk_image_new_from_icon_name ("emblem-important-symbolic");
			gtk_widget_add_css_class (status_img, "error");
			adw_action_row_add_suffix (ADW_ACTION_ROW (certificate_status_row), status_img);
		}
		adw_expander_row_add_row (ADW_EXPANDER_ROW (row), certificate_status_row);

		gtk_list_box_append (GTK_LIST_BOX (self->listbox), row);
	}

	if (is_invalid) {
		adw_status_page_set_description (ADW_STATUS_PAGE (self->status_page), ("Document integrity has been verified. It is invalid."));
	} else {
		adw_status_page_set_description (ADW_STATUS_PAGE (self->status_page), ("Document integrity has been verified. It is signed and valid."));
	}
}

void
pps_signature_details_show (PpsDocument *document,
                            GtkWindow *window)
{
	PpsSignatureDetails *self = PPS_SIGNATURE_DETAILS (g_object_new (PPS_TYPE_SIGNATURE_DETAILS, NULL));
	PpsJob *job = pps_job_signatures_new (document);

	g_signal_connect (job, "finished", G_CALLBACK (signatures_job_finished_callback), self);
	pps_job_scheduler_push_job (job, PPS_JOB_PRIORITY_NONE);

	adw_dialog_present (ADW_DIALOG (self), GTK_WIDGET (window));
}
