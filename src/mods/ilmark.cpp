#include "ilmark.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "mods/physics.h"
#include "mods/validate.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/libsavest.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

namespace ilmark {

static bool s_valid_run = false;
static bool s_is_romhack = false;

void init() {
    char gamecode[7] = {};
    mkb::memcpy(gamecode, mkb::DVD_GAME_NAME, 6);
    s_is_romhack = mkb::strcmp(gamecode, "GM2E8P") != 0;
}

void validate_attempt() {
    if (!validate::was_run_valid(false)) return;
    s_valid_run = true;
}

void tick() {
    if (mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT &&
        mkb::sub_mode != mkb::SMD_GAME_GOAL_MAIN &&
        mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_INIT &&
        mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_MAIN &&
        mkb::sub_mode != mkb::SMD_GAME_BONUS_CLEAR_INIT &&
        mkb::sub_mode != mkb::SMD_GAME_BONUS_CLEAR_MAIN) {
        s_valid_run = false;
    }

    if (libsavest::state_loaded_this_frame()) {
        s_valid_run = false;
    }
}

bool is_ilmark_enabled() {
    if (mkb::main_mode != mkb::MD_GAME) return false;

    if (mkb::main_game_mode == mkb::PRACTICE_MODE) {
        if (!pref::get(pref::BoolPref::IlMarkPractice)) return false;
    } else if (mkb::main_game_mode == mkb::STORY_MODE) {
        if (!pref::get(pref::BoolPref::IlMarkStory)) return false;
    } else if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
        if (!pref::get(pref::BoolPref::IlMarkChallenge)) return false;
    } else {
        return false;
    }

    if (s_is_romhack && !pref::get(pref::BoolPref::IlMarkRomhacks)) {
        return false;
    }

    return true;
}

void disp() {
    if (!is_ilmark_enabled() || freecam::should_hide_hud()) return;

    bool in_show_submode = mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT ||
                           mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN ||
                           mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT ||
                           mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN ||
                           mkb::sub_mode == mkb::SMD_GAME_BONUS_CLEAR_INIT ||
                           mkb::sub_mode == mkb::SMD_GAME_BONUS_CLEAR_MAIN;
    if (!in_show_submode) return;

    mkb::textdraw_reset();
    // Some good fonts that seem to be always loaded:
    // FONT32_ASC_8x16,
    // FONT32_ASC_12x12,
    // FONT32_ASC_24x24,  // Fairly big
    // FONT32_ASC_16x16P, // Doesn't support lowercase letters? P means proportional
    // FONT32_ASC_16x16,  // Doesn't support lowercase letters? Monospace
    mkb::textdraw_set_font(mkb::FONT32_ASC_8x16);
    // mkb::textdraw_set_flags(mkb::TEXTDRAW_FLAG_BORDER | mkb::TEXTDRAW_FLAG_PROPORTIONAL);

    u32 x = 634;
    u32 y = 474;
    if (!s_valid_run) {
        x -= 4;
        y -= 4;
    }

    mkb::textdraw_set_pos(x, y);
    mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT);
    mkb::textdraw_set_scale(0.8, 0.8);
    mkb::GXColor color = s_valid_run ? draw::LIGHT_GREEN : draw::LIGHT_RED;
    mkb::textdraw_set_mul_color(RGBA(color.r, color.g, color.b, color.a));
    // mkb::textdraw_set_font_style(mkb::STYLE_BOLD);

    mkb::textdraw_print(const_cast<char*>(version::get_version_str()));
}

}  // namespace ilmark
