#[macro_export]
macro_rules! hook {
    ($name:ident $(, $argname:ident: $arg:ty)* => $ret:ty, $their_func:expr, $our_func:expr) => {
        pub struct $name {
            instrs: [core::cell::Cell<usize>; 2],
            chained_func: core::cell::Cell<Option<extern "C" fn($($arg,)*) -> $ret>>,
            their_func: unsafe extern "C" fn($($arg,)*) -> $ret,
            our_func_c: extern "C" fn($($arg,)*) -> $ret,
        }

        impl Default for $name {
            fn default() -> Self {
                Self::new()
            }
        }

        impl $name {
            const fn new() -> Self {
                Self {
                    instrs: [core::cell::Cell::new(0), core::cell::Cell::new(0)],
                    chained_func: core::cell::Cell::new(None),
                    their_func: $their_func,
                    our_func_c: Self::c_hook,
                }
            }

            pub fn hook(&self) {
                unsafe {
                    let mut chained_func_addr = core::ptr::null();
                    $crate::patch::hook_function(
                        self.their_func as *mut usize,
                        self.our_func_c as *mut usize,
                        &self.instrs,
                        &mut chained_func_addr,
                    );
                    self.chained_func.set(Some(core::mem::transmute(chained_func_addr)));
                }
            }

            // We sometimes want to replace the hooked function entirely and never call the original
            #[allow(dead_code)]
            pub fn call(&self $(, $argname: $arg)*) -> $ret {
                (self.chained_func.get().unwrap())($($argname,)*)
            }

            extern "C" fn c_hook($($argname: $arg, )*) -> $ret {
                let f: fn($($arg,)*) -> $ret = $our_func;
                f($($argname, )*)
            }
        }
    }
}
