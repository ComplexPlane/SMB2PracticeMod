#pragma once

#include "mkb/mkb.h"

namespace goal {

bool is_postgoal_exact();
bool is_gameplay_exact();

bool is_between_worlds();
bool is_run_complete();

void init();

}  // namespace goal
