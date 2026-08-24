#include "mode.h"

// #include "../internal/relutil.h"
#include "mkb/mkb.h"
#include "mkb/mkb2_ghidra.h"

namespace mode {

// This file collects several submode, main game mode, scenario mode, and storymode stage select
// state checks all in one place. Since these kinds of checks are common, this is done with
// the hope of trying to make things more concise/convenient while retaining clarity

// --- game scenario submodes ---

bool is_game_scenario_return(mkb::SubMode submode) {
    // this submode doesn't seem to be entered on first 10 ball screen spin in
    return submode == mkb::SMD_GAME_SCENARIO_RETURN;
}

bool is_game_scenario_main(mkb::SubMode submode) {
    // submode that includes the 10 ball screen, the story mode file select, and name creation
    // screens
    return (submode == mkb::SMD_GAME_SCENARIO_MAIN);
}

bool is_game_scenario(mkb::SubMode submode) {
    // includes the 1 frame of game scenario return; it's useful to include this in some checks
    return (submode == mkb::SMD_GAME_SCENARIO_MAIN || submode == mkb::SMD_GAME_SCENARIO_RETURN);
}

// --- spin in ---

bool is_spin_in_first_init(mkb::SubMode submode) {
    return submode == mkb::SMD_GAME_FIRST_INIT;
}

bool is_spin_in_init(mkb::SubMode submode) {
    return submode == mkb::SMD_GAME_READY_INIT;
}

bool is_spin_in(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_READY_INIT ||
            submode == mkb::SMD_GAME_READY_MAIN);  // does not include SMD_GAME_FIRST_INIT
}

bool is_spin_in_with_first_init(mkb::SubMode submode) {
    return (is_spin_in_first_init(submode) || is_spin_in(submode));
}

// --- gameplay ---

bool is_gameplay_init(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_PLAY_INIT);
}

bool is_gameplay(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_PLAY_INIT || submode == mkb::SMD_GAME_PLAY_MAIN);
}

bool is_gameplay_main(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_PLAY_MAIN);
}

// --- goal submodes ---

bool is_postgoal_pre_replay(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_GOAL_INIT || submode == mkb::SMD_GAME_GOAL_MAIN);
}

bool is_postgoal_replay(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_GOAL_REPLAY_INIT ||
            submode == mkb::SMD_GAME_GOAL_REPLAY_MAIN);  // does not include
                                                         // SMD_GAME_SCENARIO_RETURN
}

bool is_postgoal(mkb::SubMode submode) {
    return is_postgoal_pre_replay(submode) || is_postgoal_replay(submode);
}

bool is_postgoal_with_game_return(mkb::SubMode submode) {
    return is_postgoal(submode) || submode == mkb::SMD_GAME_SCENARIO_RETURN;
}

bool is_goal_init(mkb::SubMode submode) {
    return submode == mkb::SMD_GAME_GOAL_INIT;
}

// --- death submodes ---

bool is_fallout(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_RINGOUT_INIT || submode == mkb::SMD_GAME_RINGOUT_MAIN);
}

bool is_timeover(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_TIMEOVER_INIT || submode == mkb::SMD_GAME_TIMEOVER_MAIN);
}

bool is_story_retry_screen(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_RETRY_INIT || submode == mkb::SMD_GAME_RETRY_MAIN);
}

bool is_death_submode(mkb::SubMode submode) {
    return is_fallout(submode) || is_timeover(submode) || is_story_retry_screen(submode);
}

bool is_death_init(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_RINGOUT_INIT || submode == mkb::SMD_GAME_TIMEOVER_INIT ||
            submode == mkb::SMD_GAME_RETRY_INIT);
}

bool is_timed_death_submode(mkb::SubMode submode) {
    return is_fallout(submode) || is_timeover(submode);
}

// --- actual gameplay ---

bool is_on_stage(mkb::SubMode submode) {
    return (is_spin_in(submode) || is_gameplay(submode) || is_postgoal(submode) ||
            is_death_submode(submode));
}

bool is_on_stage_with_first_init(mkb::SubMode submode) {
    return (is_spin_in_first_init(submode) || is_on_stage(submode));
}

bool is_on_stage_with_endpoints(mkb::SubMode submode) {
    return (is_spin_in_first_init(submode) || is_on_stage(submode) ||
            is_game_scenario_return(submode));
}

// --- menu submodes ---

bool is_sel_ngc_init(mkb::SubMode submode) {
    return (submode == mkb::SMD_SEL_NGC_INIT);
}

bool is_sel_ngc_main(mkb::SubMode submode) {
    return submode == mkb::SMD_SEL_NGC_MAIN;
}

bool is_sel_ngc(mkb::SubMode submode) {
    return (submode == mkb::SMD_SEL_NGC_INIT || submode == mkb::SMD_SEL_NGC_MAIN);
}

// --- main game modes ---

bool is_main_game_mode_story(mkb::MainGameMode main_game_mode) {
    return main_game_mode == mkb::STORY_MODE;
}

bool is_main_game_mode_challenge(mkb::MainGameMode main_game_mode) {
    return main_game_mode == mkb::CHALLENGE_MODE;
}

// --- story exit game submodes ---

bool is_story_exit_game_init(mkb::SubMode submode) {
    return submode == mkb::SMD_GAME_INTR_SEL_INIT;
}

bool is_story_exit_game_message(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_INTR_SEL_INIT || submode == mkb::SMD_GAME_INTR_SEL_MAIN);
}

bool is_story_exit_game_save_prompt(mkb::SubMode submode) {
    return (submode == mkb::SMD_GAME_SUGG_SAVE_INIT || submode == mkb::SMD_GAME_SUGG_SAVE_MAIN);
}

bool is_story_exit_game(mkb::SubMode submode) {
    return (is_story_exit_game_message(submode) || is_story_exit_game_save_prompt(submode));
}

// --- combinations of non-gameplay submodes ---

bool is_stage_exit_submode(mkb::SubMode submode) {
    return (is_game_scenario_main(submode) || is_sel_ngc(submode) || is_story_exit_game(submode));
}

bool is_stage_exit_init(mkb::SubMode submode) {
    return is_game_scenario_return(submode) || is_sel_ngc_init(submode) ||
           is_story_exit_game_init(submode);
}

// --- some story mode stuff ---

bool is_story_cutscene(mkb::SubMode submode) {
    return (submode == mkb::SMD_AUTHOR_PLAY_INIT || submode == mkb::SMD_AUTHOR_PLAY_MAIN ||
            submode == mkb::SMD_AUTHOR_PLAY_STORY_INIT ||
            submode == mkb::SMD_GAME_SCENSCNPLAY_RETURN);
}

// --- 10 ball screen things using StoryModeStageSelectState ---

bool is_on_10_ball_spin_in(mkb::StoryModeStageSelectState state) {
    return state == mkb::STAGE_SELECT_INTRO_SEQUENCE;
}

bool is_idle_on_10_ball_screen(mkb::StoryModeStageSelectState state) {
    return (state == mkb::STAGE_SELECT_UNK3 ||  // idle init
            state == mkb::STAGE_SELECT_IDLE);
}

bool has_selected_stage_on_10_ball_screen_init(mkb::StoryModeStageSelectState state) {
    return state == mkb::STAGE_SELECT_UNK5;  // selected init
}

bool has_selected_stage_on_10_ball_screen(mkb::StoryModeStageSelectState state) {
    return state == mkb::STAGE_SELECT_SELECTED;
}

// --- 10 ball screen related things using ScenInfo ---

// The point of the next 3 functions is to provide safety checks for being able to access
// mkb::g_storymode_stageselect_state safely at all. We need to be careful not to access
// mkb::g_storymode_stageselect_state when not in a story mode run (we're not allowed to access it
// even on the file and name entry screens, so a main_game_mode check isn't enough)

bool is_scen_mode_10_ball_screen(mkb::ScenInfo scen_info) {
    // Some notes
    // (1) Note that this will return false a little after we select
    // a stage on the 10 ball screen but before entering a stage (I think when the next stage is
    // loaded)
    // (2) Also note that this can return true while on a stage if we do any% glitch related
    // things, so be careful with that too
    // (3) Note that this also returns true after exiting game on
    // the 10 ball screen (during the exit game menu)
    return (scen_info.mode == mkb::DMD_SCEN_RETURN_INIT ||
            scen_info.mode == mkb::DMD_SCEN_SEL_FLOOR_INIT ||
            scen_info.mode == mkb::DMD_SCEN_SEL_FLOOR_MAIN);
}

bool is_scen_mode_on_stage(mkb::ScenInfo scen_info) {
    // Returns true on a stage and the tail end of the 10 ball screen (usually, see exceptions
    // explained above)
    return (scen_info.mode == mkb::DMD_SCEN_GAME_INIT || scen_info.mode == mkb::DMD_SCEN_GAME_MAIN);
}

// The next function gets rid of many of the exceptions described above by adding an additional
// submode check
bool is_on_10_ball_screen(mkb::SubMode sub_mode, mkb::ScenInfo scen_info) {
    return (is_game_scenario(sub_mode) &&
            (is_scen_mode_10_ball_screen(scen_info) || is_scen_mode_on_stage(scen_info)));
}

int get_story_stage_id_from_scen_info(mkb::ScenInfo scen_info) {
    // useful if we want to get the stage id for the stage we're currently hovering over
    // on the 10 ball screen
    return mkb::get_story_mode_stage_id(scen_info.world, scen_info.world_stage);
}

// --- story file select/name entry screens ---

bool is_storymode_file_screen_init(mkb::ScenInfo scen_info) {
    return (scen_info.mode == mkb::DMD_SCEN_LOADGAME_INIT);
}

bool is_storymode_file_screen_main(mkb::ScenInfo scen_info) {
    return (scen_info.mode == mkb::DMD_SCEN_LOADGAME_MAIN);
}

bool is_storymode_file_screen(mkb::ScenInfo scen_info) {
    return is_storymode_file_screen_init(scen_info) || is_storymode_file_screen_main(scen_info);
}

bool is_storymode_name_entry_init(mkb::ScenInfo scen_info) {
    // For some reason causes crashes when you check this during the file screen init
    return scen_info.mode == mkb::DMD_SCEN_ENTRY_INIT;
}

bool is_storymode_name_entry_screen_main(mkb::ScenInfo scen_info) {
    return (scen_info.mode == mkb::DMD_SCEN_ENTRY_MAIN);
}

// --- titlescreen related things ---

bool is_titlescreen_main(mkb::SubMode submode) {
    return (submode == mkb::SMD_ADV_TITLE_MAIN);
}

// --- misc utility functions ---

bool is_paused() {
    // pre relocate_addr version: *reinterpret_cast<u32*>(0x805BC474) & 8
    // note that checking mkb::g_some_other_flags == mkb::OF_GAME_PAUSED won't work, I think
    // g_some_other_flags is a bitfield?
    bool paused_now = (mkb::g_some_other_flags & mkb::OF_GAME_PAUSED);
    return paused_now;
}

// general utility function to get the theme id of a stage from its stage id
u16 theme_id_from_stage_id(int stage_id) {
    return static_cast<u16>(mkb::get_stage_world_theme(stage_id));
}

// --- Story Mode Status Functions ---

// Get the clear count of the current world we're on. Some notes:
// (1) mkb::get_world_unbeaten_stage_count() only increments after the game scenario return
// submode finishes
// (2) After completing a world, this function returns a value of 10 until the 10 ball screen
// of the next world (at which point it reverts to 0)
u16 get_clear_count_for_world() {
    return mkb::get_world_unbeaten_stage_count(mkb::scen_info.world);
}

u16 get_storymode_total_clear_count() {
    u16 total = 0;
    for (u16 k = 0; k < WORLD_COUNT; k++) {
        total += mkb::get_world_unbeaten_stage_count(k);
    }
    return total;
}

}  // namespace mode
