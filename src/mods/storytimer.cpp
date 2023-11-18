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
    u32 segment;     // the time taken to complete a world up until tape break on the last stage
    u32 full_world;  // the time taken to complete a world until the fade to white on the last stage
    u32 split;       // the time from the start of the run until tape break of the last stage of the
                     // world
    u32 gameplay;
    u32 non_gameplay;
    u32 world_start_correction;
};
static TimerGroup s_timer_group[10];  // each world has its own TimerGroup structure
static bool s_can_lower_stage_counter;
static bool s_start_stage_fade_out_timer;
static u32 s_stage_fade_out_timer;
static s32 s_completed_stages;            // the completed stages for the whole run
static s32 s_completed_stages_world[10];  // the completed stages in world k
static bool s_display_segment_timer;
static constexpr s32 FULLGAME_TIMER_LOCATION_X = 18 + 24;
static constexpr s32 FULLGAME_TIMER_TEXT_OFFSET = 56;
static constexpr s32 SEGMENT_TIMER_LOCATION_X = 30 + 24;
static constexpr s32 SEGMENT_TIMER_TEXT_OFFSET = 44;
static constexpr s32 IW_TIME_LOCATION_X = 42 + 24;
static constexpr s32 IW_TIME_TEXT_OFFSET = 32;
static constexpr s32 STAGE_FADE_OUT_TIME = 49;
static constexpr u32 WORLD_START_CORRECTION = 2;

u32 get_completed_stagecount() { return s_completed_stages; }

void tick() {
    // before starting the run, there are several values we zero on the file select and name entry
    // screen (this serves to reset the timer)
    if (mkb::scen_info.mode == 5 || mkb::scen_info.mode == 21) {
        // 5 is the file select screen, 21 is the name entry screen
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

    // submodes during spin in
    bool is_on_spin_in =
        (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_INIT ||
         mkb::sub_mode == mkb::SMD_GAME_READY_MAIN);

    // submodes during gameplay
    bool is_on_gameplay =
        (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT || mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN);

    // submodes entered when exiting game
    bool is_on_exit_game = (mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT ||
                            mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_MAIN ||
                            mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_INIT ||
                            mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_MAIN);

    // submodes entered when breaking the goal tape
    bool is_postgoal =
        (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN ||
         mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT ||
         mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN ||
         mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN);

    // submodes for the fallout and y/n screen
    bool is_on_fallout =
        (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
         mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN || mkb::sub_mode == mkb::SMD_GAME_RETRY_INIT ||
         mkb::sub_mode == mkb::SMD_GAME_RETRY_MAIN);

    // submodes during timeover
    bool is_timeover = (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
                        mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN);

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

    for (s32 k = 0; k < 10; k++) {
        if (mkb::scen_info.world == k) {
            if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT) {
                // need to add 1 additional frame to the timer during spin in
                // putting this code before the code below for s_timer_group[k].full_world makes
                // the timer tick up more naturally when transitioning from the 10 ball screen
                // to spin in, more specifically, this prevents the timer from skipping ahead
                // for a few frames, then pausing for a few frames to even itself out (I don't
                // understand why that happens)
                s_timer_group[k].non_gameplay += 1;
            }

            s_timer_group[k].full_world = s_timer_group[k].gameplay +
                                          s_timer_group[k].non_gameplay +
                                          s_timer_group[k].world_start_correction;

            if (s_completed_stages_world[k] <= 9) {
                s_timer_group[k].segment = s_timer_group[k].full_world;
            } else if (s_completed_stages_world[k] == 10 &&
                       mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT && !paused_now) {
                // stop world k's segment timer on tape break of the last stage of the world; we
                // need to correct by 2f since SMD_GAME_GOAL_INIT happens 2f after tape break
                s_timer_group[k].segment += -2;
            }

            if (is_on_spin_in || is_on_exit_game || is_on_fallout || is_timeover) {
                // increment the timer during every frame on spin in, exit game, fallout, and
                // timeover
                s_timer_group[k].non_gameplay += 1;
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
                s_timer_group[k].non_gameplay += 1;
            }
            if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE ||
                mkb::g_storymode_stageselect_state == 3 ||
                mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_IDLE) {
                // increment the timer every frame on the story mode stage select screen until
                // the a press input; we do not include the transition time after pressing a
                // afterwards. even ignoring completely white frames, the time spent on
                //  mkb::g_storymode_stageselect_state == mkb::STAGE_SELECTED can be highly
                //  variable (up to over 40 frames sometimes!), so for the purpose of a loadless
                //  timer, it makes sense to cut this out from the timer
                s_timer_group[k].non_gameplay += 1;
            }

            if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN && s_completed_stages % 10 != 0) {
                // need to add 2 frames to the timer when stage selecting to the 10 ball screen,
                // but don't correct if on the last stage of a world since the next frame the
                // timer should increment on is covered by s_world_start_timer_correction
                s_timer_group[k].non_gameplay += 2;
            }

            // this only gets set to 2 when you enter world k; this is needed since otherwise
            // the timer will not capture the first 2f of the 10 ball spin in when entering
            // a new world
            if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) {
                s_timer_group[k].world_start_correction = 2;
            }
        }
    }
    // s_timer_group[k].split is just the fullgame time at tape break on the last stage of world k;
    // to calculate this, just add s_timer_group[j].full_world (j<k) for the previous worlds to the
    // current world's segment timer
    u32 sum[10] = {};
    for (s32 k = 1; k < 10; k++) {
        for (s32 j = 0; j < k; j++) {
            sum[k] += s_timer_group[j].full_world;
        }
        s_timer_group[k].split = sum[k] + s_timer_group[k].segment;
    }
    s_timer_group[0].split = s_timer_group[0].segment;
    s_loadless_story_timer = s_timer_group[9].split;
}

void disp() {
    if ((mkb::main_game_mode != mkb::STORY_MODE && mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_INIT &&
         mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_MAIN) ||
        freecam::should_hide_hud()) {
        return;
    }

    bool is_between_worlds = false;
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

    bool is_run_complete = (mkb::scen_info.world == 9 && s_completed_stages_world[9] == 10);

    // move the positions of the fullgame if the death counter is on
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

    if (display_story_timer) {
        timerdisp::draw_timer(FULLGAME_TIMER_LOCATION_X, fullgame_timer_location_y,
                              FULLGAME_TIMER_TEXT_OFFSET, "Time:", s_loadless_story_timer, 0, false,
                              false, draw::WHITE);
    }

    // move the position of the segment timer depending on if the death counter and fullgame timer
    // is on; if the fullgame timer and death counter is off but the segment timer is on, move the
    // segment timer to the top line; if either the fullgame timer or death counter are on but not
    // both are on, move it to the 2nd line, if all 3 are enabled, put it on the 3rd line
    u32 segment_timer_location_y = 2;
    if (!display_story_timer && !pref::get(pref::BoolPref::ShowDeathCounter)) {
        segment_timer_location_y = 2;
    } else if (!display_story_timer || !pref::get(pref::BoolPref::ShowDeathCounter)) {
        segment_timer_location_y = 3;
    } else {
        segment_timer_location_y = 4;
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
    if (TimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) != TimerOptions::DontShow &&
        is_run_complete) {
        // haven't attempted the mkb::sprintf suggestion yet
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y, IW_TIME_TEXT_OFFSET,
                              "W1:", s_timer_group[0].split, s_timer_group[0].segment, true, false,
                              draw::WHITE);
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 1, IW_TIME_TEXT_OFFSET,
                              "W2:", s_timer_group[1].split, s_timer_group[1].segment, true, false,
                              draw::WHITE);
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 2, IW_TIME_TEXT_OFFSET,
                              "W3:", s_timer_group[2].split, s_timer_group[2].segment, true, false,
                              draw::WHITE);
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 3, IW_TIME_TEXT_OFFSET,
                              "W4:", s_timer_group[3].split, s_timer_group[3].segment, true, false,
                              draw::WHITE);
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 4, IW_TIME_TEXT_OFFSET,
                              "W5:", s_timer_group[4].split, s_timer_group[4].segment, true, false,
                              draw::WHITE);
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 5, IW_TIME_TEXT_OFFSET,
                              "W6:", s_timer_group[5].split, s_timer_group[5].segment, true, false,
                              draw::WHITE);
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 6, IW_TIME_TEXT_OFFSET,
                              "W7:", s_timer_group[6].split, s_timer_group[6].segment, true, false,
                              draw::WHITE);
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 7, IW_TIME_TEXT_OFFSET,
                              "W8:", s_timer_group[7].split, s_timer_group[7].segment, true, false,
                              draw::WHITE);
        timerdisp::draw_timer(IW_TIME_LOCATION_X, segment_timer_location_y + 8, IW_TIME_TEXT_OFFSET,
                              "W9:", s_timer_group[8].split, s_timer_group[8].segment, true, false,
                              draw::WHITE);
        // use segment timer spacing for w10 since "W10" is 3 characters long, not 2
        timerdisp::draw_timer(SEGMENT_TIMER_LOCATION_X, segment_timer_location_y + 9,
                              SEGMENT_TIMER_TEXT_OFFSET, "W10:", s_timer_group[9].split,
                              s_timer_group[9].segment, true, false, draw::WHITE);
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
}

}  // namespace storytimer