#include "mods/ilmark.h"

#include "mods/freecam.h"
#include "mods/validate.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/savest.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

static bool s_valid_run = false;
static bool s_is_romhack = false;

void ILMark_Init() {
    char gamecode[7] = {};
    mkb_memcpy(gamecode, mkb_DVD_GAME_NAME, 6);
    s_is_romhack = mkb_strcmp(gamecode, "GM2E8P") != 0;
}

void ILMark_ValidateAttempt() {
    if (!Validate_WasRunValid(false)) return;
    s_valid_run = true;
}

void ILMark_Tick() {
    if (mkb_sub_mode != mkb_SMD_GAME_PLAY_MAIN && mkb_sub_mode != mkb_SMD_GAME_GOAL_INIT &&
        mkb_sub_mode != mkb_SMD_GAME_GOAL_MAIN &&
        mkb_sub_mode != mkb_SMD_GAME_GOAL_REPLAY_INIT &&
        mkb_sub_mode != mkb_SMD_GAME_GOAL_REPLAY_MAIN &&
        mkb_sub_mode != mkb_SMD_GAME_BONUS_CLEAR_INIT &&
        mkb_sub_mode != mkb_SMD_GAME_BONUS_CLEAR_MAIN) {
        s_valid_run = false;
    }

    if (SS_GetLastAction() == SS_Action_Load) {
        s_valid_run = false;
    }
}

bool ILMark_IsEnabled() {
    if (mkb_main_mode != mkb_MD_GAME) return false;

    if (mkb_main_game_mode == mkb_PRACTICE_MODE) {
        if (!Pref_Get(Pref_IlMarkPractice)) return false;
    } else if (mkb_main_game_mode == mkb_STORY_MODE) {
        if (!Pref_Get(Pref_IlMarkStory)) return false;
    } else if (mkb_main_game_mode == mkb_CHALLENGE_MODE) {
        if (!Pref_Get(Pref_IlMarkChallenge)) return false;
    } else {
        return false;
    }

    if (s_is_romhack && !Pref_Get(Pref_IlMarkRomhacks)) {
        return false;
    }

    return true;
}

void ILMark_Disp() {
    if (!ILMark_IsEnabled() || Freecam_ShouldHideHud()) return;

    bool in_show_submode = mkb_sub_mode == mkb_SMD_GAME_GOAL_INIT ||
                           mkb_sub_mode == mkb_SMD_GAME_GOAL_MAIN ||
                           mkb_sub_mode == mkb_SMD_GAME_GOAL_REPLAY_INIT ||
                           mkb_sub_mode == mkb_SMD_GAME_GOAL_REPLAY_MAIN ||
                           mkb_sub_mode == mkb_SMD_GAME_BONUS_CLEAR_INIT ||
                           mkb_sub_mode == mkb_SMD_GAME_BONUS_CLEAR_MAIN;
    if (!in_show_submode) return;

    mkb_textdraw_reset();
    // Some good fonts that seem to be always loaded:
    // FONT32_ASC_8x16,
    // FONT32_ASC_12x12,
    // FONT32_ASC_24x24,  // Fairly big
    // FONT32_ASC_16x16P, // Doesn't support lowercase letters? P means proportional
    // FONT32_ASC_16x16,  // Doesn't support lowercase letters? Monospace
    mkb_textdraw_set_font(mkb_FONT32_ASC_8x16);
    // mkb_textdraw_set_flags(mkb_TEXTDRAW_FLAG_BORDER | mkb_TEXTDRAW_FLAG_PROPORTIONAL);

    u32 x = 634;
    u32 y = 474;
    if (!s_valid_run) {
        x -= 4;
        y -= 4;
    }

    mkb_textdraw_set_pos(x, y);
    mkb_textdraw_set_alignment(mkb_ALIGN_UPPER_LEFT);
    mkb_textdraw_set_scale(0.8, 0.8);
    GXColor color = s_valid_run ? COLOR_LIGHT_GREEN : COLOR_LIGHT_RED;
    mkb_textdraw_set_mul_color(RGBA(color.r, color.g, color.b, color.a));
    // mkb_textdraw_set_font_style(mkb_STYLE_BOLD);

    // Include the git commit hash after the version, e.g. "1.1.0 0123abcd-d"
    char mark[48];
    mkb_sprintf(mark, "%s %s", Version_GetStr(), GIT_HASH);
    mkb_textdraw_print(mark);
}
