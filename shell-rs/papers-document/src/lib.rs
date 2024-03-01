#![cfg_attr(docsrs, feature(doc_cfg))]

/// No-op.
macro_rules! skip_assert_initialized {
    () => {};
}

/// No-op.
macro_rules! assert_initialized_main_thread {
    () => {};
}

pub use auto::*;
pub use ffi;

pub mod prelude;

#[allow(unused_imports)]
mod auto;

use std::ffi::CStr;

pub fn locale_dir() -> String {
    unsafe {
        CStr::from_ptr(ffi::pps_get_locale_dir())
            .to_string_lossy()
            .into_owned()
    }
}

pub fn init() -> bool {
    unsafe { ffi::pps_init() > 0 }
}

pub fn shutdown() {
    unsafe {
        ffi::pps_shutdown();
    }
}

mod document_info;

pub fn mkdtemp(s: &str) -> Result<std::path::PathBuf, glib::Error> {
    let mut error = std::ptr::null_mut();
    let c_str = std::ffi::CString::new(s).unwrap();

    let path = unsafe { crate::ffi::pps_mkdtemp(c_str.as_ptr(), &mut error) };

    debug_assert_eq!(path.is_null(), !error.is_null());

    if error.is_null() {
        unsafe { Ok(glib::translate::from_glib_full(path)) }
    } else {
        unsafe { Err(glib::translate::from_glib_full(error)) }
    }
}
