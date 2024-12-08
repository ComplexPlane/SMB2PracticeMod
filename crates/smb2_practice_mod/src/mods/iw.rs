use core::ffi::CStr;

use mkb::mkb;

use crate::{
    asm,
    systems::{
        draw,
        pad::{Pad, Prio},
        pref::{BoolPref, Pref},
    },
    utils::{patch, timerdisp},
};

use super::freecam::Freecam;

const ANIM_STRS: &[&CStr] = &[c"/", c"-", c"\\", c" |"];

pub struct Iw {
    anim_counter: u32,
    iw_files: u32, // Bitflag for which save files are IW save files
    iw_time: u32,
    prev_retrace_count: u32,
}

impl Default for Iw {
    fn default() -> Self {
        unsafe {
            patch::write_branch(
                0x80274804 as *mut _,
                &raw const asm::stage_select_menu_hook as *const _,
            );
            patch::write_branch(
                0x8032a86c as *mut _,
                &raw const asm::pause_menu_text_hook as *const _,
            );
        }
        Self {
            anim_counter: 0,
            iw_files: 0,
            iw_time: 0,
            prev_retrace_count: 0,
        }
    }
}

impl Iw {
    fn handle_iw_selection(&mut self, pad: &Pad) {
        unsafe {
            if mkb::scen_info.mode != 5 {
                return;
            }

            if pad.analog_down(mkb::PAI_LSTICK_LEFT as mkb::PadAnalogInput, Prio::Low)
                || pad.analog_down(mkb::PAI_LSTICK_RIGHT as mkb::PadAnalogInput, Prio::Low)
            {
                return;
            }

            if pad.button_down(mkb::PAD_BUTTON_LEFT as mkb::PadDigitalInput, Prio::Low)
                || pad.button_down(mkb::PAD_BUTTON_RIGHT as mkb::PadDigitalInput, Prio::Low)
            {
                return;
            }

            let lstick_up =
                pad.analog_pressed(mkb::PAI_LSTICK_UP as mkb::PadAnalogInput, Prio::Low);
            let lstick_down =
                pad.analog_pressed(mkb::PAI_LSTICK_DOWN as mkb::PadAnalogInput, Prio::Low);
            let dpad_up = pad.button_pressed(mkb::PAD_BUTTON_UP as mkb::PadDigitalInput, Prio::Low);
            let dpad_down =
                pad.button_pressed(mkb::PAD_BUTTON_DOWN as mkb::PadDigitalInput, Prio::Low);

            let dir = if lstick_up || dpad_up {
                1
            } else if lstick_down || dpad_down {
                -1
            } else {
                0
            };

            let story_save = &mut mkb::storymode_save_files[mkb::selected_story_file_idx as usize];
            if self.iw_files & (1 << mkb::selected_story_file_idx) != 0 {
                let world = story_save.current_world as i32 + dir;
                if !(0..=9).contains(&world) {
                    story_save.is_valid = 0;
                    self.iw_files &= !(1 << mkb::selected_story_file_idx);
                } else {
                    story_save.current_world = world as u8;
                }
            } else if dir != 0 {
                story_save.is_valid = 1;
                story_save.current_world = if dir == 1 { 0 } else { 9 };
                self.iw_files |= 1 << mkb::selected_story_file_idx;
            }
        }
    }

    fn set_save_file_info(&mut self) {
        unsafe {
            if mkb::scen_info.mode != 5 {
                return;
            }

            self.anim_counter += 1;

            for i in 0..3 {
                let story_save = &mut mkb::storymode_save_files[i];
                if self.iw_files & (1 << i) != 0 {
                    let anim_frame = self.anim_counter as usize / 2 % (ANIM_STRS.len());
                    mkb::sprintf(
                        &raw mut story_save.file_name as *mut _,
                        c"W%02d IW %s".as_ptr() as *mut _,
                        story_save.current_world as i32 + 1,
                        ANIM_STRS[anim_frame].as_ptr(),
                    );
                    story_save.num_beaten_stages_in_current_world = 0;
                    story_save.score = 0;
                    story_save.playtime_in_frames = 0;
                }
            }
        }
    }

    fn handle_iw_timer(&mut self) {
        unsafe {
            let retrace_count = mkb::VIGetRetraceCount();

            // Halt the timer if we're selecting the story mode file
            // If we're still on the file selection screen and the IW file has been opened though,
            // start the timer during the open animation (to be more consistent with prior versions
            // of the IW code)
            if mkb::scen_info.mode == 5
                && mkb::data_select_menu_state != mkb::DSMS_OPEN_DATA as mkb::DataSelectMenuState
            {
                // We're not actually in the IW, zero the timer
                self.iw_time = 0;
            } else if asm::currently_playing_iw != 0 && !asm::is_iw_complete() {
                // We're in story mode playing an IW and it isn't finished, so increment the IW timer
                self.iw_time += retrace_count - self.prev_retrace_count;
            }
            // Else we're in story mode playing an IW, but we finished it, so don't change the time

            self.prev_retrace_count = retrace_count;
        }
    }

    pub fn tick(&mut self, pad: &Pad) {
        unsafe {
            asm::currently_playing_iw = 0;
        }
        unsafe {
            if mkb::main_mode != mkb::MD_GAME || mkb::main_game_mode != mkb::STORY_MODE {
                return;
            }

            let msg = c"Up/Down to Change World.";
            core::ptr::copy_nonoverlapping(
                msg.as_ptr(),
                &raw mut mkb::continue_saved_game_text as *mut _,
                msg.to_bytes_with_nul().len(),
            );
            core::ptr::copy_nonoverlapping(
                msg.as_ptr(),
                &raw mut mkb::start_game_from_beginning_text as *mut _,
                msg.to_bytes_with_nul().len(),
            );

            self.handle_iw_selection(pad);
            self.set_save_file_info();

            let file_idx = if mkb::scen_info.mode == 5 {
                mkb::selected_story_file_idx
            } else {
                mkb::scen_info.save_file_idx
            };

            // Maybe not the best way to detect if we're playing an IW but it works
            asm::currently_playing_iw = if self.iw_files & (1 << file_idx) != 0 {
                1
            } else {
                0
            };
            self.handle_iw_timer();
        }
    }

    pub fn draw(&self, pref: &Pref, freecam: &Freecam) {
        unsafe {
            if !pref.get_bool(BoolPref::IwTimer)
                || mkb::main_mode != mkb::MD_GAME
                || mkb::main_game_mode != mkb::STORY_MODE
                || asm::currently_playing_iw == 0
                || freecam.should_hide_hud(pref)
            {
                return;
            }
            timerdisp::draw_timer(self.iw_time as i32, "IW:", 0, draw::WHITE, false);
        }
    }
}
