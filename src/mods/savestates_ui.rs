use core::cell::RefCell;

use crate::{
    fmt, mkb,
    systems::{
        binds::Binds,
        draw::{self, Draw, NotifyDuration},
        pad::{Dir, Pad, Prio},
        pref::{BoolPref, Pref, U8Pref},
    },
    utils::libsavestate::{LibSaveState, LoadError, SaveError, SaveState},
};

#[derive(Default)]
pub struct SaveStatesUi {
    states: [SaveState; 8],
    active_state_slot: i32,
    created_state_last_frame: bool,
    frame_advance_mode: bool,
}

impl SaveStatesUi {
    pub fn new() -> Self {
        Self::default()
    }

    fn is_either_trigger_held(&self, pad: &Pad) -> bool {
        pad.analog_down(mkb::PAI_LTRIG as mkb::PadAnalogInput, Prio::Low)
            || pad.analog_down(mkb::PAI_RTRIG as mkb::PadAnalogInput, Prio::Low)
    }

    pub fn tick(
        &mut self,
        pad: &mut Pad,
        pref: &RefCell<Pref>,
        draw: &mut Draw,
        binds: &Binds,
        libsavestate: &mut LibSaveState,
    ) {
        // We must tightly scope our Pref usage to avoid a double borrow. libsavestate calls
        // mkb::set_minimap_mode(), we hook it, and it uses pref
        let disable_overwrite;
        let clear_bind;
        {
            let pref = &mut pref.borrow_mut();
            if !savestates_enabled(pref) {
                return;
            }
            disable_overwrite = pref.get_bool(BoolPref::SavestateDisableOverwrite);
            clear_bind = pref.get_u8(U8Pref::SavestateClearBind);
        }

        // Must tick savestates every frame
        for state in &mut self.states {
            state.tick(libsavestate);
        }

        if !self.is_either_trigger_held(pad) {
            self.frame_advance_mode = false;
        }

        // Must be in main game
        if unsafe { mkb::main_mode } != mkb::MD_GAME {
            return;
        }

        // Change the savestate slot with C stick
        let cstick_dir = pad.get_cstick_dir(Prio::Low);
        if cstick_dir != Dir::None {
            self.active_state_slot = cstick_dir as i32;
            draw.notify(
                draw::WHITE,
                NotifyDuration::Short,
                &fmt!(32, c"Slot %d Selected", self.active_state_slot + 1),
            );
        }

        if pad.button_pressed(mkb::PAD_BUTTON_X as mkb::PadDigitalInput, Prio::Low) {
            let state = &mut self.states[self.active_state_slot as usize];

            if !state.is_empty() && disable_overwrite {
                draw.notify(
                    draw::RED,
                    NotifyDuration::Short,
                    &fmt!(32, c"Slot {} Full", self.active_state_slot + 1),
                );
                return;
            }

            match state.save() {
                Ok(()) => {}
                Err(SaveError::MainMode) => {
                    // Unreachable
                    panic!("Unreachable state in savestate save");
                }
                Err(SaveError::PostFallout) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Fallout",
                    );
                    return;
                }
                Err(SaveError::PostGoal) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Goal",
                    );
                    return;
                }
                Err(SaveError::DuringRetry) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate During Retry",
                    );
                    return;
                }
                Err(SaveError::PostTimeout) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate After Timeout",
                    );
                    return;
                }
                Err(SaveError::SubMode) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate Here",
                    );
                    return;
                }
                Err(SaveError::ViewStage) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate in View Stage",
                    );
                    return;
                }
                Err(SaveError::InsufficientMemory) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Create Savestate: Not Enough Memory",
                    );
                    return;
                }
            }

            // TODO allow entering frame advance by pressing L/R while holding X in load-state mode
            self.frame_advance_mode = self.is_either_trigger_held(pad);
            if self.frame_advance_mode {
                draw.notify(
                    draw::PINK,
                    NotifyDuration::Short,
                    &fmt!(32, c"Slot {} Frame Advance", self.active_state_slot + 1),
                );
            } else {
                draw.notify(
                    draw::PINK,
                    NotifyDuration::Short,
                    &fmt!(32, c"Slot {} Saved", self.active_state_slot + 1),
                );
            }

            self.created_state_last_frame = true;
        } else if binds.bind_pressed(clear_bind, Prio::Low, pad) {
            let state = &mut self.states[self.active_state_slot as usize];
            state.clear();
            draw.notify(
                draw::BLUE,
                NotifyDuration::Short,
                &fmt!(32, c"Slot %d Cleared", self.active_state_slot + 1),
            );
        } else if pad.button_down(mkb::PAD_BUTTON_Y as mkb::PadDigitalInput, Prio::Low)
            || (pad.button_down(mkb::PAD_BUTTON_X as mkb::PadDigitalInput, Prio::Low)
                && self.created_state_last_frame)
            || self.frame_advance_mode
            || (self.is_either_trigger_held(pad) && cstick_dir != Dir::None)
        {
            let state = &mut self.states[self.active_state_slot as usize];
            match state.load(libsavestate) {
                Ok(()) => {}
                Err(LoadError::MainMode) => {
                    // Unreachable
                    panic!("Unreachable state in savestate load");
                }
                Err(LoadError::SubMode) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Load Savestate Here",
                    );
                    return;
                }
                Err(LoadError::TimeOver) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        "Cannot Load Savestate After Time Over",
                    );
                    return;
                }
                Err(LoadError::Empty) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Slot {} Empty", self.active_state_slot + 1),
                    );
                    return;
                }
                Err(LoadError::WrongStage) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Slot {} Wrong Stage", self.active_state_slot + 1),
                    );
                    return;
                }
                Err(LoadError::WrongMonkey) => {
                    // Thank you StevenCW for finding this marvelous bug
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Slot {} Wrong Monkey", self.active_state_slot + 1),
                    );
                    return;
                }
                Err(LoadError::ViewStage) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Cannot Load Savestate in View Stage"),
                    );
                    return;
                }
                Err(LoadError::PausedAndNonGameplaySubmode) => {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Short,
                        &fmt!(32, c"Cannot Load Savestate, Please Unpause"),
                    );
                    return;
                }
            }

            if !self.created_state_last_frame {
                draw.notify(
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
