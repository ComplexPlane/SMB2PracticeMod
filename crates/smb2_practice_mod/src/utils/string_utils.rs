#[macro_export]
macro_rules! fmt_buf {
    ($buf:expr, $fmt:literal $(, $arg:expr)* $(,)?) => {
        unsafe {
            let fmt: &core::ffi::CStr = $fmt;
            let ptr = $buf.as_mut_ptr();
            let written_bytes = ::mkb::mkb::sprintf(ptr as *mut _, fmt.as_ptr() as *mut _ $(, $arg)*);
            assert!(written_bytes >= 0);
            // Hopefully this panics before whatever we overwrote causes problems
            assert!((written_bytes as usize) < $buf.capacity());
            $buf.set_len(written_bytes as usize);
        }
    }
}

#[macro_export]
macro_rules! fmt {
    ($n:expr, $fmt:literal $(, $arg:expr)* $(,)?) => {{
        let mut buf = arrayvec::ArrayString::<$n>::new();
        $crate::fmt_buf!(buf, $fmt $(, $arg)*);
        buf
    }};
}

#[macro_export]
macro_rules! cstr_buf {
    ($arraystring:expr) => {{
        let mut buf = $arraystring;
        buf.push('\0');
        buf.as_mut_ptr() as *mut i8
    }};
}

#[macro_export]
macro_rules! cstr {
    ($n:expr, $s:expr) => {{
        let s: &str = $s;
        let buf = arrayvec::ArrayString::<$n>::from(s).unwrap();
        $crate::cstr_buf!(buf)
    }};
}
