#include "variant_text.h"
#include "mods/physics.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

namespace variant_text {

void init() {}
void tick() {}
void disp() {
    if (mkb::sub_mode != mkb::SMD_GAME_READY_INIT && mkb::sub_mode != mkb::SMD_GAME_READY_MAIN &&
        mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT && mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN)
        return;

    if (physics::using_custom_physics()) {
        mkb::textdraw_reset();
        mkb::textdraw_set_font(mkb::FONT32_ASC_8x16);
        u32 x = 634;
        u32 y = 474;
        mkb::textdraw_set_pos(x, y);
        mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT);
        mkb::textdraw_set_scale(1.2, 0.8);
        mkb::GXColor color = draw::WHITE;
        mkb::textdraw_set_mul_color(RGBA(color.r, color.g, color.b, color.a));
        mkb::textdraw_print("Custom Physics");
        return;
    }

    bool variants_enabled =
        pref::get(pref::BoolPref::JumpMod) || pref::get(pref::BoolPref::DisableFalloutVolumes) ||
        pref::get(pref::BoolPref::Marathon) || pref::get(pref::U8Pref::FalloutPlaneType) != 0 ||
        pref::get(pref::U8Pref::StageEditVariant) != 0;

    if (variants_enabled) {
        mkb::textdraw_reset();
        mkb::textdraw_set_font(mkb::FONT32_ASC_8x16);
        u32 x = 634;
        u32 y = 474;
        mkb::textdraw_set_pos(x, y);
        mkb::textdraw_set_alignment(mkb::ALIGN_UPPER_LEFT);
        mkb::textdraw_set_scale(1.2, 0.8);
        mkb::GXColor color = draw::WHITE;
        mkb::textdraw_set_mul_color(RGBA(color.r, color.g, color.b, color.a));
        mkb::textdraw_print("Custom Variants");
        return;
    }
}

}  // namespace variant_text