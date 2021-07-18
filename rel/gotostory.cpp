#include "gotostory.h"

#include <mkb.h>

namespace gotostory
{

enum class State
{
    Default,
    LoadMenuReq,
    LoadStoryReq,
};

static State s_state = State::Default;

void load_storymode()
{
    if (mkb::main_mode == mkb::MD_SEL) {
        s_state = State::LoadStoryReq;
    } else {
        // Mainloop may not be loaded, so loading storymode may cause main_game.rel to be
        // loaded in a different place
        // So, load the main menu first
        s_state = State::LoadMenuReq;
    }
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
    if (s_state == State::LoadMenuReq)
    {
        mkb::g_some_other_flags &= ~mkb::OF_GAME_PAUSED; // Unpause the game to avoid weird darkening issues
        mkb::main_mode_request = mkb::MD_SEL;
        // Using REINIT instead of INIT seems to prevent weird game state issues, like
        // the Final Stage sprite being shown when loading a stage in story mode
        mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;
        s_state = State::LoadStoryReq;

        // Set menu state to have chosen Main Game -> Story Mode
        mkb::menu_stack_ptr = 1;
        mkb::g_menu_stack[0] = 0; // ??
        mkb::g_menu_stack[1] = 7; // Main game
        mkb::g_focused_root_menu = 0;
        mkb::g_focused_maingame_menu = 0;

        reset_screenfade_state();
    }
    else if (s_state == State::LoadStoryReq)
    {
        mkb::main_mode_request = mkb::MD_GAME;
        mkb::sub_mode_request = mkb::SMD_GAME_SCENARIO_INIT;
        s_state = State::Default;
        reset_screenfade_state();
    }
}

}
