#include "timer.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "mods/validate.h"
#include "systems/pref.h"
#include "systems/textinfo.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

namespace timer {

static u32 s_retrace_count;
static u32 s_prev_retrace_count;
static s32 s_rta_timer;
static s32 s_pause_timer;

void init() {
    s_retrace_count = mkb::VIGetRetraceCount();
}

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
    } else if ((mkb::mode_info.ball_mode & mkb::BALLMODE_FREEZE_TIMER) == 0) {
        s_rta_timer -= s_retrace_count - s_prev_retrace_count;
        //        if (s_rtaTimer < 0) s_rtaTimer = 0;
        if (mkb::g_some_other_flags & mkb::OF_GAME_PAUSED) {
            s_pause_timer++;
        }
    }

    using Mod = textinfo::Module;
    using Slot = textinfo::Slot;
    using Format = timerdisp::TimeFormat;

    if (pref::get(pref::Pref::TimerShowRTA) && !freecam::should_hide_hud()) {
        /*  textinfo::draw_timer(Mod::StageTimer, Slot::Right, draw::WHITE, "RTA:", s_rta_timer,
                              Format::SecondsOnly); */
        textinfo::draw_timer_new(Slot::Right, draw::WHITE, "RTA:", s_rta_timer,
                                 Format::SecondsOnly);
    }

    if (pref::get(pref::Pref::TimerShowPause) && !freecam::should_hide_hud()) {
        /* textinfo::draw_timer(Mod::StageTimer, Slot::Right, draw::WHITE, "PAU:", s_pause_timer,
                             Format::SecondsOnly); */
        textinfo::draw_timer_new(Slot::Right, draw::WHITE, "PAU:", s_pause_timer,
                                 Format::SecondsOnly);
    }

    switch (mkb::sub_mode) {
        case mkb::SMD_GAME_GOAL_INIT:
        case mkb::SMD_GAME_GOAL_MAIN:
        case mkb::SMD_GAME_GOAL_REPLAY_INIT:
        case mkb::SMD_GAME_GOAL_REPLAY_MAIN:
            break;
        default:
            return;
    }

    u32 framesave = validate::get_framesave();

    if (pref::get(pref::Pref::TimerShowSubtick) && !freecam::should_hide_hud()) {
        /* textinfo::draw_subtick_timer(Mod::StageTimer, Slot::Right, draw::WHITE, "SUB:",
           s_rta_timer, framesave, false); */
    }

    if (pref::get(pref::Pref::TimerShowFramesave) && !freecam::should_hide_hud()) {
        s32 num_x = textinfo::module_and_slot_to_x_alignment(Mod::StageTimer, Slot::Right);
        s32 x = num_x - 4 * draw::DEBUG_CHAR_WIDTH;  // "FSV:" is 4 characters long
        // textinfo::draw(Mod::StageTimer, Slot::Right, x, draw::WHITE, true, "FSV:%2d%",
        // framesave);
        textinfo::draw_new(Slot::Right, x, draw::WHITE, true, "FSV:%2d%", framesave);
    }
}

void save_state(store::Store *store, store::StoreFunc func) {
    func(store, &s_retrace_count, sizeof(s_retrace_count));
    func(store, &s_prev_retrace_count, sizeof(s_prev_retrace_count));
    func(store, &s_rta_timer, sizeof(s_rta_timer));
    func(store, &s_pause_timer, sizeof(s_pause_timer));
    if (func == store::load) {
        // This might not be completely consistently correct
        u32 count = mkb::VIGetRetraceCount();
        s_prev_retrace_count = count - 1;
        s_retrace_count = count - 1;
    }
}

}  // namespace timer
