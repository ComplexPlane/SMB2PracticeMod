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
static bool s_in_story;

void tick() {

    if ( (mkb::main_game_mode == mkb::STORY_MODE)  || mkb::sub_mode == mkb::SMD_AUTHOR_PLAY_INIT || mkb::sub_mode == mkb::SMD_AUTHOR_PLAY_MAIN) {
     s_in_story = true;
    } else{
        s_in_story = false;
    }

    // set the death count to 0 on the file select screen
    if (mkb::g_storymode_mode == 5) {
        s_death_count = 0;
    }

    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
        s_can_die = true;
    }

    if ( (mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT) && s_can_die == true){
        s_death_count += 1; 
        s_can_die = false;
    }

    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
        s_can_die = false;
    }

}

void disp() {

    if (s_in_story == false || freecam::should_hide_hud() || pref::get(pref::BoolPref::ShowDeathCounter) == false ){
            return;
        }
   draw::debug_text(18, 56, draw::WHITE, "Deaths: "); 
   draw::debug_text(100, 56, draw::WHITE, "%d", s_death_count);

} 

} // namespace deathcounter
