#include "menu.h"

#include <mkb/mkb.h>

#include "jump.h"
#include "pad.h"
#include "draw.h"
#include "timer.h"
#include "savestate.h"
#include "iw.h"

constexpr f32 clamp_256(f32 x) { return x < 0 ? 0 : (x > 255 ? 255 : x); }

namespace menu
{

constexpr s32 SCREEN_WIDTH = 640;
constexpr s32 SCREEN_HEIGHT = 480;
constexpr s32 MARGIN = 20;
constexpr s32 PAD = 8;
constexpr s32 LINE_HEIGHT = 20;

static s32 s_cursor_pos = 0;
static u32 s_cursor_frame = 0;

//enum class MenuID
//{
//    ROOT,
//    MODS,
//    ABOUT,
//};
//
//struct MenuState
//{
//    MenuID id;
//    s32 cursor_pos;
//};

static bool s_visible;

//static MenuState s_menu_stack[4] = {{MenuID::ROOT, 0}};
//static s32 s_menu_stack_ptr = 0;

static bool are_practice_tools_enabled()
{
    return timer::is_enabled() && savestate::is_enabled() && iw::is_enabled();
}

static void handle_options()
{
    if (pad::button_pressed(gc::PAD_BUTTON_A, true))
    {
        if (s_cursor_pos == 0)
        {
            if (are_practice_tools_enabled())
            {
                timer::dest();
                savestate::dest();
                iw::dest();
            }
            else
            {
                timer::init();
                savestate::init();
                iw::init();
            }
        }
        else if (s_cursor_pos == 1)
        {
            if (jump::is_enabled()) jump::dest();
            else jump::init();
        }
    }
}

void tick()
{
    bool toggle = false;
    if (pad::button_pressed(gc::PAD_BUTTON_B, true))
    {
        s_visible = false;
    }
    else
    {
        toggle = pad::button_chord_pressed(gc::PAD_TRIGGER_L, gc::PAD_TRIGGER_R, true);
        s_visible ^= toggle;
    }
    bool just_opened = s_visible && toggle;

    pad::set_exclusive_mode(s_visible);

    if (!s_visible) return;

    s32 dir_delta = pad::dir_pressed(pad::DIR_UP, true) - pad::dir_pressed(pad::DIR_DOWN, true);
    s_cursor_pos += dir_delta;
    if (s_cursor_pos < 0) s_cursor_pos += 2;
    else if (s_cursor_pos > 1) s_cursor_pos -= 2;

    if (dir_delta != 0 || just_opened) s_cursor_frame = 0;
    else s_cursor_frame++;

    handle_options();
}

//void root_menu()
//{
//    // TODO
//    void about_menu();
//    about_menu();
//}
//
//void mods_menu()
//{
//    // TODO
//}

static gc::GXColor lerp_colors(gc::GXColor color1, gc::GXColor color2, f32 t)
{
    f32 r = (1.f - t) * color1.r + t * color2.r;
    f32 g = (1.f - t) * color1.g + t * color2.g;
    f32 b = (1.f - t) * color1.b + t * color2.b;
    f32 a = (1.f - t) * color1.a + t * color2.a;

    gc::GXColor ret;
    ret.r = clamp_256(r);
    ret.g = clamp_256(g);
    ret.b = clamp_256(b);
    ret.a = clamp_256(a);

    return ret;
}

static f32 sin_lerp(s32 period_frames)
{
    f32 angle = (static_cast<s32>(s_cursor_frame % period_frames) - (period_frames / 2)) * 0x8000 / (period_frames / 2);
    f32 lerp = (mkb::math_sin(angle) + 1.f) / 2.f;
    return lerp;
}

void about_menu()
{
    draw::debug_text(MARGIN + draw::DEBUG_CHAR_WIDTH * 17, MARGIN + PAD, draw::Color::PURPLE, "APESPHERE");
    draw::debug_text(MARGIN + draw::DEBUG_CHAR_WIDTH * 27, MARGIN + PAD, draw::Color::WHITE, "MENU");

    gc::GXColor light_green = {0xad, 0xff, 0xa6, 0xff};
    gc::GXColor unfocused = {0xa2, 0xad, 0xff, 0xff};

    gc::GXColor lerped_color = lerp_colors(light_green, unfocused, sin_lerp(40));

    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 1.5f * LINE_HEIGHT, draw::Color::ORANGE, "Available Mods");
    draw::debug_text(
        MARGIN + PAD,
        MARGIN + PAD + 2.5f * LINE_HEIGHT,
        s_cursor_pos == 0 ? lerped_color : unfocused,
        "  Practice Tools:     %s", are_practice_tools_enabled() ? "Enabled" : "Disabled");

    draw::debug_text(
        MARGIN + PAD,
        MARGIN + PAD + 3.5f * LINE_HEIGHT,
        s_cursor_pos == 1 ? lerped_color : unfocused,
        "  Jump Mod:           %s", jump::is_enabled() ? "Enabled" : "Disabled");

    // Draw selection arrow
    s32 height_offset = s_cursor_pos == 0 ? 0 : LINE_HEIGHT;
    draw::debug_text(MARGIN + PAD + 2, MARGIN + PAD + 2.5f * LINE_HEIGHT + height_offset, light_green, "\x1c");

    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 5 * LINE_HEIGHT, draw::Color::ORANGE, "Practice Tools Bindings");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 6 * LINE_HEIGHT, draw::Color::WHITE, "  L+R        \x1c Toggle this menu");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 7 * LINE_HEIGHT, draw::Color::WHITE, "  X          \x1c Create savestate");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 8 * LINE_HEIGHT, draw::Color::WHITE, "  Y          \x1c Load savestate");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 9 * LINE_HEIGHT, draw::Color::WHITE, "  C-Stick    \x1c Change savestate slot");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 10 * LINE_HEIGHT, draw::Color::WHITE, "  L+X or R+X \x1c Frame advance");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 11 * LINE_HEIGHT, draw::Color::WHITE, "  L+C or R+C \x1c Browse savestates");

    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 12.5f * LINE_HEIGHT, draw::Color::ORANGE, "Jump Mod Bindings");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 13.5f * LINE_HEIGHT, draw::Color::WHITE, "  A          \x1c Jump");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 14.5f * LINE_HEIGHT, draw::Color::WHITE, "  B          \x1c Resize minimap");

    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 16 * LINE_HEIGHT, draw::Color::ORANGE, "Updates");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 17 * LINE_HEIGHT, draw::Color::WHITE, "  Current version: v0.2.0");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 18 * LINE_HEIGHT, draw::Color::WHITE, "  For the newest version of the practice mod:");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 19 * LINE_HEIGHT, draw::Color::WHITE, "  github.com/ComplexPlane/ApeSphere/releases");
}

//static void (*const menu_funcs[])() = {
//    root_menu,
//    mods_menu,
//    about_menu,
//};

// How many selectable items are there in the given menu?
//static const s32 s_menu_items[] = {
//    2,
//    0,
//    0,
//};

void disp()
{
    if (!s_visible) return;

    draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN, {0x00, 0x00, 0x00, 0xd0});
    about_menu();

//    MenuState &menu = s_menu_stack[s_menu_stack_ptr];
//
//    // Advance menu
//    s32 num_menu_items = s_menu_items[static_cast<s32>(menu.id)];
//    if (pad::dir_pressed(pad::DIR_DOWN, true))
//    {
//        menu.cursor_pos = (menu.cursor_pos + 1) % num_menu_items;
//    }
//    else if (pad::dir_pressed(pad::DIR_UP, true))
//    {
//        menu.cursor_pos = (menu.cursor_pos + num_menu_items - 1) % num_menu_items;
//    }
//
//    draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN, {0x00, 0x00, 0x00, 0xd0});
//
//    menu_funcs[static_cast<s32>(menu.id)]();
}

}