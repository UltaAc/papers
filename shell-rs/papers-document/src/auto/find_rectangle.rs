// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use glib::translate::*;

glib::wrapper! {
    #[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Hash)]
    pub struct FindRectangle(Boxed<ffi::PpsFindRectangle>);

    match fn {
        copy => |ptr| ffi::pps_find_rectangle_copy(mut_override(ptr)),
        free => |ptr| ffi::pps_find_rectangle_free(ptr),
        type_ => || ffi::pps_find_rectangle_get_type(),
    }
}

impl FindRectangle {
    #[doc(alias = "pps_find_rectangle_new")]
    pub fn new() -> FindRectangle {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_find_rectangle_new()) }
    }
}

impl Default for FindRectangle {
    fn default() -> Self {
        Self::new()
    }
}
