// Generated by gir (https://github.com/gtk-rs/gir @ eb5be4f1bafe)
// from ../../ev-girs (@ b1161fa5399d+)
// from ../../gir-files (@ 20031a537e40)
// DO NOT EDIT

#![cfg(unix)]

use papers_document_sys::*;
use std::mem::{align_of, size_of};
use std::env;
use std::error::Error;
use std::ffi::OsString;
use std::path::Path;
use std::process::{Command, Stdio};
use std::str;
use tempfile::Builder;

static PACKAGES: &[&str] = &["papers-document-4.0"];

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
    let pkg_config = env::var_os("PKG_CONFIG")
        .unwrap_or_else(|| OsString::from("pkg-config"));
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

    for ((rust_name, rust_layout), (c_name, c_layout)) in
        RUST_LAYOUTS.iter().zip(c_layouts.iter())
    {
        if rust_name != c_name {
            results.record_failed();
            eprintln!("Name mismatch:\nRust: {rust_name:?}\nC:    {c_name:?}");
            continue;
        }

        if rust_layout != c_layout {
            results.record_failed();
            eprintln!(
                "Layout mismatch for {rust_name}\nRust: {rust_layout:?}\nC:    {c_layout:?}",
            );
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
    ("PpsAnnotation", Layout {size: size_of::<PpsAnnotation>(), alignment: align_of::<PpsAnnotation>()}),
    ("PpsAnnotationAttachmentClass", Layout {size: size_of::<PpsAnnotationAttachmentClass>(), alignment: align_of::<PpsAnnotationAttachmentClass>()}),
    ("PpsAnnotationClass", Layout {size: size_of::<PpsAnnotationClass>(), alignment: align_of::<PpsAnnotationClass>()}),
    ("PpsAnnotationTextClass", Layout {size: size_of::<PpsAnnotationTextClass>(), alignment: align_of::<PpsAnnotationTextClass>()}),
    ("PpsAnnotationTextIcon", Layout {size: size_of::<PpsAnnotationTextIcon>(), alignment: align_of::<PpsAnnotationTextIcon>()}),
    ("PpsAnnotationTextMarkupClass", Layout {size: size_of::<PpsAnnotationTextMarkupClass>(), alignment: align_of::<PpsAnnotationTextMarkupClass>()}),
    ("PpsAnnotationTextMarkupType", Layout {size: size_of::<PpsAnnotationTextMarkupType>(), alignment: align_of::<PpsAnnotationTextMarkupType>()}),
    ("PpsAnnotationType", Layout {size: size_of::<PpsAnnotationType>(), alignment: align_of::<PpsAnnotationType>()}),
    ("PpsAnnotationsOverMarkup", Layout {size: size_of::<PpsAnnotationsOverMarkup>(), alignment: align_of::<PpsAnnotationsOverMarkup>()}),
    ("PpsAnnotationsSaveMask", Layout {size: size_of::<PpsAnnotationsSaveMask>(), alignment: align_of::<PpsAnnotationsSaveMask>()}),
    ("PpsAttachment", Layout {size: size_of::<PpsAttachment>(), alignment: align_of::<PpsAttachment>()}),
    ("PpsAttachmentClass", Layout {size: size_of::<PpsAttachmentClass>(), alignment: align_of::<PpsAttachmentClass>()}),
    ("PpsBackendPage", Layout {size: size_of::<PpsBackendPage>(), alignment: align_of::<PpsBackendPage>()}),
    ("PpsBackendPageDestroyFunc", Layout {size: size_of::<PpsBackendPageDestroyFunc>(), alignment: align_of::<PpsBackendPageDestroyFunc>()}),
    ("PpsCompressionType", Layout {size: size_of::<PpsCompressionType>(), alignment: align_of::<PpsCompressionType>()}),
    ("PpsDocument", Layout {size: size_of::<PpsDocument>(), alignment: align_of::<PpsDocument>()}),
    ("PpsDocumentAnnotationsInterface", Layout {size: size_of::<PpsDocumentAnnotationsInterface>(), alignment: align_of::<PpsDocumentAnnotationsInterface>()}),
    ("PpsDocumentAttachmentsInterface", Layout {size: size_of::<PpsDocumentAttachmentsInterface>(), alignment: align_of::<PpsDocumentAttachmentsInterface>()}),
    ("PpsDocumentBackendInfo", Layout {size: size_of::<PpsDocumentBackendInfo>(), alignment: align_of::<PpsDocumentBackendInfo>()}),
    ("PpsDocumentClass", Layout {size: size_of::<PpsDocumentClass>(), alignment: align_of::<PpsDocumentClass>()}),
    ("PpsDocumentContainsJS", Layout {size: size_of::<PpsDocumentContainsJS>(), alignment: align_of::<PpsDocumentContainsJS>()}),
    ("PpsDocumentError", Layout {size: size_of::<PpsDocumentError>(), alignment: align_of::<PpsDocumentError>()}),
    ("PpsDocumentFindInterface", Layout {size: size_of::<PpsDocumentFindInterface>(), alignment: align_of::<PpsDocumentFindInterface>()}),
    ("PpsDocumentFontsInterface", Layout {size: size_of::<PpsDocumentFontsInterface>(), alignment: align_of::<PpsDocumentFontsInterface>()}),
    ("PpsDocumentFormsInterface", Layout {size: size_of::<PpsDocumentFormsInterface>(), alignment: align_of::<PpsDocumentFormsInterface>()}),
    ("PpsDocumentImagesInterface", Layout {size: size_of::<PpsDocumentImagesInterface>(), alignment: align_of::<PpsDocumentImagesInterface>()}),
    ("PpsDocumentInfo", Layout {size: size_of::<PpsDocumentInfo>(), alignment: align_of::<PpsDocumentInfo>()}),
    ("PpsDocumentInfoFields", Layout {size: size_of::<PpsDocumentInfoFields>(), alignment: align_of::<PpsDocumentInfoFields>()}),
    ("PpsDocumentLayersInterface", Layout {size: size_of::<PpsDocumentLayersInterface>(), alignment: align_of::<PpsDocumentLayersInterface>()}),
    ("PpsDocumentLayout", Layout {size: size_of::<PpsDocumentLayout>(), alignment: align_of::<PpsDocumentLayout>()}),
    ("PpsDocumentLicense", Layout {size: size_of::<PpsDocumentLicense>(), alignment: align_of::<PpsDocumentLicense>()}),
    ("PpsDocumentLinksInterface", Layout {size: size_of::<PpsDocumentLinksInterface>(), alignment: align_of::<PpsDocumentLinksInterface>()}),
    ("PpsDocumentLoadFlags", Layout {size: size_of::<PpsDocumentLoadFlags>(), alignment: align_of::<PpsDocumentLoadFlags>()}),
    ("PpsDocumentMediaInterface", Layout {size: size_of::<PpsDocumentMediaInterface>(), alignment: align_of::<PpsDocumentMediaInterface>()}),
    ("PpsDocumentMode", Layout {size: size_of::<PpsDocumentMode>(), alignment: align_of::<PpsDocumentMode>()}),
    ("PpsDocumentPermissions", Layout {size: size_of::<PpsDocumentPermissions>(), alignment: align_of::<PpsDocumentPermissions>()}),
    ("PpsDocumentPrintInterface", Layout {size: size_of::<PpsDocumentPrintInterface>(), alignment: align_of::<PpsDocumentPrintInterface>()}),
    ("PpsDocumentSecurityInterface", Layout {size: size_of::<PpsDocumentSecurityInterface>(), alignment: align_of::<PpsDocumentSecurityInterface>()}),
    ("PpsDocumentTextInterface", Layout {size: size_of::<PpsDocumentTextInterface>(), alignment: align_of::<PpsDocumentTextInterface>()}),
    ("PpsDocumentTransitionInterface", Layout {size: size_of::<PpsDocumentTransitionInterface>(), alignment: align_of::<PpsDocumentTransitionInterface>()}),
    ("PpsDocumentUIHints", Layout {size: size_of::<PpsDocumentUIHints>(), alignment: align_of::<PpsDocumentUIHints>()}),
    ("PpsFileExporterCapabilities", Layout {size: size_of::<PpsFileExporterCapabilities>(), alignment: align_of::<PpsFileExporterCapabilities>()}),
    ("PpsFileExporterContext", Layout {size: size_of::<PpsFileExporterContext>(), alignment: align_of::<PpsFileExporterContext>()}),
    ("PpsFileExporterFormat", Layout {size: size_of::<PpsFileExporterFormat>(), alignment: align_of::<PpsFileExporterFormat>()}),
    ("PpsFileExporterInterface", Layout {size: size_of::<PpsFileExporterInterface>(), alignment: align_of::<PpsFileExporterInterface>()}),
    ("PpsFindOptions", Layout {size: size_of::<PpsFindOptions>(), alignment: align_of::<PpsFindOptions>()}),
    ("PpsFindRectangle", Layout {size: size_of::<PpsFindRectangle>(), alignment: align_of::<PpsFindRectangle>()}),
    ("PpsFontDescription", Layout {size: size_of::<PpsFontDescription>(), alignment: align_of::<PpsFontDescription>()}),
    ("PpsFontDescriptionClass", Layout {size: size_of::<PpsFontDescriptionClass>(), alignment: align_of::<PpsFontDescriptionClass>()}),
    ("PpsFormField", Layout {size: size_of::<PpsFormField>(), alignment: align_of::<PpsFormField>()}),
    ("PpsFormFieldButton", Layout {size: size_of::<PpsFormFieldButton>(), alignment: align_of::<PpsFormFieldButton>()}),
    ("PpsFormFieldButtonClass", Layout {size: size_of::<PpsFormFieldButtonClass>(), alignment: align_of::<PpsFormFieldButtonClass>()}),
    ("PpsFormFieldButtonType", Layout {size: size_of::<PpsFormFieldButtonType>(), alignment: align_of::<PpsFormFieldButtonType>()}),
    ("PpsFormFieldChoiceClass", Layout {size: size_of::<PpsFormFieldChoiceClass>(), alignment: align_of::<PpsFormFieldChoiceClass>()}),
    ("PpsFormFieldChoiceType", Layout {size: size_of::<PpsFormFieldChoiceType>(), alignment: align_of::<PpsFormFieldChoiceType>()}),
    ("PpsFormFieldClass", Layout {size: size_of::<PpsFormFieldClass>(), alignment: align_of::<PpsFormFieldClass>()}),
    ("PpsFormFieldSignature", Layout {size: size_of::<PpsFormFieldSignature>(), alignment: align_of::<PpsFormFieldSignature>()}),
    ("PpsFormFieldSignatureClass", Layout {size: size_of::<PpsFormFieldSignatureClass>(), alignment: align_of::<PpsFormFieldSignatureClass>()}),
    ("PpsFormFieldTextClass", Layout {size: size_of::<PpsFormFieldTextClass>(), alignment: align_of::<PpsFormFieldTextClass>()}),
    ("PpsFormFieldTextType", Layout {size: size_of::<PpsFormFieldTextType>(), alignment: align_of::<PpsFormFieldTextType>()}),
    ("PpsImage", Layout {size: size_of::<PpsImage>(), alignment: align_of::<PpsImage>()}),
    ("PpsImageClass", Layout {size: size_of::<PpsImageClass>(), alignment: align_of::<PpsImageClass>()}),
    ("PpsLayer", Layout {size: size_of::<PpsLayer>(), alignment: align_of::<PpsLayer>()}),
    ("PpsLayerClass", Layout {size: size_of::<PpsLayerClass>(), alignment: align_of::<PpsLayerClass>()}),
    ("PpsLinkActionClass", Layout {size: size_of::<PpsLinkActionClass>(), alignment: align_of::<PpsLinkActionClass>()}),
    ("PpsLinkActionType", Layout {size: size_of::<PpsLinkActionType>(), alignment: align_of::<PpsLinkActionType>()}),
    ("PpsLinkClass", Layout {size: size_of::<PpsLinkClass>(), alignment: align_of::<PpsLinkClass>()}),
    ("PpsLinkDestClass", Layout {size: size_of::<PpsLinkDestClass>(), alignment: align_of::<PpsLinkDestClass>()}),
    ("PpsLinkDestType", Layout {size: size_of::<PpsLinkDestType>(), alignment: align_of::<PpsLinkDestType>()}),
    ("PpsMapping", Layout {size: size_of::<PpsMapping>(), alignment: align_of::<PpsMapping>()}),
    ("PpsMedia", Layout {size: size_of::<PpsMedia>(), alignment: align_of::<PpsMedia>()}),
    ("PpsMediaClass", Layout {size: size_of::<PpsMediaClass>(), alignment: align_of::<PpsMediaClass>()}),
    ("PpsOutlines", Layout {size: size_of::<PpsOutlines>(), alignment: align_of::<PpsOutlines>()}),
    ("PpsOutlinesClass", Layout {size: size_of::<PpsOutlinesClass>(), alignment: align_of::<PpsOutlinesClass>()}),
    ("PpsPage", Layout {size: size_of::<PpsPage>(), alignment: align_of::<PpsPage>()}),
    ("PpsPageClass", Layout {size: size_of::<PpsPageClass>(), alignment: align_of::<PpsPageClass>()}),
    ("PpsPoint", Layout {size: size_of::<PpsPoint>(), alignment: align_of::<PpsPoint>()}),
    ("PpsRectangle", Layout {size: size_of::<PpsRectangle>(), alignment: align_of::<PpsRectangle>()}),
    ("PpsRenderContext", Layout {size: size_of::<PpsRenderContext>(), alignment: align_of::<PpsRenderContext>()}),
    ("PpsRenderContextClass", Layout {size: size_of::<PpsRenderContextClass>(), alignment: align_of::<PpsRenderContextClass>()}),
    ("PpsSelectionInterface", Layout {size: size_of::<PpsSelectionInterface>(), alignment: align_of::<PpsSelectionInterface>()}),
    ("PpsSelectionStyle", Layout {size: size_of::<PpsSelectionStyle>(), alignment: align_of::<PpsSelectionStyle>()}),
    ("PpsTransitionEffect", Layout {size: size_of::<PpsTransitionEffect>(), alignment: align_of::<PpsTransitionEffect>()}),
    ("PpsTransitionEffectAlignment", Layout {size: size_of::<PpsTransitionEffectAlignment>(), alignment: align_of::<PpsTransitionEffectAlignment>()}),
    ("PpsTransitionEffectClass", Layout {size: size_of::<PpsTransitionEffectClass>(), alignment: align_of::<PpsTransitionEffectClass>()}),
    ("PpsTransitionEffectDirection", Layout {size: size_of::<PpsTransitionEffectDirection>(), alignment: align_of::<PpsTransitionEffectDirection>()}),
    ("PpsTransitionEffectType", Layout {size: size_of::<PpsTransitionEffectType>(), alignment: align_of::<PpsTransitionEffectType>()}),
];

const RUST_CONSTANTS: &[(&str, &str)] = &[
    ("(guint) PPS_ANNOTATIONS_SAVE_ALL", "2047"),
    ("(guint) PPS_ANNOTATIONS_SAVE_AREA", "4"),
    ("(guint) PPS_ANNOTATIONS_SAVE_ATTACHMENT", "512"),
    ("(guint) PPS_ANNOTATIONS_SAVE_COLOR", "2"),
    ("(guint) PPS_ANNOTATIONS_SAVE_CONTENTS", "1"),
    ("(guint) PPS_ANNOTATIONS_SAVE_LABEL", "8"),
    ("(guint) PPS_ANNOTATIONS_SAVE_NONE", "0"),
    ("(guint) PPS_ANNOTATIONS_SAVE_OPACITY", "16"),
    ("(guint) PPS_ANNOTATIONS_SAVE_POPUP_IS_OPEN", "64"),
    ("(guint) PPS_ANNOTATIONS_SAVE_POPUP_RECT", "32"),
    ("(guint) PPS_ANNOTATIONS_SAVE_TEXT_ICON", "256"),
    ("(guint) PPS_ANNOTATIONS_SAVE_TEXT_IS_OPEN", "128"),
    ("(guint) PPS_ANNOTATIONS_SAVE_TEXT_MARKUP_TYPE", "1024"),
    ("(gint) PPS_ANNOTATION_OVER_MARKUP_NOT", "3"),
    ("(gint) PPS_ANNOTATION_OVER_MARKUP_NOT_IMPLEMENTED", "0"),
    ("(gint) PPS_ANNOTATION_OVER_MARKUP_UNKNOWN", "1"),
    ("(gint) PPS_ANNOTATION_OVER_MARKUP_YES", "2"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_CIRCLE", "8"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_COMMENT", "1"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_CROSS", "7"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_HELP", "3"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_INSERT", "6"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_KEY", "2"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_NEW_PARAGRAPH", "4"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_NOTE", "0"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_PARAGRAPH", "5"),
    ("(gint) PPS_ANNOTATION_TEXT_ICON_UNKNOWN", "9"),
    ("(gint) PPS_ANNOTATION_TEXT_MARKUP_HIGHLIGHT", "0"),
    ("(gint) PPS_ANNOTATION_TEXT_MARKUP_SQUIGGLY", "3"),
    ("(gint) PPS_ANNOTATION_TEXT_MARKUP_STRIKE_OUT", "1"),
    ("(gint) PPS_ANNOTATION_TEXT_MARKUP_UNDERLINE", "2"),
    ("(gint) PPS_ANNOTATION_TYPE_ATTACHMENT", "2"),
    ("(gint) PPS_ANNOTATION_TYPE_TEXT", "1"),
    ("(gint) PPS_ANNOTATION_TYPE_TEXT_MARKUP", "3"),
    ("(gint) PPS_ANNOTATION_TYPE_UNKNOWN", "0"),
    ("(gint) PPS_COMPRESSION_BZIP2", "1"),
    ("(gint) PPS_COMPRESSION_GZIP", "2"),
    ("(gint) PPS_COMPRESSION_LZMA", "3"),
    ("(gint) PPS_COMPRESSION_NONE", "0"),
    ("(gint) PPS_DOCUMENT_CONTAINS_JS_NO", "1"),
    ("(gint) PPS_DOCUMENT_CONTAINS_JS_UNKNOWN", "0"),
    ("(gint) PPS_DOCUMENT_CONTAINS_JS_YES", "2"),
    ("(gint) PPS_DOCUMENT_ERROR_ENCRYPTED", "2"),
    ("(gint) PPS_DOCUMENT_ERROR_INVALID", "0"),
    ("(gint) PPS_DOCUMENT_ERROR_UNSUPPORTED_CONTENT", "1"),
    ("(guint) PPS_DOCUMENT_INFO_AUTHOR", "4"),
    ("(guint) PPS_DOCUMENT_INFO_CONTAINS_JS", "262144"),
    ("(guint) PPS_DOCUMENT_INFO_CREATION_DATETIME", "256"),
    ("(guint) PPS_DOCUMENT_INFO_CREATOR", "64"),
    ("(guint) PPS_DOCUMENT_INFO_FORMAT", "2"),
    ("(guint) PPS_DOCUMENT_INFO_KEYWORDS", "16"),
    ("(guint) PPS_DOCUMENT_INFO_LAYOUT", "32"),
    ("(guint) PPS_DOCUMENT_INFO_LICENSE", "131072"),
    ("(guint) PPS_DOCUMENT_INFO_LINEARIZED", "1024"),
    ("(guint) PPS_DOCUMENT_INFO_MOD_DATETIME", "512"),
    ("(guint) PPS_DOCUMENT_INFO_N_PAGES", "16384"),
    ("(guint) PPS_DOCUMENT_INFO_PAPER_SIZE", "65536"),
    ("(guint) PPS_DOCUMENT_INFO_PERMISSIONS", "8192"),
    ("(guint) PPS_DOCUMENT_INFO_PRODUCER", "128"),
    ("(guint) PPS_DOCUMENT_INFO_SECURITY", "32768"),
    ("(guint) PPS_DOCUMENT_INFO_START_MODE", "2048"),
    ("(guint) PPS_DOCUMENT_INFO_SUBJECT", "8"),
    ("(guint) PPS_DOCUMENT_INFO_TITLE", "1"),
    ("(guint) PPS_DOCUMENT_INFO_UI_HINTS", "4096"),
    ("(gint) PPS_DOCUMENT_LAYOUT_ONE_COLUMN", "1"),
    ("(gint) PPS_DOCUMENT_LAYOUT_SINGLE_PAGE", "0"),
    ("(gint) PPS_DOCUMENT_LAYOUT_TWO_COLUMN_LEFT", "2"),
    ("(gint) PPS_DOCUMENT_LAYOUT_TWO_COLUMN_RIGHT", "3"),
    ("(gint) PPS_DOCUMENT_LAYOUT_TWO_PAGE_LEFT", "4"),
    ("(gint) PPS_DOCUMENT_LAYOUT_TWO_PAGE_RIGHT", "5"),
    ("(guint) PPS_DOCUMENT_LOAD_FLAG_NONE", "0"),
    ("(guint) PPS_DOCUMENT_LOAD_FLAG_NO_CACHE", "1"),
    ("(gint) PPS_DOCUMENT_MODE_FULL_SCREEN", "3"),
    ("(gint) PPS_DOCUMENT_MODE_NONE", "0"),
    ("(gint) PPS_DOCUMENT_MODE_PRESENTATION", "3"),
    ("(gint) PPS_DOCUMENT_MODE_USE_ATTACHMENTS", "4"),
    ("(gint) PPS_DOCUMENT_MODE_USE_OC", "1"),
    ("(gint) PPS_DOCUMENT_MODE_USE_THUMBS", "2"),
    ("(guint) PPS_DOCUMENT_PERMISSIONS_FULL", "15"),
    ("(guint) PPS_DOCUMENT_PERMISSIONS_OK_TO_ADD_NOTES", "8"),
    ("(guint) PPS_DOCUMENT_PERMISSIONS_OK_TO_COPY", "4"),
    ("(guint) PPS_DOCUMENT_PERMISSIONS_OK_TO_MODIFY", "2"),
    ("(guint) PPS_DOCUMENT_PERMISSIONS_OK_TO_PRINT", "1"),
    ("(guint) PPS_DOCUMENT_UI_HINT_CENTER_WINDOW", "16"),
    ("(guint) PPS_DOCUMENT_UI_HINT_DIRECTION_RTL", "64"),
    ("(guint) PPS_DOCUMENT_UI_HINT_DISPLAY_DOC_TITLE", "32"),
    ("(guint) PPS_DOCUMENT_UI_HINT_FIT_WINDOW", "8"),
    ("(guint) PPS_DOCUMENT_UI_HINT_HIDE_MENUBAR", "2"),
    ("(guint) PPS_DOCUMENT_UI_HINT_HIDE_TOOLBAR", "1"),
    ("(guint) PPS_DOCUMENT_UI_HINT_HIDE_WINDOWUI", "4"),
    ("(guint) PPS_FILE_EXPORTER_CAN_COLLATE", "4"),
    ("(guint) PPS_FILE_EXPORTER_CAN_COPIES", "2"),
    ("(guint) PPS_FILE_EXPORTER_CAN_GENERATE_PDF", "32"),
    ("(guint) PPS_FILE_EXPORTER_CAN_GENERATE_PS", "64"),
    ("(guint) PPS_FILE_EXPORTER_CAN_NUMBER_UP", "256"),
    ("(guint) PPS_FILE_EXPORTER_CAN_PAGE_SET", "1"),
    ("(guint) PPS_FILE_EXPORTER_CAN_PREVIEW", "128"),
    ("(guint) PPS_FILE_EXPORTER_CAN_REVERSE", "8"),
    ("(guint) PPS_FILE_EXPORTER_CAN_SCALE", "16"),
    ("(gint) PPS_FILE_FORMAT_PDF", "2"),
    ("(gint) PPS_FILE_FORMAT_PS", "1"),
    ("(gint) PPS_FILE_FORMAT_UNKNOWN", "0"),
    ("(guint) PPS_FIND_CASE_SENSITIVE", "1"),
    ("(guint) PPS_FIND_DEFAULT", "0"),
    ("(guint) PPS_FIND_WHOLE_WORDS_ONLY", "2"),
    ("(gint) PPS_FORM_FIELD_BUTTON_CHECK", "1"),
    ("(gint) PPS_FORM_FIELD_BUTTON_PUSH", "0"),
    ("(gint) PPS_FORM_FIELD_BUTTON_RADIO", "2"),
    ("(gint) PPS_FORM_FIELD_CHOICE_COMBO", "0"),
    ("(gint) PPS_FORM_FIELD_CHOICE_LIST", "1"),
    ("(gint) PPS_FORM_FIELD_TEXT_FILE_SELECT", "2"),
    ("(gint) PPS_FORM_FIELD_TEXT_MULTILINE", "1"),
    ("(gint) PPS_FORM_FIELD_TEXT_NORMAL", "0"),
    ("(gint) PPS_LINK_ACTION_TYPE_EXTERNAL_URI", "2"),
    ("(gint) PPS_LINK_ACTION_TYPE_GOTO_DEST", "0"),
    ("(gint) PPS_LINK_ACTION_TYPE_GOTO_REMOTE", "1"),
    ("(gint) PPS_LINK_ACTION_TYPE_LAUNCH", "3"),
    ("(gint) PPS_LINK_ACTION_TYPE_LAYERS_STATE", "5"),
    ("(gint) PPS_LINK_ACTION_TYPE_NAMED", "4"),
    ("(gint) PPS_LINK_ACTION_TYPE_RESET_FORM", "6"),
    ("(gint) PPS_LINK_DEST_TYPE_FIT", "2"),
    ("(gint) PPS_LINK_DEST_TYPE_FITH", "3"),
    ("(gint) PPS_LINK_DEST_TYPE_FITR", "5"),
    ("(gint) PPS_LINK_DEST_TYPE_FITV", "4"),
    ("(gint) PPS_LINK_DEST_TYPE_NAMED", "6"),
    ("(gint) PPS_LINK_DEST_TYPE_PAGE", "0"),
    ("(gint) PPS_LINK_DEST_TYPE_PAGE_LABEL", "7"),
    ("(gint) PPS_LINK_DEST_TYPE_UNKNOWN", "8"),
    ("(gint) PPS_LINK_DEST_TYPE_XYZ", "1"),
    ("PPS_MAJOR_VERSION", "46"),
    ("(gint) PPS_SELECTION_STYLE_GLYPH", "0"),
    ("(gint) PPS_SELECTION_STYLE_LINE", "2"),
    ("(gint) PPS_SELECTION_STYLE_WORD", "1"),
    ("(gint) PPS_TRANSITION_ALIGNMENT_HORIZONTAL", "0"),
    ("(gint) PPS_TRANSITION_ALIGNMENT_VERTICAL", "1"),
    ("(gint) PPS_TRANSITION_DIRECTION_INWARD", "0"),
    ("(gint) PPS_TRANSITION_DIRECTION_OUTWARD", "1"),
    ("(gint) PPS_TRANSITION_EFFECT_BLINDS", "2"),
    ("(gint) PPS_TRANSITION_EFFECT_BOX", "3"),
    ("(gint) PPS_TRANSITION_EFFECT_COVER", "9"),
    ("(gint) PPS_TRANSITION_EFFECT_DISSOLVE", "5"),
    ("(gint) PPS_TRANSITION_EFFECT_FADE", "11"),
    ("(gint) PPS_TRANSITION_EFFECT_FLY", "7"),
    ("(gint) PPS_TRANSITION_EFFECT_GLITTER", "6"),
    ("(gint) PPS_TRANSITION_EFFECT_PUSH", "8"),
    ("(gint) PPS_TRANSITION_EFFECT_REPLACE", "0"),
    ("(gint) PPS_TRANSITION_EFFECT_SPLIT", "1"),
    ("(gint) PPS_TRANSITION_EFFECT_UNCOVER", "10"),
    ("(gint) PPS_TRANSITION_EFFECT_WIPE", "4"),
];


