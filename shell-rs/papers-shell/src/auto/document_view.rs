// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::ffi;
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsDocumentView")]
    pub struct DocumentView(Object<ffi::PpsDocumentView, ffi::PpsDocumentViewClass>) @extends gtk::Widget, @implements gtk::Accessible, gtk::Buildable, gtk::ConstraintTarget;

    match fn {
        type_ => || ffi::pps_document_view_get_type(),
    }
}

impl DocumentView {
    pub const NONE: Option<&'static DocumentView> = None;

    #[doc(alias = "pps_document_view_new")]
    pub fn new() -> DocumentView {
        assert_initialized_main_thread!();
        unsafe { from_glib_none(ffi::pps_document_view_new()) }
    }

    // rustdoc-stripper-ignore-next
    /// Creates a new builder-pattern struct instance to construct [`DocumentView`] objects.
    ///
    /// This method returns an instance of [`DocumentViewBuilder`](crate::builders::DocumentViewBuilder) which can be used to create [`DocumentView`] objects.
    pub fn builder() -> DocumentViewBuilder {
        DocumentViewBuilder::new()
    }
}

impl Default for DocumentView {
    fn default() -> Self {
        Self::new()
    }
}

// rustdoc-stripper-ignore-next
/// A [builder-pattern] type to construct [`DocumentView`] objects.
///
/// [builder-pattern]: https://doc.rust-lang.org/1.0.0/style/ownership/builders.html
#[must_use = "The builder must be built to be used"]
pub struct DocumentViewBuilder {
    builder: glib::object::ObjectBuilder<'static, DocumentView>,
}

impl DocumentViewBuilder {
    fn new() -> Self {
        Self {
            builder: glib::object::Object::builder(),
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

    pub fn halign(self, halign: gtk::Align) -> Self {
        Self {
            builder: self.builder.property("halign", halign),
        }
    }

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

    pub fn valign(self, valign: gtk::Align) -> Self {
        Self {
            builder: self.builder.property("valign", valign),
        }
    }

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
    /// Build the [`DocumentView`].
    #[must_use = "Building the object from the builder is usually expensive and is not expected to have side effects"]
    pub fn build(self) -> DocumentView {
        self.builder.build()
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::DocumentView>> Sealed for T {}
}

pub trait DocumentViewExt: IsA<DocumentView> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_view_close_handled")]
    fn close_handled(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_view_close_handled(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_view_focus_view")]
    fn focus_view(&self) {
        unsafe {
            ffi::pps_document_view_focus_view(self.as_ref().to_glib_none().0);
        }
    }

    #[doc(alias = "pps_document_view_get_dbus_object_path")]
    #[doc(alias = "get_dbus_object_path")]
    fn dbus_object_path(&self) -> Option<glib::GString> {
        unsafe {
            from_glib_none(ffi::pps_document_view_get_dbus_object_path(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_view_get_header_bar")]
    #[doc(alias = "get_header_bar")]
    fn header_bar(&self) -> Option<adw::HeaderBar> {
        unsafe {
            from_glib_none(ffi::pps_document_view_get_header_bar(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_view_get_metadata")]
    #[doc(alias = "get_metadata")]
    fn metadata(&self) -> Option<papers_view::Metadata> {
        unsafe {
            from_glib_none(ffi::pps_document_view_get_metadata(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_view_get_model")]
    #[doc(alias = "get_model")]
    fn model(&self) -> Option<papers_view::DocumentModel> {
        unsafe {
            from_glib_none(ffi::pps_document_view_get_model(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_view_get_uri")]
    #[doc(alias = "get_uri")]
    fn uri(&self) -> Option<glib::GString> {
        unsafe {
            from_glib_none(ffi::pps_document_view_get_uri(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_view_handle_annot_popup")]
    fn handle_annot_popup(&self, annot: &impl IsA<papers_document::Annotation>) {
        unsafe {
            ffi::pps_document_view_handle_annot_popup(
                self.as_ref().to_glib_none().0,
                annot.as_ref().to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_document_view_is_empty")]
    fn is_empty(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_view_is_empty(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_view_open_document")]
    fn open_document(
        &self,
        document: &impl IsA<papers_document::Document>,
        metadata: &papers_view::Metadata,
        dest: Option<&papers_document::LinkDest>,
    ) {
        unsafe {
            ffi::pps_document_view_open_document(
                self.as_ref().to_glib_none().0,
                document.as_ref().to_glib_none().0,
                metadata.to_glib_none().0,
                dest.to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_document_view_print_range")]
    fn print_range(&self, first_page: i32, last_page: i32) {
        unsafe {
            ffi::pps_document_view_print_range(
                self.as_ref().to_glib_none().0,
                first_page,
                last_page,
            );
        }
    }

    #[doc(alias = "pps_document_view_reload_document")]
    fn reload_document(&self, document: &impl IsA<papers_document::Document>) {
        unsafe {
            ffi::pps_document_view_reload_document(
                self.as_ref().to_glib_none().0,
                document.as_ref().to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_document_view_set_document")]
    fn set_document(&self, document: &impl IsA<papers_document::Document>) {
        unsafe {
            ffi::pps_document_view_set_document(
                self.as_ref().to_glib_none().0,
                document.as_ref().to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_document_view_set_filenames")]
    fn set_filenames(&self, display_name: &str, edit_name: &str) {
        unsafe {
            ffi::pps_document_view_set_filenames(
                self.as_ref().to_glib_none().0,
                display_name.to_glib_none().0,
                edit_name.to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_document_view_set_fullscreen_mode")]
    fn set_fullscreen_mode(&self, fullscreened: bool) {
        unsafe {
            ffi::pps_document_view_set_fullscreen_mode(
                self.as_ref().to_glib_none().0,
                fullscreened.into_glib(),
            );
        }
    }

    #[doc(alias = "pps_document_view_set_inverted_colors")]
    fn set_inverted_colors(&self, inverted: bool) {
        unsafe {
            ffi::pps_document_view_set_inverted_colors(
                self.as_ref().to_glib_none().0,
                inverted.into_glib(),
            );
        }
    }
}

impl<O: IsA<DocumentView>> DocumentViewExt for O {}
