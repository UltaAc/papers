// Generated by gir (https://github.com/gtk-rs/gir @ d7c0763cacbc)
// from ../../ev-girs (@ 5a4372b0b8ba+)
// from ../../gir-files (@ 20031a537e40)
// DO NOT EDIT

#![cfg(unix)]

use papers_view_sys::*;
use std::env;
use std::error::Error;
use std::ffi::OsString;
use std::mem::{align_of, size_of};
use std::path::Path;
use std::process::{Command, Stdio};
use std::str;
use tempfile::Builder;

static PACKAGES: &[&str] = &["papers-view-4.0"];

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
        "PpsAttachmentContext",
        Layout {
            size: size_of::<PpsAttachmentContext>(),
            alignment: align_of::<PpsAttachmentContext>(),
        },
    ),
    (
        "PpsAttachmentContextClass",
        Layout {
            size: size_of::<PpsAttachmentContextClass>(),
            alignment: align_of::<PpsAttachmentContextClass>(),
        },
    ),
    (
        "PpsAttachmentContextError",
        Layout {
            size: size_of::<PpsAttachmentContextError>(),
            alignment: align_of::<PpsAttachmentContextError>(),
        },
    ),
    (
        "PpsBookmark",
        Layout {
            size: size_of::<PpsBookmark>(),
            alignment: align_of::<PpsBookmark>(),
        },
    ),
    (
        "PpsDocumentModelClass",
        Layout {
            size: size_of::<PpsDocumentModelClass>(),
            alignment: align_of::<PpsDocumentModelClass>(),
        },
    ),
    (
        "PpsHistory",
        Layout {
            size: size_of::<PpsHistory>(),
            alignment: align_of::<PpsHistory>(),
        },
    ),
    (
        "PpsHistoryClass",
        Layout {
            size: size_of::<PpsHistoryClass>(),
            alignment: align_of::<PpsHistoryClass>(),
        },
    ),
    (
        "PpsJob",
        Layout {
            size: size_of::<PpsJob>(),
            alignment: align_of::<PpsJob>(),
        },
    ),
    (
        "PpsJobAnnots",
        Layout {
            size: size_of::<PpsJobAnnots>(),
            alignment: align_of::<PpsJobAnnots>(),
        },
    ),
    (
        "PpsJobAnnotsClass",
        Layout {
            size: size_of::<PpsJobAnnotsClass>(),
            alignment: align_of::<PpsJobAnnotsClass>(),
        },
    ),
    (
        "PpsJobAttachments",
        Layout {
            size: size_of::<PpsJobAttachments>(),
            alignment: align_of::<PpsJobAttachments>(),
        },
    ),
    (
        "PpsJobAttachmentsClass",
        Layout {
            size: size_of::<PpsJobAttachmentsClass>(),
            alignment: align_of::<PpsJobAttachmentsClass>(),
        },
    ),
    (
        "PpsJobClass",
        Layout {
            size: size_of::<PpsJobClass>(),
            alignment: align_of::<PpsJobClass>(),
        },
    ),
    (
        "PpsJobExport",
        Layout {
            size: size_of::<PpsJobExport>(),
            alignment: align_of::<PpsJobExport>(),
        },
    ),
    (
        "PpsJobExportClass",
        Layout {
            size: size_of::<PpsJobExportClass>(),
            alignment: align_of::<PpsJobExportClass>(),
        },
    ),
    (
        "PpsJobFind",
        Layout {
            size: size_of::<PpsJobFind>(),
            alignment: align_of::<PpsJobFind>(),
        },
    ),
    (
        "PpsJobFindClass",
        Layout {
            size: size_of::<PpsJobFindClass>(),
            alignment: align_of::<PpsJobFindClass>(),
        },
    ),
    (
        "PpsJobFonts",
        Layout {
            size: size_of::<PpsJobFonts>(),
            alignment: align_of::<PpsJobFonts>(),
        },
    ),
    (
        "PpsJobFontsClass",
        Layout {
            size: size_of::<PpsJobFontsClass>(),
            alignment: align_of::<PpsJobFontsClass>(),
        },
    ),
    (
        "PpsJobLayers",
        Layout {
            size: size_of::<PpsJobLayers>(),
            alignment: align_of::<PpsJobLayers>(),
        },
    ),
    (
        "PpsJobLayersClass",
        Layout {
            size: size_of::<PpsJobLayersClass>(),
            alignment: align_of::<PpsJobLayersClass>(),
        },
    ),
    (
        "PpsJobLinks",
        Layout {
            size: size_of::<PpsJobLinks>(),
            alignment: align_of::<PpsJobLinks>(),
        },
    ),
    (
        "PpsJobLinksClass",
        Layout {
            size: size_of::<PpsJobLinksClass>(),
            alignment: align_of::<PpsJobLinksClass>(),
        },
    ),
    (
        "PpsJobLoad",
        Layout {
            size: size_of::<PpsJobLoad>(),
            alignment: align_of::<PpsJobLoad>(),
        },
    ),
    (
        "PpsJobLoadClass",
        Layout {
            size: size_of::<PpsJobLoadClass>(),
            alignment: align_of::<PpsJobLoadClass>(),
        },
    ),
    (
        "PpsJobPageData",
        Layout {
            size: size_of::<PpsJobPageData>(),
            alignment: align_of::<PpsJobPageData>(),
        },
    ),
    (
        "PpsJobPageDataClass",
        Layout {
            size: size_of::<PpsJobPageDataClass>(),
            alignment: align_of::<PpsJobPageDataClass>(),
        },
    ),
    (
        "PpsJobPageDataFlags",
        Layout {
            size: size_of::<PpsJobPageDataFlags>(),
            alignment: align_of::<PpsJobPageDataFlags>(),
        },
    ),
    (
        "PpsJobPrint",
        Layout {
            size: size_of::<PpsJobPrint>(),
            alignment: align_of::<PpsJobPrint>(),
        },
    ),
    (
        "PpsJobPrintClass",
        Layout {
            size: size_of::<PpsJobPrintClass>(),
            alignment: align_of::<PpsJobPrintClass>(),
        },
    ),
    (
        "PpsJobPriority",
        Layout {
            size: size_of::<PpsJobPriority>(),
            alignment: align_of::<PpsJobPriority>(),
        },
    ),
    (
        "PpsJobRenderTexture",
        Layout {
            size: size_of::<PpsJobRenderTexture>(),
            alignment: align_of::<PpsJobRenderTexture>(),
        },
    ),
    (
        "PpsJobRenderTextureClass",
        Layout {
            size: size_of::<PpsJobRenderTextureClass>(),
            alignment: align_of::<PpsJobRenderTextureClass>(),
        },
    ),
    (
        "PpsJobSave",
        Layout {
            size: size_of::<PpsJobSave>(),
            alignment: align_of::<PpsJobSave>(),
        },
    ),
    (
        "PpsJobSaveClass",
        Layout {
            size: size_of::<PpsJobSaveClass>(),
            alignment: align_of::<PpsJobSaveClass>(),
        },
    ),
    (
        "PpsJobThumbnailTexture",
        Layout {
            size: size_of::<PpsJobThumbnailTexture>(),
            alignment: align_of::<PpsJobThumbnailTexture>(),
        },
    ),
    (
        "PpsJobThumbnailTextureClass",
        Layout {
            size: size_of::<PpsJobThumbnailTextureClass>(),
            alignment: align_of::<PpsJobThumbnailTextureClass>(),
        },
    ),
    (
        "PpsMetadataClass",
        Layout {
            size: size_of::<PpsMetadataClass>(),
            alignment: align_of::<PpsMetadataClass>(),
        },
    ),
    (
        "PpsPageLayout",
        Layout {
            size: size_of::<PpsPageLayout>(),
            alignment: align_of::<PpsPageLayout>(),
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
        "PpsSizingMode",
        Layout {
            size: size_of::<PpsSizingMode>(),
            alignment: align_of::<PpsSizingMode>(),
        },
    ),
    (
        "PpsView",
        Layout {
            size: size_of::<PpsView>(),
            alignment: align_of::<PpsView>(),
        },
    ),
    (
        "PpsViewPresentation",
        Layout {
            size: size_of::<PpsViewPresentation>(),
            alignment: align_of::<PpsViewPresentation>(),
        },
    ),
];

const RUST_CONSTANTS: &[(&str, &str)] = &[
    ("(gint) PPS_ATTACHMENT_CONTEXT_ERROR_EMPTY_INPUT", "1"),
    ("(gint) PPS_ATTACHMENT_CONTEXT_ERROR_NOT_IMPLEMENTED", "0"),
    ("(gint) PPS_JOB_N_PRIORITIES", "4"),
    ("(gint) PPS_JOB_PRIORITY_HIGH", "1"),
    ("(gint) PPS_JOB_PRIORITY_LOW", "2"),
    ("(gint) PPS_JOB_PRIORITY_NONE", "3"),
    ("(gint) PPS_JOB_PRIORITY_URGENT", "0"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_ALL", "1023"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_ANNOTS", "256"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_FORMS", "128"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_IMAGES", "64"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_LINKS", "1"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_MEDIA", "512"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_NONE", "0"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_TEXT", "2"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_TEXT_ATTRS", "16"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_TEXT_LAYOUT", "8"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_TEXT_LOG_ATTRS", "32"),
    ("(guint) PPS_PAGE_DATA_INCLUDE_TEXT_MAPPING", "4"),
    ("(gint) PPS_PAGE_LAYOUT_AUTOMATIC", "2"),
    ("(gint) PPS_PAGE_LAYOUT_DUAL", "1"),
    ("(gint) PPS_PAGE_LAYOUT_SINGLE", "0"),
    ("(gint) PPS_SIZING_AUTOMATIC", "3"),
    ("(gint) PPS_SIZING_FIT_PAGE", "0"),
    ("(gint) PPS_SIZING_FIT_WIDTH", "1"),
    ("(gint) PPS_SIZING_FREE", "2"),
];
