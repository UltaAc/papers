// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{Annotation, AnnotationMarkup, AnnotationTextMarkupType, Page};
use glib::{
    prelude::*,
    signal::{connect_raw, SignalHandlerId},
    translate::*,
};
use std::boxed::Box as Box_;

glib::wrapper! {
    #[doc(alias = "PpsAnnotationTextMarkup")]
    pub struct AnnotationTextMarkup(Object<ffi::PpsAnnotationTextMarkup, ffi::PpsAnnotationTextMarkupClass>) @extends Annotation, @implements AnnotationMarkup;

    match fn {
        type_ => || ffi::pps_annotation_text_markup_get_type(),
    }
}

impl AnnotationTextMarkup {
    #[doc(alias = "pps_annotation_text_markup_highlight_new")]
    pub fn highlight_new(page: &impl IsA<Page>) -> AnnotationTextMarkup {
        skip_assert_initialized!();
        unsafe {
            Annotation::from_glib_full(ffi::pps_annotation_text_markup_highlight_new(
                page.as_ref().to_glib_none().0,
            ))
            .unsafe_cast()
        }
    }

    #[doc(alias = "pps_annotation_text_markup_squiggly_new")]
    pub fn squiggly_new(page: &impl IsA<Page>) -> AnnotationTextMarkup {
        skip_assert_initialized!();
        unsafe {
            Annotation::from_glib_full(ffi::pps_annotation_text_markup_squiggly_new(
                page.as_ref().to_glib_none().0,
            ))
            .unsafe_cast()
        }
    }

    #[doc(alias = "pps_annotation_text_markup_strike_out_new")]
    pub fn strike_out_new(page: &impl IsA<Page>) -> AnnotationTextMarkup {
        skip_assert_initialized!();
        unsafe {
            Annotation::from_glib_full(ffi::pps_annotation_text_markup_strike_out_new(
                page.as_ref().to_glib_none().0,
            ))
            .unsafe_cast()
        }
    }

    #[doc(alias = "pps_annotation_text_markup_underline_new")]
    pub fn underline_new(page: &impl IsA<Page>) -> AnnotationTextMarkup {
        skip_assert_initialized!();
        unsafe {
            Annotation::from_glib_full(ffi::pps_annotation_text_markup_underline_new(
                page.as_ref().to_glib_none().0,
            ))
            .unsafe_cast()
        }
    }

    #[doc(alias = "pps_annotation_text_markup_get_markup_type")]
    #[doc(alias = "get_markup_type")]
    pub fn markup_type(&self) -> AnnotationTextMarkupType {
        unsafe {
            from_glib(ffi::pps_annotation_text_markup_get_markup_type(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_annotation_text_markup_set_markup_type")]
    pub fn set_markup_type(&self, markup_type: AnnotationTextMarkupType) -> bool {
        unsafe {
            from_glib(ffi::pps_annotation_text_markup_set_markup_type(
                self.to_glib_none().0,
                markup_type.into_glib(),
            ))
        }
    }

    #[doc(alias = "type")]
    pub fn type_(&self) -> AnnotationTextMarkupType {
        ObjectExt::property(self, "type")
    }

    #[doc(alias = "type")]
    pub fn set_type(&self, type_: AnnotationTextMarkupType) {
        ObjectExt::set_property(self, "type", type_)
    }

    #[doc(alias = "type")]
    pub fn connect_type_notify<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn notify_type_trampoline<F: Fn(&AnnotationTextMarkup) + 'static>(
            this: *mut ffi::PpsAnnotationTextMarkup,
            _param_spec: glib::ffi::gpointer,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(&from_glib_borrow(this))
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"notify::type\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    notify_type_trampoline::<F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }
}
