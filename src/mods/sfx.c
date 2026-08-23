#include "mods/sfx.h"

#include "systems/pref.h"
#include "utils/base.h"
#include "utils/patch.h"

static s32 soft_stream_start_hook(u32 g_looping_state, mkb_BgmTrack g_bgm_id, u32 param_3);

TRAMP(s_SoftStreamStart_tramp, mkb_SoftStreamStart, soft_stream_start_hook);

static s32 soft_stream_start_hook(u32 g_looping_state, mkb_BgmTrack g_bgm_id, u32 param_3) {
    (void)g_looping_state;
    (void)g_bgm_id;
    (void)param_3;
    return 0;
}

static void call_sound_req_hook(u32 g_sfx_idx);

TRAMP(s_call_SoundReqID_arg_0_tramp, mkb_call_SoundReqID_arg_0, call_sound_req_hook);

static void call_sound_req_hook(u32 g_sfx_idx) {
    if (!(Pref_Get(Pref_MuteTimerDing) && g_sfx_idx == 0x0003d806)) {
        s_call_SoundReqID_arg_0_tramp.chain(g_sfx_idx);
    }
}

void sfx_init() {
    // Only hook if the preference is initially set, so we don't affect background music until game
    // is rebooted
    if (Pref_Get(Pref_MuteBgm)) {
        HOOK_TRAMP(s_SoftStreamStart_tramp);
    }

    HOOK_TRAMP(s_call_SoundReqID_arg_0_tramp);
}
