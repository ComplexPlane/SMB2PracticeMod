#pragma once

#include "mkb/mkb.h"

namespace ilbattle {

static constexpr u32 NUM_LENGTHS = 4;

void track_valid_attempt();
void tick();
void disp();

}  // namespace ilbattle
