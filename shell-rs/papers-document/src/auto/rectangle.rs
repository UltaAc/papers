// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use glib::{translate::*};

glib::wrapper! {
    pub struct Rectangle(BoxedInline<ffi::PpsRectangle>);

    match fn {
        copy => |ptr| ffi::pps_rectangle_copy(mut_override(ptr)),
        free => |ptr| ffi::pps_rectangle_free(ptr),
        type_ => || ffi::pps_rectangle_get_type(),
    }
}

impl Rectangle {
    #[doc(alias = "pps_rectangle_new")]
    pub fn new() -> Rectangle {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_rectangle_new())
        }
    }
}

impl Default for Rectangle {
                     fn default() -> Self {
                         Self::new()
                     }
                 }
