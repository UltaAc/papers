// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{ffi, CertificateInfo, Signature};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsDocumentSignatures")]
    pub struct DocumentSignatures(Interface<ffi::PpsDocumentSignatures, ffi::PpsDocumentSignaturesInterface>);

    match fn {
        type_ => || ffi::pps_document_signatures_get_type(),
    }
}

impl DocumentSignatures {
    pub const NONE: Option<&'static DocumentSignatures> = None;
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::DocumentSignatures>> Sealed for T {}
}

pub trait DocumentSignaturesExt: IsA<DocumentSignatures> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_signatures_can_sign")]
    fn can_sign(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_signatures_can_sign(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_signatures_get_available_signing_certificates")]
    #[doc(alias = "get_available_signing_certificates")]
    fn available_signing_certificates(&self) -> Vec<CertificateInfo> {
        unsafe {
            FromGlibPtrContainer::from_glib_full(
                ffi::pps_document_signatures_get_available_signing_certificates(
                    self.as_ref().to_glib_none().0,
                ),
            )
        }
    }

    #[doc(alias = "pps_document_signatures_get_certificate_info")]
    #[doc(alias = "get_certificate_info")]
    fn certificate_info(&self, nick_name: &str) -> Option<CertificateInfo> {
        unsafe {
            from_glib_full(ffi::pps_document_signatures_get_certificate_info(
                self.as_ref().to_glib_none().0,
                nick_name.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_signatures_get_signatures")]
    #[doc(alias = "get_signatures")]
    fn signatures(&self) -> Vec<Signature> {
        unsafe {
            FromGlibPtrContainer::from_glib_full(ffi::pps_document_signatures_get_signatures(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_signatures_has_signatures")]
    fn has_signatures(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_signatures_has_signatures(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_signatures_set_password_callback")]
    fn set_password_callback<P: FnMut(&str) -> String>(&self, cb: P) {
        let cb_data: P = cb;
        unsafe extern "C" fn cb_func<P: FnMut(&str) -> String>(
            text: *const libc::c_char,
            user_data: glib::ffi::gpointer,
        ) -> *mut libc::c_char {
            let text: Borrowed<glib::GString> = from_glib_borrow(text);
            let callback = user_data as *mut P;
            (*callback)(text.as_str()).to_glib_full()
        }
        let cb = Some(cb_func::<P> as _);
        let super_callback0: &P = &cb_data;
        unsafe {
            ffi::pps_document_signatures_set_password_callback(
                self.as_ref().to_glib_none().0,
                cb,
                super_callback0 as *const _ as *mut _,
            );
        }
    }

    //#[doc(alias = "pps_document_signatures_sign")]
    //fn sign<P: FnOnce(Result<(), glib::Error>) + 'static>(&self, signature: &impl IsA<Signature>, cancellable: /*Ignored*/Option<&gio::Cancellable>, callback: P) -> bool {
    //    unsafe { TODO: call ffi:pps_document_signatures_sign() }
    //}

    //
    //fn sign_future(&self, signature: &(impl IsA<Signature> + Clone + 'static)) -> Pin<Box_<dyn std::future::Future<Output = Result<(), glib::Error>> + 'static>> {

    //let signature = signature.clone();
    //Box_::pin(gio::GioFuture::new(self, move |obj, cancellable, send| {
    //    obj.sign(
    //        &signature,
    //        Some(cancellable),
    //        move |res| {
    //            send.resolve(res);
    //        },
    //    );
    //}))
    //}
}

impl<O: IsA<DocumentSignatures>> DocumentSignaturesExt for O {}
