// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{DocumentModel, JobFind};
use glib::{
    prelude::*,
    signal::{connect_raw, SignalHandlerId},
    translate::*,
};
use std::boxed::Box as Box_;

glib::wrapper! {
    #[doc(alias = "PpsView")]
    pub struct View(Object<ffi::PpsView, ffi::PpsViewClass>) @extends gtk::Widget;

    match fn {
        type_ => || ffi::pps_view_get_type(),
    }
}

impl View {
    #[doc(alias = "pps_view_new")]
    pub fn new() -> View {
        assert_initialized_main_thread!();
        unsafe { from_glib_none(ffi::pps_view_new()) }
    }

    #[doc(alias = "pps_view_add_text_markup_annotation_for_selected_text")]
    pub fn add_text_markup_annotation_for_selected_text(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_view_add_text_markup_annotation_for_selected_text(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_view_begin_add_text_annotation")]
    pub fn begin_add_text_annotation(&self) {
        unsafe {
            ffi::pps_view_begin_add_text_annotation(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_can_zoom_in")]
    pub fn can_zoom_in(&self) -> bool {
        unsafe { from_glib(ffi::pps_view_can_zoom_in(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_view_can_zoom_out")]
    pub fn can_zoom_out(&self) -> bool {
        unsafe { from_glib(ffi::pps_view_can_zoom_out(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_view_cancel_add_text_annotation")]
    pub fn cancel_add_text_annotation(&self) {
        unsafe {
            ffi::pps_view_cancel_add_text_annotation(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_copy")]
    pub fn copy(&self) {
        unsafe {
            ffi::pps_view_copy(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_copy_link_address")]
    pub fn copy_link_address(&self, action: &papers_document::LinkAction) {
        unsafe {
            ffi::pps_view_copy_link_address(self.to_glib_none().0, action.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_current_event_is_type")]
    pub fn current_event_is_type(&self, type_: gdk::EventType) -> bool {
        unsafe {
            from_glib(ffi::pps_view_current_event_is_type(
                self.to_glib_none().0,
                type_.into_glib(),
            ))
        }
    }

    #[doc(alias = "pps_view_find_cancel")]
    pub fn find_cancel(&self) {
        unsafe {
            ffi::pps_view_find_cancel(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_find_next")]
    pub fn find_next(&self) {
        unsafe {
            ffi::pps_view_find_next(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_find_previous")]
    pub fn find_previous(&self) {
        unsafe {
            ffi::pps_view_find_previous(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_find_restart")]
    pub fn find_restart(&self, page: i32) {
        unsafe {
            ffi::pps_view_find_restart(self.to_glib_none().0, page);
        }
    }

    #[doc(alias = "pps_view_find_set_highlight_search")]
    pub fn find_set_highlight_search(&self, value: bool) {
        unsafe {
            ffi::pps_view_find_set_highlight_search(self.to_glib_none().0, value.into_glib());
        }
    }

    #[doc(alias = "pps_view_find_set_result")]
    pub fn find_set_result(&self, page: i32, result: i32) {
        unsafe {
            ffi::pps_view_find_set_result(self.to_glib_none().0, page, result);
        }
    }

    #[doc(alias = "pps_view_find_started")]
    pub fn find_started(&self, job: &impl IsA<JobFind>) {
        unsafe {
            ffi::pps_view_find_started(self.to_glib_none().0, job.as_ref().to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_get_allow_links_change_zoom")]
    #[doc(alias = "get_allow_links_change_zoom")]
    pub fn allows_links_change_zoom(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_view_get_allow_links_change_zoom(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_view_get_enable_spellchecking")]
    #[doc(alias = "get_enable_spellchecking")]
    pub fn enables_spellchecking(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_view_get_enable_spellchecking(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_view_get_page_extents")]
    #[doc(alias = "get_page_extents")]
    pub fn is_page_extents(
        &self,
        page: i32,
        page_area: &mut gdk::Rectangle,
        border: &mut gtk::Border,
    ) -> bool {
        unsafe {
            from_glib(ffi::pps_view_get_page_extents(
                self.to_glib_none().0,
                page,
                page_area.to_glib_none_mut().0,
                border.to_glib_none_mut().0,
            ))
        }
    }

    #[doc(alias = "pps_view_get_page_extents_for_border")]
    #[doc(alias = "get_page_extents_for_border")]
    pub fn is_page_extents_for_border(
        &self,
        page: i32,
        border: &mut gtk::Border,
        page_area: &mut gdk::Rectangle,
    ) -> bool {
        unsafe {
            from_glib(ffi::pps_view_get_page_extents_for_border(
                self.to_glib_none().0,
                page,
                border.to_glib_none_mut().0,
                page_area.to_glib_none_mut().0,
            ))
        }
    }

    #[doc(alias = "pps_view_get_selected_text")]
    #[doc(alias = "get_selected_text")]
    pub fn selected_text(&self) -> Option<glib::GString> {
        unsafe { from_glib_full(ffi::pps_view_get_selected_text(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_view_handle_link")]
    pub fn handle_link(&self, link: &papers_document::Link) {
        unsafe {
            ffi::pps_view_handle_link(self.to_glib_none().0, link.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_has_selection")]
    pub fn has_selection(&self) -> bool {
        unsafe { from_glib(ffi::pps_view_has_selection(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_view_is_caret_navigation_enabled")]
    pub fn is_caret_navigation_enabled(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_view_is_caret_navigation_enabled(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_view_is_loading")]
    pub fn is_loading(&self) -> bool {
        unsafe { from_glib(ffi::pps_view_is_loading(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_view_next_page")]
    pub fn next_page(&self) -> bool {
        unsafe { from_glib(ffi::pps_view_next_page(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_view_previous_page")]
    pub fn previous_page(&self) -> bool {
        unsafe { from_glib(ffi::pps_view_previous_page(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_view_reload")]
    pub fn reload(&self) {
        unsafe {
            ffi::pps_view_reload(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_remove_annotation")]
    pub fn remove_annotation(&self, annot: &impl IsA<papers_document::Annotation>) {
        unsafe {
            ffi::pps_view_remove_annotation(self.to_glib_none().0, annot.as_ref().to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_select_all")]
    pub fn select_all(&self) {
        unsafe {
            ffi::pps_view_select_all(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_set_allow_links_change_zoom")]
    pub fn set_allow_links_change_zoom(&self, allowed: bool) {
        unsafe {
            ffi::pps_view_set_allow_links_change_zoom(self.to_glib_none().0, allowed.into_glib());
        }
    }

    #[doc(alias = "pps_view_set_caret_cursor_position")]
    pub fn set_caret_cursor_position(&self, page: u32, offset: u32) {
        unsafe {
            ffi::pps_view_set_caret_cursor_position(self.to_glib_none().0, page, offset);
        }
    }

    #[doc(alias = "pps_view_set_caret_navigation_enabled")]
    pub fn set_caret_navigation_enabled(&self, enabled: bool) {
        unsafe {
            ffi::pps_view_set_caret_navigation_enabled(self.to_glib_none().0, enabled.into_glib());
        }
    }

    #[doc(alias = "pps_view_set_enable_spellchecking")]
    pub fn set_enable_spellchecking(&self, spellcheck: bool) {
        unsafe {
            ffi::pps_view_set_enable_spellchecking(self.to_glib_none().0, spellcheck.into_glib());
        }
    }

    #[doc(alias = "pps_view_set_model")]
    pub fn set_model(&self, model: &DocumentModel) {
        unsafe {
            ffi::pps_view_set_model(self.to_glib_none().0, model.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_set_page_cache_size")]
    pub fn set_page_cache_size(&self, cache_size: usize) {
        unsafe {
            ffi::pps_view_set_page_cache_size(self.to_glib_none().0, cache_size);
        }
    }

    #[doc(alias = "pps_view_supports_caret_navigation")]
    pub fn supports_caret_navigation(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_view_supports_caret_navigation(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_view_zoom_in")]
    pub fn zoom_in(&self) {
        unsafe {
            ffi::pps_view_zoom_in(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_zoom_out")]
    pub fn zoom_out(&self) {
        unsafe {
            ffi::pps_view_zoom_out(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_view_get_resource")]
    #[doc(alias = "get_resource")]
    pub fn resource() -> Option<gio::Resource> {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_view_get_resource()) }
    }

    #[doc(alias = "activate")]
    pub fn connect_activate<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn activate_trampoline<F: Fn(&View) + 'static>(
            this: *mut ffi::PpsView,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"activate\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    activate_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_activate(&self) {
        self.emit_by_name::<()>("activate", &[]);
    }

    #[doc(alias = "annot-added")]
    pub fn connect_annot_added<F: Fn(&Self, &papers_document::Annotation) + 'static>(
        &self,
        f: F,
    ) -> SignalHandlerId {
        unsafe extern "C" fn annot_added_trampoline<
            F: Fn(&View, &papers_document::Annotation) + 'static,
        >(
            this: *mut ffi::PpsView,
            object: *mut papers_document::ffi::PpsAnnotation,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this), &from_glib_borrow(object))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"annot-added\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    annot_added_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_annot_added(&self, object: &papers_document::Annotation) {
        self.emit_by_name::<()>("annot-added", &[&object]);
    }

    #[doc(alias = "annot-removed")]
    pub fn connect_annot_removed<F: Fn(&Self, &papers_document::Annotation) + 'static>(
        &self,
        f: F,
    ) -> SignalHandlerId {
        unsafe extern "C" fn annot_removed_trampoline<
            F: Fn(&View, &papers_document::Annotation) + 'static,
        >(
            this: *mut ffi::PpsView,
            object: *mut papers_document::ffi::PpsAnnotation,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this), &from_glib_borrow(object))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"annot-removed\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    annot_removed_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_annot_removed(&self, object: &papers_document::Annotation) {
        self.emit_by_name::<()>("annot-removed", &[&object]);
    }

    #[doc(alias = "cursor-moved")]
    pub fn connect_cursor_moved<F: Fn(&Self, i32, i32) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn cursor_moved_trampoline<F: Fn(&View, i32, i32) + 'static>(
            this: *mut ffi::PpsView,
            object: libc::c_int,
            p0: libc::c_int,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this), object, p0)
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"cursor-moved\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    cursor_moved_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    #[doc(alias = "external-link")]
    pub fn connect_external_link<F: Fn(&Self, &papers_document::LinkAction) + 'static>(
        &self,
        f: F,
    ) -> SignalHandlerId {
        unsafe extern "C" fn external_link_trampoline<
            F: Fn(&View, &papers_document::LinkAction) + 'static,
        >(
            this: *mut ffi::PpsView,
            object: *mut papers_document::ffi::PpsLinkAction,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this), &from_glib_borrow(object))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"external-link\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    external_link_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_external_link(&self, object: &papers_document::LinkAction) {
        self.emit_by_name::<()>("external-link", &[&object]);
    }

    #[doc(alias = "handle-link")]
    pub fn connect_handle_link<F: Fn(&Self, &glib::Object, &glib::Object) + 'static>(
        &self,
        f: F,
    ) -> SignalHandlerId {
        unsafe extern "C" fn handle_link_trampoline<
            F: Fn(&View, &glib::Object, &glib::Object) + 'static,
        >(
            this: *mut ffi::PpsView,
            object: *mut glib::gobject_ffi::GObject,
            p0: *mut glib::gobject_ffi::GObject,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(
                &from_glib_borrow(this),
                &from_glib_borrow(object),
                &from_glib_borrow(p0),
            )
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"handle-link\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    handle_link_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_handle_link(&self, object: &glib::Object, p0: &glib::Object) {
        self.emit_by_name::<()>("handle-link", &[&object, &p0]);
    }

    #[doc(alias = "layers-changed")]
    pub fn connect_layers_changed<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn layers_changed_trampoline<F: Fn(&View) + 'static>(
            this: *mut ffi::PpsView,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"layers-changed\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    layers_changed_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_layers_changed(&self) {
        self.emit_by_name::<()>("layers-changed", &[]);
    }

    #[doc(alias = "move-cursor")]
    pub fn connect_move_cursor<F: Fn(&Self, gtk::MovementStep, i32, bool) -> bool + 'static>(
        &self,
        f: F,
    ) -> SignalHandlerId {
        unsafe extern "C" fn move_cursor_trampoline<
            F: Fn(&View, gtk::MovementStep, i32, bool) -> bool + 'static,
        >(
            this: *mut ffi::PpsView,
            object: gtk::ffi::GtkMovementStep,
            p0: libc::c_int,
            p1: glib::ffi::gboolean,
            f: glib::ffi::gpointer,
        ) -> glib::ffi::gboolean {
            let f: &F = &*(f as *const F);
            f(
                &from_glib_borrow(this),
                from_glib(object),
                p0,
                from_glib(p1),
            )
            .into_glib()
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"move-cursor\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    move_cursor_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_move_cursor(&self, object: gtk::MovementStep, p0: i32, p1: bool) -> bool {
        self.emit_by_name("move-cursor", &[&object, &p0, &p1])
    }

    #[doc(alias = "scroll")]
    pub fn connect_scroll<F: Fn(&Self, gtk::ScrollType, gtk::Orientation) -> bool + 'static>(
        &self,
        f: F,
    ) -> SignalHandlerId {
        unsafe extern "C" fn scroll_trampoline<
            F: Fn(&View, gtk::ScrollType, gtk::Orientation) -> bool + 'static,
        >(
            this: *mut ffi::PpsView,
            object: gtk::ffi::GtkScrollType,
            p0: gtk::ffi::GtkOrientation,
            f: glib::ffi::gpointer,
        ) -> glib::ffi::gboolean {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this), from_glib(object), from_glib(p0)).into_glib()
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"scroll\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    scroll_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_scroll(&self, object: gtk::ScrollType, p0: gtk::Orientation) -> bool {
        self.emit_by_name("scroll", &[&object, &p0])
    }

    #[doc(alias = "selection-changed")]
    pub fn connect_selection_changed<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn selection_changed_trampoline<F: Fn(&View) + 'static>(
            this: *mut ffi::PpsView,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"selection-changed\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    selection_changed_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    pub fn emit_selection_changed(&self) {
        self.emit_by_name::<()>("selection-changed", &[]);
    }

    #[doc(alias = "can-zoom-in")]
    pub fn connect_can_zoom_in_notify<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn notify_can_zoom_in_trampoline<F: Fn(&View) + 'static>(
            this: *mut ffi::PpsView,
            _param_spec: glib::ffi::gpointer,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"notify::can-zoom-in\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    notify_can_zoom_in_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    #[doc(alias = "can-zoom-out")]
    pub fn connect_can_zoom_out_notify<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn notify_can_zoom_out_trampoline<F: Fn(&View) + 'static>(
            this: *mut ffi::PpsView,
            _param_spec: glib::ffi::gpointer,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"notify::can-zoom-out\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    notify_can_zoom_out_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    #[doc(alias = "is-loading")]
    pub fn connect_is_loading_notify<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn notify_is_loading_trampoline<F: Fn(&View) + 'static>(
            this: *mut ffi::PpsView,
            _param_spec: glib::ffi::gpointer,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"notify::is-loading\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    notify_is_loading_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }
}

impl Default for View {
    fn default() -> Self {
        Self::new()
    }
}
