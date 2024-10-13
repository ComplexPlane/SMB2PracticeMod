extern crate alloc;

use core::cell::RefCell;
use core::ffi::c_void;
use critical_section::Mutex;

use crate::hook;
use crate::mkb;
use crate::relutil;

hook!(PADReadHook, status: *mut mkb::PADStatus => u32, mkb::PADRead, |statuses, cx: &mut AppContext| {
    let ret = cx.padread_hook.call(statuses);

    // // Dpad can modify effective stick input, shown by input display
    // dpad::on_PADRead(statuses);
    // // pad collects original inputs before they are modified by the game
    // pad::on_PADRead(statuses);

    ret
});

hook!(ProcessInputsHook => (), mkb::process_inputs, |cx: &mut AppContext| {
    cx.process_inputs_hook.call();
});

hook!(DrawDebugTextHook => (), mkb::draw_debugtext, |cx: &mut AppContext| {
    cx.draw_debug_text_hook.call();
});

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, |cx: &mut AppContext| {

});

hook!(GamePlayTickHook => (), mkb::smd_game_play_tick, |cx: &mut AppContext| {

});

hook!(OSLinkHook,
        rel_buffer: *mut mkb::OSModuleHeader,
        bss_buffer: *mut c_void => u8,
        mkb::OSLink,
        |rel_buffer, bss_buffer, cx: &mut AppContext| {

    let ret = cx.oslink_hook.call(rel_buffer, bss_buffer);

    let module_id = relutil::ModuleId::try_from(unsafe{*rel_buffer}.info.id);
    if let Ok(relutil::ModuleId::MainGame) = module_id {
        cx.game_ready_init_hook.hook();
        cx.game_play_tick_hook.hook();
    }

    ret
});

pub struct AppContext {
    pub padread_hook: PADReadHook,
    pub process_inputs_hook: ProcessInputsHook,
    pub draw_debug_text_hook: DrawDebugTextHook,
    pub oslink_hook: OSLinkHook,
    pub game_ready_init_hook: GameReadyInitHook,
    pub game_play_tick_hook: GamePlayTickHook,
}

pub static APP_CONTEXT: Mutex<RefCell<AppContext>> = Mutex::new(RefCell::new(AppContext {
    padread_hook: PADReadHook::new(),
    process_inputs_hook: ProcessInputsHook::new(),
    draw_debug_text_hook: DrawDebugTextHook::new(),
    oslink_hook: OSLinkHook::new(),
    game_ready_init_hook: GameReadyInitHook::new(),
    game_play_tick_hook: GamePlayTickHook::new(),
}));
