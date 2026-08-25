#pragma once

#include "mkb/mkb.h"

namespace hide_sprites {

enum class RightSideUI {
    HideAll,
    HideMonkeyHead,
    HideNone,
};

void hide_banana_counter();
void hide_monkey_head();
void hide_right_side_sprites();

bool right_side_sprites_normally_visible();

void tick();

}  // namespace hide_sprites
