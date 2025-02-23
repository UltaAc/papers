// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::ffi;
use glib::{bitflags::bitflags, prelude::*, translate::*};

bitflags! {
    #[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
    #[doc(alias = "PpsAnnotationsSaveMask")]
    pub struct AnnotationsSaveMask: u32 {
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_NONE")]
        const NONE = ffi::PPS_ANNOTATIONS_SAVE_NONE as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_CONTENTS")]
        const CONTENTS = ffi::PPS_ANNOTATIONS_SAVE_CONTENTS as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_COLOR")]
        const COLOR = ffi::PPS_ANNOTATIONS_SAVE_COLOR as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_AREA")]
        const AREA = ffi::PPS_ANNOTATIONS_SAVE_AREA as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_HIDDEN")]
        const HIDDEN = ffi::PPS_ANNOTATIONS_SAVE_HIDDEN as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_LABEL")]
        const LABEL = ffi::PPS_ANNOTATIONS_SAVE_LABEL as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_OPACITY")]
        const OPACITY = ffi::PPS_ANNOTATIONS_SAVE_OPACITY as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_POPUP_RECT")]
        const POPUP_RECT = ffi::PPS_ANNOTATIONS_SAVE_POPUP_RECT as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_POPUP_IS_OPEN")]
        const POPUP_IS_OPEN = ffi::PPS_ANNOTATIONS_SAVE_POPUP_IS_OPEN as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_TEXT_IS_OPEN")]
        const TEXT_IS_OPEN = ffi::PPS_ANNOTATIONS_SAVE_TEXT_IS_OPEN as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_TEXT_ICON")]
        const TEXT_ICON = ffi::PPS_ANNOTATIONS_SAVE_TEXT_ICON as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_ATTACHMENT")]
        const ATTACHMENT = ffi::PPS_ANNOTATIONS_SAVE_ATTACHMENT as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_TEXT_MARKUP_TYPE")]
        const TEXT_MARKUP_TYPE = ffi::PPS_ANNOTATIONS_SAVE_TEXT_MARKUP_TYPE as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_FREE_TEXT_FONT")]
        const FREE_TEXT_FONT = ffi::PPS_ANNOTATIONS_SAVE_FREE_TEXT_FONT as _;
        #[doc(alias = "PPS_ANNOTATIONS_SAVE_ALL")]
        const ALL = ffi::PPS_ANNOTATIONS_SAVE_ALL as _;
    }
}

#[doc(hidden)]
impl IntoGlib for AnnotationsSaveMask {
    type GlibType = ffi::PpsAnnotationsSaveMask;

    #[inline]
    fn into_glib(self) -> ffi::PpsAnnotationsSaveMask {
        self.bits()
    }
}

#[doc(hidden)]
impl FromGlib<ffi::PpsAnnotationsSaveMask> for AnnotationsSaveMask {
    #[inline]
    unsafe fn from_glib(value: ffi::PpsAnnotationsSaveMask) -> Self {
        skip_assert_initialized!();
        Self::from_bits_truncate(value)
    }
}

impl StaticType for AnnotationsSaveMask {
    #[inline]
    #[doc(alias = "pps_annotations_save_mask_get_type")]
    fn static_type() -> glib::Type {
        unsafe { from_glib(ffi::pps_annotations_save_mask_get_type()) }
    }
}

impl glib::HasParamSpec for AnnotationsSaveMask {
    type ParamSpec = glib::ParamSpecFlags;
    type SetValue = Self;
    type BuilderFn = fn(&str) -> glib::ParamSpecFlagsBuilder<Self>;

    fn param_spec_builder() -> Self::BuilderFn {
        Self::ParamSpec::builder
    }
}

impl glib::value::ValueType for AnnotationsSaveMask {
    type Type = Self;
}

unsafe impl<'a> glib::value::FromValue<'a> for AnnotationsSaveMask {
    type Checker = glib::value::GenericValueTypeChecker<Self>;

    #[inline]
    unsafe fn from_value(value: &'a glib::Value) -> Self {
        skip_assert_initialized!();
        from_glib(glib::gobject_ffi::g_value_get_flags(value.to_glib_none().0))
    }
}

impl ToValue for AnnotationsSaveMask {
    #[inline]
    fn to_value(&self) -> glib::Value {
        let mut value = glib::Value::for_value_type::<Self>();
        unsafe {
            glib::gobject_ffi::g_value_set_flags(value.to_glib_none_mut().0, self.into_glib());
        }
        value
    }

    #[inline]
    fn value_type(&self) -> glib::Type {
        Self::static_type()
    }
}

impl From<AnnotationsSaveMask> for glib::Value {
    #[inline]
    fn from(v: AnnotationsSaveMask) -> Self {
        skip_assert_initialized!();
        ToValue::to_value(&v)
    }
}

bitflags! {
    #[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
    #[doc(alias = "PpsDocumentInfoFields")]
    pub struct DocumentInfoFields: u32 {
        #[doc(alias = "PPS_DOCUMENT_INFO_TITLE")]
        const TITLE = ffi::PPS_DOCUMENT_INFO_TITLE as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_FORMAT")]
        const FORMAT = ffi::PPS_DOCUMENT_INFO_FORMAT as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_AUTHOR")]
        const AUTHOR = ffi::PPS_DOCUMENT_INFO_AUTHOR as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_SUBJECT")]
        const SUBJECT = ffi::PPS_DOCUMENT_INFO_SUBJECT as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_KEYWORDS")]
        const KEYWORDS = ffi::PPS_DOCUMENT_INFO_KEYWORDS as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_LAYOUT")]
        const LAYOUT = ffi::PPS_DOCUMENT_INFO_LAYOUT as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_CREATOR")]
        const CREATOR = ffi::PPS_DOCUMENT_INFO_CREATOR as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_PRODUCER")]
        const PRODUCER = ffi::PPS_DOCUMENT_INFO_PRODUCER as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_CREATION_DATETIME")]
        const CREATION_DATETIME = ffi::PPS_DOCUMENT_INFO_CREATION_DATETIME as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_MOD_DATETIME")]
        const MOD_DATETIME = ffi::PPS_DOCUMENT_INFO_MOD_DATETIME as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_LINEARIZED")]
        const LINEARIZED = ffi::PPS_DOCUMENT_INFO_LINEARIZED as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_START_MODE")]
        const START_MODE = ffi::PPS_DOCUMENT_INFO_START_MODE as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_UI_HINTS")]
        const UI_HINTS = ffi::PPS_DOCUMENT_INFO_UI_HINTS as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_PERMISSIONS")]
        const PERMISSIONS = ffi::PPS_DOCUMENT_INFO_PERMISSIONS as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_N_PAGES")]
        const N_PAGES = ffi::PPS_DOCUMENT_INFO_N_PAGES as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_SECURITY")]
        const SECURITY = ffi::PPS_DOCUMENT_INFO_SECURITY as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_PAPER_SIZE")]
        const PAPER_SIZE = ffi::PPS_DOCUMENT_INFO_PAPER_SIZE as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_LICENSE")]
        const LICENSE = ffi::PPS_DOCUMENT_INFO_LICENSE as _;
        #[doc(alias = "PPS_DOCUMENT_INFO_CONTAINS_JS")]
        const CONTAINS_JS = ffi::PPS_DOCUMENT_INFO_CONTAINS_JS as _;
    }
}

#[doc(hidden)]
impl IntoGlib for DocumentInfoFields {
    type GlibType = ffi::PpsDocumentInfoFields;

    #[inline]
    fn into_glib(self) -> ffi::PpsDocumentInfoFields {
        self.bits()
    }
}

#[doc(hidden)]
impl FromGlib<ffi::PpsDocumentInfoFields> for DocumentInfoFields {
    #[inline]
    unsafe fn from_glib(value: ffi::PpsDocumentInfoFields) -> Self {
        skip_assert_initialized!();
        Self::from_bits_truncate(value)
    }
}

impl StaticType for DocumentInfoFields {
    #[inline]
    #[doc(alias = "pps_document_info_fields_get_type")]
    fn static_type() -> glib::Type {
        unsafe { from_glib(ffi::pps_document_info_fields_get_type()) }
    }
}

impl glib::HasParamSpec for DocumentInfoFields {
    type ParamSpec = glib::ParamSpecFlags;
    type SetValue = Self;
    type BuilderFn = fn(&str) -> glib::ParamSpecFlagsBuilder<Self>;

    fn param_spec_builder() -> Self::BuilderFn {
        Self::ParamSpec::builder
    }
}

impl glib::value::ValueType for DocumentInfoFields {
    type Type = Self;
}

unsafe impl<'a> glib::value::FromValue<'a> for DocumentInfoFields {
    type Checker = glib::value::GenericValueTypeChecker<Self>;

    #[inline]
    unsafe fn from_value(value: &'a glib::Value) -> Self {
        skip_assert_initialized!();
        from_glib(glib::gobject_ffi::g_value_get_flags(value.to_glib_none().0))
    }
}

impl ToValue for DocumentInfoFields {
    #[inline]
    fn to_value(&self) -> glib::Value {
        let mut value = glib::Value::for_value_type::<Self>();
        unsafe {
            glib::gobject_ffi::g_value_set_flags(value.to_glib_none_mut().0, self.into_glib());
        }
        value
    }

    #[inline]
    fn value_type(&self) -> glib::Type {
        Self::static_type()
    }
}

impl From<DocumentInfoFields> for glib::Value {
    #[inline]
    fn from(v: DocumentInfoFields) -> Self {
        skip_assert_initialized!();
        ToValue::to_value(&v)
    }
}

bitflags! {
    #[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
    #[doc(alias = "PpsDocumentLoadFlags")]
    pub struct DocumentLoadFlags: u32 {
        #[doc(alias = "PPS_DOCUMENT_LOAD_FLAG_NONE")]
        const NONE = ffi::PPS_DOCUMENT_LOAD_FLAG_NONE as _;
        #[doc(alias = "PPS_DOCUMENT_LOAD_FLAG_NO_CACHE")]
        const NO_CACHE = ffi::PPS_DOCUMENT_LOAD_FLAG_NO_CACHE as _;
    }
}

#[doc(hidden)]
impl IntoGlib for DocumentLoadFlags {
    type GlibType = ffi::PpsDocumentLoadFlags;

    #[inline]
    fn into_glib(self) -> ffi::PpsDocumentLoadFlags {
        self.bits()
    }
}

#[doc(hidden)]
impl FromGlib<ffi::PpsDocumentLoadFlags> for DocumentLoadFlags {
    #[inline]
    unsafe fn from_glib(value: ffi::PpsDocumentLoadFlags) -> Self {
        skip_assert_initialized!();
        Self::from_bits_truncate(value)
    }
}

impl StaticType for DocumentLoadFlags {
    #[inline]
    #[doc(alias = "pps_document_load_flags_get_type")]
    fn static_type() -> glib::Type {
        unsafe { from_glib(ffi::pps_document_load_flags_get_type()) }
    }
}

impl glib::HasParamSpec for DocumentLoadFlags {
    type ParamSpec = glib::ParamSpecFlags;
    type SetValue = Self;
    type BuilderFn = fn(&str) -> glib::ParamSpecFlagsBuilder<Self>;

    fn param_spec_builder() -> Self::BuilderFn {
        Self::ParamSpec::builder
    }
}

impl glib::value::ValueType for DocumentLoadFlags {
    type Type = Self;
}

unsafe impl<'a> glib::value::FromValue<'a> for DocumentLoadFlags {
    type Checker = glib::value::GenericValueTypeChecker<Self>;

    #[inline]
    unsafe fn from_value(value: &'a glib::Value) -> Self {
        skip_assert_initialized!();
        from_glib(glib::gobject_ffi::g_value_get_flags(value.to_glib_none().0))
    }
}

impl ToValue for DocumentLoadFlags {
    #[inline]
    fn to_value(&self) -> glib::Value {
        let mut value = glib::Value::for_value_type::<Self>();
        unsafe {
            glib::gobject_ffi::g_value_set_flags(value.to_glib_none_mut().0, self.into_glib());
        }
        value
    }

    #[inline]
    fn value_type(&self) -> glib::Type {
        Self::static_type()
    }
}

impl From<DocumentLoadFlags> for glib::Value {
    #[inline]
    fn from(v: DocumentLoadFlags) -> Self {
        skip_assert_initialized!();
        ToValue::to_value(&v)
    }
}

bitflags! {
    #[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
    #[doc(alias = "PpsDocumentPermissions")]
    pub struct DocumentPermissions: u32 {
        #[doc(alias = "PPS_DOCUMENT_PERMISSIONS_OK_TO_PRINT")]
        const OK_TO_PRINT = ffi::PPS_DOCUMENT_PERMISSIONS_OK_TO_PRINT as _;
        #[doc(alias = "PPS_DOCUMENT_PERMISSIONS_OK_TO_MODIFY")]
        const OK_TO_MODIFY = ffi::PPS_DOCUMENT_PERMISSIONS_OK_TO_MODIFY as _;
        #[doc(alias = "PPS_DOCUMENT_PERMISSIONS_OK_TO_COPY")]
        const OK_TO_COPY = ffi::PPS_DOCUMENT_PERMISSIONS_OK_TO_COPY as _;
        #[doc(alias = "PPS_DOCUMENT_PERMISSIONS_OK_TO_ADD_NOTES")]
        const OK_TO_ADD_NOTES = ffi::PPS_DOCUMENT_PERMISSIONS_OK_TO_ADD_NOTES as _;
        #[doc(alias = "PPS_DOCUMENT_PERMISSIONS_FULL")]
        const FULL = ffi::PPS_DOCUMENT_PERMISSIONS_FULL as _;
    }
}

#[doc(hidden)]
impl IntoGlib for DocumentPermissions {
    type GlibType = ffi::PpsDocumentPermissions;

    #[inline]
    fn into_glib(self) -> ffi::PpsDocumentPermissions {
        self.bits()
    }
}

#[doc(hidden)]
impl FromGlib<ffi::PpsDocumentPermissions> for DocumentPermissions {
    #[inline]
    unsafe fn from_glib(value: ffi::PpsDocumentPermissions) -> Self {
        skip_assert_initialized!();
        Self::from_bits_truncate(value)
    }
}

impl StaticType for DocumentPermissions {
    #[inline]
    #[doc(alias = "pps_document_permissions_get_type")]
    fn static_type() -> glib::Type {
        unsafe { from_glib(ffi::pps_document_permissions_get_type()) }
    }
}

impl glib::HasParamSpec for DocumentPermissions {
    type ParamSpec = glib::ParamSpecFlags;
    type SetValue = Self;
    type BuilderFn = fn(&str) -> glib::ParamSpecFlagsBuilder<Self>;

    fn param_spec_builder() -> Self::BuilderFn {
        Self::ParamSpec::builder
    }
}

impl glib::value::ValueType for DocumentPermissions {
    type Type = Self;
}

unsafe impl<'a> glib::value::FromValue<'a> for DocumentPermissions {
    type Checker = glib::value::GenericValueTypeChecker<Self>;

    #[inline]
    unsafe fn from_value(value: &'a glib::Value) -> Self {
        skip_assert_initialized!();
        from_glib(glib::gobject_ffi::g_value_get_flags(value.to_glib_none().0))
    }
}

impl ToValue for DocumentPermissions {
    #[inline]
    fn to_value(&self) -> glib::Value {
        let mut value = glib::Value::for_value_type::<Self>();
        unsafe {
            glib::gobject_ffi::g_value_set_flags(value.to_glib_none_mut().0, self.into_glib());
        }
        value
    }

    #[inline]
    fn value_type(&self) -> glib::Type {
        Self::static_type()
    }
}

impl From<DocumentPermissions> for glib::Value {
    #[inline]
    fn from(v: DocumentPermissions) -> Self {
        skip_assert_initialized!();
        ToValue::to_value(&v)
    }
}

bitflags! {
    #[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
    #[doc(alias = "PpsFindOptions")]
    pub struct FindOptions: u32 {
        #[doc(alias = "PPS_FIND_DEFAULT")]
        const DEFAULT = ffi::PPS_FIND_DEFAULT as _;
        #[doc(alias = "PPS_FIND_CASE_SENSITIVE")]
        const CASE_SENSITIVE = ffi::PPS_FIND_CASE_SENSITIVE as _;
        #[doc(alias = "PPS_FIND_WHOLE_WORDS_ONLY")]
        const WHOLE_WORDS_ONLY = ffi::PPS_FIND_WHOLE_WORDS_ONLY as _;
    }
}

#[doc(hidden)]
impl IntoGlib for FindOptions {
    type GlibType = ffi::PpsFindOptions;

    #[inline]
    fn into_glib(self) -> ffi::PpsFindOptions {
        self.bits()
    }
}

#[doc(hidden)]
impl FromGlib<ffi::PpsFindOptions> for FindOptions {
    #[inline]
    unsafe fn from_glib(value: ffi::PpsFindOptions) -> Self {
        skip_assert_initialized!();
        Self::from_bits_truncate(value)
    }
}

impl StaticType for FindOptions {
    #[inline]
    #[doc(alias = "pps_find_options_get_type")]
    fn static_type() -> glib::Type {
        unsafe { from_glib(ffi::pps_find_options_get_type()) }
    }
}

impl glib::HasParamSpec for FindOptions {
    type ParamSpec = glib::ParamSpecFlags;
    type SetValue = Self;
    type BuilderFn = fn(&str) -> glib::ParamSpecFlagsBuilder<Self>;

    fn param_spec_builder() -> Self::BuilderFn {
        Self::ParamSpec::builder
    }
}

impl glib::value::ValueType for FindOptions {
    type Type = Self;
}

unsafe impl<'a> glib::value::FromValue<'a> for FindOptions {
    type Checker = glib::value::GenericValueTypeChecker<Self>;

    #[inline]
    unsafe fn from_value(value: &'a glib::Value) -> Self {
        skip_assert_initialized!();
        from_glib(glib::gobject_ffi::g_value_get_flags(value.to_glib_none().0))
    }
}

impl ToValue for FindOptions {
    #[inline]
    fn to_value(&self) -> glib::Value {
        let mut value = glib::Value::for_value_type::<Self>();
        unsafe {
            glib::gobject_ffi::g_value_set_flags(value.to_glib_none_mut().0, self.into_glib());
        }
        value
    }

    #[inline]
    fn value_type(&self) -> glib::Type {
        Self::static_type()
    }
}

impl From<FindOptions> for glib::Value {
    #[inline]
    fn from(v: FindOptions) -> Self {
        skip_assert_initialized!();
        ToValue::to_value(&v)
    }
}
