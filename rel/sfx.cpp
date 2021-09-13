#include "sfx.h"

#include <mkb.h>
#include "patch.h"
#include "pref.h"

namespace sfx {

static void (*s_call_SoundReqID_arg_0_tramp)(u32 g_sfx_id);

void init() {
    // Only hook if the preference is initially set, so we don't affect background music until game
    // is rebooted
    if (pref::get_mute_bgm()) {
        patch::hook_function(
            mkb::SoftStreamStart,
            [](u32 g_looping_state, mkb::BgmTrack g_bgm_id, u32 param_3) -> s32 { return 0; });
    }

    s_call_SoundReqID_arg_0_tramp =
        patch::hook_function(mkb::call_SoundReqID_arg_0, [](u32 g_sfx_idx) {
            if (!(pref::get_mute_timer_ding() && g_sfx_idx == 0x0003d806)) {
                s_call_SoundReqID_arg_0_tramp(g_sfx_idx);
            }
        });
}

}  // namespace sfx
