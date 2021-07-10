#include "gotostory.h"

#include <mkb.h>

namespace gotostory
{

enum class State
{
    Default,
    LoadMainReq,
    LoadStoryReq,
};

static State s_state = State::Default;

void load_storymode()
{
    // Mainloop may not be loaded, so loading storymode may cause main_game.rel to be
    // loaded in a different place
    // So, load the main menu first
    s_state = State::LoadMainReq;
}

static void reset_screenfade_state()
{
    // Reset screenfade parameters to that of "begin fading back from black screen"
    mkb::g_screenfade_flags = 0x00000100;
    mkb::g_screenfade_color = 0x00000000;
    mkb::g_screenfading1 = 0x0000001a;
    mkb::g_screenfading2 = 0x0000001b;
}

void tick()
{
    if (s_state == State::LoadMainReq)
    {
        mkb::main_mode_request = mkb::MD_SEL;
        mkb::sub_mode_request = mkb::SMD_SEL_NGC_INIT;
        s_state = State::LoadStoryReq;
        reset_screenfade_state();
    }
    else if (s_state == State::LoadStoryReq)
    {
        mkb::main_mode_request = mkb::MD_GAME;
        mkb::sub_mode_request = mkb::SMD_GAME_SCENARIO_INIT;
        s_state = State::Default;

        // Set menu state to have chosen Main Game -> Story Mode
        mkb::menu_stack_ptr = 2;
        mkb::g_menu_stack[0] = 0; // Main game menu
        mkb::g_menu_stack[1] = 7; // Story mode menu
        mkb::g_focused_root_menu = 0;
        mkb::g_focused_maingame_menu = 0;
        reset_screenfade_state();
    }
}

}
