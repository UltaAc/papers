mod config {
    #![allow(dead_code)]

    include!(concat!(env!("CODEGEN_BUILD_DIR"), "/config.rs"));
}

use config::GETTEXT_PACKAGE;
use papers_shell::Application;

mod deps;
mod loader_view;
mod page_selector;
mod password_view;
mod properties_fonts;
mod properties_general;
mod properties_license;
mod properties_window;
mod sidebar_attachments;
mod sidebar_layers;
mod sidebar_links;
mod sidebar_thumbnails;
mod stack_switcher;
mod thumbnail_item;

use deps::*;

fn ensure_type() {
    // HACK: don't need gtk4::init after PpsApplicationWindow is port to rust
    let _ = gtk::init();

    // Hack: ensure type here so we don't need to add C interface
    loader_view::PpsLoaderView::ensure_type();
    page_selector::PpsPageSelector::ensure_type();
    password_view::PpsPasswordView::ensure_type();
    properties_general::PpsPropertiesGeneral::ensure_type();
    properties_window::PpsPropertiesWindow::ensure_type();
    sidebar_attachments::PpsSidebarAttachments::ensure_type();
    sidebar_layers::PpsSidebarLayers::ensure_type();
    sidebar_links::PpsSidebarLinks::ensure_type();
    sidebar_thumbnails::PpsSidebarThumbnails::ensure_type();
    sidebar_thumbnails::PpsSidebarThumbnails::ensure_type();
    stack_switcher::PpsStackSwitcher::ensure_type();
}

fn main() -> glib::ExitCode {
    let mut log_builder = env_logger::builder();
    log_builder.format_timestamp_millis();

    if !glib::log_writer_default_would_drop(glib::LogLevel::Debug, Some("papers")) {
        log_builder.filter_module("papers", log::LevelFilter::Debug);
    }

    log_builder.init();

    gettextrs::bindtextdomain(GETTEXT_PACKAGE, papers_document::locale_dir().unwrap())
        .expect("Unable to bind the text domain");
    gettextrs::bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8")
        .expect("Unable to bind the text domain codeset");
    gettextrs::textdomain(GETTEXT_PACKAGE).expect("Unable to switch to the text domain");

    ensure_type();
    Application::new().run()
}
