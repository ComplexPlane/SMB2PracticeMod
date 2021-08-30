#include "freeze.h"

#include <mkb.h>
#include "patch.h"
#include "pref.h"

namespace freeze {

static void (*s_event_info_tick_tramp)();

void init() {
    // TODO use a generic pre-draw hook instead of hooking this important function?
    s_event_info_tick_tramp = patch::hook_function(mkb::event_info_tick, []() {
        s_event_info_tick_tramp();
        if (pref::get_freeze_timer()) {
            mkb::mode_info.stage_time_frames_remaining = mkb::mode_info.stage_time_limit;
        }
    });
}

}  // namespace freeze