// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::{ffi, Job};
use glib::{
    prelude::*,
    signal::{connect_raw, SignalHandlerId},
    translate::*,
};
use std::boxed::Box as Box_;

glib::wrapper! {
    #[doc(alias = "PpsJobFind")]
    pub struct JobFind(Object<ffi::PpsJobFind, ffi::PpsJobFindClass>) @extends Job;

    match fn {
        type_ => || ffi::pps_job_find_get_type(),
    }
}

impl JobFind {
    pub const NONE: Option<&'static JobFind> = None;

    #[doc(alias = "pps_job_find_new")]
    pub fn new(
        document: &impl IsA<papers_document::Document>,
        start_page: i32,
        n_pages: i32,
        text: &str,
        options: papers_document::FindOptions,
    ) -> JobFind {
        assert_initialized_main_thread!();
        unsafe {
            Job::from_glib_full(ffi::pps_job_find_new(
                document.as_ref().to_glib_none().0,
                start_page,
                n_pages,
                text.to_glib_none().0,
                options.into_glib(),
            ))
            .unsafe_cast()
        }
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::JobFind>> Sealed for T {}
}

pub trait JobFindExt: IsA<JobFind> + sealed::Sealed + 'static {
    #[doc(alias = "pps_job_find_get_n_main_results")]
    #[doc(alias = "get_n_main_results")]
    fn n_main_results(&self, page: i32) -> i32 {
        unsafe { ffi::pps_job_find_get_n_main_results(self.as_ref().to_glib_none().0, page) }
    }

    #[doc(alias = "pps_job_find_get_options")]
    #[doc(alias = "get_options")]
    fn options(&self) -> papers_document::FindOptions {
        unsafe {
            from_glib(ffi::pps_job_find_get_options(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    //#[doc(alias = "pps_job_find_get_results")]
    //#[doc(alias = "get_results")]
    //fn results(&self) -> /*Unimplemented*/Vec<Basic: Pointer> {
    //    unsafe { TODO: call ffi:pps_job_find_get_results() }
    //}

    #[doc(alias = "pps_job_find_has_results")]
    fn has_results(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_job_find_has_results(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "updated")]
    fn connect_updated<F: Fn(&Self, i32) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn updated_trampoline<P: IsA<JobFind>, F: Fn(&P, i32) + 'static>(
            this: *mut ffi::PpsJobFind,
            object: libc::c_int,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(JobFind::from_glib_borrow(this).unsafe_cast_ref(), object)
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"updated\0".as_ptr() as *const _,
                Some(std::mem::transmute::<*const (), unsafe extern "C" fn()>(
                    updated_trampoline::<Self, F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }
}

impl<O: IsA<JobFind>> JobFindExt for O {}
