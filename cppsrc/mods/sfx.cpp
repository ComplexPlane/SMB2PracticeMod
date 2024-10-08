#include "sfx.h"

#include "mkb/mkb.h"
#include "systems/pref.h"
#include "utils/patch.h"

namespace sfx {

static patch::Tramp<decltype(&mkb::SoftStreamStart)> s_SoftStreamStart_tramp;
static patch::Tramp<decltype(&mkb::call_SoundReqID_arg_0)> s_call_SoundReqID_arg_0_tramp;

void init() {
    // Only hook if the preference is initially set, so we don't affect background music until game
    // is rebooted
    if (pref::get(pref::BoolPref::MuteBgm)) {
        patch::hook_function(
            s_SoftStreamStart_tramp, mkb::SoftStreamStart,
            [](u32 g_looping_state, mkb::BgmTrack g_bgm_id, u32 param_3) -> s32 { return 0; });
    }

    patch::hook_function(
        s_call_SoundReqID_arg_0_tramp, mkb::call_SoundReqID_arg_0, [](u32 g_sfx_idx) {
            if (!(pref::get(pref::BoolPref::MuteTimerDing) && g_sfx_idx == 0x0003d806)) {
                s_call_SoundReqID_arg_0_tramp.dest(g_sfx_idx);
            }
        });
}

}  // namespace sfx
