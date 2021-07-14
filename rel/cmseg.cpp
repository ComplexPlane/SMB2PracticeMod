#include "cmseg.h"

#include <mkb.h>

#define ARRAY_LEN(x) (sizeof((x)) / sizeof((x)[0]))

namespace cmseg
{

enum class State
{
    Default,
    LoadMenuReq,
    LoadCmReq,
};

static mkb::CmEntry s_course[ARRAY_LEN(mkb::beginner_noex_cm_entries)];
static State s_state = State::Default;
static Seg s_seg_request;

void init()
{
    // TODO remove
    mkb::OSReport("[mod] Course size: %d\n", sizeof(s_course));
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
        s_state = State::LoadCmReq;
    }
    else if (s_state == State::LoadCmReq)
    {
        mkb::selected_cm_difficulty = 0; // Beginner
        mkb::g_enter_challenge_mode();
        s_state = State::Default;
    }
}

void request_cm_seg(Seg seg)
{
    s_seg_request = seg;
    if (mkb::main_mode == mkb::MD_SEL) s_state = State::LoadCmReq; // Load challenge mode directly
    else s_state = State::LoadMenuReq; // Load main menu first
}

}