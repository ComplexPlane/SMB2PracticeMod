/*
* Mod that allows controlling the ball with the Gamecube controller's d-pad
*/

#pragma once

#include <mkb.h>

namespace dpad {

void on_PADRead(mkb::PADStatus* statuses);

}  // namespace dpad