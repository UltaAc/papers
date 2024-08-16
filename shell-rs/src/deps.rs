pub use adw::prelude::*;
pub use adw::subclass::prelude::*;

pub use papers_document::prelude::*;
pub use papers_shell::prelude::*;
pub use papers_view::prelude::*;

pub use papers_document::Document;
pub use papers_document::DocumentInfo;
pub use papers_document::DocumentLayers;
pub use papers_document::DocumentLinks;
pub use papers_document::DocumentSignatures;
pub use papers_document::DocumentText;
pub use papers_view::DocumentModel;
pub use papers_view::JobLinks;
pub use papers_view::JobThumbnailTexture;

pub use gtk::CompositeTemplate;
pub use gtk::TemplateChild;

pub use glib::subclass::InitializingObject;
pub use glib::subclass::Signal;
pub use glib::Properties;
pub use glib::SignalHandlerId;

pub use std::cell::RefCell;

pub use gettextrs::gettext;

pub use std::sync::OnceLock;

pub use log::{debug, warn};

pub use crate::config::*;
pub use crate::i18n::gettext_f;
pub use crate::window::WindowRunMode;

pub use crate::sidebar_page::{PpsSidebarPage, PpsSidebarPageExt, PpsSidebarPageImpl};
