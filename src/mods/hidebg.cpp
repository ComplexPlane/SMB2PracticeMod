#include "hidebg.h"

#include "mkb/mkb.h"
#include "systems/pref.h"
#include "utils/patch.h"

namespace hidebg {

static patch::Tramp<decltype(&mkb::g_draw_bg)> s_draw_bg_tramp;
static patch::Tramp<decltype(&mkb::g_set_clear_color)> s_clear_tramp;
static patch::Tramp<decltype(&mkb::GXSetCopyClear_cached)> s_gxclear_tramp;

static bool should_hide_bg() { return pref::get_hide_bg() && mkb::main_mode != mkb::MD_ADV; }

static void avdisp_set_fog_color_hook(u8 r, u8 g, u8 b) {
    if (should_hide_bg()) {
        mkb::avdisp_set_fog_color(0, 0, 0);
    } else {
        mkb::avdisp_set_fog_color(r, g, b);
    }
}

static void nl2ngc_set_fog_color_hook(u8 r, u8 g, u8 b) {
    if (should_hide_bg()) {
        mkb::nl2ngc_set_fog_color(0, 0, 0);
    } else {
        mkb::nl2ngc_set_fog_color(r, g, b);
    }
}

static void GXSetCopyClear_hook(mkb::GXColor color, u32 clear_z) {
    mkb::OSReport("GXSetCopyClear({%d, %d, %d, %d}, %d)\n", color.r, color.g, color.b, color.a, clear_z);
    s_gxclear_tramp.dest(color, clear_z);
}

void init() {
    patch::hook_function(s_draw_bg_tramp, mkb::g_draw_bg, []() {
        if (!should_hide_bg()) {
            s_draw_bg_tramp.dest();
        }
    });

    patch::hook_function(s_clear_tramp, mkb::g_set_clear_color, []() {
        if (should_hide_bg()) {
            mkb::GXColor backup_color = mkb::g_some_theme_color;
            u8 backup_override_r = mkb::g_override_clear_r;
            u8 backup_override_g = mkb::g_override_clear_g;
            u8 backup_override_b = mkb::g_override_clear_b;

            mkb::g_some_theme_color = mkb::GXColor{0, 0, 0, 0xff};
            mkb::g_override_clear_r = 0;
            mkb::g_override_clear_g = 0;
            mkb::g_override_clear_b = 0;

            s_clear_tramp.dest();

            mkb::g_some_theme_color = backup_color;
            mkb::g_override_clear_r = backup_override_r;
            mkb::g_override_clear_g = backup_override_g;
            mkb::g_override_clear_b = backup_override_b;
        } else {
            s_clear_tramp.dest();
        }
    });

    // Black fog
    patch::write_branch_bl(reinterpret_cast<void*>(0x80352e58),
                           reinterpret_cast<void*>(avdisp_set_fog_color_hook));
    patch::write_branch_bl(reinterpret_cast<void*>(0x80352eac),
                           reinterpret_cast<void*>(nl2ngc_set_fog_color_hook));

    // Test
    // patch::hook_function(s_gxclear_tramp, mkb::GXSetCopyClear_cached, GXSetCopyClear_hook);
}

}  // namespace hidebg
