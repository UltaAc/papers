// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

mod application;
pub use self::application::Application;

mod metadata;
pub use self::metadata::Metadata;

mod papers_application;
pub use self::papers_application::PapersApplication;

mod papers_window;
pub use self::papers_window::PapersWindow;

mod search_context;
pub use self::search_context::SearchContext;

mod search_result;
pub use self::search_result::SearchResult;

mod sidebar_page;
pub use self::sidebar_page::SidebarPage;

mod window;
pub use self::window::Window;

mod enums;
pub use self::enums::WindowRunMode;

pub(crate) mod traits {
    pub use super::papers_application::PapersApplicationExt;
    pub use super::papers_window::PapersWindowExt;
    pub use super::search_context::SearchContextExt;
    pub use super::search_result::SearchResultExt;
    pub use super::sidebar_page::SidebarPageExt;
    pub use super::window::WindowExt;
}
pub(crate) mod builders {
    pub use super::application::ApplicationBuilder;
    pub use super::search_context::SearchContextBuilder;
    pub use super::window::WindowBuilder;
}
