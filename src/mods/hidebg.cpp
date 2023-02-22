#include "hidebg.h"

#include "mkb/mkb.h"
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

    patch::hook_function(s_clear_tramp, mkb::g_set_clear_color, []() {
        if (pref::get_hide_bg() && mkb::main_mode != mkb::MD_ADV) {
            // Trick function into setting black clear color
            u32 main_mode_backup = mkb::main_mode;
            u32 sub_mode_backup = mkb::sub_mode;
            mkb::main_mode = mkb::MD_OPTION;
            mkb::sub_mode = 0;
            s_clear_tramp.dest();
            mkb::main_mode = main_mode_backup;
            mkb::sub_mode = sub_mode_backup;
        } else {
            s_clear_tramp.dest();
        }
    });
}

}  // namespace hidebg
