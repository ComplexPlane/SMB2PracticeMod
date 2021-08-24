#include "dpad.h"

#include <mkb.h>
#include "macro_utils.h"
#include "pref.h"

namespace dpad {

void tick() {
    if (!pref::get_dpad_controls()) return;

    for (auto& btnGrp : mkb::pad_status_groups) {
        bool up = btnGrp.raw.button & mkb::PAD_BUTTON_UP;
        bool down = btnGrp.raw.button & mkb::PAD_BUTTON_DOWN;
        bool left = btnGrp.raw.button & mkb::PAD_BUTTON_LEFT;
        bool right = btnGrp.raw.button & mkb::PAD_BUTTON_RIGHT;

        s32 newX = btnGrp.raw.stickX + left * -60 + right * 60;
        btnGrp.raw.stickX = CLAMP(newX, -60, 60);
        s32 newY = btnGrp.raw.stickY + down * -60 + up * 60;
        btnGrp.raw.stickY = CLAMP(newY, -60, 60);
    }
}

}  // namespace dpad