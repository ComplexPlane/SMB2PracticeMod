#pragma once

#include "mkb/mkb.h"

namespace storytimer {

void on_goal_entry();
void tick();
void disp();
u32 get_completed_stagecount();

}  // namespace storytimer