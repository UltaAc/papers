use crate::{DocumentContainsJS, DocumentInfo, DocumentInfoFields, DocumentLicense};

use glib::translate::*;

macro_rules! string_field {
    ($field:ident, $bit:ident) => {
        pub fn $field(&self) -> Option<glib::GString> {
            if DocumentInfoFields::from_bits_truncate(self.inner.fields_mask)
                .intersects(DocumentInfoFields::$bit)
            {
                unsafe { from_glib_none(self.inner.$field) }
            } else {
                None
            }
        }
    };
}

impl DocumentInfo {
    string_field!(title, TITLE);
    string_field!(format, FORMAT);
    string_field!(author, AUTHOR);
    string_field!(subject, SUBJECT);
    string_field!(keywords, KEYWORDS);
    string_field!(creator, CREATOR);
    string_field!(producer, PRODUCER);
    string_field!(linearized, LINEARIZED);
    string_field!(security, SECURITY);

    pub fn pages(&self) -> Option<i32> {
        if DocumentInfoFields::from_bits_truncate(self.inner.fields_mask)
            .intersects(DocumentInfoFields::N_PAGES)
        {
            Some(self.inner.n_pages)
        } else {
            None
        }
    }

    pub fn contains_js(&self) -> Option<DocumentContainsJS> {
        if DocumentInfoFields::from_bits_truncate(self.inner.fields_mask)
            .intersects(DocumentInfoFields::N_PAGES)
        {
            Some(unsafe { from_glib(self.inner.contains_js) })
        } else {
            None
        }
    }

    pub fn license(&self) -> Option<DocumentLicense> {
        if DocumentInfoFields::from_bits_truncate(self.inner.fields_mask)
            .intersects(DocumentInfoFields::LICENSE)
        {
            Some(unsafe { from_glib_none(self.inner.license) })
        } else {
            None
        }
    }
}
