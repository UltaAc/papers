// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::ffi;
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsFontDescription")]
    pub struct FontDescription(Object<ffi::PpsFontDescription, ffi::PpsFontDescriptionClass>);

    match fn {
        type_ => || ffi::pps_font_description_get_type(),
    }
}

impl FontDescription {
    pub const NONE: Option<&'static FontDescription> = None;

    #[doc(alias = "pps_font_description_new")]
    pub fn new() -> FontDescription {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_font_description_new()) }
    }
}

impl Default for FontDescription {
    fn default() -> Self {
        Self::new()
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::FontDescription>> Sealed for T {}
}

pub trait FontDescriptionExt: IsA<FontDescription> + sealed::Sealed + 'static {
    fn details(&self) -> Option<glib::GString> {
        ObjectExt::property(self.as_ref(), "details")
    }

    fn name(&self) -> Option<glib::GString> {
        ObjectExt::property(self.as_ref(), "name")
    }
}

impl<O: IsA<FontDescription>> FontDescriptionExt for O {}
