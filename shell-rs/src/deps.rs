pub use adw::prelude::*;
pub use adw::subclass::prelude::*;

pub use evince_document::prelude::*;
pub use evince_shell::prelude::*;
pub use evince_shell::subclass::prelude::*;
pub use evince_view::prelude::*;

pub use evince_document::Document;
pub use evince_document::DocumentInfo;
pub use evince_view::DocumentModel;

pub use gtk::CompositeTemplate;
pub use gtk::TemplateChild;

pub use glib::subclass::InitializingObject;
pub use glib::subclass::Signal;
pub use glib::Properties;

pub use std::cell::RefCell;

pub use gettextrs::gettext;

pub use once_cell::sync::Lazy;

pub use crate::utils::*;

pub use log::{debug, error, info, warn};

pub use crate::config::*;
