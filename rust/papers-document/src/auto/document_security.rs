// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::ffi;
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsDocumentSecurity")]
    pub struct DocumentSecurity(Interface<ffi::PpsDocumentSecurity, ffi::PpsDocumentSecurityInterface>);

    match fn {
        type_ => || ffi::pps_document_security_get_type(),
    }
}

impl DocumentSecurity {
    pub const NONE: Option<&'static DocumentSecurity> = None;
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::DocumentSecurity>> Sealed for T {}
}

pub trait DocumentSecurityExt: IsA<DocumentSecurity> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_security_has_document_security")]
    fn has_document_security(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_security_has_document_security(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_security_set_password")]
    fn set_password(&self, password: &str) {
        unsafe {
            ffi::pps_document_security_set_password(
                self.as_ref().to_glib_none().0,
                password.to_glib_none().0,
            );
        }
    }
}

impl<O: IsA<DocumentSecurity>> DocumentSecurityExt for O {}
