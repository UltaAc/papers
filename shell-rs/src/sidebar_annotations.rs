use crate::deps::*;
use crate::sidebar_annotations_row::PpsSidebarAnnotationsRow;
use papers_document::{AnnotationMarkup, DocumentAnnotations, Mapping};
use papers_view::{JobAnnots, JobPriority};

use gtk::graphene;

mod imp {
    use super::*;

    #[derive(CompositeTemplate, Debug, Default)]
    #[template(resource = "/org/gnome/papers/ui/sidebar-annotations.ui")]
    pub struct PpsSidebarAnnotations {
        #[template_child]
        list_view: TemplateChild<gtk::ListView>,
        #[template_child]
        stack: TemplateChild<adw::ViewStack>,
        #[template_child]
        popup: TemplateChild<gtk::PopoverMenu>,
        #[template_child]
        model: TemplateChild<gio::ListStore>,

        job: RefCell<Option<JobAnnots>>,
        job_handler: RefCell<Option<SignalHandlerId>>,
    }

    #[glib::object_subclass]
    impl ObjectSubclass for PpsSidebarAnnotations {
        const NAME: &'static str = "PpsSidebarAnnotations";
        type Type = super::PpsSidebarAnnotations;
        type ParentType = PpsSidebarPage;

        fn class_init(klass: &mut Self::Class) {
            klass.bind_template();
            klass.bind_template_callbacks();
        }

        fn instance_init(obj: &InitializingObject<Self>) {
            obj.init_template();
        }
    }

    impl ObjectImpl for PpsSidebarAnnotations {
        fn signals() -> &'static [Signal] {
            static SIGNALS: OnceLock<Vec<Signal>> = OnceLock::new();
            SIGNALS.get_or_init(|| {
                vec![
                    Signal::builder("annot-activated")
                        .run_last()
                        .action()
                        .param_types([Mapping::static_type()])
                        .build(),
                    Signal::builder("annot-removed").run_last().action().build(),
                    Signal::builder("annot-added").run_last().action().build(),
                ]
            })
        }

        fn constructed(&self) {
            if let Some(model) = self.obj().document_model() {
                model.connect_document_notify(glib::clone!(
                    #[weak(rename_to = obj)]
                    self,
                    move |model| {
                        if model
                            .document()
                            .and_dynamic_cast::<DocumentAnnotations>()
                            .is_ok()
                        {
                            obj.load();
                        }
                    }
                ));
            }

            self.obj().connect_closure(
                "annot-added",
                true,
                glib::closure_local!(move |obj: glib::Object| {
                    if let Ok(obj) = obj.downcast::<super::PpsSidebarAnnotations>() {
                        obj.imp().load();
                    }
                }),
            );

            self.obj().connect_closure(
                "annot-removed",
                true,
                glib::closure_local!(move |obj: glib::Object| {
                    if let Ok(obj) = obj.downcast::<super::PpsSidebarAnnotations>() {
                        obj.imp().load();
                    }
                }),
            );

            self.obj().connect_closure(
                "annot-activated",
                true,
                glib::closure_local!(move |obj: super::PpsSidebarAnnotations, _: Mapping| {
                    obj.navigate_to_view();
                }),
            );
        }

        fn dispose(&self) {
            self.clear_job();
        }
    }

    impl WidgetImpl for PpsSidebarAnnotations {}

    impl BinImpl for PpsSidebarAnnotations {}

    impl PpsSidebarPageImpl for PpsSidebarAnnotations {
        fn support_document(&self, document: &Document) -> bool {
            document.is::<DocumentAnnotations>()
        }
    }

    #[gtk::template_callbacks]
    impl PpsSidebarAnnotations {
        #[template_callback]
        fn list_view_factory_setup(&self, item: &gtk::ListItem, _factory: &gtk::ListItemFactory) {
            let row = PpsSidebarAnnotationsRow::new();

            let gesture = gtk::GestureClick::builder().button(0).build();

            gesture.connect_pressed(glib::clone!(
                #[weak(rename_to = obj)]
                self,
                #[weak]
                item,
                move |gesture, _, x, y| {
                    let annot = item.item().and_downcast::<AnnotationMarkup>().unwrap();
                    let mut mapping = Mapping::new();

                    mapping.set_data(annot.clone());
                    mapping.set_area(annot.area());

                    match gesture.current_button() {
                        gdk::BUTTON_PRIMARY => {
                            obj.obj().emit_by_name::<()>("annot-activated", &[&mapping])
                        }
                        gdk::BUTTON_SECONDARY => {
                            let document_view = obj
                                .obj()
                                .ancestor(papers_shell::DocumentView::static_type())
                                .and_downcast::<papers_shell::DocumentView>()
                                .unwrap();
                            let row = item.child().unwrap();

                            document_view.handle_annot_popup(&annot);

                            let point = row
                                .compute_point(
                                    &obj.popup.parent().unwrap(),
                                    &graphene::Point::new(x as f32, y as f32),
                                )
                                .unwrap();

                            obj.popup.set_pointing_to(Some(&gdk::Rectangle::new(
                                point.x() as i32,
                                point.y() as i32,
                                1,
                                1,
                            )));
                            obj.popup.popup();
                        }
                        _ => (),
                    }
                }
            ));

            row.add_controller(gesture);
            item.set_child(Some(&row));
        }

        #[template_callback]
        fn list_view_factory_bind(&self, item: &gtk::ListItem, _factory: &gtk::ListItemFactory) {
            let row = item
                .child()
                .and_downcast::<PpsSidebarAnnotationsRow>()
                .unwrap();
            let document = self
                .obj()
                .document_model()
                .and_then(|m| m.document())
                .unwrap();
            let annot = item.item().and_downcast::<AnnotationMarkup>().unwrap();

            row.set_document(document);
            row.set_annotation(annot);
        }
    }

    impl PpsSidebarAnnotations {
        fn clear_job(&self) {
            if let Some(job) = self.job.take() {
                if let Some(id) = self.job_handler.take() {
                    job.disconnect(id);
                }
            }
        }

        fn load(&self) {
            self.clear_job();

            let Some(document) = self.obj().document_model().and_then(|m| m.document()) else {
                return;
            };

            let job = JobAnnots::new(&document);

            let id = job.connect_finished(glib::clone!(
                #[weak(rename_to = obj)]
                self,
                move |job| {
                    let mapping_lists = job.annots();

                    if mapping_lists.is_empty() {
                        obj.stack.set_visible_child_name("empty");
                    } else {
                        obj.model.remove_all();

                        for mappings in mapping_lists {
                            for mapping in mappings.list() {
                                if let Some(annot) =
                                    mapping.data().and_downcast::<AnnotationMarkup>()
                                {
                                    obj.model.append(&annot);
                                }
                            }
                        }
                        obj.stack.set_visible_child_name("annot");
                    }

                    obj.clear_job();
                }
            ));

            self.job.replace(Some(job.clone()));
            self.job_handler.replace(Some(id));

            // The priority doesn't matter for this job
            job.scheduler_push_job(JobPriority::PriorityNone);
        }
    }
}

glib::wrapper! {
    pub struct PpsSidebarAnnotations(ObjectSubclass<imp::PpsSidebarAnnotations>)
    @extends gtk::Widget, adw::Bin, PpsSidebarPage;
}

impl Default for PpsSidebarAnnotations {
    fn default() -> Self {
        Self::new()
    }
}

impl PpsSidebarAnnotations {
    pub fn new() -> Self {
        glib::Object::builder().build()
    }
}
