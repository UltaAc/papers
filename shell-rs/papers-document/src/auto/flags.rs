// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use glib::{bitflags::bitflags, prelude::*, translate::*};

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
