use core::mem::transmute;
use core::ptr::null;
use paste::paste;

use crate::app::{self, AppContext};
use crate::mkb;
use crate::patch;

macro_rules! tramp {
    ($name:ident $(, $argname:ident: $arg:ty)* => $ret:ty, $their_func:expr, $our_func:expr) => {
        pub struct $name {
            instrs: [u32; 2],
            chained_func: Option<extern "C" fn($($arg,)*) -> $ret>,
            their_func: unsafe extern "C" fn($($arg,)*) -> $ret,
            our_func_c: extern "C" fn($($arg,)*) -> $ret,
        }

        paste! {
            impl $name {
                pub const fn new() -> Self {
                    Self {
                        instrs: [0, 0],
                        chained_func: None,
                        their_func: $their_func,
                        our_func_c: Self::[<$name:snake _c_hook>],
                    }
                }

                pub fn hook(&mut self) {
                    unsafe {
                        let mut chained_func_addr = null();
                        patch::hook_function(
                            self.their_func as *mut u32,
                            self.our_func_c as *mut u32,
                            &mut self.instrs,
                            &mut chained_func_addr,
                        );
                        self.chained_func = Some(transmute(chained_func_addr));
                    }
                }

                pub fn call(&self $(, $argname: $arg)*) -> $ret {
                    (self.chained_func.unwrap())($($argname,)*)
                }

                extern "C" fn [<$name:snake _c_hook>]($($argname: $arg, )*) -> $ret {
                    critical_section::with(|cs| {
                        let mut cx = app::APP_CONTEXT.borrow(cs).borrow_mut();
                        $our_func($($argname, )* &mut cx)
                    })
                }
            }
        }
    }
}

tramp!(PADReadTramp, status: *mut mkb::PADStatus => u32, mkb::PADRead, |statuses, cx: &mut AppContext| {
    cx.padread_tramp.call(statuses)
});
