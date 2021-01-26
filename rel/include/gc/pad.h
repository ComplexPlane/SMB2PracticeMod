#pragma once

namespace gc
{

struct PADStatus
{
    u16 button; /* Or-ed PAD_BUTTON_* and PAD_TRIGGER_* bits */
    s8 stickX; /* -128 <= stickX       <= 127 */
    s8 stickY; /* -128 <= stickY       <= 127 */
    s8 substickX; /* -128 <= substickX    <= 127 */
    s8 substickY; /* -128 <= substickY    <= 127 */
    u8 triggerLeft; /*    0 <= triggerLeft  <= 255 */
    u8 triggerRight; /*    0 <= triggerRight <= 255 */
    u8 analogA; /*    0 <= analogA      <= 255 */
    u8 analogB; /*    0 <= analogB      <= 255 */
    s8 err; /* one of PAD_ERR_* number */
    u8 field_0xb;
};

static_assert(sizeof(PADStatus) == 0xc);

enum PadDigitalInput
{ /* These are normally just #defines in the SDK's PAD library */
    PAD_BUTTON_LEFT = 1,
    PAD_BUTTON_RIGHT = 2,
    PAD_BUTTON_DOWN = 4,
    PAD_BUTTON_UP = 8,
    PAD_TRIGGER_Z = 16,
    PAD_TRIGGER_R = 32,
    PAD_TRIGGER_L = 64,
    PAD_BUTTON_A = 256,
    PAD_BUTTON_B = 512,
    PAD_BUTTON_X = 1024,
    PAD_BUTTON_Y = 2048,
    PAD_BUTTON_START = 4096
};

enum PadError
{ /* These are normally just #defines in the SDK's PAD library. Also these are supposed to be signed */
    PAD_ERR_NOT_READY = 254,
    PAD_ERR_TRANSFER = 253,
    PAD_ERR_NO_CONTROLLER = 255,
    PAD_ERR_NONE = 0
};

}