#[macro_export]
macro_rules! hook {
    ($name:ident $(, $argname:ident: $arg:ty)* => $ret:ty, $their_func:expr, $our_func:expr) => {
        pub struct $name {
            instrs: [core::cell::Cell<usize>; 2],
            // chained_func: core::cell::Cell<Option<extern "C" fn($($arg,)*) -> $ret>>,
            their_func: unsafe extern "C" fn($($arg,)*) -> $ret,
            // our_func_c: extern "C" fn($($arg,)*) -> $ret,
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
                    // chained_func: core::cell::Cell::new(None),
                    their_func: $their_func,
                    jumpback_addr: core::cell::Cell::new(0),
                    // our_func_c: Self::c_hook,
                }
            }

            pub fn hook(&self) {
                $crate::log!(c"Start of hook()");
                unsafe {
                    let hook_info = $crate::patch::hook_function_part1(
                        self.their_func as *mut usize,
                        Self::c_hook as *const usize,
                    );
                    self.instrs[0].set(hook_info.first_instr);
                    self.jumpback_addr.set(hook_info.jumpback_addr as usize);
                    // let chained_func = core::mem::transmute::<
                    //     *const core::ffi::c_void,
                    //     extern "C" fn($($arg,)*) -> $ret
                    // >(chained_func_addr);
                    // self.chained_func.set(Some(chained_func));
                }
                $crate::log!(c"End of hook()");
            }

            // We sometimes want to replace the hooked function entirely and never call the original
            #[allow(dead_code)]
            pub fn call(&self $(, $argname: $arg)*) -> $ret {
                $crate::log!(c"%s: Start of call()", $crate::cstr!(64, core::stringify!($name)));
                let ret = unsafe {
                    $crate::patch::hook_function_part2(
                        &self.instrs,
                        self.jumpback_addr.get() as *const usize,
                    );
                    $crate::log!(c"instrs: 0x%08X, 0x%08X", self.instrs[0].get(), self.instrs[1].get());
                    let chained_func =
                        core::mem::transmute::<
                            *const core::ffi::c_void,
                            extern "C" fn($($arg,)*) -> $ret
                        >(self.instrs.as_ptr() as *const core::ffi::c_void);
                    $crate::log!(c"chained func addr: 0x%08X", chained_func);
                    $crate::log!(c"%s: before chained func()", $crate::cstr!(64, core::stringify!($name)));
                    let ret = chained_func($($argname,)*);
                    $crate::log!(c"%s: after chained func()", $crate::cstr!(64, core::stringify!($name)));
                    ret
                };
                $crate::log!(c"End of call()");
                ret
            }

            extern "C" fn c_hook($($argname: $arg, )*) -> $ret {
                let f: fn($($arg,)*) -> $ret = $our_func;
                f($($argname, )*)
            }
        }
    }
}
