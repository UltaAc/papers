// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{Layer, LinkActionType, LinkDest};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsLinkAction")]
    pub struct LinkAction(Object<ffi::PpsLinkAction, ffi::PpsLinkActionClass>);

    match fn {
        type_ => || ffi::pps_link_action_get_type(),
    }
}

impl LinkAction {
    #[doc(alias = "pps_link_action_new_dest")]
    pub fn new_dest(dest: &LinkDest) -> LinkAction {
        skip_assert_initialized!();
        unsafe { from_glib_full(ffi::pps_link_action_new_dest(dest.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_action_new_external_uri")]
    pub fn new_external_uri(uri: &str) -> LinkAction {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_link_action_new_external_uri(uri.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_action_new_launch")]
    pub fn new_launch(filename: &str, params: &str) -> LinkAction {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_link_action_new_launch(
                filename.to_glib_none().0,
                params.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_new_layers_state")]
    pub fn new_layers_state(
        show_list: &[Layer],
        hide_list: &[Layer],
        toggle_list: &[Layer],
    ) -> LinkAction {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_link_action_new_layers_state(
                show_list.to_glib_none().0,
                hide_list.to_glib_none().0,
                toggle_list.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_new_named")]
    pub fn new_named(name: &str) -> LinkAction {
        assert_initialized_main_thread!();
        unsafe { from_glib_full(ffi::pps_link_action_new_named(name.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_action_new_remote")]
    pub fn new_remote(dest: &LinkDest, filename: &str) -> LinkAction {
        skip_assert_initialized!();
        unsafe {
            from_glib_full(ffi::pps_link_action_new_remote(
                dest.to_glib_none().0,
                filename.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_new_reset_form")]
    pub fn new_reset_form(fields: &[&str], exclude_fields: bool) -> LinkAction {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_link_action_new_reset_form(
                fields.to_glib_none().0,
                exclude_fields.into_glib(),
            ))
        }
    }

    #[doc(alias = "pps_link_action_equal")]
    pub fn equal(&self, b: &LinkAction) -> bool {
        unsafe {
            from_glib(ffi::pps_link_action_equal(
                self.to_glib_none().0,
                b.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_get_action_type")]
    #[doc(alias = "get_action_type")]
    pub fn action_type(&self) -> LinkActionType {
        unsafe { from_glib(ffi::pps_link_action_get_action_type(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_action_get_dest")]
    #[doc(alias = "get_dest")]
    pub fn dest(&self) -> Option<LinkDest> {
        unsafe { from_glib_none(ffi::pps_link_action_get_dest(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_action_get_exclude_reset_fields")]
    #[doc(alias = "get_exclude_reset_fields")]
    pub fn is_exclude_reset_fields(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_link_action_get_exclude_reset_fields(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_get_filename")]
    #[doc(alias = "get_filename")]
    pub fn filename(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_link_action_get_filename(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_action_get_hide_list")]
    #[doc(alias = "get_hide_list")]
    pub fn hide_list(&self) -> Vec<Layer> {
        unsafe {
            FromGlibPtrContainer::from_glib_none(ffi::pps_link_action_get_hide_list(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_get_name")]
    #[doc(alias = "get_name")]
    pub fn name(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_link_action_get_name(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_action_get_params")]
    #[doc(alias = "get_params")]
    pub fn params(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_link_action_get_params(self.to_glib_none().0)) }
    }

    #[doc(alias = "pps_link_action_get_reset_fields")]
    #[doc(alias = "get_reset_fields")]
    pub fn reset_fields(&self) -> Vec<glib::GString> {
        unsafe {
            FromGlibPtrContainer::from_glib_none(ffi::pps_link_action_get_reset_fields(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_get_show_list")]
    #[doc(alias = "get_show_list")]
    pub fn show_list(&self) -> Vec<Layer> {
        unsafe {
            FromGlibPtrContainer::from_glib_none(ffi::pps_link_action_get_show_list(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_get_toggle_list")]
    #[doc(alias = "get_toggle_list")]
    pub fn toggle_list(&self) -> Vec<Layer> {
        unsafe {
            FromGlibPtrContainer::from_glib_none(ffi::pps_link_action_get_toggle_list(
                self.to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_link_action_get_uri")]
    #[doc(alias = "get_uri")]
    pub fn uri(&self) -> Option<glib::GString> {
        unsafe { from_glib_none(ffi::pps_link_action_get_uri(self.to_glib_none().0)) }
    }

    #[doc(alias = "type")]
    pub fn type_(&self) -> LinkActionType {
        ObjectExt::property(self, "type")
    }
}
