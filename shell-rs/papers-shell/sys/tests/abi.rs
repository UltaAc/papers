// Generated by gir (https://github.com/gtk-rs/gir @ eb5be4f1bafe)
// from ../../ev-girs (@ 336c660da08f+)
// from ../../gir-files (@ 20031a537e40)
// DO NOT EDIT

#![cfg(unix)]

use papers_shell_sys::*;
use std::env;
use std::error::Error;
use std::ffi::OsString;
use std::mem::{align_of, size_of};
use std::path::Path;
use std::process::{Command, Stdio};
use std::str;
use tempfile::Builder;

static PACKAGES: &[&str] = &["papers-shell-4.0"];

#[derive(Clone, Debug)]
struct Compiler {
    pub args: Vec<String>,
}

impl Compiler {
    pub fn new() -> Result<Self, Box<dyn Error>> {
        let mut args = get_var("CC", "cc")?;
        args.push("-Wno-deprecated-declarations".to_owned());
        // For _Generic
        args.push("-std=c11".to_owned());
        // For %z support in printf when using MinGW.
        args.push("-D__USE_MINGW_ANSI_STDIO".to_owned());
        args.extend(get_var("CFLAGS", "")?);
        args.extend(get_var("CPPFLAGS", "")?);
        args.extend(pkg_config_cflags(PACKAGES)?);
        Ok(Self { args })
    }

    pub fn compile(&self, src: &Path, out: &Path) -> Result<(), Box<dyn Error>> {
        let mut cmd = self.to_command();
        cmd.arg(src);
        cmd.arg("-o");
        cmd.arg(out);
        let status = cmd.spawn()?.wait()?;
        if !status.success() {
            return Err(format!("compilation command {cmd:?} failed, {status}").into());
        }
        Ok(())
    }

    fn to_command(&self) -> Command {
        let mut cmd = Command::new(&self.args[0]);
        cmd.args(&self.args[1..]);
        cmd
    }
}

fn get_var(name: &str, default: &str) -> Result<Vec<String>, Box<dyn Error>> {
    match env::var(name) {
        Ok(value) => Ok(shell_words::split(&value)?),
        Err(env::VarError::NotPresent) => Ok(shell_words::split(default)?),
        Err(err) => Err(format!("{name} {err}").into()),
    }
}

fn pkg_config_cflags(packages: &[&str]) -> Result<Vec<String>, Box<dyn Error>> {
    if packages.is_empty() {
        return Ok(Vec::new());
    }
    let pkg_config = env::var_os("PKG_CONFIG").unwrap_or_else(|| OsString::from("pkg-config"));
    let mut cmd = Command::new(pkg_config);
    cmd.arg("--cflags");
    cmd.args(packages);
    cmd.stderr(Stdio::inherit());
    let out = cmd.output()?;
    if !out.status.success() {
        let (status, stdout) = (out.status, String::from_utf8_lossy(&out.stdout));
        return Err(format!("command {cmd:?} failed, {status:?}\nstdout: {stdout}").into());
    }
    let stdout = str::from_utf8(&out.stdout)?;
    Ok(shell_words::split(stdout.trim())?)
}

#[derive(Copy, Clone, Debug, Eq, PartialEq)]
struct Layout {
    size: usize,
    alignment: usize,
}

#[derive(Copy, Clone, Debug, Default, Eq, PartialEq)]
struct Results {
    /// Number of successfully completed tests.
    passed: usize,
    /// Total number of failed tests (including those that failed to compile).
    failed: usize,
}

impl Results {
    fn record_passed(&mut self) {
        self.passed += 1;
    }
    fn record_failed(&mut self) {
        self.failed += 1;
    }
    fn summary(&self) -> String {
        format!("{} passed; {} failed", self.passed, self.failed)
    }
    fn expect_total_success(&self) {
        if self.failed == 0 {
            println!("OK: {}", self.summary());
        } else {
            panic!("FAILED: {}", self.summary());
        };
    }
}

#[test]
fn cross_validate_constants_with_c() {
    let mut c_constants: Vec<(String, String)> = Vec::new();

    for l in get_c_output("constant").unwrap().lines() {
        let (name, value) = l.split_once(';').expect("Missing ';' separator");
        c_constants.push((name.to_owned(), value.to_owned()));
    }

    let mut results = Results::default();

    for ((rust_name, rust_value), (c_name, c_value)) in
        RUST_CONSTANTS.iter().zip(c_constants.iter())
    {
        if rust_name != c_name {
            results.record_failed();
            eprintln!("Name mismatch:\nRust: {rust_name:?}\nC:    {c_name:?}");
            continue;
        }

        if rust_value != c_value {
            results.record_failed();
            eprintln!(
                "Constant value mismatch for {rust_name}\nRust: {rust_value:?}\nC:    {c_value:?}",
            );
            continue;
        }

        results.record_passed();
    }

    results.expect_total_success();
}

#[test]
fn cross_validate_layout_with_c() {
    let mut c_layouts = Vec::new();

    for l in get_c_output("layout").unwrap().lines() {
        let (name, value) = l.split_once(';').expect("Missing first ';' separator");
        let (size, alignment) = value.split_once(';').expect("Missing second ';' separator");
        let size = size.parse().expect("Failed to parse size");
        let alignment = alignment.parse().expect("Failed to parse alignment");
        c_layouts.push((name.to_owned(), Layout { size, alignment }));
    }

    let mut results = Results::default();

    for ((rust_name, rust_layout), (c_name, c_layout)) in RUST_LAYOUTS.iter().zip(c_layouts.iter())
    {
        if rust_name != c_name {
            results.record_failed();
            eprintln!("Name mismatch:\nRust: {rust_name:?}\nC:    {c_name:?}");
            continue;
        }

        if rust_layout != c_layout {
            results.record_failed();
            eprintln!("Layout mismatch for {rust_name}\nRust: {rust_layout:?}\nC:    {c_layout:?}",);
            continue;
        }

        results.record_passed();
    }

    results.expect_total_success();
}

fn get_c_output(name: &str) -> Result<String, Box<dyn Error>> {
    let tmpdir = Builder::new().prefix("abi").tempdir()?;
    let exe = tmpdir.path().join(name);
    let c_file = Path::new("tests").join(name).with_extension("c");

    let cc = Compiler::new().expect("configured compiler");
    cc.compile(&c_file, &exe)?;

    let mut cmd = Command::new(exe);
    cmd.stderr(Stdio::inherit());
    let out = cmd.output()?;
    if !out.status.success() {
        let (status, stdout) = (out.status, String::from_utf8_lossy(&out.stdout));
        return Err(format!("command {cmd:?} failed, {status:?}\nstdout: {stdout}").into());
    }

    Ok(String::from_utf8(out.stdout)?)
}

const RUST_LAYOUTS: &[(&str, Layout)] = &[
    (
        "PpsFindSidebar",
        Layout {
            size: size_of::<PpsFindSidebar>(),
            alignment: align_of::<PpsFindSidebar>(),
        },
    ),
    (
        "PpsFindSidebarClass",
        Layout {
            size: size_of::<PpsFindSidebarClass>(),
            alignment: align_of::<PpsFindSidebarClass>(),
        },
    ),
    (
        "PpsMessageArea",
        Layout {
            size: size_of::<PpsMessageArea>(),
            alignment: align_of::<PpsMessageArea>(),
        },
    ),
    (
        "PpsMessageAreaClass",
        Layout {
            size: size_of::<PpsMessageAreaClass>(),
            alignment: align_of::<PpsMessageAreaClass>(),
        },
    ),
    (
        "PpsObjectIface",
        Layout {
            size: size_of::<PpsObjectIface>(),
            alignment: align_of::<PpsObjectIface>(),
        },
    ),
    (
        "PpsObjectManagerClient",
        Layout {
            size: size_of::<PpsObjectManagerClient>(),
            alignment: align_of::<PpsObjectManagerClient>(),
        },
    ),
    (
        "PpsObjectManagerClientClass",
        Layout {
            size: size_of::<PpsObjectManagerClientClass>(),
            alignment: align_of::<PpsObjectManagerClientClass>(),
        },
    ),
    (
        "PpsObjectProxy",
        Layout {
            size: size_of::<PpsObjectProxy>(),
            alignment: align_of::<PpsObjectProxy>(),
        },
    ),
    (
        "PpsObjectProxyClass",
        Layout {
            size: size_of::<PpsObjectProxyClass>(),
            alignment: align_of::<PpsObjectProxyClass>(),
        },
    ),
    (
        "PpsObjectSkeleton",
        Layout {
            size: size_of::<PpsObjectSkeleton>(),
            alignment: align_of::<PpsObjectSkeleton>(),
        },
    ),
    (
        "PpsObjectSkeletonClass",
        Layout {
            size: size_of::<PpsObjectSkeletonClass>(),
            alignment: align_of::<PpsObjectSkeletonClass>(),
        },
    ),
    (
        "PpsPapersApplicationIface",
        Layout {
            size: size_of::<PpsPapersApplicationIface>(),
            alignment: align_of::<PpsPapersApplicationIface>(),
        },
    ),
    (
        "PpsPapersApplicationProxy",
        Layout {
            size: size_of::<PpsPapersApplicationProxy>(),
            alignment: align_of::<PpsPapersApplicationProxy>(),
        },
    ),
    (
        "PpsPapersApplicationProxyClass",
        Layout {
            size: size_of::<PpsPapersApplicationProxyClass>(),
            alignment: align_of::<PpsPapersApplicationProxyClass>(),
        },
    ),
    (
        "PpsPapersApplicationSkeleton",
        Layout {
            size: size_of::<PpsPapersApplicationSkeleton>(),
            alignment: align_of::<PpsPapersApplicationSkeleton>(),
        },
    ),
    (
        "PpsPapersApplicationSkeletonClass",
        Layout {
            size: size_of::<PpsPapersApplicationSkeletonClass>(),
            alignment: align_of::<PpsPapersApplicationSkeletonClass>(),
        },
    ),
    (
        "PpsProgressMessageArea",
        Layout {
            size: size_of::<PpsProgressMessageArea>(),
            alignment: align_of::<PpsProgressMessageArea>(),
        },
    ),
    (
        "PpsProgressMessageAreaClass",
        Layout {
            size: size_of::<PpsProgressMessageAreaClass>(),
            alignment: align_of::<PpsProgressMessageAreaClass>(),
        },
    ),
    (
        "PpsSearchContext",
        Layout {
            size: size_of::<PpsSearchContext>(),
            alignment: align_of::<PpsSearchContext>(),
        },
    ),
    (
        "PpsSearchContextClass",
        Layout {
            size: size_of::<PpsSearchContextClass>(),
            alignment: align_of::<PpsSearchContextClass>(),
        },
    ),
    (
        "PpsSearchResult",
        Layout {
            size: size_of::<PpsSearchResult>(),
            alignment: align_of::<PpsSearchResult>(),
        },
    ),
    (
        "PpsSearchResultClass",
        Layout {
            size: size_of::<PpsSearchResultClass>(),
            alignment: align_of::<PpsSearchResultClass>(),
        },
    ),
    (
        "PpsSidebarAnnotations",
        Layout {
            size: size_of::<PpsSidebarAnnotations>(),
            alignment: align_of::<PpsSidebarAnnotations>(),
        },
    ),
    (
        "PpsSidebarAnnotationsClass",
        Layout {
            size: size_of::<PpsSidebarAnnotationsClass>(),
            alignment: align_of::<PpsSidebarAnnotationsClass>(),
        },
    ),
    (
        "PpsSidebarPage",
        Layout {
            size: size_of::<PpsSidebarPage>(),
            alignment: align_of::<PpsSidebarPage>(),
        },
    ),
    (
        "PpsSidebarPageClass",
        Layout {
            size: size_of::<PpsSidebarPageClass>(),
            alignment: align_of::<PpsSidebarPageClass>(),
        },
    ),
    (
        "PpsWindow",
        Layout {
            size: size_of::<PpsWindow>(),
            alignment: align_of::<PpsWindow>(),
        },
    ),
    (
        "PpsWindowClass",
        Layout {
            size: size_of::<PpsWindowClass>(),
            alignment: align_of::<PpsWindowClass>(),
        },
    ),
    (
        "PpsWindowRunMode",
        Layout {
            size: size_of::<PpsWindowRunMode>(),
            alignment: align_of::<PpsWindowRunMode>(),
        },
    ),
];

const RUST_CONSTANTS: &[(&str, &str)] = &[
    ("(gint) PPS_WINDOW_MODE_ERROR_VIEW", "4"),
    ("(gint) PPS_WINDOW_MODE_FULLSCREEN", "1"),
    ("(gint) PPS_WINDOW_MODE_LOADER_VIEW", "6"),
    ("(gint) PPS_WINDOW_MODE_NORMAL", "0"),
    ("(gint) PPS_WINDOW_MODE_PASSWORD_VIEW", "5"),
    ("(gint) PPS_WINDOW_MODE_PRESENTATION", "2"),
    ("(gint) PPS_WINDOW_MODE_START_VIEW", "3"),
];
