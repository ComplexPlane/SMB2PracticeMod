#include "pad.h"

#include <mkb/mkb.h>

namespace pad
{

static int s_konami_progress;

static bool any_button_pressed()
{
    return button_pressed(~0) || analog_pressed(~0);
}

static void update_konami()
{
    if (s_konami_progress >= 11)
    {
        s_konami_progress = 0;
    }

    if (!any_button_pressed()) return;

    switch (s_konami_progress)
    {
        case 0:
        case 1:
        {
            if (dir_pressed(DIR_UP)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        case 2:
        case 3:
        {
            if (dir_pressed(DIR_DOWN)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        case 4:
        case 6:
        {
            if (dir_pressed(DIR_LEFT)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        case 5:
        case 7:
        {
            if (dir_pressed(DIR_RIGHT)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        case 8:
        {
            if (button_pressed(BUTTON_B)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        case 9:
        {
            if (button_pressed(BUTTON_A)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        case 10:
        {
            if (button_pressed(BUTTON_START)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        default:
        {
            s_konami_progress = 0;
            break;
        }
    }
}

void tick()
{
    update_konami();
}

bool button_pressed(u16 button)
{
    bool down_this_frame = mkb::filtered_button_inputs_bitfield & (button << 16u);
    bool down_last_frame = mkb::filtered_button_inputs_bitfield & button;
    return down_this_frame && !down_last_frame;
}

bool buttonReleased(u16 button)
{
    bool down_this_frame = mkb::filtered_button_inputs_bitfield & (button << 16u);
    bool down_last_frame = mkb::filtered_button_inputs_bitfield & button;
    return !down_this_frame && down_last_frame;
}

bool button_down(u16 button)
{
    return mkb::filtered_button_inputs_bitfield & (button << 16);
}

bool button_chord_pressed(u16 btn1, u16 btn2)
{
    return (button_down(btn1) && button_pressed(btn2)) || (button_pressed(btn1) && button_down(btn2));
}
int get_cstick_dir()
{
    bool left = mkb::filtered_analog_inputs_bitfield & (AR_CSTICK_LEFT << 16u);
    bool right = mkb::filtered_analog_inputs_bitfield & (AR_CSTICK_RIGHT << 16u);
    bool up = mkb::filtered_analog_inputs_bitfield & (AR_CSTICK_UP << 16u);
    bool down = mkb::filtered_analog_inputs_bitfield & (AR_CSTICK_DOWN << 16u);

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

bool analog_pressed(u16 analog_region)
{
    bool down_this_frame = mkb::filtered_analog_inputs_bitfield & (analog_region << 16u);
    bool down_last_frame = mkb::filtered_analog_inputs_bitfield & analog_region;
    return down_this_frame && !down_last_frame;
}

bool analog_released(u16 analog_region)
{
    bool down_this_frame = mkb::filtered_analog_inputs_bitfield & (analog_region << 16u);
    bool down_last_frame = mkb::filtered_analog_inputs_bitfield & analog_region;
    return !down_this_frame && down_last_frame;
}

bool dir_down(u16 dir)
{
    switch (dir)
    {
        case DIR_UP:
        {
            return button_down(BUTTON_DPAD_UP) || analog_down(AR_LSTICK_UP);
        }
        case DIR_DOWN:
        {
            return button_down(BUTTON_DPAD_DOWN) || analog_down(AR_LSTICK_DOWN);
        }
        case DIR_LEFT:
        {
            return button_down(BUTTON_DPAD_LEFT) || analog_down(AR_LSTICK_LEFT);
        }
        case DIR_RIGHT:
        {
            return button_down(BUTTON_DPAD_RIGHT) || analog_down(AR_LSTICK_RIGHT);
        }
        default:
        {
            return false;
        }
    }
}

bool dir_pressed(u16 dir)
{
    switch (dir)
    {
        case DIR_UP:
        {
            return button_pressed(BUTTON_DPAD_UP) || analog_pressed(AR_LSTICK_UP);
        }
        case DIR_DOWN:
        {
            return button_pressed(BUTTON_DPAD_DOWN) || analog_pressed(AR_LSTICK_DOWN);
        }
        case DIR_LEFT:
        {
            return button_pressed(BUTTON_DPAD_LEFT) || analog_pressed(AR_LSTICK_LEFT);;
        }
        case DIR_RIGHT:
        {
            return button_pressed(BUTTON_DPAD_RIGHT) || analog_pressed(AR_LSTICK_RIGHT);;
        }
        default:
        {
            return false;
        }
    }
}

bool analog_down(u16 analog_region)
{
    return mkb::filtered_analog_inputs_bitfield & (analog_region << 16);
}

bool konami_pressed()
{
    return s_konami_progress == 11;
}

}
