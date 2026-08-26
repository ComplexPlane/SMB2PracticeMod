#include "hide_sprites.h"

#include "utils/mode.h"

namespace hide_sprites {

static bool s_is_showing_banana_counter = false;
static bool s_is_showing_monkey_head = false;

void hide_banana_counter() {
    s_is_showing_banana_counter = false;
}

void hide_monkey_head() {
    s_is_showing_monkey_head = false;
}

void hide_right_side_sprites() {
    hide_banana_counter();
    hide_monkey_head();
}

bool right_side_sprites_normally_visible() {
    return mode::is_on_stage(mkb::sub_mode) && !mode::is_story_retry_screen(mkb::sub_mode);
}

static void set_banana_counter_visible(bool show) {
    if (mkb::main_mode != mkb::MD_GAME) return;

    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite &sprite = mkb::sprites[i];
        if (sprite.bmp == 0x502 || sprite.tick_func == mkb::sprite_banana_icon_tick ||
            sprite.tick_func == mkb::sprite_banana_icon_shadow_tick ||
            sprite.tick_func == mkb::sprite_banana_count_tick) {
            if ((show && sprite.depth < 0.f) || (!show && sprite.depth >= 0.f)) {
                sprite.depth = -sprite.depth;
            }
        }
    }
}

static void set_monkey_head_visible(bool show) {
    if (mkb::main_mode != mkb::MD_GAME) return;

    for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
        if (mkb::sprite_pool_info.status_list[i] == 0) continue;

        mkb::Sprite &sprite = mkb::sprites[i];
        if (sprite.bmp == 0x503 || sprite.tick_func == mkb::sprite_monkey_counter_tick ||
            sprite.disp_func == mkb::sprite_monkey_counter_icon_disp ||
            mkb::strcmp(sprite.text, ":") == 0 ||
            sprite.disp_func == mkb::sprite_hud_player_num_disp) {
            if ((show && sprite.depth < 0.f) || (!show && sprite.depth >= 0.f)) {
                sprite.depth = -sprite.depth;
            }
        }
    }
}

// Mods are only allowed to set the flags s_is_showing_banana_counter (and the corresponding monkey
// head one) to false, they are not allowed to directly use the static functions above
// The reason for doing this is that this means mods never have to worry about "undoing" a hiding
// action/setting these flags to true
// In other words, the relevant sprites are hidden when at least one mod decides that it should be
// hidden

void tick() {
    set_banana_counter_visible(s_is_showing_banana_counter);
    set_monkey_head_visible(s_is_showing_monkey_head);

    // Important that these get set to true after the above calls
    s_is_showing_banana_counter = true;
    s_is_showing_monkey_head = true;
}

}  // namespace hide_sprites