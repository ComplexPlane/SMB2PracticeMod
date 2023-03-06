#include "freeze.h"

#include "mkb/mkb.h"
#include "systems/pref.h"
#include "utils/patch.h"
#include "mods/freecam.h"

namespace freeze {

static patch::Tramp<decltype(&mkb::event_info_tick)> s_event_info_tick_tramp;

void init() {
    // TODO use a generic pre-draw hook instead of hooking this important function?
    patch::hook_function(s_event_info_tick_tramp, mkb::event_info_tick, []() {
        s_event_info_tick_tramp.dest();
        if (pref::get(pref::BoolPref::FreezeTimer) || freecam::should_freeze_timer()) {
            mkb::mode_info.stage_time_frames_remaining = mkb::mode_info.stage_time_limit;
        }
    });
}

}  // namespace freeze
