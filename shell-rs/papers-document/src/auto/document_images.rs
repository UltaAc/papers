// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{ffi, Image, MappingList, Page};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsDocumentImages")]
    pub struct DocumentImages(Interface<ffi::PpsDocumentImages, ffi::PpsDocumentImagesInterface>);

    match fn {
        type_ => || ffi::pps_document_images_get_type(),
    }
}

impl DocumentImages {
    pub const NONE: Option<&'static DocumentImages> = None;
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::DocumentImages>> Sealed for T {}
}

pub trait DocumentImagesExt: IsA<DocumentImages> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_images_get_image")]
    #[doc(alias = "get_image")]
    fn image(&self, image: &impl IsA<Image>) -> Option<gdk_pixbuf::Pixbuf> {
        unsafe {
            from_glib_full(ffi::pps_document_images_get_image(
                self.as_ref().to_glib_none().0,
                image.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_images_get_image_mapping")]
    #[doc(alias = "get_image_mapping")]
    fn image_mapping(&self, page: &impl IsA<Page>) -> Option<MappingList> {
        unsafe {
            from_glib_full(ffi::pps_document_images_get_image_mapping(
                self.as_ref().to_glib_none().0,
                page.as_ref().to_glib_none().0,
            ))
        }
    }
}

impl<O: IsA<DocumentImages>> DocumentImagesExt for O {}
