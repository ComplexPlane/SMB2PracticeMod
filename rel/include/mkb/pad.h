#pragma once

#include <gc/pad.h>

namespace mkb
{

enum PadAnalogInput
{ /* Thresholded analog input bits used by SMB2 */
    PAI_LSTICK_LEFT = 0x0001,
    PAI_LSTICK_RIGHT = 0x0002,
    PAI_LSTICK_DOWN = 0x0004,
    PAI_LSTICK_UP = 0x0008,
    PAI_CSTICK_LEFT = 0x0010,
    PAI_CSTICK_RIGHT = 0x0020,
    PAI_CSTICK_DOWN = 0x0040,
    PAI_CSTICK_UP = 0x0080,
    PAI_LTRIG = 0x0100,
    PAI_RTRIG = 0x0200,
};

enum PadInputID
{
    PIID_LEFT = 0,
    PIID_RIGHT = 1,
    PIID_UP = 2,
    PIID_DOWN = 3,
    PIID_L = 4,
    PIID_R = 5,
    PIID_A = 6,
    PIID_B = 7,
    PIID_X = 8
};

struct PadStatusGroup
{ /* A set of PADStatus structs for a given controller, with each representing a different "filtered" version of the inputs */
    gc::PADStatus raw; /* The raw PADStatus read by PADRead() for the controller */
    gc::PADStatus prev_tick; /* The PADStatus from the previous frame */
    gc::PADStatus pressed; /* PADStatus representing the digital inputs that were just pressed this frame */
    gc::PADStatus released; /* PADStatus representing the digital inputs that were released this frame */
    gc::PADStatus repeated; /* PADStatus of digital inputs that "repeat" similar to holding a key down on a keyboard: bit is high on first press, then there's a pause of many frames, then the bit is high every 4 frames */
};

static_assert(sizeof(PadStatusGroup) == 0x3C);

struct DigitalInputGroup
{ /* A PadDigitalIinput u16 bitfield for corresponding to the five types of PadStatusGroup */
    u16 raw;
    u16 prev_tick;
    u16 pressed;
    u16 released;
    u16 repeated;
};

static_assert(sizeof(DigitalInputGroup) == 0xA);

struct AnalogInputGroup
{ /* A PadAnalogInput u16 bitfield for corresponding to the five types of PadStatusGroup */
    u16 raw;
    u16 prev_tick;
    u16 pressed;
    u16 released;
    u16 repeated;
};

static_assert(sizeof(AnalogInputGroup) == 0xA);

extern "C"
{

extern AnalogInputGroup merged_analog_inputs;
extern DigitalInputGroup merged_digital_inputs;
extern AnalogInputGroup analog_inputs[4];
extern PadStatusGroup pad_status_groups[4];

void g_process_inputs(void);

}

}
