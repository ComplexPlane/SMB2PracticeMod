#include "pad.h"

#include <mkb/mkb.h>

namespace pad
{

static int s_konamiProgress;

static bool anyButtonPressed()
{
    return buttonPressed(~0) || analogPressed(~0);
}

static void updateKonami()
{
    if (s_konamiProgress >= 11)
    {
        s_konamiProgress = 0;
    }

    if (!anyButtonPressed()) return;

    switch (s_konamiProgress)
    {
        case 0:
        case 1:
        {
            if (dirPressed(DIR_UP)) s_konamiProgress++;
            else s_konamiProgress = 0;
            break;
        }
        case 2:
        case 3:
        {
            if (dirPressed(DIR_DOWN)) s_konamiProgress++;
            else s_konamiProgress = 0;
            break;
        }
        case 4:
        case 6:
        {
            if (dirPressed(DIR_LEFT)) s_konamiProgress++;
            else s_konamiProgress = 0;
            break;
        }
        case 5:
        case 7:
        {
            if (dirPressed(DIR_RIGHT)) s_konamiProgress++;
            else s_konamiProgress = 0;
            break;
        }
        case 8:
        {
            if (buttonPressed(BUTTON_B)) s_konamiProgress++;
            else s_konamiProgress = 0;
            break;
        }
        case 9:
        {
            if (buttonPressed(BUTTON_A)) s_konamiProgress++;
            else s_konamiProgress = 0;
            break;
        }
        case 10:
        {
            if (buttonPressed(BUTTON_START)) s_konamiProgress++;
            else s_konamiProgress = 0;
            break;
        }
        default:
        {
            s_konamiProgress = 0;
            break;
        }
    }
}

void tick()
{
    updateKonami();
}

bool buttonPressed(uint16_t button)
{
    bool downThisFrame = mkb::padButtonBitfield & (button << 16u);
    bool downLastFrame = mkb::padButtonBitfield & button;
    return downThisFrame && !downLastFrame;
}

bool buttonReleased(uint16_t button)
{
    bool downThisFrame = mkb::padButtonBitfield & (button << 16u);
    bool downLastFrame = mkb::padButtonBitfield & button;
    return !downThisFrame && downLastFrame;
}

bool buttonDown(uint16_t button)
{
    return mkb::padButtonBitfield & (button << 16);
}

bool buttonChordPressed(uint16_t btn1, uint16_t btn2)
{
    return (buttonDown(btn1) && buttonPressed(btn2)) || (buttonPressed(btn1) && buttonDown(btn2));
}

int getCStickDir()
{
    bool left = mkb::padAnalogBitfield & (AR_CSTICK_LEFT << 16u);
    bool right = mkb::padAnalogBitfield & (AR_CSTICK_RIGHT << 16u);
    bool up = mkb::padAnalogBitfield & (AR_CSTICK_UP << 16u);
    bool down = mkb::padAnalogBitfield & (AR_CSTICK_DOWN << 16u);

    if (up && left) return DIR_UPLEFT;
    else if (up && right) return DIR_UPRIGHT;
    else if (down && left) return DIR_DOWNLEFT;
    else if (down && right) return DIR_DOWNRIGHT;
    else if (up) return DIR_UP;
    else if (down) return DIR_DOWN;
    else if (left) return DIR_LEFT;
    else if (right) return DIR_RIGHT;
    else return DIR_NONE;
}

bool analogPressed(uint16_t analogRegion)
{
    bool downThisFrame = mkb::padAnalogBitfield & (analogRegion << 16u);
    bool downLastFrame = mkb::padAnalogBitfield & analogRegion;
    return downThisFrame && !downLastFrame;
}

bool analogReleased(uint16_t analogRegion)
{
    bool downThisFrame = mkb::padAnalogBitfield & (analogRegion << 16u);
    bool downLastFrame = mkb::padAnalogBitfield & analogRegion;
    return !downThisFrame && downLastFrame;
}

bool dirDown(uint16_t dir)
{
    switch (dir)
    {
        case DIR_UP:
        {
            return buttonDown(BUTTON_DPAD_UP) || analogDown(AR_LSTICK_UP);
        }
        case DIR_DOWN:
        {
            return buttonDown(BUTTON_DPAD_DOWN) || analogDown(AR_LSTICK_DOWN);
        }
        case DIR_LEFT:
        {
            return buttonDown(BUTTON_DPAD_LEFT) || analogDown(AR_LSTICK_LEFT);
        }
        case DIR_RIGHT:
        {
            return buttonDown(BUTTON_DPAD_RIGHT) || analogDown(AR_LSTICK_RIGHT);
        }
        default:
        {
            return false;
        }
    }
}

bool dirPressed(uint16_t dir)
{
    switch (dir)
    {
        case DIR_UP:
        {
            return buttonPressed(BUTTON_DPAD_UP) || analogPressed(AR_LSTICK_UP);
        }
        case DIR_DOWN:
        {
            return buttonPressed(BUTTON_DPAD_DOWN) || analogPressed(AR_LSTICK_DOWN);
        }
        case DIR_LEFT:
        {
            return buttonPressed(BUTTON_DPAD_LEFT) || analogPressed(AR_LSTICK_LEFT);;
        }
        case DIR_RIGHT:
        {
            return buttonPressed(BUTTON_DPAD_RIGHT) || analogPressed(AR_LSTICK_RIGHT);;
        }
        default:
        {
            return false;
        }
    }
}

bool analogDown(uint16_t analogRegion)
{
    return mkb::padAnalogBitfield & (analogRegion << 16);
}

bool konamiPressed()
{
    return s_konamiProgress == 11;
}

}
