use mkb::mkb;

use crate::{
    systems::{
        draw,
        pref::{BoolPref, Pref},
    },
    utils::{memstore::MemStore, timerdisp},
};

use super::{freecam::Freecam, validate::Validate};

pub struct Timer {
    retrace_count: u32,
    prev_retrace_count: u32,
    rta_timer: i32,
    pause_timer: i32,
}

impl Default for Timer {
    fn default() -> Self {
        Self {
            retrace_count: unsafe { mkb::VIGetRetraceCount() },
            prev_retrace_count: 0,
            rta_timer: 0,
            pause_timer: 0,
        }
    }
}

impl Timer {
    pub fn draw(&mut self, pref: &Pref, freecam: &Freecam, validate: &Validate) {
        unsafe {
            if mkb::main_mode != mkb::MD_GAME {
                return;
            }

            match mkb::sub_mode {
                mkb::SMD_GAME_READY_INIT
                | mkb::SMD_GAME_READY_MAIN
                | mkb::SMD_GAME_PLAY_INIT
                | mkb::SMD_GAME_PLAY_MAIN
                | mkb::SMD_GAME_GOAL_INIT
                | mkb::SMD_GAME_GOAL_MAIN
                | mkb::SMD_GAME_RINGOUT_INIT
                | mkb::SMD_GAME_RINGOUT_MAIN
                | mkb::SMD_GAME_TIMEOVER_INIT
                | mkb::SMD_GAME_TIMEOVER_MAIN
                | mkb::SMD_GAME_GOAL_REPLAY_INIT
                | mkb::SMD_GAME_GOAL_REPLAY_MAIN => {}
                _ => {
                    self.pause_timer = 0;
                    return;
                }
            }

            self.prev_retrace_count = self.retrace_count;
            self.retrace_count = mkb::VIGetRetraceCount();

            if mkb::sub_mode == mkb::SMD_GAME_READY_INIT {
                self.rta_timer = mkb::mode_info.stage_time_limit as i32;
                self.pause_timer = 0;
            } else if mkb::mode_info.ball_mode & mkb::BALLMODE_FREEZE_TIMER == 0 {
                self.rta_timer -= (self.retrace_count - self.prev_retrace_count) as i32;
                if mkb::g_some_other_flags & mkb::OF_GAME_PAUSED != 0 {
                    self.pause_timer += 1;
                }
            }

            let mut row = 1;

            if pref.get(BoolPref::TimerShowRTA) && !freecam.should_hide_hud(pref) {
                timerdisp::draw_timer(self.rta_timer, "RTA:", row, draw::WHITE, true);
                row += 1;
            }

            if pref.get(BoolPref::TimerShowPause) && !freecam.should_hide_hud(pref) {
                timerdisp::draw_timer(self.pause_timer, "PAU:", row, draw::WHITE, true);
                row += 1;
            }

            match mkb::sub_mode {
                mkb::SMD_GAME_GOAL_INIT
                | mkb::SMD_GAME_GOAL_MAIN
                | mkb::SMD_GAME_GOAL_REPLAY_INIT
                | mkb::SMD_GAME_GOAL_REPLAY_MAIN => {}
                _ => return,
            }

            let framesave = validate.get_framesave();

            if pref.get(BoolPref::TimerShowSubtick) && !freecam.should_hide_hud(pref) {
                timerdisp::draw_subtick_timer(
                    mkb::mode_info.stage_time_frames_remaining as i32,
                    "SUB:",
                    row,
                    draw::WHITE,
                    framesave,
                    false,
                );
                row += 1;
            }

            if pref.get(BoolPref::TimerShowFramesave) && !freecam.should_hide_hud(pref) {
                timerdisp::draw_percentage(framesave as i32, "FSV:", row, draw::WHITE);
            }
        }
    }

    pub fn save_state(&mut self, store: &mut MemStore) {
        store.scan_obj(&raw mut self.retrace_count);
        store.scan_obj(&raw mut self.prev_retrace_count);
        store.scan_obj(&raw mut self.rta_timer);
        store.scan_obj(&raw mut self.pause_timer);

        if matches!(store, MemStore::Load(..)) {
            let count = unsafe { mkb::VIGetRetraceCount() };
            self.prev_retrace_count = count - 1;
            self.retrace_count = count - 1;
        }
    }
}
