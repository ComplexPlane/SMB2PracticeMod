use core::ffi::c_char;

// TODO get this from bindgen
extern "C" {
    pub fn OSReport(format: *const c_char, ...) -> i32;
}

// Wrap OSReport() because Rust's formatting functions have a large code size footprint (especially
// for floats). It would be cool if there was some safe macro wrapper around printf-style
// formatting.
#[macro_export]
macro_rules! log {
    ($fmt:expr $(, $($arg:tt)*)?) => {{
        let mut c_fmt = ArrayString::<512>::from("[pracmod] ").unwrap();
        c_fmt.push_str($fmt);
        c_fmt.push_str("\n\0");

        // I'm not sure it's possible to expand arg in a safe context
        #[allow(clippy::macro_metavars_in_unsafe)]
        unsafe {
            log::OSReport(c_fmt.as_ptr() as *const c_char $(, $($arg)*)?);
        }
    }};
}
