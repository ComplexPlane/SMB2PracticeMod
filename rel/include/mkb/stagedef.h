#pragma once

#include <cstdint>

namespace mkb
{

enum BananaType
{
    BANANA_BUNCH = 1,
    BANANA_SINGLE = 0
};

enum StagedefAnimType
{
    ANIM_LOOP,
    ANIM_ONCE,
    ANIM_SEESAW,
};

enum PlaybackState
{
    PLAYBACK_BACKWARD = 2,
    PLAYBACK_FAST_BACKWARD = 4,
    PLAYBACK_FAST_FORWARD = 3,
    PLAYBACK_FORWARD = 0,
    PLAYBACK_PAUSE = 1
};

struct StagedefAnimHeader
{
    u32 rot_x_keyframe_count;
    struct StagedefAnimKeyframe *rot_x_keyframe_list;
    u32 rot_y_keyframe_count;
    struct StagedefAnimKeyframe *rot_y_keyframe_list;
    u32 rot_z_keyframe_count;
    struct StagedefAnimKeyframe *rot_z_keyframe_list;
    u32 pos_x_keyframe_count;
    struct StagedefAnimKeyframe *pos_x_keyframe_list;
    u32 pos_y_keyframe_count;
    struct StagedefAnimKeyframe *pos_y_keyframe_list;
    u32 pos_z_keyframe_count;
    struct StagedefAnimKeyframe *pos_z_keyframe_list;
    u8 unk_0x30[16];
} __attribute__((__packed__));

static_assert(sizeof(StagedefAnimHeader) == 0x40);

struct StagedefAnimKeyframe
{
    u32 easing;
    f32 time;
    f32 value;
    u8 unk_0xc[8];
} __attribute__((__packed__));

static_assert(sizeof(StagedefAnimKeyframe) == 0x14);

struct StagedefBackgroundAnim2Header
{
    u8 unk_0x0[4];
    f32 loop_point_seconds;
    u32 unk1_keyframe_count;
    struct StagedefAnimKeyframe *unk1_keyframe_list;
    u32 unk2_keyframe_count;
    struct StagedefAnimKeyframe *unk2_keyframe_list;
    u32 rot_x_keyframe_count;
    struct StagedefAnimKeyframe *rot_x_keyframe_list;
    u32 rot_y_keyframe_count;
    struct StagedefAnimKeyframe *rot_y_keyframe_list;
    u32 rot_z_keyframe_count;
    struct StagedefAnimKeyframe *rot_z_keyframe_list;
    u32 pos_x_keyframe_count;
    struct StagedefAnimKeyframe *pos_x_keyframe_list;
    u32 pos_y_keyframe_count;
    struct StagedefAnimKeyframe *pos_y_keyframe_list;
    u32 pos_z_keyframe_count;
    struct StagedefAnimKeyframe *pos_z_keyframe_list;
    u32 unk9_keyframe_count;
    struct StagedefAnimKeyframe *unk9_keyframe_list;
    u32 unk10_keyframe_count;
    struct StagedefAnimKeyframe *unk10_keyframe_list;
    u32 unk11_keyframe_count;
    struct StagedefAnimKeyframe *unk11_keyframe_list;
} __attribute__((__packed__));

static_assert(sizeof(StagedefBackgroundAnim2Header) == 0x60);

struct StagedefBackgroundAnimHeader
{
    u8 unk_0x0[4];
    f32 loop_point_seconds;
    u8 unk_0x8[8];
    u32 rot_x_keyframe_count;
    struct StagedefAnimKeyframe *rot_x_keyframe_list;
    u32 rot_y_keyframe_count;
    struct StagedefAnimKeyframe *rot_y_keyframe_list;
    u32 rot_z_keyframe_count;
    struct StagedefAnimKeyframe *rot_z_keyframe_list;
    u32 pos_x_keyframe_count;
    struct StagedefAnimKeyframe *pos_x_keyframe_list;
    u32 pos_y_keyframe_count;
    struct StagedefAnimKeyframe *pos_y_keyframe_list;
    u32 pos_z_keyframe_count;
    struct StagedefAnimKeyframe *pos_z_keyframe_list;
    u8 unk_0x40[16];
} __attribute__((__packed__));

static_assert(sizeof(StagedefBackgroundAnimHeader) == 0x50);

struct StagedefEffectHeader
{
    u32 fx1_keyframe_count;
    struct StagedefEffect1 *fx1_keyframe_list;
    u32 fx2_keyframe_count;
    struct StagedefEffect2 *fx2_keyframe_list;
    struct StagedefTextureScroll *texture_scroll;
    u8 unk_0x14[28];
} __attribute__((__packed__));

static_assert(sizeof(StagedefEffectHeader) == 0x30);

struct StagedefEffect1
{ /* Likely used for animated textures, such as the silhouettes in Night */
    u32 unk_0x0;
    u32 unk_0x4;
    u32 unk_0x8;
    u16 unk_0x12;
    u16 unk_0x14;
    u16 unk_0x16;
    u8 unk_0x18[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefEffect1) == 0x14);

struct StagedefEffect2
{ /* May be used for the flames in Storm? */
    f32 unk_0x0;
    f32 unk_0x4;
    f32 unk_0x8;
    u8 unk_0xc[4];
} __attribute__((__packed__));

static_assert(sizeof(StagedefEffect2) == 0x10);

struct StagedefTextureScroll
{
    struct Vec2f speed;
} __attribute__((__packed__));

static_assert(sizeof(StagedefTextureScroll) == 0x8);

struct StagedefBackgroundModel
{
    u8 unk_0x0[4];
    char *model_name;
    u8 unk_0x8[4];
    struct Vec3f position;
    struct Vec3s rotation;
    u8 padding[2];
    struct Vec3f scale;
    struct StagedefBackgroundAnimHeader *background_anim_header;
    struct StagedefBackgroundAnim2Header *background_anim2_header;
    struct StagedefEffectHeader *effect_header;
} __attribute__((__packed__));

static_assert(sizeof(StagedefBackgroundModel) == 0x38);

struct StagedefBanana
{
    struct Vec3f position;
    u32 type;
} __attribute__((__packed__));

static_assert(sizeof(StagedefBanana) == 0x10);

struct StagedefStageModelInstance
{
    struct StagedefStageModelPtrA *stage_model_a;
    struct Vec3f position;
    struct Vec3s rotation;
    u8 padding[2];
    struct Vec3f scale;
} __attribute__((__packed__));

static_assert(sizeof(StagedefStageModelInstance) == 0x24);

struct StagedefFogAnimHeader
{
    u32 start_distance_keyframe_count;
    struct StagedefAnimKeyframe *start_distance_keyframe_list;
    u32 end_distance_keyframe_count;
    struct StagedefAnimKeyframe *end_distance_keyframe_list;
    u32 red_keyframe_count;
    struct StagedefAnimKeyframe *red_keyframe_list;
    u32 green_keyframe_count;
    struct StagedefAnimKeyframe *green_keyframe_list;
    u32 blue_keyframe_count;
    struct StagedefAnimKeyframe *blue_keyframe_list;
    u32 unk_keyframe_count;
    struct StagedefAnimKeyframe *unk_keyframe_list;
} __attribute__((__packed__));

static_assert(sizeof(StagedefFogAnimHeader) == 0x30);

struct StagedefConeCollision
{
    struct Vec3f position;
    struct Vec3s rotation;
    u8 padding[2];
    struct Vec3f scale;
} __attribute__((__packed__));

static_assert(sizeof(StagedefConeCollision) == 0x20);

struct StagedefBumper
{
    struct Vec3f position;
    struct Vec3s rotation;
    u8 padding[2];
    struct Vec3f scale;
} __attribute__((__packed__));

static_assert(sizeof(StagedefBumper) == 0x20);

struct StagedefReflectiveStageModel
{
    char *model_name;
    u8 unk_0x8[8];
} __attribute__((__packed__));

static_assert(sizeof(StagedefReflectiveStageModel) == 0xc);

struct StagedefFallout
{
    f32 y;
} __attribute__((__packed__));

static_assert(sizeof(StagedefFallout) == 0x4);

struct StagedefStageModel
{
    u8 unk_0x0[4];
    char *model_name;
    u8 unk_0x8[8];
} __attribute__((__packed__));

static_assert(sizeof(StagedefStageModel) == 0x10);

struct StagedefCollisionHeader
{
    struct Vec3f origin;
    struct Vec3s initial_rotation;
    s16 anim_loop_type_and_seesaw;
    struct StagedefAnimHeader *animation_header;
    struct Vec3f conveyor_speed;
    struct StagedefCollisionTri *collision_triangle_list;
    u16 **collision_grid_triangle_idx_list_list;
    struct Vec2f collision_grid_start;
    struct Vec2f collision_grid_step;
    struct Vec2i collision_grid_step_count;
    u32 goal_count;
    struct StagedefGoal *goal_list;
    u32 bumper_count;
    struct StagedefBumper *bumper_list;
    u32 jamabar_count;
    struct StagedefJamabar *jamabar_list;
    u32 banana_count;
    struct StagedefBanana *banana_list;
    u32 cone_collision_object_count;
    struct StagedefConeCollision *cone_collision_object_list;
    u32 sphere_collision_object_count;
    struct StagedefSphereCollision *sphere_collision_object_list;
    u32 cylinder_collision_object_count;
    struct StagedefCylinderCollision *cylinder_collision_object_list;
    u32 fallout_volume_count;
    struct StagedefFalloutVolume *fallout_volume_list;
    u32 reflective_stage_model_count;
    struct StagedefReflectiveStageModel *reflective_stage_model_list;
    u32 stage_model_instance_count;
    struct StagedefStageModelInstance *stage_model_instance_list;
    u32 stage_model_b_count;
    struct StagedefStageModelPtrB *stage_model_b_list;
    u8 unk_0x9c[8];
    u16 anim_group_id; /* For use with buttons - Must be non-zero if you want to assign a group ID */
    u8 padding[2];
    u32 button_count;
    struct StagedefButton *button_list;
    u8 unk_0xb0[4];
    struct StagedefMystery5 *mystery5;
    f32 seesaw_sensitivity; /* Higher is more sensitive, negative makes the seesaw reversed */
    f32 seesaw_friction; /* Lower is looser */
    f32 seesaw_spring; /* 0 prevents the seesaw from resetting */
    u32 wormhole_count;
    struct stagedef_wormhole *wormhole_list;
    u32 initial_playback_state; /* Should this be split into 2x padding bytes + PlaybackState enum? */
    u8 unk_0xd0[4];
    f32 anim_loop_point_seconds;
    struct StagedefTextureScroll *texture_scroll;
    u8 unk_0xdc[960];
} __attribute__((__packed__));

static_assert(sizeof(StagedefCollisionHeader) == 0x49c);

struct StagedefStageModelPtrA
{
    u8 unk_0x0[8];
    struct StagedefStageModel *stage_model;
} __attribute__((__packed__));

static_assert(sizeof(StagedefStageModelPtrA) == 0xc);

struct StagedefForegroundModel
{
    u8 unk_0x0[4]; /* Unknown - typically 0x0000001F, sometimes 0x00000007 or 0x0000000F */
    char *model_name;
    u8 unk_0x8[4];
    Vec3f position;
    Vec3s rotation;
    u8 padding[2];
    Vec3f scale;
    u8 unk_0x2c[4];
    struct BackgroundAnim2Header *backgroundAnim2Header;
    void *unk_0x34;
} __attribute__((__packed__));

static_assert(sizeof(StagedefForegroundModel) == 0x38);

struct StagedefStageModelPtrB
{
    struct StagedefStageModelPtrA *stage_model_a;
} __attribute__((__packed__));

static_assert(sizeof(StagedefStageModelPtrB) == 0x4);

struct StagedefMystery5
{
    u8 unk_0x0[4];
    f32 unk_0x4;
    f32 unk_0x8;
    f32 unk_0xc;
    f32 unk_0x10;
} __attribute__((__packed__));

static_assert(sizeof(StagedefMystery5) == 0x14);

struct StagedefSphereCollision
{
    struct Vec3f position;
    f32 radius;
    u8 unk_0x10[4]; /* Nullable */
} __attribute__((__packed__));

static_assert(sizeof(StagedefSphereCollision) == 0x14);

struct StagedefFileHeader
{
    u32 magic_number_a;
    u32 magic_number_b;
    u32 collision_header_count;
    struct StagedefCollisionHeader *collision_header_list;
    struct StagedefStart *start;
    struct StagedefFallout *fallout;
    u32 goal_count;
    struct StagedefGoal *goal_list;
    u32 bumper_count;
    struct StagedefBumper *bumper_list;
    u32 jamabar_count;
    struct StagedefJamabar *jamabar_list;
    u32 banana_count;
    struct StagedefBanana *banana_list;
    u32 cone_collision_object_count;
    struct StagedefConeCollision *cone_collision_object_list;
    u32 sphere_collision_object_count;
    struct StagedefSphereCollision *sphere_collision_object_list;
    u32 cylinder_collision_object_count;
    struct StagedefCylinderCollision *cylinder_collision_object_list;
    u32 fallout_volume_count;
    struct StagedefFalloutVolume *fallout_volume_list;
    u32 background_model_count;
    struct StagedefBackgroundModel *background_model_list;
    u32 foreground_model_count;
    struct StagedefForegroundModel *foreground_model_list;
    u8 unk_0x68[8];
    u32 reflective_stage_model_count;
    struct StagedefReflectiveStageModel *reflective_stage_model_list;
    u8 unk_0x78[12];
    u32 stage_model_instance_count;
    struct StagedefStageModelInstance *stage_model_instance_list;
    u32 stage_model_a_count;
    struct StagedefStageModelPtrA *stage_model_a_list;
    u32 stage_model_b_count;
    struct StagedefStageModelPtrB *stage_model_b_list;
    u8 unk_0x9c[12];
    u32 button_count;
    struct StagedefButton *button_list;
    struct StagedefFogAnimHeader *fog_animation_header; /* Nullable */
    u32 wormhole_count;
    struct StagedefWormhole *wormhole_list;
    struct StagedefFog *fog; /* Nullable */
    u8 unk_0xc0[20];
    struct StagedefMystery3 *mystery3;
    u8 unk_0xd8[1988];
} __attribute__((__packed__));

static_assert(sizeof(StagedefFileHeader) == 0x89c);

struct StagedefJamabar
{
    struct Vec3f position;
    struct Vec3s rotation;
    u8 padding[2];
    struct Vec3f scale;
} __attribute__((__packed__));

static_assert(sizeof(StagedefJamabar) == 0x20);

struct StagedefMystery3
{
    f32 unk_0x0;
    f32 unk_0x4;
    f32 unk_0x8;
    u8 unk_0xc[24];
} __attribute__((__packed__));

static_assert(sizeof(StagedefMystery3) == 0x24);

struct StagedefFalloutVolume
{
    struct Vec3f position;
    struct Vec3f size;
    struct Vec3s rotation;
    u8 padding[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefFalloutVolume) == 0x20);

struct StagedefCollisionTri
{
    struct Vec3f point1_position;
    struct Vec3f normal;
    struct Vec3s rotation_from_xy;
    u8 padding[2];
    struct Vec2f point2_delta_pos_from_point1; /* Before rotation is applied */
    struct Vec2f point3_delta_pos_from_point1;
    struct Vec2f tangent;
    struct Vec2f bitangent;
} __attribute__((__packed__));

static_assert(sizeof(StagedefCollisionTri) == 0x40);

struct StagedefGoal
{
    struct Vec3f position;
    struct Vec3s rotation;
    s16 goal_flags;
} __attribute__((__packed__));

static_assert(sizeof(StagedefGoal) == 0x14);

struct StagedefWormhole
{
    u8 field_0x0;
    u8 field_0x1;
    u8 field_0x2;
    u8 field_0x3;
    struct Vec3f positon;
    struct Vec3s rotation;
    u8 padding[2];
    struct StagedefWormhole *destination;
} __attribute__((__packed__));

static_assert(sizeof(StagedefWormhole) == 0x1c);

struct StagedefCylinderCollision
{
    struct Vec3f position;
    f32 radius;
    f32 height;
    struct Vec3s rotation;
    u8 padding[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefCylinderCollision) == 0x1c);

struct StagedefStart
{
    struct Vec3f position;
    struct Vec3s rotation;
    u8 padding[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefStart) == 0x14);

struct StagedefButton
{
    struct Vec3f position;
    struct Vec3s rotation;
    u16 playback_state;
    u16 anim_group_id;
    u8 padding[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefButton) == 0x18);

struct StagedefFog
{
    u8 type; /* Same as GXFogType */
    u8 padding[3];
    f32 fog_start_distance;
    f32 fog_end_distance;
    struct Vec3f color;
    u8 unk_0x18[12];
} __attribute__((__packed__));

static_assert(sizeof(StagedefFog) == 0x24);

}
