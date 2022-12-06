#include "timer.h"

#include "mkb/mkb.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/timerdisp.h"

namespace timer {

static u32 s_retrace_count;
static u32 s_prev_retrace_count;
static s32 s_rta_timer;
static s32 s_pause_timer;

void init() { s_retrace_count = mkb::VIGetRetraceCount(); }

// Need to do logic in disp() so that we can know the game state _after_ the frame has processed
void disp() {
    if (mkb::main_mode != mkb::MD_GAME) return;

    switch (mkb::sub_mode) {
        case mkb::SMD_GAME_READY_INIT:
        case mkb::SMD_GAME_READY_MAIN:
        case mkb::SMD_GAME_PLAY_INIT:
        case mkb::SMD_GAME_PLAY_MAIN:
        case mkb::SMD_GAME_GOAL_INIT:
        case mkb::SMD_GAME_GOAL_MAIN:
        case mkb::SMD_GAME_RINGOUT_INIT:
        case mkb::SMD_GAME_RINGOUT_MAIN:
        case mkb::SMD_GAME_TIMEOVER_INIT:
        case mkb::SMD_GAME_TIMEOVER_MAIN:
        case mkb::SMD_GAME_GOAL_REPLAY_INIT:
        case mkb::SMD_GAME_GOAL_REPLAY_MAIN:
            break;
        default:
            s_pause_timer = 0;
            return;
    }

    s_prev_retrace_count = s_retrace_count;
    s_retrace_count = mkb::VIGetRetraceCount();

    if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
        s_rta_timer = mkb::mode_info.stage_time_limit;
        s_pause_timer = 0;
    } else if ((mkb::mode_info.g_ball_mode & mkb::BALLMODE_FREEZE_TIMER) == 0) {
        s_rta_timer -= s_retrace_count - s_prev_retrace_count;
        //        if (s_rtaTimer < 0) s_rtaTimer = 0;
        if (mkb::g_some_other_flags & mkb::OF_GAME_PAUSED) {
            s_pause_timer++;
        }
    }

    if (pref::get_rta_pause_timer()) {
        timerdisp::draw_timer(s_rta_timer, "RTA:", 1, draw::WHITE, true);
        timerdisp::draw_timer(s_pause_timer, "PAU:", 2, draw::WHITE, true);
    }
}

void save_state(memstore::MemStore* store) {
    store->do_region(&s_retrace_count, sizeof(s_retrace_count));
    store->do_region(&s_prev_retrace_count, sizeof(s_prev_retrace_count));
    store->do_region(&s_rta_timer, sizeof(s_rta_timer));
    store->do_region(&s_pause_timer, sizeof(s_pause_timer));
    if (store->get_mode() == memstore::Mode::LOAD) {
        // This might not be completely consistently correct
        u32 count = mkb::VIGetRetraceCount();
        s_prev_retrace_count = count - 1;
        s_retrace_count = count - 1;
    }
}

}  // namespace timer
