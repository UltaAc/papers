use crate::deps::*;

use papers_document::AnnotationMarkup;
use papers_document::AnnotationTextMarkup;
use papers_document::AnnotationTextMarkupType;
use papers_document::AnnotationType;

mod imp {
    use super::*;

    #[derive(Properties, Default, Debug)]
    #[properties(wrapper_type = super::PpsSidebarAnnotationsRow)]
    pub struct PpsSidebarAnnotationsRow {
        #[property(set = Self::set_annotation)]
        pub(super) annotation: RefCell<Option<AnnotationMarkup>>,
        label_notify_id: RefCell<Option<SignalHandlerId>>,
        modified_notify_id: RefCell<Option<SignalHandlerId>>,
        contents_notify_id: RefCell<Option<SignalHandlerId>>,
        image: gtk::Image,
    }

    #[glib::object_subclass]
    impl ObjectSubclass for PpsSidebarAnnotationsRow {
        const NAME: &'static str = "PpsSidebarAnnotationsRow";
        type Type = super::PpsSidebarAnnotationsRow;
        type ParentType = adw::ActionRow;
    }

    #[glib::derived_properties]
    impl ObjectImpl for PpsSidebarAnnotationsRow {
        fn constructed(&self) {
            self.obj().add_prefix(&self.image);
        }
    }

    impl WidgetImpl for PpsSidebarAnnotationsRow {}

    impl ListBoxRowImpl for PpsSidebarAnnotationsRow {}

    impl PreferencesRowImpl for PpsSidebarAnnotationsRow {}

    impl ActionRowImpl for PpsSidebarAnnotationsRow {}

    impl PpsSidebarAnnotationsRow {
        fn set_row_tooltip(&self, annot: Option<&AnnotationMarkup>) {
            let tooltip = annot.filter(|annot| annot.label().is_some()).map(|annot| {
                let label = annot.label().unwrap();
                annot.modified().map_or(
                    format!("<span weight=\"bold\">{}</span>", label),
                    |modified| format!("<span weight=\"bold\">{}</span>\n{}", label, modified),
                )
            });
            self.obj().set_tooltip_markup(tooltip.as_deref());
        }

        fn set_row_title(&self, annot: Option<&AnnotationMarkup>) {
            let default = format!("<i>{}</i>", gettext("No Comment"));
            let markup = annot.map_or(default.clone(), |annot| {
                annot
                    .contents()
                    .filter(|s| !s.is_empty())
                    .map(|content| glib::markup_escape_text(&content).trim().to_string())
                    .unwrap_or_else(|| default.clone())
            });

            self.obj().set_title(&markup);
        }

        fn annot(&self) -> Option<AnnotationMarkup> {
            self.annotation.borrow().clone()
        }

        fn set_annotation(&self, annot: Option<&AnnotationMarkup>) {
            if self.annot().as_ref() == annot {
                return;
            }

            // clear the old
            if let Some(annot) = self.annotation.take() {
                if let Some(id) = self.label_notify_id.take() {
                    annot.disconnect(id);
                }

                if let Some(id) = self.modified_notify_id.take() {
                    annot.disconnect(id);
                }

                if let Some(id) = self.contents_notify_id.take() {
                    annot.disconnect(id);
                }

                // If there is too many handlers, simply use an Vec<SignalHandlerId> to hold them
                // For example, search_box.rs - set_context
            }

            // setup the new one
            self.set_row_tooltip(annot);
            self.set_row_title(annot);

            let label_notify_id = annot.map(|annot| {
                annot.connect_label_notify(glib::clone!(@weak self as obj => move |annot| {
                    obj.set_row_tooltip(Some(annot));
                }))
            });

            let modified_notify_id = annot.map(|annot| {
                annot.connect_modified_notify(glib::clone!(@weak self as obj => move |annot| {
                    obj.set_row_tooltip(Some(annot));
                }))
            });

            let contents_notify_id = annot.map(|annot| {
                annot.connect_contents_notify(glib::clone!(@weak self as obj => move |annot| {
                    obj.set_row_title(Some(annot));
                }))
            });

            let icon_name = annot.map(|annot| match annot.annotation_type() {
                AnnotationType::Attachment => "mail-attachment-symbolic",
                AnnotationType::TextMarkup => match annot
                    .dynamic_cast_ref::<AnnotationTextMarkup>()
                    .unwrap()
                    .markup_type()
                {
                    AnnotationTextMarkupType::StrikeOut => "format-text-strikethrough-symbolic",
                    AnnotationTextMarkupType::Underline => "format-text-underline-symbolic",
                    AnnotationTextMarkupType::Squiggly => "annotations-squiggly-symbolic",
                    AnnotationTextMarkupType::Highlight => "format-justify-left-symbolic",
                    _ => unimplemented!(),
                },
                AnnotationType::Text => "annotations-text-symbolic",
                _ => unimplemented!(),
            });
            self.image.set_from_icon_name(icon_name);

            self.label_notify_id.replace(label_notify_id);
            self.modified_notify_id.replace(modified_notify_id);
            self.contents_notify_id.replace(contents_notify_id);

            self.annotation.replace(annot.cloned());
        }
    }
}

glib::wrapper! {
    pub struct PpsSidebarAnnotationsRow(ObjectSubclass<imp::PpsSidebarAnnotationsRow>)
        @extends adw::ActionRow, adw::PreferencesRow, gtk::ListBoxRow, gtk::Widget;
}
