use core::borrow::BorrowMut;
use core::ffi::c_void;
use core::mem::transmute;
use core::ptr::null;

use crate::app::{self, AppContext};
use crate::patch;
use crate::{mkb, relutil};

pub struct Tramp {
    instrs: [u32; 2],
    dest: Option<extern "C" fn()>,
}

impl Tramp {
    pub const fn new() -> Self {
        Self {
            instrs: [0, 0],
            dest: None,
        }
    }

    pub unsafe fn hook(&mut self, func: unsafe extern "C" fn(), target: extern "C" fn()) {
        let mut dest = null();
        patch::hook_function(
            func as *mut u32,
            target as *mut u32,
            &mut self.instrs,
            &mut dest,
        );
        self.dest = Some(transmute(dest));
    }

    pub fn call(&self) {
        (self.dest.unwrap())();
    }
}

pub struct PadReadTramp {
    instrs: [u32; 2],
    dest: Option<extern "C" fn(*mut mkb::PADStatus)>,
}

impl PadReadTramp {
    pub const fn new() -> Self {
        Self {
            instrs: [0, 0],
            dest: None,
        }
    }

    pub unsafe fn hook(
        &mut self,
        func: unsafe extern "C" fn(statuses: *mut mkb::PADStatus),
        target: extern "C" fn(statuses: *mut mkb::PADStatus),
    ) {
        let mut dest = null();
        patch::hook_function(
            func as *mut u32,
            target as *mut u32,
            &mut self.instrs,
            &mut dest,
        );
        self.dest = Some(transmute(dest));
    }

    pub fn call(&self, statuses: *mut mkb::PADStatus) {
        (self.dest.unwrap())(statuses);
    }
}

extern "C" fn padread_c_hook(statuses: *mut mkb::PADStatus) {
    critical_section::with(|cs| {
        let mut cx = app::APP_CONTEXT.borrow(cs).borrow_mut();
        padread_hook(&mut cx, statuses);
    });
}

fn padread_hook(cx: &mut AppContext, statuses: *mut mkb::PADStatus) {
    cx.padread_tramp.call(statuses);
}

pub struct OSLinkTramp {
    instrs: [u32; 2],
    dest: Option<
        extern "C" fn(rel_buffer: *mut mkb::OSModuleHeader, bss_buffer: *mut c_void) -> bool,
    >,
}

impl OSLinkTramp {
    pub const fn new() -> Self {
        Self {
            instrs: [0, 0],
            dest: None,
        }
    }

    pub unsafe fn hook(
        &mut self,
        func: unsafe extern "C" fn(
            rel_buffer: *mut mkb::OSModuleHeader,
            bss_buffer: *mut c_void,
        ) -> bool,
        target: extern "C" fn(
            rel_buffer: *mut mkb::OSModuleHeader,
            bss_buffer: *mut c_void,
        ) -> bool,
    ) {
        let mut dest = null();
        patch::hook_function(
            func as *mut u32,
            target as *mut u32,
            &mut self.instrs,
            &mut dest,
        );
        self.dest = Some(transmute(dest));
    }

    pub fn call(&self, rel_buffer: *mut mkb::OSModuleHeader, bss_buffer: *mut c_void) -> bool {
        (self.dest.unwrap())(rel_buffer, bss_buffer)
    }
}

extern "C" fn oslink_c_hook(rel_buffer: *mut mkb::OSModuleHeader, bss_buffer: *mut c_void) -> bool {
    critical_section::with(|cs| {
        let mut cx = app::APP_CONTEXT.borrow(cs).borrow_mut();
        oslink_hook(rel_buffer, bss_buffer, &mut cx)
    })
}

fn oslink_hook(
    rel_buffer: *mut mkb::OSModuleHeader,
    bss_buffer: *mut c_void,
    cx: &mut AppContext,
) -> bool {
    let ret = cx.oslink_tramp.call(rel_buffer, bss_buffer);

    let module_id = unsafe { (*rel_buffer).info.id };
    let module_id = relutil::ModuleId::try_from(module_id).unwrap();

    if module_id == relutil::ModuleId::MainGame {
        unsafe {
            cx.game_ready_init_tramp
                .hook(mkb::smd_game_ready_init, game_ready_init_c_hook);
        }
    }

    ret
}

pub struct GameReadyInitTramp {
    instrs: [u32; 2],
    dest: Option<extern "C" fn()>,
}

impl GameReadyInitTramp {
    pub const fn new() -> Self {
        Self {
            instrs: [0, 0],
            dest: None,
        }
    }

    pub unsafe fn hook(&mut self, func: unsafe extern "C" fn(), target: extern "C" fn()) {
        let mut dest = null();
        patch::hook_function(
            func as *mut u32,
            target as *mut u32,
            &mut self.instrs,
            &mut dest,
        );
        self.dest = Some(transmute(dest));
    }

    pub fn call(&self) {
        (self.dest.unwrap())()
    }
}

extern "C" fn game_ready_init_c_hook() {
    critical_section::with(|cs| {
        let mut cx = app::APP_CONTEXT.borrow(cs).borrow_mut();
        game_ready_init_hook(&mut cx)
    })
}

fn game_ready_init_hook(cx: &mut AppContext) {
    // TODO stuff here
    cx.game_ready_init_tramp.call();
}
