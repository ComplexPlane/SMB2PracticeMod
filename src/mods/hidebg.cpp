#include "hidebg.h"

#include "utils/patch.h"

namespace hidebg {

static patch::Tramp<decltype(&mkb::g_draw_bg)> s_draw_bg_tramp;

void init() {
    patch::hook_function(s_draw_bg_tramp, mkb::g_draw_bg, []() {
        if (pref::get_hide_bg())
    });
}

}  // namespace hidebg
