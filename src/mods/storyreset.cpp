#include "storyreset.h"

#include "gotostory.h"
#include "systems/goal.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/mode.h"

namespace storyreset {

// This file is used by both storytimer.cpp and deathcounter.cpp to determine when to reset the run
// since we want to allow the possibility for accidental exit games taking us back to the menus but
// still have the run be considered "active"

static bool s_is_run_active = false;
static u8 s_active_save_file_idx = 0;  // for handling resetting
static u8 s_last_active_world = 0;     // for handling resetting

bool is_run_active() {
    return s_is_run_active;
}
u8 get_active_file_index() {
    return s_active_save_file_idx;
}
u8 get_active_world() {
    return s_last_active_world;
}

void set_run_active_status(bool is_active) {
    // setter that is called in storytimer.cpp so that deathcounter.cpp can know whether the run is
    // active or not without having access to the stuff in storytimer.cpp (checking for loadless
    // time being nonzero works to see if a run is active, but checking for deaths being nonzero
    // doesn't work)
    s_is_run_active = is_active;
}

void reset_active_run_info() {
    s_active_save_file_idx = 0;
    s_last_active_world = 0;
}

// This only gets called during the exit game init
// We store the world we were on + what file we were on so that we're able to properly handle
// resetting on the file screen
void record_run_status() {
    s_last_active_world = mkb::scen_info.world;
    s_active_save_file_idx = mkb::scen_info.save_file_idx;
}

// --- checks for if we should reset ---

bool detect_selecting_wrong_file() {
    return mkb::data_select_menu_state == mkb::DSMS_OPEN_DATA &&
           mkb::selected_story_file_idx != s_active_save_file_idx;
}

// Catches using the IW picker on our current active file
bool detect_wrong_world_on_active_file() {
    mkb::StoryModeSaveFile active_file = mkb::storymode_save_files[s_active_save_file_idx];
    bool wrong_world = active_file.current_world != s_last_active_world;
    bool active_file_empty = active_file.playtime_in_frames == 0;
    return wrong_world || active_file_empty;
}

bool should_reset_on_file_screen() {
    if (mode::is_main_game_mode_story(mkb::main_game_mode) &&
        mode::is_storymode_file_screen_main(mkb::scen_info)) {
        return detect_wrong_world_on_active_file() || detect_selecting_wrong_file();
    }
    return false;
}

// If we select challenge mode, practice mode, or go back to the main menu
bool is_on_wrong_menu() {
    if (mode::is_sel_ngc_main(mkb::sub_mode)) {
        mkb::MenuScreenID menu = mkb::g_currently_visible_menu_screen;
        return (menu == mkb::MENUSCREEN_NUMBER_OF_PLAYERS ||
                menu == mkb::MENUSCREEN_CHARACTER_SELECT_1 || menu == mkb::MENUSCREEN_MODE_SELECT);
    }
    return false;
}

// We also need this in case we use challenge mode segments to enter challenge mode without going
// through the menus
bool in_challenge_mode() {
    return mode::is_main_game_mode_challenge(mkb::main_game_mode);
}

bool used_go_to_story() {
    return gotostory::get_gotostory_state() != gotostory::State::Default;
}

bool should_reset_completed_run() {
    if (mode::is_sel_ngc(mkb::sub_mode) || mode::is_titlescreen_main(mkb::sub_mode)) {
        return goal::is_run_complete();
    }
    return false;
}

bool should_reset_run() {
    return should_reset_on_file_screen() || is_on_wrong_menu() || used_go_to_story() ||
           in_challenge_mode() || should_reset_completed_run();
}

void tick() {
    if (mode::is_main_game_mode_story(mkb::main_game_mode) &&
        mode::is_story_exit_game_init(mkb::sub_mode)) {
        record_run_status();
    }
}

// --- stuff for handling displaying the run reset message ---

bool all_loadless_timer_prefs_off() {
    StoryDisplayOptions fullgame_pref =
        StoryDisplayOptions(pref::get(pref::Pref::FullgameTimerOptions));
    StoryDisplayOptions segment_pref =
        StoryDisplayOptions(pref::get(pref::Pref::SegmentTimerOptions));
    bool breakdown_pref = pref::get(pref::Pref::ShowRunBreakdown);

    return fullgame_pref == StoryDisplayOptions::DontShow &&
           segment_pref == StoryDisplayOptions::DontShow && !breakdown_pref;
}

bool death_counter_pref_off() {
    StoryDisplayOptions death_counter_pref =
        StoryDisplayOptions(pref::get(pref::Pref::DeathCounterDisplayOptions));
    return death_counter_pref == StoryDisplayOptions::DontShow;
}

// During an accidental full exit game to the menu mid-run, the only time a timer/death counter is
// visible is if "always show" is turned on for that display
bool displaying_on_menus_during_accidental_exit_game() {
    StoryDisplayOptions fullgame_pref =
        StoryDisplayOptions(pref::get(pref::Pref::FullgameTimerOptions));
    StoryDisplayOptions segment_pref =
        StoryDisplayOptions(pref::get(pref::Pref::SegmentTimerOptions));
    StoryDisplayOptions death_counter_pref =
        StoryDisplayOptions(pref::get(pref::Pref::DeathCounterDisplayOptions));
    return fullgame_pref == StoryDisplayOptions::AlwaysShow ||
           segment_pref == StoryDisplayOptions::AlwaysShow ||
           death_counter_pref == StoryDisplayOptions::AlwaysShow;
}

bool is_silent_reset_type() {
    // ie, any reset that's not triggered by gotostory or on the menu after completing a run
    return should_reset_on_file_screen() || is_on_wrong_menu();
}

void display_reset_run_message() {
    if (pref::get(pref::Pref::HideRunResetMessage)) {
        return;
    }

    // If the hide run reset message pref is off, we still want to be "minimal" with displaying it
    // So, we only display a reset message if:
    // (1) it's a less obvious reset trigger (ie not go to story, or if we've already completed the
    // run)
    // (2) at least one timer/death counter pref is on (don't bother displaying a reset message
    // if everything is turned off)
    // (3) the player has "always show" turned on for at least one of
    // the timers/death counter. The idea here is if always show is turned on and if the timer gets
    // zero-ed, it's obvious that the run was reset and no message needs to be displayed

    if (!is_silent_reset_type()) {
        return;
    }

    if (all_loadless_timer_prefs_off() && death_counter_pref_off()) {
        return;
    }

    if (displaying_on_menus_during_accidental_exit_game()) {
        return;
    }

    draw::notify(draw::WHITE, "Run Was Reset");
}

}  // namespace storyreset