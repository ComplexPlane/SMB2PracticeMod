extern crate alloc;

use core::cell::RefCell;
use core::ffi::c_void;
use critical_section::Mutex;
use once_cell::sync::Lazy;

use crate::hook;
use crate::mkb;
use crate::mods::ballcolor::BallColor;
use crate::mods::freecam::Freecam;
use crate::mods::inputdisp::InputDisplay;
use crate::mods::savestates_ui;
use crate::mods::savestates_ui::SaveStatesUi;
use crate::mods::scratch::Scratch;
use crate::mods::unlock::Unlock;
use crate::systems::binds::Binds;
use crate::systems::draw::Draw;
use crate::systems::heap;
use crate::systems::menu_impl::MenuImpl;
use crate::systems::pad::Pad;
use crate::systems::pref::BoolPref;
use crate::systems::pref::Pref;
use crate::utils::libsavestate::LibSaveState;
use crate::utils::modlink::ModLink;
use crate::utils::relutil;

pub fn with_app<F, R>(f: F) -> R
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

        let pad = &mut cx.pad.borrow_mut();
        let draw = &mut cx.draw.borrow_mut();
        let binds = &mut cx.binds.borrow_mut();
        let libsavestates = &mut cx.libsavestate.borrow_mut();

        // These run after all controller inputs have been processed on the current frame,
        // to ensure lowest input delay
        pad.tick();
        binds.tick(pad);
        // cardio::tick();
        cx.unlock.borrow_mut().tick();
        // iw::tick();
        cx.savestates_ui.borrow_mut().tick(pad, &cx.pref, draw, binds, libsavestates);
        // savest_ui::tick();
        // anything checking for pref changes should run after menu_impl.tick()

        let pref = &mut cx.pref.borrow_mut();

        cx.menu_impl.borrow_mut().tick(pad, pref, draw, binds);
        // fallout::tick();
        // jump::tick();     // (edits physics preset)
        // physics::tick();  // anything editing physics presets must run before physics::tick()
        cx.inputdisplay.borrow_mut().tick(pref);
        // gotostory::tick();
        // cmseg::tick();
        // banans::tick();
        // marathon::tick();
        cx.ballcolor.borrow_mut().tick(pref);
        cx.freecam.borrow_mut().tick(pref, pad, draw);
        // ilbattle::tick();
        // ilmark::tick();
        // camera::tick();
        // stage_edits::tick();
        // validate::tick();
        cx.scratch.borrow_mut().tick(draw);
        // // Pref runs last to track the prefs from the previous frame
        pref.tick(draw);
    });
});

hook!(DrawDebugTextHook => (), mkb::draw_debugtext, || {
    with_app(|cx| {
        cx.draw_debug_text_hook.borrow().call();

        let pad = &mut cx.pad.borrow_mut();
        let pref = &mut cx.pref.borrow_mut();
        let draw = &mut cx.draw.borrow_mut();
        let binds = &mut cx.binds.borrow_mut();

        // // When the game is paused, screenshot the game's draw buffer before we draw our custom UI
        //         // elements. The original screenshot call is nopped.
        //         if (mkb::g_pause_status == 1) {
        //             mkb::take_pausemenu_screenshot(&mkb::fullscreen_texture_buf, 0, 0,
        //                                            mkb::current_render_mode->fbWidth,
        //                                            mkb::current_render_mode->efbHeight, mkb::GX_TF_RGB5A3);
        //         }

        draw.predraw();
        //         timer::disp();
        //         iw::disp();
        //         Tetris::get_instance().disp();
        //         ilbattle::disp();
        //         cmseg::disp();
        cx.inputdisplay.borrow_mut().disp(pad, pref, &mut cx.freecam.borrow_mut(), &mut cx.ballcolor.borrow_mut());
        cx.menu_impl.borrow_mut().disp(pad, pref, draw, binds);
        draw.disp();
        //         ilmark::disp();
        //         physics::disp();
        cx.scratch.borrow_mut().disp(draw);
    })
});

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, || {
    with_app(|cx| {
        // stage_edits::smd_game_ready_init();
        cx.ballcolor.borrow_mut().switch_monkey(&mut cx.pref.borrow_mut());
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

hook!(EventCameraTickHook => (), mkb::event_camera_tick, || {
    with_app(|cx| {
        cx.freecam.borrow_mut().on_event_camera_tick(&mut cx.pref.borrow_mut());

        cx.event_camera_tick_hook.borrow().call();
    })
});

hook!(SetMinimapModeHook, mode: mkb::MinimapMode => (), mkb::set_minimap_mode, |mode| {
    with_app(|cx| {
        let pref = &mut cx.pref.borrow_mut();
        unsafe {
            if !savestates_ui::savestates_enabled(pref)
                || !(mkb::main_mode == mkb::MD_GAME
                && mkb::main_game_mode == mkb::PRACTICE_MODE &&
                mode == mkb::MINIMAP_SHRINK)
            {
                cx.set_minimap_mode_hook.borrow().call(mode);
            }
        }
    });
});

hook!(SoundReqIdHook, sfx_idx: u32 => (), mkb::call_SoundReqID_arg_0, |sfx_idx| {
    with_app(|cx| {
        let libsavestate = &cx.libsavestate.borrow();
        if !libsavestate.state_loaded_this_frame {
            cx.sound_req_id_hook.borrow().call(sfx_idx);
        }
    });
});

hook!(CreateSpeedSpritesHook, x: f32, y: f32 => (), mkb::create_speed_sprites, |x, y| {
    with_app(|cx| {
        cx.create_speed_sprites_hook.borrow().call(x + 5.0, y);
    })
});

hook!(LoadStagedefHook, stage_id: u32 => (), mkb::load_stagedef, |stage_id| {
    with_app(|cx| {
        cx.load_stagedef_hook.borrow().call(stage_id);
        cx.ballcolor.borrow_mut().switch_monkey(&mut cx.pref.borrow_mut());
    });
});

pub struct AppContext {
    pub padread_hook: RefCell<PADReadHook>,
    pub process_inputs_hook: RefCell<ProcessInputsHook>,
    pub draw_debug_text_hook: RefCell<DrawDebugTextHook>,
    pub oslink_hook: RefCell<OSLinkHook>,
    pub game_ready_init_hook: RefCell<GameReadyInitHook>,
    pub game_play_tick_hook: RefCell<GamePlayTickHook>,
    pub event_camera_tick_hook: RefCell<EventCameraTickHook>,
    pub set_minimap_mode_hook: RefCell<SetMinimapModeHook>,
    pub sound_req_id_hook: RefCell<SoundReqIdHook>,
    pub create_speed_sprites_hook: RefCell<CreateSpeedSpritesHook>,
    pub load_stagedef_hook: RefCell<LoadStagedefHook>,

    pub draw: RefCell<Draw>,
    pub pad: RefCell<Pad>,
    pub freecam: RefCell<Freecam>,
    pub menu_impl: RefCell<MenuImpl>,
    pub binds: RefCell<Binds>,
    pub unlock: RefCell<Unlock>,
    pub pref: RefCell<Pref>,
    pub libsavestate: RefCell<LibSaveState>,
    pub savestates_ui: RefCell<SaveStatesUi>,
    pub ballcolor: RefCell<BallColor>,
    pub inputdisplay: RefCell<InputDisplay>,
    pub scratch: RefCell<Scratch>,
}

impl AppContext {
    fn new() -> Self {
        let modlink = ModLink::new();
        heap::HEAP.init(&modlink);
        let pref = Pref::new(&modlink);
        Self {
            padread_hook: RefCell::new(PADReadHook::new()),
            process_inputs_hook: RefCell::new(ProcessInputsHook::new()),
            draw_debug_text_hook: RefCell::new(DrawDebugTextHook::new()),
            oslink_hook: RefCell::new(OSLinkHook::new()),
            game_ready_init_hook: RefCell::new(GameReadyInitHook::new()),
            game_play_tick_hook: RefCell::new(GamePlayTickHook::new()),
            event_camera_tick_hook: RefCell::new(EventCameraTickHook::new()),
            set_minimap_mode_hook: RefCell::new(SetMinimapModeHook::new()),
            sound_req_id_hook: RefCell::new(SoundReqIdHook::new()),
            create_speed_sprites_hook: RefCell::new(CreateSpeedSpritesHook::new()),
            load_stagedef_hook: RefCell::new(LoadStagedefHook::new()),

            draw: RefCell::new(Draw::new()),
            pad: RefCell::new(Pad::new()),
            freecam: RefCell::new(Freecam::new()),
            menu_impl: RefCell::new(MenuImpl::new()),
            binds: RefCell::new(Binds::new()),
            unlock: RefCell::new(Unlock::new(&pref)),
            pref: RefCell::new(pref),
            libsavestate: RefCell::new(LibSaveState::new()),
            savestates_ui: RefCell::new(SaveStatesUi::new()),
            ballcolor: RefCell::new(BallColor::new()),
            inputdisplay: RefCell::new(InputDisplay::new()),
            scratch: RefCell::new(Scratch::new()),
        }
    }

    // For now, must be called after non-movable stuff (hooks) have been permanently allocated
    pub fn init(&self) {
        with_app(|cx| {
            // TODO see if we can move these into constructor functions
            // Tetris::get_instance().init();
            // physics::init();
            // iw::init();
            // timer::init();
            // inputdisp::init();
            // cmseg::init();
            // ballcolor::init();
            // sfx::init();
            // menu_defn::init();
            // hide::init();
            // ilmark::init();
            // camera::init();
            // fallout::init();
            // stage_edits::init();
            // validate::init();

            cx.padread_hook.borrow_mut().hook();
            cx.process_inputs_hook.borrow_mut().hook();
            cx.draw_debug_text_hook.borrow_mut().hook();
            cx.oslink_hook.borrow_mut().hook();
            cx.event_camera_tick_hook.borrow_mut().hook();
            cx.set_minimap_mode_hook.borrow_mut().hook();
            cx.sound_req_id_hook.borrow_mut().hook();
            cx.create_speed_sprites_hook.borrow_mut().hook();
            cx.load_stagedef_hook.borrow_mut().hook();
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

        with_app(|cx| {
            let pref = cx.pref.borrow_mut();
            if pref.get_bool(BoolPref::DebugMode) {
                mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
            } else {
                mkb::dip_switches &= !(mkb::DIP_DEBUG | mkb::DIP_DISP);
            }

            cx.pad.borrow_mut().on_frame_start();
        })
    }
}
