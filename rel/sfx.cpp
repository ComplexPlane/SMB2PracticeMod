#include "sfx.h"

#include <mkb.h>
#include "patch.h"
#include "pref.h"

namespace sfx {

static s32 (*s_SoftStreamStart_tramp)(u32 g_looping_state, mkb::BgmTrack g_bgm_id, u32 param_3);

void init() {
    // Only hook if the preference is initially set, so we don't affect background music until game
    // is rebooted
    if (pref::get_mute_bgm()) {
        s_SoftStreamStart_tramp = patch::hook_function(
            mkb::SoftStreamStart,
            [](u32 g_looping_state, mkb::BgmTrack g_bgm_id, u32 param_3) -> s32 {
                if (g_looping_state == 1) return -1;
                return 0;
//                s32 ret = s_SoftStreamStart_tramp(g_looping_state, g_bgm_id, param_3);
//                mkb::OSReport("SoftStreamStart(%d, %d, %d) -> %d\n", g_looping_state, g_bgm_id,
//                              param_3, ret);
//                return ret;
            });
    }
}

void reboot() {}

}  // namespace sfx
