#include <pad.h>
#include <draw.h>
#include "menu.h"

namespace menu
{

constexpr s32 SCREEN_WIDTH = 640;
constexpr s32 SCREEN_HEIGHT = 480;
constexpr s32 MARGIN = 20;
constexpr s32 PAD = 8;
constexpr s32 LINE_HEIGHT = 20;

enum class MenuID
{
    ROOT,
    MODS,
    ABOUT,
};

struct MenuState
{
    MenuID id;
    s32 cursor_pos;
};

static bool s_visible;

static MenuState s_menu_stack[4] = {{MenuID::ROOT, 0}};
static s32 s_menu_stack_ptr = 0;

void tick()
{
    s_visible ^= pad::button_chord_pressed(pad::BUTTON_Z, pad::BUTTON_RTRIG);
}

void root_menu()
{
    // TODO
    void about_menu();
    about_menu();
}

void mods_menu()
{
    // TODO
}

void about_menu()
{
    draw::debug_text(MARGIN + draw::DEBUG_CHAR_WIDTH * 16, MARGIN + PAD, draw::Color::PURPLE, "PRACTICE MOD");
    draw::debug_text(MARGIN + draw::DEBUG_CHAR_WIDTH * 29, MARGIN + PAD, draw::Color::WHITE, "HELP");

    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 3 * LINE_HEIGHT, draw::Color::ORANGE, "Controller Bindings");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 4 * LINE_HEIGHT, draw::Color::WHITE, "  R+Z        \x1c Toggle this help menu");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 5 * LINE_HEIGHT, draw::Color::WHITE, "  X          \x1c Create savestate");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 6 * LINE_HEIGHT, draw::Color::WHITE, "  Y          \x1c Load savestate");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 7 * LINE_HEIGHT, draw::Color::WHITE, "  C-Stick    \x1c Change savestate slot");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 8 * LINE_HEIGHT, draw::Color::WHITE, "  L+X or R+X \x1c Frame advance");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 9 * LINE_HEIGHT, draw::Color::WHITE, "  L+C or R+C \x1c Browse savestates");

    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 12 * LINE_HEIGHT, draw::Color::ORANGE, "Updates");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 13 * LINE_HEIGHT, draw::Color::WHITE, "  Current version: v0.1.3-beta");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 14 * LINE_HEIGHT, draw::Color::WHITE, "  For the newest version of the practice mod:");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 15 * LINE_HEIGHT, draw::Color::BLUE, "  github.com/ComplexPlane/ApeSphere/releases");
}

static void (*const menu_funcs[])() = {
    root_menu,
    mods_menu,
    about_menu,
};

// How many selectable items are there in the given menu?
static const s32 s_menu_items[] = {
    2,
    0,
    0,
};

void disp()
{
    if (!s_visible) return;

    MenuState &menu = s_menu_stack[s_menu_stack_ptr];

    // Advance menu
    s32 num_menu_items = s_menu_items[static_cast<s32>(menu.id)];
    if (pad::dir_pressed(pad::DIR_DOWN))
    {
        menu.cursor_pos = (menu.cursor_pos + 1) % num_menu_items;
    }
    else if (pad::dir_pressed(pad::DIR_UP))
    {
        menu.cursor_pos = (menu.cursor_pos + num_menu_items - 1) % num_menu_items;
    }

    draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN, {0x00, 0x00, 0x00, 0xd0});

    menu_funcs[static_cast<s32>(menu.id)]();
}

}