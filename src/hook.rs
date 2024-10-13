#[macro_export]
macro_rules! hook {
    ($name:ident $(, $argname:ident: $arg:ty)* => $ret:ty, $their_func:expr, $our_func:expr) => {
        pub struct $name {
            instrs: [u32; 2],
            chained_func: Option<extern "C" fn($($arg,)*) -> $ret>,
            their_func: unsafe extern "C" fn($($arg,)*) -> $ret,
            our_func_c: extern "C" fn($($arg,)*) -> $ret,
        }

        impl $name {
            pub const fn new() -> Self {
                Self {
                    instrs: [0, 0],
                    chained_func: None,
                    their_func: $their_func,
                    our_func_c: Self::c_hook,
                }
            }

            pub fn hook(&mut self) {
                unsafe {
                    let mut chained_func_addr = core::ptr::null();
                    $crate::patch::hook_function(
                        self.their_func as *mut u32,
                        self.our_func_c as *mut u32,
                        &mut self.instrs,
                        &mut chained_func_addr,
                    );
                    self.chained_func = Some(core::mem::transmute(chained_func_addr));
                }
            }

            pub fn call(&self $(, $argname: $arg)*) -> $ret {
                (self.chained_func.unwrap())($($argname,)*)
            }

            extern "C" fn c_hook($($argname: $arg, )*) -> $ret {
                $our_func($($argname, )*)
            }
        }
    }
}
