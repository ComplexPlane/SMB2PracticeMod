use core::ffi::c_char;

// TODO get this from bindgen
extern "C" {
    pub fn g_very_similar_to_puts(s: *const c_char);
}

#[macro_export]
macro_rules! log {
    ($($arg:tt)*) => {{
        let mut str = ArrayString::<512>::from("[pracmod] ").unwrap();
        write!(&mut str, $($arg)*).unwrap();
        str.push('\0');
        unsafe {
            log::g_very_similar_to_puts(str.as_ptr() as *const c_char);
        }
    }};
}
