// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{ffi, LinkDestType};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsLinkDest")]
    pub struct LinkDest(Object<ffi::PpsLinkDest, ffi::PpsLinkDestClass>);

    match fn {
        type_ => || ffi::pps_link_dest_get_type(),
    }
}

impl LinkDest {
    #[doc(alias = "pps_link_dest_new_fit")]
    pub fn new_fit(page: i32) -> LinkDest {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_link_dest_new_fit(page)) }
    }

    #[doc(alias = "pps_link_dest_new_fith")]
    pub fn new_fith(page: i32, top: f64, change_top: bool) -> LinkDest {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_link_dest_new_fith(
                page,
                top,
                change_top.into_glib(),
            ))
        }
    }

    #[doc(alias = "pps_link_dest_new_fitr")]
    pub fn new_fitr(page: i32, left: f64, bottom: f64, right: f64, top: f64) -> LinkDest {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_link_dest_new_fitr(page, left, bottom, right, top)) }
    }

    #[doc(alias = "pps_link_dest_new_fitv")]
    pub fn new_fitv(page: i32, left: f64, change_left: bool) -> LinkDest {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_link_dest_new_fitv(
                page,
                left,
                change_left.into_glib(),
            ))
        }
    }

    #[doc(alias = "pps_link_dest_new_named")]
    pub fn new_named(named_dest: &str) -> LinkDest {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_link_dest_new_named(named_dest.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_dest_new_page")]
    pub fn new_page(page: i32) -> LinkDest {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_link_dest_new_page(page)) }
    }

    #[doc(alias = "pps_link_dest_new_page_label")]
    pub fn new_page_label(page_label: &str) -> LinkDest {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_link_dest_new_page_label(
                page_label.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_dest_new_xyz")]
    pub fn new_xyz(
        page: i32,
        left: f64,
        top: f64,
        zoom: f64,
        change_left: bool,
        change_top: bool,
        change_zoom: bool,
    ) -> LinkDest {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_link_dest_new_xyz(
                page,
                left,
                top,
                zoom,
                change_left.into_glib(),
                change_top.into_glib(),
                change_zoom.into_glib(),
            ))
        }
    }

    #[doc(alias = "pps_link_dest_equal")]
    pub fn equal(&self, b: &LinkDest) -> bool {
        unsafe {
            from_glib(ffi::pps_link_dest_equal(
                self.to_glib_none().0,
                b.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_dest_get_bottom")]
    #[doc(alias = "get_bottom")]
    pub fn bottom(&self) -> f64 {
        unsafe { ffi::pps_link_dest_get_bottom(self.to_glib_none().0) }
    }

    #[doc(alias = "pps_link_dest_get_dest_type")]
    #[doc(alias = "get_dest_type")]
    pub fn dest_type(&self) -> LinkDestType {
        unsafe { from_glib(ffi::pps_link_dest_get_dest_type(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_dest_get_left")]
    #[doc(alias = "get_left")]
    pub fn left(&self) -> (f64, bool) {
        unsafe {
            let mut change_left = std::mem::MaybeUninit::uninit();
            let ret = ffi::pps_link_dest_get_left(self.to_glib_none().0, change_left.as_mut_ptr());
            (ret, from_glib(change_left.assume_init()))
        }
    }

    #[doc(alias = "pps_link_dest_get_named_dest")]
    #[doc(alias = "get_named_dest")]
    pub fn named_dest(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_link_dest_get_named_dest(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_dest_get_page")]
    #[doc(alias = "get_page")]
    pub fn page(&self) -> i32 {
        unsafe { ffi::pps_link_dest_get_page(self.to_glib_none().0) }
    }

    #[doc(alias = "pps_link_dest_get_page_label")]
    #[doc(alias = "get_page_label")]
    #[doc(alias = "page-label")]
    pub fn page_label(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_link_dest_get_page_label(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_dest_get_right")]
    #[doc(alias = "get_right")]
    pub fn right(&self) -> f64 {
        unsafe { ffi::pps_link_dest_get_right(self.to_glib_none().0) }
    }

    #[doc(alias = "pps_link_dest_get_top")]
    #[doc(alias = "get_top")]
    pub fn top(&self) -> (f64, bool) {
        unsafe {
            let mut change_top = std::mem::MaybeUninit::uninit();
            let ret = ffi::pps_link_dest_get_top(self.to_glib_none().0, change_top.as_mut_ptr());
            (ret, from_glib(change_top.assume_init()))
        }
    }

    #[doc(alias = "pps_link_dest_get_zoom")]
    #[doc(alias = "get_zoom")]
    pub fn zoom(&self) -> (f64, bool) {
        unsafe {
            let mut change_zoom = std::mem::MaybeUninit::uninit();
            let ret = ffi::pps_link_dest_get_zoom(self.to_glib_none().0, change_zoom.as_mut_ptr());
            (ret, from_glib(change_zoom.assume_init()))
        }
    }

    pub fn change(&self) -> u32 {
        ObjectExt::property(self, "change")
    }

    pub fn named(&self) -> Option<glib::GString> {
        ObjectExt::property(self, "named")
    }

    #[doc(alias = "type")]
    pub fn type_(&self) -> LinkDestType {
        ObjectExt::property(self, "type")
    }
}
