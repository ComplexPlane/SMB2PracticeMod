use core::cell::RefCell;

use crate::{
    hook, mkb,
    mods::{
        ballcolor::BallColor, banans::Banans, camera::Camera, cmseg::CmSeg, dpad::Dpad,
        fallout::Fallout, freecam::Freecam, gotostory::GoToStory, hide::Hide, ilbattle::IlBattle,
        ilmark::IlMark, inputdisp::InputDisplay, iw::Iw, jump::Jump, marathon::Marathon,
        physics::Physics, savestates_ui::SaveStatesUi, scratch::Scratch, sfx::Sfx,
        stage_edits::StageEdits, timer::Timer, unlock::Unlock, validate::Validate,
    },
    systems::{
        binds::Binds,
        draw::Draw,
        menu_impl::MenuImpl,
        pad::Pad,
        pref::{BoolPref, Pref},
    },
    utils::{libsavestate::LibSaveState, relutil::ModuleId},
};

pub static APP_CONTEXT: once_cell::sync::Lazy<critical_section::Mutex<AppContext>> =
    once_cell::sync::Lazy::new(|| critical_section::Mutex::new(AppContext::new()));

pub struct AppContext {
    pub process_inputs_hook: RefCell<ProcessInputsHook>,
    pub draw_debug_text_hook: RefCell<DrawDebugTextHook>,
    pub oslink_hook: RefCell<OSLinkHook>,
    pub game_ready_init_hook: RefCell<GameReadyInitHook>,
    pub game_play_tick_hook: RefCell<GamePlayTickHook>,

    pub pad: RefCell<Pad>,
    pub binds: RefCell<Binds>,
    pub menu_impl: RefCell<MenuImpl>,
    pub unlock: RefCell<Unlock>,
    pub iw: RefCell<Iw>,
    pub lib_save_state: RefCell<LibSaveState>,
    pub save_states_ui: RefCell<SaveStatesUi>,
    pub fallout: RefCell<Fallout>,
    pub jump: RefCell<Jump>,
    pub physics: RefCell<Physics>,
    pub input_display: RefCell<InputDisplay>,
    pub go_to_story: RefCell<GoToStory>,
    pub cm_seg: RefCell<CmSeg>,
    pub banans: RefCell<Banans>,
    pub marathon: RefCell<Marathon>,
    pub ball_color: RefCell<BallColor>,
    pub freecam: RefCell<Freecam>,
    pub timer: RefCell<Timer>,
    pub dpad: RefCell<Dpad>,
    pub il_battle: RefCell<IlBattle>,
    pub il_mark: RefCell<IlMark>,
    pub camera: RefCell<Camera>,
    pub stage_edits: RefCell<StageEdits>,
    pub hide: RefCell<Hide>,
    pub sfx: RefCell<Sfx>,
    pub scratch: RefCell<Scratch>,
    pub validate: RefCell<Validate>,
    pub draw: RefCell<Draw>,
    pub pref: RefCell<Pref>,
}

impl AppContext {
    fn new() -> Self {
        crate::systems::heap::HEAP.init();

        // TODO: derive default?
        Self {
            process_inputs_hook: RefCell::new(ProcessInputsHook::default()),
            draw_debug_text_hook: RefCell::new(DrawDebugTextHook::default()),
            oslink_hook: RefCell::new(OSLinkHook::default()),
            game_ready_init_hook: RefCell::new(GameReadyInitHook::default()),
            game_play_tick_hook: RefCell::new(GamePlayTickHook::default()),

            pad: RefCell::new(Pad::new()),
            binds: RefCell::new(Binds::new()),
            menu_impl: RefCell::new(MenuImpl::new()),
            unlock: RefCell::new(Unlock::new()),
            iw: RefCell::new(Iw::new()),
            lib_save_state: RefCell::new(LibSaveState::new()),
            save_states_ui: RefCell::new(SaveStatesUi::new()),
            fallout: RefCell::new(Fallout::new()),
            jump: RefCell::new(Jump::new()),
            physics: RefCell::new(Physics::new()),
            input_display: RefCell::new(InputDisplay::new()),
            go_to_story: RefCell::new(GoToStory::new()),
            cm_seg: RefCell::new(CmSeg::new()),
            banans: RefCell::new(Banans::new()),
            marathon: RefCell::new(Marathon::new()),
            ball_color: RefCell::new(BallColor::new()),
            freecam: RefCell::new(Freecam::new()),
            timer: RefCell::new(Timer::new()),
            dpad: RefCell::new(Dpad::new()),
            il_battle: RefCell::new(IlBattle::new()),
            il_mark: RefCell::new(IlMark::new()),
            camera: RefCell::new(Camera::new()),
            stage_edits: RefCell::new(StageEdits::new()),
            hide: RefCell::new(Hide::new()),
            sfx: RefCell::new(Sfx::new()),
            scratch: RefCell::new(Scratch::new()),
            validate: RefCell::new(Validate::new()),
            draw: RefCell::new(Draw::new()),
            pref: RefCell::new(Pref::new()),
        }
    }

    fn init(&self) {
        self.process_inputs_hook.borrow_mut().hook();
        self.draw_debug_text_hook.borrow_mut().hook();
        self.oslink_hook.borrow_mut().hook();

        self.pad.borrow_mut().on_main_loop_load(self);
        self.lib_save_state.borrow_mut().on_main_loop_load(self);
        self.save_states_ui.borrow_mut().on_main_loop_load(self);
        self.fallout.borrow_mut().on_main_loop_load(self);
        self.cm_seg.borrow_mut().on_main_loop_load(self);
        self.ball_color.borrow_mut().on_main_loop_load(self);
        self.freecam.borrow_mut().on_main_loop_load(self);
        self.dpad.borrow_mut().on_main_loop_load(self);
        self.stage_edits.borrow_mut().on_main_loop_load(self);
        self.hide.borrow_mut().on_main_loop_load(self);
        self.sfx.borrow_mut().on_main_loop_load(self);
        self.validate.borrow_mut().on_main_loop_load(self);
    }
}

// Tick functions hook
hook!(ProcessInputsHook => (), mkb::process_inputs, |cx| {
    cx.process_inputs_hook.borrow().call();

    cx.pad.borrow_mut().tick(cx);
    cx.binds.borrow_mut().tick(cx);
    cx.unlock.borrow_mut().tick(cx);
    cx.iw.borrow_mut().tick(cx);
    cx.save_states_ui.borrow_mut().tick(cx);
    cx.menu_impl.borrow_mut().tick(cx);
    cx.fallout.borrow_mut().tick(cx);
    cx.jump.borrow_mut().tick(cx);
    cx.physics.borrow_mut().tick(cx);
    cx.input_display.borrow_mut().tick(cx);
    cx.go_to_story.borrow_mut().tick(cx);
    cx.cm_seg.borrow_mut().tick(cx);
    cx.banans.borrow_mut().tick(cx);
    cx.marathon.borrow_mut().tick(cx);
    cx.ball_color.borrow_mut().tick(cx);
    cx.freecam.borrow_mut().tick(cx);
    cx.il_battle.borrow_mut().tick(cx);
    cx.il_mark.borrow_mut().tick(cx);
    cx.camera.borrow_mut().tick(cx);
    cx.stage_edits.borrow_mut().tick(cx);
    cx.validate.borrow_mut().tick(cx);
    cx.scratch.borrow_mut().tick(cx);
    cx.pref.borrow_mut().tick(cx);
});

// Draw functions hook
hook!(DrawDebugTextHook => (), mkb::draw_debugtext, |cx| {
    cx.draw_debug_text_hook.borrow().call();

    // When the game is paused, screenshot the game's draw buffer before we draw our custom UI
    // elements. The original screenshot call is nopped.
    unsafe {
        if (mkb::g_pause_status == 1) {
            mkb::take_pausemenu_screenshot(
                &raw mut mkb::fullscreen_texture_buf as *mut _,
                0,
                0,
                (*mkb::current_render_mode).fbWidth as i16,
                (*mkb::current_render_mode).efbHeight as i16,
                mkb::GX_TF_RGB5A3,
            );
        }
    };

    cx.draw.borrow().predraw();

    cx.timer.borrow_mut().draw(cx);
    cx.iw.borrow_mut().draw(cx);
    cx.il_battle.borrow_mut().draw(cx);
    cx.cm_seg.borrow_mut().draw(cx);
    cx.input_display.borrow_mut().draw(cx);
    cx.menu_impl.borrow_mut().draw(cx);
    cx.draw.borrow_mut().draw(cx);
    cx.il_mark.borrow_mut().draw(cx);
    cx.physics.borrow_mut().draw(cx);
    cx.scratch.borrow_mut().draw(cx);
});

// MainGameLoad calls
hook!(OSLinkHook,
        rel_buffer: *mut mkb::OSModuleHeader,
        bss_buffer: *mut core::ffi::c_void => u8,
        mkb::OSLink,
        |rel_buffer, bss_buffer, cx| {
    let ret = cx.oslink_hook.borrow().call(rel_buffer, bss_buffer);

    let module_id = ModuleId::try_from(unsafe{*rel_buffer}.info.id);
    if let Ok(ModuleId::MainGame) = module_id {
        cx.game_ready_init_hook.borrow_mut().hook();
        cx.game_play_tick_hook.borrow_mut().hook();
    }

    ret
});

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, |cx| {
    cx.stage_edits.borrow_mut().on_game_ready_init(cx);
    cx.ball_color.borrow_mut().switch_monkey(&mut cx.pref.borrow_mut());
    cx.game_ready_init_hook.borrow().call();
});

hook!(GamePlayTickHook => (), mkb::smd_game_play_tick, |cx| {
    cx.game_play_tick_hook.borrow().call();

    let validate = &mut cx.validate.borrow_mut();
    let pref = &mut cx.pref.borrow_mut();
    let lib_save_state = &mut cx.lib_save_state.borrow_mut();
    let menu_impl = &mut cx.menu_impl.borrow_mut();
    let physics = &mut cx.physics.borrow_mut();
    let pad = &mut cx.pad.borrow_mut();

    validate.validate_run(pref, lib_save_state, menu_impl, physics, pad);
    cx.il_mark.borrow_mut().validate_attempt(validate);
    cx.il_battle.borrow_mut().validate_attempt(validate);
});

pub fn init() {
    critical_section::with(|cs| {
        APP_CONTEXT.borrow(cs).init();
    });
}

pub fn tick() {
    unsafe {
        // Replace overwritten function call
        mkb::perf_init_timer(4);

        critical_section::with(|cs| {
            let cx = APP_CONTEXT.borrow(cs);
            let pref = cx.pref.borrow_mut();
            if pref.get_bool(BoolPref::DebugMode) {
                mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
            } else {
                mkb::dip_switches &= !(mkb::DIP_DEBUG | mkb::DIP_DISP);
            }

            cx.pad.borrow_mut().on_frame_start();
        });
    }
}
