#include "mods/deathcounter.h"

#include "mkb/mkb.h"

#include "mods/freecam.h"
#include "systems/assembly.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

namespace deathcounter {

static bool s_can_die;
static u32 s_death_count;

void tick() {
    // bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    // set the death count to 0 on the file select screen
    if (mkb::g_storymode_mode == 5) {
        s_death_count = 0;
    }

    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
        s_can_die = true;
    }

    if (s_can_die &&
        (mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
         mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
         mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN ||
         mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT)) {
        // you can die either by retrying after dropping in, falling out, timing over, stage
        // selecting after dropping in (but before breaking the tape), or exiting game after
        // dropping in (but before breaking the tape)
        s_death_count += 1;
        s_can_die = false;
    }

    // first framing should not increase the death counter, and retrying after breaking the tape
    // should not increase it either to do: however, if you retry after breaking the tape on the
    // very first frame (so the frame before goal init), it does count as a death when it should not
    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT ||
        mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) {
        s_can_die = false;
    }

    
}

void disp() {
    if ((mkb::main_game_mode != mkb::STORY_MODE && mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_INIT &&
         mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_MAIN) ||
        freecam::should_hide_hud() || pref::get(pref::BoolPref::ShowDeathCounter) == false) {
        return;
    }
    draw::debug_text(18, 56, draw::WHITE, "Deaths: ");
    draw::debug_text(98, 56, draw::WHITE, "%d", s_death_count);
}

}  // namespace deathcounter
