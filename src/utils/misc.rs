// Iterate a static mutable array (in mkb) without creating a reference
pub fn for_c_arr<const N: usize, T, F>(ptr: *mut [T; N], mut f: F)
where
    F: FnMut(usize, *mut T),
{
    unsafe {
        for i in 0..N {
            f(i, &raw mut (*ptr)[i]);
        }
    }
}
