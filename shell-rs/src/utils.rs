use crate::deps::*;

/**
 * ev_document_misc_get_widget_dpi:
 * @widget: a #GtkWidget
 *
 * Returns sensible guess for DPI of monitor on which given widget has been
 * realized. If HiDPI display, use 192, else 96.
 * Returns 96 as fallback value.
 *
 * Returns: DPI as gdouble
 */

pub fn widget_dpi(widget: &impl IsA<gtk::Widget>) -> f64 {
    let native = widget.native();
    let display = widget.display();

    if let Some(native) = native {
        if let Some(monitor) = display.monitor_at_surface(&native.surface()) {
            let geometry = monitor.geometry();
            let is_landscape = geometry.width() > geometry.height();

            // DPI is 192 if height ≥ 1080 and the orientation is not portrait,
            // which is, incidentally, how GTK detects HiDPI displays and set a
            // scaling factor for the logical output
            // https://gitlab.gnome.org/GNOME/gtk/-/issues/3115#note_904622 */
            if is_landscape && geometry.height() > 1080 {
                return 192.0;
            }
        }
    }

    // The only safe assumption you can make, on Unix-like/X11 and
    // Linux/Wayland, is to always set the DPI to 96, regardless of
    // physical/logical resolution, because that's the only safe
    // guarantee we can make.
    // https://gitlab.gnome.org/GNOME/gtk/-/issues/3115#note_904622 */
    96.0
}
