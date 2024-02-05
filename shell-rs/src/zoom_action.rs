use crate::deps::*;

use papers_document::widget_dpi;
use papers_view::SizingMode;

mod imp {
    use super::*;

    #[derive(Properties, Default, Debug, CompositeTemplate)]
    #[properties(wrapper_type = super::PpsZoomAction)]
    #[template(resource = "/org/gnome/papers/ui/zoom-action.ui")]
    pub struct PpsZoomAction {
        #[template_child]
        pub(super) entry: TemplateChild<gtk::Entry>,
        #[template_child]
        pub(super) popup: TemplateChild<gtk::Popover>,
        #[template_child]
        pub(super) free_section: TemplateChild<gio::Menu>,
        #[property(name = "document-model", nullable, set = Self::set_model)]
        pub(super) model: RefCell<Option<DocumentModel>>,
    }

    #[glib::object_subclass]
    impl ObjectSubclass for PpsZoomAction {
        const NAME: &'static str = "PpsZoomAction";
        type Type = super::PpsZoomAction;
        type ParentType = gtk::Box;

        fn class_init(klass: &mut Self::Class) {
            klass.bind_template();
            klass.bind_template_callbacks();
        }

        fn instance_init(obj: &InitializingObject<Self>) {
            obj.init_template();
        }
    }

    #[glib::derived_properties]
    impl ObjectImpl for PpsZoomAction {
        fn signals() -> &'static [Signal] {
            static SIGNALS: OnceLock<Vec<Signal>> = OnceLock::new();
            SIGNALS.get_or_init(|| vec![Signal::builder("activated").run_last().build()])
        }

        fn constructed(&self) {
            self.parent_constructed();
            self.setup_initial_entry_size();
        }
    }

    impl BoxImpl for PpsZoomAction {}

    impl WidgetImpl for PpsZoomAction {}

    #[gtk::template_callbacks]
    impl PpsZoomAction {
        #[template_callback]
        fn entry_icon_press(&self, _pos: gtk::EntryIconPosition, entry: &gtk::Entry) {
            // This cannot be done during init, as window does not yet exist
            // therefore the rectangle is not yet available */
            if let (false, _) = self.popup.pointing_to() {
                let rect = entry.icon_area(gtk::EntryIconPosition::Secondary);
                self.popup.set_pointing_to(Some(&rect));
            }

            self.popup.popup();
        }

        #[template_callback]
        fn entry_activated(&self, entry: &gtk::Entry) {
            let text = entry.text();
            let trimed_text = text.trim_end_matches('%');
            let zoom_perc = trimed_text.parse::<f64>();

            if let Some(model) = self.model() {
                if let Ok(zoom_perc) = zoom_perc {
                    let zoom = zoom_perc / 100.0;
                    model.set_sizing_mode(SizingMode::Free);
                    model.set_scale(zoom * widget_dpi(&self.obj().clone()) / 72.0);
                } else {
                    self.update_zoom_level();
                }
            }

            self.obj().emit_by_name::<()>("activated", &[]);
        }

        #[template_callback]
        fn update_zoom_level(&self) {
            if let Some(document) = self.model() {
                self.set_zoom_level(document.scale() * 72.0 / widget_dpi(&self.obj().clone()));
            }
        }

        fn model(&self) -> Option<DocumentModel> {
            self.model.borrow().clone()
        }

        fn set_model(&self, model: DocumentModel) {
            let obj = self.obj();

            model.connect_document_notify(glib::clone!(@weak obj => move |model| {
                if let Some(document) = model.document() {
                    obj.set_sensitive(document.n_pages() > 0);
                    obj.imp().update_zoom_level();
                } else {
                    obj.set_sensitive(false);
                }
            }));

            model.connect_scale_notify(glib::clone!(@weak obj => move |_| {
                obj.imp().update_zoom_level();
            }));

            model.connect_scale_notify(glib::clone!(@weak obj => move |model| {
                obj.imp().populate_free_zoom_section(model.max_scale());
            }));

            self.model.replace(Some(model));
        }

        fn setup_initial_entry_size(&self) {
            let width = ZOOM_LEVELS.last().unwrap().name.chars().count();
            self.set_width_chars(width as i32);
        }

        fn set_width_chars(&self, width: i32) {
            // width + 2 (one decimals and the comma)
            self.entry.set_width_chars(width + 2);
        }

        fn set_zoom_level(&self, zoom: f64) {
            for z in ZOOM_LEVELS {
                if (zoom - z.level).abs() < EPSILON {
                    self.entry.set_text(z.name);
                    return;
                }
            }

            let zoom_perc = zoom * 100.0;
            let zoom_text = if (zoom_perc.round() - zoom_perc).abs() < 0.01 {
                format!("{}%", zoom_perc as i32)
            } else {
                format!("{zoom_perc:.1}%")
            };

            self.entry.set_text(&zoom_text);
        }

        fn populate_free_zoom_section(&self, max_scale: f64) {
            let mut width = 0;

            self.free_section.remove_all();

            for z in ZOOM_LEVELS.iter().filter(|z| z.level <= max_scale) {
                width = width.max(z.name.chars().count());

                let item = gio::MenuItem::new(Some(z.name), None);
                item.set_action_and_target_value(Some("win.zoom"), Some(&z.level.to_variant()));
                self.free_section.append_item(&item);
            }

            self.set_width_chars(width as i32);
        }
    }

    struct ZoomLevel {
        name: &'static str,
        level: f64,
    }

    const EPSILON: f64 = 0.000001;

    const ZOOM_LEVELS: &[ZoomLevel] = &[
        ZoomLevel {
            name: "50%",
            level: 0.5,
        },
        ZoomLevel {
            name: "70%",
            level: std::f64::consts::FRAC_1_SQRT_2,
        },
        ZoomLevel {
            name: "85%",
            level: 0.8408964152,
        },
        ZoomLevel {
            name: "100%",
            level: 1.0,
        },
        ZoomLevel {
            name: "125%",
            level: 1.1892071149,
        },
        ZoomLevel {
            name: "150%",
            level: std::f64::consts::SQRT_2,
        },
        ZoomLevel {
            name: "175%",
            level: 1.6817928304,
        },
        ZoomLevel {
            name: "200%",
            level: 2.0,
        },
        ZoomLevel {
            name: "300%",
            level: 2.8284271247,
        },
        ZoomLevel {
            name: "400%",
            level: 4.0,
        },
        ZoomLevel {
            name: "800%",
            level: 8.0,
        },
        ZoomLevel {
            name: "1600%",
            level: 16.0,
        },
        ZoomLevel {
            name: "3200%",
            level: 32.0,
        },
        ZoomLevel {
            name: "6400%",
            level: 64.0,
        },
    ];
}

glib::wrapper! {
    pub struct PpsZoomAction(ObjectSubclass<imp::PpsZoomAction>)
        @extends gtk::Box, gtk::Widget;
}

impl PpsZoomAction {
    pub fn new() -> Self {
        glib::Object::builder().build()
    }
}

impl Default for PpsZoomAction {
    fn default() -> Self {
        Self::new()
    }
}
