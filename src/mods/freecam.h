#pragma once

enum {
    Freecam_TURBO_SPEED_MIN = 2,
    Freecam_TURBO_SPEED_MAX = 200,
};

void Freecam_Init();
void Freecam_Tick();
bool Freecam_Enabled();
bool Freecam_ShouldFreezeTimer();
bool Freecam_ShouldHideHud();
