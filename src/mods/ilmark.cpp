#include "ilmark.h"

#include "mkb/mkb.h"
#include "systems/pad.h"
#include "systems/pref.h"
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

        // Loading savestate disallowed
        if (libsavest::state_loaded_this_frame()) s_valid_run = false;

        // Using dpad controls disallowed
        bool dpad_down =
            pad::button_down(mkb::PAD_BUTTON_DOWN) || pad::button_down(mkb::PAD_BUTTON_LEFT) ||
            pad::button_down(mkb::PAD_BUTTON_RIGHT) || pad::button_down(mkb::PAD_BUTTON_UP);
        if (pref::get_dpad_controls() && dpad_down) s_valid_run = false;

        // Using these tools/mods at all is disallowed
        bool using_disallowed_mod = pref::get_freeze_timer() || pref::get_freecam() ||
                                    pref::get_debug_mode() || pref::get_jump_mod() ||
                                    pref::get_moon() || pref::get_marathon();
        if (using_disallowed_mod) s_valid_run = false;
    }
}

void disp() {
    mkb::textdraw_reset();
    // Some good fonts that seem to be always loaded:
    // FONT32_ASC_8x16,
    // FONT32_ASC_12x12,
    // FONT32_ASC_24x24,  // Fairly big
    // FONT32_ASC_16x16P, // Doesn't support lowercase letters? P means proportional
    // FONT32_ASC_16x16,  // Doesn't support lowercase letters? Monospace
    mkb::textdraw_set_font(mkb::FONT32_ASC_8x16);
    // mkb::textdraw_set_flags(mkb::TEXTDRAW_FLAG_BORDER | mkb::TEXTDRAW_FLAG_PROPORTIONAL);
    mkb::textdraw_set_pos(450, 300);
    mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_RIGHT);
    mkb::textdraw_set_scale(1, 1);
    mkb::textdraw_set_mul_color(RGBA(0xFF, 0x60, 0xFF, 0xFF));
    // mkb::textdraw_set_font_style(mkb::STYLE_BOLD);
    mkb::textdraw_printf("Hello monkey. %d:%d\n", 30, 25);
}

}  // namespace ilmark
