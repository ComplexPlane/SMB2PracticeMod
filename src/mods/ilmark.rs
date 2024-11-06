use core::ffi::c_char;

use arrayvec::ArrayString;

use crate::{
    app::AppContext,
    cstr_buf, mkb,
    systems::{
        draw,
        pref::{BoolPref, Pref},
    },
    utils::version,
};

use super::validate::Validate;

pub struct IlMark {
    valid_run: bool,
    is_romhack: bool,
}

impl IlMark {
    pub fn new() -> Self {
        const VANILLA_GAME_NAME: [c_char; 4] = [b'G' as i8, b'M' as i8, b'2' as i8, b'E' as i8];
        const VANILLA_GAME_COMPANY: [c_char; 2] = [b'8' as i8, b'P' as i8];
        let is_romhack = unsafe {
            mkb::DVD_GAME_NAME != VANILLA_GAME_NAME || mkb::DVD_COMPANY != VANILLA_GAME_COMPANY
        };
        Self {
            valid_run: false,
            is_romhack,
        }
    }

    pub fn validate_attempt(&mut self, validate: &Validate) {
        if !validate.was_run_valid(false) {
            return;
        }
        self.valid_run = true;
    }

    pub fn tick(&mut self, cx: &AppContext) {
        unsafe {
            if mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN
                && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT
                && mkb::sub_mode != mkb::SMD_GAME_GOAL_MAIN
                && mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_INIT
                && mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_MAIN
                && mkb::sub_mode != mkb::SMD_GAME_BONUS_CLEAR_INIT
                && mkb::sub_mode != mkb::SMD_GAME_BONUS_CLEAR_MAIN
            {
                self.valid_run = false;
            }

            if cx.lib_save_state.borrow().state_loaded_this_frame {
                self.valid_run = false;
            }
        }
    }

    pub fn is_ilmark_enabled(&self, pref: &Pref) -> bool {
        unsafe {
            if mkb::main_mode != mkb::MD_GAME {
                return false;
            }

            if mkb::main_game_mode == mkb::PRACTICE_MODE {
                if !pref.get_bool(BoolPref::IlMarkPractice) {
                    return false;
                }
            } else if mkb::main_game_mode == mkb::STORY_MODE {
                if !pref.get_bool(BoolPref::IlMarkStory) {
                    return false;
                }
            } else if mkb::main_game_mode == mkb::CHALLENGE_MODE {
                if !pref.get_bool(BoolPref::IlMarkChallenge) {
                    return false;
                }
            } else {
                return false;
            }

            if self.is_romhack && !pref.get_bool(BoolPref::IlMarkRomhacks) {
                return false;
            }

            true
        }
    }

    pub fn draw(&self, cx: &AppContext) {
        let pref = &cx.pref.borrow();
        let freecam = &cx.freecam.borrow();
        if !self.is_ilmark_enabled(pref) || freecam.should_hide_hud(pref) {
            return;
        }

        unsafe {
            let in_show_submode = mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_BONUS_CLEAR_INIT
                || mkb::sub_mode == mkb::SMD_GAME_BONUS_CLEAR_MAIN;
            if !in_show_submode {
                return;
            }

            mkb::textdraw_reset();
            // Some good fonts that seem to be always loaded:
            // FONT32_ASC_8x16,
            // FONT32_ASC_12x12,
            // FONT32_ASC_24x24,  // Fairly big
            // FONT32_ASC_16x16P, // Doesn't support lowercase letters? P means proportional
            // FONT32_ASC_16x16,  // Doesn't support lowercase letters? Monospace
            mkb::textdraw_set_font(mkb::FONT32_ASC_8x16);
            // mkb::textdraw_set_flags(mkb::TEXTDRAW_FLAG_BORDER | mkb::TEXTDRAW_FLAG_PROPORTIONAL);

            let mut x = 634;
            let mut y = 474;
            if !self.valid_run {
                x -= 4;
                y -= 4;
            }

            mkb::textdraw_set_pos(x as f32, y as f32);
            mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT as mkb::SpriteAlignment);
            mkb::textdraw_set_scale(0.8, 0.8);
            let color = if self.valid_run {
                draw::LIGHT_GREEN
            } else {
                draw::LIGHT_RED
            };
            mkb::textdraw_set_mul_color(color.into());

            let mut version_str = ArrayString::<32>::new();
            version::get_version_str(&mut version_str);
            mkb::textdraw_print(cstr_buf!(version_str));
        }
    }
}
