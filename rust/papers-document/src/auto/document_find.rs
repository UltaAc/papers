// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::{ffi, FindOptions, FindRectangle, Page};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsDocumentFind")]
    pub struct DocumentFind(Interface<ffi::PpsDocumentFind, ffi::PpsDocumentFindInterface>);

    match fn {
        type_ => || ffi::pps_document_find_get_type(),
    }
}

impl DocumentFind {
    pub const NONE: Option<&'static DocumentFind> = None;
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::DocumentFind>> Sealed for T {}
}

pub trait DocumentFindExt: IsA<DocumentFind> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_find_find_text")]
    fn find_text(
        &self,
        page: &impl IsA<Page>,
        text: &str,
        options: FindOptions,
    ) -> Vec<FindRectangle> {
        unsafe {
            FromGlibPtrContainer::from_glib_full(ffi::pps_document_find_find_text(
                self.as_ref().to_glib_none().0,
                page.as_ref().to_glib_none().0,
                text.to_glib_none().0,
                options.into_glib(),
            ))
        }
    }

    #[doc(alias = "pps_document_find_get_supported_options")]
    #[doc(alias = "get_supported_options")]
    fn supported_options(&self) -> FindOptions {
        unsafe {
            from_glib(ffi::pps_document_find_get_supported_options(
                self.as_ref().to_glib_none().0,
            ))
        }
    }
}

impl<O: IsA<DocumentFind>> DocumentFindExt for O {}
