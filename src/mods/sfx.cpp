#include "sfx.h"

#include "mkb/mkb.h"
#include "systems/pref.h"
#include "utils/patch.h"

namespace sfx {

TRAMP(s_SoftStreamStart_tramp, mkb::SoftStreamStart,
      [](u32 g_looping_state, mkb::BgmTrack g_bgm_id, u32 param_3) -> s32 { return 0; });

TRAMP(s_call_SoundReqID_arg_0_tramp, mkb::call_SoundReqID_arg_0, [](u32 g_sfx_idx) {
    if (!(pref::get(pref::BoolPref::MuteTimerDing) && g_sfx_idx == 0x0003d806)) {
        s_call_SoundReqID_arg_0_tramp.chain(g_sfx_idx);
    }
});

void init() {
    // Only hook if the preference is initially set, so we don't affect background music until game
    // is rebooted
    if (pref::get(pref::BoolPref::MuteBgm)) {
        HOOK_TRAMP(s_SoftStreamStart_tramp);
    }

    HOOK_TRAMP(s_call_SoundReqID_arg_0_tramp);
}

}  // namespace sfx
