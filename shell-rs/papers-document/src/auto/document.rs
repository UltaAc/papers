// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{ffi, DocumentInfo, Page};
use glib::{
    prelude::*,
    signal::{connect_raw, SignalHandlerId},
    translate::*,
};
use std::boxed::Box as Box_;

glib::wrapper! {
    #[doc(alias = "PpsDocument")]
    pub struct Document(Object<ffi::PpsDocument, ffi::PpsDocumentClass>);

    match fn {
        type_ => || ffi::pps_document_get_type(),
    }
}

impl Document {
    pub const NONE: Option<&'static Document> = None;

    #[doc(alias = "pps_document_factory_add_filters")]
    pub fn factory_add_filters(dialog: &gtk::FileDialog, document: Option<&impl IsA<Document>>) {
        assert_initialized_main_thread!();
        unsafe {
            ffi::pps_document_factory_add_filters(
                dialog.to_glib_none().0,
                document.map(|p| p.as_ref()).to_glib_none().0,
            );
        }
    }

    #[doc(alias = "pps_document_factory_get_document")]
    pub fn factory_get_document(uri: &str) -> Result<Document, glib::Error> {
        assert_initialized_main_thread!();
        unsafe {
            let mut error = std::ptr::null_mut();
            let ret = ffi::pps_document_factory_get_document(uri.to_glib_none().0, &mut error);
            if error.is_null() {
                Ok(from_glib_full(ret))
            } else {
                Err(from_glib_full(error))
            }
        }
    }

    //#[cfg(feature = "v42")]
    //#[cfg_attr(docsrs, doc(cfg(feature = "v42")))]
    //#[doc(alias = "pps_document_factory_get_document_for_fd")]
    //pub fn factory_get_document_for_fd(fd: i32, mime_type: &str, flags: /*Ignored*/DocumentLoadFlags, cancellable: /*Ignored*/Option<&gio::Cancellable>) -> Result<Document, glib::Error> {
    //    unsafe { TODO: call ffi:pps_document_factory_get_document_for_fd() }
    //}

    //#[doc(alias = "pps_document_factory_get_document_full")]
    //pub fn factory_get_document_full(uri: &str, flags: /*Ignored*/DocumentLoadFlags) -> Result<Document, glib::Error> {
    //    unsafe { TODO: call ffi:pps_document_factory_get_document_full() }
    //}

    #[doc(alias = "pps_document_misc_format_datetime")]
    pub fn misc_format_datetime(dt: &glib::DateTime) -> Option<glib::GString> {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_document_misc_format_datetime(dt.to_glib_none().0)) }
    }

    #[doc(alias = "pps_document_misc_get_pointer_position")]
    pub fn misc_get_pointer_position(widget: &impl IsA<gtk::Widget>) -> (i32, i32) {
        assert_initialized_main_thread!();
        unsafe {
            let mut x = std::mem::MaybeUninit::uninit();
            let mut y = std::mem::MaybeUninit::uninit();
            ffi::pps_document_misc_get_pointer_position(
                widget.as_ref().to_glib_none().0,
                x.as_mut_ptr(),
                y.as_mut_ptr(),
            );
            (x.assume_init(), y.assume_init())
        }
    }

    #[doc(alias = "pps_document_misc_get_widget_dpi")]
    pub fn misc_get_widget_dpi(widget: &impl IsA<gtk::Widget>) -> f64 {
        assert_initialized_main_thread!();
        unsafe { ffi::pps_document_misc_get_widget_dpi(widget.as_ref().to_glib_none().0) }
    }

    //#[doc(alias = "pps_document_misc_pixbuf_from_surface")]
    //pub fn misc_pixbuf_from_surface(surface: /*Ignored*/&mut cairo::Surface) -> Option<gdk_pixbuf::Pixbuf> {
    //    unsafe { TODO: call ffi:pps_document_misc_pixbuf_from_surface() }
    //}

    //#[doc(alias = "pps_document_misc_surface_from_pixbuf")]
    //pub fn misc_surface_from_pixbuf(pixbuf: &gdk_pixbuf::Pixbuf) -> /*Ignored*/Option<cairo::Surface> {
    //    unsafe { TODO: call ffi:pps_document_misc_surface_from_pixbuf() }
    //}

    //#[doc(alias = "pps_document_misc_surface_rotate_and_scale")]
    //pub fn misc_surface_rotate_and_scale(surface: /*Ignored*/&mut cairo::Surface, dest_width: i32, dest_height: i32, dest_rotation: i32) -> /*Ignored*/Option<cairo::Surface> {
    //    unsafe { TODO: call ffi:pps_document_misc_surface_rotate_and_scale() }
    //}

    //#[doc(alias = "pps_document_misc_texture_from_surface")]
    //pub fn misc_texture_from_surface(surface: /*Ignored*/&mut cairo::Surface) -> /*Ignored*/Option<gdk::Texture> {
    //    unsafe { TODO: call ffi:pps_document_misc_texture_from_surface() }
    //}
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::Document>> Sealed for T {}
}

pub trait DocumentExt: IsA<Document> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_check_dimensions")]
    fn check_dimensions(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_check_dimensions(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_doc_mutex_lock")]
    fn doc_mutex_lock(&self) {
        unsafe {
            ffi::pps_document_doc_mutex_lock(self.as_ref().to_glib_none().0);
        }
    }

    #[doc(alias = "pps_document_doc_mutex_trylock")]
    fn doc_mutex_trylock(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_doc_mutex_trylock(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_doc_mutex_unlock")]
    fn doc_mutex_unlock(&self) {
        unsafe {
            ffi::pps_document_doc_mutex_unlock(self.as_ref().to_glib_none().0);
        }
    }

    #[doc(alias = "pps_document_find_page_by_label")]
    fn find_page_by_label(&self, page_label: &str) -> Option<i32> {
        unsafe {
            let mut page_index = std::mem::MaybeUninit::uninit();
            let ret = from_glib(ffi::pps_document_find_page_by_label(
                self.as_ref().to_glib_none().0,
                page_label.to_glib_none().0,
                page_index.as_mut_ptr(),
            ));
            if ret {
                Some(page_index.assume_init())
            } else {
                None
            }
        }
    }

    //#[doc(alias = "pps_document_get_backend_info")]
    //#[doc(alias = "get_backend_info")]
    //fn is_backend_info(&self, info: /*Ignored*/&mut DocumentBackendInfo) -> bool {
    //    unsafe { TODO: call ffi:pps_document_get_backend_info() }
    //}

    #[doc(alias = "pps_document_get_info")]
    #[doc(alias = "get_info")]
    fn info(&self) -> Option<DocumentInfo> {
        unsafe { from_glib_full(ffi::pps_document_get_info(self.as_ref().to_glib_none().0)) }
    }

    #[doc(alias = "pps_document_get_max_label_len")]
    #[doc(alias = "get_max_label_len")]
    fn max_label_len(&self) -> i32 {
        unsafe { ffi::pps_document_get_max_label_len(self.as_ref().to_glib_none().0) }
    }

    #[doc(alias = "pps_document_get_max_page_size")]
    #[doc(alias = "get_max_page_size")]
    fn max_page_size(&self) -> (f64, f64) {
        unsafe {
            let mut width = std::mem::MaybeUninit::uninit();
            let mut height = std::mem::MaybeUninit::uninit();
            ffi::pps_document_get_max_page_size(
                self.as_ref().to_glib_none().0,
                width.as_mut_ptr(),
                height.as_mut_ptr(),
            );
            (width.assume_init(), height.assume_init())
        }
    }

    #[doc(alias = "pps_document_get_min_page_size")]
    #[doc(alias = "get_min_page_size")]
    fn min_page_size(&self) -> (f64, f64) {
        unsafe {
            let mut width = std::mem::MaybeUninit::uninit();
            let mut height = std::mem::MaybeUninit::uninit();
            ffi::pps_document_get_min_page_size(
                self.as_ref().to_glib_none().0,
                width.as_mut_ptr(),
                height.as_mut_ptr(),
            );
            (width.assume_init(), height.assume_init())
        }
    }

    #[doc(alias = "pps_document_get_modified")]
    #[doc(alias = "get_modified")]
    #[doc(alias = "modified")]
    fn is_modified(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_get_modified(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_get_n_pages")]
    #[doc(alias = "get_n_pages")]
    fn n_pages(&self) -> i32 {
        unsafe { ffi::pps_document_get_n_pages(self.as_ref().to_glib_none().0) }
    }

    #[doc(alias = "pps_document_get_page")]
    #[doc(alias = "get_page")]
    fn page(&self, index: i32) -> Option<Page> {
        unsafe {
            from_glib_full(ffi::pps_document_get_page(
                self.as_ref().to_glib_none().0,
                index,
            ))
        }
    }

    #[doc(alias = "pps_document_get_page_label")]
    #[doc(alias = "get_page_label")]
    fn page_label(&self, page_index: i32) -> Option<glib::GString> {
        unsafe {
            from_glib_full(ffi::pps_document_get_page_label(
                self.as_ref().to_glib_none().0,
                page_index,
            ))
        }
    }

    #[doc(alias = "pps_document_get_page_size")]
    #[doc(alias = "get_page_size")]
    fn page_size(&self, page_index: i32) -> (f64, f64) {
        unsafe {
            let mut width = std::mem::MaybeUninit::uninit();
            let mut height = std::mem::MaybeUninit::uninit();
            ffi::pps_document_get_page_size(
                self.as_ref().to_glib_none().0,
                page_index,
                width.as_mut_ptr(),
                height.as_mut_ptr(),
            );
            (width.assume_init(), height.assume_init())
        }
    }

    #[doc(alias = "pps_document_get_size")]
    #[doc(alias = "get_size")]
    fn size(&self) -> u64 {
        unsafe { ffi::pps_document_get_size(self.as_ref().to_glib_none().0) }
    }

    //#[doc(alias = "pps_document_get_thumbnail")]
    //#[doc(alias = "get_thumbnail")]
    //fn thumbnail(&self, rc: /*Ignored*/&RenderContext) -> Option<gdk_pixbuf::Pixbuf> {
    //    unsafe { TODO: call ffi:pps_document_get_thumbnail() }
    //}

    //#[doc(alias = "pps_document_get_thumbnail_surface")]
    //#[doc(alias = "get_thumbnail_surface")]
    //fn thumbnail_surface(&self, rc: /*Ignored*/&RenderContext) -> /*Ignored*/Option<cairo::Surface> {
    //    unsafe { TODO: call ffi:pps_document_get_thumbnail_surface() }
    //}

    #[doc(alias = "pps_document_get_title")]
    #[doc(alias = "get_title")]
    fn title(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_document_get_title(self.as_ref().to_glib_none().0)) }
    }

    #[doc(alias = "pps_document_get_uri")]
    #[doc(alias = "get_uri")]
    fn uri(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_document_get_uri(self.as_ref().to_glib_none().0)) }
    }

    #[doc(alias = "pps_document_has_text_page_labels")]
    fn has_text_page_labels(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_has_text_page_labels(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_is_page_size_uniform")]
    fn is_page_size_uniform(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_is_page_size_uniform(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_load")]
    fn load(&self, uri: &str) -> Result<(), glib::Error> {
        unsafe {
            let mut error = std::ptr::null_mut();
            let is_ok = ffi::pps_document_load(
                self.as_ref().to_glib_none().0,
                uri.to_glib_none().0,
                &mut error,
            );
            debug_assert_eq!(is_ok == glib::ffi::GFALSE, !error.is_null());
            if error.is_null() {
                Ok(())
            } else {
                Err(from_glib_full(error))
            }
        }
    }

    //#[cfg(feature = "v42")]
    //#[cfg_attr(docsrs, doc(cfg(feature = "v42")))]
    //#[doc(alias = "pps_document_load_fd")]
    //fn load_fd(&self, fd: i32, flags: /*Ignored*/DocumentLoadFlags, cancellable: /*Ignored*/Option<&gio::Cancellable>) -> Result<(), glib::Error> {
    //    unsafe { TODO: call ffi:pps_document_load_fd() }
    //}

    //#[doc(alias = "pps_document_load_full")]
    //fn load_full(&self, uri: &str, flags: /*Ignored*/DocumentLoadFlags) -> Result<(), glib::Error> {
    //    unsafe { TODO: call ffi:pps_document_load_full() }
    //}

    //#[doc(alias = "pps_document_render")]
    //fn render(&self, rc: /*Ignored*/&RenderContext) -> /*Ignored*/Option<cairo::Surface> {
    //    unsafe { TODO: call ffi:pps_document_render() }
    //}

    #[doc(alias = "pps_document_save")]
    fn save(&self, uri: &str) -> Result<(), glib::Error> {
        unsafe {
            let mut error = std::ptr::null_mut();
            let is_ok = ffi::pps_document_save(
                self.as_ref().to_glib_none().0,
                uri.to_glib_none().0,
                &mut error,
            );
            debug_assert_eq!(is_ok == glib::ffi::GFALSE, !error.is_null());
            if error.is_null() {
                Ok(())
            } else {
                Err(from_glib_full(error))
            }
        }
    }

    #[doc(alias = "pps_document_set_modified")]
    #[doc(alias = "modified")]
    fn set_modified(&self, modified: bool) {
        unsafe {
            ffi::pps_document_set_modified(self.as_ref().to_glib_none().0, modified.into_glib());
        }
    }

    #[doc(alias = "modified")]
    fn connect_modified_notify<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn notify_modified_trampoline<P: IsA<Document>, F: Fn(&P) + 'static>(
            this: *mut ffi::PpsDocument,
            _param_spec: glib::ffi::gpointer,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(Document::from_glib_borrow(this).unsafe_cast_ref())
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"notify::modified\0".as_ptr() as *const _,
                Some(std::mem::transmute::<*const (), unsafe extern "C" fn()>(
                    notify_modified_trampoline::<Self, F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }
}

impl<O: IsA<Document>> DocumentExt for O {}
