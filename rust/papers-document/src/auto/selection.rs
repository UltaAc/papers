// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../gir-files
// from ../pps-girs
// DO NOT EDIT

use crate::{ffi, Page, Rectangle, SelectionStyle};
use glib::{prelude::*, translate::*};

glib::wrapper! {
    #[doc(alias = "PpsSelection")]
    pub struct Selection(Interface<ffi::PpsSelection, ffi::PpsSelectionInterface>);

    match fn {
        type_ => || ffi::pps_selection_get_type(),
    }
}

impl Selection {
    pub const NONE: Option<&'static Selection> = None;
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::IsA<super::Selection>> Sealed for T {}
}

pub trait SelectionExt: IsA<Selection> + sealed::Sealed + 'static {
    #[doc(alias = "pps_selection_get_selected_text")]
    #[doc(alias = "get_selected_text")]
    fn selected_text(
        &self,
        page: &impl IsA<Page>,
        style: SelectionStyle,
        points: &mut Rectangle,
    ) -> Option<glib::GString> {
        unsafe {
            from_glib_full(ffi::pps_selection_get_selected_text(
                self.as_ref().to_glib_none().0,
                page.as_ref().to_glib_none().0,
                style.into_glib(),
                points.to_glib_none_mut().0,
            ))
        }
    }

    //#[doc(alias = "pps_selection_get_selection_region")]
    //#[doc(alias = "get_selection_region")]
    //fn selection_region(&self, rc: /*Ignored*/&RenderContext, style: SelectionStyle, points: &mut Rectangle) -> /*Ignored*/Option<cairo::Region> {
    //    unsafe { TODO: call ffi:pps_selection_get_selection_region() }
    //}

    //#[doc(alias = "pps_selection_render_selection")]
    //fn render_selection(&self, rc: /*Ignored*/&RenderContext, surface: /*Ignored*/&mut cairo::Surface, points: &mut Rectangle, old_points: &mut Rectangle, style: SelectionStyle, text: &mut gdk::RGBA, base: &mut gdk::RGBA) {
    //    unsafe { TODO: call ffi:pps_selection_render_selection() }
    //}
}

impl<O: IsA<Selection>> SelectionExt for O {}
