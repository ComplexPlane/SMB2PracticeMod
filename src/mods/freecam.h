#pragma once

namespace freecam {

constexpr int TURBO_SPEED_MIN = 2;
constexpr int TURBO_SPEED_MAX = 200;

void init();
void tick();
bool enabled();

}  // namespace freecam
