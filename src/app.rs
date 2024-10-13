extern crate alloc;

use core::cell::RefCell;
use core::ffi::c_void;
use critical_section::Mutex;

use crate::draw::Draw;
use crate::hook;
use crate::mkb;
use crate::relutil;
use crate::scratch::Scratch;

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

    // // These run after all controller inputs have been processed on the current frame,
    // // to ensure lowest input delay
    // pad::tick();
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
    with_mut(&APP_CONTEXT.draw, |draw| {
        with_mut(&APP_CONTEXT.scratch, |cx| cx.tick(draw));
    });
    // // Pref runs last to track the prefs from the previous frame
    // pref::tick();
});

hook!(DrawDebugTextHook => (), mkb::draw_debugtext, || {
    with_mut(&APP_CONTEXT.draw_debug_text_hook, |cx| cx.call());

    // // When the game is paused, screenshot the game's draw buffer before we draw our custom UI
    //         // elements. The original screenshot call is nopped.
    //         if (mkb::g_pause_status == 1) {
    //             mkb::take_pausemenu_screenshot(&mkb::fullscreen_texture_buf, 0, 0,
    //                                            mkb::current_render_mode->fbWidth,
    //                                            mkb::current_render_mode->efbHeight, mkb::GX_TF_RGB5A3);
    //         }

    with_mut(&APP_CONTEXT.draw, |cx| cx.predraw());
    //         timer::disp();
    //         iw::disp();
    //         Tetris::get_instance().disp();
    //         ilbattle::disp();
    //         cmseg::disp();
    //         inputdisp::disp();
    //         menu_impl::disp();
    with_mut(&APP_CONTEXT.draw, |cx| cx.disp());
    //         ilmark::disp();
    //         physics::disp();
    with_mut(&APP_CONTEXT.draw, |draw| {
        with_mut(&APP_CONTEXT.scratch, |cx| cx.disp(draw));
    })
});

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, || {
    with_mut(&APP_CONTEXT.game_ready_init_hook, |cx| {
        // stage_edits::smd_game_ready_init();
        // ballcolor::switch_monkey();
        cx.call();
    })
});

hook!(GamePlayTickHook => (), mkb::smd_game_play_tick, || {
    with_mut(&APP_CONTEXT.game_play_tick_hook, |cx| {
        cx.call();
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
            // jump::patch_minimap();
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

    pub draw: Mutex<RefCell<Draw>>,
    pub scratch: Mutex<RefCell<Scratch>>,
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

            draw: Mutex::new(RefCell::new(Draw::new())),
            scratch: Mutex::new(RefCell::new(Scratch::new())),
        }
    }

    pub fn init(&self) {
        // heap::init();
        // cardio::init();
        // pref::init();
        // unlock::init();
        with_mut(&self.draw, |cx| cx.init());
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

        with_mut(&self.padread_hook, |cx| cx.hook());
        with_mut(&self.process_inputs_hook, |cx| cx.hook());
        with_mut(&self.draw_debug_text_hook, |cx| cx.hook());
        with_mut(&self.oslink_hook, |cx| cx.hook());
    }
}

pub static APP_CONTEXT: AppContext = AppContext::new();
