use core::ffi::c_int;
use critical_section::Mutex;
use mkb::mkb;
use mkb::{byte, Vec};

use crate::app::with_app;
use crate::systems::pad::Button;
use crate::utils::misc::with_mutex;
use crate::{
    hook,
    systems::{
        menu_impl::MenuImpl,
        pad::{Pad, Prio},
        pref::{BoolPref, I16Pref, Pref},
    },
    utils::libsavestate::LibSaveState,
};

use super::physics::Physics;

hook!(DidBallEnterGoalHook, ball: *mut mkb::Ball, out_stage_goal_idx: *mut c_int,
        out_itemgroup_id: *mut c_int, out_goal_flags: *mut byte => u8, mkb::did_ball_enter_goal,
        |ball, out_stage_goal_idx, out_itemgroup_id, out_goal_flags| {

    let result = with_mutex(&GLOBALS, |cx| {
        cx.did_ball_enter_goal_hook.call(ball, out_stage_goal_idx, out_itemgroup_id, out_goal_flags)
    });

    with_app(|cx| {
        if result != 0 {
            // Determine framesave percentage
            cx.validate.find_framesave(ball);
            cx.validate.entered_goal = result != 0;
        }
        result
    })
});

struct Globals {
    did_ball_enter_goal_hook: DidBallEnterGoalHook,
}

static GLOBALS: Mutex<Globals> = Mutex::new(Globals {
    did_ball_enter_goal_hook: DidBallEnterGoalHook::new(),
});

pub struct Validate {
    framesave: u32,
    entered_goal: bool,
    used_mods: bool,
    has_paused: bool,
    loaded_savestate: bool,
}

impl Default for Validate {
    fn default() -> Self {
        with_mutex(&GLOBALS, |cx| {
            cx.did_ball_enter_goal_hook.hook();
        });
        Self {
            framesave: 0,
            entered_goal: false,
            used_mods: false,
            has_paused: false,
            loaded_savestate: false,
        }
    }
}

const INVALID_BOOL_PREFS: &[BoolPref] = &[
    BoolPref::DisableFalloutVolumes,
    BoolPref::JumpMod,
    BoolPref::Marathon,
    BoolPref::DebugMode,
];

const INVALID_U8_PREFS: &[I16Pref] = &[
    I16Pref::PhysicsPreset,
    I16Pref::TimerType,
    I16Pref::FalloutPlaneType,
    I16Pref::StageEditVariant,
];

impl Validate {
    pub fn disable_invalidating_settings(pref: &mut Pref) {
        // Set all bool prefs to default
        for &invalid_pref in INVALID_BOOL_PREFS {
            pref.set(invalid_pref, pref.get_default(invalid_pref));
        }

        // Set all u8 prefs to default
        for &invalid_pref in INVALID_U8_PREFS {
            pref.set(invalid_pref, pref.get_default(invalid_pref));
        }

        pref.save();
    }

    pub fn validate_run(
        &mut self,
        pref: &Pref,
        lib_save_state: &LibSaveState,
        menu_impl: &MenuImpl,
        physics: &Physics,
        pad: &Pad,
    ) {
        unsafe {
            // Track pauses
            let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
            if paused_now && !self.entered_goal {
                self.has_paused = true;
            }

            // Track savestates
            if lib_save_state.loaded_this_frame() {
                self.loaded_savestate = true;
            }

            // Using dpad controls is disallowed
            let dpad_down = pad.button_down(Button::DpadDown, Prio::Low)
                || pad.button_down(Button::DpadLeft, Prio::Low)
                || pad.button_down(Button::DpadRight, Prio::Low)
                || pad.button_down(Button::DpadUp, Prio::Low);
            if pref.get(BoolPref::DpadControls) && dpad_down {
                self.used_mods = true;
            }

            // Opening menu is disallowed
            if menu_impl.is_visible() {
                self.used_mods = true;
            }

            // Physics must be default or custom with default values
            if physics.using_custom_physics(pref) {
                self.used_mods = true;
            }

            // Invalid bool/u8 prefs are enabled
            for &invalid_pref in INVALID_BOOL_PREFS {
                if pref.get(invalid_pref) != pref.get_default(invalid_pref) {
                    self.used_mods = true;
                }
            }

            for &invalid_pref in INVALID_U8_PREFS {
                if pref.get(invalid_pref) != pref.get_default(invalid_pref) {
                    self.used_mods = true;
                }
            }
        }
    }

    fn line_intersects(&mut self, line_start: &Vec, line_end: &Vec, rect: &mut mkb::Rect) -> bool {
        let mut start: Vec = *line_start;
        let mut end: Vec = *line_end;

        unsafe {
            mkb::mtxa_from_translate(&raw mut rect.pos);
            mkb::mtxa_rotate_z(rect.rot.z);
            mkb::mtxa_rotate_y(rect.rot.y);
            mkb::mtxa_rotate_x(rect.rot.x);
            mkb::mtxa_rigid_inv_tf_point(&raw mut start, &raw mut start);
            mkb::mtxa_rigid_inv_tf_point(&raw mut end, &raw mut end);

            if (end.z < 0.0 && start.z < 0.0) || (0.0 < end.z && 0.0 < start.z) {
                return false;
            }

            let half_width = start.z - end.z;
            if 1.19209e-07 < half_width {
                end.x -= (start.x - end.x) * (end.z / half_width);
                end.y -= (start.y - end.y) * (end.z / half_width);
            }

            let half_width = rect.width * 0.5;
            let half_height = rect.height * 0.5;

            if end.x < -half_width || half_width < end.x {
                return false;
            }
            if end.y < -half_height || half_height < end.y {
                return false;
            }

            // Update framesave if first goal entered
            if mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT
                && mkb::sub_mode != mkb::SMD_GAME_GOAL_MAIN
                && mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_INIT
                && mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_MAIN
            {
                self.framesave = ((start.z / (start.z - end.z)) * 100.0) as u32;
            }

            true
        }
    }

    fn find_framesave(&mut self, ball: *mut mkb::Ball) {
        unsafe {
            let mut physicsball = mkb::PhysicsBall::default();
            mkb::init_physicsball_from_ball(ball, &raw mut physicsball);

            let mut itemgroup = (*mkb::stagedef).coli_header_list;
            let mut itemgroup_idx = 0;

            loop {
                if itemgroup_idx >= (*mkb::stagedef).coli_header_count {
                    break;
                }

                if (*itemgroup).goal_count > 0 {
                    if itemgroup_idx != physicsball.itemgroup_idx {
                        mkb::tf_physball_to_itemgroup_space(
                            &raw mut physicsball,
                            itemgroup_idx as c_int,
                        );
                    }

                    let mut goal = (*itemgroup).goal_list;
                    for _itemgroup_goal_idx in 0..(*itemgroup).goal_count {
                        mkb::mtxa_from_translate(&raw mut (*goal).position);
                        mkb::mtxa_rotate_z((*goal).rotation.z);
                        mkb::mtxa_rotate_y((*goal).rotation.y);
                        mkb::mtxa_rotate_x((*goal).rotation.x);

                        let mut goal_trigger = mkb::Rect::default();
                        goal_trigger.pos.x = 0.0;
                        goal_trigger.pos.y = 1.5;
                        goal_trigger.pos.z = 0.0;
                        mkb::mtxa_tf_point(&raw mut goal_trigger.pos, &raw mut goal_trigger.pos);
                        goal_trigger.rot = (*goal).rotation;
                        goal_trigger.width = 3.0;
                        goal_trigger.height = 3.0;

                        if self.line_intersects(
                            &physicsball.pos,
                            &physicsball.prev_pos,
                            &mut goal_trigger,
                        ) {
                            return; // Found goal ball travelled through
                        }

                        goal = goal.add(1);
                    }
                }

                itemgroup_idx += 1;
                itemgroup = itemgroup.add(1);
            }
        }
    }

    pub fn tick(&mut self) {
        unsafe {
            if mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT {
                self.entered_goal = false;
                self.used_mods = false;
                self.has_paused = false;
                self.loaded_savestate = false;
            }
        }
    }

    pub fn was_run_valid(&self, mods_allowed: bool) -> bool {
        (!self.used_mods || mods_allowed)
            && !self.has_paused
            && !self.loaded_savestate
            && self.entered_goal
    }

    pub fn get_framesave(&self) -> u32 {
        self.framesave
    }
}
