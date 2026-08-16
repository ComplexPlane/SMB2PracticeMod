#pragma once

#include "../mkb/mkb.h"

namespace goal {

u8 get_frames_until_goal_submode();
bool get_goal_flag_game_return();
bool get_goal_flag_exit_game();
bool get_goal_flag_last_stage();

bool is_postgoal_exact();
bool is_gameplay_exact();
bool is_between_worlds();
bool is_run_complete();

void init();

}  // namespace goal
