// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::JobPriority;
use glib::{
    prelude::*,
    signal::{connect_raw, SignalHandlerId},
    translate::*,
};
use std::boxed::Box as Box_;

glib::wrapper! {
    #[doc(alias = "PpsJob")]
    pub struct Job(Object<ffi::PpsJob, ffi::PpsJobClass>);

    match fn {
        type_ => || ffi::pps_job_get_type(),
    }
}

impl Job {
    pub const NONE: Option<&'static Job> = None;

    #[doc(alias = "pps_job_scheduler_get_running_thread_job")]
    pub fn scheduler_get_running_thread_job() -> Option<Job> {
        assert_initialized_main_thread!();
        unsafe { from_glib_none(ffi::pps_job_scheduler_get_running_thread_job()) }
    }

    #[doc(alias = "pps_job_scheduler_wait")]
    pub fn scheduler_wait() {
        assert_initialized_main_thread!();
        unsafe {
            ffi::pps_job_scheduler_wait();
        }
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::Job>> Sealed for T {}
}

pub trait JobExt: IsA<Job> + sealed::Sealed + 'static {
    #[doc(alias = "pps_job_cancel")]
    fn cancel(&self) {
        unsafe {
            ffi::pps_job_cancel(self.as_ref().to_glib_none().0);
        }
    }

    //#[doc(alias = "pps_job_failed")]
    //fn failed(&self, domain: /*Ignored*/glib::Quark, code: i32, format: &str, : /*Unknown conversion*//*Unimplemented*/Basic: VarArgs) {
    //    unsafe { TODO: call ffi:pps_job_failed() }
    //}

    #[doc(alias = "pps_job_failed_from_error")]
    fn failed_from_error(&self, error: &mut glib::Error) {
        unsafe {
            ffi::pps_job_failed_from_error(
                self.as_ref().to_glib_none().0,
                error.to_glib_none_mut().0,
            );
        }
    }

    #[doc(alias = "pps_job_get_document")]
    #[doc(alias = "get_document")]
    fn document(&self) -> Option<papers_document::Document> {
        unsafe { from_glib_none(ffi::pps_job_get_document(self.as_ref().to_glib_none().0)) }
    }

    //#[doc(alias = "pps_job_get_run_mode")]
    //#[doc(alias = "get_run_mode")]
    //fn run_mode(&self) -> /*Ignored*/JobRunMode {
    //    unsafe { TODO: call ffi:pps_job_get_run_mode() }
    //}

    #[doc(alias = "pps_job_is_failed")]
    fn is_failed(&self) -> Result<(), glib::Error> {
        unsafe {
            let mut error = std::ptr::null_mut();
            let is_ok = ffi::pps_job_is_failed(self.as_ref().to_glib_none().0, &mut error);
            debug_assert_eq!(is_ok == glib::ffi::GFALSE, !error.is_null());
            if error.is_null() {
                Ok(())
            } else {
                Err(from_glib_full(error))
            }
        }
    }

    #[doc(alias = "pps_job_is_finished")]
    fn is_finished(&self) -> bool {
        unsafe { from_glib(ffi::pps_job_is_finished(self.as_ref().to_glib_none().0)) }
    }

    #[doc(alias = "pps_job_run")]
    fn run(&self) -> bool {
        unsafe { from_glib(ffi::pps_job_run(self.as_ref().to_glib_none().0)) }
    }

    #[doc(alias = "pps_job_scheduler_push_job")]
    fn scheduler_push_job(&self, priority: JobPriority) {
        unsafe {
            ffi::pps_job_scheduler_push_job(self.as_ref().to_glib_none().0, priority.into_glib());
        }
    }

    #[doc(alias = "pps_job_scheduler_update_job")]
    fn scheduler_update_job(&self, priority: JobPriority) {
        unsafe {
            ffi::pps_job_scheduler_update_job(self.as_ref().to_glib_none().0, priority.into_glib());
        }
    }

    //#[doc(alias = "pps_job_set_run_mode")]
    //fn set_run_mode(&self, run_mode: /*Ignored*/JobRunMode) {
    //    unsafe { TODO: call ffi:pps_job_set_run_mode() }
    //}

    #[doc(alias = "pps_job_succeeded")]
    fn succeeded(&self) {
        unsafe {
            ffi::pps_job_succeeded(self.as_ref().to_glib_none().0);
        }
    }

    #[doc(alias = "cancelled")]
    fn connect_cancelled<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn cancelled_trampoline<P: IsA<Job>, F: Fn(&P) + 'static>(
            this: *mut ffi::PpsJob,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(Job::from_glib_borrow(this).unsafe_cast_ref())
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"cancelled\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    cancelled_trampoline::<Self, F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }

    #[doc(alias = "finished")]
    fn connect_finished<F: Fn(&Self) + 'static>(&self, f: F) -> SignalHandlerId {
        unsafe extern "C" fn finished_trampoline<P: IsA<Job>, F: Fn(&P) + 'static>(
            this: *mut ffi::PpsJob,
            f: glib::ffi::gpointer,
        ) {
            let f: &F = &*(f as *const F);
            f(Job::from_glib_borrow(this).unsafe_cast_ref())
        }
        unsafe {
            let f: Box_<F> = Box_::new(f);
            connect_raw(
                self.as_ptr() as *mut _,
                b"finished\0".as_ptr() as *const _,
                Some(std::mem::transmute::<_, unsafe extern "C" fn()>(
                    finished_trampoline::<Self, F> as *const (),
                )),
                Box_::into_raw(f),
            )
        }
    }
}

impl<O: IsA<Job>> JobExt for O {}
