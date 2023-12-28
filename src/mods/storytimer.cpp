#include "storytimer.h"

#include "mkb/mkb.h"

#include "mods/freecam.h"
#include "mods/validate.h"
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

static constexpr s32 FULLGAME_TIMER_LOCATION_X = 18 + 24;
static constexpr s32 FULLGAME_TIMER_TEXT_OFFSET = 56;
static constexpr s32 SEGMENT_TIMER_LOCATION_X = 30 + 24;
static constexpr s32 SEGMENT_TIMER_TEXT_OFFSET = 44;
static constexpr s32 IW_TIME_LOCATION_X = 42 + 24;
static constexpr s32 IW_TIME_TEXT_OFFSET = 32;
static constexpr u32 WORLD_START_CORRECTION = 2;
static constexpr s32 WORLD_COUNT = 10;
static constexpr s32 STAGES_PER_WORLD = 10;
static constexpr u32 SECOND_FRAMES = 60;
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;
static bool s_entered_goal;
struct TimerGroup {
    u32 segment;     // the time taken to complete a world up until tape break on the last stage
    u32 full_world;  // the time taken to complete a world until the fade to white on the last stage
    // char timer_str[32];
};
static TimerGroup s_timer_group[WORLD_COUNT];  // each world has its own TimerGroup structure
static s32 s_completed_stages;                 // the completed stages for the whole run

u32 get_completed_stagecount() { return s_completed_stages; }

void on_goal_entry() {
    if (!validate::has_entered_goal()) {
        // no code yet
    }
}

// before starting the run, there are several values we zero
static void reset_timer() {
    s_completed_stages = 0;
    for (s32 k = 0; k < WORLD_COUNT; k++) {
        s_timer_group[k] = {};
    }
}

void tick() {
    // reset the timer on the file select screen (scen_info.mode == 5) and the name entry screen
    // (scen_info.mode == 21)
    if (mkb::scen_info.mode == 5 || mkb::scen_info.mode == 21) {
        reset_timer();
    }

    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;

    u32 sum = 0;
    for (s32 k = 0; k < WORLD_COUNT; k++) {
        sum += mkb::get_world_unbeaten_stage_count(k);
    }
    s_completed_stages = sum;

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

    for (s32 k = 0; k < WORLD_COUNT; k++) {
        if (mkb::scen_info.world == k) {
            if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT) {
                // need to add 1 additional frame to the timer during spin in
                // putting this code before the code below for s_timer_group[k].full_world makes
                // the timer tick up more naturally when transitioning from the 10 ball screen
                // to spin in, more specifically, this prevents the timer from skipping ahead
                // for a few frames, then pausing for a few frames to even itself out (I don't
                // understand why that happens)
                s_timer_group[k].full_world += 1;
            }

            if (is_on_spin_in || is_on_exit_game || is_on_fallout || is_timeover) {
                // increment the timer during every frame on spin in, exit game, fallout, and
                // timeover
                s_timer_group[k].full_world += 1;
            }

            if (is_on_gameplay) {
                // increment the timer every frame during gameplay
                s_timer_group[k].full_world += 1;
            }
            if (is_postgoal) {
                // increment the timer every frame after game goal init happens; once you press
                // stage select, a separate 49 frame timer is started (fade out from stage
                // select to the first completely white frame takes 49 frames). once the timer
                // hits 49 frames, stop incrementing the timer until the 10 ball screen starts
                // spinning in
                s_timer_group[k].full_world += 1;
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
                s_timer_group[k].full_world += 1;
            }

            if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN &&
                s_completed_stages % STAGES_PER_WORLD != 0) {
                // need to add 1 frame to the timer when stage selecting to the 10 ball screen,
                // but don't correct if on the last stage of a world since the next frame the
                // timer should increment on is covered by s_world_start_timer_correction
                // NOTE: change back to 2 if I add the stage fade out timer back in
                s_timer_group[k].full_world += 1;
            }

            if (mkb::get_world_unbeaten_stage_count(k) < 9 ||
                (mkb::get_world_unbeaten_stage_count(k) == 9 && !is_postgoal)) {
                s_timer_group[k].segment = s_timer_group[k].full_world;
            } else if (mkb::get_world_unbeaten_stage_count(k) == 9 &&
                       mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT && !paused_now) {
                // stop world k's segment timer on tape break of the last stage of the world; we
                // need to correct by 2f since SMD_GAME_GOAL_INIT happens 2f after tape break
                s_timer_group[k].segment += -2;
            }
        }
    }
}

void disp() {
    if ((mkb::main_game_mode != mkb::STORY_MODE && mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_INIT &&
         mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_MAIN) ||
        freecam::should_hide_hud()) {
        return;
    }

    bool is_between_worlds = false;
    if ((s_completed_stages % STAGES_PER_WORLD == 9) && mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
        is_between_worlds = true;
    } else if ((s_completed_stages % STAGES_PER_WORLD == 0 &&
                mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) /*||
               s_completed_stages % STAGES_PER_WORLD != 0 */) {
        // no longer "between worlds" if you enter the next world's 10 ball screen, or if you break
        // the tape on the last stage of the current world, but retry
        is_between_worlds = false;
    }

    bool is_postgoal =
        (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN ||
         mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT ||
         mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN ||
         mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN);

    bool is_run_complete = (mkb::scen_info.world == 9 &&
                            ((mkb::get_world_unbeaten_stage_count(9) == 9 && is_postgoal) ||
                             mkb::get_world_unbeaten_stage_count(9) == 10));

    // move the position of the fullgame timer if the death counter is on
    u32 fullgame_timer_location_y = 2;
    if (pref::get(pref::BoolPref::ShowDeathCounter)) {
        fullgame_timer_location_y++;
    }

    bool display_story_timer = false;
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

    // at the start of each world, we need to correct the timer by 1 frame

    u32 full_world[10] = {};
    u32 segment[10] = {};

    for (s32 k = 0; k < WORLD_COUNT; k++) {
        if (s_timer_group[k].full_world > 0) {
            // don't apply the correction until the world actually starts
            full_world[k] = s_timer_group[k].full_world + 1;
            segment[k] = s_timer_group[k].segment + 1;
        }
    }

    // split[k] is just the fullgame time at tape break on the last stage of world k;
    // to calculate this, just add full_world[j] (j<k) for the previous worlds to the
    // current world's segment timer

    u32 sum[WORLD_COUNT] = {};
    u32 split[WORLD_COUNT] = {};
    for (s32 k = 1; k < WORLD_COUNT; k++) {
        for (s32 j = 0; j < k; j++) {
            sum[k] += full_world[j];
        }
        split[k] = sum[k] + segment[k];
    }
    split[0] = s_timer_group[0].segment;
    u32 loadless_story_timer = split[9];

    if (display_story_timer) {
        timerdisp::draw_timer(FULLGAME_TIMER_LOCATION_X, fullgame_timer_location_y,
                              FULLGAME_TIMER_TEXT_OFFSET, "Time:", loadless_story_timer, 0, false,
                              false, draw::WHITE);
    }

    // move the position of the segment timer depending on if the death counter and fullgame timer
    // is on
    u32 segment_timer_location_y = 2;
    if (display_story_timer) {
        segment_timer_location_y++;
    }
    if (pref::get(pref::BoolPref::ShowDeathCounter)) {
        segment_timer_location_y++;
    }

    switch (TimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions))) {
        case TimerOptions::AlwaysShow:
            for (s32 k = 0; k < WORLD_COUNT; k++) {
                if (mkb::scen_info.world == k && !is_run_complete) {
                    timerdisp::draw_timer(SEGMENT_TIMER_LOCATION_X, segment_timer_location_y,
                                          SEGMENT_TIMER_TEXT_OFFSET, "Seg:", segment[k], 0, false,
                                          false, draw::WHITE);
                }
            }
            break;
        case TimerOptions::BetweenWorlds:
            for (s32 k = 0; k < WORLD_COUNT; k++) {
                if (is_between_worlds && mkb::scen_info.world == k && k != 9) {
                    timerdisp::draw_timer(SEGMENT_TIMER_LOCATION_X, segment_timer_location_y,
                                          SEGMENT_TIMER_TEXT_OFFSET, "Seg:", segment[k], 0, false,
                                          false, draw::WHITE);
                }
            }
            break;
        case TimerOptions::EndOfRun:
            break;
        case TimerOptions::DontShow:
            break;
    }

    u32 split_hours[10] = {};
    u32 split_minutes[10] = {};
    u32 split_seconds[10] = {};
    u32 split_centiseconds[10] = {};

    u32 segment_hours[10] = {};
    u32 segment_minutes[10] = {};
    u32 segment_seconds[10] = {};
    u32 segment_centiseconds[10] = {};

    for (s32 k = 0; k < WORLD_COUNT; k++) {
        split_hours[k] = split[k] / HOUR_FRAMES;
        split_minutes[k] = split[k] % HOUR_FRAMES / MINUTE_FRAMES;
        split_seconds[k] = split[k] % MINUTE_FRAMES / SECOND_FRAMES;
        split_centiseconds[k] = (split[k] % SECOND_FRAMES) * 100 / 60;

        segment_hours[k] = segment[k] / HOUR_FRAMES;
        segment_minutes[k] = segment[k] % HOUR_FRAMES / MINUTE_FRAMES;
        segment_seconds[k] = segment[k] % MINUTE_FRAMES / SECOND_FRAMES;
        segment_centiseconds[k] = (segment[k] % SECOND_FRAMES) * 100 / 60;
    }

    char timer_str[10][32] = {};

    // if the segment timer is enabled in any capacity, after the tape is broken on the last stage,
    // show all 10 split and iw times in the format "Wk: split[k] (segment[k])"
    // to do: it is very likely that the current timer formatting text below is redundant given that
    // there is similar code in timerdisp.cpp; for later: implement complex's suggestion for this
    if (TimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) != TimerOptions::DontShow &&
        is_run_complete) {
        for (s32 k = 0; k < WORLD_COUNT; k++) {
            u32 X[10] = {};  // horizontal position for the line of text "Wk: split[k] (segment[k])"
            if (k != 9) {
                X[k] = IW_TIME_LOCATION_X;
            } else {
                X[k] =
                    SEGMENT_TIMER_LOCATION_X;  // change the position for world 10 because W10 takes
                                               // up 3 characters instead of 2 like the rest
            }
            u32 Y[k] = {};  // vertical position for the line of text "Wk: split[k] (segment[k])"
            Y[k] = 24 + (segment_timer_location_y + k) * 16;
            if (split_hours[k] > 0) {
                if (segment_hours[k] > 0) {
                    mkb::sprintf(timer_str[k], "W%d:%d:%02d:%02d.%02d (%d:%02d:%02d.%02d)", k + 1,
                                 split_hours[k], split_minutes[k], split_seconds[k],
                                 split_centiseconds[k], segment_hours[k], segment_minutes[k],
                                 segment_seconds[k], segment_centiseconds[k]);
                    draw::debug_text(X[k], Y[k], draw::WHITE, "%s", timer_str[k]);
                } else {
                    mkb::sprintf(timer_str[k], "W%d:%d:%02d:%02d.%02d (%02d:%02d.%02d)", k + 1,
                                 split_hours[k], split_minutes[k], split_seconds[k],
                                 split_centiseconds[k], segment_minutes[k], segment_seconds[k],
                                 segment_centiseconds[k]);
                    draw::debug_text(X[k], Y[k], draw::WHITE, "%s", timer_str[k]);
                }
            } else {
                mkb::sprintf(timer_str[k], "W%d:%02d:%02d.%02d (%02d:%02d.%02d)", k + 1,
                             split_minutes[k], split_seconds[k], split_centiseconds[k],
                             segment_minutes[k], segment_seconds[k], segment_centiseconds[k]);
                draw::debug_text(X[k], Y[k], draw::WHITE, "%s", timer_str[k]);
            }
        }
    }

    // debugging
    /*
    timerdisp::draw_timer(450, 1, IW_TIME_TEXT_OFFSET, "dbg:", 60 * s_completed_stages, 0, false,
                          false, draw::WHITE);

    timerdisp::draw_timer(450, 2, IW_TIME_TEXT_OFFSET,
                          "dbg:", 60 * mkb::get_world_unbeaten_stage_count(1), 0, false, false,
                          draw::WHITE);
    */
}

}  // namespace storytimer