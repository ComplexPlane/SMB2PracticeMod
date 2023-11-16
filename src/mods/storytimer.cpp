#include "storytimer.h"

#include "mkb/mkb.h"

#include "mods/freecam.h"
#include "systems/assembly.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

namespace storytimer {

enum class TimerOptions {
    DontShow = 0,
    AlwaysShow = 1,
    BetweenWorlds = 2,
    EndOfRun = 3,
};

static u32 s_loadless_story_timer;
struct TimerGroup {
    u32 segment;
    u32 full_world;
    u32 spin_in;
    u32 spin_in_correction;
    u32 gameplay;
    u32 postgoal;
    u32 stage_select;
    u32 game_scenario_return_correction;
    u32 world_start_correction;
    u32 exit_game;
    u32 fallout;
    u32 timeover;
    u32 last_stage_postgoal;
};
static TimerGroup s_timer_group[10];
static bool s_in_story;
static bool s_is_between_worlds;
static bool s_is_run_complete;
static bool s_can_lower_stage_counter;
static bool s_start_stage_fade_out_timer;
static u32 s_stage_fade_out_timer;
static u32 s_prev_completed_stage_count;
static s32 s_completed_stages;
static s32 s_completed_stages_world[10];
static u32 s_segment_timer[10];  // IW timer for world k
static u32 s_split[10];  // s_split[k] is the loadless time on tape break of the 10th stage of world
                         // k
static u32 s_segment_start_time[10];  // the loadless time at the start of world k, used to
                                      // calculate s_segment_timer[k]
static bool s_can_change_segment_start_time[10];
static bool s_display_story_timer;
static bool s_display_segment_timer;
static u32 s_fullgame_timer_location_y;
static u32 s_segment_timer_location_y;
static constexpr s32 FULLGAME_TIMER_LOCATION_X = 18 + 24;
static constexpr s32 FULLGAME_TIMER_TEXT_OFFSET = 56;
static constexpr s32 SEGMENT_TIMER_LOCATION_X = 30 + 24;
static constexpr s32 SEGMENT_TIMER_TEXT_OFFSET = 44;
static constexpr s32 IW_TIME_LOCATION_X = 42 + 24;
static constexpr s32 IW_TIME_TEXT_OFFSET = 32;
static constexpr s32 STAGE_FADE_OUT_TIME = 49;
static constexpr u32 WORLD_START_CORRECTION = 2;
static u32 s_dummy;
static u32 s_dummy_2;
static u32 s_dummy_3;

void tick() {
    // before starting the run, there are several values we zero on the file select screen (this
    // serves to reset the timer) to do: in the future, have the timer not reset unless the file's
    // data is reset (either manually or by using the IW move up/down feature)

    if (mkb::scen_info.mode == 5 || mkb::scen_info.mode == 21) {
        // zero the timer on the file select screen & name entry screen, and set the number of
        // completed stages to 0
        s_can_lower_stage_counter = false;
        s_loadless_story_timer = 0;
        s_completed_stages = 0;
        s_start_stage_fade_out_timer = false;
        s_stage_fade_out_timer = 0;
        for (s32 k = 0; k < 10; k++) {
            s_timer_group[k] = {};
            s_completed_stages_world[k] = 0;
        }
    }

    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    // for later use, it's useful to record how many stages we've completed
    // increment the completed stages by 1 during the init
    // need to check that the game is not paused to ensure the counter only goes up by 1
    for (s32 k = 0; k < 10; k++) {
        if (mkb::scen_info.world == k) {
            if (!paused_now && mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
                s_completed_stages_world[k] += 1;
                s_can_lower_stage_counter = true;
            }

            // if you retry after SMD_GAME_GOAL_INIT but before returning to the stage select
            // screen, lower the counter by exactly 1
            if (s_can_lower_stage_counter && mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
                s_completed_stages_world[k] += -1;
                s_can_lower_stage_counter = false;
            }

            // once you leave a stage, set this to false to ensure the completed stage count is not
            // lowered when entering spin in on the next stage
            if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN) {
                s_can_lower_stage_counter = false;
            }
        }
        s_completed_stages += s_completed_stages_world[k];
    }

    // for later, it's useful to record what submodes correspond to spin in, gameplay, etc.
    // splitting up the timer this way also makes it easier in the future if I decide to implement
    // features such as menuing timeloss

    bool is_on_spin_in = 0;
    bool is_on_stage_select = 0;
    bool is_on_gameplay = 0;
    bool is_on_exit_game = 0;
    bool is_postgoal = 0;
    bool is_on_fallout = 0;
    bool is_timeover = 0;
    bool in_story = 0;

    // submodes during spin in
    if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_READY_MAIN) {
        is_on_spin_in = true;
    } else {
        is_on_spin_in = false;
    }

    // story mode states entered during the story select screen
    if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE ||
        mkb::g_storymode_stageselect_state == 3 ||
        mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_IDLE ||
        mkb::g_storymode_stageselect_state == 5 ||
        mkb::g_storymode_stageselect_state == mkb::STAGE_SELECTED) {
        // 3, 5 unlabelled inits
        is_on_stage_select = true;
    } else {
        is_on_stage_select = false;
    }

    // submodes during gameplay
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT || mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
        is_on_gameplay = true;
    } else {
        is_on_gameplay = false;
    }

    // submodes entered when exiting game
    if (mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_MAIN) {
        is_on_exit_game = true;
    } else {
        is_on_exit_game = false;
    }

    // submodes entered when breaking the goal tape
    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN) {
        is_postgoal = true;
    } else {
        is_postgoal = false;
    }

    // submodes for the fallout and y/n screen
    if (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN || mkb::sub_mode == mkb::SMD_GAME_RETRY_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_RETRY_MAIN) {
        is_on_fallout = true;
    } else {
        is_on_fallout = false;
    }

    // submodes during timeover
    if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
        is_timeover = true;
    } else {
        is_timeover = false;
    }

    if (mkb::main_game_mode == mkb::STORY_MODE || mkb::sub_mode == mkb::SMD_AUTHOR_PLAY_INIT ||
        mkb::sub_mode == mkb::SMD_AUTHOR_PLAY_MAIN) {
        in_story = true;
    } else {
        in_story = false;
    }

    // this code is used to halt the timer once the screen becomes completely white when stage
    // selecting out of a level
    if (mkb::pausemenu_type == mkb::PMT_STORY_PLAY &&
        mkb::g_current_focused_pause_menu_entry == 4 && pad::button_pressed(mkb::PAD_BUTTON_A) &&
        is_postgoal) {
        // stage select is on line 4 of the pause menu (top line is line 0)
        s_start_stage_fade_out_timer = true;
    } else if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) {
        s_start_stage_fade_out_timer = false;
    }
    if (s_start_stage_fade_out_timer) {
        s_stage_fade_out_timer += 1;
    } else {
        s_stage_fade_out_timer = 0;
    }

    if (in_story == true) {
        for (s32 k = 0; k < 10; k++) {
            if (mkb::scen_info.world == k) {
                if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT) {
                    // need to add 1 additional frame to the timer during spin in
                    // putting this code before the code below for s_loadless_story_timer makes the
                    // timer tick up more naturally when transitioning from the 10 ball screen to
                    // spin in, more specifically, this prevents the timer from skipping ahead for a
                    // few frames, then pausing for a few frames to even itself out (I don't
                    // understand why that happens)
                    s_timer_group[k].spin_in_correction += 1;
                }

                s_timer_group[k].full_world =
                    s_timer_group[k].spin_in + s_timer_group[k].spin_in_correction +
                    s_timer_group[k].gameplay + s_timer_group[k].postgoal +
                    s_timer_group[k].stage_select +
                    s_timer_group[k].game_scenario_return_correction + s_timer_group[k].exit_game +
                    s_timer_group[k].fallout + s_timer_group[k].timeover +
                    s_timer_group[k].world_start_correction + s_timer_group[k].last_stage_postgoal;

                if (s_completed_stages_world[k] <= 9) {
                    s_timer_group[k].segment = s_timer_group[k].full_world;
                } else if (s_completed_stages_world[k] == 10 &&
                           mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT && !paused_now) {
                    s_timer_group[k].segment += -2;
                }

                if (is_on_spin_in) {
                    // increment the timer every frame during spin in
                    s_timer_group[k].spin_in += 1;
                }
                if (is_on_gameplay) {
                    // increment the timer every frame during gameplay
                    s_timer_group[k].gameplay += 1;
                }
                if (is_postgoal && s_stage_fade_out_timer <= STAGE_FADE_OUT_TIME) {
                    // increment the timer every frame after game goal init happens; once you press
                    // stage select, a separate 49 frame timer is started (fade out from stage
                    // select to the first completely white frame takes 49 frames). once the timer
                    // hits 49 frames, stop incrementing the timer until the 10 ball screen starts
                    // spinning in
                    if (mkb::get_world_unbeaten_stage_count(k) <= 8) {
                        s_timer_group[k].postgoal += 1;
                    } else if (mkb::get_world_unbeaten_stage_count(k) == 9) {
                        s_timer_group[k].last_stage_postgoal += 1;
                    }
                }
                if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE ||
                    mkb::g_storymode_stageselect_state == 3 ||
                    mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_IDLE) {
                    // increment the timer every frame on the story mode select screen until the a
                    // press input; we do not include the transition time after pressing a
                    // afterwards
                    //  even ignoring completely white frames, the time spent on
                    //  mkb::g_storymode_stageselect_state == mkb::STAGE_SELECTED can be highly
                    //  variable (up to over 40 frames sometimes!), so for the purpose of a loadless
                    //  timer, it makes sense to cut this out from the timer
                    s_timer_group[k].stage_select += 1;
                }
                if (is_on_exit_game) {
                    // increment the timer every frame on the exit game screen
                    s_timer_group[k].exit_game += 1;
                }
                if (is_on_fallout) {
                    // increment the timer every frame during the fallout sequence and y/n screen
                    s_timer_group[k].fallout += 1;
                }
                if (is_timeover) {
                    // increment the timer every frame during the timeover sequence
                    s_timer_group[k].timeover += 1;
                }

                if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN &&
                    s_completed_stages % 10 != 0) {
                    // need to add 2 frames to the timer when stage selecting to the 10 ball screen,
                    // but don't correct if on the last stage of a world since the next frame the
                    // timer should increment on is covered by s_world_start_timer_correction
                    s_timer_group[k].game_scenario_return_correction += 2;
                }

                // this only gets set to 2 when you enter world k
                if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) {
                    s_timer_group[k].world_start_correction = 2;
                }
            }
        }
    }
    // s_split[k] is just the fullgame time at tape break on the last stage of world k; to calculate
    // that, we just add the corresponding world timers
    u32 sum[10] = {};
    for (s32 k = 1; k < 10; k++) {
        for (s32 j = 0; j < k; j++) {
            sum[k] += s_timer_group[j].full_world;
        }
        s_split[k] = sum[k] + s_timer_group[k].segment;
    }
    s_split[0] = s_timer_group[0].segment;
    s_loadless_story_timer = s_split[9];

    /*
    u32 sum = 0;
    for (s32 k = 0; k < 9; k++) {
        sum += s_timer_group[k].full_world;  // doesn't work :(
        for (s32 j = 0; j < k; j++) {
            s_split[k] += s_timer_group[j].full_world + s_timer_group[k].segment;
        }
    }
    s_loadless_story_timer = sum + s_timer_group[9].segment;
    */

    /*
    s_loadless_story_timer = s_timer_group[0].full_world + s_timer_group[1].full_world +
                             s_timer_group[2].full_world + s_timer_group[3].full_world +
                             s_timer_group[4].full_world + s_timer_group[5].full_world +
                             s_timer_group[6].full_world + s_timer_group[7].full_world +
                             s_timer_group[8].full_world + s_timer_group[9].segment;
    */

    // s_loadless_story_timer = s_split[9];

    // it is useful to know when we are between worlds (for the "between worlds" option in the menu)
    /*
    bool is_between_worlds = 0;
    if ((s_completed_stages % 10 == 0) && s_completed_stages != 0 &&
        mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
        s_is_between_worlds = true;
        is_between_worlds = true;
    } else if ((s_completed_stages % 10 == 0 &&
                mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) ||
               s_completed_stages % 10 != 0) {
        // no longer "between worlds" if you enter the next worlds 10 ball screen, or if you break
        // the tape on the last stage of the current world, but retry
        s_is_between_worlds = false;
        is_between_worlds = false;
    }
    */
}

void disp() {
    if ((mkb::main_game_mode != mkb::STORY_MODE && mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_INIT &&
         mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_MAIN) ||
        freecam::should_hide_hud()) {
        return;
    }

    bool is_between_worlds = 0;
    if ((s_completed_stages % 10 == 0) && s_completed_stages != 0 &&
        mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
        is_between_worlds = true;
    } else if ((s_completed_stages % 10 == 0 &&
                mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) ||
               s_completed_stages % 10 != 0) {
        // no longer "between worlds" if you enter the next worlds 10 ball screen, or if you break
        // the tape on the last stage of the current world, but retry
        is_between_worlds = false;
    }

    bool is_run_complete = 0;
    is_run_complete = (mkb::scen_info.world == 9 && s_completed_stages_world[9] == 10);

    // move the positions of the fullgame and segment timers if the death counter is on
    u32 fullgame_timer_location_y = 2;
    if (pref::get(pref::BoolPref::ShowDeathCounter)) {
        fullgame_timer_location_y = 3;
    } else {
        fullgame_timer_location_y = 2;
    }

    bool display_story_timer = 0;
    switch (TimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions))) {
        case TimerOptions::AlwaysShow:
            display_story_timer = true;
            break;
        case TimerOptions::BetweenWorlds:
            display_story_timer = is_between_worlds;
            break;
        case TimerOptions::EndOfRun:
            display_story_timer = is_run_complete;
            break;
        case TimerOptions::DontShow:
            display_story_timer = false;
            break;
    }

    // if the fullgame timer and death counter is off but the segment timer is on, move the segment
    // timer to the top line; if either the fullgame timer or death counter are on but not both are
    // on, move it to the 2nd line, if all 3 are enabled, put it on the 3rd line
    u32 segment_timer_location_y = 2;
    if (!display_story_timer && !pref::get(pref::BoolPref::ShowDeathCounter)) {
        segment_timer_location_y = 2;
    } else if (!display_story_timer || !pref::get(pref::BoolPref::ShowDeathCounter)) {
        segment_timer_location_y = 3;
    } else {
        segment_timer_location_y = 4;
    }

    if (display_story_timer) {
        timerdisp::draw_timer(FULLGAME_TIMER_LOCATION_X, fullgame_timer_location_y,
                              FULLGAME_TIMER_TEXT_OFFSET, "Time:", s_loadless_story_timer, 0, false,
                              false, draw::WHITE);
    }

    switch (TimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions))) {
        case TimerOptions::AlwaysShow:
            for (s32 k = 0; k < 10; k++) {
                if (mkb::scen_info.world == k && !is_run_complete) {
                    timerdisp::draw_timer(SEGMENT_TIMER_LOCATION_X, segment_timer_location_y,
                                          SEGMENT_TIMER_TEXT_OFFSET,
                                          "Seg:", s_timer_group[k].segment, 0, false, false,
                                          draw::WHITE);
                }
            }
            break;
        case TimerOptions::BetweenWorlds:
            for (s32 k = 0; k < 10; k++) {
                if (is_between_worlds && mkb::scen_info.world == k && k != 9) {
                    timerdisp::draw_timer(SEGMENT_TIMER_LOCATION_X, segment_timer_location_y,
                                          SEGMENT_TIMER_TEXT_OFFSET,
                                          "Seg:", s_timer_group[k].segment, 0, false, false,
                                          draw::WHITE);
                }
            }
            break;
        case TimerOptions::DontShow:
            s_display_segment_timer = false;
            break;
    }

    // if the segment timer is enabled in any capacity, show all 10 split times + iw times after the
    // tape is broken on the last stage
    if (TimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) != TimerOptions::DontShow) {
        if (s_completed_stages_world[9] == 10) {
            // I'm so sorry :(
            // I don't know how to get the text to show "Wk" where k ranges in a for loop
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y, IW_TIME_TEXT_OFFSET,
                                  "W1:", s_split[0], s_timer_group[0].segment, true, false,
                                  draw::WHITE);
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 1,
                                  IW_TIME_TEXT_OFFSET, "W2:", s_split[1], s_timer_group[1].segment,
                                  true, false, draw::WHITE);
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 2,
                                  IW_TIME_TEXT_OFFSET, "W3:", s_split[2], s_timer_group[2].segment,
                                  true, false, draw::WHITE);
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 3,
                                  IW_TIME_TEXT_OFFSET, "W4:", s_split[3], s_timer_group[3].segment,
                                  true, false, draw::WHITE);
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 4,
                                  IW_TIME_TEXT_OFFSET, "W5:", s_split[4], s_timer_group[4].segment,
                                  true, false, draw::WHITE);
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 5,
                                  IW_TIME_TEXT_OFFSET, "W6:", s_split[5], s_timer_group[5].segment,
                                  true, false, draw::WHITE);
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 6,
                                  IW_TIME_TEXT_OFFSET, "W7:", s_split[6], s_timer_group[6].segment,
                                  true, false, draw::WHITE);
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 7,
                                  IW_TIME_TEXT_OFFSET, "W8:", s_split[7], s_timer_group[7].segment,
                                  true, false, draw::WHITE);
            timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 8,
                                  IW_TIME_TEXT_OFFSET, "W9:", s_split[8], s_timer_group[8].segment,
                                  true, false, draw::WHITE);
            // use segment timer spacing for w10 since "W10" is 3 characters long, not 2
            timerdisp::draw_timer(SEGMENT_TIMER_LOCATION_X, segment_timer_location_y + 9,
                                  SEGMENT_TIMER_TEXT_OFFSET, "W10:", s_split[9],
                                  s_timer_group[9].segment, true, false, draw::WHITE);
        }
    }

    // show warning on the name entry screen if no timers are on (if the toggle for the warning is
    // turned on)
    if (pref::get(pref::BoolPref::StoryTimerWarning) &&
        TimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions)) == TimerOptions::DontShow &&
        TimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) == TimerOptions::DontShow &&
        mkb::scen_info.mode == 21) {
        // mkb::scen_info.mode 21 is the name entry screen, not sure if it has a name in ghidra
        draw::debug_text(460, 425, draw::RED, "Timer Not On!");
    }

    // debugging

    if (s_is_between_worlds == true) {
        s_dummy = 1;
    } else {
        s_dummy = 0;
        /* things tested that didn't work for exit game so far
        SMD_GAME_FORCE_EXIT_MAIN=93
        SMD_GAME_FORCE_OVER_MAIN=96
        SMD_GAME_OVER_POINT_MAIN=86

        things that do work
        SMD_GAME_SUGG_SAVE_MAIN, doesn't include playpoint text, only the save data question after
        SMD_GAME_INTR_SEL_MAIN is the playpoint text

        fallout submode testing
        test: 50, 51, 58, 59, 90, 91, 48, 49,

        SMD_GAME_READY_INIT=48,
        SMD_GAME_READY_MAIN=49,
        SMD_GAME_RINGOUT_INIT=58,
        SMD_GAME_RINGOUT_MAIN=59
        SMD_GAME_RETRY_INIT=90,
        SMD_GAME_RETRY_MAIN=91

        conclusion: ringout = fallout submode, game retry init/main = y/n menu

        FOR LATER, TRY THESE:
        SMD_GAME_SCENSCNPLAY_RETURN=94 (missing frame on world entry?)
        SMD_AUTHOR_PLAY_INIT=247,
        SMD_AUTHOR_PLAY_MAIN=248,
        SMD_AUTHOR_PLAY_STORY_INIT=249,

        */
    }
    if (pad::button_pressed(mkb::PAD_BUTTON_A) == true) {
        s_dummy_2 = 1;
    } else {
        s_dummy_2 = 0;
    }

    /*
    if (pad::button_pressed(mkb::PAD_BUTTON_DOWN)) {
        s_completed_stages = 91;
    }
    */

    if (TimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions)) == TimerOptions::AlwaysShow) {
        timerdisp::draw_timer(380, 0, 44, "dbg:", static_cast<s32>(s_timer_group[0].segment), 1,
                              false, false, draw::WHITE);
        timerdisp::draw_timer(380, 1, 44,
                              "dbg:", static_cast<s32>(60 * s_completed_stages_world[0]), 1, false,
                              true, draw::WHITE);
        timerdisp::draw_timer(380, 2, 44,
                              "dbg:", static_cast<s32>(60 * s_completed_stages_world[1]), 1, false,
                              true, draw::WHITE);
    }
    // mkb::scen_info.world
    // 10*mkb::scen_info.world+mkb::get_world_unbeaten_stage_count(mkb::scen_info.world)
    // mkb::scen_info.next_world == 10
}

}  // namespace storytimer
