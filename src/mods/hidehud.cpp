#include "hidehud.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/patch.h"

namespace hidehud {

static patch::Tramp<decltype(&mkb::draw_sprite)> s_draw_sprite_tramp;
static patch::Tramp<decltype(&mkb::g_draw_minimap)> s_draw_minimap_tramp;

void init() {
    patch::hook_function(s_draw_sprite_tramp, mkb::draw_sprite, [](mkb::Sprite* sprite) {
        // Hide every sprite except the pause menu
        bool hide_hud = pref::get(pref::BoolPref::HideHud);
        bool freecam_hide = freecam::should_hide_hud();
        bool is_pausemenu_sprite = sprite->disp_func == mkb::sprite_pausemenu_disp;
        if (!((hide_hud || freecam_hide) && !is_pausemenu_sprite)) {
            s_draw_sprite_tramp.dest(sprite);
        }
    });

    patch::hook_function(s_draw_minimap_tramp, mkb::g_draw_minimap, []() {
        bool hide_hud = pref::get(pref::BoolPref::HideHud);
        bool freecam_hide = freecam::should_hide_hud();
        if (!(hide_hud || freecam_hide)) {
            s_draw_minimap_tramp.dest();
        }
    });
}

}  // namespace hidehud
