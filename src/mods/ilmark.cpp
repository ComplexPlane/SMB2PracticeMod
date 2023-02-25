#include "ilmark.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/libsavest.h"
#include "utils/macro_utils.h"

namespace ilmark {

static bool s_valid_run = false;

void tick() {
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT) {
        s_valid_run = true;

    } else if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
        bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
        if (paused_now) s_valid_run = false;

        // Loading savestates is disallowed
        if (libsavest::state_loaded_this_frame()) s_valid_run = false;

        // Using dpad controls is disallowed
        bool dpad_down =
            pad::button_down(mkb::PAD_BUTTON_DOWN) || pad::button_down(mkb::PAD_BUTTON_LEFT) ||
            pad::button_down(mkb::PAD_BUTTON_RIGHT) || pad::button_down(mkb::PAD_BUTTON_UP);
        if (pref::get(pref::BoolPref::DpadControls) && dpad_down) s_valid_run = false;

        // Using these tools/mods at all is disallowed
        bool using_disallowed_mod =
            pref::get(pref::BoolPref::FreezeTimer) || freecam::enabled() ||
            pref::get(pref::BoolPref::DebugMode) || pref::get(pref::BoolPref::JumpMod) ||
            pref::get(pref::BoolPref::Moon) || pref::get(pref::BoolPref::Marathon);
        if (using_disallowed_mod) s_valid_run = false;

        // Opening the mod menu is disallowed
        if (menu_impl::is_visible()) s_valid_run = false;
    }
}

void disp() {
    if (mkb::main_mode != mkb::MD_GAME) return;

    if (mkb::main_game_mode == mkb::PRACTICE_MODE) {
        if (!pref::get(pref::BoolPref::IlMarkPractice)) return;
    } else if (mkb::main_game_mode == mkb::STORY_MODE) {
        if (!pref::get(pref::BoolPref::IlMarkStory)) return;
    } else if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
        if (!pref::get(pref::BoolPref::IlMarkChallenge)) return;
    } else {
        return;
    }

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
