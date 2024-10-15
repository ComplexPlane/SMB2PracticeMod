#[macro_export]
macro_rules! sprintf {
    ($buf:expr, $fmt:literal $(, $arg:expr)* $(,)?) => {
        unsafe {
            let ptr = $buf.as_mut_ptr();
            let written_bytes = $crate::mkb::sprintf(ptr as *mut _, $fmt.as_ptr() as *mut _ $(, $arg)*);
            assert!(written_bytes >= 0);
            // Hopefully this panics before whatever we overwrote causes problems
            assert!((written_bytes as usize) < $buf.capacity());
            $buf.set_len(written_bytes as usize);
        }
    }
}
