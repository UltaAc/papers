// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{ffi, WindowRunMode};
use glib::translate::*;

//#[doc(alias = "pps_gdk_pixbuf_format_by_extension")]
//pub fn gdk_pixbuf_format_by_extension(uri: &str) -> /*Ignored*/Option<gdk_pixbuf::PixbufFormat> {
//    unsafe { TODO: call ffi:pps_gdk_pixbuf_format_by_extension() }
//}

//#[doc(alias = "pps_get_resource")]
//#[doc(alias = "get_resource")]
//pub fn resource() -> /*Ignored*/Option<gio::Resource> {
//    unsafe { TODO: call ffi:pps_get_resource() }
//}

//#[doc(alias = "pps_print_region_contents")]
//pub fn print_region_contents(region: /*Ignored*/&mut cairo::Region) {
//    unsafe { TODO: call ffi:pps_print_region_contents() }
//}

#[doc(alias = "pps_spawn")]
pub fn spawn(uri: Option<&str>, dest: Option<&papers_document::LinkDest>, mode: WindowRunMode) {
    assert_initialized_main_thread!();
    unsafe {
        ffi::pps_spawn(
            uri.to_glib_none().0,
            dest.to_glib_none().0,
            mode.into_glib(),
        );
    }
}
