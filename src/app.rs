extern crate alloc;

use core::cell::RefCell;
use core::ffi::c_void;
use critical_section::Mutex;

use crate::hook;
use crate::mkb;
use crate::relutil;

fn with_mut<T, F, R>(v: &Mutex<RefCell<T>>, f: F) -> R
where
    F: FnOnce(&mut T) -> R,
{
    critical_section::with(|cs| {
        let mut borrow = v.borrow(cs).borrow_mut();
        f(&mut borrow)
    })
}

hook!(PADReadHook, status: *mut mkb::PADStatus => u32, mkb::PADRead, |statuses| {
    with_mut(&APP_CONTEXT.padread_hook, |cx| {
        let ret = cx.call(statuses);

        // // Dpad can modify effective stick input, shown by input display
        // dpad::on_PADRead(statuses);
        // // pad collects original inputs before they are modified by the game
        // pad::on_PADRead(statuses);

        ret
    })
});

hook!(ProcessInputsHook => (), mkb::process_inputs, || {
    with_mut(&APP_CONTEXT.process_inputs_hook, |cx| {
        cx.call();
    });
});

hook!(DrawDebugTextHook => (), mkb::draw_debugtext, || {
    with_mut(&APP_CONTEXT.draw_debug_text_hook, |cx| {
        cx.call();
    })
});

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, || {
    with_mut(&APP_CONTEXT.game_ready_init_hook, |cx| {
        cx.call();
    })
});

hook!(GamePlayTickHook => (), mkb::smd_game_play_tick, || {
    with_mut(&APP_CONTEXT.game_play_tick_hook, |cx| {
        cx.call();
    })
});

hook!(OSLinkHook,
        rel_buffer: *mut mkb::OSModuleHeader,
        bss_buffer: *mut c_void => u8,
        mkb::OSLink,
        |rel_buffer, bss_buffer| {

    with_mut(&APP_CONTEXT.oslink_hook, |cx| {
        let ret = cx.call(rel_buffer, bss_buffer);

        let module_id = relutil::ModuleId::try_from(unsafe{*rel_buffer}.info.id);
        if let Ok(relutil::ModuleId::MainGame) = module_id {
            with_mut(&APP_CONTEXT.game_ready_init_hook, |cx| {
                cx.hook();
            });
            with_mut(&APP_CONTEXT.game_play_tick_hook, |cx| {
                cx.hook();
            });
        }

        ret
    })
});

pub struct AppContext {
    pub padread_hook: Mutex<RefCell<PADReadHook>>,
    pub process_inputs_hook: Mutex<RefCell<ProcessInputsHook>>,
    pub draw_debug_text_hook: Mutex<RefCell<DrawDebugTextHook>>,
    pub oslink_hook: Mutex<RefCell<OSLinkHook>>,
    pub game_ready_init_hook: Mutex<RefCell<GameReadyInitHook>>,
    pub game_play_tick_hook: Mutex<RefCell<GamePlayTickHook>>,
}

impl AppContext {
    const fn new() -> Self {
        Self {
            padread_hook: Mutex::new(RefCell::new(PADReadHook::new())),
            process_inputs_hook: Mutex::new(RefCell::new(ProcessInputsHook::new())),
            draw_debug_text_hook: Mutex::new(RefCell::new(DrawDebugTextHook::new())),
            oslink_hook: Mutex::new(RefCell::new(OSLinkHook::new())),
            game_ready_init_hook: Mutex::new(RefCell::new(GameReadyInitHook::new())),
            game_play_tick_hook: Mutex::new(RefCell::new(GamePlayTickHook::new())),
        }
    }

    pub fn init(&self) {
        with_mut(&self.padread_hook, |cx| cx.hook());
        with_mut(&self.process_inputs_hook, |cx| cx.hook());
        with_mut(&self.draw_debug_text_hook, |cx| cx.hook());
        with_mut(&self.oslink_hook, |cx| cx.hook());
    }
}

pub static APP_CONTEXT: AppContext = AppContext::new();
