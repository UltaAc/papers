// Generated by gir (https://github.com/gtk-rs/gir @ eb5be4f1bafe)
// from ../../ev-girs (@ 43ea65c6e8e2+)
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
