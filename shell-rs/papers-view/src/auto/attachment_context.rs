// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{ffi, DocumentModel};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsAttachmentContext")]
    pub struct AttachmentContext(Object<ffi::PpsAttachmentContext, ffi::PpsAttachmentContextClass>);

    match fn {
        type_ => || ffi::pps_attachment_context_get_type(),
    }
}

impl AttachmentContext {
    pub const NONE: Option<&'static AttachmentContext> = None;

    #[doc(alias = "pps_attachment_context_new")]
    pub fn new(model: &DocumentModel) -> AttachmentContext {
        skip_assert_initialized!();
        unsafe { from_glib_full(ffi::pps_attachment_context_new(model.to_glib_none().0)) }
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::AttachmentContext>> Sealed for T {}
}

pub trait AttachmentContextExt: IsA<AttachmentContext> + sealed::Sealed + 'static {
    #[doc(alias = "pps_attachment_context_get_model")]
    #[doc(alias = "get_model")]
    fn model(&self) -> Option<gio::ListModel> {
        unsafe {
            from_glib_none(ffi::pps_attachment_context_get_model(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    //#[doc(alias = "pps_attachment_context_save_attachments_async")]
    //fn save_attachments_async<P: FnOnce(Result<(), glib::Error>) + 'static>(&self, attachments: impl IsA<gio::ListModel>, parent: Option<&impl IsA<gtk::Window>>, cancellable: /*Ignored*/Option<&gio::Cancellable>, callback: P) {
    //    unsafe { TODO: call ffi:pps_attachment_context_save_attachments_async() }
    //}

    //
    //fn save_attachments_future(&self, attachments: impl IsA<gio::ListModel> + Clone + 'static, parent: Option<&(impl IsA<gtk::Window> + Clone + 'static)>) -> Pin<Box_<dyn std::future::Future<Output = Result<(), glib::Error>> + 'static>> {

    //let attachments = attachments.clone();
    //let parent = parent.map(ToOwned::to_owned);
    //Box_::pin(gio::GioFuture::new(self, move |obj, cancellable, send| {
    //    obj.save_attachments_async(
    //        &attachments,
    //        parent.as_ref().map(::std::borrow::Borrow::borrow),
    //        Some(cancellable),
    //        move |res| {
    //            send.resolve(res);
    //        },
    //    );
    //}))
    //}
}

impl<O: IsA<AttachmentContext>> AttachmentContextExt for O {}
