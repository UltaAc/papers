use crate::deps::*;
use gtk::accessible::{Property, Relation, State};
use std::collections::HashMap;

mod imp {
    use super::*;

    #[derive(Debug, Default, Properties)]
    #[properties(wrapper_type = super::PpsStackSwitcher)]
    pub struct PpsStackSwitcher {
        #[property(get, nullable, set = Self::set_stack)]
        stack: RefCell<Option<gtk::Stack>>,
        pages: RefCell<Option<gtk::SelectionModel>>,
        buttons: RefCell<HashMap<gtk::StackPage, gtk::ToggleButton>>,
        sig_handlers: RefCell<Vec<glib::SignalHandlerId>>,
    }

    impl PpsStackSwitcher {
        fn pages(&self) -> Option<gtk::SelectionModel> {
            self.pages.borrow().clone()
        }

        fn clear_switcher(&self) {
            for (_, button) in self.buttons.borrow_mut().drain() {
                button.unparent();
            }
        }

        fn populate_switcher(&self) {
            if let Some(pages) = self.pages() {
                for i in 0..pages.n_items() {
                    self.add_child(i);
                }
            }
        }

        fn disconnect_stack_signals(&self) {
            if let Some(pages) = self.pages() {
                for sig in self.sig_handlers.take() {
                    pages.disconnect(sig);
                }
            }
        }

        fn connect_stack_signals(&self) {
            let Some(pages) = self.pages() else {
                return;
            };

            let items_sig = pages.connect_items_changed(glib::clone!(
                #[weak(rename_to = obj)]
                self,
                move |_, _, _, _| {
                    obj.clear_switcher();
                    obj.populate_switcher();
                }
            ));

            let selection_sig = pages.connect_selection_changed(glib::clone!(
                #[weak(rename_to = obj)]
                self,
                move |pages, position, n_items| {
                    for i in position..position + n_items {
                        if let Some(button) = pages
                            .item(i)
                            .and_then(|page| obj.buttons.borrow().get(&page).cloned())
                        {
                            let selected = pages.is_selected(i);

                            button.set_active(selected);
                            button.update_state(&[State::Selected(Some(selected))]);
                        }
                    }
                }
            ));

            let mut sigs = self.sig_handlers.borrow_mut();
            sigs.push(items_sig);
            sigs.push(selection_sig);
        }

        fn add_child(&self, position: u32) {
            let button = gtk::ToggleButton::builder()
                .accessible_role(gtk::AccessibleRole::Tab)
                .hexpand(true)
                .vexpand(true)
                .focus_on_click(false)
                .build();

            if let Some(box_) = self.obj().child().and_downcast::<gtk::Box>() {
                box_.append(&button);
            }

            let Some(pages) = self.pages() else {
                return;
            };

            let Some(page) = pages.item(position).and_downcast::<gtk::StackPage>() else {
                return;
            };

            let selected = pages.is_selected(position);

            button.update_state(&[State::Selected(Some(selected))]);

            let accessible = page.upcast_ref::<gtk::Accessible>();
            button.update_relation(&[Relation::Controls(&[accessible])]);

            let title = page.title().unwrap_or_default();
            let visible = page.is_visible();

            for (source, target) in [
                ("icon-name", "icon-name"),
                ("title", "tooltip-text"),
                ("visible", "visible"),
            ] {
                page.bind_property(source, &button, target)
                    .sync_create()
                    .build();
            }

            button.update_property(&[Property::Label(title.as_str())]);

            button.set_active(selected);
            button.set_visible(visible);

            let mut buttons = self.buttons.borrow_mut();
            buttons.insert(page.clone(), button.clone());

            button.connect_active_notify(move |button| {
                let active = button.is_active();
                let mut selected = active;

                if active {
                    pages.select_item(position, true);
                } else {
                    selected = pages.is_selected(position);
                    button.set_active(selected);
                }

                button.update_state(&[State::Selected(Some(selected))]);
            });
        }

        fn set_stack(&self, stack: gtk::Stack) {
            if let Some(sw_stack) = self.stack.borrow().as_ref() {
                if sw_stack == &stack {
                    return;
                } else {
                    self.disconnect_stack_signals();
                    self.clear_switcher();
                }
            }

            // We need to store a reference to stack.pages() since the internal
            // of this function simply add a weak reference to it and return it
            // to the caller. This object will be destroyed immediately without
            // holding a reference.
            let pages = stack.pages();
            self.stack.replace(Some(stack));
            self.pages.replace(Some(pages));

            self.populate_switcher();
            self.connect_stack_signals();
        }
    }

    #[glib::object_subclass]
    impl ObjectSubclass for PpsStackSwitcher {
        const NAME: &'static str = "PpsStackSwitcher";
        type Type = super::PpsStackSwitcher;
        type ParentType = adw::Bin;

        fn class_init(klass: &mut Self::Class) {
            klass.set_accessible_role(gtk::AccessibleRole::TabList);
        }
    }

    #[glib::derived_properties]
    impl ObjectImpl for PpsStackSwitcher {
        fn constructed(&self) {
            let box_ = gtk::Box::builder()
                .orientation(gtk::Orientation::Horizontal)
                .spacing(0)
                .css_classes(["toolbar"])
                .build();

            self.obj().set_child(Some(&box_));
        }
    }

    impl WidgetImpl for PpsStackSwitcher {}

    impl BinImpl for PpsStackSwitcher {}
}

glib::wrapper! {
    pub struct PpsStackSwitcher(ObjectSubclass<imp::PpsStackSwitcher>)
    @extends gtk::Widget, adw::Bin;
}

impl Default for PpsStackSwitcher {
    fn default() -> Self {
        Self::new()
    }
}

impl PpsStackSwitcher {
    fn new() -> PpsStackSwitcher {
        glib::object::Object::new()
    }
}
