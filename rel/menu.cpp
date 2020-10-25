#include <pad.h>
#include <draw.h>
#include "menu.h"

namespace menu
{

static bool s_enabled;

void tick()
{
    s_enabled ^= pad::button_chord_pressed(pad::BUTTON_Z, pad::BUTTON_RTRIG);
}

void disp()
{
    if (!s_enabled) return;

    constexpr int SCREEN_WIDTH = 640;
    constexpr int SCREEN_HEIGHT = 480;
    constexpr int MARGIN = 20;
    constexpr int PAD = 8;
    constexpr int LINE_HEIGHT = 20;

    draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN, {0x00, 0x00, 0x00, 0xd0});

    draw::debug_text(MARGIN + draw::DEBUG_CHAR_WIDTH * 16, MARGIN + PAD, draw::Color::PURPLE, "PRACTICE MOD");
    draw::debug_text(MARGIN + draw::DEBUG_CHAR_WIDTH * 29, MARGIN + PAD, draw::Color::WHITE, "HELP");

    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 3 * LINE_HEIGHT, draw::Color::ORANGE,  "Controller Bindings");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 4 * LINE_HEIGHT, draw::Color::WHITE,   "  R+Z        \x1c Toggle this help menu");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 5 * LINE_HEIGHT, draw::Color::WHITE,   "  X          \x1c Create savestate");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 6 * LINE_HEIGHT, draw::Color::WHITE,   "  Y          \x1c Load savestate");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 7 * LINE_HEIGHT, draw::Color::WHITE,   "  C-Stick    \x1c Change savestate slot");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 8 * LINE_HEIGHT, draw::Color::WHITE,   "  L+X or R+X \x1c Frame advance");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 9 * LINE_HEIGHT, draw::Color::WHITE,   "  L+C or R+C \x1c Browse savestates");

    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 12 * LINE_HEIGHT, draw::Color::ORANGE, "Updates");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 13 * LINE_HEIGHT, draw::Color::WHITE,  "  Current version: v0.1.1-beta");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 14 * LINE_HEIGHT, draw::Color::WHITE,  "  For the newest version of the practice mod:");
    draw::debug_text(MARGIN + PAD, MARGIN + PAD + 15 * LINE_HEIGHT, draw::Color::BLUE,   "  github.com/ComplexPlane/ApeSphere/releases");
}

}