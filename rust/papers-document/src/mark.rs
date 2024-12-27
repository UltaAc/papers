use crate::Mark;
use crate::Point;
use std::fmt;

impl Mark {
    #[inline]
    pub fn page_index(&self) -> i32 {
        self.inner.page_index
    }

    #[inline]
    pub fn doc_point(&self) -> Point {
        Point {
            inner: self.inner.doc_point,
        }
    }
}

impl fmt::Debug for Mark {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        f.debug_struct("Mark")
            .field("page_index", &self.page_index())
            .field("doc_point", &self.doc_point())
            .finish()
    }
}
