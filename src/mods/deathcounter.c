#include "mods/deathcounter.h"

#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/draw.h"

static bool s_can_die;
static u32 s_death_count;

void deathcounter_tick() {
    if (mkb_scen_info.mode == 5) s_death_count = 0;
    if (mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN) s_can_die = true;

    if (s_can_die &&
        (mkb_sub_mode == mkb_SMD_GAME_READY_INIT || mkb_sub_mode == mkb_SMD_GAME_RINGOUT_INIT ||
         mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_INIT ||
         mkb_sub_mode == mkb_SMD_GAME_SCENARIO_RETURN ||
         mkb_sub_mode == mkb_SMD_GAME_INTR_SEL_INIT)) {
        s_death_count++;
        s_can_die = false;
    }

    if (mkb_sub_mode == mkb_SMD_GAME_GOAL_INIT ||
        mkb_g_storymode_stageselect_state == mkb_STAGE_SELECT_INTRO_SEQUENCE) {
        s_can_die = false;
    }
}

void deathcounter_disp() {
    if ((mkb_main_game_mode != mkb_STORY_MODE && mkb_sub_mode != mkb_SMD_AUTHOR_PLAY_INIT &&
         mkb_sub_mode != mkb_SMD_AUTHOR_PLAY_MAIN) ||
        freecam_should_hide_hud() || !pref_get(Pref_ShowDeathCounter)) {
        return;
    }
    draw_debug_text(18, 56, COLOR_WHITE, "Deaths: ");
    draw_debug_text(98, 56, COLOR_WHITE, "%d", s_death_count);
}
