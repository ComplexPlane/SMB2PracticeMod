#[macro_export]
macro_rules! hook {
    ($name:ident $(, $argname:ident: $arg:ty)* => $ret:ty, $their_func:expr, $our_func:expr) => {
        #[derive(Clone)]
        pub struct $name {
            instrs: [core::cell::Cell<usize>; 2],
            jumpback_addr: core::cell::Cell<usize>,
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
                    jumpback_addr: core::cell::Cell::new(0),
                }
            }

            pub fn hook(&self) {
                let their_func = $their_func;
                unsafe {
                    let hook_info = $crate::patch::hook_function_part1(
                        their_func as *mut usize,
                        Self::c_hook as *const usize,
                    );
                    self.instrs[0].set(hook_info.first_instr);
                    self.jumpback_addr.set(hook_info.jumpback_addr as usize);
                }
            }

            // We sometimes want to replace the hooked function entirely and never call the original
            #[allow(dead_code)]
            pub fn call(self $(, $argname: $arg)*) -> $ret {
                // We take self as a compile-time reminder that hooks must be cloned and moved out
                // of Mutex scope before being called
                unsafe {
                    $crate::patch::hook_function_part2(
                        &self.instrs,
                        self.jumpback_addr.get() as *const usize,
                    );
                    let chained_func =
                        core::mem::transmute::<
                            *const core::ffi::c_void,
                            extern "C" fn($($arg,)*) -> $ret
                        >(self.instrs.as_ptr() as *const core::ffi::c_void);
                    chained_func($($argname,)*)
                }
            }

            extern "C" fn c_hook($($argname: $arg, )*) -> $ret {
                let f: fn($($arg,)*) -> $ret = $our_func;
                f($($argname, )*)
            }
        }
    }
}
