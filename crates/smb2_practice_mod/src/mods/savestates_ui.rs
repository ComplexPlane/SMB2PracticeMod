// TODO:
// Maybe try to see why size jumped up by 8kb?
// Make sure stuff works with frame advance
// Don't spam notifications while holding a button
// Add menu bind for clearing all savestates (and resets selected slot to 1)

use core::cell::Cell;

use critical_section::Mutex;
use mkb::mkb;
use num_enum::TryFromPrimitive;

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

// Re-entrant hook, cannot use app state
hook!(SetMinimapModeHook, mode: mkb::MinimapMode => (), mkb::set_minimap_mode, |mode| {
    with_mutex(&GLOBALS, |cx| {
        let enabled = !(
            cx.savestates_enabled.get() &&
            unsafe {mkb::main_mode} == mkb::MD_GAME &&
            unsafe {mkb::main_game_mode} == mkb::PRACTICE_MODE &&
            mode == mkb::MINIMAP_SHRINK
        );
        if enabled {
            cx.set_minimap_mode_hook.call(mode);
        }
    });
});

struct Context<'a> {
    lib_save_state: &'a LibSaveState,
    pref: &'a Pref,
    pad: &'a Pad,
    draw: &'a mut Draw,
    binds: &'a Binds,
    timer: &'a mut Timer,
}

#[derive(TryFromPrimitive)]
#[repr(u8)]
enum SaveTo {
    Selected,
    NextEmpty,
    NextEmptyThenOldest,
}

pub struct SaveStatesUi {
    states: [SaveState; 8],
    active_state_slot: usize,
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

    fn find_next_empty(&self) -> Option<usize> {
        for i in 0..self.states.len() {
            let slot_idx = (self.active_state_slot + i) % self.states.len();
            if self.states[slot_idx].is_empty() {
                return Some(slot_idx);
            }
        }
        None
    }

    fn pick_save_slot(&self, cx: &mut Context) -> Option<usize> {
        let save_to = SaveTo::try_from(cx.pref.get_u8(U8Pref::SavestateSaveTo)).unwrap();
        match save_to {
            SaveTo::Selected => Some(self.active_state_slot),
            SaveTo::NextEmpty => self.find_next_empty(),
            SaveTo::NextEmptyThenOldest => {
                let next_empty = self.find_next_empty();
                if next_empty.is_some() {
                    return next_empty;
                }

                let mut oldest_idx = 0;
                for i in 0..self.states.len() {
                    if self.states[i].timestamp() < self.states[oldest_idx].timestamp() {
                        oldest_idx = i;
                    }
                }
                Some(oldest_idx)
            }
        }
    }

    fn save_slot(&mut self, cx: &mut Context) {
        let Some(slot_idx) = self.pick_save_slot(cx) else {
            cx.draw.notify(
                draw::RED,
                NotifyDuration::Short,
                "Cannot Create Savestate: No Slots Left",
            );
            return;
        };
        let state = &mut self.states[slot_idx];

        if let Err(code) = state.save(cx.lib_save_state, cx.timer) {
            match code {
                SaveError::MainMode => {
                    // Unreachable
                    panic!("Unreachable state in savestate save");
                }
                SaveError::PostFallout => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Fallout",
                    );
                }
                SaveError::PostGoal => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Goal",
                    );
                }
                SaveError::DuringRetry => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate During Retry",
                    );
                }
                SaveError::PostTimeout => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Timeout",
                    );
                }
                SaveError::SubMode => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate Here",
                    );
                }
                SaveError::ViewStage => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate in View Stage",
                    );
                }
                SaveError::InsufficientMemory => {
                    cx.draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate: Not Enough Memory",
                    );
                }
            }
            return;
        }

        self.active_state_slot = slot_idx;
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
    }

    fn clear_slot(&mut self, cx: &mut Context) {
        let state = &mut self.states[self.active_state_slot];
        state.clear(cx.lib_save_state);
        cx.draw.notify(
            draw::BLUE,
            NotifyDuration::Short,
            &fmt!(32, c"Slot %d Cleared", self.active_state_slot + 1),
        );
    }

    fn load_slot(&mut self, cx: &mut Context) {
        let state = &mut self.states[self.active_state_slot];
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
    }

    pub fn tick(
        &mut self,
        lib_save_state: &LibSaveState,
        pref: &Pref,
        pad: &Pad,
        draw: &mut Draw,
        binds: &Binds,
        timer: &mut Timer,
    ) {
        let cx = &mut Context {
            lib_save_state,
            pref,
            pad,
            draw,
            binds,
            timer,
        };

        with_mutex(&GLOBALS, |g| {
            g.savestates_enabled.set(savestates_enabled(cx.pref));
        });

        if !savestates_enabled(cx.pref) {
            return;
        }
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
            self.active_state_slot = cstick_dir as usize;
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
            self.save_slot(cx);
        } else if cx.binds.bind_pressed(clear_bind, Prio::Low, cx.pad) {
            self.clear_slot(cx);
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
            self.load_slot(cx);
        } else {
            self.created_state_last_frame = false;
        }
    }
}

pub fn savestates_enabled(pref: &Pref) -> bool {
    pref.get_bool(BoolPref::Savestates) && !pref.get_bool(BoolPref::Freecam)
}
