// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::ffi;
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsDocumentFonts")]
    pub struct DocumentFonts(Interface<ffi::PpsDocumentFonts, ffi::PpsDocumentFontsInterface>);

    match fn {
        type_ => || ffi::pps_document_fonts_get_type(),
    }
}

impl DocumentFonts {
    pub const NONE: Option<&'static DocumentFonts> = None;
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::DocumentFonts>> Sealed for T {}
}

pub trait DocumentFontsExt: IsA<DocumentFonts> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_fonts_get_fonts_summary")]
    #[doc(alias = "get_fonts_summary")]
    fn fonts_summary(&self) -> Option<glib::GString> {
        unsafe {
            from_glib_none(ffi::pps_document_fonts_get_fonts_summary(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_fonts_get_model")]
    #[doc(alias = "get_model")]
    fn model(&self) -> Option<gio::ListModel> {
        unsafe {
            from_glib_full(ffi::pps_document_fonts_get_model(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_fonts_scan")]
    fn scan(&self) {
        unsafe {
            ffi::pps_document_fonts_scan(self.as_ref().to_glib_none().0);
        }
    }
}

impl<O: IsA<DocumentFonts>> DocumentFontsExt for O {}
