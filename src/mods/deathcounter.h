#pragma once

#include "../mkb/mkb.h"

namespace deathcounter {

/* enum class DeathCounterOptions {
    DontShow = 0,
    AlwaysShow = 1,
    BetweenWorlds = 2,
    EndOfRun = 3,
}; */

u32 get_total_death_count();
u32 get_world_death_count(u16 world_idx);
bool should_display_death_counter();

void tick();
void disp();

}  // namespace deathcounter