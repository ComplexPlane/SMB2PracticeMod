use arrayvec::ArrayString;
use mkb::mkb;

use crate::{
    systems::{
        draw,
        pref::{BoolPref, FromPref, I16Pref, Pref},
    },
    utils::timerdisp::{self, Format},
};

use num_enum::TryFromPrimitive;

use super::{freecam::Freecam, validate::Validate};

const FULLGAME_TIMER_LOCATION_X: u32 = 18 + 24;
const FULLGAME_TIMER_TEXT_OFFSET: u32 = 56;
const SEGMENT_TIMER_LOCATION_X: u32 = 30 + 24;
const SEGMENT_TIMER_TEXT_OFFSET: u32 = 44;
const IW_TIME_LOCATION_X: u32 = 42 + 24;
const WORLD_COUNT: usize = 10;
const STAGES_PER_WORLD: u32 = 10;
const SECOND_FRAMES: u32 = 60;
const MINUTE_FRAMES: u32 = SECOND_FRAMES * 60;
const HOUR_FRAMES: u32 = MINUTE_FRAMES * 60;

#[derive(TryFromPrimitive, PartialEq, Eq)]
#[repr(i16)]
pub enum ShowWhen {
    DontShow,
    AlwaysShow,
    BetweenWorlds,
    EndOfRun,
}

#[derive(Default, Copy, Clone)]
struct TimerGroup {
    segment: u32, // the time taken to complete a world up until tape break on the last stage
    full_world: u32, // the time taken to complete a world until the fade to white on the last stage
}

pub struct FullgameTimer {
    timer_groups: [TimerGroup; WORLD_COUNT],
    completed_stages: u32,
}

impl FullgameTimer {
    pub fn get_completed_stagecount(&self) -> u32 {
        self.completed_stages
    }

    fn reset_timer(&mut self) {
        self.completed_stages = 0;
        self.timer_groups = [TimerGroup::default(); WORLD_COUNT];
    }

    pub fn tick(&mut self, validate: &Validate) {
        unsafe {
            if mkb::scen_info.mode == 5 || mkb::scen_info.mode == 21 {
                self.reset_timer();
            }

            let mut sum = 0;
            for k in 0..WORLD_COUNT {
                sum += mkb::get_world_unbeaten_stage_count(k as i32) as u32;
            }
            self.completed_stages = sum;

            let is_on_spin_in = mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT
                || mkb::sub_mode == mkb::SMD_GAME_READY_INIT
                || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN;

            let is_on_gameplay = mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT
                || mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN;

            let is_on_exit_game = mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT
                || mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_INIT
                || mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_MAIN;

            let is_postgoal = mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN;

            let is_on_fallout = mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT
                || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_RETRY_INIT
                || mkb::sub_mode == mkb::SMD_GAME_RETRY_MAIN;

            let is_timeover = mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT
                || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN;

            let is_pre_load_in_stage_select = mkb::g_storymode_stageselect_state
                == mkb::STAGE_SELECT_INTRO_SEQUENCE as u16
                || mkb::g_storymode_stageselect_state == 3
                || mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_IDLE as u16;

            for k in 0..WORLD_COUNT {
                if mkb::scen_info.world == k as u16 {
                    if is_on_spin_in
                        || is_on_exit_game
                        || is_on_fallout
                        || is_timeover
                        || is_on_gameplay
                        || is_postgoal
                        || is_pre_load_in_stage_select
                    {
                        self.timer_groups[k].full_world += 1;
                    }
                    if self.completed_stages % STAGES_PER_WORLD != 9
                        && mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN
                    {
                        self.timer_groups[k].full_world += 1;
                    }
                    if (mkb::get_world_unbeaten_stage_count(k as i32) as u32) < 9
                        || ((mkb::get_world_unbeaten_stage_count(k as i32) as u32) == 9
                            && (is_pre_load_in_stage_select
                                || is_on_spin_in
                                || !validate.get_entered_goal()))
                    {
                        self.timer_groups[k].segment = self.timer_groups[k].full_world;
                    }
                }
            }
        }
    }

    pub fn draw(&self, pref: &Pref, freecam: &Freecam) {
        unsafe {
            if (mkb::main_game_mode != mkb::STORY_MODE
                && mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_INIT
                && mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_MAIN)
                || freecam.should_hide_hud(pref)
            {
                return;
            }

            let is_postgoal = mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT
                || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN;

            let mut is_between_worlds = false;
            if (self.completed_stages % STAGES_PER_WORLD == 9) && is_postgoal {
                is_between_worlds = true;
            } else if (self.completed_stages % STAGES_PER_WORLD == 0
                && mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE as u16)
                || self.completed_stages % STAGES_PER_WORLD != 0
            {
                is_between_worlds = false;
            }

            let is_run_complete = mkb::scen_info.world == 9
                && ((mkb::get_world_unbeaten_stage_count(9) == 9 && is_postgoal)
                    || mkb::get_world_unbeaten_stage_count(9) == 10);

            let mut fullgame_timer_location_y = 2;
            if pref.get(BoolPref::ShowDeathCounter) {
                fullgame_timer_location_y += 1;
            }

            let mut display_story_timer = false;
            match ShowWhen::from_pref(I16Pref::FullgameTimerShowWhen, pref) {
                ShowWhen::AlwaysShow => display_story_timer = true,
                ShowWhen::BetweenWorlds => display_story_timer = is_between_worlds,
                ShowWhen::EndOfRun => display_story_timer = is_run_complete,
                ShowWhen::DontShow => display_story_timer = false,
            }

            let mut full_world = [0u32; WORLD_COUNT];
            let mut segment = [0u32; WORLD_COUNT];

            for k in 0..WORLD_COUNT {
                if self.timer_groups[k].full_world > 0 {
                    full_world[k] = self.timer_groups[k].full_world + 1;
                    segment[k] = self.timer_groups[k].segment + 1;
                }
            }

            let mut sum = [0u32; WORLD_COUNT];
            let mut split = [0u32; WORLD_COUNT];

            for k in 1..WORLD_COUNT {
                for j in 0..k {
                    sum[k] += full_world[j];
                }
                split[k] = sum[k] + segment[k];
            }
            split[0] = segment[0];
            let loadless_story_timer = split[9];

            draw::debug_text(
                FULLGAME_TIMER_LOCATION_X,
                fullgame_timer_location_y,
                draw::WHITE,
                "Time:",
            );
            let mut timer_text = ArrayString::<32>::new();
            timerdisp::format_timer(loadless_story_timer as i32, Format::Full, &mut timer_text);
            draw::debug_text(
                FULLGAME_TIMER_LOCATION_X + FULLGAME_TIMER_TEXT_OFFSET,
                fullgame_timer_location_y,
                draw::WHITE,
                timer_text.as_str(),
            );

            // if display_story_timer {
            //     timerdisp::draw_timer(
            //         FULLGAME_TIMER_LOCATION_X,
            //         fullgame_timer_location_y,
            //         FULLGAME_TIMER_TEXT_OFFSET,
            //         c"Time:",
            //         loadless_story_timer,
            //         false,
            //         draw::WHITE,
            //     );
            // }

            let mut segment_timer_location_y = 2;
            if display_story_timer {
                segment_timer_location_y += 1;
            }
            if pref.get(BoolPref::ShowDeathCounter) {
                segment_timer_location_y += 1;
            }

            match ShowWhen::from_pref(I16Pref::SegmentTimerShowWhen, pref) {
                ShowWhen::AlwaysShow => {
                    for k in 0..WORLD_COUNT {
                        if mkb::scen_info.world == k as u16 && !is_run_complete {
                            timerdisp::draw_timer(
                                SEGMENT_TIMER_LOCATION_X,
                                segment_timer_location_y,
                                SEGMENT_TIMER_TEXT_OFFSET,
                                c"Seg:",
                                segment[k],
                                false,
                                draw::WHITE,
                            );
                        }
                    }
                }
                ShowWhen::BetweenWorlds => {
                    for k in 0..WORLD_COUNT {
                        if is_between_worlds && mkb::scen_info.world == k as u16 && k != 9 {
                            timerdisp::draw_timer(
                                SEGMENT_TIMER_LOCATION_X,
                                segment_timer_location_y,
                                SEGMENT_TIMER_TEXT_OFFSET,
                                c"Seg:",
                                segment[k],
                                false,
                                draw::WHITE,
                            );
                        }
                    }
                }
                ShowWhen::EndOfRun | ShowWhen::DontShow => {}
            }

            let mut split_hours = [0u32; 10];
            let mut split_minutes = [0u32; 10];
            let mut split_seconds = [0u32; 10];
            let mut split_centiseconds = [0u32; 10];

            let mut segment_hours = [0u32; 10];
            let mut segment_minutes = [0u32; 10];
            let mut segment_seconds = [0u32; 10];
            let mut segment_centiseconds = [0u32; 10];

            for k in 0..WORLD_COUNT {
                split_hours[k] = split[k] / HOUR_FRAMES;
                split_minutes[k] = split[k] % HOUR_FRAMES / MINUTE_FRAMES;
                split_seconds[k] = split[k] % MINUTE_FRAMES / SECOND_FRAMES;
                split_centiseconds[k] = (split[k] % SECOND_FRAMES) * 100 / 60;

                segment_hours[k] = segment[k] / HOUR_FRAMES;
                segment_minutes[k] = segment[k] % HOUR_FRAMES / MINUTE_FRAMES;
                segment_seconds[k] = segment[k] % MINUTE_FRAMES / SECOND_FRAMES;
                segment_centiseconds[k] = (segment[k] % SECOND_FRAMES) * 100 / 60;
            }

            let mut timer_str = [[0u8; 32]; 10];

            let segment_show = ShowWhen::from_pref(I16Pref::SegmentTimerShowWhen, pref);
            if segment_show != ShowWhen::DontShow && is_run_complete {
                for k in 0..WORLD_COUNT {
                    let x = if k != 9 {
                        IW_TIME_LOCATION_X
                    } else {
                        SEGMENT_TIMER_LOCATION_X
                    };
                    let y = 24 + (segment_timer_location_y + k) * 16;
                    if split_hours[k] > 0 {
                        if segment_hours[k] > 0 {
                            mkb::sprintf(
                                &raw mut timer_str[k] as *mut _,
                                c"W%d:%d:%02d:%02d.%02d (%d:%02d:%02d.%02d)".as_ptr() as *mut _,
                                k + 1,
                                split_hours[k],
                                split_minutes[k],
                                split_seconds[k],
                                split_centiseconds[k],
                                segment_hours[k],
                                segment_minutes[k],
                                segment_seconds[k],
                                segment_centiseconds[k],
                            );
                            draw::debug_text(
                                x as u32,
                                y as u32,
                                draw::WHITE,
                                c"%s",
                                &raw const timer_str[k] as *const _,
                            );
                        } else {
                            mkb::sprintf(
                                &raw mut timer_str[k] as *mut _,
                                c"W%d:%d:%02d:%02d.%02d (%02d:%02d.%02d)".as_ptr() as *mut _,
                                k + 1,
                                split_hours[k],
                                split_minutes[k],
                                split_seconds[k],
                                split_centiseconds[k],
                                segment_minutes[k],
                                segment_seconds[k],
                                segment_centiseconds[k],
                            );
                            draw::debug_text(
                                x as u32,
                                y as u32,
                                draw::WHITE,
                                c"%s",
                                &raw const timer_str[k] as *const _,
                            );
                        }
                    } else {
                        mkb::sprintf(
                            &raw mut timer_str[k] as *mut _,
                            c"W%d:%02d:%02d.%02d (%02d:%02d.%02d)".as_ptr() as *mut _,
                            k + 1,
                            split_minutes[k],
                            split_seconds[k],
                            split_centiseconds[k],
                            segment_minutes[k],
                            segment_seconds[k],
                            segment_centiseconds[k],
                        );
                        draw::debug_text(
                            x as u32,
                            y as u32,
                            draw::WHITE,
                            c"%s",
                            &raw const timer_str[k] as *const _,
                        );
                    }
                }
            }
        }
    }
}
