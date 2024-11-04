use crate::{app_defn::AppContext, mkb, systems::pref::BoolPref};

#[derive(Clone, Copy, PartialEq, Eq, Default)]
enum MarathonState {
    #[default]
    WaitForGoal, // No velocity is stored
    StoringVel,        // Goal was just passed, vel will be stored next frame
    WaitForFirstApply, // Vel stored, waiting to be applied (but not stored, so we dont store
    // multiple times in one goal)
    WaitForApplyOrGoal, // Vel stored, waiting to be applied or stored again
}

pub struct Marathon {
    state: MarathonState,
    saved_vel: mkb::Vec,
}

impl Marathon {
    pub fn new() -> Self {
        Self {
            state: MarathonState::default(),
            saved_vel: mkb::Vec::default(),
        }
    }

    fn apply_saved_vel(&self) {
        unsafe {
            let ball = &mut mkb::balls[mkb::curr_player_idx as usize];

            // Set up the TF matrix for World ---> Start
            mkb::mtxa_from_rotate_z((*(*mkb::stagedef).start).rotation.z);
            mkb::mtxa_rotate_y((*(*mkb::stagedef).start).rotation.y);
            mkb::mtxa_rotate_x((*(*mkb::stagedef).start).rotation.x);

            let mut entered_goal_vel_rt_start = mkb::Vec::default();

            let mut saved_vel = self.saved_vel;

            mkb::mtxa_tf_vec(&raw mut saved_vel, &raw mut entered_goal_vel_rt_start);

            ball.vel = entered_goal_vel_rt_start;
        }
    }

    fn store_saved_vel(&mut self) {
        unsafe {
            let ball = &mut mkb::balls[mkb::curr_player_idx as usize];

            let goal_idx = mkb::mode_info.entered_goal_idx as usize;
            let goalIG_idx = mkb::mode_info.entered_goal_itemgroup_idx as usize;

            // Set up the TF matrix for Goal ---> World (Inverse of World ---> Goal)
            mkb::mtxa_from_mtx(&raw mut (*mkb::itemgroups.add(goalIG_idx)).transform);
            mkb::mtxa_translate(
                &raw mut (*(*(*mkb::stagedef).coli_header_list.add(goalIG_idx))
                    .goal_list
                    .add(goal_idx))
                .position,
            );
            mkb::mtxa_rotate_z(
                (*(*(*mkb::stagedef).coli_header_list.add(goalIG_idx))
                    .goal_list
                    .add(goal_idx))
                .rotation
                .z,
            );
            mkb::mtxa_rotate_y(
                (*(*(*mkb::stagedef).coli_header_list.add(goalIG_idx))
                    .goal_list
                    .add(goal_idx))
                .rotation
                .y,
            );
            mkb::mtxa_rotate_x(
                (*(*(*mkb::stagedef).coli_header_list.add(goalIG_idx))
                    .goal_list
                    .add(goal_idx))
                .rotation
                .x,
            );

            let mut entered_goal_vel_rt_world = mkb::Vec {
                x: ball.vel.x,
                y: ball.vel.y,
                z: ball.vel.z,
            };
            let mut entered_goal_vel_rt_goal = mkb::Vec::default();
            mkb::mtxa_rigid_inv_tf_vec(
                &raw mut entered_goal_vel_rt_world,
                &raw mut entered_goal_vel_rt_goal,
            );

            self.saved_vel = entered_goal_vel_rt_goal;
            self.state = MarathonState::WaitForFirstApply;
        }
    }

    fn wait_for_goal(&mut self) {
        unsafe {
            if mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN
            {
                self.state = MarathonState::StoringVel;
            }
        }
    }

    fn wait_for_apply(&mut self) {
        unsafe {
            if mkb::mode_info.stage_time_frames_remaining
                == (mkb::mode_info.stage_time_limit - 1) as i16
            {
                self.apply_saved_vel();
                self.state = MarathonState::WaitForApplyOrGoal;
            }
        }
    }

    pub fn tick(&mut self, cx: &AppContext) {
        let pref = &mut cx.pref.borrow_mut();
        if pref.get_bool(BoolPref::Marathon) {
            match self.state {
                MarathonState::WaitForGoal => self.wait_for_goal(),
                MarathonState::StoringVel => self.store_saved_vel(),
                MarathonState::WaitForFirstApply => self.wait_for_apply(),
                MarathonState::WaitForApplyOrGoal => {
                    self.wait_for_apply();
                    self.wait_for_goal();
                }
            }
        } else {
            self.state = MarathonState::WaitForGoal;
        }
    }
}
