#include "dpad.h"

#include <mkb.h>
#include "macro_utils.h"
#include "pref.h"

namespace dpad {

void on_PADRead(mkb::PADStatus* statuses) {
    if (!pref::get_dpad_controls()) return;

    for (u32 i = 0; i < 4; i++) {
        mkb::PADStatus& status = statuses[i];
        if (status.err != mkb::PAD_ERR_NONE) continue;

        bool up = status.button & mkb::PAD_BUTTON_UP;
        bool down = status.button & mkb::PAD_BUTTON_DOWN;
        bool left = status.button & mkb::PAD_BUTTON_LEFT;
        bool right = status.button & mkb::PAD_BUTTON_RIGHT;

        s32 newX = status.stickX;
        s32 newY = status.stickY;
        constexpr s32 CARDINAL = 105;
        constexpr s32 DIAG = 78;

        // Modify raw stick input so that input display still looks reasonable when using dpad
        if (up && left) {
            newX -= DIAG;
            newY += DIAG;
        } else if (up && right) {
            newX += DIAG;
            newY += DIAG;
        } else if (down && left) {
            newX -= DIAG;
            newY -= DIAG;
        } else if (down && right) {
            newX += DIAG;
            newY -= DIAG;
        } else if (up) {
            newY += CARDINAL;
        } else if (down) {
            newY -= CARDINAL;
        } else if (left) {
            newX -= CARDINAL;
        } else if (right) {
            newX += CARDINAL;
        }
        if (up || down || left || right) {
            // This is enough to prevent overflow, but diagonals can still exceed their normal range
            // when using stick+dpad
            newX = CLAMP(newX, -CARDINAL, CARDINAL);
            newY = CLAMP(newY, -CARDINAL, CARDINAL);
        }

        status.stickX = newX;
        status.stickY = newY;
    }
}

}  // namespace dpad