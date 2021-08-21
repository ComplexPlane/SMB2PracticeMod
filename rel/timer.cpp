#include "timer.h"

#include <mkb.h>
#include <timerdisp.h>

#include "draw.h"

namespace timer {

// Enabled for the timer just means "is drawing on the screen"; it's always running
// regardless to prevent fudging IL RTA times
static bool s_visible = true;

static u32 s_retrace_count;
static u32 s_prev_retrace_count;
static s32 s_rta_timer;

void init() { s_retrace_count = mkb::VIGetRetraceCount(); }

void set_visible(bool visible) { s_visible = visible; }
bool is_visible() { return s_visible; }

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
            return;
    }

    s_prev_retrace_count = s_retrace_count;
    s_retrace_count = mkb::VIGetRetraceCount();

    if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
        s_rta_timer = mkb::mode_info.stage_time_limit;
    } else if ((mkb::mode_info.ball_mode & mkb::BALLMODE_FREEZE_TIMER) == 0) {
        s_rta_timer -= s_retrace_count - s_prev_retrace_count;
        //        if (s_rtaTimer < 0) s_rtaTimer = 0;
    }

    if (s_visible) {
        timerdisp::draw_timer(s_rta_timer, "RTA:", 1, draw::WHITE, true);
        timerdisp::draw_timer(mkb::mode_info.stage_time_frames_remaining - s_rta_timer, "PAU:", 2,
                              draw::WHITE, true);
    }
}

void save_state(memstore::MemStore* store) {
    store->do_region(&s_retrace_count, sizeof(s_retrace_count));
    store->do_region(&s_prev_retrace_count, sizeof(s_prev_retrace_count));
    store->do_region(&s_rta_timer, sizeof(s_rta_timer));
    if (store->get_mode() == memstore::Mode::LOAD) {
        // This might not be completely consistently correct
        u32 count = mkb::VIGetRetraceCount();
        s_prev_retrace_count = count - 1;
        s_retrace_count = count - 1;
    }
}

}  // namespace timer