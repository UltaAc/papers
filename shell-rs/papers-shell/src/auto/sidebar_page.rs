// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsSidebarPage")]
    pub struct SidebarPage(Object<ffi::PpsSidebarPage, ffi::PpsSidebarPageClass>) @extends gtk::Widget, @implements gtk::Accessible, gtk::Buildable, gtk::ConstraintTarget;

    match fn {
        type_ => || ffi::pps_sidebar_page_get_type(),
    }
}

impl SidebarPage {
    pub const NONE: Option<&'static SidebarPage> = None;
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::SidebarPage>> Sealed for T {}
}

pub trait SidebarPageExt: IsA<SidebarPage> + sealed::Sealed + 'static {
    #[doc(alias = "pps_sidebar_page_get_document_model")]
    #[doc(alias = "get_document_model")]
    fn document_model(&self) -> Option<papers_view::DocumentModel> {
        unsafe {
            from_glib_none(ffi::pps_sidebar_page_get_document_model(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_sidebar_page_support_document")]
    fn support_document(&self, document: &impl IsA<papers_document::Document>) -> bool {
        unsafe {
            from_glib(ffi::pps_sidebar_page_support_document(
                self.as_ref().to_glib_none().0,
                document.as_ref().to_glib_none().0,
            ))
        }
    }
}

impl<O: IsA<SidebarPage>> SidebarPageExt for O {}
