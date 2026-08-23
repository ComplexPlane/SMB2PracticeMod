#pragma once

#include "utils/base.h"

void validate_run();
bool validate_was_run_valid(bool mods_allowed);
void validate_disable_invalidating_settings();
u32 validate_get_framesave();

void validate_init();
void validate_tick();
