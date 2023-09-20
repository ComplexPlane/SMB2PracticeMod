#pragma once

#include "mkb/mkb.h"

namespace binds {

bool button_chord_pressed(u8 chord_id, bool priority);
u8 get_current_encoding();

void init();
void tick();

}  // namespace binds
