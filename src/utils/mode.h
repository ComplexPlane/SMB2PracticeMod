#pragma once

#include "../mkb/mkb.h"

namespace mode {

constexpr u16 WORLD_COUNT = 10;
constexpr u16 STAGES_PER_WORLD = 10;

// submodes that are on the stage

bool is_spin_in_first_init(mkb::SubMode submode);
bool is_spin_in_init(mkb::SubMode submode);
bool is_spin_in(mkb::SubMode submode);

bool is_gameplay_init(mkb::SubMode submode);
bool is_gameplay_main(mkb::SubMode submode);
bool is_gameplay(mkb::SubMode submode);

bool is_goal_init(mkb::SubMode submode);
bool is_postgoal(mkb::SubMode submode);
bool is_postgoal_replay(mkb::SubMode submode);
bool is_postgoal_with_game_return(mkb::SubMode submode);

bool is_story_retry_screen(mkb::SubMode submode);
bool is_death_init(mkb::SubMode submode);
bool is_death_submode(mkb::SubMode submode);
bool is_timed_death_submode(mkb::SubMode submode);

bool is_on_stage(mkb::SubMode submode);
bool is_on_stage_with_endpoints(mkb::SubMode submode);

// other submodes

bool is_sel_ngc_init(mkb::SubMode submode);
bool is_sel_ngc_main(mkb::SubMode submode);
bool is_sel_ngc(mkb::SubMode submode);
bool is_game_scenario_main(mkb::SubMode submode);
bool is_game_scenario_return(mkb::SubMode submode);
bool is_titlescreen_main(mkb::SubMode submode);
bool is_story_exit_game_init(mkb::SubMode submode);
bool is_story_exit_game(mkb::SubMode submode);
bool is_stage_exit_init(mkb::SubMode submode);
bool is_stage_exit_submode(mkb::SubMode submode);

// 10 ball screen stuff

bool is_on_10_ball_spin_in(mkb::StoryModeStageSelectState state);
bool is_idle_on_10_ball_screen(mkb::StoryModeStageSelectState state);
bool has_selected_stage_on_10_ball_screen_init(mkb::StoryModeStageSelectState state);
bool has_selected_stage_on_10_ball_screen(mkb::StoryModeStageSelectState state);

bool is_on_10_ball_screen(mkb::SubMode sub_mode, mkb::ScenInfo scen_info);

// Other storymode stuff

bool is_main_game_mode_story(mkb::MainGameMode main_game_mode);

bool is_storymode_file_screen_init(mkb::ScenInfo scen_info);
bool is_storymode_file_screen_main(mkb::ScenInfo scen_info);
bool is_storymode_name_entry_screen_main(mkb::ScenInfo scen_info);

bool is_story_cutscene(mkb::SubMode submode);

// story mode status

u16 get_clear_count_for_world();
u16 get_storymode_total_clear_count();
// bool is_between_worlds(bool has_entered_goal);
// bool is_run_complete(bool has_entered_goal);

// misc utility

u16 theme_id_from_stage_id(int stage_id);
int get_story_stage_id_from_scen_info(mkb::ScenInfo scen_info);
bool is_paused();

}  // namespace mode
