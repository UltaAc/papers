// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

mod annotation;
pub use self::annotation::Annotation;

mod attachment;
pub use self::attachment::Attachment;

mod document;
pub use self::document::Document;

mod document_attachments;
pub use self::document_attachments::DocumentAttachments;

mod document_fonts;
pub use self::document_fonts::DocumentFonts;

mod document_layers;
pub use self::document_layers::DocumentLayers;

mod document_links;
pub use self::document_links::DocumentLinks;

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

mod rectangle;
pub use self::rectangle::Rectangle;

mod enums;
pub use self::enums::AnnotationType;
pub use self::enums::CompressionType;
pub use self::enums::DocumentContainsJS;
pub use self::enums::SelectionStyle;
pub use self::enums::TransitionEffectAlignment;
pub use self::enums::TransitionEffectDirection;
pub use self::enums::TransitionEffectType;

mod flags;
pub use self::flags::DocumentInfoFields;

pub(crate) mod functions;

pub(crate) mod traits {
    pub use super::annotation::AnnotationExt;
    pub use super::attachment::AttachmentExt;
    pub use super::document::DocumentExt;
    pub use super::document_attachments::DocumentAttachmentsExt;
    pub use super::document_fonts::DocumentFontsExt;
    pub use super::document_layers::DocumentLayersExt;
    pub use super::document_links::DocumentLinksExt;
    pub use super::image::ImageExt;
    pub use super::layer::LayerExt;
    pub use super::outlines::OutlinesExt;
    pub use super::selection::SelectionExt;
    pub use super::transition_effect::TransitionEffectExt;
}
