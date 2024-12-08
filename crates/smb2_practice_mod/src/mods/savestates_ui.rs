use core::cell::Cell;

use critical_section::Mutex;
use mkb::mkb;

use crate::{
    fmt, hook,
    systems::{
        binds::Binds,
        draw::{self, Draw, NotifyDuration},
        pad::{Dir, Pad, Prio},
        pref::{BoolPref, Pref, U8Pref},
    },
    utils::{
        libsavestate::{LibSaveState, LoadError, SaveError, SaveState},
        misc::with_mutex,
    },
};

use super::timer::Timer;

struct Globals {
    set_minimap_mode_hook: SetMinimapModeHook,
    savestates_enabled: Cell<bool>,
}

static GLOBALS: Mutex<Globals> = Mutex::new(Globals {
    set_minimap_mode_hook: SetMinimapModeHook::new(),
    savestates_enabled: Cell::new(false),
});

// Reentrant hook, cannot use app state
hook!(SetMinimapModeHook, mode: mkb::MinimapMode => (), mkb::set_minimap_mode, |mode| {
    with_mutex(&GLOBALS, |cx| {
        let enabled = !cx.savestates_enabled.get()
            || !(unsafe {mkb::main_mode} == mkb::MD_GAME
            && unsafe {mkb::main_game_mode} == mkb::PRACTICE_MODE &&
            mode == mkb::MINIMAP_SHRINK);
        if enabled {
            cx.set_minimap_mode_hook.call(mode);
        }
    });
});

struct Context<'a> {
    pref: &'a Pref,
    pad: &'a Pad,
    draw: &'a mut Draw,
    binds: &'a Binds,
    lib_save_state: &'a mut LibSaveState,
    timer: &'a mut Timer,
}

pub struct SaveStatesUi {
    states: [SaveState; 8],
    active_state_slot: i32,
    created_state_last_frame: bool,
    frame_advance_mode: bool,
}

impl Default for SaveStatesUi {
    fn default() -> Self {
        with_mutex(&GLOBALS, |cx| {
            cx.set_minimap_mode_hook.hook();
        });
        Self {
            states: Default::default(),
            active_state_slot: 0,
            created_state_last_frame: false,
            frame_advance_mode: false,
        }
    }
}

impl SaveStatesUi {
    fn is_either_trigger_held(&self, pad: &Pad) -> bool {
        pad.analog_down(mkb::PAI_LTRIG as mkb::PadAnalogInput, Prio::Low)
            || pad.analog_down(mkb::PAI_RTRIG as mkb::PadAnalogInput, Prio::Low)
    }

    pub fn tick(
        &mut self,
        pref: &Pref,
        pad: &Pad,
        draw: &mut Draw,
        binds: &Binds,
        lib_save_state: &mut LibSaveState,
        timer: &mut Timer,
    ) {
        let cx = &mut Context {
            pref,
            pad,
            draw,
            binds,
            lib_save_state,
            timer,
        };

        with_mutex(&GLOBALS, |g| {
            g.savestates_enabled.set(savestates_enabled(cx.pref));
        });

        if !savestates_enabled(cx.pref) {
            return;
        }
        let disable_overwrite = pref.get_bool(BoolPref::SavestateDisableOverwrite);
        let clear_bind = pref.get_u8(U8Pref::SavestateClearBind);

        // Must tick savestates every frame
        for state in &mut self.states {
            state.tick(cx.lib_save_state, cx.timer);
        }

        if !self.is_either_trigger_held(cx.pad) {
            self.frame_advance_mode = false;
        }

        // Must be in main game
        if unsafe { mkb::main_mode } != mkb::MD_GAME {
            return;
        }

        // Change the savestate slot with C stick
        let cstick_dir = cx.pad.get_cstick_dir(Prio::Low);
        if cstick_dir != Dir::None {
            self.active_state_slot = cstick_dir as i32;
            cx.draw.notify(
                draw::WHITE,
                NotifyDuration::Short,
                &fmt!(32, c"Slot %d Selected", self.active_state_slot + 1),
            );
        }

        if cx
            .pad
            .button_pressed(mkb::PAD_BUTTON_X as mkb::PadDigitalInput, Prio::Low)
        {
            let state = &mut self.states[self.active_state_slot as usize];

            if !state.is_empty() && disable_overwrite {
                cx.draw.notify(
                    draw::RED,
                    NotifyDuration::Short,
                    &fmt!(32, c"Slot %d Full", self.active_state_slot + 1),
                );
                return;
            }

            match state.save(cx.timer) {
                Ok(()) => {}
                Err(SaveError::MainMode) => {
                    // Unreachable
                    panic!("Unreachable state in savestate save");
                }
                Err(SaveError::PostFallout) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Fallout",
                    );
                    return;
                }
                Err(SaveError::PostGoal) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Goal",
                    );
                    return;
                }
                Err(SaveError::DuringRetry) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate During Retry",
                    );
                    return;
                }
                Err(SaveError::PostTimeout) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Timeout",
                    );
                    return;
                }
                Err(SaveError::SubMode) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate Here",
                    );
                    return;
                }
                Err(SaveError::ViewStage) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate in View Stage",
                    );
                    return;
                }
                Err(SaveError::InsufficientMemory) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate: Not Enough Memory",
                    );
                    return;
                }
            }

            self.frame_advance_mode = self.is_either_trigger_held(cx.pad);
            if self.frame_advance_mode {
                cx.draw.notify(
                    draw::PINK,
                    NotifyDuration::Short,
                    &fmt!(32, c"Slot %d Frame Advance", self.active_state_slot + 1),
                );
            } else {
                cx.draw.notify(
                    draw::PINK,
                    NotifyDuration::Short,
                    &fmt!(32, c"Slot %d Saved", self.active_state_slot + 1),
                );
            }

            self.created_state_last_frame = true;
        } else if cx.binds.bind_pressed(clear_bind, Prio::Low, cx.pad) {
            let state = &mut self.states[self.active_state_slot as usize];
            state.clear();
            cx.draw.notify(
                draw::BLUE,
                NotifyDuration::Short,
                &fmt!(32, c"Slot %d Cleared", self.active_state_slot + 1),
            );
        } else if cx
            .pad
            .button_down(mkb::PAD_BUTTON_Y as mkb::PadDigitalInput, Prio::Low)
            || (cx
                .pad
                .button_down(mkb::PAD_BUTTON_X as mkb::PadDigitalInput, Prio::Low)
                && self.created_state_last_frame)
            || self.frame_advance_mode
            || (self.is_either_trigger_held(cx.pad) && cstick_dir != Dir::None)
        {
            let state = &mut self.states[self.active_state_slot as usize];
            match state.load(cx.lib_save_state, cx.timer) {
                Ok(()) => {}
                Err(LoadError::MainMode) => {
                    // Unreachable
                    panic!("Unreachable state in savestate load");
                }
                Err(LoadError::SubMode) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Load Savestate Here",
                    );
                    return;
                }
                Err(LoadError::TimeOver) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Load Savestate After Time Over",
                    );
                    return;
                }
                Err(LoadError::Empty) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Slot %d Empty", self.active_state_slot + 1),
                    );
                    return;
                }
                Err(LoadError::WrongStage) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Slot %d Wrong Stage", self.active_state_slot + 1),
                    );
                    return;
                }
                Err(LoadError::WrongMonkey) => {
                    // Thank you StevenCW for finding this marvelous bug
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Slot %d Wrong Monkey", self.active_state_slot + 1),
                    );
                    return;
                }
                Err(LoadError::ViewStage) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Cannot Load Savestate in View Stage"),
                    );
                    return;
                }
                Err(LoadError::PausedAndNonGameplaySubmode) => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Cannot Load Savestate, Please Unpause"),
                    );
                    return;
                }
            }

            if !self.created_state_last_frame {
                cx.draw.notify(
                    draw::BLUE,
                    NotifyDuration::Short,
                    &fmt!(32, c"Slot %d Loaded", self.active_state_slot + 1),
                );
            }
        } else {
            self.created_state_last_frame = false;
        }
    }
}

pub fn savestates_enabled(pref: &Pref) -> bool {
    pref.get_bool(BoolPref::Savestates) && !pref.get_bool(BoolPref::Freecam)
}
