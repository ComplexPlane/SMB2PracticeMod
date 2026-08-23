#include "deathcounter.h"

#include "mkb/mkb.h"

#include "freecam.h"
#include "storyreset.h"
#include "systems/goal.h"
#include "systems/pref.h"
#include "systems/savest.h"
#include "systems/textinfo.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/mode.h"

namespace deathcounter {

constexpr u16 WORLD_COUNT = mode::WORLD_COUNT;

static u32 s_world_death_count[WORLD_COUNT] = {};
static savest::Action s_previous_frame_action = savest::Action::None;
// Flag to determine when we should/shouldn't increment the death counter
static bool s_can_incr_death_counter = false;

using Mod = textinfo::Module;
using Slot = textinfo::Slot;
using Format = timerdisp::TimeFormat;

u32 get_total_death_count() {
    u32 total = 0;
    for (u16 k = 0; k < WORLD_COUNT; k++) {
        total += s_world_death_count[k];
    }
    return total;
}

u32 get_world_death_count(u16 world_idx) {
    u16 clamped_idx = MIN(world_idx, WORLD_COUNT - 1);  // clamp for safety
    return s_world_death_count[clamped_idx];
}

void increment_world_death_counter() {
    // Check the pref for count first stage deaths and if we're on the first stage
    if (!pref::get(pref::Pref::CountFirstStageDeaths) &&
        mode::get_storymode_total_clear_count() == 0) {
        return;
    }
    s_world_death_count[mkb::scen_info.world] += 1;  // death counter for the current world
    s_can_incr_death_counter = false;
}

void reset_flag() {
    s_can_incr_death_counter = false;
}

void reset_death_counters() {
    for (u16 k = 0; k < WORLD_COUNT; k++) {
        s_world_death_count[k] = 0;
    }
    reset_flag();
}

bool loaded_state() {
    return savest::get_last_action() == savest::Action::Load &&
           s_previous_frame_action == savest::Action::None;
}

// When we're done holding the savestate button/when gameplay resumes
void update_flag_on_state_release() {
    if (goal::is_gameplay_exact() && savest::get_last_action() == savest::Action::None) {
        // As soon as we're done holding the load state button (or just any time we're controlling
        // the monkey on the stage), we're allowed to die
        s_can_incr_death_counter = true;
    }
}

bool should_count_as_normal_death() {
    bool retried_without_clearing =
        mode::is_spin_in_init(mkb::sub_mode) && s_can_incr_death_counter;
    bool left_stage_without_clearing =
        mode::is_stage_exit_submode(mkb::sub_mode) && s_can_incr_death_counter;
    // Need to also check the flag for death init submodes in case we fall out and let the animation
    // play out (ie we enter 2 "death init" submodes before resuming gameplay)
    bool died = mode::is_death_init(mkb::sub_mode) && s_can_incr_death_counter;
    return retried_without_clearing || left_stage_without_clearing || died;
}

bool should_count_as_savestate_death() {
    return loaded_state() && s_can_incr_death_counter;
}

void count_deaths() {
    if (goal::is_postgoal_exact()) {
        s_can_incr_death_counter = false;
    }

    if (should_count_as_normal_death() || should_count_as_savestate_death()) {
        increment_world_death_counter();
    }
}

void tick() {
    if (storyreset::should_reset_run()) {
        reset_death_counters();
    }

    // Whenever entering a new stage, reset our flag
    if (mode::is_spin_in_first_init(mkb::sub_mode)) {
        reset_flag();
    }

    update_flag_on_state_release();
    count_deaths();

    // Only after we're done doing death checks for this frame do we update s_previous_frame_action
    s_previous_frame_action = savest::get_last_action();
}

bool should_display_death_counter() {
    u8 pref = pref::get(pref::Pref::DeathCounterDisplayOptions);

    using DeathCounterOptions = storyreset::StoryDisplayOptions;

    switch (DeathCounterOptions(pref)) {
        case DeathCounterOptions::AlwaysShow:
            return true;
        case DeathCounterOptions::BetweenWorlds:
            return goal::is_between_worlds();
        case DeathCounterOptions::EndOfRun:
            return goal::is_run_complete();
        case DeathCounterOptions::DontShow:
            return false;
        default:
            // Unreachable
            return false;
    }
}

bool should_not_display_counter_at_all() {
    if (!mode::is_main_game_mode_story(mkb::main_game_mode)) {
        // If we're in the menus outside of a story mode run due to an accidental exit game, we
        // still want to be able to display the counter if we haven't reset it yet
        return !storyreset::is_run_active();
    }
    return freecam::should_hide_hud();
}

void disp() {
    if (should_not_display_counter_at_all()) {
        return;
    }

    if (should_display_death_counter()) {
        // Technically not a timer, but we can still use this function without specifying any
        // special formatting
        textinfo::draw_timer(Mod::DeathCounter, Slot::Left, draw::WHITE,
                             "Deaths:", get_total_death_count(), Format::Unformatted);
    }
}

}  // namespace deathcounter
