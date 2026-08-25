#include "mods/timer.h"

#include "mods/freecam.h"
#include "mods/validate.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/draw.h"
#include "utils/timerdisp.h"

static u32 s_retrace_count;
static u32 s_prev_retrace_count;
static s32 s_rta_timer;
static s32 s_pause_timer;

void Timer_Init() {
    s_retrace_count = mkb_VIGetRetraceCount();
}

// Need to do logic in disp() so that we can know the game state _after_ the frame has processed
void Timer_Disp() {
    if (mkb_main_mode != mkb_MD_GAME) return;

    switch (mkb_sub_mode) {
        case mkb_SMD_GAME_READY_INIT:
        case mkb_SMD_GAME_READY_MAIN:
        case mkb_SMD_GAME_PLAY_INIT:
        case mkb_SMD_GAME_PLAY_MAIN:
        case mkb_SMD_GAME_GOAL_INIT:
        case mkb_SMD_GAME_GOAL_MAIN:
        case mkb_SMD_GAME_RINGOUT_INIT:
        case mkb_SMD_GAME_RINGOUT_MAIN:
        case mkb_SMD_GAME_TIMEOVER_INIT:
        case mkb_SMD_GAME_TIMEOVER_MAIN:
        case mkb_SMD_GAME_GOAL_REPLAY_INIT:
        case mkb_SMD_GAME_GOAL_REPLAY_MAIN:
            break;
        default:
            s_pause_timer = 0;
            return;
    }

    s_prev_retrace_count = s_retrace_count;
    s_retrace_count = mkb_VIGetRetraceCount();

    if (mkb_sub_mode == mkb_SMD_GAME_READY_INIT) {
        s_rta_timer = mkb_mode_info.stage_time_limit;
        s_pause_timer = 0;
    } else if ((mkb_mode_info.ball_mode & mkb_BALLMODE_FREEZE_TIMER) == 0) {
        s_rta_timer -= s_retrace_count - s_prev_retrace_count;
        //        if (s_rtaTimer < 0) s_rtaTimer = 0;
        if (mkb_g_some_other_flags & mkb_OF_GAME_PAUSED) {
            s_pause_timer++;
        }
    }

    u32 row = 1;

    if (Pref_Get(Pref_TimerShowRTA) && !Freecam_ShouldHideHud()) {
        TimerDisp_DrawTimer(s_rta_timer, "RTA:", row++, COLOR_WHITE, true);
    }

    if (Pref_Get(Pref_TimerShowPause) && !Freecam_ShouldHideHud()) {
        TimerDisp_DrawTimer(s_pause_timer, "PAU:", row++, COLOR_WHITE, true);
    }

    switch (mkb_sub_mode) {
        case mkb_SMD_GAME_GOAL_INIT:
        case mkb_SMD_GAME_GOAL_MAIN:
        case mkb_SMD_GAME_GOAL_REPLAY_INIT:
        case mkb_SMD_GAME_GOAL_REPLAY_MAIN:
            break;
        default:
            return;
    }

    u32 framesave = Validate_GetFramesave();

    if (Pref_Get(Pref_TimerShowSubtick) && !Freecam_ShouldHideHud()) {
        TimerDisp_DrawSubtickTimer(mkb_mode_info.stage_time_frames_remaining, "SUB:", row++,
                                   COLOR_WHITE, true, framesave, false);
    }

    if (Pref_Get(Pref_TimerShowFramesave) && !Freecam_ShouldHideHud()) {
        TimerDisp_DrawPercentage(framesave, "FSV:", row++, COLOR_WHITE);
    }
}

void Timer_SaveState(Store *store) {
    Store_DoPass(store, &s_retrace_count, sizeof(s_retrace_count));
    Store_DoPass(store, &s_prev_retrace_count, sizeof(s_prev_retrace_count));
    Store_DoPass(store, &s_rta_timer, sizeof(s_rta_timer));
    Store_DoPass(store, &s_pause_timer, sizeof(s_pause_timer));
    if (store->state == Store_State_Load) {
        // This might not be completely consistently correct
        u32 count = mkb_VIGetRetraceCount();
        s_prev_retrace_count = count - 1;
        s_retrace_count = count - 1;
    }
}
