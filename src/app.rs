extern crate alloc;

use core::cell::RefCell;
use core::ffi::c_void;
use critical_section::Mutex;
use once_cell::sync::Lazy;

use crate::draw::Draw;
use crate::hook;
use crate::mkb;
use crate::pad::Pad;
use crate::relutil;
use crate::scratch::Scratch;

fn with_app<F, R>(f: F) -> R
where
    F: FnOnce(&AppContext) -> R,
{
    critical_section::with(|cs| f(APP_CONTEXT.borrow(cs)))
}

hook!(PADReadHook, status: *mut mkb::PADStatus => u32, mkb::PADRead, |statuses| {
    with_app(|cx| {
        let ret = cx.padread_hook.borrow().call(statuses);

        // // Dpad can modify effective stick input, shown by input display
        // dpad::on_PADRead(statuses);
        // pad collects original inputs before they are modified by the game
        let status_array = unsafe { core::slice::from_raw_parts(statuses, 4)};
        let status_array: &[mkb::PADStatus; 4] = status_array.try_into().unwrap();
        cx.pad.borrow_mut().on_padread(status_array);

        ret
    })
});

hook!(ProcessInputsHook => (), mkb::process_inputs, || {
    with_app(|cx| {
        cx.process_inputs_hook.borrow().call();

    // // These run after all controller inputs have been processed on the current frame,
    // // to ensure lowest input delay
    cx.pad.borrow_mut().tick();
    // binds::tick();
    // cardio::tick();
    // unlock::tick();
    // iw::tick();
    // savest_ui::tick();
    // menu_impl::tick();  // anything checking for pref changes should run after menu_impl::tick()
    // fallout::tick();
    // jump::tick();     // (edits physics preset)
    // physics::tick();  // anything editing physics presets must run before physics::tick()
    // inputdisp::tick();
    // gotostory::tick();
    // cmseg::tick();
    // banans::tick();
    // marathon::tick();
    // ballcolor::tick();
    // freecam::tick();
    // ilbattle::tick();
    // ilmark::tick();
    // camera::tick();
    // stage_edits::tick();
    // validate::tick();
        cx.scratch.borrow_mut().tick(&mut cx.draw.borrow_mut());
    // // Pref runs last to track the prefs from the previous frame
    // pref::tick();
    });
});

hook!(DrawDebugTextHook => (), mkb::draw_debugtext, || {
    with_app(|cx| {
        cx.draw_debug_text_hook.borrow().call();

        // // When the game is paused, screenshot the game's draw buffer before we draw our custom UI
        //         // elements. The original screenshot call is nopped.
        //         if (mkb::g_pause_status == 1) {
        //             mkb::take_pausemenu_screenshot(&mkb::fullscreen_texture_buf, 0, 0,
        //                                            mkb::current_render_mode->fbWidth,
        //                                            mkb::current_render_mode->efbHeight, mkb::GX_TF_RGB5A3);
        //         }

        cx.draw.borrow_mut().predraw();
        //         timer::disp();
        //         iw::disp();
        //         Tetris::get_instance().disp();
        //         ilbattle::disp();
        //         cmseg::disp();
        //         inputdisp::disp();
        //         menu_impl::disp();
        cx.draw.borrow_mut().disp();
        //         ilmark::disp();
        //         physics::disp();
        let mut draw = cx.draw.borrow_mut();
        cx.scratch.borrow_mut().disp(&mut draw);
    })
});

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, || {
    with_app(|cx| {
        // stage_edits::smd_game_ready_init();
        // ballcolor::switch_monkey();
        cx.game_ready_init_hook.borrow().call();
    })
});

hook!(GamePlayTickHook => (), mkb::smd_game_play_tick, || {
    with_app(|cx| {
        cx.game_play_tick_hook.borrow().call();
        // validate::validate_run();
        // ilmark::validate_attempt();
        // ilbattle::validate_attempt();
    })
});

hook!(OSLinkHook,
        rel_buffer: *mut mkb::OSModuleHeader,
        bss_buffer: *mut c_void => u8,
        mkb::OSLink,
        |rel_buffer, bss_buffer| {

    with_app(|cx| {
        let ret = cx.oslink_hook.borrow().call(rel_buffer, bss_buffer);

        let module_id = relutil::ModuleId::try_from(unsafe{*rel_buffer}.info.id);
        if let Ok(relutil::ModuleId::MainGame) = module_id {
            cx.game_ready_init_hook.borrow_mut().hook();
            cx.game_play_tick_hook.borrow_mut().hook();
            // jump::patch_minimap();
        }

        ret
    })
});

pub struct AppContext {
    pub padread_hook: RefCell<PADReadHook>,
    pub process_inputs_hook: RefCell<ProcessInputsHook>,
    pub draw_debug_text_hook: RefCell<DrawDebugTextHook>,
    pub oslink_hook: RefCell<OSLinkHook>,
    pub game_ready_init_hook: RefCell<GameReadyInitHook>,
    pub game_play_tick_hook: RefCell<GamePlayTickHook>,

    pub draw: RefCell<Draw>,
    pub pad: RefCell<Pad>,
    pub scratch: RefCell<Scratch>,
}

impl AppContext {
    fn new() -> Self {
        Self {
            padread_hook: RefCell::new(PADReadHook::new()),
            process_inputs_hook: RefCell::new(ProcessInputsHook::new()),
            draw_debug_text_hook: RefCell::new(DrawDebugTextHook::new()),
            oslink_hook: RefCell::new(OSLinkHook::new()),
            game_ready_init_hook: RefCell::new(GameReadyInitHook::new()),
            game_play_tick_hook: RefCell::new(GamePlayTickHook::new()),

            draw: RefCell::new(Draw::new()),
            pad: RefCell::new(Pad::new()),
            scratch: RefCell::new(Scratch::new()),
        }
    }

    // For now, must be called after non-movable stuff (hooks) have been permanently allocated
    pub fn init(&self) {
        with_app(|cx| {
            // heap::init();
            // cardio::init();
            // pref::init();
            // unlock::init();
            cx.draw.borrow_mut().init();
            // Tetris::get_instance().init();
            // physics::init();
            // iw::init();
            // libsavest::init();
            // timer::init();
            // inputdisp::init();
            // cmseg::init();
            // ballcolor::init();
            // sfx::init();
            // menu_defn::init();
            // freecam::init();
            // hide::init();
            // ilmark::init();
            // camera::init();
            // fallout::init();
            // stage_edits::init();
            // scratch::init();
            // validate::init();

            cx.padread_hook.borrow_mut().hook();
            cx.process_inputs_hook.borrow_mut().hook();
            cx.draw_debug_text_hook.borrow_mut().hook();
            cx.oslink_hook.borrow_mut().hook();
        });
    }
}

static APP_CONTEXT: Lazy<Mutex<AppContext>> = Lazy::new(|| Mutex::new(AppContext::new()));

pub fn init() {
    with_app(|cx| cx.init());
}

pub fn tick() {
    unsafe {
        // Replace overwritten function call
        mkb::perf_init_timer(4);

        // if pref::get_bool(pref::BoolPref::DebugMode) {
        // mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
        // } else {
        mkb::dip_switches &= !(mkb::DIP_DEBUG | mkb::DIP_DISP);
        // }

        with_app(|cx| {
            cx.pad.borrow_mut().on_frame_start();
        })
    }
}
