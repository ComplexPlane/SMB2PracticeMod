#include "mods/dpad.h"

#include "systems/pref.h"
#include "utils/macro_utils.h"

void dpad_on_PADRead(mkb_PADStatus *statuses) {
    if (!Pref_Get(Pref_DpadControls)) return;

    for (u32 i = 0; i < 4; i++) {
        mkb_PADStatus *status = &statuses[i];
        if (status->err != mkb_PAD_ERR_NONE) continue;

        bool up = status->button & mkb_PAD_BUTTON_UP;
        bool down = status->button & mkb_PAD_BUTTON_DOWN;
        bool left = status->button & mkb_PAD_BUTTON_LEFT;
        bool right = status->button & mkb_PAD_BUTTON_RIGHT;
        s32 new_x = status->stickX;
        s32 new_y = status->stickY;
        constexpr s32 CARDINAL = 107;
        constexpr s32 DIAG = 78;

        if (up && left) {
            new_x -= DIAG;
            new_y += DIAG;
        } else if (up && right) {
            new_x += DIAG;
            new_y += DIAG;
        } else if (down && left) {
            new_x -= DIAG;
            new_y -= DIAG;
        } else if (down && right) {
            new_x += DIAG;
            new_y -= DIAG;
        } else if (up) {
            new_y += CARDINAL;
        } else if (down) {
            new_y -= CARDINAL;
        } else if (left) {
            new_x -= CARDINAL;
        } else if (right) {
            new_x += CARDINAL;
        }
        if (up || down || left || right) {
            new_x = CLAMP(new_x, -CARDINAL, CARDINAL);
            new_y = CLAMP(new_y, -CARDINAL, CARDINAL);
        }
        status->stickX = new_x;
        status->stickY = new_y;
    }
}
