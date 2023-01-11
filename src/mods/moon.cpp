#include "moon.h"
#include "mkb/mkb.h"
#include "systems/pref.h"

namespace moon {

static void moon_gravity() {
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;  // Makes paused_now work
    if (mkb::mode_info.stage_time_frames_remaining < mkb::mode_info.stage_time_limit &&
        !paused_now) {
        mkb::balls[mkb::curr_player_idx].vel.y += .005;
    }
}

void tick() {
    if (pref::get_moon()) {
        moon_gravity();
    }
}

}  // Namespace moon