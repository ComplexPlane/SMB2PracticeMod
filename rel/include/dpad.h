#pragma once

/*
 * Mod that allows controlling the ball with the Gamecube controller's d-pad
 */

namespace dpad {

void tick();
void set_visible(bool visible);
bool is_visible();

}  // namespace dpad