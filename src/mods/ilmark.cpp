#include "ilmark.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "mods/physics.h"
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
static s16 s_paused_frame = 0;
static bool s_is_romhack = false;

static constexpr pref::BoolPref INVALID_BOOL_PREFS[] = {
    pref::BoolPref::DisableFalloutVolumes,
    pref::BoolPref::JumpMod,
    pref::BoolPref::Marathon,
    pref::BoolPref::DebugMode,
};

static constexpr pref::U8Pref INVALID_U8_PREFS[] = {
    pref::U8Pref::TimerType,        pref::U8Pref::Friction,         pref::U8Pref::Restitution,
    pref::U8Pref::FalloutPlaneType, pref::U8Pref::StageEditVariant,
};

void disable_invalidating_settings() {
    // set all bool prefs to default
    for (u8 i = 0; i < LEN(INVALID_BOOL_PREFS); i++) {
        pref::set(INVALID_BOOL_PREFS[i], pref::get_default(INVALID_BOOL_PREFS[i]));
    }
    // set all u8 prefs to default
    for (u8 i = 0; i < LEN(INVALID_U8_PREFS); i++) {
        pref::set(INVALID_U8_PREFS[i], pref::get_default(INVALID_U8_PREFS[i]));
    }

    pref::save();
}

void init() {
    char gamecode[7] = {};
    mkb::memcpy(gamecode, mkb::DVD_GAME_NAME, 6);
    s_is_romhack = mkb::strcmp(gamecode, "GM2E8P") != 0;
}

void tick() {
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT) {
        s_valid_run = true;
        s_paused_frame = 0;

    } else if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
        bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
        if (paused_now) {
            if (s_paused_frame == 0) {
                s_paused_frame = mkb::mode_info.stage_time_frames_remaining;
            } else {
                s_valid_run = false;
            }
        }
        // Loading savestates is disallowed
        if (libsavest::state_loaded_this_frame()) s_valid_run = false;

        // Using dpad controls is disallowed
        bool dpad_down =
            pad::button_down(mkb::PAD_BUTTON_DOWN) || pad::button_down(mkb::PAD_BUTTON_LEFT) ||
            pad::button_down(mkb::PAD_BUTTON_RIGHT) || pad::button_down(mkb::PAD_BUTTON_UP);
        if (pref::get(pref::BoolPref::DpadControls) && dpad_down) s_valid_run = false;

        // Opening the mod menu is disallowed
        if (menu_impl::is_visible()) s_valid_run = false;

        // Physics must be default or custom (with all default values)
        if (physics::using_custom_physics()) s_valid_run = false;

        // Invalid bool prefs are enabled
        for (u8 i = 0; i < LEN(INVALID_BOOL_PREFS); i++) {
            if (pref::get(INVALID_BOOL_PREFS[i]) != pref::get_default(INVALID_BOOL_PREFS[i])) {
                s_valid_run = false;
            }
        }
        // Invalid u8 prefs are enabled
        for (u8 i = 0; i < LEN(INVALID_U8_PREFS); i++) {
            if (pref::get(INVALID_U8_PREFS[i]) != pref::get_default(INVALID_U8_PREFS[i])) {
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
