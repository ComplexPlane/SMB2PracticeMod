#![cfg(feature = "mkb2")]

use core::ffi::CStr;

use crate::{
    app::AppContext,
    asm,
    mkb2::mkb2,
    systems::{
        draw,
        pad::{Pad, Prio},
        pref::BoolPref,
    },
    utils::{patch, timerdisp},
};

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

struct Context<'a> {
    pad: &'a mut Pad,
}

impl Iw {
    fn handle_iw_selection(&mut self, cx: &mut Context) {
        unsafe {
            if mkb2::scen_info.mode != 5 {
                return;
            }

            if cx
                .pad
                .analog_down(mkb2::PAI_LSTICK_LEFT as mkb2::PadAnalogInput, Prio::Low)
                || cx
                    .pad
                    .analog_down(mkb2::PAI_LSTICK_RIGHT as mkb2::PadAnalogInput, Prio::Low)
            {
                return;
            }

            if cx
                .pad
                .button_down(mkb2::PAD_BUTTON_LEFT as mkb2::PadDigitalInput, Prio::Low)
                || cx
                    .pad
                    .button_down(mkb2::PAD_BUTTON_RIGHT as mkb2::PadDigitalInput, Prio::Low)
            {
                return;
            }

            let lstick_up = cx
                .pad
                .analog_pressed(mkb2::PAI_LSTICK_UP as mkb2::PadAnalogInput, Prio::Low);
            let lstick_down = cx
                .pad
                .analog_pressed(mkb2::PAI_LSTICK_DOWN as mkb2::PadAnalogInput, Prio::Low);
            let dpad_up = cx
                .pad
                .button_pressed(mkb2::PAD_BUTTON_UP as mkb2::PadDigitalInput, Prio::Low);
            let dpad_down = cx
                .pad
                .button_pressed(mkb2::PAD_BUTTON_DOWN as mkb2::PadDigitalInput, Prio::Low);

            let dir = if lstick_up || dpad_up {
                1
            } else if lstick_down || dpad_down {
                -1
            } else {
                0
            };

            let story_save =
                &mut mkb2::storymode_save_files[mkb2::selected_story_file_idx as usize];
            if self.iw_files & (1 << mkb2::selected_story_file_idx) != 0 {
                let world = story_save.current_world as i32 + dir;
                if world < 0 || world > 9 {
                    story_save.is_valid = 0;
                    self.iw_files &= !(1 << mkb2::selected_story_file_idx);
                } else {
                    story_save.current_world = world as u8;
                }
            } else if dir != 0 {
                story_save.is_valid = 1;
                story_save.current_world = if dir == 1 { 0 } else { 9 };
                self.iw_files |= 1 << mkb2::selected_story_file_idx;
            }
        }
    }

    fn set_save_file_info(&mut self) {
        unsafe {
            if mkb2::scen_info.mode != 5 {
                return;
            }

            self.anim_counter += 1;

            for i in 0..3 {
                let story_save = &mut mkb2::storymode_save_files[i];
                if self.iw_files & (1 << i) != 0 {
                    let anim_frame = self.anim_counter as usize / 2 % (ANIM_STRS.len());
                    mkb2::sprintf(
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
            let retrace_count = mkb2::VIGetRetraceCount();

            // Halt the timer if we're selecting the story mode file
            // If we're still on the file selection screen and the IW file has been opened though,
            // start the timer during the open animation (to be more consistent with prior versions
            // of the IW code)
            if mkb2::scen_info.mode == 5
                && mkb2::data_select_menu_state != mkb2::DSMS_OPEN_DATA as mkb2::DataSelectMenuState
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

    pub fn tick(&mut self, cx: &AppContext) {
        let cx = &mut Context {
            pad: &mut cx.pad.borrow_mut(),
        };

        unsafe {
            asm::currently_playing_iw = 0;
        }
        unsafe {
            if mkb2::main_mode != mkb2::MD_GAME || mkb2::main_game_mode != mkb2::STORY_MODE {
                return;
            }

            let msg = c"Up/Down to Change World.";
            core::ptr::copy_nonoverlapping(
                msg.as_ptr(),
                &raw mut mkb2::continue_saved_game_text as *mut _,
                msg.to_bytes_with_nul().len(),
            );
            core::ptr::copy_nonoverlapping(
                msg.as_ptr(),
                &raw mut mkb2::start_game_from_beginning_text as *mut _,
                msg.to_bytes_with_nul().len(),
            );

            self.handle_iw_selection(cx);
            self.set_save_file_info();

            let file_idx = if mkb2::scen_info.mode == 5 {
                mkb2::selected_story_file_idx
            } else {
                mkb2::scen_info.save_file_idx
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

    pub fn draw(&self, cx: &AppContext) {
        let pref = &mut cx.pref.borrow_mut();
        let freecam = &mut cx.freecam.borrow_mut();
        unsafe {
            if !pref.get_bool(BoolPref::IwTimer)
                || mkb2::main_mode != mkb2::MD_GAME
                || mkb2::main_game_mode != mkb2::STORY_MODE
                || !(asm::currently_playing_iw != 0)
                || freecam.should_hide_hud(pref)
            {
                return;
            }
            timerdisp::draw_timer(self.iw_time as i32, "IW:", 0, draw::WHITE, false);
        }
    }
}
