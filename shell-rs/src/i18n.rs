pub fn gettext_f(format: &str, args: impl IntoIterator<Item = impl AsRef<str>>) -> String {
    let mut s = gettextrs::gettext(format);

    for arg in args {
        s = s.replacen("{}", arg.as_ref(), 1);
    }

    s
}
