// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

mod document_view;
pub use self::document_view::DocumentView;

mod find_sidebar;
pub use self::find_sidebar::FindSidebar;

mod progress_message_area;
pub use self::progress_message_area::ProgressMessageArea;

mod sidebar_page;
pub use self::sidebar_page::SidebarPage;

pub(crate) mod functions;

pub(crate) mod traits {
    pub use super::document_view::DocumentViewExt;
    pub use super::find_sidebar::FindSidebarExt;
    pub use super::progress_message_area::ProgressMessageAreaExt;
    pub use super::sidebar_page::SidebarPageExt;
}
pub(crate) mod builders {
    pub use super::document_view::DocumentViewBuilder;
    pub use super::find_sidebar::FindSidebarBuilder;
    pub use super::progress_message_area::ProgressMessageAreaBuilder;
}
