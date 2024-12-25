use crate::deps::*;
use papers_document::DocumentContainsJS;

mod imp {
    use super::*;

    #[derive(Properties, Default, Debug)]
    #[properties(wrapper_type = super::PpsPropertiesGeneral)]
    pub struct PpsPropertiesGeneral {
        #[property(set = Self::set_document, nullable)]
        pub(super) document: RefCell<Option<Document>>,
    }

    #[glib::object_subclass]
    impl ObjectSubclass for PpsPropertiesGeneral {
        const NAME: &'static str = "PpsPropertiesGeneral";
        type Type = super::PpsPropertiesGeneral;
        type ParentType = adw::Bin;
    }

    #[glib::derived_properties]
    impl ObjectImpl for PpsPropertiesGeneral {}

    impl BinImpl for PpsPropertiesGeneral {}

    impl WidgetImpl for PpsPropertiesGeneral {}

    impl PpsPropertiesGeneral {
        fn set_document(&self, document: Option<Document>) {
            if document == self.document.borrow().clone() {
                return;
            }

            self.obj().set_child(None::<&gtk::Widget>);

            if let Some(ref document) = document {
                let uri = document.uri().unwrap_or_default();
                let uri = glib::uri_unescape_string(uri, None::<&str>).unwrap_or_default();

                if let Some(info) = document.info() {
                    self.refresh_properties(uri.as_str(), document.size(), &info);
                }
            }

            self.document.replace(document);
        }

        fn refresh_properties(&self, uri: &str, size: u64, info: &DocumentInfo) {
            macro_rules! insert_string_field {
                ($group:ident, $title:expr, $field:ident) => {
                    if let Some(value) = info.$field() {
                        self.add_list_box_item(&$group, &$title, Some(value.to_string().as_str()));
                    }
                };
            }

            macro_rules! insert_property_time {
                ($group:ident, $title:expr, $field:ident) => {
                    if let Some(time) = info.$field() {
                        self.add_list_box_item(
                            &$group,
                            &$title,
                            Document::misc_format_datetime(&time).as_deref(),
                        );
                    }
                };
            }

            let page = adw::PreferencesPage::new();

            // File group
            let group = adw::PreferencesGroup::builder()
                .title(gettext("File"))
                .build();

            let file = gio::File::for_uri(uri);

            if let Some(parent) = file.parent() {
                let text = parent
                    .basename()
                    .map(|p| p.display().to_string())
                    .unwrap_or(gettext("None"));

                let row = adw::ActionRow::builder()
                    .title(gettext("Folder"))
                    .css_classes(["property"])
                    .build();

                let button = gtk::Button::builder()
                    .tooltip_text("Open File Location")
                    .icon_name("folder-open-symbolic")
                    .valign(gtk::Align::Center)
                    .css_classes(["flat"])
                    .build();

                let uri = uri.to_string();

                button.connect_clicked(glib::clone!(
                    #[weak(rename_to = obj)]
                    self,
                    move |_| {
                        let native = obj.obj().native();
                        let window = native.and_dynamic_cast_ref::<gtk::Window>();
                        let uri = uri.clone();

                        // FIXME: It's broken on MacOS due to lack of support in GTK4
                        gtk::FileLauncher::new(Some(&file)).open_containing_folder(
                            window,
                            gio::Cancellable::NONE,
                            move |result| {
                                if let Err(e) = result {
                                    glib::g_warning!(
                                        "",
                                        "Could not show containing folder for \"{}\": {}",
                                        uri,
                                        e.message()
                                    );
                                }
                            },
                        );
                    }
                ));

                row.set_subtitle(&text);
                row.add_suffix(&button);
                group.add(&row);
            }

            if size != 0 {
                self.add_list_box_item(
                    &group,
                    &gettext("Size"),
                    Some(glib::format_size(size).as_str()),
                );
            }
            page.add(&group);

            // Content group
            let group = adw::PreferencesGroup::builder()
                .title(gettext("Content"))
                .build();
            insert_string_field!(group, gettext("Title"), title);
            insert_string_field!(group, gettext("Subject"), subject);
            insert_string_field!(group, gettext("Author"), author);
            insert_string_field!(group, gettext("Keywords"), keywords);
            insert_string_field!(group, gettext("Producer"), producer);
            insert_string_field!(group, gettext("Creator"), creator);
            page.add(&group);

            // Date and time group
            let group = adw::PreferencesGroup::builder()
                .title(gettext("Date &amp; Time"))
                .build();
            insert_property_time!(group, gettext("Created"), created_datetime);
            insert_property_time!(group, gettext("Modified"), modified_datetime);
            page.add(&group);

            // Format group
            let group = adw::PreferencesGroup::builder()
                .title(gettext("Format"))
                .build();
            insert_string_field!(group, gettext("Format"), format);
            insert_string_field!(group, gettext("Number of Pages"), pages);
            insert_string_field!(group, gettext("Optimized"), linearized);
            insert_string_field!(group, gettext("Security"), security);
            insert_string_field!(group, gettext("Paper Size"), regular_paper_size);

            if let Some(contain_js) = info.contains_js() {
                let text = match contain_js {
                    DocumentContainsJS::Yes => gettext("Yes"),
                    DocumentContainsJS::No => gettext("No"),
                    _ => gettext("Unknown"),
                };
                self.add_list_box_item(&group, &gettext("Contains Javascript"), Some(&text));
            }
            page.add(&group);

            self.obj().set_child(Some(&page));
        }

        fn add_list_box_item(
            &self,
            group: &impl IsA<adw::PreferencesGroup>,
            label: &str,
            text: Option<&str>,
        ) {
            let row = adw::ActionRow::builder()
                .title(gettext(label))
                .use_markup(text.is_none())
                .css_classes(["property"])
                .subtitle_selectable(true)
                .build();

            // translators: This is used when a document property does not have
            // a value.  Examples:
            // Author: None
            // Keywords: None
            let text = match text {
                Some("") | None => gettext("None"),
                Some(text) => text.to_owned(),
            };

            row.set_subtitle(&text);
            group.add(&row);
        }
    }
}

glib::wrapper! {
    pub struct PpsPropertiesGeneral(ObjectSubclass<imp::PpsPropertiesGeneral>)
        @extends adw::Bin, gtk::Widget;
}

impl Default for PpsPropertiesGeneral {
    fn default() -> Self {
        Self::new()
    }
}

impl PpsPropertiesGeneral {
    pub fn new() -> Self {
        glib::Object::builder().build()
    }
}
