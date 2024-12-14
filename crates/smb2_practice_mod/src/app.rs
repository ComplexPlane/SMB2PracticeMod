use core::cell::RefCell;

use critical_section::Mutex;
use mkb::mkb;
use once_cell::sync::Lazy;

use crate::{
    hook,
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
        menu_defn::MenuContext,
        menu_impl::MenuImpl,
        pad::Pad,
        pref::{BoolPref, Pref},
    },
    utils::{libsavestate::LibSaveState, relutil::ModuleId},
};

static APP_CONTEXT: Lazy<Mutex<RefCell<AppContext>>> =
    Lazy::new(|| Mutex::new(RefCell::new(AppContext::new())));

pub fn with_app<T>(f: impl FnOnce(&mut AppContext) -> T) -> T {
    critical_section::with(|cs| f(&mut APP_CONTEXT.borrow_ref_mut(cs)))
}

#[derive(Default)]
pub struct AppContext {
    pub pad: Pad,
    pub binds: Binds,
    pub menu_impl: MenuImpl,
    pub unlock: Unlock,
    pub iw: Iw,
    pub lib_save_state: LibSaveState,
    pub save_states_ui: SaveStatesUi,
    pub fallout: Fallout,
    pub jump: Jump,
    pub physics: Physics,
    pub input_display: InputDisplay,
    pub go_to_story: GoToStory,
    pub cm_seg: CmSeg,
    pub banans: Banans,
    pub marathon: Marathon,
    pub ball_color: BallColor,
    pub freecam: Freecam,
    pub timer: Timer,
    pub dpad: Dpad,
    pub il_battle: IlBattle,
    pub il_mark: IlMark,
    pub camera: Camera,
    pub stage_edits: StageEdits,
    pub hide: Hide,
    pub sfx: Sfx,
    pub scratch: Scratch,
    pub validate: Validate,
    pub draw: Draw,
    pub pref: Pref,

    process_inputs_hook: ProcessInputsHook,
    draw_debug_text_hook: DrawDebugTextHook,
    oslink_hook: OSLinkHook,
    game_ready_init_hook: GameReadyInitHook,
    game_play_tick_hook: GamePlayTickHook,
}

impl AppContext {
    fn new() -> Self {
        crate::systems::heap::HEAP.init();

        let app_context = Self::default();
        app_context.process_inputs_hook.hook();
        app_context.draw_debug_text_hook.hook();
        app_context.oslink_hook.hook();
        app_context
    }
}

// Tick functions hook
hook!(ProcessInputsHook => (), mkb::process_inputs, || {
    with_app(|cx| cx.process_inputs_hook.clone()).call();

    with_app(|cx| {
        cx.pad.tick();
        cx.binds.tick(&cx.pad);
        cx.unlock.tick(&cx.pref);
        cx.iw.tick(&cx.pad);
        cx.lib_save_state.tick();
        cx.save_states_ui.tick(&cx.lib_save_state, &cx.pref, &cx.pad, &mut cx.draw, &cx.binds, &mut cx.timer);
        let mut menu_context = MenuContext {
            pad: &mut cx.pad,
            pref: &mut cx.pref,
            draw: &mut cx.draw,
            binds: &mut cx.binds,
            cm_seg: &mut cx.cm_seg,
            go_to_story: &mut cx.go_to_story,
            stage_edits: &mut cx.stage_edits,
            unlock: &mut cx.unlock,
        };
        cx.menu_impl.tick(&mut menu_context);
        cx.fallout.tick(&cx.pref, &cx.freecam);
        cx.jump.tick(&mut cx.pref, &cx.pad);
        cx.physics.tick(&cx.pref, &cx.freecam);
        cx.input_display.tick(&cx.pref);
        cx.go_to_story.tick();
        cx.cm_seg.tick(&cx.pref);
        cx.banans.tick(&cx.pref);
        cx.marathon.tick(&cx.pref);
        cx.ball_color.tick(&cx.pref);
        cx.freecam.tick(&mut cx.pref, &cx.pad, &mut cx.draw, &cx.binds);
        cx.il_battle.tick(&cx.pref, &cx.freecam, &cx.binds, &cx.pad);
        cx.il_mark.tick(&cx.lib_save_state);
        cx.camera.tick(&cx.pref);
        cx.stage_edits.tick();
        cx.validate.tick();
        cx.sfx.tick(&cx.pref);
        cx.scratch.tick();
        cx.pref.tick(&mut cx.draw);
    });
});

// Draw functions hook
hook!(DrawDebugTextHook => (), mkb::draw_debugtext, || {
    with_app(|cx| cx.draw_debug_text_hook.clone()).call();

    with_app(|cx| {
        // When the game is paused, screenshot the game's draw buffer before we draw our custom UI
        // elements. The original screenshot call is nopped.
        unsafe {
            if mkb::g_pause_status == 1 {
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

        cx.draw.predraw();
        cx.timer.draw(&cx.pref, &cx.freecam, &cx.validate);
        cx.iw.draw(&cx.pref, &cx.freecam);
        cx.il_battle.draw(&cx.pref, &cx.freecam, &cx.binds, &cx.pad);
        cx.cm_seg.draw(&cx.pref, &cx.freecam);
        cx.input_display.draw(&cx.pref, &cx.pad, &cx.freecam, &cx.ball_color);
        let mut menu_context = MenuContext {
            pad: &mut cx.pad,
            pref: &mut cx.pref,
            draw: &mut cx.draw,
            binds: &mut cx.binds,
            cm_seg: &mut cx.cm_seg,
            go_to_story: &mut cx.go_to_story,
            stage_edits: &mut cx.stage_edits,
            unlock: &mut cx.unlock,
        };
        cx.menu_impl.draw(&mut menu_context);
        cx.draw.draw();
        cx.il_mark.draw(&cx.pref, &cx.freecam);
        cx.physics.draw(&cx.pref, &cx.freecam);
        cx.scratch.draw();
    });
});

// MainGameLoad calls
hook!(OSLinkHook,
        rel_buffer: *mut mkb::OSModuleHeader,
        bss_buffer: *mut core::ffi::c_void => u8,
        mkb::OSLink,
        |rel_buffer, bss_buffer| {

    let ret = with_app(|cx| cx.oslink_hook.clone()).call(rel_buffer, bss_buffer);

    with_app(|cx| {
        let module_id = ModuleId::try_from(unsafe{*rel_buffer}.info.id);
        if let Ok(ModuleId::MainGame) = module_id {
            cx.game_ready_init_hook.hook();
            cx.game_play_tick_hook.hook();
        }
    });

    ret
});

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, || {
    with_app(|cx| {
        cx.stage_edits.on_game_ready_init(&cx.pref);
        cx.ball_color.switch_monkey(&cx.pref);

        cx.game_ready_init_hook.clone()
    }).call();
});

hook!(GamePlayTickHook => (), mkb::smd_game_play_tick, || {
    with_app(|cx| cx.game_play_tick_hook.clone()).call();

    with_app(|cx| {
        cx.validate.validate_run(&cx.pref, &cx.lib_save_state, &cx.menu_impl, &cx.physics, &cx.pad);
        cx.il_mark.validate_attempt(&cx.validate);
        cx.il_battle.validate_attempt(&mut cx.validate);
    });
});

pub fn init() {
    with_app(|_| {});
}

pub fn tick() {
    unsafe {
        // Replace overwritten function call
        mkb::perf_init_timer(4);

        with_app(|cx| {
            if cx.pref.get_bool(BoolPref::DebugMode) {
                mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
            } else {
                mkb::dip_switches &= !(mkb::DIP_DEBUG | mkb::DIP_DISP);
            }
            cx.pad.on_frame_start();
        });
    }
}
