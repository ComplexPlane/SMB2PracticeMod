use core::{ffi::c_long, ptr::null_mut};

use critical_section::Mutex;
use mkb::mkb;
use num_enum::TryFromPrimitive;

use crate::{
    app::with_app,
    hook,
    systems::{
        draw,
        pref::{BoolPref, Pref, U8Pref},
    },
    utils::{misc::with_mutex, timerdisp},
};

use super::freecam::Freecam;

struct Globals {
    reset_cm_course_hook: ResetCmCourseHook,
}

static GLOBALS: Mutex<Globals> = Mutex::new(Globals {
    reset_cm_course_hook: ResetCmCourseHook::new(),
});

hook!(ResetCmCourseHook => (), mkb::g_reset_cm_course, || {
    with_mutex(&GLOBALS, |cx| {
        cx.reset_cm_course_hook.call();
    });
    with_app(|cx| {
        cx.cm_seg.on_reset_cm_course();
    });
});

#[derive(Copy, Clone, PartialEq, Default)]
enum State {
    #[default]
    Default,
    LoadMenu,
    EnterCm,
    SegActive,
    SegComplete,
}

#[derive(Copy, Clone, Default)]
pub enum Seg {
    #[default]
    Beginner1,
    BeginnerExtra,
    Advanced1,
    Advanced11,
    Advanced21,
    AdvancedExtra,
    Expert1,
    Expert11,
    Expert21,
    Expert31,
    Expert41,
    ExpertExtra,
    Master1,
    MasterExtra,
}

#[derive(Copy, Clone, TryFromPrimitive)]
#[repr(u8)]
enum Chara {
    AiAi,
    MeeMee,
    Baby,
    GonGon,
    Random,
}

pub struct CmSeg {
    state: State,
    seg_request: Seg,
    start_time: u32,
    seg_time: u32,
    overwritten_course_idx: usize,
    overwritten_entry_idx: usize,
    overwritten_opcode: mkb::CourseCommandOpcode,
    overwritten_starting_monkeys: i8,
    pbs: [u32; 14],
}

impl Default for CmSeg {
    fn default() -> Self {
        with_mutex(&GLOBALS, |cx| {
            cx.reset_cm_course_hook.hook();
        });
        Self {
            state: Default::default(),
            seg_request: Default::default(),
            start_time: 0,
            seg_time: 0,
            overwritten_course_idx: 0,
            overwritten_entry_idx: 0,
            overwritten_opcode: 0,
            overwritten_starting_monkeys: 0,
            pbs: [u32::MAX; 14],
        }
    }
}

const APE_CHARAS: [mkb::ApeCharacter; 4] = [
    mkb::APE_AIAI,
    mkb::APE_MEEMEE,
    mkb::APE_BABY,
    mkb::APE_GONGON,
];

impl CmSeg {
    fn gen_course(&mut self, course_idx: usize, start_course_stage_num: u16, stage_count: u16) {
        let mut start_entry_idx = None;
        let mut end_entry_idx = None;

        let mut curr_stage_count = 0;
        unsafe {
            let course = mkb::cm_courses[course_idx];
            let mut i = 0;
            loop {
                let entry = course.add(i as usize);
                if (*entry).opcode == mkb::COURSE_CMD_INFO as mkb::CourseCommandOpcode
                    && (*entry).type_ == 0
                {
                    curr_stage_count += 1;
                    if curr_stage_count == start_course_stage_num {
                        start_entry_idx = Some(i);
                    } else if curr_stage_count == start_course_stage_num + stage_count {
                        end_entry_idx = Some(i);
                        break;
                    }
                } else if (*entry).opcode == mkb::COURSE_CMD_END as mkb::CourseCommandOpcode {
                    if curr_stage_count == start_course_stage_num + stage_count - 1 {
                        end_entry_idx = Some(i);
                    }
                    break;
                }
                i += 1;
            }

            let start_entry_idx = start_entry_idx.unwrap();
            let end_entry_idx = end_entry_idx.unwrap();

            // Write new course end marker
            self.overwritten_course_idx = course_idx;
            self.overwritten_entry_idx = end_entry_idx;
            let overwritten_entry = course.add(end_entry_idx);
            self.overwritten_opcode = (*overwritten_entry).opcode;
            (*overwritten_entry).opcode = mkb::COURSE_CMD_END as mkb::CourseCommandOpcode;

            let first_stage_id = (*course.add(start_entry_idx as usize)).value as u16;
            mkb::mode_info.cm_course_stage_num = start_course_stage_num;
            mkb::mode_info.cm_stage_id = first_stage_id as i16;
            mkb::current_cm_entry = course.add((start_entry_idx + 1) as usize);
            mkb::g_another_stage_id = first_stage_id;

            // Make up "previous" stage for "current" stage
            let curr_stage = &mut mkb::cm_player_progress[0].curr_stage;
            curr_stage.stage_course_num = (start_course_stage_num - 1) as c_long;
            curr_stage.stage_id = (first_stage_id - 1) as c_long;

            // Next stage for player is the first one we want to start on
            let next_stage = &mut mkb::cm_player_progress[0].next_stages[0];
            next_stage.stage_course_num = start_course_stage_num as c_long;
            next_stage.stage_id = first_stage_id as c_long;
        }
    }

    unsafe fn state_load_menu(&mut self) {
        mkb::g_some_other_flags &= !mkb::OF_GAME_PAUSED;
        mkb::main_mode_request = mkb::MD_SEL;
        mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;

        mkb::sel_menu_info.menu_stack_ptr = 1;
        mkb::sel_menu_info.menu_stack[0] = 0;
        mkb::sel_menu_info.menu_stack[1] = 7;
        mkb::g_focused_root_menu = 0;
        mkb::g_focused_maingame_menu = 1;

        mkb::fade_screen_to_color(0x100, 0, 0x1e);

        self.state = State::EnterCm;
    }

    unsafe fn state_enter_cm(&mut self) {
        mkb::num_players = 1;
        self.overwritten_starting_monkeys = mkb::number_of_starting_monkeys;
        mkb::number_of_starting_monkeys = 100;
        mkb::menu_start_challenge_mode();

        mkb::g_playpoint_msg_counter = 0;

        self.start_time = mkb::VIGetRetraceCount();
        self.state = State::SegActive;
    }

    unsafe fn restore_overwritten_state(&mut self) {
        let overwritten_entry =
            mkb::cm_courses[self.overwritten_course_idx].add(self.overwritten_entry_idx);
        (*overwritten_entry).opcode = self.overwritten_opcode;
        mkb::number_of_starting_monkeys = self.overwritten_starting_monkeys;
    }

    unsafe fn check_exit_seg(&mut self) {
        if mkb::main_mode != mkb::MD_GAME {
            self.restore_overwritten_state();
            self.state = State::Default;
        }
    }

    unsafe fn state_seg_active(&mut self, pref: &Pref) {
        if mkb::sub_mode_request == mkb::SMD_GAME_READY_INIT {
            let ch = Chara::try_from(pref.get_u8(U8Pref::CmChara)).unwrap();
            mkb::active_monkey_id[0] = match ch {
                Chara::Random => APE_CHARAS[mkb::rand() as usize % 4],
                _ => APE_CHARAS[ch as usize],
            };
        }

        if self.overwritten_opcode != mkb::COURSE_CMD_END as mkb::CourseCommandOpcode {
            for i in 0..(mkb::sprite_pool_info.upper_bound as usize) {
                if *mkb::sprite_pool_info.status_list.add(i) == 0 {
                    continue;
                }
                let sprite = &raw mut mkb::sprites[i];
                let tick_func = (*sprite).tick_func;
                if tick_func == Some(mkb::sprite_final_stage_tick) {
                    *mkb::sprite_pool_info.status_list.add(i) = 0;
                    break;
                }
            }
        }

        self.seg_time = mkb::VIGetRetraceCount() - self.start_time;
        if mkb::mode_info.cm_stage_id == -1 && mkb::is_stage_complete(null_mut()) != 0 {
            let seg = self.seg_request as u32;
            if self.seg_time < self.pbs[seg as usize] {
                self.pbs[seg as usize] = self.seg_time;
            }
            self.state = State::SegComplete;
        }

        self.check_exit_seg();
    }

    unsafe fn state_seg_complete(&mut self) {
        if mkb::mode_info.cm_stage_id == -1
            && mkb::mode_info.ball_mode & mkb::BALLMODE_ON_BONUS_STAGE != 0
        {
            if mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT
                || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT
            {
                mkb::sub_mode_frame_counter += 120;
            } else if (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN)
                && mkb::sub_mode_frame_counter == 0x3c
            {
                mkb::fade_screen_to_color(0x101, 0, 0x3d);
                mkb::g_fade_track_volume(0x3c, 2);
            } else if mkb::sub_mode_request == mkb::SMD_GAME_READY_INIT {
                mkb::main_mode_request = mkb::MD_SEL;
                mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;
            }
        }

        if mkb::sub_mode_request == mkb::SMD_GAME_EXTRA_INIT
            || mkb::sub_mode_request == mkb::SMD_GAME_RESULT_INIT
            || mkb::main_mode_request == mkb::MD_AUTHOR
        {
            mkb::main_mode_request = mkb::MD_SEL;
            mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;
        }

        self.check_exit_seg();
    }

    unsafe fn init_seg(&mut self) {
        let course_idx;
        let start_course_stage_num;

        mkb::mode_flags &= !(mkb::MF_G_PLAYING_MASTER_COURSE
            | mkb::MF_PLAYING_EXTRA_COURSE
            | mkb::MF_PLAYING_MASTER_NOEX_COURSE
            | mkb::MF_PLAYING_MASTER_EX_COURSE);

        match self.seg_request {
            Seg::Beginner1 => {
                mkb::curr_difficulty = mkb::DIFF_BEGINNER;
                course_idx = 0;
                start_course_stage_num = 1;
            }
            Seg::BeginnerExtra => {
                mkb::curr_difficulty = mkb::DIFF_BEGINNER;
                mkb::mode_flags |= mkb::MF_PLAYING_EXTRA_COURSE;
                course_idx = 3;
                start_course_stage_num = 1;
            }
            Seg::Advanced1 => {
                mkb::curr_difficulty = mkb::DIFF_ADVANCED;
                course_idx = 1;
                start_course_stage_num = 1;
            }
            Seg::Advanced11 => {
                mkb::curr_difficulty = mkb::DIFF_ADVANCED;
                course_idx = 1;
                start_course_stage_num = 11;
            }
            Seg::Advanced21 => {
                mkb::curr_difficulty = mkb::DIFF_ADVANCED;
                course_idx = 1;
                start_course_stage_num = 21;
            }
            Seg::AdvancedExtra => {
                mkb::curr_difficulty = mkb::DIFF_ADVANCED;
                mkb::mode_flags |= mkb::MF_PLAYING_EXTRA_COURSE;
                course_idx = 4;
                start_course_stage_num = 1;
            }
            Seg::Expert1 => {
                mkb::curr_difficulty = mkb::DIFF_EXPERT;
                course_idx = 2;
                start_course_stage_num = 1;
            }
            Seg::Expert11 => {
                mkb::curr_difficulty = mkb::DIFF_EXPERT;
                course_idx = 2;
                start_course_stage_num = 11;
            }
            Seg::Expert21 => {
                mkb::curr_difficulty = mkb::DIFF_EXPERT;
                course_idx = 2;
                start_course_stage_num = 21;
            }
            Seg::Expert31 => {
                mkb::curr_difficulty = mkb::DIFF_EXPERT;
                course_idx = 2;
                start_course_stage_num = 31;
            }
            Seg::Expert41 => {
                mkb::curr_difficulty = mkb::DIFF_EXPERT;
                course_idx = 2;
                start_course_stage_num = 41;
            }
            Seg::ExpertExtra => {
                mkb::curr_difficulty = mkb::DIFF_EXPERT;
                mkb::mode_flags |= mkb::MF_PLAYING_EXTRA_COURSE;
                course_idx = 5;
                start_course_stage_num = 1;
            }
            Seg::Master1 => {
                mkb::curr_difficulty = mkb::DIFF_EXPERT;
                mkb::mode_flags |= mkb::MF_PLAYING_EXTRA_COURSE
                    | mkb::MF_G_PLAYING_MASTER_COURSE
                    | mkb::MF_PLAYING_MASTER_NOEX_COURSE;
                course_idx = 6;
                start_course_stage_num = 1;
            }
            Seg::MasterExtra => {
                mkb::curr_difficulty = mkb::DIFF_EXPERT;
                mkb::mode_flags = 0x0280071D;
                course_idx = 7;
                start_course_stage_num = 1;
            }
        }
        self.gen_course(course_idx, start_course_stage_num, 10);
    }

    pub fn request_cm_seg(&mut self, seg: Seg) {
        self.seg_request = seg;
        if self.state == State::SegActive || self.state == State::SegComplete {
            unsafe {
                self.restore_overwritten_state();
            }
        }
        unsafe {
            if mkb::main_mode == mkb::MD_SEL {
                self.state = State::EnterCm;
            } else {
                self.state = State::LoadMenu;
            }
        }
    }

    pub fn tick(&mut self, pref: &Pref) {
        unsafe {
            match self.state {
                State::LoadMenu => self.state_load_menu(),
                State::EnterCm => self.state_enter_cm(),
                State::SegActive => self.state_seg_active(pref),
                State::SegComplete => self.state_seg_complete(),
                State::Default => {}
            }
        }
    }

    pub fn draw(&self, pref: &Pref, freecam: &Freecam) {
        if !pref.get_bool(BoolPref::CmTimer) || freecam.should_hide_hud(pref) {
            return;
        }

        if self.state == State::SegActive || self.state == State::SegComplete {
            let seg = self.seg_request as usize;
            let color = if self.state == State::SegComplete && self.pbs[seg] == self.seg_time {
                draw::GOLD
            } else {
                draw::WHITE
            };

            timerdisp::draw_timer(self.seg_time as i32, "SEG:", 0, color, false);
        }
    }

    pub fn on_reset_cm_course(&mut self) {
        if self.state == State::SegActive {
            unsafe {
                self.init_seg();
            }
        }
    }
}
