// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::{ffi, Job};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsJobLinks")]
    pub struct JobLinks(Object<ffi::PpsJobLinks, ffi::PpsJobLinksClass>) @extends Job;

    match fn {
        type_ => || ffi::pps_job_links_get_type(),
    }
}

impl JobLinks {
    pub const NONE: Option<&'static JobLinks> = None;

    #[doc(alias = "pps_job_links_new")]
    pub fn new(document: &impl IsA<papers_document::Document>) -> JobLinks {
        assert_initialized_main_thread!();
        unsafe {
            Job::from_glib_full(ffi::pps_job_links_new(document.as_ref().to_glib_none().0))
                .unsafe_cast()
        }
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::JobLinks>> Sealed for T {}
}

pub trait JobLinksExt: IsA<JobLinks> + sealed::Sealed + 'static {
    #[doc(alias = "pps_job_links_get_model")]
    #[doc(alias = "get_model")]
    fn model(&self) -> Option<gio::ListModel> {
        unsafe { from_glib_none(ffi::pps_job_links_get_model(self.as_ref().to_glib_none().0)) }
    }
}

impl<O: IsA<JobLinks>> JobLinksExt for O {}
