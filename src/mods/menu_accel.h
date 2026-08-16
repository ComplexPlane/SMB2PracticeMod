#pragma once

#include "../mkb/mkb.h"

namespace menu_accel {

enum class MenuAccelOptions {
    Disabled,
    AlwaysEnabled,
    OnlyInStory,
};

void tick();

}  // namespace menu_accel
