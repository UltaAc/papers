#![cfg_attr(docsrs, feature(doc_cfg))]

/// No-op.
macro_rules! assert_initialized_main_thread {
    () => {};
}

pub use auto::functions::*;
pub use auto::*;
pub use ffi;
#[allow(unused_imports)]
mod auto;

pub mod prelude;
