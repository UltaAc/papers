// Generated by gir (https://github.com/gtk-rs/gir @ d7c0763cacbc)
// from ../../ev-girs (@ 9ed647f47a13+)
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
