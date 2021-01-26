#include "pad.h"

#include <mkb/mkb.h>
#include <cstring>

#include "patch.h"

namespace mkb
{
extern "C"
{
    extern u32 g_some_other_flags;
}
}

namespace pad
{

static s32 s_konami_progress;
static bool s_exclusive_mode;

static mkb::AnalogInputGroup s_merged_analog_inputs;
static mkb::DigitalInputGroup s_merged_digital_inputs;
static mkb::AnalogInputGroup s_analog_inputs[4];
static mkb::PadStatusGroup s_pad_status_groups[4];

bool button_down(u16 digital_input)
{
    return s_merged_digital_inputs.raw & digital_input;
}

bool button_pressed(u16 digital_input)
{
    return s_merged_digital_inputs.pressed & digital_input;
}

bool button_released(u16 digital_input)
{
    return s_merged_digital_inputs.released & digital_input;
}

bool analog_down(u16 analog_input)
{
    return s_merged_analog_inputs.raw & analog_input;
}

bool analog_pressed(u16 analog_input)
{
    return s_merged_analog_inputs.pressed & analog_input;
}

bool analog_released(u16 analog_input)
{
    return s_merged_analog_inputs.released & analog_input;
}

static bool any_input_pressed()
{
    return s_merged_analog_inputs.pressed | s_merged_digital_inputs.pressed;
}

static void update_konami()
{
    if (s_konami_progress >= 11)
    {
        s_konami_progress = 0;
    }

    if (!any_input_pressed()) return;

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
            if (button_pressed(gc::PAD_BUTTON_B)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        case 9:
        {
            if (button_pressed(gc::PAD_BUTTON_A)) s_konami_progress++;
            else s_konami_progress = 0;
            break;
        }
        case 10:
        {
            if (button_pressed(gc::PAD_BUTTON_START)) s_konami_progress++;
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

bool button_chord_pressed(u16 btn1, u16 btn2)
{
    return (button_down(btn1) && button_pressed(btn2)) || (button_pressed(btn1) && button_down(btn2));
}
s32 get_cstick_dir()
{
    bool left = analog_down(mkb::PAI_CSTICK_LEFT);
    bool right = analog_down(mkb::PAI_CSTICK_RIGHT);
    bool up = analog_down(mkb::PAI_CSTICK_UP);
    bool down = analog_down(mkb::PAI_CSTICK_DOWN);

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

bool dir_down(u16 dir)
{
    switch (dir)
    {
        case DIR_UP:
        {
            return button_down(gc::PAD_BUTTON_UP) || analog_down(mkb::PAI_LSTICK_UP);
        }
        case DIR_LEFT:
        {
            return button_down(gc::PAD_BUTTON_LEFT) || analog_down(mkb::PAI_LSTICK_LEFT);
        }
        case DIR_RIGHT:
        {
            return button_down(gc::PAD_BUTTON_RIGHT) || analog_down(mkb::PAI_LSTICK_RIGHT);
        }
        case DIR_DOWN:
        {
            return button_down(gc::PAD_BUTTON_DOWN) || analog_down(mkb::PAI_LSTICK_DOWN);
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
            return button_pressed(gc::PAD_BUTTON_UP) || analog_pressed(mkb::PAI_LSTICK_UP);
        }
        case DIR_LEFT:
        {
            return button_pressed(gc::PAD_BUTTON_LEFT) || analog_pressed(mkb::PAI_LSTICK_LEFT);
        }
        case DIR_RIGHT:
        {
            return button_pressed(gc::PAD_BUTTON_RIGHT) || analog_pressed(mkb::PAI_LSTICK_RIGHT);
        }
        case DIR_DOWN:
        {
            return button_pressed(gc::PAD_BUTTON_DOWN) || analog_pressed(mkb::PAI_LSTICK_DOWN);
        }
        default:
        {
            return false;
        }
    }
}

bool konami_pressed()
{
    return s_konami_progress == 11;
}

void set_exclusive_mode(bool enabled)
{
    s_exclusive_mode = enabled;
}

void on_frame_start()
{
//    if (s_exclusive_mode)
//    {
//        mkb::filtered_button_inputs_bitfield = s_filtered_buttons;
//        mkb::filtered_analog_inputs_bitfield = s_filtered_analog;
//        memcpy(mkb::pad_status_groups, s_pad_status_groups, sizeof(mkb::pad_status_groups));
//        memcpy(mkb::g_important_pad_array, s_important_pad_array, sizeof(mkb::g_important_pad_array));
////        mkb::g_some_other_flags = s_some_other_flags;
//    }
}

void on_input_processing_finished()
{
//    s_filtered_buttons = mkb::filtered_button_inputs_bitfield;
//    s_filtered_analog = mkb::filtered_analog_inputs_bitfield;
//    memcpy(s_pad_status_groups, mkb::pad_status_groups, sizeof(mkb::pad_status_groups));
//    memcpy(s_important_pad_array, mkb::g_important_pad_array, sizeof(mkb::g_important_pad_array));
////    s_some_other_flags = mkb::g_some_other_flags;

    s_merged_analog_inputs = mkb::merged_analog_inputs;
    s_merged_digital_inputs = mkb::merged_digital_inputs;
    memcpy(s_pad_status_groups, mkb::pad_status_groups, sizeof(mkb::pad_status_groups));
    memcpy(s_analog_inputs, mkb::analog_inputs, sizeof(mkb::analog_inputs));
//
//    if (s_exclusive_mode)
//    {
//        mkb::filtered_button_inputs_bitfield = 0;
//        mkb::filtered_analog_inputs_bitfield = 0;
//        memset(mkb::pad_status_groups, 0, sizeof(mkb::pad_status_groups));
//
//        // Experimental
//        memset(mkb::g_important_pad_array, 0, sizeof(mkb::g_important_pad_array));
////        mkb::g_some_other_flags = 0;
//        memset(reinterpret_cast<void *>(0x80145210), 0, 0x3C * 4);
//    }

    update_konami();
}

}
