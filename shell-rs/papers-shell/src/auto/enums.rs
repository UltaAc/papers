// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use glib::{prelude::*,translate::*};

#[derive(Debug, Eq, PartialEq, Ord, PartialOrd, Hash)]
#[derive(Clone, Copy)]
#[non_exhaustive]
#[doc(alias = "PpsWindowRunMode")]
pub enum WindowRunMode {
    #[doc(alias = "PPS_WINDOW_MODE_NORMAL")]
    Normal,
    #[doc(alias = "PPS_WINDOW_MODE_FULLSCREEN")]
    Fullscreen,
    #[doc(alias = "PPS_WINDOW_MODE_PRESENTATION")]
    Presentation,
    #[doc(alias = "PPS_WINDOW_MODE_START_VIEW")]
    StartView,
    #[doc(alias = "PPS_WINDOW_MODE_ERROR_VIEW")]
    ErrorView,
    #[doc(alias = "PPS_WINDOW_MODE_PASSWORD_VIEW")]
    PasswordView,
    #[doc(alias = "PPS_WINDOW_MODE_LOADER_VIEW")]
    LoaderView,
#[doc(hidden)]
    __Unknown(i32),
}

#[doc(hidden)]
impl IntoGlib for WindowRunMode {
    type GlibType = ffi::PpsWindowRunMode;

    #[inline]
fn into_glib(self) -> ffi::PpsWindowRunMode {
match self {
            Self::Normal => ffi::PPS_WINDOW_MODE_NORMAL,
            Self::Fullscreen => ffi::PPS_WINDOW_MODE_FULLSCREEN,
            Self::Presentation => ffi::PPS_WINDOW_MODE_PRESENTATION,
            Self::StartView => ffi::PPS_WINDOW_MODE_START_VIEW,
            Self::ErrorView => ffi::PPS_WINDOW_MODE_ERROR_VIEW,
            Self::PasswordView => ffi::PPS_WINDOW_MODE_PASSWORD_VIEW,
            Self::LoaderView => ffi::PPS_WINDOW_MODE_LOADER_VIEW,
            Self::__Unknown(value) => value,
}
}
}

#[doc(hidden)]
impl FromGlib<ffi::PpsWindowRunMode> for WindowRunMode {
    #[inline]
unsafe fn from_glib(value: ffi::PpsWindowRunMode) -> Self {
        skip_assert_initialized!();
        
match value {
            ffi::PPS_WINDOW_MODE_NORMAL => Self::Normal,
            ffi::PPS_WINDOW_MODE_FULLSCREEN => Self::Fullscreen,
            ffi::PPS_WINDOW_MODE_PRESENTATION => Self::Presentation,
            ffi::PPS_WINDOW_MODE_START_VIEW => Self::StartView,
            ffi::PPS_WINDOW_MODE_ERROR_VIEW => Self::ErrorView,
            ffi::PPS_WINDOW_MODE_PASSWORD_VIEW => Self::PasswordView,
            ffi::PPS_WINDOW_MODE_LOADER_VIEW => Self::LoaderView,
            value => Self::__Unknown(value),
}
}
}

impl StaticType for WindowRunMode {
                #[inline]
    #[doc(alias = "pps_window_run_mode_get_type")]
   fn static_type() -> glib::Type {
                    unsafe { from_glib(ffi::pps_window_run_mode_get_type()) }
                }
            }

impl glib::HasParamSpec for WindowRunMode {
                type ParamSpec = glib::ParamSpecEnum;
                type SetValue = Self;
                type BuilderFn = fn(&str, Self) -> glib::ParamSpecEnumBuilder<Self>;
    
                fn param_spec_builder() -> Self::BuilderFn {
                    Self::ParamSpec::builder_with_default
                }
}

impl glib::value::ValueType for WindowRunMode {
    type Type = Self;
}

unsafe impl<'a> glib::value::FromValue<'a> for WindowRunMode {
    type Checker = glib::value::GenericValueTypeChecker<Self>;

    #[inline]
    unsafe fn from_value(value: &'a glib::Value) -> Self {
        skip_assert_initialized!();
        from_glib(glib::gobject_ffi::g_value_get_enum(value.to_glib_none().0))
    }
}

impl ToValue for WindowRunMode {
    #[inline]
    fn to_value(&self) -> glib::Value {
        let mut value = glib::Value::for_value_type::<Self>();
        unsafe {
            glib::gobject_ffi::g_value_set_enum(value.to_glib_none_mut().0, self.into_glib());
        }
        value
    }

    #[inline]
    fn value_type(&self) -> glib::Type {
        Self::static_type()
    }
}

impl From<WindowRunMode> for glib::Value {
    #[inline]
    fn from(v: WindowRunMode) -> Self {
        skip_assert_initialized!();
        ToValue::to_value(&v)
    }
}

