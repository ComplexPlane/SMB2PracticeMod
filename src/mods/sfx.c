#include "mods/sfx.h"

#include "systems/pref.h"
#include "utils/base.h"
#include "utils/patch.h"

static s32 soft_stream_start(u32 looping_state, mkb_BgmTrack bgm_id, u32 param_3);
static void sound_req(u32 sfx_idx);

TRAMP(s_soft_stream_start_tramp, mkb_SoftStreamStart, soft_stream_start);
TRAMP(s_sound_req_tramp, mkb_call_SoundReqID_arg_0, sound_req);

static s32 soft_stream_start(u32 looping_state, mkb_BgmTrack bgm_id, u32 param_3) {
    return 0;
}

static void sound_req(u32 sfx_idx) {
    if (!(Pref_Get(Pref_MuteTimerDing) && sfx_idx == 0x0003d806)) {
        s_sound_req_tramp.chain(sfx_idx);
    }
}

void sfx_init() {
    if (Pref_Get(Pref_MuteBgm)) HOOK_TRAMP(s_soft_stream_start_tramp);
    HOOK_TRAMP(s_sound_req_tramp);
}

void sfx_reboot() {
}
