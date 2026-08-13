#include "ilmark.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/gamecode.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"
#include "utils/relutil.h"
#include "utils/savest.h"

namespace ilmark {

static bool s_valid_run = false;
static s16 s_paused_frame = 0;

static constexpr pref::Pref INVALID_PREFS[] = {
    pref::Pref::DisableFalloutVolumes,
    pref::Pref::UseCustomPhysics,
    pref::Pref::JumpMod,
    pref::Pref::Moon,
    pref::Pref::Marathon,
    pref::Pref::DebugMode,
    pref::Pref::TimerType,
    pref::Pref::Friction,
    pref::Pref::Restitution,
    pref::Pref::Camera,
    pref::Pref::FalloutPlaneType,
};

void disable_invalidating_settings() {
    for (u8 i = 0; i < LEN(INVALID_PREFS); i++) {
        pref::set(INVALID_PREFS[i], pref::get_default(INVALID_PREFS[i]));
    }

    pref::save();
}

void tick() {
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT) {
        s_valid_run = true;
        s_paused_frame = 0;

    } else if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
        bool paused_now = *reinterpret_cast<u32 *>(relutil::relocate_addr(0x805BC474)) & 8;
        if (paused_now) {
            if (s_paused_frame == 0) {
                s_paused_frame = mkb::mode_info.stage_time_frames_remaining;
            } else {
                s_valid_run = false;
            }
        }
        // Loading savestates is disallowed
        if (savest::get_history().curr_frame_action == savest::Action::Load) s_valid_run = false;

        // Using dpad controls is disallowed
        bool dpad_down =
            pad::button_down(mkb::PAD_BUTTON_DOWN) || pad::button_down(mkb::PAD_BUTTON_LEFT) ||
            pad::button_down(mkb::PAD_BUTTON_RIGHT) || pad::button_down(mkb::PAD_BUTTON_UP);
        if (pref::get(pref::Pref::DpadControls) && dpad_down) s_valid_run = false;

        // Opening the mod menu is disallowed
        if (menu_impl::is_visible()) s_valid_run = false;

        // Invalid prefs are enabled
        for (u8 i = 0; i < LEN(INVALID_PREFS); i++) {
            if (pref::get(INVALID_PREFS[i]) != pref::get_default(INVALID_PREFS[i])) {
                s_valid_run = false;
            }
        }
    } else if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
        s_valid_run = s_valid_run && s_paused_frame <= mkb::mode_info.stage_time_frames_remaining;
    }
}

bool is_ilmark_enabled() {
    if (mkb::main_mode != mkb::MD_GAME) return false;

    if (mkb::main_game_mode == mkb::PRACTICE_MODE) {
        if (!pref::get(pref::Pref::IlMarkPractice)) return false;
    } else if (mkb::main_game_mode == mkb::STORY_MODE) {
        if (!pref::get(pref::Pref::IlMarkStory)) return false;
    } else if (mkb::main_game_mode == mkb::CHALLENGE_MODE) {
        if (!pref::get(pref::Pref::IlMarkChallenge)) return false;
    } else {
        return false;
    }

    if (!gamecode::is_vanilla() && !pref::get(pref::Pref::IlMarkRomhacks)) {
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
    GXColor color = s_valid_run ? draw::LIGHT_GREEN : draw::LIGHT_RED;
    mkb::textdraw_set_mul_color(RGBA(color.r, color.g, color.b, color.a));
    // mkb::textdraw_set_font_style(mkb::STYLE_BOLD);

    mkb::textdraw_print(const_cast<char *>(version::get_version_str()));
}

}  // namespace ilmark
