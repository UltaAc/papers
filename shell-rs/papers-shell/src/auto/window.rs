// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::WindowRunMode;
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsWindow")]
    pub struct Window(Object<ffi::PpsWindow, ffi::PpsWindowClass>) @extends adw::ApplicationWindow, gtk::ApplicationWindow, gtk::Window, gtk::Widget, @implements gtk::Accessible, gtk::Buildable, gtk::ConstraintTarget, gtk::Native, gtk::Root, gtk::ShortcutManager, gio::ActionGroup, gio::ActionMap;

    match fn {
        type_ => || ffi::pps_window_get_type(),
    }
}

impl Window {
    pub const NONE: Option<&'static Window> = None;

    #[doc(alias = "pps_window_new")]
    pub fn new() -> Window {
        assert_initialized_main_thread!();
        unsafe { from_glib_none(ffi::pps_window_new()) }
    }

    // rustdoc-stripper-ignore-next
    /// Creates a new builder-pattern struct instance to construct [`Window`] objects.
    ///
    /// This method returns an instance of [`WindowBuilder`](crate::builders::WindowBuilder) which can be used to create [`Window`] objects.
    pub fn builder() -> WindowBuilder {
        WindowBuilder::new()
    }
}

impl Default for Window {
    fn default() -> Self {
        Self::new()
    }
}

// rustdoc-stripper-ignore-next
/// A [builder-pattern] type to construct [`Window`] objects.
///
/// [builder-pattern]: https://doc.rust-lang.org/1.0.0/style/ownership/builders.html
#[must_use = "The builder must be built to be used"]
pub struct WindowBuilder {
    builder: glib::object::ObjectBuilder<'static, Window>,
}

impl WindowBuilder {
    fn new() -> Self {
        Self {
            builder: glib::object::Object::builder(),
        }
    }

    pub fn content(self, content: &impl IsA<gtk::Widget>) -> Self {
        Self {
            builder: self.builder.property("content", content.clone().upcast()),
        }
    }

    pub fn show_menubar(self, show_menubar: bool) -> Self {
        Self {
            builder: self.builder.property("show-menubar", show_menubar),
        }
    }

    pub fn application(self, application: &impl IsA<gtk::Application>) -> Self {
        Self {
            builder: self
                .builder
                .property("application", application.clone().upcast()),
        }
    }

    pub fn child(self, child: &impl IsA<gtk::Widget>) -> Self {
        Self {
            builder: self.builder.property("child", child.clone().upcast()),
        }
    }

    pub fn decorated(self, decorated: bool) -> Self {
        Self {
            builder: self.builder.property("decorated", decorated),
        }
    }

    pub fn default_height(self, default_height: i32) -> Self {
        Self {
            builder: self.builder.property("default-height", default_height),
        }
    }

    pub fn default_widget(self, default_widget: &impl IsA<gtk::Widget>) -> Self {
        Self {
            builder: self
                .builder
                .property("default-widget", default_widget.clone().upcast()),
        }
    }

    pub fn default_width(self, default_width: i32) -> Self {
        Self {
            builder: self.builder.property("default-width", default_width),
        }
    }

    pub fn deletable(self, deletable: bool) -> Self {
        Self {
            builder: self.builder.property("deletable", deletable),
        }
    }

    pub fn destroy_with_parent(self, destroy_with_parent: bool) -> Self {
        Self {
            builder: self
                .builder
                .property("destroy-with-parent", destroy_with_parent),
        }
    }

    pub fn display(self, display: &gdk::Display) -> Self {
        Self {
            builder: self.builder.property("display", display.clone()),
        }
    }

    pub fn focus_visible(self, focus_visible: bool) -> Self {
        Self {
            builder: self.builder.property("focus-visible", focus_visible),
        }
    }

    pub fn focus_widget(self, focus_widget: &impl IsA<gtk::Widget>) -> Self {
        Self {
            builder: self
                .builder
                .property("focus-widget", focus_widget.clone().upcast()),
        }
    }

    pub fn fullscreened(self, fullscreened: bool) -> Self {
        Self {
            builder: self.builder.property("fullscreened", fullscreened),
        }
    }

    #[cfg(feature = "gtk_v4_2")]
    #[cfg_attr(docsrs, doc(cfg(feature = "gtk_v4_2")))]
    pub fn handle_menubar_accel(self, handle_menubar_accel: bool) -> Self {
        Self {
            builder: self
                .builder
                .property("handle-menubar-accel", handle_menubar_accel),
        }
    }

    pub fn hide_on_close(self, hide_on_close: bool) -> Self {
        Self {
            builder: self.builder.property("hide-on-close", hide_on_close),
        }
    }

    pub fn icon_name(self, icon_name: impl Into<glib::GString>) -> Self {
        Self {
            builder: self.builder.property("icon-name", icon_name.into()),
        }
    }

    pub fn maximized(self, maximized: bool) -> Self {
        Self {
            builder: self.builder.property("maximized", maximized),
        }
    }

    pub fn mnemonics_visible(self, mnemonics_visible: bool) -> Self {
        Self {
            builder: self
                .builder
                .property("mnemonics-visible", mnemonics_visible),
        }
    }

    pub fn modal(self, modal: bool) -> Self {
        Self {
            builder: self.builder.property("modal", modal),
        }
    }

    pub fn resizable(self, resizable: bool) -> Self {
        Self {
            builder: self.builder.property("resizable", resizable),
        }
    }

    pub fn startup_id(self, startup_id: impl Into<glib::GString>) -> Self {
        Self {
            builder: self.builder.property("startup-id", startup_id.into()),
        }
    }

    pub fn title(self, title: impl Into<glib::GString>) -> Self {
        Self {
            builder: self.builder.property("title", title.into()),
        }
    }

    #[cfg(feature = "gtk_v4_6")]
    #[cfg_attr(docsrs, doc(cfg(feature = "gtk_v4_6")))]
    pub fn titlebar(self, titlebar: &impl IsA<gtk::Widget>) -> Self {
        Self {
            builder: self.builder.property("titlebar", titlebar.clone().upcast()),
        }
    }

    pub fn transient_for(self, transient_for: &impl IsA<gtk::Window>) -> Self {
        Self {
            builder: self
                .builder
                .property("transient-for", transient_for.clone().upcast()),
        }
    }

    pub fn can_focus(self, can_focus: bool) -> Self {
        Self {
            builder: self.builder.property("can-focus", can_focus),
        }
    }

    pub fn can_target(self, can_target: bool) -> Self {
        Self {
            builder: self.builder.property("can-target", can_target),
        }
    }

    pub fn css_classes(self, css_classes: impl Into<glib::StrV>) -> Self {
        Self {
            builder: self.builder.property("css-classes", css_classes.into()),
        }
    }

    pub fn css_name(self, css_name: impl Into<glib::GString>) -> Self {
        Self {
            builder: self.builder.property("css-name", css_name.into()),
        }
    }

    //pub fn cursor(self, cursor: /*Ignored*/&gdk::Cursor) -> Self {
    //    Self { builder: self.builder.property("cursor", cursor), }
    //}

    pub fn focus_on_click(self, focus_on_click: bool) -> Self {
        Self {
            builder: self.builder.property("focus-on-click", focus_on_click),
        }
    }

    pub fn focusable(self, focusable: bool) -> Self {
        Self {
            builder: self.builder.property("focusable", focusable),
        }
    }

    //pub fn halign(self, halign: /*Ignored*/gtk::Align) -> Self {
    //    Self { builder: self.builder.property("halign", halign), }
    //}

    pub fn has_tooltip(self, has_tooltip: bool) -> Self {
        Self {
            builder: self.builder.property("has-tooltip", has_tooltip),
        }
    }

    pub fn height_request(self, height_request: i32) -> Self {
        Self {
            builder: self.builder.property("height-request", height_request),
        }
    }

    pub fn hexpand(self, hexpand: bool) -> Self {
        Self {
            builder: self.builder.property("hexpand", hexpand),
        }
    }

    pub fn hexpand_set(self, hexpand_set: bool) -> Self {
        Self {
            builder: self.builder.property("hexpand-set", hexpand_set),
        }
    }

    //pub fn layout_manager(self, layout_manager: &impl IsA</*Ignored*/gtk::LayoutManager>) -> Self {
    //    Self { builder: self.builder.property("layout-manager", layout_manager.clone().upcast()), }
    //}

    pub fn margin_bottom(self, margin_bottom: i32) -> Self {
        Self {
            builder: self.builder.property("margin-bottom", margin_bottom),
        }
    }

    pub fn margin_end(self, margin_end: i32) -> Self {
        Self {
            builder: self.builder.property("margin-end", margin_end),
        }
    }

    pub fn margin_start(self, margin_start: i32) -> Self {
        Self {
            builder: self.builder.property("margin-start", margin_start),
        }
    }

    pub fn margin_top(self, margin_top: i32) -> Self {
        Self {
            builder: self.builder.property("margin-top", margin_top),
        }
    }

    pub fn name(self, name: impl Into<glib::GString>) -> Self {
        Self {
            builder: self.builder.property("name", name.into()),
        }
    }

    pub fn opacity(self, opacity: f64) -> Self {
        Self {
            builder: self.builder.property("opacity", opacity),
        }
    }

    //pub fn overflow(self, overflow: /*Ignored*/gtk::Overflow) -> Self {
    //    Self { builder: self.builder.property("overflow", overflow), }
    //}

    pub fn receives_default(self, receives_default: bool) -> Self {
        Self {
            builder: self.builder.property("receives-default", receives_default),
        }
    }

    pub fn sensitive(self, sensitive: bool) -> Self {
        Self {
            builder: self.builder.property("sensitive", sensitive),
        }
    }

    pub fn tooltip_markup(self, tooltip_markup: impl Into<glib::GString>) -> Self {
        Self {
            builder: self
                .builder
                .property("tooltip-markup", tooltip_markup.into()),
        }
    }

    pub fn tooltip_text(self, tooltip_text: impl Into<glib::GString>) -> Self {
        Self {
            builder: self.builder.property("tooltip-text", tooltip_text.into()),
        }
    }

    //pub fn valign(self, valign: /*Ignored*/gtk::Align) -> Self {
    //    Self { builder: self.builder.property("valign", valign), }
    //}

    pub fn vexpand(self, vexpand: bool) -> Self {
        Self {
            builder: self.builder.property("vexpand", vexpand),
        }
    }

    pub fn vexpand_set(self, vexpand_set: bool) -> Self {
        Self {
            builder: self.builder.property("vexpand-set", vexpand_set),
        }
    }

    pub fn visible(self, visible: bool) -> Self {
        Self {
            builder: self.builder.property("visible", visible),
        }
    }

    pub fn width_request(self, width_request: i32) -> Self {
        Self {
            builder: self.builder.property("width-request", width_request),
        }
    }

    //pub fn accessible_role(self, accessible_role: /*Ignored*/gtk::AccessibleRole) -> Self {
    //    Self { builder: self.builder.property("accessible-role", accessible_role), }
    //}

    // rustdoc-stripper-ignore-next
    /// Build the [`Window`].
    #[must_use = "Building the object from the builder is usually expensive and is not expected to have side effects"]
    pub fn build(self) -> Window {
        self.builder.build()
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::Window>> Sealed for T {}
}

pub trait WindowExt: IsA<Window> + sealed::Sealed + 'static {
    #[doc(alias = "pps_window_focus_view")]
    fn focus_view(&self) {
        unsafe {
            ffi::pps_window_focus_view(self.as_ref().to_glib_none().0);
        }
    }

    #[doc(alias = "pps_window_get_dbus_object_path")]
    #[doc(alias = "get_dbus_object_path")]
    fn dbus_object_path(&self) -> Option<glib::GString> {
        unsafe {
            from_glib_none(ffi::pps_window_get_dbus_object_path(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    //#[doc(alias = "pps_window_get_metadata")]
    //#[doc(alias = "get_metadata")]
    //fn metadata(&self) -> /*Ignored*/Option<Metadata> {
    //    unsafe { TODO: call ffi:pps_window_get_metadata() }
    //}

    #[doc(alias = "pps_window_get_toolbar")]
    #[doc(alias = "get_toolbar")]
    fn toolbar(&self) -> Option<adw::HeaderBar> {
        unsafe { from_glib_none(ffi::pps_window_get_toolbar(self.as_ref().to_glib_none().0)) }
    }

    #[doc(alias = "pps_window_get_uri")]
    #[doc(alias = "get_uri")]
    fn uri(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_window_get_uri(self.as_ref().to_glib_none().0)) }
    }

    #[doc(alias = "pps_window_handle_annot_popup")]
    fn handle_annot_popup(&self, annot: &impl IsA<papers_document::Annotation>) {
        unsafe {
            ffi::pps_window_handle_annot_popup(
                self.as_ref().to_glib_none().0,
                annot.as_ref().to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_window_is_empty")]
    fn is_empty(&self) -> bool {
        unsafe { from_glib(ffi::pps_window_is_empty(self.as_ref().to_glib_none().0)) }
    }

    #[doc(alias = "pps_window_open_document")]
    fn open_document(
        &self,
        document: &impl IsA<papers_document::Document>,
        dest: &papers_document::LinkDest,
        mode: WindowRunMode,
        search_string: &str,
    ) {
        unsafe {
            ffi::pps_window_open_document(
                self.as_ref().to_glib_none().0,
                document.as_ref().to_glib_none().0,
                dest.to_glib_none().0,
                mode.into_glib(),
                search_string.to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_window_open_uri")]
    fn open_uri(
        &self,
        uri: &str,
        dest: Option<&papers_document::LinkDest>,
        mode: WindowRunMode,
        search_string: Option<&str>,
    ) {
        unsafe {
            ffi::pps_window_open_uri(
                self.as_ref().to_glib_none().0,
                uri.to_glib_none().0,
                dest.to_glib_none().0,
                mode.into_glib(),
                search_string.to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_window_print_range")]
    fn print_range(&self, first_page: i32, last_page: i32) {
        unsafe {
            ffi::pps_window_print_range(self.as_ref().to_glib_none().0, first_page, last_page);
        }
    }

    #[doc(alias = "pps_window_start_page_selector_search")]
    fn start_page_selector_search(&self) {
        unsafe {
            ffi::pps_window_start_page_selector_search(self.as_ref().to_glib_none().0);
        }
    }
}

impl<O: IsA<Window>> WindowExt for O {}
