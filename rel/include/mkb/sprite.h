#pragma once

#include "pool.h"

namespace mkb
{

constexpr s32 MAX_SPRITES = 80;

struct Sprite
{
    u8 visible; /* Whether it's visible or not? */
    u8 field_0x1; /* Seems to affect the font size/type on the pause menu? */
    u8 index;
    u8 field_0x3;
    Vec2f pos;
    u8 red;
    u8 green;
    u8 blue; /* Actually called "bule" in game.. yup */
    u8 field_0x0f;
    s16 field_0x10;
    s16 field_0x12;
    u8 field_0x14;
    u8 field_0x15;
    u8 field_0x16;
    u8 field_0x17;
    u8 field_0x18;
    u8 field_0x19;
    u8 field_0x1a;
    u8 field_0x1b;
    u8 field_0x1c;
    u8 field_0x1d;
    u8 field_0x1e;
    u8 field_0x1f;
    float field_0x20;
    u32 field_0x24;
    u8 field_0x28;
    u8 field_0x29;
    u8 field_0x2a;
    u8 field_0x2b;
    struct SpriteTex *tex;
    void (*dest_func)(Sprite *sprite);
    void (*tick_func)(u8 *status, Sprite *sprite);
    void (*disp_func)(Sprite *sprite);
    u16 g_texture_id;
    u8 field_0x3e;
    u8 field_0x3f;
    float width;
    float height;
    float depth;
    s32 some_frame_count;
    u32 field_0x50;
    u32 field_0x54;
    float lerp_value;
    float field_0x5c;
    u8 field_0x60;
    u8 field_0x61;
    u8 field_0x62;
    u8 field_0x63;
    struct Sprite *prev_sprite;
    struct Sprite *next_sprite;
    u32 field_0x6c;
    u32 field_0x70;
    u32 field_0x74;
    u32 field_0x78;
    u8 field_0x7c;
    u8 field_0x7d;
    u8 field_0x7e;
    u8 field_0x7f;
    float field_0x80;
    u8 field_0x84;
    u8 field_0x85;
    u8 field_0x86;
    u8 field_0x87;
    u32 g_flags;
    u8 field_0x8c;
    u8 field_0x8d;
    u8 field_0x8e;
    u8 field_0x8f;
    u32 field_0x90;
    u32 field_0x94;
    float field_0x98;
    float field_0x9c;
    char text[48]; /* If this sprite displays text, this is what it shows, otherwise this is usually just an identifier name */
} __attribute__((__packed__));

static_assert(sizeof(Sprite) == 208);

extern "C"
{
extern Sprite sprites[MAX_SPRITES];
extern PoolInfo sprite_pool_info;

void sprite_pausemenu_disp(Sprite *sprite);
void sprite_goal_disp(Sprite *sprite);
void sprite_clear_score_disp(Sprite *sprite);
void sprite_warp_bonus_disp(Sprite *sprite);
void sprite_time_bonus_disp(Sprite *sprite);
void sprite_stage_score_disp(Sprite *sprite);
void sprite_fallout_tick(u8 *status, Sprite *sprite);
void sprite_bonus_finish_or_perfect_tick(u8 *status, Sprite *sprite);
void sprite_timer_ball_tick(u8 *status, Sprite *sprite);
void sprite_score_tick(u8 *status, Sprite *sprite);
void sprite_score_disp(Sprite *sprite);
void sprite_go_tick(u8 *status, Sprite *sprite);
void sprite_ready_tick(u8 *status, Sprite *sprite);
void sprite_player_num_tick(u8 *status, Sprite *sprite);
void sprite_replay_tick(u8 *status, Sprite *sprite);
void sprite_loadin_stage_name_tick(u8 *status, Sprite *sprite);
void sprite_bonus_stage_tick(u8 *status, Sprite *sprite);
void sprite_final_stage_tick(u8 *status, Sprite *sprite);
void create_score_sprites(f32 pos_x, f32 pos_y);
}

}
