// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::{ffi, LinkAction, MappingList, Page};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsDocumentForms")]
    pub struct DocumentForms(Interface<ffi::PpsDocumentForms, ffi::PpsDocumentFormsInterface>);

    match fn {
        type_ => || ffi::pps_document_forms_get_type(),
    }
}

impl DocumentForms {
    pub const NONE: Option<&'static DocumentForms> = None;
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::DocumentForms>> Sealed for T {}
}

pub trait DocumentFormsExt: IsA<DocumentForms> + sealed::Sealed + 'static {
    #[doc(alias = "pps_document_forms_document_is_modified")]
    fn document_is_modified(&self) -> bool {
        unsafe {
            from_glib(ffi::pps_document_forms_document_is_modified(
                self.as_ref().to_glib_none().0,
            ))
        }
    }

    //#[doc(alias = "pps_document_forms_form_field_button_get_state")]
    //fn form_field_button_get_state(&self, field: /*Ignored*/&FormField) -> bool {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_button_get_state() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_button_set_state")]
    //fn form_field_button_set_state(&self, field: /*Ignored*/&FormField, state: bool) {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_button_set_state() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_choice_get_item")]
    //fn form_field_choice_get_item(&self, field: /*Ignored*/&FormField, index: i32) -> Option<glib::GString> {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_choice_get_item() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_choice_get_n_items")]
    //fn form_field_choice_get_n_items(&self, field: /*Ignored*/&FormField) -> i32 {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_choice_get_n_items() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_choice_get_text")]
    //fn form_field_choice_get_text(&self, field: /*Ignored*/&FormField) -> Option<glib::GString> {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_choice_get_text() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_choice_is_item_selected")]
    //fn form_field_choice_is_item_selected(&self, field: /*Ignored*/&FormField, index: i32) -> bool {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_choice_is_item_selected() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_choice_select_item")]
    //fn form_field_choice_select_item(&self, field: /*Ignored*/&FormField, index: i32) {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_choice_select_item() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_choice_set_text")]
    //fn form_field_choice_set_text(&self, field: /*Ignored*/&FormField, text: &str) {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_choice_set_text() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_choice_toggle_item")]
    //fn form_field_choice_toggle_item(&self, field: /*Ignored*/&FormField, index: i32) {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_choice_toggle_item() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_choice_unselect_all")]
    //fn form_field_choice_unselect_all(&self, field: /*Ignored*/&FormField) {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_choice_unselect_all() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_text_get_text")]
    //fn form_field_text_get_text(&self, field: /*Ignored*/&FormField) -> Option<glib::GString> {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_text_get_text() }
    //}

    //#[doc(alias = "pps_document_forms_form_field_text_set_text")]
    //fn form_field_text_set_text(&self, field: /*Ignored*/&FormField, text: &str) {
    //    unsafe { TODO: call ffi:pps_document_forms_form_field_text_set_text() }
    //}

    #[doc(alias = "pps_document_forms_get_form_fields")]
    #[doc(alias = "get_form_fields")]
    fn form_fields(&self, page: &impl IsA<Page>) -> Option<MappingList> {
        unsafe {
            from_glib_full(ffi::pps_document_forms_get_form_fields(
                self.as_ref().to_glib_none().0,
                page.as_ref().to_glib_none().0,
            ))
        }
    }

    #[doc(alias = "pps_document_forms_reset_form")]
    fn reset_form(&self, action: &LinkAction) {
        unsafe {
            ffi::pps_document_forms_reset_form(
                self.as_ref().to_glib_none().0,
                action.to_glib_none().0,
            );
        }
    }
}

impl<O: IsA<DocumentForms>> DocumentFormsExt for O {}
