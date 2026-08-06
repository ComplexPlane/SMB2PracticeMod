#include "mods/ilmark.h"

#include "mods/freecam.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/base.h"
#include "utils/draw.h"
#include "utils/libsavest.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

static bool s_valid_run = false;
static s16 s_paused_frame = 0;
static bool s_is_romhack = false;

static const Pref INVALID_BOOL_PREFS[] = {
    Pref_DisableFalloutVolumes,
    Pref_UseCustomPhysics,
    Pref_JumpMod,
    Pref_Moon,
    Pref_Marathon,
    Pref_DebugMode,
};

static const Pref INVALID_U8_PREFS[] = {
    Pref_TimerType, Pref_Friction, Pref_Restitution, Pref_Camera, Pref_FalloutPlaneType,
};

void ilmark_disable_invalidating_settings() {
    // set all bool prefs to default
    for (u8 i = 0; i < LEN(INVALID_BOOL_PREFS); i++) {
        pref_set(INVALID_BOOL_PREFS[i], pref_get_default(INVALID_BOOL_PREFS[i]));
    }
    // set all u8 prefs to default
    for (u8 i = 0; i < LEN(INVALID_U8_PREFS); i++) {
        pref_set(INVALID_U8_PREFS[i], pref_get_default(INVALID_U8_PREFS[i]));
    }

    pref_save();
}

void ilmark_init() {
    char gamecode[7] = {};
    mkb_memcpy(gamecode, mkb_DVD_GAME_NAME, 6);
    s_is_romhack = mkb_strcmp(gamecode, "GM2E8P") != 0;
}

void ilmark_tick() {
    if (mkb_sub_mode == mkb_SMD_GAME_PLAY_INIT) {
        s_valid_run = true;
        s_paused_frame = 0;

    } else if (mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN) {
        bool paused_now = *(u32 *)(0x805BC474) & 8;
        if (paused_now) {
            if (s_paused_frame == 0) {
                s_paused_frame = mkb_mode_info.stage_time_frames_remaining;
            } else {
                s_valid_run = false;
            }
        }
        // Loading savestates is disallowed
        if (savest_was_state_loaded_this_frame()) s_valid_run = false;

        // Using dpad controls is disallowed
        bool dpad_down = pad_button_down(mkb_PAD_BUTTON_DOWN, false) ||
                         pad_button_down(mkb_PAD_BUTTON_LEFT, false) ||
                         pad_button_down(mkb_PAD_BUTTON_RIGHT, false) ||
                         pad_button_down(mkb_PAD_BUTTON_UP, false);
        if (pref_get(Pref_DpadControls) && dpad_down) s_valid_run = false;

        // Opening the mod menu is disallowed
        if (menu_impl_is_visible()) s_valid_run = false;

        // Invalid bool prefs are enabled
        for (u8 i = 0; i < LEN(INVALID_BOOL_PREFS); i++) {
            if (pref_get(INVALID_BOOL_PREFS[i]) != pref_get_default(INVALID_BOOL_PREFS[i])) {
                s_valid_run = false;
            }
        }
        // Invalid u8 prefs are enabled
        for (u8 i = 0; i < LEN(INVALID_U8_PREFS); i++) {
            if (pref_get(INVALID_U8_PREFS[i]) != pref_get_default(INVALID_U8_PREFS[i])) {
                s_valid_run = false;
            }
        }
    } else if (mkb_sub_mode == mkb_SMD_GAME_GOAL_INIT) {
        s_valid_run = s_valid_run && s_paused_frame <= mkb_mode_info.stage_time_frames_remaining;
    }
}

bool ilmark_is_ilmark_enabled() {
    if (mkb_main_mode != mkb_MD_GAME) return false;

    if (mkb_main_game_mode == mkb_PRACTICE_MODE) {
        if (!pref_get(Pref_IlMarkPractice)) return false;
    } else if (mkb_main_game_mode == mkb_STORY_MODE) {
        if (!pref_get(Pref_IlMarkStory)) return false;
    } else if (mkb_main_game_mode == mkb_CHALLENGE_MODE) {
        if (!pref_get(Pref_IlMarkChallenge)) return false;
    } else {
        return false;
    }

    if (s_is_romhack && !pref_get(Pref_IlMarkRomhacks)) {
        return false;
    }

    return true;
}

void ilmark_disp() {
    if (!ilmark_is_ilmark_enabled() || freecam_should_hide_hud()) return;

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

    mkb_textdraw_print((char *)(version_get_str()));
}
