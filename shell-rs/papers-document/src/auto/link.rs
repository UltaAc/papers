// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::LinkAction;
use glib::translate::*;

glib::wrapper! {
    #[doc(alias = "PpsLink")]
    pub struct Link(Object<ffi::PpsLink, ffi::PpsLinkClass>);

    match fn {
        type_ => || ffi::pps_link_get_type(),
    }
}

impl Link {
    #[doc(alias = "pps_link_new")]
    pub fn new(title: &str, action: &LinkAction) -> Link {
        skip_assert_initialized!();
        unsafe {
            from_glib_full(ffi::pps_link_new(
                title.to_glib_none().0,
                action.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_get_action")]
    #[doc(alias = "get_action")]
    pub fn action(&self) -> Option<LinkAction> {
        unsafe { from_glib_none(ffi::pps_link_get_action(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_get_title")]
    #[doc(alias = "get_title")]
    pub fn title(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_link_get_title(self.to_glib_none().0)) }
    }
}
