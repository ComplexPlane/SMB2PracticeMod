#include "storytimer.h"

#include "mkb/mkb.h"

#include "deathcounter.h"
#include "freecam.h"
#include "storyreset.h"
#include "systems/goal.h"
#include "systems/pref.h"
#include "systems/textinfo.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/mode.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

namespace storytimer {

enum class TimerType {
    Fullgame,
    Segment,
};

constexpr u16 WORLD_COUNT = mode::WORLD_COUNT;
constexpr u16 STAGES_PER_WORLD = mode::STAGES_PER_WORLD;

static WorldTimer s_world_timer[WORLD_COUNT];  // timer info for each world

// using Mod = textinfo::Module;
using Slot = textinfo::Slot;
using Format = timerdisp::TimeFormat;

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

// Special case of textinfo::draw_timer() useful for us
void draw_timer(char *prefix, u32 frames, Format format) {
    // textinfo::draw_timer(Mod::LoadlessTimer, Slot::Left, draw::WHITE, prefix, frames, format);
    textinfo::draw_timer_new(Slot::Left, draw::WHITE, prefix, frames, format);
}

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
    if (should_display_timer(TimerType::Fullgame)) {
        draw_timer("Time:", get_loadless_time(), Format::AlwaysLeadNonHours);
    }

    u16 world_idx = get_current_segment_idx();  // index of the current world
    if (should_display_timer(TimerType::Segment)) {
        draw_timer("Seg:", s_world_timer[world_idx].segment, Format::AlwaysLeadNonHours);
    }
}

// We only use this function for 0 <= row <= 10
s32 get_breakdown_row_x_pos(u16 row) {
    // s32 num_x_pos = textinfo::module_and_slot_to_x_alignment(Mod::LoadlessTimer, Slot::Left);
    s32 num_x_pos = textinfo::get_slot_x_alignment(Slot::Left);
    if (row < WORLD_COUNT - 1) {  // "Wk:" is 3 characters long if 1 <= k <= 9
        return num_x_pos - 3 * draw::DEBUG_CHAR_WIDTH;
    } else if (row == WORLD_COUNT - 1) {  // "W10:" is 4 characters long
        return num_x_pos - 4 * draw::DEBUG_CHAR_WIDTH;
    } else {  // "Totals:" is 7 characters long
        return num_x_pos - 7 * draw::DEBUG_CHAR_WIDTH;
    }
}

void draw_breakdown_screen() {
    // Format: "Wk: world k split time (world k segment time) (world k deaths)"
    char split_buf[16] = {};
    char seg_buf[16] = {};
    char row_info_buf[32] = {};

    for (u16 idx = 0; idx < WORLD_COUNT; idx++) {
        s32 pos_x = get_breakdown_row_x_pos(idx);
        u32 world_deaths = deathcounter::get_world_death_count(idx);

        timerdisp::format_time(split_buf, get_split_timer_for_world(idx),
                               Format::MinutesAlwaysLeadingZero);
        timerdisp::format_time(seg_buf, s_world_timer[idx].segment,
                               Format::MinutesAlwaysLeadingZero);
        mkb::sprintf(row_info_buf, "W%d:%s (%s) (%d)", idx + 1, split_buf, seg_buf, world_deaths);

        // textinfo::draw(Mod::LoadlessTimer, Slot::Left, pos_x, draw::WHITE, true, row_info_buf);
        textinfo::draw_new(Slot::Left, pos_x, draw::WHITE, true, row_info_buf);
    }

    // For the totals row
    s32 totals_x_pos = get_breakdown_row_x_pos(WORLD_COUNT);
    u32 total_deaths = deathcounter::get_total_death_count();

    char total_time_buf[16] = {};
    timerdisp::format_time(total_time_buf, get_loadless_time(), Format::MinutesAlwaysLeadingZero);
    /* textinfo::draw(Mod::LoadlessTimer, Slot::Left, totals_x_pos, draw::WHITE, true,
                   "Totals:%s (%d)", total_time_buf, total_deaths); */
    textinfo::draw_new(Slot::Left, totals_x_pos, draw::WHITE, true, "Totals:%s (%d)",
                       total_time_buf, total_deaths);
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
        return;
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