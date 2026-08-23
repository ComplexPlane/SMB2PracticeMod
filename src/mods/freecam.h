#pragma once

enum {
    freecam_TURBO_SPEED_MIN = 2,
    freecam_TURBO_SPEED_MAX = 200,
};

void freecam_init();
void freecam_tick();
bool freecam_enabled();
bool freecam_should_freeze_timer();
bool freecam_should_hide_hud();
