#include "mods/fallout.h"
#include "mods/freecam.h"
#include "systems/log.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

typedef enum FalloutPlaneType FalloutPlaneType;
enum FalloutPlaneType {
    FalloutPlaneType_Normal,
    FalloutPlaneType_Disabled,
    FalloutPlaneType_Bouncy,
};

typedef enum TimerType TimerType;
enum TimerType {
    TimerType_Default,
    TimerType_FreezeInstantly,
    TimerType_FreezeAtZero,
    TimerType_CountUpwards,
    TimerType_Invalid  // this state is used to determine when
};

static mkb_BOOL32 did_ball_fallout_hook(mkb_Ball *ball);
TRAMP(s_did_ball_fallout_tramp, mkb_did_ball_fallout, did_ball_fallout_hook);

static TimerType s_prev_pref = TimerType_Invalid;
static TimerType s_prev_freecam = TimerType_Invalid;

static bool s_halted;  // freeze timer for TimerType_FreezeAtZero

void fallout_init() {
    // stop fallouts
    HOOK_TRAMP(s_did_ball_fallout_tramp);
}

static mkb_BOOL32 did_ball_fallout_hook(mkb_Ball *ball) {
    mkb_BOOL32 orig_result = s_did_ball_fallout_tramp.chain(ball);
    bool below_fallout = ball->pos.y < mkb_stagedef->fallout->y;
    bool volumes_disabled = pref_get(Pref_DisableFalloutVolumes);

    switch ((FalloutPlaneType)(pref_get(Pref_FalloutPlaneType))) {
    case FalloutPlaneType_Normal: {
        if (volumes_disabled) {
            return (mkb_BOOL32)(below_fallout);
        } else {
            return (mkb_BOOL32)(orig_result);
        }
        break;
    }
    case FalloutPlaneType_Disabled: {
        if (below_fallout) {
            return (mkb_BOOL32)(false);
        } else if (volumes_disabled) {
            return (mkb_BOOL32)(false);
        }
        break;
    }
    case FalloutPlaneType_Bouncy: {
        if (below_fallout) {
            ball->vel.y = ABS(ball->vel.y) * 1.05;
            return (mkb_BOOL32)(false);
        } else if (volumes_disabled) {
            return (mkb_BOOL32)(false);
        }
        break;
    }
    }

    return orig_result;
}

void freeze_timer() {
    TimerType current_pref = (TimerType)(pref_get(Pref_TimerType));
    bool update_timer_incr = mkb_sub_mode == mkb_SMD_GAME_READY_INIT || current_pref != s_prev_pref;
    s_prev_pref = current_pref;

    switch (current_pref) {
    case TimerType_Default: {
        // time over at 0 frames
        *(u32 *)(0x80297548) = 0x2c000000;
        // add -1 to timer each frame
        if (update_timer_incr) {
            patch_write_word((u32 *)(0x80297534), 0x3803ffff);
        }
        break;
    }
    case TimerType_FreezeInstantly: {
        // time over at 0 frames
        *(u32 *)(0x80297548) = 0x2c000000;
        // add 0 to timer each frame (timer doesnt move)
        if (update_timer_incr) {
            patch_write_word((u32 *)(0x80297534), 0x38030000);
        }
        break;
    }
    case TimerType_FreezeAtZero: {
        // time over at -60 frames (so timer is able to stop at 0.00)
        *(u32 *)(0x80297548) = 0x2c00ffa0;
        // add -1 to timer each frame (will need to freeze timer at 0.00 and unfreeze on retry)
        if (update_timer_incr) {
            patch_write_word((u32 *)(0x80297534), 0x3803ffff);
        }

        // when timer hits 0, add 0 to timer each frame
        if (mkb_mode_info.stage_time_frames_remaining <= 0 && !s_halted) {
            patch_write_word((u32 *)(0x80297534), 0x38030000);
            s_halted = true;
        }
        // when timer is reset on retry, add -1 to timer each frame
        else if (mkb_mode_info.stage_time_frames_remaining > 0 && s_halted) {
            patch_write_word((u32 *)(0x80297534), 0x3803ffff);
            s_halted = false;
        }
        break;
    }
    case TimerType_CountUpwards: {
        if (mkb_sub_mode == mkb_SMD_GAME_READY_INIT) {
            mkb_mode_info.stage_time_frames_remaining = 0;
        }
        // time over at -60 frames (so timer is able to stop at 0.00)
        *(u32 *)(0x80297548) = 0x2c00ffa0;
        // add 1 to timer each frame
        if (update_timer_incr) {
            patch_write_word((u32 *)(0x80297534), 0x38030001);
        }
        break;
    }
    case TimerType_Invalid: {
        break;
    }
    }
}

void fallout_tick() {
    if (freecam_should_freeze_timer() && s_prev_freecam == TimerType_Invalid) {
        s_prev_freecam = (TimerType)(pref_get(Pref_TimerType));
        pref_set(Pref_TimerType, 1);
    } else if (!freecam_should_freeze_timer() && s_prev_freecam != TimerType_Invalid) {
        pref_set(Pref_TimerType, (u8)(s_prev_freecam));
        s_prev_freecam = TimerType_Invalid;
    }
    freeze_timer();
}
void fallout_disp() {
}
