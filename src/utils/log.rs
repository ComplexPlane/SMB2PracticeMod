// Wrap OSReport() because Rust's formatting functions have a large code size footprint (especially
// for floats). It would be cool if there was some safe macro wrapper around printf-style
// formatting.
// TODO handle converting string arguments to C strings
#[macro_export]
macro_rules! log {
    ($fmt:literal $(, $arg:expr)*) => {{
        unsafe {
            mkb::OSReport(c"[pracmod] ".as_ptr() as *mut _);
            // I'm not sure it's possible to expand arg in a safe context
            #[allow(clippy::macro_metavars_in_unsafe)]
            mkb::OSReport($fmt.as_ptr() as *mut _ $(, $arg)*);
            mkb::OSReport(c"\n".as_ptr() as *mut _);
        }
    }};
}
