// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::ffi;
use glib::translate::*;

glib::wrapper! {
    #[doc(alias = "PpsPage")]
    pub struct Page(Object<ffi::PpsPage, ffi::PpsPageClass>);

    match fn {
        type_ => || ffi::pps_page_get_type(),
    }
}

impl Page {
    pub const NONE: Option<&'static Page> = None;

    #[doc(alias = "pps_page_new")]
    pub fn new(index: i32) -> Page {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_page_new(index)) }
    }
}
