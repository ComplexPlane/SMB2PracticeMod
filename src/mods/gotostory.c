#include "mods/gotostory.h"

#include "utils/base.h"

typedef enum State State;
enum State {
    State_Default,
    State_LoadMenuReq,
    State_LoadStoryReq,
};

static State s_state;

void gotostory_load_storymode(void) {
    s_state = mkb_main_mode == mkb_MD_SEL ? State_LoadStoryReq : State_LoadMenuReq;
}

static void reset_screenfade_state(void) {
    mkb_g_screenfade_flags = 0x00000100;
    mkb_g_screenfade_color = 0x00000000;
    mkb_g_screenfading1 = 0x0000001a;
    mkb_g_screenfading2 = 0x0000001b;
}

void gotostory_tick(void) {
    if (s_state == State_LoadMenuReq) {
        mkb_g_some_other_flags &= ~mkb_OF_GAME_PAUSED;
        mkb_main_mode_request = mkb_MD_SEL;
        mkb_sub_mode_request = mkb_SMD_SEL_NGC_REINIT;
        s_state = State_LoadStoryReq;
        mkb_sel_menu_info.menu_stack_ptr = 1;
        mkb_sel_menu_info.menu_stack[0] = 0;
        mkb_sel_menu_info.menu_stack[1] = 7;
        mkb_g_focused_root_menu = 0;
        mkb_g_focused_maingame_menu = 0;
        reset_screenfade_state();
    } else if (s_state == State_LoadStoryReq) {
        mkb_main_mode_request = mkb_MD_GAME;
        mkb_sub_mode_request = mkb_SMD_GAME_SCENARIO_INIT;
        mkb_g_playpoint_msg_counter = 0;
        s_state = State_Default;
        reset_screenfade_state();
    }
}
