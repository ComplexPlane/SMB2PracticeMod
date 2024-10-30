use core::cell::RefCell;

use critical_section::Mutex;
use once_cell::sync::Lazy;

#[macro_export]
macro_rules! hook {
    ($name:ident $(, $argname:ident: $arg:ty)* => $ret:ty, $their_func:expr, $our_func:expr) => {
        pub struct $name {
            instrs: [usize; 2],
            chained_func: Option<extern "C" fn($($arg,)*) -> $ret>,
            their_func: unsafe extern "C" fn($($arg,)*) -> $ret,
            our_func_c: extern "C" fn($($arg,)*) -> $ret,
        }

        impl Default for $name {
            fn default() -> Self {
                Self {
                    instrs: [0, 0],
                    chained_func: None,
                    their_func: $their_func,
                    our_func_c: Self::c_hook,
                }
            }
        }

        impl $name {
            pub fn new() -> Self {
                Self::default()
            }

            pub fn hook(&mut self) {
                unsafe {
                    let mut chained_func_addr = core::ptr::null();
                    $crate::patch::hook_function(
                        self.their_func as *mut usize,
                        self.our_func_c as *mut usize,
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
                let f: fn($($arg,)* &$crate::app_defn::AppContext) -> $ret = $our_func;
                critical_section::with(|cs| {
                    f($($argname, )* $crate::app_defn::APP_CONTEXT.borrow(cs))
                })
            }
        }
    }
}
