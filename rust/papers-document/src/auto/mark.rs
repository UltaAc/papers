// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::ffi;
use glib::translate::*;

glib::wrapper! {
    pub struct Mark(BoxedInline<ffi::PpsMark>);

    match fn {
        copy => |ptr| glib::gobject_ffi::g_boxed_copy(ffi::pps_mark_get_type(), ptr as *mut _) as *mut ffi::PpsMark,
        free => |ptr| glib::gobject_ffi::g_boxed_free(ffi::pps_mark_get_type(), ptr as *mut _),
        type_ => || ffi::pps_mark_get_type(),
    }
}
