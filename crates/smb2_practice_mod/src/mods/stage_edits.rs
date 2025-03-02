use critical_section::Mutex;
use mkb::mkb;

use num_enum::TryFromPrimitive;

use crate::{
    app::with_app,
    hook,
    systems::pref::{FromPref, I16Pref, Pref},
    utils::misc::with_mutex,
};

hook!(LoadStagedefHook, stage_id: u32 => (), mkb::load_stagedef, |stage_id| {
    with_mutex(&GLOBALS, |cx| {
        cx.load_stagedef_hook.call(stage_id);
    });
    with_app(|cx| {
        cx.stage_edits.on_load_stagedef(&cx.pref);
    });
});

#[derive(Clone, Copy, PartialEq, Eq, TryFromPrimitive, Default)]
#[repr(i16)]
pub enum ActiveMode {
    #[default]
    None = 0,
    Golden = 1,
    Dark = 2,
    Reverse = 3,
}

pub struct StageEdits {
    current_mode: ActiveMode,
    rev_goal_idx: u32,
    new_goal: bool,
}

struct Globals {
    load_stagedef_hook: LoadStagedefHook,
}

static GLOBALS: Mutex<Globals> = Mutex::new(Globals {
    load_stagedef_hook: LoadStagedefHook::new(),
});

impl Default for StageEdits {
    fn default() -> Self {
        with_mutex(&GLOBALS, |cx| {
            cx.load_stagedef_hook.hook();
        });
        Self {
            current_mode: Default::default(),
            rev_goal_idx: 0,
            new_goal: false,
        }
    }
}

impl StageEdits {
    pub fn select_new_goal(&mut self) {
        self.new_goal = true;
    }

    unsafe fn undo_mode(&self, mode: ActiveMode) {
        unsafe {
            match mode {
                ActiveMode::None => {}
                ActiveMode::Golden => {
                    // disable goals somehow
                    for i in 0..((*mkb::stagedef).goal_count as usize) {
                        (*(*mkb::stagedef).goal_list.add(i)).position.y += 10000.0;
                    }
                }
                ActiveMode::Dark => {}
                ActiveMode::Reverse => {
                    if (*mkb::stagedef).goal_count < 1 {
                        return;
                    }

                    let start = (*mkb::stagedef).start;
                    let goal = (*mkb::stagedef).goal_list.add(self.rev_goal_idx as usize);

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
    }

    unsafe fn set_mode(&mut self, mode: ActiveMode) {
        unsafe {
            match mode {
                ActiveMode::None => {}
                ActiveMode::Golden => {
                    // disable goals somehow
                    for i in 0..((*mkb::stagedef).goal_count as usize) {
                        (*(*mkb::stagedef).goal_list.add(i)).position.y -= 10000.0;
                    }
                }
                ActiveMode::Dark => {}
                ActiveMode::Reverse => {
                    if (*mkb::stagedef).goal_count < 1 {
                        return;
                    }
                    self.rev_goal_idx %= (*mkb::stagedef).goal_count;

                    let start = (*mkb::stagedef).start;
                    let goal = (*mkb::stagedef).goal_list.add(self.rev_goal_idx as usize);

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
    }

    fn on_load_stagedef(&mut self, pref: &Pref) {
        let next_mode = ActiveMode::from_pref(I16Pref::StageEditVariant, pref);
        self.current_mode = next_mode;
        unsafe {
            self.set_mode(self.current_mode);
        }
    }

    pub fn on_game_ready_init(&mut self, pref: &Pref) {
        let next_mode = ActiveMode::from_pref(I16Pref::StageEditVariant, pref);
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

    pub fn tick(&mut self) {
        unsafe {
            match self.current_mode {
                ActiveMode::None => {}
                ActiveMode::Golden => {
                    if mkb::mode_info.bananas_remaining == 0 {
                        mkb::mode_info.ball_mode |= 0x228;
                    }
                }
                ActiveMode::Dark => {
                    if !mkb::stagedef.is_null()
                        && mkb::mode_info.bananas_remaining != (*mkb::stagedef).banana_count
                    {
                        mkb::mode_info.ball_mode |= mkb::BALLMODE_FALLEN_OUT;
                    }
                }
                ActiveMode::Reverse => {}
            }
        }
    }
}
