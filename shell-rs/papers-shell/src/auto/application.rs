// This file was generated by gir (https://github.com/gtk-rs/gir)
// from ../ev-girs
// from ../gir-files
// DO NOT EDIT

use crate::{WindowRunMode};
use glib::{prelude::*,translate::*};

glib::wrapper! {
    #[doc(alias = "PpsApplication")]
    pub struct Application(Object<ffi::PpsApplication, ffi::PpsApplicationClass>) @extends adw::Application, gtk::Application, gio::Application, @implements gio::ActionGroup, gio::ActionMap;

    match fn {
        type_ => || ffi::pps_application_get_type(),
    }
}

impl Application {
    #[doc(alias = "pps_application_new")]
    pub fn new() -> Application {
        assert_initialized_main_thread!();
        unsafe {
            from_glib_full(ffi::pps_application_new())
        }
    }

            // rustdoc-stripper-ignore-next
            /// Creates a new builder-pattern struct instance to construct [`Application`] objects.
            ///
            /// This method returns an instance of [`ApplicationBuilder`](crate::builders::ApplicationBuilder) which can be used to create [`Application`] objects.
            pub fn builder() -> ApplicationBuilder {
                ApplicationBuilder::new()
            }
        

    #[doc(alias = "pps_application_get_n_windows")]
    #[doc(alias = "get_n_windows")]
    pub fn n_windows(&self) -> u32 {
        unsafe {
            ffi::pps_application_get_n_windows(self.to_glib_none().0)
        }
    }

    #[doc(alias = "pps_application_open_start_view")]
    pub fn open_start_view(&self) {
        unsafe {
            ffi::pps_application_open_start_view(self.to_glib_none().0);
        }
    }

    #[doc(alias = "pps_application_open_uri_at_dest")]
    pub fn open_uri_at_dest(&self, uri: &str, dest: &papers_document::LinkDest, mode: WindowRunMode) {
        unsafe {
            ffi::pps_application_open_uri_at_dest(self.to_glib_none().0, uri.to_glib_none().0, dest.to_glib_none().0, mode.into_glib());
        }
    }

    #[doc(alias = "pps_application_open_uri_list")]
    pub fn open_uri_list(&self, files: &impl IsA<gio::ListModel>) {
        unsafe {
            ffi::pps_application_open_uri_list(self.to_glib_none().0, files.as_ref().to_glib_none().0);
        }
    }
}

impl Default for Application {
                     fn default() -> Self {
                         Self::new()
                     }
                 }

// rustdoc-stripper-ignore-next
        /// A [builder-pattern] type to construct [`Application`] objects.
        ///
        /// [builder-pattern]: https://doc.rust-lang.org/1.0.0/style/ownership/builders.html
#[must_use = "The builder must be built to be used"]
pub struct ApplicationBuilder {
            builder: glib::object::ObjectBuilder<'static, Application>,
        }

        impl ApplicationBuilder {
        fn new() -> Self {
            Self { builder: glib::object::Object::builder() }
        }

                            //pub fn menubar(self, menubar: &impl IsA</*Ignored*/gio::MenuModel>) -> Self {
                        //    Self { builder: self.builder.property("menubar", menubar.clone().upcast()), }
                        //}

                            pub fn register_session(self, register_session: bool) -> Self {
                            Self { builder: self.builder.property("register-session", register_session), }
                        }

                            pub fn action_group(self, action_group: &impl IsA<gio::ActionGroup>) -> Self {
                            Self { builder: self.builder.property("action-group", action_group.clone().upcast()), }
                        }

                            pub fn application_id(self, application_id: impl Into<glib::GString>) -> Self {
                            Self { builder: self.builder.property("application-id", application_id.into()), }
                        }

                            //pub fn flags(self, flags: /*Ignored*/gio::ApplicationFlags) -> Self {
                        //    Self { builder: self.builder.property("flags", flags), }
                        //}

                            pub fn inactivity_timeout(self, inactivity_timeout: u32) -> Self {
                            Self { builder: self.builder.property("inactivity-timeout", inactivity_timeout), }
                        }

                            pub fn resource_base_path(self, resource_base_path: impl Into<glib::GString>) -> Self {
                            Self { builder: self.builder.property("resource-base-path", resource_base_path.into()), }
                        }

    // rustdoc-stripper-ignore-next
    /// Build the [`Application`].
    #[must_use = "Building the object from the builder is usually expensive and is not expected to have side effects"]
    pub fn build(self) -> Application {
    self.builder.build() }
}
