// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::{ffi, Job};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsJobSave")]
    pub struct JobSave(Object<ffi::PpsJobSave, ffi::PpsJobSaveClass>) @extends Job;

    match fn {
        type_ => || ffi::pps_job_save_get_type(),
    }
}

impl JobSave {
    pub const NONE: Option<&'static JobSave> = None;

    #[doc(alias = "pps_job_save_new")]
    pub fn new(
        document: &impl IsA<papers_document::Document>,
        uri: &str,
        document_uri: &str,
    ) -> JobSave {
        assert_initialized_main_thread!();
        unsafe {
            Job::from_glib_full(ffi::pps_job_save_new(
                document.as_ref().to_glib_none().0,
                uri.to_glib_none().0,
                document_uri.to_glib_none().0,
            ))
            .unsafe_cast()
        }
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::JobSave>> Sealed for T {}
}

pub trait JobSaveExt: IsA<JobSave> + sealed::Sealed + 'static {
    #[doc(alias = "pps_job_save_get_uri")]
    #[doc(alias = "get_uri")]
    fn uri(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_job_save_get_uri(self.as_ref().to_glib_none().0)) }
    }
}

impl<O: IsA<JobSave>> JobSaveExt for O {}
