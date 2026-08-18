#include "storytimer.h"

#include "../mkb/mkb.h"

#include "../systems/goal.h"
#include "../systems/pref.h"
#include "../utils/draw.h"
#include "../utils/macro_utils.h"
#include "../utils/mode.h"
#include "../utils/patch.h"
#include "../utils/timerdisp.h"
#include "deathcounter.h"
#include "freecam.h"
#include "storyreset.h"

namespace storytimer {

constexpr s16 FULLGAME_TIMER_LOCATION_X = 18 + 24;
constexpr u16 FULLGAME_TIMER_TEXT_OFFSET = 5 * draw::DEBUG_CHAR_WIDTH;
constexpr s16 SEGMENT_TIMER_LOCATION_X = 30 + 24;
constexpr u16 SEGMENT_TIMER_TEXT_OFFSET = 4 * draw::DEBUG_CHAR_WIDTH;
constexpr s16 BREAKDOWN_ROW_LOCATION_X = 42 + 24;
constexpr s16 TOTALS_ROW_LOCATION_X = 18;
constexpr u16 STARTING_ROW = 2;

constexpr u16 WORLD_COUNT = mode::WORLD_COUNT;
constexpr u16 STAGES_PER_WORLD = mode::STAGES_PER_WORLD;

static WorldTimer s_world_timer[WORLD_COUNT];  // timer info for each world

// --- some getters that other files can use (if needed) ---

WorldTimer get_world_timer_info(u16 world_idx) {
    // clamp for safety so we don't access outside the bounds of the array
    u16 clamped_idx = MIN(world_idx, WORLD_COUNT - 1);
    return s_world_timer[clamped_idx];
}

// Used to calculate our split times for the breakdown screen; also gives us a
// convenient way to calculate fullgame loadless time
u32 get_split_timer_for_world(u16 world_idx) {
    u16 clamped_idx = MIN(world_idx, WORLD_COUNT - 1);
    u32 prev_world_sum = 0;
    for (u16 k = 0; k < clamped_idx; k++) {
        prev_world_sum += s_world_timer[k].full_world;
    }
    return prev_world_sum + s_world_timer[clamped_idx].segment;
}

u32 get_loadless_time() {
    return get_split_timer_for_world(WORLD_COUNT - 1);
}

bool is_run_active() {
    return get_loadless_time() != 0 && !goal::is_run_complete();
}

// --- main timer logic ---

void reset_timer() {
    if (get_loadless_time() != 0) {
        for (u16 k = 0; k < WORLD_COUNT; k++) {
            s_world_timer[k] = {};
        }
        storyreset::reset_active_run_info();
        storyreset::display_reset_run_message();
        // mkb::OSReport("Reset timer \n");
    }
}

// Importantly, we don't increment the timer on the 10 ball screen after selecting a stage
// since the time between doing so and entering the next stage can be highly variable
// To properly increment the timer on the 10 ball screen, we run this
// update function inside a hook for mkb::g_handle_storymode_stageselect_state
void update_timers_on_10_ball_screen(mkb::StoryModeStageSelectState state) {
    for (u16 k = 0; k < WORLD_COUNT; k++) {
        if (mkb::scen_info.world == k) {  // World (k+1)'s timer
            if (mode::is_on_10_ball_spin_in(state) || mode::is_idle_on_10_ball_screen(state) ||
                mode::has_selected_stage_on_10_ball_screen_init(state)) {
                s_world_timer[k].full_world += 1;
                s_world_timer[k].segment = s_world_timer[k].full_world;
            }
        }
    }
}

// The above function does not get run when the game is paused, so we also need this
void update_timers_while_paused_on_10_ball_screen() {
    for (u16 k = 0; k < WORLD_COUNT; k++) {
        if (mkb::scen_info.world == k) {
            if (mode::is_on_10_ball_screen(mkb::sub_mode, mkb::scen_info) && mode::is_paused()) {
                s_world_timer[k].full_world += 1;
                s_world_timer[k].segment = s_world_timer[k].full_world;
            }
        }
    }
}

// mode::is_on_stage_with_endpoints(mkb::sub_mode)
// Increment the timer on every submode on the stage (and exit game screen)
void update_timers_on_stage() {
    for (u16 k = 0; k < WORLD_COUNT; k++) {
        if (mkb::scen_info.world == k) {  // World (k+1)'s timer
            if (mode::is_on_stage(mkb::sub_mode) || mode::is_story_exit_game(mkb::sub_mode)) {
                s_world_timer[k].full_world += 1;

                // Increment the segment timer until tape breek on the last stage of the world
                if (!goal::is_between_worlds()) {
                    s_world_timer[k].segment = s_world_timer[k].full_world;
                }
            }
        }
    }
}

// For if we fully exit game by accident
void update_timers_on_menus() {
    if ((mode::is_sel_ngc(mkb::sub_mode) || mode::is_storymode_file_screen_main(mkb::scen_info)) &&
        is_run_active()) {
        u8 active_world_idx = storyreset::get_active_world();
        s_world_timer[active_world_idx].full_world += 1;
        s_world_timer[active_world_idx].segment = s_world_timer[active_world_idx].full_world;
    }
}

// So deathcounter.cpp can know when the run is active
void update_run_active_flag() {
    storyreset::set_run_active_status(is_run_active());
}

void tick() {
    if (storyreset::should_reset_run()) {
        reset_timer();
    }

    if (mode::is_main_game_mode_story(mkb::main_game_mode)) {
        update_timers_on_stage();
        update_timers_while_paused_on_10_ball_screen();
    }

    update_timers_on_menus();
    update_run_active_flag();
}

// --- display stuff ---

// The run breakdown screen replaces the segment timer at the end of the run
// if the pref for it is on
bool should_display_timer(TimerType type) {
    u8 pref;
    if (type == TimerType::Fullgame) {
        pref = pref::get(pref::Pref::FullgameTimerOptions);
    } else {  // Segment timer
        pref = pref::get(pref::Pref::SegmentTimerOptions);
    }

    using TimerOptions = storyreset::StoryDisplayOptions;

    switch (TimerOptions(pref)) {
        case TimerOptions::AlwaysShow:
            if (type == TimerType::Fullgame) {
                return true;
            } else if (pref::get(pref::Pref::ShowRunBreakdown)) {
                // type is segment timer + show breakdown on
                return !goal::is_run_complete();
            } else {  // type is segment timer + show breakdown off
                return true;
            }
        case TimerOptions::BetweenWorlds:
            if (type == TimerType::Fullgame) {
                return goal::is_between_worlds();
            } else if (pref::get(pref::Pref::ShowRunBreakdown)) {
                return goal::is_between_worlds() && !goal::is_run_complete();
            } else {
                return goal::is_between_worlds();
            }
        case TimerOptions::EndOfRun:
            if (type == TimerType::Fullgame) {
                return goal::is_run_complete();
            } else {
                // We don't have an end of run pref for the segment timer (since it doesn't really
                // make sense to have one)
                return false;
            }
        case TimerOptions::DontShow:
            return false;
        default:
            // Unreachable
            return false;
    }
}

// Doing this for now until a better display setup is figured out
u16 get_timer_row(TimerType type) {
    u16 row = STARTING_ROW;
    bool is_displaying_death_counter = deathcounter::should_display_death_counter();

    if (type == TimerType::Fullgame) {
        if (is_displaying_death_counter) {
            row++;
        }
    } else {  // Segment timer
        if (should_display_timer(TimerType::Fullgame)) {
            row++;
        }
        if (is_displaying_death_counter) {
            row++;
        }
    }

    return row;
}

// Bundle up the info timerdisp::draw_timer uses into a struct for convenience
struct TimerDisplayInfo {
    u16 pos_x;
    u16 row;
    u16 text_offset;
};

TimerDisplayInfo get_timer_display_info(TimerType type) {
    if (type == TimerType::Fullgame) {
        return {FULLGAME_TIMER_LOCATION_X, get_timer_row(type), FULLGAME_TIMER_TEXT_OFFSET};
    } else {
        return {SEGMENT_TIMER_LOCATION_X, get_timer_row(type), SEGMENT_TIMER_TEXT_OFFSET};
    }
}

u16 get_current_segment_idx() {
    // mkb::scen_info.world gets reset to 0 on the file screen, so if we accidentally exit game to
    // the menus, the only case where we can't use mkb::scen_info.world is when we are on the file
    // screen with the timer still running
    if (mode::is_storymode_file_screen_main(mkb::scen_info) && get_loadless_time() != 0) {
        // The world we were on before exit-gaming
        return storyreset::get_active_world();
    } else {
        // if we're in a run (not on the menus), this is the index of the current world (between 0
        // and 9 inclusive)
        return mkb::scen_info.world;
    }
}

void draw_timers() {
    TimerDisplayInfo fullgame_info = get_timer_display_info(TimerType::Fullgame);
    u32 loadless_time = get_loadless_time();

    if (should_display_timer(TimerType::Fullgame)) {
        timerdisp::draw_timer(fullgame_info.pos_x, fullgame_info.row, fullgame_info.text_offset,
                              "Time:", loadless_time, false, draw::WHITE);
    }

    u16 world_idx = get_current_segment_idx();  // index of the current world
    TimerDisplayInfo seg_info = get_timer_display_info(TimerType::Segment);

    if (should_display_timer(TimerType::Segment)) {
        timerdisp::draw_timer(seg_info.pos_x, seg_info.row, seg_info.text_offset,
                              "Seg:", s_world_timer[world_idx].segment, false, draw::WHITE);
    }
}

// We only use this function for 0 <= row <= 10
Vec2d get_breakdown_row_position(u16 row) {
    u16 starting_row = get_timer_row(TimerType::Segment);
    float pos_y = timerdisp::row_number_to_vertical_pos(starting_row + row);
    if (row < WORLD_COUNT - 1) {
        return {BREAKDOWN_ROW_LOCATION_X, pos_y};
    } else if (row == WORLD_COUNT - 1) {  // "W10" takes up more space than "Wk" for "1 <= k <= 9"
        return {BREAKDOWN_ROW_LOCATION_X - draw::DEBUG_CHAR_WIDTH, pos_y};
    } else {  // For the totals row
        return {TOTALS_ROW_LOCATION_X, pos_y};
    }
}

void draw_breakdown_screen() {
    // Format: "Wk: world k split time (world k segment time) (world k deaths)"
    char split_buf[WORLD_COUNT][16] = {};
    char seg_buf[WORLD_COUNT][16] = {};
    char row_info_buf[WORLD_COUNT][32] = {};

    for (u16 idx = 0; idx < WORLD_COUNT; idx++) {
        Vec2d pos = get_breakdown_row_position(idx);
        u32 world_deaths = deathcounter::get_world_death_count(idx);

        timerdisp::format_time_to_buffer(split_buf[idx], get_split_timer_for_world(idx),
                                         timerdisp::TimeFormatType::MinutesAlwaysLeadingZero);
        timerdisp::format_time_to_buffer(seg_buf[idx], s_world_timer[idx].segment,
                                         timerdisp::TimeFormatType::MinutesAlwaysLeadingZero);
        mkb::sprintf(row_info_buf[idx], "W%d:%s (%s) (%d)", idx + 1, split_buf[idx], seg_buf[idx],
                     world_deaths);

        draw::debug_text(pos.x, pos.y, draw::WHITE, "%s", row_info_buf[idx]);
    }

    // For the totals row
    char totals_row_buf[32] = {};
    Vec2d totals_row_pos = get_breakdown_row_position(WORLD_COUNT);
    u32 total_deaths = deathcounter::get_total_death_count();

    mkb::sprintf(totals_row_buf, "Totals:%s (%d)", split_buf[WORLD_COUNT - 1], total_deaths);
    draw::debug_text(totals_row_pos.x, totals_row_pos.y, draw::WHITE, "%s", totals_row_buf);
}

bool should_not_display_timer_at_all() {
    // The only time we can ever display the timer outside of story mode is
    // when we fully exit game and the timer is still running
    if (!mode::is_main_game_mode_story(mkb::main_game_mode)) {
        // return get_loadless_time() == 0;
        return !is_run_active();
    }
    // If in story, hide the timer if freecamming (if the pref is on)
    return freecam::should_hide_hud();
}

void disp() {
    if (should_not_display_timer_at_all()) {
        return;  // testing, remember to uncomment
    }

    draw_timers();

    if (pref::get(pref::Pref::ShowRunBreakdown) && goal::is_run_complete()) {
        draw_breakdown_screen();
    }
}

static patch::Tramp<mkb::g_handle_storymode_stageselect_state>
    s_g_handle_storymode_stageselect_state_tramp([]() {
        s_g_handle_storymode_stageselect_state_tramp.chain();
        update_timers_on_10_ball_screen(mkb::g_storymode_stageselect_state);
    });

void init_main_game() {
    s_g_handle_storymode_stageselect_state_tramp.hook();
}

}  // namespace storytimer