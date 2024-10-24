/* pps-annotation.c
 *  this file is part of papers, a gnome document viewer
 *
 * Copyright (C) 2009 Carlos Garcia Campos <carlosgc@gnome.org>
 * Copyright (C) 2007 Iñigo Martinez <inigomartinez@gmail.com>
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

#include "pps-annotation.h"
#include "pps-document-misc.h"
#include "pps-document-type-builtins.h"

/* PpsAnnotation*/
typedef struct
{
	PpsAnnotationType type;
	PpsPage *page;

	gchar *contents;
	gchar *name;
	gchar *modified;
	GdkRGBA rgba;
	PpsRectangle area;
} PpsAnnotationPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (PpsAnnotation, pps_annotation, G_TYPE_OBJECT);
#define GET_ANNOT_PRIVATE(o) pps_annotation_get_instance_private (o)

/* PpsAnnotationMarkup*/
struct _PpsAnnotationMarkupInterface {
	GTypeInterface base_iface;
};

G_DEFINE_INTERFACE (PpsAnnotationMarkup, pps_annotation_markup, PPS_TYPE_ANNOTATION);

/* PpsAnnotationText*/
typedef struct
{
	gboolean is_open : 1;
	PpsAnnotationTextIcon icon;
} PpsAnnotationTextPrivate;

struct _PpsAnnotationText {
	PpsAnnotation parent;
};

static void pps_annotation_text_markup_iface_init (PpsAnnotationMarkupInterface *iface);

G_DEFINE_TYPE_WITH_CODE (PpsAnnotationText,
                         pps_annotation_text,
                         PPS_TYPE_ANNOTATION,
                         G_IMPLEMENT_INTERFACE (PPS_TYPE_ANNOTATION_MARKUP,
                                                pps_annotation_text_markup_iface_init)
                             G_ADD_PRIVATE (PpsAnnotationText));
#define GET_ANNOT_TEXT_PRIVATE(o) pps_annotation_text_get_instance_private (o)

/* PpsAnnotationAttachment */
typedef struct
{
	PpsAttachment *attachment;
} PpsAnnotationAttachmentPrivate;

struct _PpsAnnotationAttachment {
	PpsAnnotation parent;
};

static void pps_annotation_attachment_markup_iface_init (PpsAnnotationMarkupInterface *iface);

G_DEFINE_TYPE_WITH_CODE (PpsAnnotationAttachment,
                         pps_annotation_attachment,
                         PPS_TYPE_ANNOTATION,
                         G_IMPLEMENT_INTERFACE (PPS_TYPE_ANNOTATION_MARKUP,
                                                pps_annotation_attachment_markup_iface_init)
                             G_ADD_PRIVATE (PpsAnnotationAttachment));
#define GET_ANNOT_ATTACH_PRIVATE(o) pps_annotation_attachment_get_instance_private (o)

/* PpsAnnotationTextMarkup */
typedef struct
{
	PpsAnnotationTextMarkupType type;
} PpsAnnotationTextMarkupPrivate;

struct _PpsAnnotationTextMarkup {
	PpsAnnotation parent;
};

static void pps_annotation_text_markup_markup_iface_init (PpsAnnotationMarkupInterface *iface);

G_DEFINE_TYPE_WITH_CODE (PpsAnnotationTextMarkup,
                         pps_annotation_text_markup,
                         PPS_TYPE_ANNOTATION,
                         G_IMPLEMENT_INTERFACE (PPS_TYPE_ANNOTATION_MARKUP,
                                                pps_annotation_text_markup_markup_iface_init)
                             G_ADD_PRIVATE (PpsAnnotationTextMarkup));
#define GET_ANNOT_TEXT_MARKUP_PRIVATE(o) pps_annotation_text_markup_get_instance_private (o)

/* PpsAnnotation */
enum {
	PROP_ANNOT_0,
	PROP_ANNOT_PAGE,
	PROP_ANNOT_CONTENTS,
	PROP_ANNOT_NAME,
	PROP_ANNOT_MODIFIED,
	PROP_ANNOT_RGBA,
	PROP_ANNOT_AREA
};

/* PpsAnnotationMarkup */
enum {
	PROP_MARKUP_0,
	PROP_MARKUP_LABEL,
	PROP_MARKUP_OPACITY,
	PROP_MARKUP_CAN_HAVE_POPUP,
	PROP_MARKUP_HAS_POPUP,
	PROP_MARKUP_RECTANGLE,
	PROP_MARKUP_POPUP_IS_OPEN
};

/* PpsAnnotationText */
enum {
	PROP_TEXT_ICON = PROP_MARKUP_POPUP_IS_OPEN + 1,
	PROP_TEXT_IS_OPEN
};

/* PpsAnnotationAttachment */
enum {
	PROP_ATTACHMENT_ATTACHMENT = PROP_MARKUP_POPUP_IS_OPEN + 1
};

/* PpsAnnotationTextMarkup */
enum {
	PROP_TEXT_MARKUP_TYPE = PROP_MARKUP_POPUP_IS_OPEN + 1
};

/* PpsAnnotation */
static void
pps_annotation_finalize (GObject *object)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (PPS_ANNOTATION (object));

	g_clear_object (&priv->page);
	g_clear_pointer (&priv->contents, g_free);
	g_clear_pointer (&priv->name, g_free);
	g_clear_pointer (&priv->modified, g_free);

	G_OBJECT_CLASS (pps_annotation_parent_class)->finalize (object);
}

static void
pps_annotation_init (PpsAnnotation *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	priv->type = PPS_ANNOTATION_TYPE_UNKNOWN;
	priv->contents = g_strdup ("");
	priv->area.x1 = -1;
	priv->area.y1 = -1;
	priv->area.x2 = -1;
	priv->area.y2 = -1;
}

static void
pps_annotation_set_property (GObject *object,
                             guint prop_id,
                             const GValue *value,
                             GParamSpec *pspec)
{
	PpsAnnotation *annot = PPS_ANNOTATION (object);
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	switch (prop_id) {
	case PROP_ANNOT_PAGE:
		priv->page = g_value_dup_object (value);
		break;
	case PROP_ANNOT_CONTENTS:
		pps_annotation_set_contents (annot, g_value_get_string (value));
		break;
	case PROP_ANNOT_NAME:
		pps_annotation_set_name (annot, g_value_get_string (value));
		break;
	case PROP_ANNOT_MODIFIED:
		pps_annotation_set_modified (annot, g_value_get_string (value));
		break;
	case PROP_ANNOT_RGBA:
		pps_annotation_set_rgba (annot, g_value_get_boxed (value));
		break;
	case PROP_ANNOT_AREA:
		pps_annotation_set_area (annot, g_value_get_boxed (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_get_property (GObject *object,
                             guint prop_id,
                             GValue *value,
                             GParamSpec *pspec)
{
	PpsAnnotation *annot = PPS_ANNOTATION (object);
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	switch (prop_id) {
	case PROP_ANNOT_CONTENTS:
		g_value_set_string (value, pps_annotation_get_contents (annot));
		break;
	case PROP_ANNOT_NAME:
		g_value_set_string (value, pps_annotation_get_name (annot));
		break;
	case PROP_ANNOT_MODIFIED:
		g_value_set_string (value, pps_annotation_get_modified (annot));
		break;
	case PROP_ANNOT_RGBA:
		g_value_set_boxed (value, &priv->rgba);
		break;
	case PROP_ANNOT_AREA:
		g_value_set_boxed (value, &priv->area);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_class_init (PpsAnnotationClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	g_object_class->finalize = pps_annotation_finalize;
	g_object_class->set_property = pps_annotation_set_property;
	g_object_class->get_property = pps_annotation_get_property;

	g_object_class_install_property (g_object_class,
	                                 PROP_ANNOT_PAGE,
	                                 g_param_spec_object ("page",
	                                                      "Page",
	                                                      "The page wehere the annotation is",
	                                                      PPS_TYPE_PAGE,
	                                                      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY |
	                                                          G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (g_object_class,
	                                 PROP_ANNOT_CONTENTS,
	                                 g_param_spec_string ("contents",
	                                                      "Contents",
	                                                      "The annotation contents",
	                                                      NULL,
	                                                      G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (g_object_class,
	                                 PROP_ANNOT_NAME,
	                                 g_param_spec_string ("name",
	                                                      "Name",
	                                                      "The annotation unique name",
	                                                      NULL,
	                                                      G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (g_object_class,
	                                 PROP_ANNOT_MODIFIED,
	                                 g_param_spec_string ("modified",
	                                                      "Modified",
	                                                      "Last modified date as string",
	                                                      NULL,
	                                                      G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));

	/**
	 * PpsAnnotation:rgba:
	 *
	 * The colour of the annotation as a #GdkRGBA.
	 *
	 * Since: 3.6
	 */
	g_object_class_install_property (g_object_class,
	                                 PROP_ANNOT_RGBA,
	                                 g_param_spec_boxed ("rgba", NULL, NULL,
	                                                     GDK_TYPE_RGBA,
	                                                     G_PARAM_READWRITE |
	                                                         G_PARAM_STATIC_STRINGS));

	/**
	 * PpsAnnotation:area:
	 *
	 * The area of the page where the annotation is placed.
	 *
	 * Since 3.18
	 */
	g_object_class_install_property (g_object_class,
	                                 PROP_ANNOT_AREA,
	                                 g_param_spec_boxed ("area",
	                                                     "Area",
	                                                     "The area of the page where the annotation is placed",
	                                                     PPS_TYPE_RECTANGLE,
	                                                     G_PARAM_READWRITE |
	                                                         G_PARAM_STATIC_STRINGS));
}

PpsAnnotationType
pps_annotation_get_annotation_type (PpsAnnotation *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), 0);

	return priv->type;
}

/**
 * pps_annotation_get_page:
 * @annot: an #PpsAnnotation
 *
 * Get the page where @annot appears.
 *
 * Returns: (transfer none): the #PpsPage where @annot appears
 */
PpsPage *
pps_annotation_get_page (PpsAnnotation *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), NULL);

	return priv->page;
}

/**
 * pps_annotation_get_page_index:
 * @annot: an #PpsAnnotation
 *
 * Get the index of the page where @annot appears. Note that the index
 * is 0 based.
 *
 * Returns: the page index.
 */
guint
pps_annotation_get_page_index (PpsAnnotation *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), 0);

	return priv->page->index;
}

/**
 * pps_annotation_equal:
 * @annot: an #PpsAnnotation
 * @other: another #PpsAnnotation
 *
 * Compare @annot and @other.
 *
 * Returns: %TRUE if @annot is equal to @other, %FALSE otherwise
 */
gboolean
pps_annotation_equal (PpsAnnotation *annot,
                      PpsAnnotation *other)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), FALSE);
	g_return_val_if_fail (PPS_IS_ANNOTATION (other), FALSE);

	return (annot == other ||
	        g_strcmp0 (priv->name, pps_annotation_get_name (other)) == 0);
}

/**
 * pps_annotation_get_contents:
 * @annot: an #PpsAnnotation
 *
 * Get the contents of @annot. The contents of
 * @annot is the text that is displayed in the annotation, or an
 * alternate description of the annotation's content for non-text annotations
 *
 * Returns: a string with the contents of the annotation or
 * %NULL if @annot has no contents.
 */
const gchar *
pps_annotation_get_contents (PpsAnnotation *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), NULL);

	return priv->contents;
}

/**
 * pps_annotation_set_contents:
 * @annot: an #PpsAnnotation
 *
 * Set the contents of @annot. You can monitor
 * changes in the annotation's  contents by connecting to
 * notify::contents signal of @annot.
 *
 * Returns: %TRUE if the contents have been changed, %FALSE otherwise.
 */
gboolean
pps_annotation_set_contents (PpsAnnotation *annot,
                             const gchar *contents)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), FALSE);

	if (g_strcmp0 (priv->contents, contents) == 0)
		return FALSE;

	if (priv->contents)
		g_free (priv->contents);
	priv->contents = contents ? g_strdup (contents) : NULL;

	g_object_notify (G_OBJECT (annot), "contents");

	return TRUE;
}

/**
 * pps_annotation_get_name:
 * @annot: an #PpsAnnotation
 *
 * Get the name of @annot. The name of the annotation is a string
 * that uniquely indenftifies @annot amongs all the annotations
 * in the same page.
 *
 * Returns: the string with the annotation's name.
 */
const gchar *
pps_annotation_get_name (PpsAnnotation *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), NULL);

	return priv->name;
}

/**
 * pps_annotation_set_name:
 * @annot: an #PpsAnnotation
 *
 * Set the name of @annot.
 * You can monitor changes of the annotation name by connecting
 * to the notify::name signal on @annot.
 *
 * Returns: %TRUE when the name has been changed, %FALSE otherwise.
 */
gboolean
pps_annotation_set_name (PpsAnnotation *annot,
                         const gchar *name)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), FALSE);

	if (g_strcmp0 (priv->name, name) == 0)
		return FALSE;

	if (priv->name)
		g_free (priv->name);
	priv->name = name ? g_strdup (name) : NULL;

	g_object_notify (G_OBJECT (annot), "name");

	return TRUE;
}

/**
 * pps_annotation_get_modified:
 * @annot: an #PpsAnnotation
 *
 * Get the last modification date of @annot.
 *
 * Returns: A string containing the last modification date.
 */
const gchar *
pps_annotation_get_modified (PpsAnnotation *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), NULL);

	return priv->modified;
}

/**
 * pps_annotation_set_modified:
 * @annot: an #PpsAnnotation
 * @modified: string with the last modification date.
 *
 * Set the last modification date of @annot to @modified. To
 * set the last modification date using a #time_t, use
 * pps_annotation_set_modified_from_time_t() instead. You can monitor
 * changes to the last modification date by connecting to the
 * notify::modified signal on @annot.
 *
 * Returns: %TRUE if the last modification date has been updated, %FALSE otherwise.
 */
gboolean
pps_annotation_set_modified (PpsAnnotation *annot,
                             const gchar *modified)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), FALSE);

	if (g_strcmp0 (priv->modified, modified) == 0)
		return FALSE;

	if (priv->modified)
		g_free (priv->modified);
	priv->modified = modified ? g_strdup (modified) : NULL;

	g_object_notify (G_OBJECT (annot), "modified");

	return TRUE;
}

/**
 * pps_annotation_set_modified_from_time_t:
 * @annot: an #PpsAnnotation
 * @utime: a #time_t
 *
 * Set the last modification date of @annot to @utime.  You can
 * monitor changes to the last modification date by connecting to the
 * notify::modified sinal on @annot.
 * For the time-format used, see pps_document_misc_format_datetime().
 *
 * Returns: %TRUE if the last modified date has been updated, %FALSE otherwise.
 */
gboolean
pps_annotation_set_modified_from_time_t (PpsAnnotation *annot,
                                         time_t utime)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);
	gchar *modified;
	g_autoptr (GDateTime) dt = g_date_time_new_from_unix_utc ((gint64) utime);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), FALSE);

	modified = pps_document_misc_format_datetime (dt);

	if (g_strcmp0 (priv->modified, modified) == 0) {
		g_free (modified);
		return FALSE;
	}

	if (priv->modified)
		g_free (priv->modified);

	priv->modified = modified;
	g_object_notify (G_OBJECT (annot), "modified");

	return TRUE;
}

/**
 * pps_annotation_get_rgba:
 * @annot: an #PpsAnnotation
 * @rgba: (out): a #GdkRGBA to be filled with the annotation color
 *
 * Gets the color of @annot.
 *
 * Since: 3.6
 */
void
pps_annotation_get_rgba (PpsAnnotation *annot,
                         GdkRGBA *rgba)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_if_fail (PPS_IS_ANNOTATION (annot));
	g_return_if_fail (rgba != NULL);

	*rgba = priv->rgba;
}

/**
 * pps_annotation_set_rgba:
 * @annot: an #Ppsannotation
 * @rgba: a #GdkRGBA
 *
 * Set the color of the annotation to @rgba.
 *
 * Returns: %TRUE if the color has been changed, %FALSE otherwise
 *
 * Since: 3.6
 */
gboolean
pps_annotation_set_rgba (PpsAnnotation *annot,
                         const GdkRGBA *rgba)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), FALSE);
	g_return_val_if_fail (rgba != NULL, FALSE);

	if (gdk_rgba_equal (rgba, &priv->rgba))
		return FALSE;

	priv->rgba = *rgba;
	g_object_notify (G_OBJECT (annot), "rgba");

	return TRUE;
}

/**
 * pps_annotation_get_area:
 * @annot: an #PpsAnnotation
 * @area: (out): a #PpsRectangle to be filled with the annotation area
 *
 * Gets the area of @annot.
 *
 * Since: 3.18
 */
void
pps_annotation_get_area (PpsAnnotation *annot,
                         PpsRectangle *area)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);

	g_return_if_fail (PPS_IS_ANNOTATION (annot));
	g_return_if_fail (area != NULL);

	*area = priv->area;
}

/**
 * pps_annotation_set_area:
 * @annot: an #Ppsannotation
 * @area: a #PpsRectangle
 *
 * Set the area of the annotation to @area.
 *
 * Returns: %TRUE if the area has been changed, %FALSE otherwise
 *
 * Since: 3.18
 */
gboolean
pps_annotation_set_area (PpsAnnotation *annot,
                         const PpsRectangle *area)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (annot);
	gboolean was_initial;

	g_return_val_if_fail (PPS_IS_ANNOTATION (annot), FALSE);
	g_return_val_if_fail (area != NULL, FALSE);

	if (pps_rect_cmp ((PpsRectangle *) area, &priv->area) == 0)
		return FALSE;

	was_initial = priv->area.x1 == -1 && priv->area.x2 == -1 && priv->area.y1 == -1 && priv->area.y2 == -1;
	priv->area = *area;
	if (!was_initial)
		g_object_notify (G_OBJECT (annot), "area");

	return TRUE;
}

/* PpsAnnotationMarkup */
typedef struct
{
	gchar *label;
	gdouble opacity;
	gboolean can_have_popup;
	gboolean has_popup;
	gboolean popup_is_open;
	PpsRectangle rectangle;
} PpsAnnotationMarkupProps;

static void
pps_annotation_markup_default_init (PpsAnnotationMarkupInterface *iface)
{
	static gboolean initialized = FALSE;

	if (!initialized) {
		g_object_interface_install_property (iface,
		                                     g_param_spec_string ("label",
		                                                          "Label",
		                                                          "Label of the markup annotation",
		                                                          NULL,
		                                                          G_PARAM_READWRITE |
		                                                              G_PARAM_STATIC_STRINGS));
		g_object_interface_install_property (iface,
		                                     g_param_spec_double ("opacity",
		                                                          "Opacity",
		                                                          "Opacity of the markup annotation",
		                                                          0,
		                                                          G_MAXDOUBLE,
		                                                          1.,
		                                                          G_PARAM_READWRITE |
		                                                              G_PARAM_STATIC_STRINGS));
		g_object_interface_install_property (iface,
		                                     g_param_spec_boolean ("can-have-popup",
		                                                           "Can have popup",
		                                                           "Whether it is allowed to have a popup "
		                                                           "window for this type of markup annotation",
		                                                           FALSE,
		                                                           G_PARAM_READWRITE |
		                                                               G_PARAM_STATIC_STRINGS));
		g_object_interface_install_property (iface,
		                                     g_param_spec_boolean ("has-popup",
		                                                           "Has popup",
		                                                           "Whether the markup annotation has "
		                                                           "a popup window associated",
		                                                           TRUE,
		                                                           G_PARAM_READWRITE |
		                                                               G_PARAM_STATIC_STRINGS));
		g_object_interface_install_property (iface,
		                                     g_param_spec_boxed ("rectangle",
		                                                         "Rectangle",
		                                                         "The Rectangle of the popup associated "
		                                                         "to the markup annotation",
		                                                         PPS_TYPE_RECTANGLE,
		                                                         G_PARAM_READWRITE |
		                                                             G_PARAM_STATIC_STRINGS));
		g_object_interface_install_property (iface,
		                                     g_param_spec_boolean ("popup-is-open",
		                                                           "PopupIsOpen",
		                                                           "Whether the popup associated to "
		                                                           "the markup annotation is open",
		                                                           FALSE,
		                                                           G_PARAM_READWRITE |
		                                                               G_PARAM_STATIC_STRINGS));
		initialized = TRUE;
	}
}

static void
pps_annotation_markup_props_free (PpsAnnotationMarkupProps *props)
{
	g_free (props->label);
	g_slice_free (PpsAnnotationMarkupProps, props);
}

static PpsAnnotationMarkupProps *
pps_annotation_markup_get_properties (PpsAnnotationMarkup *markup)
{
	PpsAnnotationMarkupProps *props;
	static GQuark props_key = 0;

	if (!props_key)
		props_key = g_quark_from_static_string ("pps-annotation-markup-props");

	props = g_object_get_qdata (G_OBJECT (markup), props_key);
	if (!props) {
		props = g_slice_new0 (PpsAnnotationMarkupProps);
		g_object_set_qdata_full (G_OBJECT (markup),
		                         props_key, props,
		                         (GDestroyNotify) pps_annotation_markup_props_free);
	}

	return props;
}

static void
pps_annotation_markup_set_property (GObject *object,
                                    guint prop_id,
                                    const GValue *value,
                                    GParamSpec *pspec)
{
	PpsAnnotationMarkup *markup = PPS_ANNOTATION_MARKUP (object);

	switch (prop_id) {
	case PROP_MARKUP_LABEL:
		pps_annotation_markup_set_label (markup, g_value_get_string (value));
		break;
	case PROP_MARKUP_OPACITY:
		pps_annotation_markup_set_opacity (markup, g_value_get_double (value));
		break;
	case PROP_MARKUP_CAN_HAVE_POPUP: {
		PpsAnnotationMarkupProps *props;

		props = pps_annotation_markup_get_properties (markup);
		props->can_have_popup = g_value_get_boolean (value);
		break;
	}
	case PROP_MARKUP_HAS_POPUP:
		pps_annotation_markup_set_has_popup (markup, g_value_get_boolean (value));
		break;
	case PROP_MARKUP_RECTANGLE:
		pps_annotation_markup_set_rectangle (markup, g_value_get_boxed (value));
		break;
	case PROP_MARKUP_POPUP_IS_OPEN:
		pps_annotation_markup_set_popup_is_open (markup, g_value_get_boolean (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_markup_get_property (GObject *object,
                                    guint prop_id,
                                    GValue *value,
                                    GParamSpec *pspec)
{
	PpsAnnotationMarkupProps *props;

	props = pps_annotation_markup_get_properties (PPS_ANNOTATION_MARKUP (object));

	switch (prop_id) {
	case PROP_MARKUP_LABEL:
		g_value_set_string (value, props->label);
		break;
	case PROP_MARKUP_OPACITY:
		g_value_set_double (value, props->opacity);
		break;
	case PROP_MARKUP_CAN_HAVE_POPUP:
		g_value_set_boolean (value, props->can_have_popup);
		break;
	case PROP_MARKUP_HAS_POPUP:
		g_value_set_boolean (value, props->has_popup);
		break;
	case PROP_MARKUP_RECTANGLE:
		g_value_set_boxed (value, &props->rectangle);
		break;
	case PROP_MARKUP_POPUP_IS_OPEN:
		g_value_set_boolean (value, props->popup_is_open);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_markup_class_install_properties (GObjectClass *klass)
{
	klass->set_property = pps_annotation_markup_set_property;
	klass->get_property = pps_annotation_markup_get_property;

	g_object_class_override_property (klass, PROP_MARKUP_LABEL, "label");
	g_object_class_override_property (klass, PROP_MARKUP_OPACITY, "opacity");
	g_object_class_override_property (klass, PROP_MARKUP_CAN_HAVE_POPUP, "can-have-popup");
	g_object_class_override_property (klass, PROP_MARKUP_HAS_POPUP, "has-popup");
	g_object_class_override_property (klass, PROP_MARKUP_RECTANGLE, "rectangle");
	g_object_class_override_property (klass, PROP_MARKUP_POPUP_IS_OPEN, "popup-is-open");
}

const gchar *
pps_annotation_markup_get_label (PpsAnnotationMarkup *markup)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), NULL);

	props = pps_annotation_markup_get_properties (markup);
	return props->label;
}

gboolean
pps_annotation_markup_set_label (PpsAnnotationMarkup *markup,
                                 const gchar *label)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), FALSE);
	g_return_val_if_fail (label != NULL, FALSE);

	props = pps_annotation_markup_get_properties (markup);
	if (g_strcmp0 (props->label, label) == 0)
		return FALSE;

	if (props->label)
		g_free (props->label);
	props->label = g_strdup (label);

	g_object_notify (G_OBJECT (markup), "label");

	return TRUE;
}

gdouble
pps_annotation_markup_get_opacity (PpsAnnotationMarkup *markup)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), 1.0);

	props = pps_annotation_markup_get_properties (markup);
	return props->opacity;
}

gboolean
pps_annotation_markup_set_opacity (PpsAnnotationMarkup *markup,
                                   gdouble opacity)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), FALSE);

	props = pps_annotation_markup_get_properties (markup);
	if (props->opacity == opacity)
		return FALSE;

	props->opacity = opacity;

	g_object_notify (G_OBJECT (markup), "opacity");

	return TRUE;
}

gboolean
pps_annotation_markup_can_have_popup (PpsAnnotationMarkup *markup)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), FALSE);

	props = pps_annotation_markup_get_properties (markup);
	return props->can_have_popup;
}

gboolean
pps_annotation_markup_has_popup (PpsAnnotationMarkup *markup)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), FALSE);

	props = pps_annotation_markup_get_properties (markup);
	return props->has_popup;
}

gboolean
pps_annotation_markup_set_has_popup (PpsAnnotationMarkup *markup,
                                     gboolean has_popup)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), FALSE);

	props = pps_annotation_markup_get_properties (markup);
	if (props->has_popup == has_popup)
		return FALSE;

	props->has_popup = has_popup;

	g_object_notify (G_OBJECT (markup), "has-popup");

	return TRUE;
}

void
pps_annotation_markup_get_rectangle (PpsAnnotationMarkup *markup,
                                     PpsRectangle *pps_rect)
{
	PpsAnnotationMarkupProps *props;

	g_return_if_fail (PPS_IS_ANNOTATION_MARKUP (markup));
	g_return_if_fail (pps_rect != NULL);

	props = pps_annotation_markup_get_properties (markup);
	*pps_rect = props->rectangle;
}

gboolean
pps_annotation_markup_set_rectangle (PpsAnnotationMarkup *markup,
                                     const PpsRectangle *pps_rect)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), FALSE);
	g_return_val_if_fail (pps_rect != NULL, FALSE);

	props = pps_annotation_markup_get_properties (markup);
	if (props->rectangle.x1 == pps_rect->x1 &&
	    props->rectangle.y1 == pps_rect->y1 &&
	    props->rectangle.x2 == pps_rect->x2 &&
	    props->rectangle.y2 == pps_rect->y2)
		return FALSE;

	props->rectangle = *pps_rect;

	g_object_notify (G_OBJECT (markup), "rectangle");

	return TRUE;
}

gboolean
pps_annotation_markup_get_popup_is_open (PpsAnnotationMarkup *markup)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), FALSE);

	props = pps_annotation_markup_get_properties (markup);
	return props->popup_is_open;
}

gboolean
pps_annotation_markup_set_popup_is_open (PpsAnnotationMarkup *markup,
                                         gboolean is_open)
{
	PpsAnnotationMarkupProps *props;

	g_return_val_if_fail (PPS_IS_ANNOTATION_MARKUP (markup), FALSE);

	props = pps_annotation_markup_get_properties (markup);
	if (props->popup_is_open == is_open)
		return FALSE;

	props->popup_is_open = is_open;

	g_object_notify (G_OBJECT (markup), "popup_is_open");

	return TRUE;
}

/* PpsAnnotationText */
static void
pps_annotation_text_init (PpsAnnotationText *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (PPS_ANNOTATION (annot));

	priv->type = PPS_ANNOTATION_TYPE_TEXT;
}

static void
pps_annotation_text_set_property (GObject *object,
                                  guint prop_id,
                                  const GValue *value,
                                  GParamSpec *pspec)
{
	PpsAnnotationText *annot = PPS_ANNOTATION_TEXT (object);

	if (prop_id < PROP_ATTACHMENT_ATTACHMENT) {
		pps_annotation_markup_set_property (object, prop_id, value, pspec);
		return;
	}

	switch (prop_id) {
	case PROP_TEXT_ICON:
		pps_annotation_text_set_icon (annot, g_value_get_enum (value));
		break;
	case PROP_TEXT_IS_OPEN:
		pps_annotation_text_set_is_open (annot, g_value_get_boolean (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_text_get_property (GObject *object,
                                  guint prop_id,
                                  GValue *value,
                                  GParamSpec *pspec)
{
	PpsAnnotationText *annot = PPS_ANNOTATION_TEXT (object);
	PpsAnnotationTextPrivate *priv = GET_ANNOT_TEXT_PRIVATE (annot);

	if (prop_id < PROP_ATTACHMENT_ATTACHMENT) {
		pps_annotation_markup_get_property (object, prop_id, value, pspec);
		return;
	}

	switch (prop_id) {
	case PROP_TEXT_ICON:
		g_value_set_enum (value, priv->icon);
		break;
	case PROP_TEXT_IS_OPEN:
		g_value_set_boolean (value, priv->is_open);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_text_class_init (PpsAnnotationTextClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	pps_annotation_markup_class_install_properties (g_object_class);

	g_object_class->set_property = pps_annotation_text_set_property;
	g_object_class->get_property = pps_annotation_text_get_property;

	g_object_class_install_property (g_object_class,
	                                 PROP_TEXT_ICON,
	                                 g_param_spec_enum ("icon",
	                                                    "Icon",
	                                                    "The icon fo the text annotation",
	                                                    PPS_TYPE_ANNOTATION_TEXT_ICON,
	                                                    PPS_ANNOTATION_TEXT_ICON_NOTE,
	                                                    G_PARAM_READWRITE |
	                                                        G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (g_object_class,
	                                 PROP_TEXT_IS_OPEN,
	                                 g_param_spec_boolean ("is-open",
	                                                       "IsOpen",
	                                                       "Whether text annot is initially open",
	                                                       FALSE,
	                                                       G_PARAM_READWRITE |
	                                                           G_PARAM_STATIC_STRINGS));
}

static void
pps_annotation_text_markup_iface_init (PpsAnnotationMarkupInterface *iface)
{
}

PpsAnnotation *
pps_annotation_text_new (PpsPage *page)
{
	return PPS_ANNOTATION (g_object_new (PPS_TYPE_ANNOTATION_TEXT,
	                                     "page", page,
	                                     NULL));
}

PpsAnnotationTextIcon
pps_annotation_text_get_icon (PpsAnnotationText *text)
{
	PpsAnnotationTextPrivate *priv = GET_ANNOT_TEXT_PRIVATE (text);

	g_return_val_if_fail (PPS_IS_ANNOTATION_TEXT (text), 0);

	return priv->icon;
}

gboolean
pps_annotation_text_set_icon (PpsAnnotationText *text,
                              PpsAnnotationTextIcon icon)
{
	PpsAnnotationTextPrivate *priv = GET_ANNOT_TEXT_PRIVATE (text);

	g_return_val_if_fail (PPS_IS_ANNOTATION_TEXT (text), FALSE);

	if (priv->icon == icon)
		return FALSE;

	priv->icon = icon;

	g_object_notify (G_OBJECT (text), "icon");

	return TRUE;
}

gboolean
pps_annotation_text_get_is_open (PpsAnnotationText *text)
{
	PpsAnnotationTextPrivate *priv = GET_ANNOT_TEXT_PRIVATE (text);

	g_return_val_if_fail (PPS_IS_ANNOTATION_TEXT (text), FALSE);

	return priv->is_open;
}

gboolean
pps_annotation_text_set_is_open (PpsAnnotationText *text,
                                 gboolean is_open)
{
	PpsAnnotationTextPrivate *priv = GET_ANNOT_TEXT_PRIVATE (text);

	g_return_val_if_fail (PPS_IS_ANNOTATION_TEXT (text), FALSE);

	if (priv->is_open == is_open)
		return FALSE;

	priv->is_open = is_open;

	g_object_notify (G_OBJECT (text), "is_open");

	return TRUE;
}

/* PpsAnnotationAttachment */
static void
pps_annotation_attachment_finalize (GObject *object)
{
	PpsAnnotationAttachment *annot = PPS_ANNOTATION_ATTACHMENT (object);
	PpsAnnotationAttachmentPrivate *priv = GET_ANNOT_ATTACH_PRIVATE (annot);

	g_clear_object (&priv->attachment);

	G_OBJECT_CLASS (pps_annotation_attachment_parent_class)->finalize (object);
}

static void
pps_annotation_attachment_init (PpsAnnotationAttachment *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (PPS_ANNOTATION (annot));

	priv->type = PPS_ANNOTATION_TYPE_ATTACHMENT;
}

static void
pps_annotation_attachment_set_property (GObject *object,
                                        guint prop_id,
                                        const GValue *value,
                                        GParamSpec *pspec)
{
	PpsAnnotationAttachment *annot = PPS_ANNOTATION_ATTACHMENT (object);

	if (prop_id < PROP_ATTACHMENT_ATTACHMENT) {
		pps_annotation_markup_set_property (object, prop_id, value, pspec);
		return;
	}

	switch (prop_id) {
	case PROP_ATTACHMENT_ATTACHMENT:
		pps_annotation_attachment_set_attachment (annot, g_value_get_object (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_attachment_get_property (GObject *object,
                                        guint prop_id,
                                        GValue *value,
                                        GParamSpec *pspec)
{
	PpsAnnotationAttachment *annot = PPS_ANNOTATION_ATTACHMENT (object);
	PpsAnnotationAttachmentPrivate *priv = GET_ANNOT_ATTACH_PRIVATE (annot);

	if (prop_id < PROP_ATTACHMENT_ATTACHMENT) {
		pps_annotation_markup_get_property (object, prop_id, value, pspec);
		return;
	}

	switch (prop_id) {
	case PROP_ATTACHMENT_ATTACHMENT:
		g_value_set_object (value, priv->attachment);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_attachment_class_init (PpsAnnotationAttachmentClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (klass);

	pps_annotation_markup_class_install_properties (g_object_class);

	g_object_class->set_property = pps_annotation_attachment_set_property;
	g_object_class->get_property = pps_annotation_attachment_get_property;
	g_object_class->finalize = pps_annotation_attachment_finalize;

	g_object_class_install_property (g_object_class,
	                                 PROP_ATTACHMENT_ATTACHMENT,
	                                 g_param_spec_object ("attachment",
	                                                      "Attachment",
	                                                      "The attachment of the annotation",
	                                                      PPS_TYPE_ATTACHMENT,
	                                                      G_PARAM_CONSTRUCT |
	                                                          G_PARAM_READWRITE |
	                                                          G_PARAM_STATIC_STRINGS));
}

static void
pps_annotation_attachment_markup_iface_init (PpsAnnotationMarkupInterface *iface)
{
}

PpsAnnotation *
pps_annotation_attachment_new (PpsPage *page,
                               PpsAttachment *attachment)
{
	g_return_val_if_fail (PPS_IS_ATTACHMENT (attachment), NULL);

	return PPS_ANNOTATION (g_object_new (PPS_TYPE_ANNOTATION_ATTACHMENT,
	                                     "page", page,
	                                     "attachment", attachment,
	                                     NULL));
}

/**
 * pps_annotation_attachment_get_attachment:
 * @annot: an #PpsAnnotationAttachment
 *
 * Returns: (transfer none): an #PpsAttachment
 */
PpsAttachment *
pps_annotation_attachment_get_attachment (PpsAnnotationAttachment *annot)
{
	PpsAnnotationAttachmentPrivate *priv = GET_ANNOT_ATTACH_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION_ATTACHMENT (annot), NULL);

	return priv->attachment;
}

gboolean
pps_annotation_attachment_set_attachment (PpsAnnotationAttachment *annot,
                                          PpsAttachment *attachment)
{
	PpsAnnotationAttachmentPrivate *priv = GET_ANNOT_ATTACH_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION_ATTACHMENT (annot), FALSE);

	if (g_set_object (&priv->attachment, attachment)) {
		g_object_notify (G_OBJECT (annot), "attachment");
		return TRUE;
	}

	return FALSE;
}

/* PpsAnnotationTextMarkup */
static void
pps_annotation_text_markup_get_property (GObject *object,
                                         guint prop_id,
                                         GValue *value,
                                         GParamSpec *pspec)
{
	PpsAnnotationTextMarkup *annot = PPS_ANNOTATION_TEXT_MARKUP (object);
	PpsAnnotationTextMarkupPrivate *priv = GET_ANNOT_TEXT_MARKUP_PRIVATE (annot);

	if (prop_id < PROP_TEXT_MARKUP_TYPE) {
		pps_annotation_markup_get_property (object, prop_id, value, pspec);
		return;
	}

	switch (prop_id) {
	case PROP_TEXT_MARKUP_TYPE:
		g_value_set_enum (value, priv->type);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_text_markup_set_property (GObject *object,
                                         guint prop_id,
                                         const GValue *value,
                                         GParamSpec *pspec)
{
	PpsAnnotationTextMarkup *annot = PPS_ANNOTATION_TEXT_MARKUP (object);
	PpsAnnotationTextMarkupPrivate *priv = GET_ANNOT_TEXT_MARKUP_PRIVATE (annot);

	if (prop_id < PROP_TEXT_MARKUP_TYPE) {
		pps_annotation_markup_set_property (object, prop_id, value, pspec);
		return;
	}

	switch (prop_id) {
	case PROP_TEXT_MARKUP_TYPE:
		priv->type = g_value_get_enum (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
pps_annotation_text_markup_init (PpsAnnotationTextMarkup *annot)
{
	PpsAnnotationPrivate *priv = GET_ANNOT_PRIVATE (PPS_ANNOTATION (annot));

	priv->type = PPS_ANNOTATION_TYPE_TEXT_MARKUP;
}

static void
pps_annotation_text_markup_class_init (PpsAnnotationTextMarkupClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS (class);

	pps_annotation_markup_class_install_properties (g_object_class);

	g_object_class->get_property = pps_annotation_text_markup_get_property;
	g_object_class->set_property = pps_annotation_text_markup_set_property;

	g_object_class_install_property (g_object_class,
	                                 PROP_TEXT_MARKUP_TYPE,
	                                 g_param_spec_enum ("type",
	                                                    "Type",
	                                                    "The text markup annotation type",
	                                                    PPS_TYPE_ANNOTATION_TEXT_MARKUP_TYPE,
	                                                    PPS_ANNOTATION_TEXT_MARKUP_HIGHLIGHT,
	                                                    G_PARAM_READWRITE |
	                                                        G_PARAM_CONSTRUCT |
	                                                        G_PARAM_STATIC_STRINGS));
}

static void
pps_annotation_text_markup_markup_iface_init (PpsAnnotationMarkupInterface *iface)
{
}

PpsAnnotation *
pps_annotation_text_markup_highlight_new (PpsPage *page)
{
	PpsAnnotation *annot = PPS_ANNOTATION (g_object_new (PPS_TYPE_ANNOTATION_TEXT_MARKUP,
	                                                     "page", page,
	                                                     "type", PPS_ANNOTATION_TEXT_MARKUP_HIGHLIGHT,
	                                                     NULL));
	return annot;
}

PpsAnnotation *
pps_annotation_text_markup_strike_out_new (PpsPage *page)
{
	PpsAnnotation *annot = PPS_ANNOTATION (g_object_new (PPS_TYPE_ANNOTATION_TEXT_MARKUP,
	                                                     "page", page,
	                                                     "type", PPS_ANNOTATION_TEXT_MARKUP_STRIKE_OUT,
	                                                     NULL));
	return annot;
}

PpsAnnotation *
pps_annotation_text_markup_underline_new (PpsPage *page)
{
	PpsAnnotation *annot = PPS_ANNOTATION (g_object_new (PPS_TYPE_ANNOTATION_TEXT_MARKUP,
	                                                     "page", page,
	                                                     "type", PPS_ANNOTATION_TEXT_MARKUP_UNDERLINE,
	                                                     NULL));
	return annot;
}

PpsAnnotation *
pps_annotation_text_markup_squiggly_new (PpsPage *page)
{
	PpsAnnotation *annot = PPS_ANNOTATION (g_object_new (PPS_TYPE_ANNOTATION_TEXT_MARKUP,
	                                                     "page", page,
	                                                     "type", PPS_ANNOTATION_TEXT_MARKUP_SQUIGGLY,
	                                                     NULL));
	return annot;
}

PpsAnnotationTextMarkupType
pps_annotation_text_markup_get_markup_type (PpsAnnotationTextMarkup *annot)
{
	PpsAnnotationTextMarkupPrivate *priv = GET_ANNOT_TEXT_MARKUP_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION_TEXT_MARKUP (annot),
	                      PPS_ANNOTATION_TEXT_MARKUP_HIGHLIGHT);

	return priv->type;
}

gboolean
pps_annotation_text_markup_set_markup_type (PpsAnnotationTextMarkup *annot,
                                            PpsAnnotationTextMarkupType markup_type)
{
	PpsAnnotationTextMarkupPrivate *priv = GET_ANNOT_TEXT_MARKUP_PRIVATE (annot);

	g_return_val_if_fail (PPS_IS_ANNOTATION_TEXT_MARKUP (annot), FALSE);

	if (priv->type == markup_type)
		return FALSE;

	priv->type = markup_type;
	g_object_notify (G_OBJECT (annot), "type");

	return TRUE;
}
