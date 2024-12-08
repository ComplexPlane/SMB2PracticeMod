// Wrap OSReport() because Rust's formatting functions have a large code size footprint (especially
// for floats). It would be cool if there was some safe macro wrapper around printf-style
// formatting.
#[macro_export]
macro_rules! log {
    ($fmt:literal $(, $arg:expr)* $(,)?) => {{
        // I'm not sure it's possible to expand arg in a safe context
        #[allow(clippy::macro_metavars_in_unsafe)]
        unsafe {
            mkb::mkb::OSReport(c"[pracmod] ".as_ptr() as *mut _);
            let fmt: &core::ffi::CStr = $fmt;
            mkb::mkb::OSReport(fmt.as_ptr() as *mut _ $(, $arg)*);
            mkb::mkb::OSReport(c"\n".as_ptr() as *mut _);
        }
    }};
}
