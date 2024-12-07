use core::cell::RefCell;

use critical_section::{CriticalSection, Mutex};
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
        menu_impl::MenuImpl,
        pad::Pad,
        pref::{BoolPref, Pref},
    },
    utils::{libsavestate::LibSaveState, relutil::ModuleId},
};

pub static APP_CONTEXT: once_cell::sync::Lazy<critical_section::Mutex<AppContext>> =
    once_cell::sync::Lazy::new(|| critical_section::Mutex::new(AppContext::new()));

#[derive(Default)]
struct AppContext {
    process_inputs_hook: ProcessInputsHook,
    draw_debug_text_hook: DrawDebugTextHook,
    oslink_hook: OSLinkHook,
    game_ready_init_hook: GameReadyInitHook,
    game_play_tick_hook: GamePlayTickHook,

    pad: Pad,
    binds: Binds,
    menu_impl: MenuImpl,
    unlock: Unlock,
    iw: Iw,
    lib_save_state: LibSaveState,
    save_states_ui: SaveStatesUi,
    fallout: Fallout,
    jump: Jump,
    physics: Physics,
    input_display: InputDisplay,
    go_to_story: GoToStory,
    cm_seg: CmSeg,
    banans: Banans,
    marathon: Marathon,
    ball_color: BallColor,
    freecam: Freecam,
    timer: Timer,
    dpad: Dpad,
    il_battle: IlBattle,
    il_mark: IlMark,
    camera: Camera,
    stage_edits: StageEdits,
    hide: Hide,
    sfx: Sfx,
    scratch: Scratch,
    validate: Validate,
    draw: Draw,
    pref: Pref,
}

impl AppContext {
    fn new() -> Self {
        crate::systems::heap::HEAP.init();
        Self::default()
    }

    fn init(&mut self) {
        self.process_inputs_hook.hook();
        self.draw_debug_text_hook.hook();
        self.oslink_hook.hook();

        self.pad.on_main_loop_load();
        self.lib_save_state.on_main_loop_load();
        self.save_states_ui.on_main_loop_load();
        self.fallout.on_main_loop_load();
        self.cm_seg.on_main_loop_load();
        self.ball_color.on_main_loop_load();
        self.freecam.on_main_loop_load();
        self.dpad.on_main_loop_load();
        self.stage_edits.on_main_loop_load();
        self.hide.on_main_loop_load();
        with_global_cx(|gcx| {
            self.sfx.on_main_loop_load(gcx);
        });
        self.validate.on_main_loop_load();
    }
}

// Tick functions hook
hook!(ProcessInputsHook => (), mkb::process_inputs, || {
    critical_section::with(|cs| {
        let cx = &mut APP_CONTEXT.borrow_ref_mut(cs);

        cx.process_inputs_hook.call();

        with_global_cx(|gcx| {
            cx.pad.tick();
            cx.binds.tick(&mut cx.pad);
            cx.unlock.tick(cx);
            cx.iw.tick(cx);
            cx.save_states_ui.tick(cx);
            cx.menu_impl.tick(cx);
            cx.fallout.tick(cx);
            cx.jump.tick(cx);
            cx.physics.tick(cx);
            cx.input_display.tick(cx);
            cx.go_to_story.tick(cx);
            cx.cm_seg.tick(cx);
            cx.banans.tick(cx);
            cx.marathon.tick(cx);
            cx.ball_color.tick(gcx);
            cx.freecam.tick(cx);
            cx.il_battle.tick(cx);
            cx.il_mark.tick(cx);
            cx.camera.tick(cx);
            cx.stage_edits.tick(cx);
            cx.validate.tick(cx);
            cx.scratch.tick(cx);
            cx.pref.tick(cx);
        })
    });
});

// Draw functions hook
hook!(DrawDebugTextHook => (), mkb::draw_debugtext, || {
    critical_section::with(|cs| {
        let cx = &mut APP_CONTEXT.borrow_ref_mut(cs);

        cx.draw_debug_text_hook.call();

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

        cx.timer.draw(cx);
        cx.iw.draw(cx);
        cx.il_battle.draw(cx);
        cx.cm_seg.draw(cx);
        cx.input_display.draw(cx);
        cx.menu_impl.draw(cx);
        cx.draw.draw(cx);
        cx.il_mark.draw(cx);
        cx.physics.draw(cx);
        cx.scratch.draw(cx);
    });
});

// MainGameLoad calls
hook!(OSLinkHook,
        rel_buffer: *mut mkb::OSModuleHeader,
        bss_buffer: *mut core::ffi::c_void => u8,
        mkb::OSLink,
        |rel_buffer, bss_buffer| {

    critical_section::with(|cs| {
        let cx = &mut APP_CONTEXT.borrow_ref_mut(cs);

        let ret = cx.oslink_hook.call(rel_buffer, bss_buffer);

        let module_id = ModuleId::try_from(unsafe{*rel_buffer}.info.id);
        if let Ok(ModuleId::MainGame) = module_id {
            cx.game_ready_init_hook.hook();
            cx.game_play_tick_hook.hook();
        }

        ret
    })
});

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, || {
    critical_section::with(|cs| {
        let cx = &mut APP_CONTEXT.borrow_ref_mut(cs);

        cx.stage_edits.on_game_ready_init(cx);
        with_global_cx(|gcx| {
            BallColor::switch_monkey(gcx);
        });
        cx.game_ready_init_hook.call();
    });
});

hook!(GamePlayTickHook => (), mkb::smd_game_play_tick, || {
    critical_section::with(|cs| {
        let cx = &mut APP_CONTEXT.borrow_ref_mut(cs);

        cx.game_play_tick_hook.call();

        with_global_cx(|gcx| {
            cx.validate.validate_run(gcx, &cx.lib_save_state, &cx.menu_impl, &cx.physics, &cx.pad);
        });

        cx.il_mark.validate_attempt(&cx.validate);
        cx.il_battle.validate_attempt(&cx.validate);
    });
});

pub fn init() {
    critical_section::with(|cs| {
        APP_CONTEXT.borrow_ref_mut(cs).init();
    });
}

pub fn tick() {
    unsafe {
        // Replace overwritten function call
        mkb::perf_init_timer(4);

        with_global_cx(|cx| {
            if cx.pref.get_bool(BoolPref::DebugMode) {
                mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
            } else {
                mkb::dip_switches &= !(mkb::DIP_DEBUG | mkb::DIP_DISP);
            }
        });

        critical_section::with(|cs| {
            let cx = &mut APP_CONTEXT.borrow_ref_mut(cs);

            cx.pad.on_frame_start();
        });
    }
}
