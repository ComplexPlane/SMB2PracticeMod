#pragma once

#include "../mkb/mkb.h"

namespace storyreset {

// Does it make more sense to move this enum class to pref.h?
// We need this enum class in storyreset.cpp, but we also need it in both deathcounter.cpp and
// storytimer.cpp
enum class StoryDisplayOptions {
    DontShow,
    AlwaysShow,
    BetweenWorlds,
    EndOfRun,
};
// The fullgame loadless timer and death counter share the same display options. The loadless
// segment timer options are a subset of this enum class (everything except an EndOfRun setting),
// and the segment timer options show up in the same order in the menu as the corresponding fullgame
// one, so we don't need to make a separate enum class for it

bool is_run_active();
void set_run_active_status(bool is_active);

u8 get_active_file_index();
u8 get_active_world();
void reset_active_run_info();
bool should_reset_run();
void display_reset_run_message();
bool all_loadless_timer_prefs_off();

void tick();

}  // namespace storyreset
