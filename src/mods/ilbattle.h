#pragma once

#include "../mkb/mkb.h"

namespace ilbattle {

static constexpr u32 NUM_LENGTHS = 4;

enum class IlBattleState {
    NotReady,
    WaitForFirstRetry,     // Mod is enabled but first retry hasnt ocurred
    BattleRunning,         // Battle is running
    BuzzerBeater,          // Time has run out, but an attempt is still going
    BuzzerBeaterPostgoal,  // Final attempt is over, wait for postgoals for score
    BattleDone,            // Final attempt is over, show times in blue
};

void tick();
void disp();
void new_battle();

}  // namespace ilbattle
