#include "hidebg.h"

#include "mkb/mkb2_ghidra.h"
#include "utils/patch.h"
#include "systems/pref.h"

namespace hidebg {

static patch::Tramp<decltype(&mkb::g_draw_bg)> s_draw_bg_tramp;
static patch::Tramp<decltype(&mkb::g_set_clear_color)> s_clear_tramp;

void init() {
    patch::hook_function(s_draw_bg_tramp, mkb::g_draw_bg, []() {
        if (!pref::get_hide_bg()) {
            s_draw_bg_tramp.dest();
        }
    });

    patch::hook_function(s_draw_bg_tramp, mkb::g_set_clear_color, []() {
        if (pref::get_hide_bg()) {
            mkb::GXSetCopyClear_cached({0, 0, 0, 0}, 0xFFFFFF); // Black
        } else {
            s_clear_tramp.dest();
        }
    });
}

}  // namespace hidebg
