// Generated by gir (https://github.com/gtk-rs/gir @ 20a5b17f6da9)
// from ../../ev-girs (@ 18d14d3d4bfb+)
// from ../../gir-files (@ 20031a537e40)
// DO NOT EDIT

#[cfg(not(docsrs))]
use std::process;

#[cfg(docsrs)]
fn main() {} // prevent linking libraries to avoid documentation failure

#[cfg(not(docsrs))]
fn main() {
    if let Err(s) = system_deps::Config::new().probe() {
        println!("cargo:warning={s}");
        process::exit(1);
    }
}
