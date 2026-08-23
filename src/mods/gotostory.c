#include "mods/gotostory.h"

#include "utils/base.h"

typedef enum {
    State_Default,
    State_LoadMenuReq,
    State_LoadStoryReq,
} State;

static State s_state = State_Default;

void GoToStory_LoadStoryMode() {
    if (mkb_main_mode == mkb_MD_SEL) {
        s_state = State_LoadStoryReq;
    } else {
        // Mainloop may not be loaded, so loading storymode may cause main_game.rel to be
        // loaded in a different place
        // So, load the main menu first
        s_state = State_LoadMenuReq;
    }
}

static void reset_screenfade_state() {
    // Reset screenfade parameters to that of "begin fading back from black screen"
    mkb_g_screenfade_flags = 0x00000100;
    mkb_g_screenfade_color = 0x00000000;
    mkb_g_screenfading1 = 0x0000001a;
    mkb_g_screenfading2 = 0x0000001b;
}

void GoToStory_Tick() {
    if (s_state == State_LoadMenuReq) {
        mkb_g_some_other_flags &=
            ~mkb_OF_GAME_PAUSED;  // Unpause the game to avoid weird darkening issues
        mkb_main_mode_request = mkb_MD_SEL;
        // Using REINIT instead of INIT seems to prevent weird game state issues, like
        // the Final Stage sprite being shown when loading a stage in story mode
        mkb_sub_mode_request = mkb_SMD_SEL_NGC_REINIT;
        s_state = State_LoadStoryReq;

        // Set menu state to have chosen Main Game -> Story Mode
        mkb_sel_menu_info.menu_stack_ptr = 1;
        mkb_sel_menu_info.menu_stack[0] = 0;  // ??
        mkb_sel_menu_info.menu_stack[1] = 7;  // Main game
        mkb_g_focused_root_menu = 0;
        mkb_g_focused_maingame_menu = 0;

        reset_screenfade_state();
    } else if (s_state == State_LoadStoryReq) {
        mkb_main_mode_request = mkb_MD_GAME;
        mkb_sub_mode_request = mkb_SMD_GAME_SCENARIO_INIT;

        // Reset this timer to prevent dark pause menu when leaving an Exit Game menu. The game
        // draws the pause menu sprite at a different depth depending on this global, which can
        // incorrectly place it behind the pause menu dim quad.
        mkb_g_playpoint_msg_counter = 0;

        s_state = State_Default;
        reset_screenfade_state();
    }
}
