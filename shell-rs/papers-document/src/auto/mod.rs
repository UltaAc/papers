// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

mod annotation;
pub use self::annotation::Annotation;

mod annotation_attachment;
pub use self::annotation_attachment::AnnotationAttachment;

mod annotation_markup;
pub use self::annotation_markup::AnnotationMarkup;

mod annotation_text;
pub use self::annotation_text::AnnotationText;

mod annotation_text_markup;
pub use self::annotation_text_markup::AnnotationTextMarkup;

mod attachment;
pub use self::attachment::Attachment;

mod document;
pub use self::document::Document;

mod document_annotations;
pub use self::document_annotations::DocumentAnnotations;

mod document_attachments;
pub use self::document_attachments::DocumentAttachments;

mod document_find;
pub use self::document_find::DocumentFind;

mod document_fonts;
pub use self::document_fonts::DocumentFonts;

mod document_forms;
pub use self::document_forms::DocumentForms;

mod document_images;
pub use self::document_images::DocumentImages;

mod document_layers;
pub use self::document_layers::DocumentLayers;

mod document_links;
pub use self::document_links::DocumentLinks;

mod document_security;
pub use self::document_security::DocumentSecurity;

mod document_text;
pub use self::document_text::DocumentText;

mod image;
pub use self::image::Image;

mod layer;
pub use self::layer::Layer;

mod link;
pub use self::link::Link;

mod link_action;
pub use self::link_action::LinkAction;

mod link_dest;
pub use self::link_dest::LinkDest;

mod outlines;
pub use self::outlines::Outlines;

mod page;
pub use self::page::Page;

mod selection;
pub use self::selection::Selection;

mod transition_effect;
pub use self::transition_effect::TransitionEffect;

mod document_info;
pub use self::document_info::DocumentInfo;

mod document_license;
pub use self::document_license::DocumentLicense;

mod find_rectangle;
pub use self::find_rectangle::FindRectangle;

mod rectangle;
pub use self::rectangle::Rectangle;

mod enums;
pub use self::enums::AnnotationTextIcon;
pub use self::enums::AnnotationTextMarkupType;
pub use self::enums::AnnotationType;
pub use self::enums::CompressionType;
pub use self::enums::DocumentContainsJS;
pub use self::enums::DocumentError;
pub use self::enums::DocumentMode;
pub use self::enums::LinkActionType;
pub use self::enums::LinkDestType;
pub use self::enums::SelectionStyle;
pub use self::enums::TransitionEffectAlignment;
pub use self::enums::TransitionEffectDirection;
pub use self::enums::TransitionEffectType;

mod flags;
pub use self::flags::DocumentInfoFields;
pub use self::flags::DocumentPermissions;
pub use self::flags::FindOptions;

pub(crate) mod functions;

pub(crate) mod traits {
    pub use super::annotation::AnnotationExt;
    pub use super::annotation_markup::AnnotationMarkupExt;
    pub use super::attachment::AttachmentExt;
    pub use super::document::DocumentExt;
    pub use super::document_annotations::DocumentAnnotationsExt;
    pub use super::document_attachments::DocumentAttachmentsExt;
    pub use super::document_find::DocumentFindExt;
    pub use super::document_fonts::DocumentFontsExt;
    pub use super::document_forms::DocumentFormsExt;
    pub use super::document_images::DocumentImagesExt;
    pub use super::document_layers::DocumentLayersExt;
    pub use super::document_links::DocumentLinksExt;
    pub use super::document_security::DocumentSecurityExt;
    pub use super::document_text::DocumentTextExt;
    pub use super::image::ImageExt;
    pub use super::layer::LayerExt;
    pub use super::outlines::OutlinesExt;
    pub use super::selection::SelectionExt;
    pub use super::transition_effect::TransitionEffectExt;
}
