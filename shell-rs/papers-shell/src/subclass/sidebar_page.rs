use glib::translate::*;
use gtk::subclass::prelude::WidgetImpl;
use papers_document::Document;

use crate::{prelude::*, subclass::prelude::*, SidebarPage};

pub trait SidebarPageImpl: ObjectImpl {
    fn support_document(&self, document: &Document) -> bool {
        self.parent_support_document(document)
    }
}

mod sealed {
    pub trait Sealed {}
    impl<T: super::SidebarPageImplExt> Sealed for T {}
}

pub trait SidebarPageImplExt: sealed::Sealed + ObjectSubclass {
    fn parent_support_document(&self, document: &Document) -> bool {
        unsafe {
            let type_data = Self::type_data();
            let parent_class = type_data.as_ref().parent_class() as *const ffi::PpsSidebarPageClass;

            if let Some(func) = (*parent_class).support_document {
                return from_glib(func(
                    self.obj().unsafe_cast_ref::<SidebarPage>().to_glib_none().0,
                    document.to_glib_none().0,
                ));
            }
            false
        }
    }
}

impl<T: SidebarPageImpl> SidebarPageImplExt for T {}

unsafe impl<T: SidebarPageImpl + WidgetImpl> IsSubclassable<T> for SidebarPage {
    fn class_init(class: &mut glib::Class<Self>) {
        Self::parent_class_init::<T>(class);

        let klass = class.as_mut();
        klass.support_document = Some(sidebar_page_support_document::<T>);
    }
}

unsafe extern "C" fn sidebar_page_support_document<T: SidebarPageImpl>(
    sidebar_page: *mut ffi::PpsSidebarPage,
    document: *mut papers_document::ffi::PpsDocument,
) -> glib::ffi::gboolean {
    let instance = &*(sidebar_page as *mut T::Instance);
    let imp = instance.imp();

    imp.support_document(&from_glib_borrow(document))
        .into_glib()
}
