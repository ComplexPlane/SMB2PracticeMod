#![cfg(feature = "mkb2")]

use num_enum::TryFromPrimitive;

use crate::mkb2::mkb2;
use crate::{app::AppContext, hook, systems::pref::U8Pref};

hook!(LoadStagedefHook, stage_id: u32 => (), mkb2::load_stagedef, |stage_id, cx| {
    cx.stage_edits.borrow_mut().on_load_stagedef(stage_id, cx);
});

#[derive(Clone, Copy, PartialEq, Eq, TryFromPrimitive, Default)]
#[repr(u8)]
pub enum ActiveMode {
    #[default]
    None = 0,
    Golden = 1,
    Dark = 2,
    Reverse = 3,
}

#[derive(Default)]
pub struct StageEdits {
    current_mode: ActiveMode,
    rev_goal_idx: u32,
    new_goal: bool,
    load_stagedef_hook: LoadStagedefHook,
}

impl StageEdits {
    pub fn on_main_loop_load(&mut self, _cx: &AppContext) {
        self.load_stagedef_hook.hook();
    }

    pub fn select_new_goal(&mut self) {
        self.new_goal = true;
    }

    unsafe fn undo_mode(&self, mode: ActiveMode) {
        match mode {
            ActiveMode::None => {}
            ActiveMode::Golden => {
                // disable goals somehow
                for i in 0..((*mkb2::stagedef).goal_count as usize) {
                    (*(*mkb2::stagedef).goal_list.add(i)).position.y += 10000.0;
                }
            }
            ActiveMode::Dark => {}
            ActiveMode::Reverse => {
                if (*mkb2::stagedef).goal_count < 1 {
                    return;
                }

                let start = (*mkb2::stagedef).start;
                let goal = (*mkb2::stagedef).goal_list.add(self.rev_goal_idx as usize);

                let x = (*start).position.x;
                let y = (*start).position.y;
                let z = (*start).position.z;
                let sx = (*start).rotation.x;
                let sy = (*start).rotation.y;
                let sz = (*start).rotation.z;

                (*start).position.x = (*goal).position.x;
                (*start).position.y = (*goal).position.y + 0.5;
                (*start).position.z = (*goal).position.z;
                (*start).rotation.x = (*goal).rotation.x;
                (*start).rotation.y = (*goal).rotation.y + 32766;
                (*start).rotation.z = (*goal).rotation.z;

                (*goal).position.x = x;
                (*goal).position.y = y - 0.5;
                (*goal).position.z = z;
                (*goal).rotation.x = sx;
                (*goal).rotation.y = sy - 32766;
                (*goal).rotation.z = sz;
            }
        }
    }

    unsafe fn set_mode(&mut self, mode: ActiveMode) {
        match mode {
            ActiveMode::None => {}
            ActiveMode::Golden => {
                // disable goals somehow
                for i in 0..((*mkb2::stagedef).goal_count as usize) {
                    (*(*mkb2::stagedef).goal_list.add(i)).position.y -= 10000.0;
                }
            }
            ActiveMode::Dark => {}
            ActiveMode::Reverse => {
                if (*mkb2::stagedef).goal_count < 1 {
                    return;
                }
                self.rev_goal_idx %= (*mkb2::stagedef).goal_count as u32;

                let start = (*mkb2::stagedef).start;
                let goal = (*mkb2::stagedef).goal_list.add(self.rev_goal_idx as usize);

                let x = (*start).position.x;
                let y = (*start).position.y;
                let z = (*start).position.z;
                let sx = (*start).rotation.x;
                let sy = (*start).rotation.y;
                let sz = (*start).rotation.z;

                (*start).position.x = (*goal).position.x;
                (*start).position.y = (*goal).position.y + 0.5;
                (*start).position.z = (*goal).position.z;
                (*start).rotation.x = (*goal).rotation.x;
                (*start).rotation.y = (*goal).rotation.y + 32766;
                (*start).rotation.z = (*goal).rotation.z;

                (*goal).position.x = x;
                (*goal).position.y = y - 0.5;
                (*goal).position.z = z;
                (*goal).rotation.x = sx;
                (*goal).rotation.y = sy - 32766;
                (*goal).rotation.z = sz;
            }
        }
    }

    fn on_load_stagedef(&mut self, stage_id: u32, cx: &AppContext) {
        self.load_stagedef_hook.call(stage_id);

        let pref = &cx.pref.borrow();
        let next_mode = ActiveMode::try_from(pref.get_u8(U8Pref::StageEditVariant)).unwrap();
        self.current_mode = next_mode;
        unsafe {
            self.set_mode(self.current_mode);
        }
    }

    pub fn on_game_ready_init(&mut self, cx: &AppContext) {
        let pref = &cx.pref.borrow();
        let next_mode = ActiveMode::try_from(pref.get_u8(U8Pref::StageEditVariant)).unwrap();
        if self.current_mode != next_mode {
            unsafe {
                self.undo_mode(self.current_mode);
                self.current_mode = next_mode;
                self.set_mode(self.current_mode);
            }
        } else if self.current_mode == ActiveMode::Reverse && self.new_goal {
            unsafe {
                self.undo_mode(ActiveMode::Reverse);
                self.rev_goal_idx += 1;
                self.set_mode(ActiveMode::Reverse);
            }
        }
        self.new_goal = false;
    }

    pub fn tick(&mut self, _cx: &AppContext) {
        unsafe {
            match self.current_mode {
                ActiveMode::None => {}
                ActiveMode::Golden => {
                    if mkb2::mode_info.bananas_remaining == 0 {
                        mkb2::mode_info.ball_mode |= 0x228;
                    }
                }
                ActiveMode::Dark => {
                    if !mkb2::stagedef.is_null()
                        && mkb2::mode_info.bananas_remaining != (*mkb2::stagedef).banana_count
                    {
                        mkb2::mode_info.ball_mode |= mkb2::BALLMODE_FALLEN_OUT;
                    }
                }
                ActiveMode::Reverse => {}
            }
        }
    }
}
