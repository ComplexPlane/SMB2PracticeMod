#include "mods/fallout.h"

#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"
#include "utils/relutil.h"

typedef enum {
    FalloutPlaneType_Normal,
    FalloutPlaneType_Disabled,
    FalloutPlaneType_Bouncy,
} FalloutPlaneType;

typedef enum {
    TimerType_Default,
    TimerType_FreezeInstantly,
    TimerType_FreezeAtZero,
    TimerType_CountUpwards,
} TimerType;

static u32 s_timeover_condition = 0x2c000000;  // Timeover at 0.00
static u32 s_timer_increment = 0x3803ffff;     // Add -1 to timer each frame
static bool s_toggled_freecam = false;

// stop fallouts
static mkb_BOOL32 did_ball_fallout_hook(mkb_Ball *ball);

TRAMP(s_did_ball_fallout_tramp, mkb_did_ball_fallout, did_ball_fallout_hook);

static mkb_BOOL32 did_ball_fallout_hook(mkb_Ball *ball) {
    mkb_BOOL32 orig_result = s_did_ball_fallout_tramp.chain(ball);
    bool below_fallout = ball->pos.y < mkb_stagedef->fallout->y;
    bool volumes_disabled = Pref_Get(Pref_DisableFalloutVolumes);

    switch ((FalloutPlaneType)Pref_Get(Pref_FalloutPlaneType)) {
        case FalloutPlaneType_Normal: {
            if (volumes_disabled) {
                return (mkb_BOOL32)below_fallout;
            } else {
                return (mkb_BOOL32)orig_result;
            }
            break;
        }
        case FalloutPlaneType_Disabled: {
            if (below_fallout) {
                return (mkb_BOOL32)false;
            } else if (volumes_disabled) {
                return (mkb_BOOL32)false;
            }
            break;
        }
        case FalloutPlaneType_Bouncy: {
            if (below_fallout) {
                ball->vel.y = ABS(ball->vel.y) * 1.05;
                return (mkb_BOOL32)false;
            } else if (volumes_disabled) {
                return (mkb_BOOL32)false;
            }
            break;
        }
    }

    return orig_result;
}

static void load_stagedef_hook(u32 stage_id);

TRAMP(s_load_stagedef_tramp, mkb_load_stagedef, load_stagedef_hook);

static void load_stagedef_hook(u32 stage_id) {
    // Set the current default values before loading the stagedef
    Patch_WriteWord(Rel_RelocateAddr(0x80297548), s_timeover_condition);
    Patch_WriteWord(Rel_RelocateAddr(0x80297534), s_timer_increment);
    s_load_stagedef_tramp.chain(stage_id);
    // Stardust's custom code sets the timers after loading the stagedef, this will run
    // afterwards and collect those timer defaults
    // For non-Stardust packs, this will simply collect the default values again (and not affect
    // anything)
    s_timeover_condition = *(u32 *)Rel_RelocateAddr(0x80297548);
    s_timer_increment = *(u32 *)Rel_RelocateAddr(0x80297534);
}

void Fallout_Init() {
    HOOK_TRAMP(s_did_ball_fallout_tramp);
    HOOK_TRAMP(s_load_stagedef_tramp);
}

static void freeze_timer() {
    TimerType type = (TimerType)Pref_Get(Pref_TimerType);
    if (Freecam_ShouldFreezeTimer()) {
        type = TimerType_FreezeInstantly;
        s_toggled_freecam = true;
    }

    switch (type) {
        case TimerType_Default: {
            if (Pref_DidChange(Pref_TimerType) || s_toggled_freecam) {
                // time over at 0 frames
                Patch_WriteWord(Rel_RelocateAddr(0x80297548), s_timeover_condition);
                // add -1 to timer each frame
                Patch_WriteWord(Rel_RelocateAddr(0x80297534), s_timer_increment);
                s_toggled_freecam = false;
            }
            break;
        }
        case TimerType_FreezeInstantly: {
            // time over at -60 frames (for leniency when switching modes)
            Patch_WriteWord(Rel_RelocateAddr(0x80297548), 0x2c00ffa0);
            // add 0 to timer each frame (timer doesnt move)
            Patch_WriteWord(Rel_RelocateAddr(0x80297534), 0x38030000);
            break;
        }
        case TimerType_FreezeAtZero: {
            // time over at -60 frames (so timer is able to stop at 0.00)
            Patch_WriteWord(Rel_RelocateAddr(0x80297548), 0x2c00ffa0);

            if (mkb_mode_info.stage_time_frames_remaining <= 0) {
                // when timer hits 0, add 0 to timer each frame
                Patch_WriteWord(Rel_RelocateAddr(0x80297534), 0x38030000);
            } else {
                // timer is ticking normally, add -1 to timer each frame
                Patch_WriteWord(Rel_RelocateAddr(0x80297534), 0x3803ffff);
            }
            break;
        }
        case TimerType_CountUpwards: {
            if (mkb_sub_mode == mkb_SMD_GAME_READY_INIT) {
                mkb_mode_info.stage_time_frames_remaining = 0;
            }
            // time over at -60 frames (so timer is able to stop at 0.00)
            Patch_WriteWord(Rel_RelocateAddr(0x80297548), 0x2c00ffa0);

            // getting close to signed integer overflow, freeze timer to prevent time-over
            if (mkb_mode_info.stage_time_frames_remaining >= 32400) {
                // add 0 to timer each frame
                Patch_WriteWord(Rel_RelocateAddr(0x80297534), 0x38030000);
            } else {
                // timer is ticking normally, add +1 to timer each frame
                Patch_WriteWord(Rel_RelocateAddr(0x80297534), 0x38030001);
            }
            break;
        }
    }
}

void Fallout_Tick() {
    freeze_timer();
}
void Fallout_Disp() {
}
