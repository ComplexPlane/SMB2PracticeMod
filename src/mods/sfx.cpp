#include "sfx.h"

#include "mkb/mkb.h"
#include "systems/pref.h"
#include "utils/patch.h"

namespace sfx {

static patch::Tramp<mkb::SoftStreamStart> s_SoftStreamStart_tramp(
    [](u32 g_looping_state, mkb::BgmTrack g_bgm_id, u32 param_3) -> s32 { return 0; });

static patch::Tramp<mkb::call_SoundReqID_arg_0> s_call_SoundReqID_arg_0_tramp([](u32 g_sfx_idx) {
    if (!(pref::get(pref::Pref::MuteTimerDing) && g_sfx_idx == 0x0003d806)) {
        s_call_SoundReqID_arg_0_tramp.chain(g_sfx_idx);
    }
});

void init() {
    // Only hook if the preference is initially set, so we don't affect background music until game
    // is rebooted
    if (pref::get(pref::Pref::MuteBgm)) {
        s_SoftStreamStart_tramp.hook();
    }

    s_call_SoundReqID_arg_0_tramp.hook();
}

}  // namespace sfx
