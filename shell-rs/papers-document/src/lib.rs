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
