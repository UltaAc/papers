// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{Link,LinkDest};
use glib::{prelude::*,translate::*};

glib::wrapper! {
    #[doc(alias = "PpsDocumentLinks")]
    pub struct DocumentLinks(Interface<ffi::PpsDocumentLinks, ffi::PpsDocumentLinksInterface>);

    match fn {
        type_ => || ffi::pps_document_links_get_type(),
    }
}

impl DocumentLinks {
        pub const NONE: Option<&'static DocumentLinks> = None;
    
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::DocumentLinks>> Sealed for T {}
}

pub trait DocumentLinksExt: IsA<DocumentLinks> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_links_find_link_dest")]
    fn find_link_dest(&self, link_name: &str) -> Option<LinkDest> {
        unsafe {
            from_glib_full(ffi::pps_document_links_find_link_dest(self.as_ref().to_glib_none().0, link_name.to_glib_none().0))
        }
    }

    #[doc(alias = "pps_document_links_find_link_page")]
    fn find_link_page(&self, link_name: &str) -> i32 {
        unsafe {
            ffi::pps_document_links_find_link_page(self.as_ref().to_glib_none().0, link_name.to_glib_none().0)
        }
    }

    #[doc(alias = "pps_document_links_get_dest_page")]
    #[doc(alias = "get_dest_page")]
    fn dest_page(&self, dest: &LinkDest) -> i32 {
        unsafe {
            ffi::pps_document_links_get_dest_page(self.as_ref().to_glib_none().0, dest.to_glib_none().0)
        }
    }

    #[doc(alias = "pps_document_links_get_dest_page_label")]
    #[doc(alias = "get_dest_page_label")]
    fn dest_page_label(&self, dest: &LinkDest) -> Option<glib::GString> {
        unsafe {
            from_glib_full(ffi::pps_document_links_get_dest_page_label(self.as_ref().to_glib_none().0, dest.to_glib_none().0))
        }
    }

    #[doc(alias = "pps_document_links_get_link_page")]
    #[doc(alias = "get_link_page")]
    fn link_page(&self, link: &Link) -> i32 {
        unsafe {
            ffi::pps_document_links_get_link_page(self.as_ref().to_glib_none().0, link.to_glib_none().0)
        }
    }

    #[doc(alias = "pps_document_links_get_link_page_label")]
    #[doc(alias = "get_link_page_label")]
    fn link_page_label(&self, link: &Link) -> Option<glib::GString> {
        unsafe {
            from_glib_full(ffi::pps_document_links_get_link_page_label(self.as_ref().to_glib_none().0, link.to_glib_none().0))
        }
    }

    //#[doc(alias = "pps_document_links_get_links")]
    //#[doc(alias = "get_links")]
    //fn links(&self, page: &impl IsA<Page>) -> /*Ignored*/Option<MappingList> {
    //    unsafe { TODO: call ffi:pps_document_links_get_links() }
    //}

    #[doc(alias = "pps_document_links_get_links_model")]
    #[doc(alias = "get_links_model")]
    fn links_model(&self) -> Option<gio::ListModel> {
        unsafe {
            from_glib_full(ffi::pps_document_links_get_links_model(self.as_ref().to_glib_none().0))
        }
    }

    #[doc(alias = "pps_document_links_has_document_links")]
    fn has_document_links(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_links_has_document_links(self.as_ref().to_glib_none().0))
        }
    }
}

impl<O: IsA<DocumentLinks>> DocumentLinksExt for O {}
