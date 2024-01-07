pub mod sidebar_page;

pub mod prelude {
    pub use super::sidebar_page::SidebarPageImpl;
    pub use glib::subclass::prelude::*;
}
