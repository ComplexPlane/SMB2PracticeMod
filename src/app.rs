use core::cell::RefCell;

use crate::hook;
#[cfg(feature = "mkb1")]
use crate::mkb1::mkb1_platform::Mkb1Platform;
use crate::mkb2::mkb2;
#[cfg(feature = "mkb2")]
use crate::mkb2::mkb2_platform::Mkb2Platform;

#[cfg(feature = "mkb2")]
use crate::mods::ballcolor::BallColor;
#[cfg(feature = "mkb2")]
use crate::mods::banans::Banans;
#[cfg(feature = "mkb2")]
use crate::mods::camera::Camera;
#[cfg(feature = "mkb2")]
use crate::mods::cmseg::CmSeg;
#[cfg(feature = "mkb2")]
use crate::mods::dpad::Dpad;
#[cfg(feature = "mkb2")]
use crate::mods::fallout::Fallout;
#[cfg(feature = "mkb2")]
use crate::mods::freecam::Freecam;
#[cfg(feature = "mkb2")]
use crate::mods::gotostory::GoToStory;
#[cfg(feature = "mkb2")]
use crate::mods::hide::Hide;
#[cfg(feature = "mkb2")]
use crate::mods::ilbattle::IlBattle;
#[cfg(feature = "mkb2")]
use crate::mods::ilmark::IlMark;
#[cfg(feature = "mkb2")]
use crate::mods::inputdisp::InputDisplay;
#[cfg(feature = "mkb2")]
use crate::mods::iw::Iw;
#[cfg(feature = "mkb2")]
use crate::mods::jump::Jump;
#[cfg(feature = "mkb2")]
use crate::mods::marathon::Marathon;
#[cfg(feature = "mkb2")]
use crate::mods::physics::Physics;
#[cfg(feature = "mkb2")]
use crate::mods::savestates_ui::SaveStatesUi;
#[cfg(feature = "mkb2")]
use crate::mods::scratch::Scratch;
#[cfg(feature = "mkb2")]
use crate::mods::sfx::Sfx;
#[cfg(feature = "mkb2")]
use crate::mods::stage_edits::StageEdits;
#[cfg(feature = "mkb2")]
use crate::mods::timer::Timer;
#[cfg(feature = "mkb2")]
use crate::mods::unlock::Unlock;
#[cfg(feature = "mkb2")]
use crate::mods::validate::Validate;
#[cfg(feature = "mkb2")]
use crate::systems::binds::Binds;
#[cfg(feature = "mkb2")]
use crate::systems::draw::Draw;
#[cfg(feature = "mkb2")]
use crate::systems::menu_impl::MenuImpl;
#[cfg(feature = "mkb2")]
use crate::systems::pad::Pad;
#[cfg(feature = "mkb2")]
use crate::systems::pref::BoolPref;
#[cfg(feature = "mkb2")]
use crate::systems::pref::Pref;
#[cfg(feature = "mkb2")]
use crate::utils::libsavestate::LibSaveState;
#[cfg(feature = "mkb2")]
use crate::utils::relutil::ModuleId;

#[cfg(feature = "mkb1")]
pub static MKB: Mkb1Platform = Mkb1Platform {};
#[cfg(feature = "mkb2")]
pub static MKB: Mkb2Platform = Mkb2Platform {};

pub static APP_CONTEXT: once_cell::sync::Lazy<critical_section::Mutex<AppContext>> =
    once_cell::sync::Lazy::new(|| critical_section::Mutex::new(AppContext::new()));

#[derive(Default)]
pub struct AppContext {
    #[cfg(feature = "mkb2")]
    pub process_inputs_hook: RefCell<ProcessInputsHook>,
    #[cfg(feature = "mkb2")]
    pub draw_debug_text_hook: RefCell<DrawDebugTextHook>,
    #[cfg(feature = "mkb2")]
    pub oslink_hook: RefCell<OSLinkHook>,
    #[cfg(feature = "mkb2")]
    pub game_ready_init_hook: RefCell<GameReadyInitHook>,
    #[cfg(feature = "mkb2")]
    pub game_play_tick_hook: RefCell<GamePlayTickHook>,

    #[cfg(feature = "mkb2")]
    pub pad: RefCell<Pad>,
    #[cfg(feature = "mkb2")]
    pub binds: RefCell<Binds>,
    #[cfg(feature = "mkb2")]
    pub menu_impl: RefCell<MenuImpl>,
    #[cfg(feature = "mkb2")]
    pub unlock: RefCell<Unlock>,
    #[cfg(feature = "mkb2")]
    pub iw: RefCell<Iw>,
    #[cfg(feature = "mkb2")]
    pub lib_save_state: RefCell<LibSaveState>,
    #[cfg(feature = "mkb2")]
    pub save_states_ui: RefCell<SaveStatesUi>,
    #[cfg(feature = "mkb2")]
    pub fallout: RefCell<Fallout>,
    #[cfg(feature = "mkb2")]
    pub jump: RefCell<Jump>,
    #[cfg(feature = "mkb2")]
    pub physics: RefCell<Physics>,
    #[cfg(feature = "mkb2")]
    pub input_display: RefCell<InputDisplay>,
    #[cfg(feature = "mkb2")]
    pub go_to_story: RefCell<GoToStory>,
    #[cfg(feature = "mkb2")]
    pub cm_seg: RefCell<CmSeg>,
    #[cfg(feature = "mkb2")]
    pub banans: RefCell<Banans>,
    #[cfg(feature = "mkb2")]
    pub marathon: RefCell<Marathon>,
    #[cfg(feature = "mkb2")]
    pub ball_color: RefCell<BallColor>,
    #[cfg(feature = "mkb2")]
    pub freecam: RefCell<Freecam>,
    #[cfg(feature = "mkb2")]
    pub timer: RefCell<Timer>,
    #[cfg(feature = "mkb2")]
    pub dpad: RefCell<Dpad>,
    #[cfg(feature = "mkb2")]
    pub il_battle: RefCell<IlBattle>,
    #[cfg(feature = "mkb2")]
    pub il_mark: RefCell<IlMark>,
    #[cfg(feature = "mkb2")]
    pub camera: RefCell<Camera>,
    #[cfg(feature = "mkb2")]
    pub stage_edits: RefCell<StageEdits>,
    #[cfg(feature = "mkb2")]
    pub hide: RefCell<Hide>,
    #[cfg(feature = "mkb2")]
    pub sfx: RefCell<Sfx>,
    #[cfg(feature = "mkb2")]
    pub scratch: RefCell<Scratch>,
    #[cfg(feature = "mkb2")]
    pub validate: RefCell<Validate>,
    #[cfg(feature = "mkb2")]
    pub draw: RefCell<Draw>,
    #[cfg(feature = "mkb2")]
    pub pref: RefCell<Pref>,
}

impl AppContext {
    fn new() -> Self {
        #[cfg(feature = "mkb2")]
        crate::systems::heap::HEAP.init();

        Self::default()
    }

    fn init(&self) {
        #[cfg(feature = "mkb2")]
        self.process_inputs_hook.borrow_mut().hook();
        #[cfg(feature = "mkb2")]
        self.draw_debug_text_hook.borrow_mut().hook();
        #[cfg(feature = "mkb2")]
        self.oslink_hook.borrow_mut().hook();

        #[cfg(feature = "mkb2")]
        self.pad.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.lib_save_state.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.save_states_ui.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.fallout.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.cm_seg.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.ball_color.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.freecam.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.dpad.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.stage_edits.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.hide.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.sfx.borrow_mut().on_main_loop_load(self);
        #[cfg(feature = "mkb2")]
        self.validate.borrow_mut().on_main_loop_load(self);
    }
}

// Tick functions hook
#[cfg(feature = "mkb2")]
hook!(ProcessInputsHook => (), mkb2::process_inputs, |cx| {
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
#[cfg(feature = "mkb2")]
hook!(DrawDebugTextHook => (), mkb2::draw_debugtext, |cx| {
    cx.draw_debug_text_hook.borrow().call();

    // When the game is paused, screenshot the game's draw buffer before we draw our custom UI
    // elements. The original screenshot call is nopped.
    unsafe {
        if mkb2::g_pause_status == 1 {
            mkb2::take_pausemenu_screenshot(
                &raw mut mkb2::fullscreen_texture_buf as *mut _,
                0,
                0,
                (*mkb2::current_render_mode).fbWidth as i16,
                (*mkb2::current_render_mode).efbHeight as i16,
                mkb2::GX_TF_RGB5A3,
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
#[cfg(feature = "mkb2")]
hook!(OSLinkHook,
        rel_buffer: *mut mkb2::OSModuleHeader,
        bss_buffer: *mut core::ffi::c_void => u8,
        mkb2::OSLink,
        |rel_buffer, bss_buffer, cx| {
    let ret = cx.oslink_hook.borrow().call(rel_buffer, bss_buffer);

    let module_id = ModuleId::try_from(unsafe{*rel_buffer}.info.id);
    if let Ok(ModuleId::MainGame) = module_id {
        cx.game_ready_init_hook.borrow_mut().hook();
        cx.game_play_tick_hook.borrow_mut().hook();
    }

    ret
});

#[cfg(feature = "mkb2")]
hook!(GameReadyInitHook => (), mkb2::smd_game_ready_init, |cx| {
    cx.stage_edits.borrow_mut().on_game_ready_init(cx);
    cx.ball_color.borrow_mut().switch_monkey(&mut cx.pref.borrow_mut());
    cx.game_ready_init_hook.borrow().call();
});

#[cfg(feature = "mkb2")]
hook!(GamePlayTickHook => (), mkb2::smd_game_play_tick, |cx| {
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
    #[cfg(feature = "mkb2")]
    unsafe {
        // Replace overwritten function call
        mkb2::perf_init_timer(4);

        critical_section::with(|cs| {
            let cx = APP_CONTEXT.borrow(cs);
            let pref = cx.pref.borrow_mut();
            if pref.get_bool(BoolPref::DebugMode) {
                mkb2::dip_switches |= mkb2::DIP_DEBUG | mkb2::DIP_DISP;
            } else {
                mkb2::dip_switches &= !(mkb2::DIP_DEBUG | mkb2::DIP_DISP);
            }

            cx.pad.borrow_mut().on_frame_start();
        });
    }
}
