#include "mods/deathcounter.h"

#include "mkb/mkb.h"

#include "mods/freecam.h"
#include "mods/storytimer.h"
#include "mods/validate.h"
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
    // set the death count to 0 on the file select screen
    if (mkb::scen_info.mode == 5) {
        s_death_count = 0;
        s_can_die = false;
    }

    // Don't increment the death counter on stage 1 if the setting is ticked
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN && !validate::has_entered_goal()) {
        if (pref::get(pref::BoolPref::CountFirstStageDeaths)) {
            s_can_die = true;
        } else if (!pref::get(pref::BoolPref::CountFirstStageDeaths) &&
                   storytimer::get_completed_stagecount() != 0) {
            s_can_die = true;
        }
    } else if (validate::has_entered_goal()) {
        s_can_die = false;
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
        s_can_die = false;  // once the death counter is incremented, set this to false so we only
                            // increment it by 1
    }
}

void disp() {
    if ((mkb::main_game_mode != mkb::STORY_MODE && mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_INIT &&
         mkb::sub_mode != mkb::SMD_AUTHOR_PLAY_MAIN) ||
        freecam::should_hide_hud() || !pref::get(pref::BoolPref::ShowDeathCounter)) {
        return;
    }
    draw::debug_text(18, 56, draw::WHITE, "Deaths: ");
    draw::debug_text(98, 56, draw::WHITE, "%d", s_death_count);
}

}  // namespace deathcounter
