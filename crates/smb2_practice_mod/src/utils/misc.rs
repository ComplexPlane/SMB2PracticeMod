use critical_section::Mutex;

// Iterate a static mutable array (in mkb) without creating a reference
pub fn for_c_arr<const N: usize, T, F>(ptr: *mut [T; N], mut f: F)
where
    F: FnMut(*mut T),
{
    for i in 0..N {
        f(unsafe { &raw mut (*ptr)[i] });
    }
}

pub fn for_c_arr_idx<const N: usize, T, F>(ptr: *mut [T; N], mut f: F)
where
    F: FnMut(usize, *mut T),
{
    for i in 0..N {
        f(i, unsafe { &raw mut (*ptr)[i] });
    }
}

pub fn with_mutex<T, R>(m: &Mutex<T>, f: impl FnOnce(&T) -> R) -> R {
    critical_section::with(|cs| f(m.borrow(cs)))
}
