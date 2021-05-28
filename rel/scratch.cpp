#include "scratch.h"

#include <gc/gc.h>
#include <mkb/mkb.h>
#include <cstring>

#include "patch.h"

namespace scratch
{

static void (*s_PADRead_tramp)(gc::PADStatus *statuses);

static gc::PADStatus s_raw_inputs[4];

static void draw_circle(u32 pts, Vec2f center, f32 radius, gc::GXColor color)
{
    // "Blank" texture object which seems to let us set a color and draw a poly with it idk??
    gc::GXTexObj *texobj = reinterpret_cast<gc::GXTexObj *>(0x807ad0e0);
    mkb::GXLoadTexObj_cached(texobj, gc::GX_TEXMAP0);
    gc::GXSetTevColor(gc::GX_TEVREG0, color);
    float z = -1.0f / 128.0f;

    gc::GXBegin(gc::GX_TRIANGLES, gc::GX_VTXFMT0, pts * 3);

    for (u32 i = 0; i < pts; i++)
    {
        u16 angle = 0xFFFF * i / pts;
        f32 sin_cos[2];
        mkb::math_sin_cos_v(static_cast<s32>(angle), sin_cos);
        f32 x = sin_cos[0] * radius + center.x;
        f32 y = sin_cos[1] * radius + center.y;

        // TODO factor this out or something?
        u16 next_angle = 0xFFFF * ((i + 1) % pts) / pts;
        f32 next_sin_cos[2];
        mkb::math_sin_cos_v(static_cast<s32>(next_angle), next_sin_cos);
        f32 next_x = next_sin_cos[0] * radius + center.x;
        f32 next_y = next_sin_cos[1] * radius + center.y;

        gc::GXPosition3f32(center.x, center.y, z);
        gc::GXTexCoord2f32(0, 0);
        gc::GXPosition3f32(next_x, next_y, z);
        gc::GXTexCoord2f32(0, 1);
        gc::GXPosition3f32(x, y, z);
        gc::GXTexCoord2f32(1, 0);
    }
}

void init()
{
    // Hook PADRead to give us raw PAD inputs before the game processes them
    s_PADRead_tramp = patch::hook_function(
        gc::PADRead, [](gc::PADStatus *statuses)
        {
            s_PADRead_tramp(statuses);
            memcpy(s_raw_inputs, statuses, sizeof(s_raw_inputs));
        }
    );
}

void tick()
{
}

void disp()
{
    draw_circle(8, {100, 100}, 60, {0x00, 0x00, 0x00, 0xFF});
    draw_circle(8, {100, 100}, 58, {0xb1, 0x5a, 0xff, 0xff});
    draw_circle(8, {100, 100}, 50, {0x00, 0x00, 0x00, 0xFF});

    // Accumulate stick inputs from all controllers since we don't always
    // know which player is active, like in menus
    s32 x = 0, y = 0;
    for (int i = 0; i < 4; i++)
    {
        gc::PADStatus &status = s_raw_inputs[i];
        if (status.err == gc::PAD_ERR_NONE)
        {
            x += status.stickX;
            y += status.stickY;
        }
    }

    Vec2f scaled_input = {
        100.f + static_cast<f32>(x) / 2.8f,
        100.f - static_cast<f32>(y) / 2.8f,
    };

    draw_circle(16, scaled_input, 10, {0xFF, 0xFF, 0xFF, 0xFF});
}

}

