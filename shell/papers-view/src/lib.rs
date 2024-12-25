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
mod attachment_context;
mod auto;
pub mod prelude;
