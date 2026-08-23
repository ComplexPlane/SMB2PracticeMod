#pragma once

#define MKB_CONCAT_IMPL(a, b) a##b
#define MKB_CONCAT(a, b) MKB_CONCAT_IMPL(a, b)
#ifdef __cplusplus
#define MKB(name) name
#else
#define MKB(name) MKB_CONCAT(mkb_, name)
#endif
#ifdef __cplusplus
namespace mkb {
#endif

typedef unsigned char   MKB(undefined);

#ifndef __cplusplus
#if __STDC_VERSION__ < 23
#include <stdbool.h>
#endif
#endif
typedef unsigned char    byte;
typedef unsigned int    dword;
typedef long long    longlong;
typedef unsigned long long    qword;
typedef unsigned char    uchar;
typedef unsigned int    uint;
typedef unsigned long long    uint16;
typedef unsigned int    uint3;
typedef unsigned long    ulong;
typedef unsigned long long    ulonglong;
typedef unsigned char    undefined1;
typedef unsigned short    undefined2;
typedef unsigned int    undefined3;
typedef unsigned int    undefined4;
typedef unsigned long long    undefined7;
typedef unsigned long long    undefined8;
typedef unsigned short    ushort;
typedef unsigned short    word;
typedef struct MKB(SeesawState) MKB(SeesawState), *MKB(PSeesawState);

typedef float MKB(f32);

typedef MKB(f32) MKB(Mtx)[3][4];

struct MKB(SeesawState) {
    float angle; /* Current rotation angle as a float, in range [-0x8000, 0x7FFF] similar to s16 rotation angles */
    float prev_angle;
    float angle_vel;
    float sensitivity; /* Created by retype action */
    float friction; /* Created by retype action */
    float spring; /* Created by retype action */
    MKB(Mtx) inv_transform;
    MKB(Mtx) transform;
} __attribute__((__packed__));
static_assert(sizeof(MKB(SeesawState)) == 0x78);

enum { /* The character associated with an Ape */
    MKB(CHARA_AIAI)=0,
    MKB(CHARA_MEEMEE)=1,
    MKB(CHARA_BABY)=2,
    MKB(CHARA_GONGON)=3,
    MKB(CHARA_MADH)=4,
    MKB(CHARA_KOBUN)=5,
    MKB(CHARA_MALE)=6,
    MKB(CHARA_FEMALE)=7,
    MKB(CHARA_JIJI)=8,
    MKB(CHARA_BABA)=9,
    MKB(CHARA_MADSPA)=10,
    MKB(CHARA_MADFRG)=11,
    MKB(CHARA_MADNKD)=12,
    MKB(CHARA_ROB)=13,
    MKB(CHARA_WHALE)=14,
    MKB(CHARA_MADCRS)=15,
    MKB(CHARA_KOBSPA)=16
};
typedef undefined4 MKB(ApeCharacter);

enum { /* When we're in Master, current difficulty is still 0x2 */
    MKB(DIFF_BEGINNER)=0,
    MKB(DIFF_ADVANCED)=1,
    MKB(DIFF_EXPERT)=2
};
typedef undefined4 MKB(Difficulty);

enum {
    MKB(G_STORY_LEVEL_SELECT_NO_BACKGROUND)=0,
    MKB(G_LAVA_NO_BACKGROUND)=1,
    MKB(JUNGLE1)=2,
    MKB(WATER1)=3,
    MKB(NIGHT)=4,
    MKB(SKY)=5,
    MKB(SPACE1)=6,
    MKB(SAND)=7,
    MKB(ICE)=8,
    MKB(STORM)=9,
    MKB(BONUS1)=10,
    MKB(TARGET1)=11,
    MKB(G_CONTINUE_A)=12,
    MKB(G_CONTINUE_B)=13,
    MKB(G_FIGHT2)=14,
    MKB(MASTER)=15,
    MKB(G_ENDING)=16,
    MKB(LAVA)=17,
    MKB(WATER2)=18,
    MKB(JUNGLE2)=19,
    MKB(PARK)=20,
    MKB(POT)=21,
    MKB(SPACE2)=22,
    MKB(ELECTRIC)=23,
    MKB(GEAR)=24,
    MKB(BUBBLY)=25,
    MKB(BONUS2)=26,
    MKB(SOCCER)=27,
    MKB(BOWLING2)=28,
    MKB(TARGET2)=29,
    MKB(BILLIARDS2)=30,
    MKB(BOAT)=31,
    MKB(WHALE)=32,
    MKB(GOLF)=33,
    MKB(POT_FIGHT)=34,
    MKB(VILLAGE_STORY)=35,
    MKB(BUBBLY_STORY)=36,
    MKB(PARK_STORY)=37,
    MKB(GEAR_STORY)=38,
    MKB(UNDERWATER_STORY)=39,
    MKB(WATER_SURFACE_STORY)=40,
    MKB(BOWLING_STORY)=41
};
typedef undefined2 MKB(WorldTheme);

typedef struct MKB(GoalTape) MKB(GoalTape), *MKB(PGoalTape);

typedef long MKB(s32);

typedef struct MKB(Stobj) MKB(Stobj), *MKB(PStobj);

typedef struct MKB(PhysicsBall) MKB(PhysicsBall), *MKB(PPhysicsBall);

enum {
    MKB(STOBJ_BUMPER)=0,
    MKB(STOBJ_JAMABAR)=1,
    MKB(STOBJ_GOALTAPE)=2,
    MKB(STOBJ_GOALBAG)=3,
    MKB(STOBJ_GOALBAG_EXMASTER)=4,
    MKB(STOBJ_MF_PNL_BUMPER)=5,
    MKB(STOBJ_MF_PNL_ELECTRAP)=6,
    MKB(STOBJ_MF_BULLET_TEST)=7,
    MKB(STOBJ_MF_BOX)=8,
    MKB(STOBJ_BUMPER_BGSPECIAL)=9,
    MKB(STOBJ_NAMEENT_BTN)=10,
    MKB(STOBJ_BUTTON)=11,
    MKB(STOBJ_RETURNGATE)=12
};
typedef undefined2 MKB(StobjType);

typedef struct MKB(Vec) MKB(Vec), *MKB(PVec);

typedef struct MKB(ColiPlane) MKB(ColiPlane), *MKB(PColiPlane);

typedef struct MKB(GmaModel) MKB(GmaModel), *MKB(PGmaModel);

typedef struct MKB(S16Vec) MKB(S16Vec), *MKB(PS16Vec);

typedef signed char MKB(s8);

typedef ushort MKB(u16);

enum { /* Per-GMA model attributes */
    MKB(GCMF_ATTR_16BIT)=1,
    MKB(GCMF_ATTR_STITCHING_MODEL)=4,
    MKB(GCMF_ATTR_SKIN_MODEL)=8,
    MKB(GCMF_ATTR_EFFECTIVE_MODEL)=16
};
typedef undefined4 MKB(GcmfAttributes);

typedef struct MKB(GXTexObj) MKB(GXTexObj), *MKB(PGXTexObj);

typedef short MKB(s16);

enum {
    MKB(GX_TF_I4)=0,
    MKB(GX_TF_I8)=1,
    MKB(GX_TF_IA4)=2,
    MKB(GX_TF_IA8)=3,
    MKB(GX_TF_RGB565)=4,
    MKB(GX_TF_RGB5A3)=5,
    MKB(GX_TF_RGBA8)=6,
    MKB(GX_TF_CMPR)=14,
    MKB(GX_CTF_R4)=15,
    MKB(GX_CTF_RA4)=16,
    MKB(GX_CTF_RA8)=17,
    MKB(GX_CTF_YUVA8)=18,
    MKB(GX_CTF_A8)=19,
    MKB(GX_CTF_R8)=20,
    MKB(GX_CTF_G8)=21,
    MKB(GX_CTF_B8)=22,
    MKB(GX_CTF_RG8)=23,
    MKB(GX_CTF_GB8)=24,
    MKB(GX_TF_Z8)=25,
    MKB(GX_TF_Z16)=26,
    MKB(GX_TF_Z24X8)=27,
    MKB(GX_CTF_Z4)=28,
    MKB(GX_CTF_Z8M)=29,
    MKB(GX_CTF_Z8L)=30,
    MKB(GX_CTF_Z16L)=31,
    MKB(GX_TF_A8)=32
};
typedef undefined4 MKB(GXTexFmt);

struct MKB(Vec) {
    float x;
    float y;
    float z;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Vec)) == 0xc);

struct MKB(ColiPlane) {
    struct MKB(Vec) point;
    struct MKB(Vec) normal;
    MKB(u16) g_flags1;
    MKB(u16) g_flags2;
} __attribute__((__packed__));
static_assert(sizeof(MKB(ColiPlane)) == 0x1c);

struct MKB(S16Vec) { /* Often used for rotations */
    MKB(s16) x;
    MKB(s16) y;
    MKB(s16) z;
} __attribute__((__packed__));
static_assert(sizeof(MKB(S16Vec)) == 0x6);

struct MKB(GoalTape) { /* Extra goaltape-specific data pointed to by goaltape StageObjects */
    MKB(undefined) field_0x0[0x4];
    float field4_0x4;
    float field5_0x8;
    float field6_0xc;
    MKB(s32) g_timer_frames_remaining;
    struct MKB(Stobj) * g_stobj_ptr;
    float field9_0x18;
    float field10_0x1c;
    float field11_0x20;
    float field12_0x24;
    float field13_0x28;
    float field14_0x2c;
    float field15_0x30;
    float field16_0x34;
    float field17_0x38;
    float field18_0x3c;
    float field19_0x40;
    float field20_0x44;
    MKB(undefined) field_0x48[0x148];
    uint field349_0x190;
    MKB(undefined) field_0x194[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GoalTape)) == 0x198);

struct MKB(GXTexObj) {
    MKB(undefined) field_0x0[0x14];
    MKB(GXTexFmt)  format; /* Created by retype action */
    MKB(undefined) field_0x18[0x8];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXTexObj)) == 0x20);

struct MKB(PhysicsBall) { /* A representation of a Ball with just the physics/collision-related info */
    dword flags;
    struct MKB(Vec) pos;
    struct MKB(Vec) prev_pos;
    struct MKB(Vec) vel;
    float radius;
    float acceleration;
    float restitution;
    dword hardest_coli_speed;
    struct MKB(ColiPlane) hardest_coli_plane;
    dword hardest_coli_ig_idx;
    float friction;
    dword itemgroup_idx; /* The itemgroup that this PhysicsBall is relative to, aka in the local space of */
} __attribute__((__packed__));
static_assert(sizeof(MKB(PhysicsBall)) == 0x60);

struct MKB(Stobj) { /* A "stage object" which is one of a: bumper, jamabar, goaltape, party ball, wormhole, and others. */
    word idx;
    short id;
    MKB(StobjType)  type;
    MKB(undefined) field_0x6[0x2];
    uint g_some_bitflag;
    short g_mode;
    short g_counter;
    struct MKB(Vec) g_model_origin;
    struct MKB(Vec) position;
    struct MKB(Vec) position_2; /* Copy of position? */
    float bounding_sphere_radius; /* Has something to do w/ collision */
    void (* coli_func)(struct MKB(Stobj) *, struct MKB(PhysicsBall) *);
    struct MKB(Vec) scale;
    float field14_0x48;
    float field15_0x4c;
    float field16_0x50;
    struct MKB(GmaModel) * model;
    struct MKB(Vec) pos; /* Has something to do w/ position */
    struct MKB(Vec) vel;
    struct MKB(S16Vec) rot;
    short field21_0x76;
    short field22_0x78;
    MKB(undefined) field_0x7a[0x2];
    struct MKB(Vec) g_prev_pos;
    struct MKB(S16Vec) g_prev_rot;
    MKB(undefined) field_0x8e[0x2];
    float field29_0x90;
    float field30_0x94;
    float field31_0x98;
    float field32_0x9c;
    MKB(s8) itemgroup_idx;
    MKB(undefined) field_0xa1[0x3];
    void * extra_data; /* Extra stobj-type-specific data, such as switch stagedef header for switches or goaltape struct for goaltapes. Maybe worth making a union */
    struct MKB(Vec) g_some_pos2;
    struct MKB(Vec) g_local_position;
    struct MKB(Vec) g_local_velocity;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Stobj)) == 0xcc);

struct MKB(GmaModel) { /* Also known as a GCMF (GameCube Model Format?) */
    char gcmf_magic[4]; /* Just the string "GCMF" */
    MKB(GcmfAttributes)  attrs;
    struct MKB(Vec) bound_sphere_center; /* Also the center of the bounding sphere */
    float bound_sphere_radius;
    MKB(u16) tev_layer_count;
    MKB(u16) opaque_shape_count;
    MKB(u16) translucent_shape_count;
    MKB(s8) mtx_count;
    MKB(undefined) field_0x1f[0x1];
    MKB(s32) header_size; /* Memory size of this structure in bytes, including texture description array and transform matrices. */
    struct MKB(GXTexObj) * texobjs; /* Array of texobjs, one for each texture in the model */
    MKB(s8) mtx_indexes[8]; /* Default (root?) indices into Transform Matrix array */
    MKB(undefined) field_0x30[0x10];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GmaModel)) == 0x40);

typedef struct MKB(MenuScreen) MKB(MenuScreen), *MKB(PMenuScreen);

typedef uchar MKB(u8);

typedef struct MKB(MenuEntry) MKB(MenuEntry), *MKB(PMenuEntry);

typedef uint MKB(uint32_t);

typedef MKB(uint32_t) MKB(u32);

enum {
    MKB(MENUSCREEN_MODE_SELECT)=0,
    MKB(MENUSCREEN_PARTY_GAME_SELECT)=1,
    MKB(MENUSCREEN_OPTIONS)=2,
    MKB(MENUSCREEN_RETURN_TO_TITLE_SCREEN)=3,
    MKB(MENUSCREEN_NUMBER_OF_PLAYERS)=4,
    MKB(MENUSCREEN_CHARACTER_SELECT_1)=5,
    MKB(MENUSCREEN_CHARACTER_SELECT_2)=6,
    MKB(MENUSCREEN_MAIN_GAME_SELECT)=7,
    MKB(MENUSCREEN_LEVEL_SELECT_NOMASTER)=8,
    MKB(MENUSCREEN_LEVEL_SELECT_WITHMASTER)=9,
    MKB(MENUSCREEN_PRACTICE_STAGE_SELECT)=10,
    MKB(MENUSCREEN_CHALLENGE_GAMEPLAY_SETTINGS)=11,
    MKB(MENUSCREEN_STORY_MODE_SELECTED)=12,
    MKB(MENUSCREEN_CHALLENGE_DIFFICULTY)=13,
    MKB(MENUSCREEN_MONKEY_RACE_MODE_SELECT)=15,
    MKB(MENUSCREEN_MONKEY_RACE_COURSE_SELECT)=16,
    MKB(MENUSCREEN_REPLAY)=80,
    MKB(MENUSCREEN_PLAY_POINTS)=81,
    MKB(MENUSCREEN_GIFT)=82,
    MKB(MENUSCREEN_RANKING)=83,
    MKB(MENUSCREEN_GAME_DATA_SETTINGS)=84,
    MKB(MENUSCREEN_CONTROLLER_SETTINGS)=85,
    MKB(MENUSCREEN_SCREEN_SETTINGS)=86,
    MKB(MENUSCREEN_SOUND_SETTINGS)=87
};
typedef undefined1 MKB(MenuScreenID);

struct MKB(MenuScreen) {
    MKB(u8) field0_0x0;
    MKB(u8) entry_count;
    MKB(undefined) field_0x2[0x2];
    struct MKB(MenuEntry) * entries;
    void * tick;
    MKB(u32) bitflag; /* 0x40 repositions stuff and makes stuff up/down controls */
} __attribute__((__packed__));
static_assert(sizeof(MKB(MenuScreen)) == 0x10);

struct MKB(MenuEntry) {
    MKB(MenuScreenID)  next_screen_id;
    MKB(undefined) padding_0x1[0x3];
    char * name_en;
    char * name_de;
    char * name_fr;
    char * name_es;
    char * name_it;
    char * name_ja;
    char * description_en;
    char * description_de;
    char * description_fr;
    char * description_es;
    char * description_it;
    char * description_ja;
} __attribute__((__packed__));
static_assert(sizeof(MKB(MenuEntry)) == 0x34);

typedef struct MKB(BmpInfo) MKB(BmpInfo), *MKB(PBmpInfo);

typedef int MKB(BOOL32);

typedef struct MKB(TplBuffer) MKB(TplBuffer), *MKB(PTplBuffer);

typedef int MKB(OSHeapHandle);

typedef struct MKB(TplTextureHeader) MKB(TplTextureHeader), *MKB(PTplTextureHeader);

struct MKB(TplBuffer) { /* Buffer allocated for TPL files (with 32 extra bytes at the beginning compared to the on-disc TPL file). Amusement Vision TPL is different than standard Gamecube TPL */
    MKB(s32) texture_count;
    struct MKB(TplTextureHeader) * texture_headers;
    void * raw_tpl_buffer; /* Pointer to the raw TPL data loaded from disc */
    struct MKB(GXTexObj) * texobjs; /* Array of texobjs, one for each texture in the TPL */
    MKB(undefined) field_0x10[0x10];
} __attribute__((__packed__));
static_assert(sizeof(MKB(TplBuffer)) == 0x20);

struct MKB(BmpInfo) { /* Bitmap info. Corresponds to a loaded TPL in bmp/ */
    MKB(BOOL32) is_loaded;
    char * filepath;
    char * category;
    MKB(undefined) field_0xc[0x4];
    struct MKB(TplBuffer) * tpl;
    MKB(OSHeapHandle) heap;
} __attribute__((__packed__));
static_assert(sizeof(MKB(BmpInfo)) == 0x18);

struct MKB(TplTextureHeader) {
    MKB(GXTexFmt)  format;
    dword data_offset;
    MKB(u16) width;
    MKB(u16) height;
    MKB(u16) mipmap_count;
    MKB(u16) always_0x1234;
} __attribute__((__packed__));
static_assert(sizeof(MKB(TplTextureHeader)) == 0x10);

typedef struct MKB(CoinType) MKB(CoinType), *MKB(PCoinType);

struct MKB(CoinType) {
    MKB(s16) banana_count;
    MKB(s16) g_score_increment_flag;
    MKB(s16) g_score_value;
    struct MKB(S16Vec) angular_velocity;
    MKB(undefined) field_0xc[0x8];
} __attribute__((__packed__));
static_assert(sizeof(MKB(CoinType)) == 0x14);

typedef struct MKB(GMotionData) MKB(GMotionData), *MKB(PGMotionData);

struct MKB(GMotionData) {
    MKB(undefined) field_0x0[0x4c];
    float progress;
    float length;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GMotionData)) == 0x54);

enum { /* NULL, INIT, NORMAL, and DEST, and FREEZE seem to be the most common */
    MKB(STAT_NULL)=0,
    MKB(STAT_INIT)=1,
    MKB(STAT_NORMAL)=2,
    MKB(STAT_DEST)=3,
    MKB(STAT_FREEZE)=4,
    MKB(STAT_INIT_FIRST)=5,
    MKB(STAT_GOAL_INIT)=6,
    MKB(STAT_GOAL)=7,
    MKB(STAT_RINGOUT_INIT)=8,
    MKB(STAT_RINGOUT)=9,
    MKB(STAT_MINI_MODE_0_INIT)=10,
    MKB(STAT_MINI_MODE_0)=11
};
typedef undefined1 MKB(Status);

typedef struct MKB(GSomeBgStruct) MKB(GSomeBgStruct), *MKB(PGSomeBgStruct);

struct MKB(GSomeBgStruct) {
    MKB(u32) some_flag;
    char * bg_model_list;
    MKB(u32) field2_0x8;
    MKB(u32) * some_ptr_2;
    MKB(u32) field4_0x10;
    MKB(u32) * some_ptr_3;
    MKB(u32) field6_0x18;
    MKB(u32) * some_ptr_4;
    MKB(u32) field8_0x20;
    MKB(u32) * some_ptr_5;
    MKB(u32) some_counter;
    MKB(u32) field11_0x2c;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GSomeBgStruct)) == 0x30);

enum { /* Sourced from 0x80370244 in Ghidra - I'm guessing SMD stands for Sub MoDe here? / Also I added SMD_INVALID, it's `-1` -Crafted */
    MKB(SMD_ADV_TOP)=0,
    MKB(SMD_ADV_MOVIE_INIT)=1,
    MKB(SMD_ADV_MOVIE_RETURN)=2,
    MKB(SMD_ADV_LOGO_INIT)=3,
    MKB(SMD_ADV_LOGO_MAIN)=4,
    MKB(SMD_ADV_WARNING_INIT)=5,
    MKB(SMD_ADV_WARNING_MAIN)=6,
    MKB(SMD_ADV_DEMO_INIT)=7,
    MKB(SMD_ADV_DEMO_MAIN)=8,
    MKB(SMD_ADV_START_INIT)=9,
    MKB(SMD_ADV_START_MAIN)=10,
    MKB(SMD_ADV_RANKING_INIT)=11,
    MKB(SMD_ADV_RANKING_MAIN)=12,
    MKB(SMD_ADV_INFO_INIT)=13,
    MKB(SMD_ADV_INFO_MAIN)=14,
    MKB(SMD_ADV_GAME_READY_INIT)=15,
    MKB(SMD_ADV_GAME_READY_MAIN)=16,
    MKB(SMD_ADV_GAME_PLAY_INIT)=17,
    MKB(SMD_ADV_GAME_PLAY_MAIN)=18,
    MKB(SMD_ADV_TITLE_INIT)=19,
    MKB(SMD_ADV_TITLE_MAIN)=20,
    MKB(SMD_ADV_TITLE_REINIT)=21,
    MKB(SMD_ADV_PROG_INIT)=22,
    MKB(SMD_ADV_PROG_MAIN)=23,
    MKB(SMD_ADV_FIRST_LOGO_INIT)=24,
    MKB(SMD_ADV_FIRST_LOGO_MAIN)=25,
    MKB(SMD_ADV_DMY8)=26,
    MKB(SMD_ADV_BOTTOM)=27,
    MKB(SMD_SEL_TOP)=28,
    MKB(SMD_SEL_STAGE_INIT)=29,
    MKB(SMD_SEL_STAGE_MAIN)=30,
    MKB(SMD_SEL_NGC_INIT)=31,
    MKB(SMD_SEL_NGC_MAIN)=32,
    MKB(SMD_SEL_NGC_REINIT)=33,
    MKB(SMD_SEL_NGC_DEST)=34,
    MKB(SMD_SEL_DMY1)=35,
    MKB(SMD_SEL_DMY2)=36,
    MKB(SMD_SEL_DMY3)=37,
    MKB(SMD_SEL_DMY4)=38,
    MKB(SMD_SEL_DMY5)=39,
    MKB(SMD_SEL_DMY6)=40,
    MKB(SMD_SEL_DMY7)=41,
    MKB(SMD_SEL_DMY8)=42,
    MKB(SMD_SEL_DMY9)=43,
    MKB(SMD_SEL_DMYa)=44,
    MKB(SMD_SEL_DMYb)=45,
    MKB(SMD_SEL_BOTTOM)=46,
    MKB(SMD_GAME_TOP)=47,
    MKB(SMD_GAME_READY_INIT)=48,
    MKB(SMD_GAME_READY_MAIN)=49,
    MKB(SMD_GAME_PLAY_INIT)=50,
    MKB(SMD_GAME_PLAY_MAIN)=51,
    MKB(SMD_GAME_GOAL_INIT)=52,
    MKB(SMD_GAME_GOAL_MAIN)=53,
    MKB(SMD_GAME_GOAL_REPLAY_INIT)=54,
    MKB(SMD_GAME_GOAL_REPLAY_MAIN)=55,
    MKB(SMD_GAME_TIMEOVER_INIT)=56,
    MKB(SMD_GAME_TIMEOVER_MAIN)=57,
    MKB(SMD_GAME_RINGOUT_INIT)=58,
    MKB(SMD_GAME_RINGOUT_MAIN)=59,
    MKB(SMD_GAME_FIRST_INIT)=60,
    MKB(SMD_GAME_OVER_INIT)=61,
    MKB(SMD_GAME_OVER_MAIN)=62,
    MKB(SMD_GAME_NAMEENTRY_INIT)=63,
    MKB(SMD_GAME_NAMEENTRY_MAIN)=64,
    MKB(SMD_GAME_CONTINUE_INIT)=65,
    MKB(SMD_GAME_CONTINUE_MAIN)=66,
    MKB(SMD_GAME_RESTART_INIT)=67,
    MKB(SMD_GAME_NAMEENTRY_READY_INIT)=68,
    MKB(SMD_GAME_NAMEENTRY_READY_MAIN)=69,
    MKB(SMD_GAME_BONUS_CLEAR_INIT)=70,
    MKB(SMD_GAME_BONUS_CLEAR_MAIN)=71,
    MKB(SMD_GAME_ENDING_INIT)=72,
    MKB(SMD_GAME_ENDING_MAIN)=73,
    MKB(SMD_GAME_EXTRA_INIT)=74,
    MKB(SMD_GAME_EXTRA_WAIT)=75,
    MKB(SMD_GAME_RESULT_INIT)=76,
    MKB(SMD_GAME_RESULT_MAIN)=77,
    MKB(SMD_GAME_RESULT_MENU)=78,
    MKB(SMD_GAME_INTR_SEL_INIT)=79,
    MKB(SMD_GAME_INTR_SEL_MAIN)=80,
    MKB(SMD_GAME_OVER_SAVE)=81,
    MKB(SMD_GAME_OVER_DEST)=82,
    MKB(SMD_GAME_ROLL_INIT)=83,
    MKB(SMD_GAME_ROLL_MAIN)=84,
    MKB(SMD_GAME_OVER_POINT_INIT)=85,
    MKB(SMD_GAME_OVER_POINT_MAIN)=86,
    MKB(SMD_GAME_SCENARIO_INIT)=87,
    MKB(SMD_GAME_SCENARIO_MAIN)=88,
    MKB(SMD_GAME_SCENARIO_RETURN)=89,
    MKB(SMD_GAME_RETRY_INIT)=90,
    MKB(SMD_GAME_RETRY_MAIN)=91,
    MKB(SMD_GAME_FORCE_EXIT_INIT)=92,
    MKB(SMD_GAME_FORCE_EXIT_MAIN)=93,
    MKB(SMD_GAME_SCENSCNPLAY_RETURN)=94,
    MKB(SMD_GAME_FORCE_OVER_INIT)=95,
    MKB(SMD_GAME_FORCE_OVER_MAIN)=96,
    MKB(SMD_GAME_SUGG_SAVE_INIT)=97,
    MKB(SMD_GAME_SUGG_SAVE_MAIN)=98,
    MKB(SMD_GAME_DMY3)=99,
    MKB(SMD_GAME_DMY4)=100,
    MKB(SMD_GAME_DMY5)=101,
    MKB(SMD_GAME_DMY6)=102,
    MKB(SMD_GAME_DMY7)=103,
    MKB(SMD_GAME_DMY8)=104,
    MKB(SMD_GAME_DMY9)=105,
    MKB(SMD_GAME_DMYA)=106,
    MKB(SMD_GAME_DMYB)=107,
    MKB(SMD_GAME_DMYC)=108,
    MKB(SMD_GAME_DMYD)=109,
    MKB(SMD_GAME_DMYE)=110,
    MKB(SMD_GAME_DMYF)=111,
    MKB(SMD_GAME_BOTTOM)=112,
    MKB(SMD_TEST_TOP)=113,
    MKB(SMD_TEST_SELECT_INIT)=114,
    MKB(SMD_TEST_SELECT_MAIN)=115,
    MKB(SMD_TEST_INPUT_INIT)=116,
    MKB(SMD_TEST_INPUT_MAIN)=117,
    MKB(SMD_TEST_DIP_INIT)=118,
    MKB(SMD_TEST_DIP_MAIN)=119,
    MKB(SMD_TEST_SOUND_INIT)=120,
    MKB(SMD_TEST_SOUND_MAIN)=121,
    MKB(SMD_TEST_BLUR_INIT)=122,
    MKB(SMD_TEST_BLUR_MAIN)=123,
    MKB(SMD_TEST_SHADOW_INIT)=124,
    MKB(SMD_TEST_SHADOW_MAIN)=125,
    MKB(SMD_TEST_FOCUS_INIT)=126,
    MKB(SMD_TEST_FOCUS_MAIN)=127,
    MKB(SMD_TEST_INDIRECT_INIT)=128,
    MKB(SMD_TEST_INDIRECT_MAIN)=129,
    MKB(SMD_TEST_NLOBJPUT_INIT)=130,
    MKB(SMD_TEST_NLOBJPUT_MAIN)=131,
    MKB(SMD_TEST_BITMAP_INIT)=132,
    MKB(SMD_TEST_BITMAP_MAIN)=133,
    MKB(SMD_TEST_FONT_INIT)=134,
    MKB(SMD_TEST_FONT_MAIN)=135,
    MKB(SMD_TEST_REPLAY_INIT)=136,
    MKB(SMD_TEST_REPLAY_MAIN)=137,
    MKB(SMD_TEST_MOTION_INIT)=138,
    MKB(SMD_TEST_MOTION_MAIN)=139,
    MKB(SMD_TEST_ADVREPLAY_INIT)=140,
    MKB(SMD_TEST_MODEL_INIT)=141,
    MKB(SMD_TEST_MODEL_MAIN)=142,
    MKB(SMD_TEST_NEWMODEL_INIT)=143,
    MKB(SMD_TEST_NEWMODEL_MAIN)=144,
    MKB(SMD_TEST_PATTERN_INIT)=145,
    MKB(SMD_TEST_PATTERN_MAIN)=146,
    MKB(SMD_TEST_ROMFONT_INIT)=147,
    MKB(SMD_TEST_ROMFONT_MAIN)=148,
    MKB(SMD_TEST_PREREND_INIT)=149,
    MKB(SMD_TEST_PREREND_MAIN)=150,
    MKB(SMD_TEST_PREVIEW_INIT)=151,
    MKB(SMD_TEST_PREVIEW_MAIN)=152,
    MKB(SMD_TEST_ADX_INIT)=153,
    MKB(SMD_TEST_ADX_MAIN)=154,
    MKB(SMD_TEST_NEWMOTION_INIT)=155,
    MKB(SMD_TEST_NEWMOTION_MAIN)=156,
    MKB(SMD_TEST_ARAM_FONT_INIT)=157,
    MKB(SMD_TEST_ARAM_FONT_MAIN)=158,
    MKB(SMD_TEST_SCENE_PLAY_INIT)=159,
    MKB(SMD_TEST_SCENE_PLAY_MAIN)=160,
    MKB(SMD_TEST_DIALOG_INIT)=161,
    MKB(SMD_TEST_DIALOG_MAIN)=162,
    MKB(SMD_TEST_DIFFICULTY_INIT)=163,
    MKB(SMD_TEST_DIFFICULTY_MAIN)=164,
    MKB(SMD_TEST_CHK_REPSIZE_INIT)=165,
    MKB(SMD_TEST_CHK_REPSIZE_MAIN)=166,
    MKB(SMD_TEST_BOTTOM)=167,
    MKB(SMD_MINI_TOP)=168,
    MKB(SMD_MINI_SELECT_INIT)=169,
    MKB(SMD_MINI_SELECT_MAIN)=170,
    MKB(SMD_MINI_BOWLING_INIT)=171,
    MKB(SMD_MINI_BOWLING_MAIN)=172,
    MKB(SMD_MINI_BILLIARDS_INIT)=173,
    MKB(SMD_MINI_BILLIARDS_MAIN)=174,
    MKB(SMD_MINI_SHOOTING_INIT)=175,
    MKB(SMD_MINI_SHOOTING_MAIN)=176,
    MKB(SMD_MINI_REL_SAMPLE_INIT)=177,
    MKB(SMD_MINI_REL_SAMPLE_MAIN)=178,
    MKB(SMD_MINI_COMMEND_INIT)=179,
    MKB(SMD_MINI_COMMEND_MAIN)=180,
    MKB(SMD_MINI_RANKING_INIT)=181,
    MKB(SMD_MINI_RANKING_MAIN)=182,
    MKB(SMD_MINI_ENDING_INIT)=183,
    MKB(SMD_MINI_ENDING_MAIN)=184,
    MKB(SMD_MINI_RACE2_INIT)=185,
    MKB(SMD_MINI_RACE2_MAIN)=186,
    MKB(SMD_MINI_GOLF2_INIT)=187,
    MKB(SMD_MINI_GOLF2_MAIN)=188,
    MKB(SMD_MINI_FIGHT2_INIT)=189,
    MKB(SMD_MINI_FIGHT2_MAIN)=190,
    MKB(SMD_MINI_PILOT2_INIT)=191,
    MKB(SMD_MINI_PILOT2_MAIN)=192,
    MKB(SMD_MINI_BOAT_INIT)=193,
    MKB(SMD_MINI_BOAT_MAIN)=194,
    MKB(SMD_MINI_S_ROLL_INIT)=195,
    MKB(SMD_MINI_S_ROLL_MAIN)=196,
    MKB(SMD_MINI_FUTSAL_INIT)=197,
    MKB(SMD_MINI_FUTSAL_MAIN)=198,
    MKB(SMD_MINI_DOGFIGHT_INIT)=199,
    MKB(SMD_MINI_DOGFIGHT_MAIN)=200,
    MKB(SMD_MINI_BASEBALL_INIT)=201,
    MKB(SMD_MINI_BASEBALL_MAIN)=202,
    MKB(SMD_MINI_TENNIS_INIT)=203,
    MKB(SMD_MINI_TENNIS_MAIN)=204,
    MKB(SMD_MINI_DMY9)=205,
    MKB(SMD_MINI_DMY10)=206,
    MKB(SMD_MINI_DMY11)=207,
    MKB(SMD_MINI_DMY12)=208,
    MKB(SMD_MINI_BOTTOM)=209,
    MKB(SMD_OPTION_TOP)=210,
    MKB(SMD_OPTION_SELECT_INIT)=211,
    MKB(SMD_OPTION_SELECT_MAIN)=212,
    MKB(SMD_OPTION_SOUND_INIT)=213,
    MKB(SMD_OPTION_SOUND_MAIN)=214,
    MKB(SMD_OPTION_PAD_INIT)=215,
    MKB(SMD_OPTION_PAD_MAIN)=216,
    MKB(SMD_OPTION_MEMCARD_INIT)=217,
    MKB(SMD_OPTION_MEMCARD_MAIN)=218,
    MKB(SMD_OPTION_POINT_INIT)=219,
    MKB(SMD_OPTION_POINT_MAIN)=220,
    MKB(SMD_OPTION_RANKING_INIT)=221,
    MKB(SMD_OPTION_RANKING_MAIN)=222,
    MKB(SMD_OPTION_AUTOSAVE_INIT)=223,
    MKB(SMD_OPTION_AUTOSAVE_MAIN)=224,
    MKB(SMD_OPTION_REPLAY_INIT)=225,
    MKB(SMD_OPTION_REPLAY_MAIN)=226,
    MKB(SMD_OPTION_SEPARATE_INIT)=227,
    MKB(SMD_OPTION_SEPARATE_MAIN)=228,
    MKB(SMD_OPTION_REPLAY_PLAY_INIT)=229,
    MKB(SMD_OPTION_REPLAY_PLAY_MAIN)=230,
    MKB(SMD_OPTION_PAD_MENU_INIT)=231,
    MKB(SMD_OPTION_PAD_MENU_MAIN)=232,
    MKB(SMD_OPTION_PAD_VIB_INIT)=233,
    MKB(SMD_OPTION_PAD_VIB_MAIN)=234,
    MKB(SMD_OPTION_DMY0)=235,
    MKB(SMD_OPTION_DMY1)=236,
    MKB(SMD_OPTION_DMY2)=237,
    MKB(SMD_OPTION_DMY3)=238,
    MKB(SMD_OPTION_DMY4)=239,
    MKB(SMD_OPTION_DMY5)=240,
    MKB(SMD_OPTION_DMY6)=241,
    MKB(SMD_OPTION_DMY7)=242,
    MKB(SMD_OPTION_BOTTOM)=243,
    MKB(SMD_AUTHOR_TOP)=244,
    MKB(SMD_AUTHOR_SELECT_INIT)=245,
    MKB(SMD_AUTHOR_SELECT_MAIN)=246,
    MKB(SMD_AUTHOR_PLAY_INIT)=247,
    MKB(SMD_AUTHOR_PLAY_MAIN)=248,
    MKB(SMD_AUTHOR_PLAY_STORY_INIT)=249,
    MKB(SMD_AUTHOR_PLAY_FROM_SEL)=250,
    MKB(SMD_AUTHOR_PLAY_RET_SEL)=251,
    MKB(SMD_AUTHOR_PLAY_ENDING_INIT)=252,
    MKB(SMD_AUTHOR_DMY6)=253,
    MKB(SMD_AUTHOR_DMY7)=254,
    MKB(SMD_AUTHOR_BOTTOM)=255,
    MKB(SMD_EXOPT_TOP)=256,
    MKB(SMD_EXOPT_REPLAY_LOAD_INIT)=257,
    MKB(SMD_EXOPT_REPLAY_LOAD_MAIN)=258,
    MKB(SMD_EXOPT_REPLAY_INIT)=259,
    MKB(SMD_EXOPT_REPLAY_MAIN)=260,
    MKB(SMD_EXOPT_DMY5)=261,
    MKB(SMD_EXOPT_DMY6)=262,
    MKB(SMD_EXOPT_DMY7)=263,
    MKB(SMD_EXOPT_BOTTOM)=264,
    MKB(SMD_INVALID)=4294967295
};
typedef undefined4 MKB(SubMode);

enum { /* I made MD_INVALID, it's `-1` -Crafted */
    MKB(MD_ADV)=0,
    MKB(MD_SEL)=1,
    MKB(MD_GAME)=2,
    MKB(MD_TEST)=3,
    MKB(MD_MINI)=4,
    MKB(MD_OPTION)=5,
    MKB(MD_AUTHOR)=6,
    MKB(MD_EXOPT)=7,
    MKB(MD_INVALID)=4294967295
};
typedef undefined4 MKB(MainMode);

enum { /* I added DIP_NONE -Crafted */
    MKB(DIP_NONE)=0,
    MKB(DIP_DEBUG)=1,
    MKB(DIP_DISP)=2,
    MKB(DIP_STCOLI)=4,
    MKB(DIP_TRIANGLE)=8,
    MKB(DIP_TAIKEN)=16,
    MKB(DIP_TIME_STOP)=32,
    MKB(DIP_NAMEENTRY)=64,
    MKB(DIP_FIX_WORLD)=128,
    MKB(DIP_TEST_CAM)=256,
    MKB(DIP_NO_INTR)=512,
    MKB(DIP_CAPTURE)=1024,
    MKB(DIP_PERF_ALWAYS)=2048,
    MKB(DIP_PLAY_STG_ALL)=4096,
    MKB(DIP_PLAY_PNT_x10)=8192,
    MKB(DIP_SARU_0)=16384,
    MKB(DIP_SWITCH15)=32768,
    MKB(DIP_ONLY24MB)=65536,
    MKB(DIP_SHADOW_DISP)=131072,
    MKB(DIP_SPRITE_OFF)=262144,
    MKB(DIP_SNDREQ_DISP)=524288,
    MKB(DIP_SE_VOL_RESET)=1048576,
    MKB(DIP_APE_FACEDIR)=2097152,
    MKB(DIP_SEL_ALL_CTRL)=4194304,
    MKB(DIP_APE_NUMBER)=8388608,
    MKB(DIP_1P_TEST_CAM)=16777216,
    MKB(DIP_PREVIEW_SAVE)=33554432,
    MKB(DIP_AUTHOR_DEBUG)=67108864,
    MKB(DIP_SWITCH27)=134217728,
    MKB(DIP_SOA)=268435456,
    MKB(DIP_TEST_TEAM)=536870912,
    MKB(DIP_NO_MINIMAP)=1073741824,
    MKB(DIP_NO_STAGE)=2147483648
};
typedef undefined4 MKB(DipSwitch);

typedef struct MKB(Replay) MKB(Replay), *MKB(PReplay);

struct MKB(Replay) { /* Unknown size atm */
    MKB(undefined) field_0x0[0x4];
    MKB(u8) difficulty;
    MKB(u8) floorNum;
    MKB(u8) monkey;
    MKB(undefined) field_0x7[0x9];
    MKB(u32) score;
    MKB(undefined) field_0x14[0x4];
    MKB(s16) g_stageTimeLimit1;
    MKB(s16) g_duration;
    MKB(undefined) field_0x1c[0x2];
    MKB(s16) goalId;
    MKB(undefined) field_0x20[0x4];
    struct MKB(Vec) g_ballVelAtGoal;
    MKB(undefined) field_0x30[0x4];
    MKB(u32) size;
    MKB(s16) g_currFrame;
    MKB(s16) g_stageTimeLimit2;
    struct MKB(Vec) initBallPos;
    MKB(u8) * ballDeltaXLo;
    MKB(u8) * ballDeltaXHi;
    MKB(u8) * ballDeltaYLo;
    MKB(u8) * ballDeltaYHi;
    MKB(u8) * ballDeltaZLo;
    MKB(u8) * ballDeltaZHi;
    MKB(u8) * ballRotXLo;
    MKB(u8) * ballRotXHi;
    MKB(u8) * ballRotYLo;
    MKB(u8) * ballRotYHi;
    MKB(u8) * ballRotZLo;
    MKB(u8) * ballRotZHi;
    MKB(u8) * sparkDirX;
    MKB(u8) * sparkDirY;
    MKB(u8) * sparkDirZ;
    MKB(u8) * sparkIntensity;
    MKB(u8) * coliFlags0;
    MKB(u8) * coliFlags1;
    MKB(u8) * coliFlags2;
    MKB(u8) * coliFlags3;
    MKB(u8) * stageTiltXLo;
    MKB(u8) * stageTiltXHi;
    MKB(u8) * stageTiltZLo;
    MKB(u8) * stageTiltZHi;
    MKB(u8) field63_0xa8[4];
    MKB(s16) playableItemgroupCount;
    MKB(s16) seesawCount;
    MKB(u8) * playableItemgroupAnimFrames;
    MKB(u8) * seesawState;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Replay)) == 0xb8);

typedef struct MKB(Ape) MKB(Ape), *MKB(PApe);

typedef struct MKB(SomeApeState) MKB(SomeApeState), *MKB(PSomeApeState);

typedef struct MKB(Mal) MKB(Mal), *MKB(PMal);

typedef struct MKB(ApeArchive) MKB(ApeArchive), *MKB(PApeArchive);

typedef struct MKB(Component) MKB(Component), *MKB(PComponent);

typedef struct MKB(GComponentDefList) MKB(GComponentDefList), *MKB(PGComponentDefList);

typedef struct MKB(GmaBuffer) MKB(GmaBuffer), *MKB(PGmaBuffer);

enum {
    MKB(SCENE_COMMON)=0,
    MKB(SCENE_MAIN)=1,
    MKB(SCENE_DUMMY1)=2,
    MKB(SCENE_DUMMY2)=3,
    MKB(SCENE_DUMMY3)=4,
    MKB(SCENE_DUMMY4)=5,
    MKB(SCENE_DUMMY5)=6,
    MKB(SCENE_DUMMY6)=7,
    MKB(SCENE_DUMMY7)=8,
    MKB(SCENE_DUMMY8)=9,
    MKB(SCENE_DUMMY9)=10,
    MKB(SCENE_DUMMY10)=11,
    MKB(SCENE_RACE)=12,
    MKB(SCENE_FGT)=13,
    MKB(SCENE_PILOT)=14,
    MKB(SCENE_GOLF)=15,
    MKB(SCENE_BOWL)=16,
    MKB(SCENE_BILL)=17,
    MKB(SCENE_BOAT)=18,
    MKB(SCENE_DOGF)=19,
    MKB(SCENE_TENNIS)=20,
    MKB(SCENE_BBALL)=21,
    MKB(SCENE_GUNS)=22,
    MKB(SCENE_ADV)=23,
    MKB(SCENE_SEL)=24,
    MKB(SCENE_COMMEND)=25,
    MKB(SCENE_SCENE1A)=26,
    MKB(SCENE_SCENE1B)=27,
    MKB(SCENE_SCENE2)=28,
    MKB(SCENE_SCENE3)=29,
    MKB(SCENE_SCENE4)=30,
    MKB(SCENE_SCENE5)=31,
    MKB(SCENE_SCENE6)=32,
    MKB(SCENE_SCENE7)=33,
    MKB(SCENE_SCENE8)=34,
    MKB(SCENE_SCENE9)=35,
    MKB(SCENE_SCENE10)=36,
    MKB(SCENE_LASTSCENE)=37,
    MKB(SCENE_ENDING1)=38,
    MKB(SCENE_ENDING2)=39,
    MKB(SCENE_ENDING3)=40,
    MKB(SCENE_ENDING4)=41
};
typedef undefined2 MKB(SceneId);

enum { /* Whether ape models are skinning or stitching? */
    MKB(DEFORM_SKINNING)=0,
    MKB(DEFORM_STITCHING)=1
};
typedef undefined1 MKB(DeformType);

enum {
    MKB(FACE_BASE)=0,
    MKB(FACE_CLOSEEYE)=1,
    MKB(FACE_SMILE)=2,
    MKB(FACE_PISTOFF)=3,
    MKB(FACE_SOB)=4,
    MKB(FACE_SURPRISE)=5,
    MKB(FACE_TALK)=6,
    MKB(FACE_SMILETALK)=7,
    MKB(FACE_PISTOFFTALK)=8,
    MKB(FACE_SOBTALK)=9
};
typedef undefined2 MKB(ApeFace);

enum {
    MKB(G_APE_RENDER_TRANSPARENT)=1048576,
    MKB(G_APE_RENDER_LOWER_Y)=16777216,
    MKB(G_APE_RENDER_UNCULLED)=67108864
};
typedef undefined4 MKB(ApeRenderFlag);

typedef struct MKB(Quat) MKB(Quat), *MKB(PQuat);

enum {
    MKB(LOD_HIGH)=0,
    MKB(LOD_MIDDLE)=1,
    MKB(LOD_LOW)=2,
    MKB(LOD_LOWEST)=3
};
typedef undefined4 MKB(ApeLOD);

typedef struct MKB(ArcFileInfo) MKB(ArcFileInfo), *MKB(PArcFileInfo);

typedef struct MKB(SkeletonState) MKB(SkeletonState), *MKB(PSkeletonState);

typedef struct MKB(SkeletonDef) MKB(SkeletonDef), *MKB(PSkeletonDef);

typedef struct MKB(GComponentDef) MKB(GComponentDef), *MKB(PGComponentDef);

typedef struct MKB(GmaModelEntry) MKB(GmaModelEntry), *MKB(PGmaModelEntry);

typedef struct MKB(ARCHandle) MKB(ARCHandle), *MKB(PARCHandle);

typedef struct MKB(BoneState) MKB(BoneState), *MKB(PBoneState);

typedef struct MKB(GSkeletonStateSubA) MKB(GSkeletonStateSubA), *MKB(PGSkeletonStateSubA);

typedef struct MKB(BoneKeyframe) MKB(BoneKeyframe), *MKB(PBoneKeyframe);

struct MKB(SomeApeState) {
    MKB(u8) g_chara_idx;
    MKB(undefined) field_0x1[0x7];
    void * g_buf5;
    MKB(undefined) field_0xc[0x10];
    struct MKB(ArcFileInfo) * g_arc;
    MKB(undefined) field_0x20[0x8];
    void * g_buf3;
    void * g_buf1;
    void * g_buf2;
    void * g_buf4;
    MKB(undefined) field_0x38[0x28];
} __attribute__((__packed__));
static_assert(sizeof(MKB(SomeApeState)) == 0x60);

struct MKB(ApeArchive) { /* Extremely simple archive format used for packing Ape skeleton/animation files into a single file. List of pointers/offsets to objects immediately follows/ */
    MKB(u32) count; /* Number of objects in the archive (e.g. skeletons for .ska) */
    char * name_table; /* Pointer/offset to packed object name string list */
} __attribute__((__packed__));
static_assert(sizeof(MKB(ApeArchive)) == 0x8);

struct MKB(ArcFileInfo) {
    struct MKB(ARCHandle) * Handle;
    int startoffset;
    int length;
} __attribute__((__packed__));
static_assert(sizeof(MKB(ArcFileInfo)) == 0xc);

struct MKB(GComponentDefList) {
    struct MKB(GComponentDef) * list;
    MKB(u32) count;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GComponentDefList)) == 0x8);

struct MKB(GmaBuffer) { /* Represents the first 32 bytes of buffer allocated for loaded GMA files. The first 32 bytes are extra; not part of the original GMA file */
    MKB(s32) model_count;
    void * model_list_ptr; /* Pointer to the first model (after GMA header) */
    struct MKB(GmaModelEntry) * model_entries;
    dword model_names_ptr;
    dword g_initially_zero;
    MKB(undefined) field_0x14[0xc];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GmaBuffer)) == 0x20);

struct MKB(BoneKeyframe) {
    MKB(undefined) field_0x0[0x8];
} __attribute__((__packed__));
static_assert(sizeof(MKB(BoneKeyframe)) == 0x8);

struct MKB(GSkeletonStateSubA) {
    short field0_0x0;
    short field1_0x2;
    short g_anim_duration;
    short field3_0x6;
    MKB(undefined) field_0x8[0x4];
    float field8_0xc;
    float field9_0x10;
    short field10_0x14;
    MKB(undefined) field_0x16[0x2];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GSkeletonStateSubA)) == 0x18);

struct MKB(SkeletonState) { /* Bone state array is allocated after this */
    MKB(u16) bone_count;
    MKB(u16) flags;
    float field2_0x4;
    struct MKB(BoneState) * bone_states;
    float field4_0xc;
    float field5_0x10;
    float field6_0x14;
    undefined4 field7_0x18;
    undefined4 field8_0x1c;
    undefined4 field9_0x20;
    struct MKB(ApeArchive) * mta;
    undefined4 field11_0x28;
    undefined4 field12_0x2c;
    undefined4 field13_0x30;
    undefined4 field14_0x34;
    undefined4 field15_0x38;
    void * field16_0x3c;
    struct MKB(GSkeletonStateSubA) g_skeleton_state_subA_1;
    struct MKB(GSkeletonStateSubA) g_skeleton_state_subA_2;
} __attribute__((__packed__));
static_assert(sizeof(MKB(SkeletonState)) == 0x70);

struct MKB(GComponentDef) {
    MKB(undefined) field_0x0[0x8];
    char * g_name;
    MKB(undefined) field_0xc[0x1c];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GComponentDef)) == 0x28);

struct MKB(Quat) {
    MKB(f32) x;
    MKB(f32) y;
    MKB(f32) z;
    MKB(f32) w;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Quat)) == 0x10);

struct MKB(ARCHandle) { /* I don't actually know the struct contents in the slightest */
    void * arc_data;
    void * g_first_node_offset;
    MKB(s32) c;
    MKB(s32) d;
    MKB(s32) e;
    MKB(s32) f;
    MKB(s32) g;
} __attribute__((__packed__));
static_assert(sizeof(MKB(ARCHandle)) == 0x1c);

struct MKB(SkeletonDef) { /* Bone definitions immediately follow */
    short bone_count;
    undefined2 padding;
} __attribute__((__packed__));
static_assert(sizeof(MKB(SkeletonDef)) == 0x4);

struct MKB(Component) { /* Subcomponent of an Ape, like body, face, hands, eyes */
    struct MKB(SkeletonState) * skeleton_state;
    struct MKB(SkeletonDef) * skeleton_def;
    struct MKB(ApeArchive) * mta; /* Animation definitions */
} __attribute__((__packed__));
static_assert(sizeof(MKB(Component)) == 0xc);

struct MKB(Ape) {
    struct MKB(SomeApeState) * g_some_ape_state;
    struct MKB(Mal) * common_mal;
    void * game_mal;
    void * field3_0xc;
    struct MKB(ApeArchive) * g_skel_ska; /* Pointer to contents of "skel.ska", which has one skeleton per character */
    void * field5_0x14;
    struct MKB(Component) g_face_component;
    struct MKB(Component) g_handr_component;
    struct MKB(Component) g_handl_component;
    struct MKB(Component) g_tail_component;
    struct MKB(GComponentDefList) * g_component_defs;
    struct MKB(GmaBuffer) * gma;
    struct MKB(TplBuffer) * tpl;
    undefined2 field13_0x54;
    MKB(undefined) field_0x56[0x2];
    undefined2 field16_0x58;
    MKB(undefined) field_0x5a[0x2];
    void * anim_func; /* Created by retype action */
    float float_0x60;
    MKB(undefined) field_0x64[0x10];
    MKB(SceneId)  scene_id; /* Created by retype action */
    MKB(DeformType)  g_deform_type;
    MKB(undefined) field_0x77[0x1];
    int g_smth_with_scene; /* Created by retype action */
    undefined4 field41_0x7c;
    undefined2 field42_0x80;
    MKB(undefined) field_0x82[0x2];
    short field45_0x84;
    MKB(u8) chara_anim_type; /* Some value that changes the type of animation the character is doing. From standing still, to walking, to the "I lost" state, etc */
    byte g_anim_step;
    int g_next_ape_idx; /* Something to do with character spinning post-goal */
    ushort g_handr_short;
    ushort g_handl_short;
    MKB(ApeFace)  face;
    MKB(undefined) field_0x92[0x6];
    float g_handl_float;
    float g_handr_float;
    ushort some_flag;
    ushort some_flag_1;
    MKB(undefined) field_0xa4[0x4];
    int g_some_length1;
    int field67_0xac;
    float field68_0xb0;
    undefined4 field69_0xb4;
    undefined4 field70_0xb8;
    undefined4 field71_0xbc;
    int field72_0xc0;
    struct MKB(GmaModel) * g_component_models[40];
    struct MKB(GmaModel) * g_eye_models1[15]; /* Not unique models, repeated! Blink pattern? */
    struct MKB(GmaModel) * g_eye_models2[15]; /* Same underlying models as above, but blink pattern may differ between characters?!? */
    struct MKB(GmaModel) * face_models[10];
    struct MKB(GmaModel) * handl_fist_model;
    struct MKB(GmaModel) * handl_flat_model;
    struct MKB(GmaModel) * handr_fist_model;
    struct MKB(GmaModel) * handr_flat_model;
    float float_0x214;
    short field82_0x218;
    MKB(undefined) field_0x21a[0x2];
    char flag2;
    MKB(undefined) field_0x21d[0x3];
    uint flag3;
    uint flag4;
    MKB(undefined) field_0x228[0x14];
    undefined4 field111_0x23c;
    undefined4 field112_0x240;
    MKB(ApeCharacter)  chara;
    MKB(ApeRenderFlag)  g_render_flags;
    undefined4 field115_0x24c;
    undefined4 field116_0x250;
    undefined4 field117_0x254;
    struct MKB(Vec) pos;
    struct MKB(Vec) some_vec3;
    undefined4 field120_0x270;
    undefined4 field121_0x274;
    undefined4 field122_0x278;
    undefined4 field123_0x27c;
    float scale;
    MKB(undefined) field_0x284[0x4];
    struct MKB(Quat) ape_rotation;
    int ape_idx;
    undefined4 field131_0x29c;
    MKB(ApeLOD)  lod;
    undefined4 field133_0x2a4;
    float g_smth_w_velocity1;
    float g_smth_w_velocity2;
    float g_smth_w_velocity3;
    float g_smth_w_velocity4;
    undefined4 field138_0x2b8;
    int color_index;
    MKB(undefined) field_0x2c0[0x8];
    char ball_idx; /* Created by retype action */
    byte camera_visible_bitmask;
    MKB(undefined) field_0x2ca[0x2];
    float field152_0x2cc;
    float field153_0x2d0;
    float field154_0x2d4;
    float field155_0x2d8;
    float field156_0x2dc;
    float field157_0x2e0;
    float field158_0x2e4;
    float field159_0x2e8;
    float field160_0x2ec;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Ape)) == 0x2f0);

struct MKB(BoneState) {
    char * bone_name;
    short flags;
    short parent_id;
    struct MKB(Vec) translation;
    float translation_distance; /* Created by retype action */
    struct MKB(Vec) g_translation_copy;
    float g_float1;
    MKB(Mtx) g_rotation_mtx1;
    MKB(Mtx) g_rotation_mtx2; /* Created by retype action */
    MKB(Mtx) mtx; /* Created by retype action */
    struct MKB(Vec) g_vec; /* Created by retype action */
    MKB(undefined) field_0xc4[0xc];
    undefined4 field23_0xd0;
    short g_keyframe_a_count[3];
    MKB(undefined) field_0xda[0x6];
    short g_keyframe_b_count[3];
    MKB(undefined) field_0xe6[0x6];
    short g_keyframe_c_count[3];
    MKB(undefined) field_0xf2[0x6];
    short g_keyframe_a_unk[3];
    MKB(undefined) field_0xfe[0x6];
    short g_keyframe_b_unk[3];
    MKB(undefined) field_0x10a[0x6];
    short g_keyframe_c_unk[3];
    MKB(undefined) field_0x116[0x6];
    struct MKB(BoneKeyframe) * g_keyframes_a[3];
    MKB(undefined) field_0x128[0xc];
    struct MKB(BoneKeyframe) * g_keyframes_b[3];
    MKB(undefined) field_0x140[0xc];
    struct MKB(BoneKeyframe) * g_keyframes_c[3];
    MKB(undefined) field_0x158[0x1c];
} __attribute__((__packed__));
static_assert(sizeof(MKB(BoneState)) == 0x174);

struct MKB(Mal) { /* Structure of common_mal ape animation files? Header size unknown atm */
    MKB(undefined) field_0x0[0x21];
    MKB(u8) g_some_count;
    MKB(undefined) field_0x22[0xe];
} __attribute__((__packed__));
static_assert(sizeof(MKB(Mal)) == 0x30);

struct MKB(GmaModelEntry) {
    struct MKB(GmaModel) * model;
    char * name;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GmaModelEntry)) == 0x8);

typedef struct MKB(Camera) MKB(Camera), *MKB(PCamera);

typedef struct MKB(Vec2d) MKB(Vec2d), *MKB(PVec2d);

struct MKB(Vec2d) {
    float x;
    float y;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Vec2d)) == 0x8);

struct MKB(Camera) {
    struct MKB(Vec) pos; /* Position of the camera */
    struct MKB(Vec) pivot; /* Called 'intr' in the debug menu. The point which the camera looks at - close to the monkey in normal play, but can also be modified with the C-stick in test camera. Camera pose is actually controled by pos and rot - pivot is an intermediate value */
    struct MKB(S16Vec) rot; /* Rotation of the camera. Called 'ang' in the debug menu */
    MKB(u8) mode; /* One byte representing camera mode (1 (dec) = SMB 1 style camera, 75 (dec) = SMB 2 style camera, and there's a lot more (everything from 1 to about 105 - a few are duplicates. Each value  corresponds to a different function in the camera function table */
    MKB(u8) submode; /* Called 'SUB' in the debug menu. Not sure of the purpose */
    float g_some_float;
    MKB(u16) g_some_bitflags;
    MKB(u8) g_some_flag; /* Set to '2' in a fallout state, set to 1 during spin-in */
    MKB(u8) g_some_flag2;
    float g_some_float2;
    float g_some_float3;
    MKB(s16) fov; /* Field of view of the camera, called 'pers' in the debug menu */
    MKB(s16) next_fov; /* Next field of view - when the camera FOV changes through some event (goal replay, new level start), this value is taken and used for the FOV */
    float aspect; /* Aspect ratio of the camera */
    float fov_tangent; /* Tangent of (fov/32768)*(pi/2) */
    float fov_cotangent; /* Cotangent of (fov/32768)*(pi/2) */
    float near; /* Near clip plane distance */
    float far; /* Far clip plane distance */
    struct MKB(Vec2d) viewport_pos;
    struct MKB(Vec2d) viewport_size;
    MKB(u16) g_some_counter1;
    MKB(u16) g_some_short;
    struct MKB(Vec) g_initial_pivot; /* The pivot is set to this point at the beginning of the spin-in sequence, and approaches g_dest_intr */
    float g_spinin_value_1; /* Affects something with camera spin-in */
    float g_spinin_value_2; /* Affects something with camera spin-in */
    undefined2 field25_0x70;
    undefined2 field26_0x72;
    undefined2 field27_0x74;
    undefined2 field28_0x76;
    MKB(undefined) field_0x78[0x4];
    struct MKB(Vec) g_final_pivot; /* The pivot moves towards this point, and reaches it at the end of the spin-in sequence */
    MKB(undefined) field_0x88[0x8];
    undefined2 field42_0x90;
    undefined2 field43_0x92;
    MKB(undefined) field_0x94[0x8];
    struct MKB(Vec) vel;
    struct MKB(Vec) pivot_vel;
    struct MKB(Vec) g_pos_desire; /* Something to do with camera rotation interpolation? */
    MKB(s16) g_pivot_x_rot;
    MKB(undefined) field_0xc2[0x52];
    MKB(s16) g_y_rot_vel;
    MKB(undefined) field_0x116[0x2];
    undefined1 g_some_goal_idx; /* Created by retype action */
    MKB(undefined) field_0x119[0x1b];
    struct MKB(Vec) g_some_vec5;
    struct MKB(Vec) g_some_vec6;
    MKB(Mtx) g_some_mtx1;
    MKB(Mtx) g_some_mtx2;
    MKB(Mtx) g_some_mtx3;
    MKB(Mtx) g_some_mtx4;
    MKB(u32) idx;
    MKB(undefined) field_0x210[0x7c];
} __attribute__((__packed__));
static_assert(sizeof(MKB(Camera)) == 0x28c);

typedef struct MKB(Rect) MKB(Rect), *MKB(PRect);

struct MKB(Rect) {
    struct MKB(Vec) pos;
    struct MKB(S16Vec) rot;
    MKB(undefined) field_0x12[0xe];
    float width;
    float height;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Rect)) == 0x28);

typedef struct MKB(OrdTblNode) MKB(OrdTblNode), *MKB(POrdTblNode);

struct MKB(OrdTblNode) {
    struct MKB(OrdTblNode) * next;
    void * draw_func;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OrdTblNode)) == 0x8);

enum {
    MKB(CHALLENGE_MODE)=0,
    MKB(COMPETITION_MODE)=1,
    MKB(PRACTICE_MODE)=2,
    MKB(MONKEY_RACE)=3,
    MKB(MONKEY_FIGHT)=4,
    MKB(MONKEY_TARGET)=5,
    MKB(MONKEY_BILLIARDS)=6,
    MKB(MONKEY_BOWLING)=7,
    MKB(MONKEY_GOLF)=8,
    MKB(MONKEY_SHOT)=9,
    MKB(MONKEY_BOAT)=10,
    MKB(G_SPAWN_OPPOSITE_OF_STARTPOS)=11,
    MKB(MONKEY_SOCCER)=12,
    MKB(MONKEY_DOGFIGHT)=13,
    MKB(STORY_MODE)=14,
    MKB(MONKEY_BASEBALL)=15,
    MKB(MONKEY_TENNIS)=16
};
typedef undefined4 MKB(MainGameMode);

typedef struct MKB(Sprite) MKB(Sprite), *MKB(PSprite);

enum { /* How to render the sprite if a disp() function is not provided */
    MKB(SPRT_TEXT)=0,
    MKB(SPRT_BMP)=1,
    MKB(SPRT_TEXTURE)=2
};
typedef undefined1 MKB(SpriteType);

enum {
    MKB(FONT_ASCII)=0,
    MKB(FONT_ASC_8x16)=1,
    MKB(FONT_ASC_12x12)=2,
    MKB(FONT_ASC_24x24)=3,
    MKB(FONT_ASC_16x16P)=4,
    MKB(FONT_ASC_16x16)=5,
    MKB(FONT_DMY03)=6,
    MKB(FONT_DMY04)=7,
    MKB(FONT_DMY05)=8,
    MKB(FONT_ASC_72x64)=9,
    MKB(FONT_DMY06)=10,
    MKB(FONT_DMY07)=11,
    MKB(FONT_DMY08)=12,
    MKB(FONT_DMY09)=13,
    MKB(FONT_DMY10)=14,
    MKB(FONT_DMY11)=15,
    MKB(FONT_DMY12)=16,
    MKB(FONT_DMY13)=17,
    MKB(FONT_DMY14)=18,
    MKB(FONT_DMY15)=19,
    MKB(FONT_DMY16)=20,
    MKB(FONT_ICON_SD)=21,
    MKB(FONT_ICON_SD2)=22,
    MKB(FONT_DMY20)=23,
    MKB(FONT_DMY21)=24,
    MKB(FONT_DMY22)=25,
    MKB(FONT_DMY23)=26,
    MKB(FONT_DMY_RNK00)=27,
    MKB(FONT_DMY_RNK01)=28,
    MKB(FONT_DMY_RNK02)=29,
    MKB(FONT_DMY_RNK03)=30,
    MKB(FONT_DMY_RNK04)=31,
    MKB(FONT_RNK_32x32)=32,
    MKB(FONT_RNK_NUM)=33,
    MKB(FONT_MINI_RNK)=34,
    MKB(FONT_SCORE_NUM)=35,
    MKB(FONT_DMY_RNK0)=36,
    MKB(FONT_DMY_RNK1)=37,
    MKB(FONT_DMY_RNK2)=38,
    MKB(FONT_DMY_RNK3)=39,
    MKB(FONT_DMY_RNK4)=40,
    MKB(FONT_DMY_RNK5)=41,
    MKB(FONT_DMY_RNK6)=42,
    MKB(FONT_DMY_RNK7)=43,
    MKB(FONT_DMY_RNK8)=44,
    MKB(FONT_DMY_RNK9)=45,
    MKB(FONT_DMY_RNKa)=46,
    MKB(FONT_DMY_RNKb)=47,
    MKB(FONT_DMY_RNKc)=48,
    MKB(FONT_DMY_RNKd)=49,
    MKB(FONT_DMY_RNKe)=50,
    MKB(FONT_DMY_RNKf)=51,
    MKB(FONT_SEL_CTRL_PORT)=52,
    MKB(FONT_SEL_CTRL_R)=53,
    MKB(FONT_SEL_CTRL_B)=54,
    MKB(FONT_SEL_CTRL_Y)=55,
    MKB(FONT_SEL_CTRL_G)=56,
    MKB(FONT_SEL_CTRL_SDW)=57,
    MKB(FONT_DMY42)=58,
    MKB(FONT_DMY43)=59,
    MKB(FONT_BWL_SCORE)=60,
    MKB(FONT_DMY50)=61,
    MKB(FONT_DMY51)=62,
    MKB(FONT_DMY52)=63,
    MKB(FONT_DMY53)=64,
    MKB(FONT_DMY54)=65,
    MKB(FONT_DMY55)=66,
    MKB(FONT_DMY56)=67,
    MKB(FONT_DMY57)=68,
    MKB(FONT_RAC_DMY01)=69,
    MKB(FONT_RAC_PLAYER)=70,
    MKB(FONT_RAC_RANK)=71,
    MKB(FONT_RAC_LAP_MARK_NUM)=72,
    MKB(FONT_RAC_LAP_NUM)=73,
    MKB(FONT_RAC_TIME_NUM)=74,
    MKB(FONT_RAC_LAP_MARK_NUM_S)=75,
    MKB(FONT_RAC_DMY08)=76,
    MKB(FONT_RAC_DMY09)=77,
    MKB(FONT_RAC_DMY10)=78,
    MKB(FONT_RAC_SPD_NUM)=79,
    MKB(FONT_RAC_DMY12)=80,
    MKB(FONT_RAC_DMY13)=81,
    MKB(FONT_TGT_PLAYER)=82,
    MKB(FONT_TGT_MSCORE)=83,
    MKB(FONT_TGT_SCORE)=84,
    MKB(FONT_TGT_SCORE_S)=85,
    MKB(FONT_TGT_SPEED)=86,
    MKB(FONT_TGT_ROUND)=87,
    MKB(FONT_TGT_ALT)=88,
    MKB(FONT_TGT_WIND)=89,
    MKB(FONT_DMY71)=90,
    MKB(FONT_DMY72)=91,
    MKB(FONT_DMY73)=92,
    MKB(FONT_NUM_NML_SCORE)=93,
    MKB(FONT_NUM_NML_TIME)=94,
    MKB(FONT_NUM_NML_TIME_S)=95,
    MKB(FONT_NUM_NML_SPEED)=96,
    MKB(FONT_NUM_NML_SARU)=97,
    MKB(FONT_ASC_NML_D5)=98,
    MKB(FONT_ASC_NML_D6)=99,
    MKB(FONT_ASC_NML_D7)=100,
    MKB(FONT_ASC_NML_D8)=101,
    MKB(FONT_ASC_NML_D9)=102,
    MKB(FONT_DMY79)=103,
    MKB(FONT_DMY7a)=104,
    MKB(FONT_DMY7b)=105,
    MKB(FONT_DMY7c)=106,
    MKB(FONT_DMY7d)=107,
    MKB(FONT_DMY7e)=108,
    MKB(FONT_DMY7f)=109,
    MKB(FONT_BOA_TIME_NUM)=110,
    MKB(FONT_BOA_DMY02)=111,
    MKB(FONT_BOA_RANK)=112,
    MKB(FONT_BOA_PLAYER)=113,
    MKB(FONT_BOA_SPD_NUM)=114,
    MKB(FONT_BOA_LAP_MARK_NUM)=115,
    MKB(FONT_BOA_LAP_MARK_NUM_S)=116,
    MKB(FONT_BOA_DMY08)=117,
    MKB(FONT_BOA_DMY09)=118,
    MKB(FONT_BOA_DMY10)=119,
    MKB(FONT_BOA_LAP_NUM)=120,
    MKB(FONT_BOA_DMY12)=121,
    MKB(FONT_BOA_DMY13)=122,
    MKB(FONT_BOA_DMY14)=123,
    MKB(FONT_BOA_DMY15)=124,
    MKB(FONT_BOA_DMY16)=125,
    MKB(FONT_DOG_SPEED)=126,
    MKB(FONT_DOG_SPEED_S)=127,
    MKB(FONT_DOG_MSCORE)=128,
    MKB(FONT_DOG_SCORE)=129,
    MKB(FONT_DOG_SCORE_M)=130,
    MKB(FONT_DOG_SCORE_S)=131,
    MKB(FONT_DOG_ROUND)=132,
    MKB(FONT_DOG_ALT)=133,
    MKB(FONT_DOG_RESULT)=134,
    MKB(FONT_DOG_TIME)=135,
    MKB(FONT_DOG_DMY02)=136,
    MKB(FONT_DOG_DMY03)=137,
    MKB(FONT_DOG_DMY04)=138,
    MKB(FONT_DOG_DMY05)=139,
    MKB(FONT_JAP_TAG)=140,
    MKB(FONT_JAP_DMY)=141,
    MKB(FONT_JAP_24x24_2)=142,
    MKB(FONT_JAP_24x24_2P)=143,
    MKB(FONT_JAP_24x24_I)=144
};
typedef undefined1 MKB(Font8);

enum {
    MKB(ALIGN_LOWER_RIGHT)=0,
    MKB(ALIGN_CENTER_RIGHT)=1,
    MKB(ALIGN_UPPER_RIGHT)=2,
    MKB(ALIGN_LOWER_CENTER)=3,
    MKB(ALIGN_CENTER)=4,
    MKB(ALIGN_UPPER_CENTER)=5,
    MKB(ALIGN_LOWER_LEFT)=6,
    MKB(ALIGN_CENTER_LEFT)=7,
    MKB(ALIGN_UPPER_LEFT)=8
};
typedef undefined1 MKB(SpriteAlignment);

typedef struct MKB(Rgb24) MKB(Rgb24), *MKB(PRgb24);

enum {
    MKB(SPRITE_PRESS_START)=1,
    MKB(SPRITE_LOGO)=3,
    MKB(SPRITE_PAUSE_MENU)=4,
    MKB(SPRITE_HOW_TO)=10,
    MKB(SPRITE_SPECIAL_STAGE)=14,
    MKB(SPRITE_LOADIN_STAGE_NAME)=15,
    MKB(SPRITE_1UP)=16,
    MKB(SPRITE_SARU_CONTROL_DESCRIPTION)=17,
    MKB(SPRITE_BANANA_ICON)=21,
    MKB(SPRITE_HUD_STAGE_NAME)=34,
    MKB(SPRITE_HUD_STAGE_NUMBER)=35,
    MKB(SPRITE_COPYRIGHT_TEXT_1)=37,
    MKB(SPRITE_COPYRIGHT_TEXT_2)=38,
    MKB(SPRITE_COPYRIGHT_TEXT_3)=39,
    MKB(SPRITE_DEMO_MES)=58,
    MKB(SPRITE_GAMEDATA)=95,
    MKB(SPRITE_HUD_KIWAKU_WORLD)=112,
    MKB(SPRITE_TITLE)=113
};
typedef undefined1 MKB(SpriteUniqueID);

typedef struct MKB(SpriteTex) MKB(SpriteTex), *MKB(PSpriteTex);

typedef struct MKB(DVDFileInfo) MKB(DVDFileInfo), *MKB(PDVDFileInfo);

typedef struct MKB(DVDCommandBlock) MKB(DVDCommandBlock), *MKB(PDVDCommandBlock);

typedef struct MKB(DVDDiskID) MKB(DVDDiskID), *MKB(PDVDDiskID);

struct MKB(Rgb24) {
    MKB(u8) red;
    MKB(u8) green;
    MKB(u8) blue;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Rgb24)) == 0x3);

struct MKB(Sprite) {
    MKB(SpriteType)  type; /* Whether it's visible or not? */
    MKB(Font8)  font; /* Seems to affect the font size/type on the pause menu? */
    MKB(u8) index;
    MKB(SpriteAlignment)  alignment; /* Alignment of the sprite relative to the origin */
    struct MKB(Vec2d) pos;
    struct MKB(Rgb24) mult_color;
    MKB(SpriteUniqueID)  unique_id; /* Some sort of unique ID for sprites. The ID is mostly unique, there are only a few exceptions where sprites share the same unique ID, and when they do, they're usually related. Seems to be used to find or destroy specific sprites. */
    MKB(s16) g_counter; /* At least in the press start/select text sprites, this is used as some kind of counter when ticking */
    MKB(s16) widescreen_translation_x; /* When widescreen mode is enabled, the sprite is scaled down and then translated to be aligned properly. This value indicates the X amount by which this sprite is translated. */
    MKB(undefined) field_0x14[0x6];
    MKB(u8) horizontal_spacing; /* Used for adding horizontal spaces between letters when drawing a text sprite */
    MKB(u8) vertical_spacing; /* Same as horizontal_spacing, but vertical (for newlines). Appears to be supported, but is never used. */
    MKB(undefined) field_0x1c[0x4];
    float field21_0x20;
    undefined4 field22_0x24;
    MKB(undefined) field_0x28[0x4];
    struct MKB(SpriteTex) * tex;
    void (* dest_func)(struct MKB(Sprite) *);
    void (* tick_func)(MKB(u8) *, struct MKB(Sprite) *);
    void (* disp_func)(struct MKB(Sprite) *);
    undefined2 bmp;
    MKB(undefined) field_0x3e[0x2];
    float width;
    float height;
    float depth;
    MKB(s32) para1; /* Arbitrary int param 1 */
    MKB(s32) para2; /* Arbitrary int param 2 */
    MKB(s32) para3; /* Arbitrary int param 3 */
    MKB(f32) fpara1; /* Arbitrary float param 1 */
    MKB(f32) fpara2; /* Arbitrary float param 2 */
    MKB(f32) fpara3; /* Arbitrary float param 3 */
    struct MKB(Sprite) * prev_sprite;
    struct MKB(Sprite) * next_sprite;
    MKB(s32) left;
    MKB(s32) top;
    MKB(s32) right;
    MKB(s32) bottom;
    MKB(u16) rot;
    MKB(u16) field50_0x7e;
    float alpha; /* called trnsl in game? */
    struct MKB(Rgb24) add_color;
    MKB(undefined) field_0x87[0x1];
    MKB(u32) g_flags1;
    MKB(u32) g_flags2;
    MKB(f32) u1;
    MKB(f32) v1;
    MKB(f32) u2;
    MKB(f32) v2;
    char text[48]; /* If this sprite displays text, this is what it shows, otherwise this is usually just an identifier name */
} __attribute__((__packed__));
static_assert(sizeof(MKB(Sprite)) == 0xd0);

struct MKB(DVDCommandBlock) {
    struct MKB(DVDCommandBlock) * next;
    struct MKB(DVDCommandBlock) * prev;
    MKB(u32) command;
    MKB(s32) state;
    MKB(u32) offset;
    MKB(u32) length;
    void * addr;
    MKB(u32) currTransferSize;
    MKB(u32) transferredSize;
    struct MKB(DVDDiskID) * id;
    void (* callback)(MKB(s32), struct MKB(DVDCommandBlock) *);
    void * userData;
} __attribute__((__packed__));
static_assert(sizeof(MKB(DVDCommandBlock)) == 0x30);

struct MKB(DVDFileInfo) { /* Not entirely sure about this one... I've filled in some fields that only exist within `#ifdef MACOS` blocks according to the GC SDK on my end. Leaving them out seems wrong if I look at DVDFastOpen. Also I haven't put *all* MACOS fields in there since I don't know the size of IOParam (from a Mac header). */
    struct MKB(DVDCommandBlock) cb;
    MKB(u32) startAddr;
    MKB(u32) length;
    void (* callback)(MKB(s32), struct MKB(DVDFileInfo) *);
} __attribute__((__packed__));
static_assert(sizeof(MKB(DVDFileInfo)) == 0x3c);

struct MKB(DVDDiskID) {
    char gameName[4];
    char company[2];
    MKB(u8) diskNumber;
    MKB(u8) gameVersion;
    MKB(u8) streaming;
    MKB(u8) streamingBufSize; /* 0 = default */
    MKB(u8) padding[22]; /* 0's are stored */
} __attribute__((__packed__));
static_assert(sizeof(MKB(DVDDiskID)) == 0x20);

struct MKB(SpriteTex) {
    MKB(u8) field0_0x0;
    MKB(s8) field1_0x1;
    MKB(u16) field2_0x2;
    MKB(s32) tex_index;
    struct MKB(GXTexObj) texobj;
    void * tex_data;
    MKB(u32) tex_data_size;
    MKB(u16) width;
    MKB(u16) height;
    MKB(GXTexFmt)  format;
    MKB(OSHeapHandle) heap;
    struct MKB(DVDFileInfo) dvd_file;
    MKB(undefined) field_0x78[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(SpriteTex)) == 0x7c);

typedef struct MKB(ShadowReq) MKB(ShadowReq), *MKB(PShadowReq);

typedef struct MKB(GXColor) MKB(GXColor), *MKB(PGXColor);

struct MKB(GXColor) { /* A generic color structure used by various GX API functions. */
    MKB(u8) r;
    MKB(u8) g;
    MKB(u8) b;
    MKB(u8) a;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXColor)) == 0x4);

struct MKB(ShadowReq) { /* Parameters for drawing a shadow texture/quad */
    struct MKB(Vec) pos;
    struct MKB(S16Vec) rot;
    MKB(undefined) field_0x12[0x2];
    struct MKB(Vec) scale;
    MKB(f32) g_f32_1;
    MKB(f32) g_f32_2;
    struct MKB(GmaModel) * model;
    struct MKB(GXColor) color;
    MKB(f32) g_uniform_scale;
    MKB(undefined) field_0x34[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(ShadowReq)) == 0x38);

typedef struct MKB(HeapConfig) MKB(HeapConfig), *MKB(PHeapConfig);

struct MKB(HeapConfig) { /* Set of sizes for game heaps */
    MKB(u32) stage_heap_size;
    MKB(u32) bg_heap_size;
    MKB(u32) chara_heap_size;
    MKB(u32) replay_heap_size;
    MKB(u32) flags;
} __attribute__((__packed__));
static_assert(sizeof(MKB(HeapConfig)) == 0x14);

typedef struct MKB(GSomethingWithPadMotorsStruct) MKB(GSomethingWithPadMotorsStruct), *MKB(PGSomethingWithPadMotorsStruct);

struct MKB(GSomethingWithPadMotorsStruct) {
    undefined1 a;
    MKB(undefined) field_0x1[0x1];
    undefined2 b;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GSomethingWithPadMotorsStruct)) == 0x4);

typedef struct MKB(SpriteDrawRequest) MKB(SpriteDrawRequest), *MKB(PSpriteDrawRequest);

enum {
    MKB(SDRF_G_SCREENFADE_RELATED)=262144,
    MKB(SDRF_FLIP_X)=524288,
    MKB(SDRF_FLIP_Y)=1048576,
    MKB(SDRF_G_ID_FORMAT_RELATED)=4194304,
    MKB(SDRF_G_X_SCALE_RELATED)=16777216
};
typedef undefined4 MKB(SpriteDrawReqFlags);

struct MKB(SpriteDrawRequest) { /* Used by Sprite disp() functions to render a texture on the screen, sometimes multiple times per disp() call to render multiple things on-screen per Sprite object */
    MKB(s32) id; /* At least some of the time: bits 16-24 are category, 24-31 are id in the category. Sometimes used as just an ID with no category? */
    struct MKB(Vec) pos;
    struct MKB(Vec2d) scale;
    float u1; /* First texture coordinate, U component */
    float v1; /* First texture coordinate, V component */
    float u2; /* Second texture coordinate, U component */
    float v2; /* Second texture coordinate, V component */
    MKB(s32) rot_z; /* For some reason this is 32-bit instead of s16? */
    float alpha;
    MKB(s32) g_unk1;
    MKB(SpriteDrawReqFlags)  flags;
    MKB(u32) mult_color; /* RGB24 multiply blend color */
    MKB(u32) add_color; /* RGB24 add blend color */
    MKB(s16) widescreen_translation_x;
    MKB(undefined) field_0x42[0xe];
} __attribute__((__packed__));
static_assert(sizeof(MKB(SpriteDrawRequest)) == 0x50);

typedef struct MKB(CmPlayerProgress) MKB(CmPlayerProgress), *MKB(PCmPlayerProgress);

typedef struct MKB(CmStage) MKB(CmStage), *MKB(PCmStage);

struct MKB(CmStage) { /* Stage in Challenge Mode. Stage course number is what's shown in bottom left of screen */
    MKB(s32) stage_course_num;
    MKB(s32) stage_id;
} __attribute__((__packed__));
static_assert(sizeof(MKB(CmStage)) == 0x8);

struct MKB(CmPlayerProgress) { /* Seems to be one of these per player, not sure what they are exactly yet */
    struct MKB(CmStage) curr_stage;
    struct MKB(CmStage) next_stages[3]; /* Appears to track the "next" stage for the player in the first element (assuming a blue goal is taken). The latter two elements are never updated, and I don't think the first element is even used for anything */
    MKB(s16) field2_0x20;
    MKB(s16) g_next_stage_idx;
} __attribute__((__packed__));
static_assert(sizeof(MKB(CmPlayerProgress)) == 0x24);

enum { /* These are normally just #defines in the SDK's PAD library. Also these are supposed to be signed */
    MKB(PAD_ERR_NONE)=0,
    MKB(PAD_ERR_TRANSFER)=253,
    MKB(PAD_ERR_NOT_READY)=254,
    MKB(PAD_ERR_NO_CONTROLLER)=255
};
typedef undefined1 MKB(PadError);

typedef struct MKB(GoalBag) MKB(GoalBag), *MKB(PGoalBag);

struct MKB(GoalBag) { /* Extra goalbag-specific state pointed to by goalbag StageObjects */
    MKB(undefined) field_0x0[0x4];
    undefined4 field4_0x4;
    undefined4 field5_0x8;
    struct MKB(Stobj) * stobj; /* Created by retype action */
    struct MKB(Vec) g_pos;
    MKB(undefined) field_0x1c[0x8];
    undefined4 field16_0x24;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GoalBag)) == 0x28);

typedef struct MKB(WorldInfo) MKB(WorldInfo), *MKB(PWorldInfo);

struct MKB(WorldInfo) {
    MKB(s16) stage_tilt_x;
    MKB(s16) stage_tilt_z;
    MKB(undefined) field_0x4[0x38];
} __attribute__((__packed__));
static_assert(sizeof(MKB(WorldInfo)) == 0x3c);

enum { /* Requested parameter to be returned from textdraw_chara_load. */
    MKB(RETURN_WIDTH)=0,
    MKB(RETURN_CHARACTER_COUNT)=1,
    MKB(G_RETURN_SOME_BOOL)=2,
    MKB(RETURN_LINE_COUNT)=3,
    MKB(G_LOAD_ARAM_FONT)=4,
    MKB(G_LOAD_ARAM_FONT_2)=5
};
typedef undefined1 MKB(TextdrawCharaLoadReturnParameter);

typedef struct MKB(SeesawInfo) MKB(SeesawInfo), *MKB(PSeesawInfo);

struct MKB(SeesawInfo) { /* Allocated on the heap for an itemgroup if it's a seesaw. Points to another struct also allocated on the heap that contains physics state info like current rotation angle. */
    short itemgroup_idx;
    word g_type; /* Seems to always be zero, but the way it's set seems to indicate there may have been more than one type of seesaw at some point */
    struct MKB(SeesawState) * state;
    void * init_func;
    void * dest_func;
    void * tick_func;
    void * coli_func;
    void * g_some_func5;
    void * g_replay_func;
} __attribute__((__packed__));
static_assert(sizeof(MKB(SeesawInfo)) == 0x20);

typedef struct MKB(GSoundCue) MKB(GSoundCue), *MKB(PGSoundCue);

struct MKB(GSoundCue) {
    short g_sfx_id;
    short g_sfx_rid;
    char field2_0x4;
    byte g_sfx_pan; /* From 0 - 127? */
    char g_sfx_span;
    char g_sfx_grp;
    int player_id;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GSoundCue)) == 0xc);

enum {
    MKB(DATA_SELECT)=0,
    MKB(STAGE_SELECT_PRE_INTRO_SEQUENCE)=1,
    MKB(STAGE_SELECT_INTRO_SEQUENCE)=2,
    MKB(STAGE_SELECT_UNK3)=3,
    MKB(STAGE_SELECT_IDLE)=4,
    MKB(STAGE_SELECT_UNK5)=5,
    MKB(STAGE_SELECT_SELECTED)=6
};
typedef undefined2 MKB(StoryModeStageSelectState);

enum {
    MKB(MF_NONE)=0,
    MKB(MF_0x1)=1,
    MKB(MF_0x2)=2,
    MKB(MF_0x4)=4,
    MKB(MF_PLAYING_EXTRA_COURSE)=8,
    MKB(MF_PLAYING_MASTER_NOEX_COURSE)=16,
    MKB(MF_0x20)=32,
    MKB(MF_ADDITIONAL_REL_LOADED)=512,
    MKB(MG_G_NO_BANANAS)=4096,
    MKB(MF_0x2000)=8192,
    MKB(MF_OPTION_MODE)=262144,
    MKB(MF_G_STOP_GAME_LOOP)=2097152,
    MKB(MF_0x400000)=4194304,
    MKB(MF_PLAYING_MASTER_EX_COURSE)=8388608,
    MKB(MF_G_PLAYING_MASTER_COURSE)=33554432
};
typedef undefined4 MKB(ModeFlag);

typedef struct MKB(GSomeSpriteStruct) MKB(GSomeSpriteStruct), *MKB(PGSomeSpriteStruct);

struct MKB(GSomeSpriteStruct) {
    struct MKB(Sprite) * g_some_sprite;
    struct MKB(GSomeSpriteStruct) * g_prev;
    struct MKB(GSomeSpriteStruct) * g_next;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GSomeSpriteStruct)) == 0xc);

typedef struct MKB(GStaffRollStruct) MKB(GStaffRollStruct), *MKB(PGStaffRollStruct);

struct MKB(GStaffRollStruct) {
    MKB(undefined) field_0x0[0x4];
    undefined4 field4_0x4;
    MKB(undefined) field_0x8[0x50];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GStaffRollStruct)) == 0x58);

typedef struct MKB(Item) MKB(Item), *MKB(PItem);

enum {
    MKB(ITEM_COIN)=0,
    MKB(ITEM_FGT_BANANA)=1,
    MKB(ITEM_MINI_RACE)=2,
    MKB(ITEM_FGT_POWER)=3,
    MKB(ITEM_FGT_PUNCH)=4,
    MKB(ITEM_PILOT)=5,
    MKB(ITEM_DOGFIGHT)=6,
    MKB(ITEM_TMP001)=7,
    MKB(ITEM_TMP002)=8,
    MKB(ITEM_TMP003)=9,
    MKB(ITEM_TMP004)=10,
    MKB(ITEM_TMP005)=11,
    MKB(ITEM_TMP006)=12,
    MKB(ITEM_TMP007)=13,
    MKB(ITEM_TMP008)=14,
    MKB(ITEM_TMP009)=15,
    MKB(ITEM_TMP010)=16,
    MKB(ITEM_TMP011)=17,
    MKB(ITEM_TMP012)=18,
    MKB(ITEM_TMP013)=19,
    MKB(ITEM_TMP014)=20,
    MKB(ITEM_TMP015)=21,
    MKB(ITEM_TMP016)=22
};
typedef undefined2 MKB(ItemType);

typedef struct MKB(RaycastHit) MKB(RaycastHit), *MKB(PRaycastHit);

struct MKB(RaycastHit) {
    MKB(u32) flags;
    struct MKB(Vec) pos; /* Position of ray-geometry intersection */
    struct MKB(Vec) normal; /* Geometry normal at point of ray-geometry intersection */
} __attribute__((__packed__));
static_assert(sizeof(MKB(RaycastHit)) == 0x1c);

struct MKB(Item) { /* Represents an item that can be picked up by the player. These are bananas in main game, but can be other pickups in the party games. Are "tickable" like events, effects, etc. */
    word index;
    short id;
    MKB(ItemType)  type;
    MKB(s16) coin_type;
    MKB(u32) g_some_bitfield;
    undefined2 field5_0xc;
    undefined2 g_some_flag;
    short field7_0x10;
    undefined2 g_some_flag2;
    float scale;
    float field10_0x18;
    MKB(undefined) * g_something_with_gma_model;
    struct MKB(Vec) position;
    struct MKB(Vec) velocity;
    struct MKB(S16Vec) rotation;
    struct MKB(S16Vec) angular_velocity;
    struct MKB(Vec) g_position_copy;
    struct MKB(S16Vec) g_rotation_copy;
    MKB(undefined) field_0x56[0x2];
    void (* item_coli_func)(struct MKB(Item) *, struct MKB(PhysicsBall) *); /* Created by retype action */
    MKB(u8) itemgroup_idx;
    MKB(undefined) field_0x5d[0x1];
    MKB(s16) g_some_frame_counter;
    struct MKB(Vec) * g_some_vec3f_ptr;
    undefined4 field25_0x64;
    struct MKB(GmaModel) * shadow_model;
    struct MKB(S16Vec) shadow_rot;
    MKB(undefined) field_0x72[0x2];
    float shadow_y_offset;
    float g_something_with_shadow_disp;
    struct MKB(Vec) shadow_scale;
    float shadow_intensity;
    struct MKB(Vec) g_position_copy_2;
    struct MKB(RaycastHit) shadow_raycast;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Item)) == 0xb4);

enum { /* Background music tracks by ID */
    MKB(BGM_SEL_WORLD)=0,
    MKB(BGM_CAVE)=1,
    MKB(BGM_SEL_FUTSAL)=2,
    MKB(BGM_FUTSAL)=3,
    MKB(BGM_WATER)=4,
    MKB(BGM_SEL_DEF)=5,
    MKB(BGM_NABE)=6,
    MKB(BGM_JUNGLE)=7,
    MKB(BGM_BOAT)=8,
    MKB(BGM_BILI)=9,
    MKB(BGM_SEL_BOWLING)=10,
    MKB(BGM_SEL_GOLF)=11,
    MKB(BGM_CONTINUE)=12,
    MKB(BGM_OVER)=13,
    MKB(BGM_FIGHT)=14,
    MKB(BGM_SEL_FIGHT)=15,
    MKB(BGM_COLONY)=16,
    MKB(BGM_BILL_FOUL)=17,
    MKB(BGM_BILL_LOSE)=18,
    MKB(BGM_BILL_SENKOU)=19,
    MKB(BGM_BILL_WIN)=20,
    MKB(BGM_BOWL_MISS)=21,
    MKB(BGM_BOWL_NORMAL)=22,
    MKB(BGM_BOWL_PERFECT)=23,
    MKB(BGM_BOWL_SPARE)=24,
    MKB(BGM_BOWL_STRIKE)=25,
    MKB(BGM_BOWL_TURKEY)=26,
    MKB(BGM_FIGHT_WIN)=27,
    MKB(BGM_GOLF_BIRDIE)=28,
    MKB(BGM_GOLF_BOGIE)=29,
    MKB(BGM_GOLF_GIVEUP1)=30,
    MKB(BGM_GOLF_HOLEIN1)=31,
    MKB(BGM_GOLF_OB)=32,
    MKB(BGM_GOLF_PAR)=33,
    MKB(BGM_RACE_LOSE)=34,
    MKB(BGM_RACE_WIN)=35,
    MKB(BGM_TAG_MISS)=36,
    MKB(BGM_TAG_SCORE_HI)=37,
    MKB(BGM_TAG_SCORE_NML)=38,
    MKB(BGM_TAG_SCORE_SP)=39,
    MKB(BGM_AVLOGO)=40,
    MKB(BGM_10MIN1)=41,
    MKB(BGM_BONUS)=42,
    MKB(BGM_BOWLING)=43,
    MKB(BGM_10MIN2)=44,
    MKB(BGM_SEL_BASEBALL)=45,
    MKB(BGM_SEL_BOAT)=46,
    MKB(BGM_SEL_TARGET)=47,
    MKB(BGM_ENTRY)=48,
    MKB(BGM_HYOSHO)=49,
    MKB(BGM_ROLL)=50,
    MKB(BGM_S1A_SNG1)=51,
    MKB(BGM_S1A_SNG2)=52,
    MKB(BGM_S1A_SNG3)=53,
    MKB(BGM_TITLE)=54,
    MKB(BGM_S1A_SNG4)=55,
    MKB(BGM_FIGHT_VIEW)=56,
    MKB(BGM_GOLF_VIEW)=57,
    MKB(BGM_SF)=58,
    MKB(BGM_SEL_TENNIS)=59,
    MKB(BGM_TARGET)=60,
    MKB(BGM_YUENCHI)=61,
    MKB(BGM_SEL_RACE)=62,
    MKB(BGM_SENTAKU)=63,
    MKB(BGM_KUJIRA)=64,
    MKB(BGM_RACE)=65,
    MKB(BGM_GOLF)=66,
    MKB(BGM_SEL_BILI)=67,
    MKB(BGM_GREP)=68,
    MKB(BGM_RACE_ADVAN)=69,
    MKB(BGM_CLOCKTOWER)=70,
    MKB(BGM_BOAT_ADVAN)=71,
    MKB(BGM_SEL_SHOT)=72,
    MKB(BGM_RACE_EXPERT)=73,
    MKB(BGM_SEL_DOG)=74,
    MKB(BGM_BOAT_EXPERT)=75,
    MKB(BGM_BOAT_LOSE)=76,
    MKB(BGM_BOAT_WIN)=77,
    MKB(BGM_SHOT)=78,
    MKB(BGM_SHOT_CLEAR)=79,
    MKB(BGM_SHOT_LOSE)=80,
    MKB(BGM_DOG)=81,
    MKB(BGM_SHOT_ADVAN)=82,
    MKB(BGM_SHOT_BOSS)=83,
    MKB(BGM_DOG_WIN)=84,
    MKB(BGM_DOG_LOSE)=85,
    MKB(BGM_FUTSAL_GOAL)=86,
    MKB(BGM_POINT)=87,
    MKB(BGM_SHOT_EXPERT)=88,
    MKB(BGM_GOLF_EAGLE)=89,
    MKB(BGM_SEL_CLEAR)=90,
    MKB(BGM_SCEN_ENDING)=91,
    MKB(BGM_S3_SNG1)=92,
    MKB(BGM_S3_SNG2)=93,
    MKB(BGM_S3_SNG3)=94,
    MKB(BGM_ADV)=95,
    MKB(BGM_S3_SNG4)=96,
    MKB(BGM_S3_SNG5)=97,
    MKB(BGM_S3_SNG6)=98,
    MKB(BGM_S3_SNG7)=99,
    MKB(BGM_S3_SNG8)=100,
    MKB(BGM_SCEN_ENDING4)=101,
    MKB(BGM_S1B_SNG1)=102,
    MKB(BGM_S1B_SNG2)=103,
    MKB(BGM_S1B_SNG3)=104,
    MKB(BGM_S2_SNG1)=105,
    MKB(BGM_S2_SNG2)=106,
    MKB(BGM_S2_SNG3)=107,
    MKB(BGM_S2_SNG4)=108,
    MKB(BGM_S2_SNG5)=109,
    MKB(BGM_S2_SNG6)=110,
    MKB(BGM_S2_SNG7)=111,
    MKB(BGM_S2_SNG8)=112,
    MKB(BGM_S4_SNG1)=113,
    MKB(BGM_S4_SNG2)=114,
    MKB(BGM_S4_SNG3)=115,
    MKB(BGM_S4_SNG4)=116,
    MKB(BGM_S4_SNG5)=117,
    MKB(BGM_S4_SNG6)=118,
    MKB(BGM_S4_SNG7)=119,
    MKB(BGM_S6_SNG1)=120,
    MKB(BGM_S6_SNG2)=121,
    MKB(BGM_S6_SNG3)=122,
    MKB(BGM_S5_SNG1)=123,
    MKB(BGM_S5_SNG2)=124,
    MKB(BGM_S5_SNG3)=125,
    MKB(BGM_S5_SNG4)=126,
    MKB(BGM_S5_SNG5)=127,
    MKB(BGM_S5_SNG6)=128,
    MKB(BGM_S5_SNG7)=129,
    MKB(BGM_S7_SNG1)=130,
    MKB(BGM_S7_SNG2)=131,
    MKB(BGM_S7_SNG3)=132,
    MKB(BGM_S7_SNG4)=133,
    MKB(BGM_S7_SNG5)=134,
    MKB(BGM_S7_SNG6)=135,
    MKB(BGM_SLAST_SNG1)=136,
    MKB(BGM_SLAST_SNG2)=137,
    MKB(BGM_S8_SNG1)=138,
    MKB(BGM_S8_SNG2)=139,
    MKB(BGM_S9_SNG1)=140,
    MKB(BGM_S9_SNG2)=141,
    MKB(BGM_S10_SNG1)=142,
    MKB(BGM_S10_SNG2)=143,
    MKB(BGM_S10_SNG3)=144,
    MKB(BGM_S10_SNG4)=145,
    MKB(BGM_S10_SNG5)=146
};
typedef undefined2 MKB(BgmTrack);

enum {
    MKB(STYLE_ORIGINAL)=0,
    MKB(STYLE_REGULAR)=1,
    MKB(STYLE_BOLD)=2,
    MKB(STYLE_ITALIC)=3,
    MKB(STYLE_BOLD_ITALIC)=4,
    MKB(STYLE_TEGAKI)=5
};
typedef undefined1 MKB(FontStyle);

typedef struct MKB(CourseCommand) MKB(CourseCommand), *MKB(PCourseCommand);

enum { /* See cmEntryFormat.txt notes */
    MKB(COURSE_CMD_IF)=0,
    MKB(COURSE_CMD_THEN)=1,
    MKB(COURSE_CMD_INFO)=2,
    MKB(COURSE_CMD_END)=3
};
typedef undefined1 MKB(CourseCommandOpcode);

struct MKB(CourseCommand) { /* Challenge Mode Entry, see cmEntryFormat.txt by TwixNinja or SMB1 decompilation for more info */
    MKB(CourseCommandOpcode)  opcode;
    MKB(u8) type;
    MKB(undefined) field2_0x2[2];
    MKB(s32) value;
    MKB(undefined) field4_0x8[20];
} __attribute__((__packed__));
static_assert(sizeof(MKB(CourseCommand)) == 0x1c);

typedef struct MKB(RelBufferInfo) MKB(RelBufferInfo), *MKB(PRelBufferInfo);

struct MKB(RelBufferInfo) {
    void * rel_buffer;
    void * bss_buffer;
} __attribute__((__packed__));
static_assert(sizeof(MKB(RelBufferInfo)) == 0x8);

typedef struct MKB(UnlockInfo) MKB(UnlockInfo), *MKB(PUnlockInfo);

struct MKB(UnlockInfo) { /* Info about which stuff in the game is unlocked, such as master, number of monkeys, party games, etc. Which story mode / challenge mode stages are unlocked are stored separately however. */
    byte master_unlocked;
    byte monkeys; /* Number of monkeys (lives) unlocked, aka the max you can use in challenge mode */
    MKB(undefined) field_0x2[0x1];
    byte staff_credits_game_unlocked;
    dword play_points; /* Seems to function as "current number of play points" when you don't have everything unlocked, and "highest play points record" when you do */
    dword newest_play_point_record;
    MKB(undefined) field_0xc[0x2];
    word movies; /* One bit for each of the 12 movies */
    dword party_games; /* Bitfield; normally 0x0001b600 when all are unlocked, and not zero when only base games are unlocked */
    word g_movies_watched; /* Determines whether unlocked movies are grayed out; does this mean "movie watched"? */
    MKB(undefined) field_0x16[0x6e];
} __attribute__((__packed__));
static_assert(sizeof(MKB(UnlockInfo)) == 0x84);

typedef struct MKB(DigitalInputGroup) MKB(DigitalInputGroup), *MKB(PDigitalInputGroup);

enum { /* These are normally just #defines in the SDK's PAD library */
    MKB(PAD_BUTTON_LEFT)=1,
    MKB(PAD_BUTTON_RIGHT)=2,
    MKB(PAD_BUTTON_DOWN)=4,
    MKB(PAD_BUTTON_UP)=8,
    MKB(PAD_TRIGGER_Z)=16,
    MKB(PAD_TRIGGER_R)=32,
    MKB(PAD_TRIGGER_L)=64,
    MKB(PAD_BUTTON_A)=256,
    MKB(PAD_BUTTON_B)=512,
    MKB(PAD_BUTTON_X)=1024,
    MKB(PAD_BUTTON_Y)=2048,
    MKB(PAD_BUTTON_START)=4096
};
typedef undefined2 MKB(PadDigitalInput);

struct MKB(DigitalInputGroup) { /* Consolidated bitfields for digital button inputs corresponding to the five PadStatusGroup types */
    MKB(PadDigitalInput)  raw;
    MKB(PadDigitalInput)  prev_tick;
    MKB(PadDigitalInput)  pressed;
    MKB(PadDigitalInput)  released;
    MKB(PadDigitalInput)  repeated;
} __attribute__((__packed__));
static_assert(sizeof(MKB(DigitalInputGroup)) == 0xa);

typedef struct MKB(Ball) MKB(Ball), *MKB(PBall);

enum {
    MKB(PHYS_MODE_G_GOALED)=5,
    MKB(PHYS_MODE_SLOWDOWN_BLASTOFF)=6,
    MKB(PHYS_MODE_NORMAL)=32
};
typedef undefined1 MKB(BallMode);

enum { /* Flags that mostly affect ball physics and controls */
    MKB(PHYS_NONE)=0,
    MKB(PHYS_ON_GROUND)=1,
    MKB(PHYS_UNK_0x2)=2,
    MKB(PHYS_UNK_0x4)=4,
    MKB(PHYS_UNK_0x8)=8,
    MKB(PHYS_UNK_0x10)=16,
    MKB(PHYS_UNK_0x20)=32,
    MKB(PHYS_UNK_0x40)=64,
    MKB(PHYS_UNK_0x80)=128,
    MKB(PHYS_DISABLE_GRAVITY)=256,
    MKB(PHYS_BLAST_UP)=512,
    MKB(PHYS_SLOW_DOWN)=1024,
    MKB(PHYS_G_DISABLE_GOAL_DETECTION)=2048,
    MKB(PHYS_DISABLE_CONTROLS)=4096,
    MKB(PHYS_G_SECRET_POSTGOAL_DANCE)=8192,
    MKB(PHYS_UNK_0x4000)=16384
};
typedef undefined4 MKB(BallPhysFlags);

struct MKB(Ball) {
    MKB(Status)  status; /* Actually called just "STAT" in the debug menu */
    MKB(undefined) field_0x1[0x1];
    MKB(u8) monkey_count; /* Life counter */
    MKB(BallMode)  mode; /* Ball's mode, which determines how its physics behave */
    struct MKB(Vec) pos;
    struct MKB(Vec) prev_pos;
    struct MKB(Vec) vel; /* Velocity/speed */
    struct MKB(S16Vec) visual_rot;
    MKB(u8) idx; /* The index of the ball in the ball pool, aka 0 for the first ball, 1 for the second */
    MKB(undefined) field_0x2f[0x1];
    MKB(Mtx) model_transform;
    struct MKB(S16Vec) visual_rot_vel;
    short padding2;
    float ball_size;
    float gravity;
    float restitution;
    float visual_scale;
    MKB(s32) banana_count;
    MKB(s32) score;
    MKB(s32) frames_since_spawn;
    MKB(undefined) field_0x84[0xe];
    MKB(s16) g_monkey_angle; /* Seems to be related to the direction the monkey is facing */
    MKB(undefined) field_0x94[0x4];
    MKB(u32) g_effect_flags;
    MKB(BallPhysFlags)  phys_flags; /* Some more flags related to ball state? The lowest-order bit may represent "is ball touching the ground" and I believe if affects the physics */
    struct MKB(Quat) g_monkey_rotation; /* Rotation of the monkey inside the ball? */
    struct MKB(Quat) g_ball_rotation; /* Rotation of the ball itself? */
    struct MKB(Vec) some_vec3;
    float some_length;
    MKB(Mtx) ball_transform_copy;
    float speed;
    struct MKB(Ape) * ape;
    int field48_0x108;
    struct MKB(Vec) ape_facedir_point; /* The point of interest that the monkey looks at (goal, banana, etc) */
    float something_with_ape_facedir; /* Approaches 1 the closer you are to the point of interest */
    struct MKB(Vec) g_last_coli_normal; /* Maybe inverse of the normal of the last triangle collided with? */
    MKB(undefined) field_0x128[0x4];
    dword g_race_flags;
    short g_other_counter;
    undefined2 field58_0x132;
    MKB(s16) field59_0x134;
    MKB(s16) g_something_timer; /* Created by retype action */
    MKB(undefined) field_0x138[0x4];
    float hardest_coli_speed;
    int g_banana_count_copy;
    MKB(undefined) field_0x144[0x8];
    float physical_ball_radius;
    MKB(undefined) field_0x150[0x4];
    undefined1 field80_0x154;
    undefined1 field81_0x155;
    MKB(u8) g_ball_color_index; /* Ball color by player index: 0 red, 1 blue, 2 yellow, 3 green, etc */
    undefined1 field83_0x157;
    MKB(undefined) field_0x158[0x2];
    short g_some_counter;
    struct MKB(Vec) g_some_pos1;
    MKB(undefined) field_0x168[0x10];
    MKB(u8) g_some_game_flag; /* Some values make the ball return to the start position */
    MKB(u8) g_some_model_flag; /* Changes the ball model to lower poly variants, also apparantly the Dole blimp? */
    MKB(undefined) field_0x17a[0x2];
    struct MKB(RaycastHit) raycast_down_hit; /* Result of raycast downward from ball's origin with the stage */
    MKB(undefined) field_0x198[0x18];
} __attribute__((__packed__));
static_assert(sizeof(MKB(Ball)) == 0x1b0);

typedef struct MKB(File) MKB(File), *MKB(PFile);

typedef struct MKB(FileCacheEntry) MKB(FileCacheEntry), *MKB(PFileCacheEntry);

struct MKB(FileCacheEntry) {
    MKB(s32) has_data;
    MKB(s32) entry_num;
    MKB(u32) aram_addr;
    MKB(u32) aram_size;
} __attribute__((__packed__));
static_assert(sizeof(MKB(FileCacheEntry)) == 0x10);

struct MKB(File) {
    MKB(BOOL32) is_cached;
    struct MKB(DVDFileInfo) dvd_file_info;
    struct MKB(FileCacheEntry) cache_entry;
} __attribute__((__packed__));
static_assert(sizeof(MKB(File)) == 0x50);

typedef struct MKB(SmWorldInfo) MKB(SmWorldInfo), *MKB(PSmWorldInfo);

typedef struct MKB(SmStageInfo) MKB(SmStageInfo), *MKB(PSmStageInfo);

struct MKB(SmWorldInfo) {
    MKB(s16) stage_count; /* Number of stages in world */
    MKB(undefined) field_0x2[0x2];
    struct MKB(SmStageInfo) * stages; /* List of infos for each stage in world */
} __attribute__((__packed__));
static_assert(sizeof(MKB(SmWorldInfo)) == 0x8);

struct MKB(SmStageInfo) { /* A list of 10 of these is used to define a world */
    MKB(s16) stage_id;
    MKB(s16) difficulty;
} __attribute__((__packed__));
static_assert(sizeof(MKB(SmStageInfo)) == 0x4);

enum {
    MKB(DMD_SCEN_1ST_INIT)=0,
    MKB(DMD_SCEN_RETURN_INIT)=1,
    MKB(DMD_SCEN_NEWGAME_INIT)=2,
    MKB(DMD_SCEN_NEWGAME_MAIN)=3,
    MKB(DMD_SCEN_LOADGAME_INIT)=4,
    MKB(DMD_SCEN_LOADGAME_MAIN)=5,
    MKB(DMD_SCEN_SCENEPLAY_INIT)=6,
    MKB(DMD_SCEN_SCENEPLAY_MAIN)=7,
    MKB(DMD_SCEN_SEL_WORLD_INIT)=8,
    MKB(DMD_SCEN_SEL_WORLD_NEXT)=9,
    MKB(DMD_SCEN_SEL_FLOOR_INIT)=10,
    MKB(DMD_SCEN_SEL_FLOOR_MAIN)=11,
    MKB(DMD_SCEN_GAME_INIT)=12,
    MKB(DMD_SCEN_GAME_MAIN)=13,
    MKB(DMD_SCEN_GAME_CLEAR_INIT)=14,
    MKB(DMD_SCEN_GAME_CLEAR_MAIN)=15,
    MKB(DMD_SCEN_GAME_OVER_INIT)=16,
    MKB(DMD_SCEN_GAME_OVER_MAIN)=17,
    MKB(DMD_SCEN_SELECT_INIT)=18,
    MKB(DMD_SCEN_SELECT_MAIN)=19,
    MKB(DMD_SCEN_ENTRY_INIT)=20,
    MKB(DMD_SCEN_ENTRY_MAIN)=21,
    MKB(DMD_SCEN_SCNPLAY_RETURN)=22,
    MKB(DMD_SCEN_SCNPLAY_RETURN_LOADING)=23,
    MKB(DMD_SCEN_LOADGAME_RESTORE)=24,
    MKB(DMD_SCEN_NAMEENTRY_INIT)=25,
    MKB(DMD_SCEN_NAMEENTRY_MAIN)=26
};
typedef undefined2 MKB(ScenMode);

typedef struct MKB(FontDefinition) MKB(FontDefinition), *MKB(PFontDefinition);

struct MKB(FontDefinition) {
    short g_aram_flag;
    byte width;
    byte height;
    int g_start_index;
    int g_end_index;
    char field5_0xc;
    MKB(undefined) field_0xd[0x3];
    float field9_0x10;
    float field10_0x14;
    float field11_0x18;
    float field12_0x1c;
    char field13_0x20;
    char field14_0x21;
    char field15_0x22;
    char field16_0x23;
    char * name;
    ushort g_some_size;
    MKB(undefined) field_0x2a[0x2];
    ushort * g_some_avtext_lookup; /* Some short that usually always begins with '8' */
    short * g_aram_char_lookup;
    float field23_0x34;
} __attribute__((__packed__));
static_assert(sizeof(MKB(FontDefinition)) == 0x38);

typedef struct MKB(Itemgroup) MKB(Itemgroup), *MKB(PItemgroup);

struct MKB(Itemgroup) { /* Contains the current animation-related state of each item group in a stage (each thing corresponding to a collision header in the stagedef) */
    dword playback_state; /* Corresponding to the switch playback type which is controlling the item group, see PlaybackState */
    MKB(s32) anim_frame;
    struct MKB(Vec) position;
    struct MKB(Vec) prev_position;
    struct MKB(S16Vec) rotation;
    struct MKB(S16Vec) prev_rotation;
    MKB(Mtx) transform;
    MKB(Mtx) prev_transform;
    MKB(undefined) field_0x8c[0x10];
    struct MKB(SeesawInfo) * seesaw_info;
} __attribute__((__packed__));
static_assert(sizeof(MKB(Itemgroup)) == 0xa0);

typedef struct MKB(GSoundGroupEntry) MKB(GSoundGroupEntry), *MKB(PGSoundGroupEntry);

struct MKB(GSoundGroupEntry) {
    undefined2 g_flags;
    MKB(undefined) field_0x2[0x2];
    undefined2 field3_0x4;
    MKB(undefined) field_0x6[0x2];
    undefined2 field6_0x8;
    MKB(undefined) field_0xa[0x2];
    char * g_name;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GSoundGroupEntry)) == 0x10);

typedef char MKB(CharPair)[2];

typedef struct MKB(StoryModeSaveFile) MKB(StoryModeSaveFile), *MKB(PStoryModeSaveFile);

struct MKB(StoryModeSaveFile) {
    MKB(undefined) field0_0x0[4];
    MKB(BOOL32) is_valid;
    char file_name[13];
    MKB(u8) current_world;
    MKB(u8) field4_0x16;
    MKB(undefined) field_0x17[0x1];
    MKB(u32) playtime_in_frames;
    MKB(u32) score;
    MKB(u32) bananas;
    MKB(u8) num_beaten_stages_in_current_world;
    MKB(u8) beaten_stage_indices[10];
    MKB(undefined) field11_0x2f[85];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StoryModeSaveFile)) == 0x84);

typedef struct MKB(BoneChannel) MKB(BoneChannel), *MKB(PBoneChannel);

struct MKB(BoneChannel) {
    MKB(u16) keyframe_count;
    MKB(u16) padding;
} __attribute__((__packed__));
static_assert(sizeof(MKB(BoneChannel)) == 0x4);

typedef struct MKB(BoneDef) MKB(BoneDef), *MKB(PBoneDef);

struct MKB(BoneDef) { /* Null-terminated bone name directly follows, followed by 4(?) byte-aligned next bone */
    short parent_bone_id;
    short flags;
    struct MKB(Quat) g_rotation1;
    struct MKB(Quat) g_rotation2;
    struct MKB(Vec) translation;
} __attribute__((__packed__));
static_assert(sizeof(MKB(BoneDef)) == 0x30);

typedef struct MKB(RankingEntry) MKB(RankingEntry), *MKB(PRankingEntry);

struct MKB(RankingEntry) {
    char * name;
    MKB(undefined) field_0x4[0x4];
    MKB(u32) score; /* Created by retype action */
    MKB(u32) g_ape_id;
    MKB(undefined) field_0x10[0x4];
    undefined4 field11_0x14;
    MKB(undefined) field_0x18[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(RankingEntry)) == 0x1c);

typedef struct MKB(GTableEntry) MKB(GTableEntry), *MKB(PGTableEntry);

struct MKB(GTableEntry) {
    MKB(undefined) field_0x0[0x3];
    byte field3_0x3;
    MKB(undefined) field_0x4[0x8];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GTableEntry)) == 0xc);

typedef struct MKB(BoneAnim) MKB(BoneAnim), *MKB(PBoneAnim);

struct MKB(BoneAnim) {
    ushort channels_bitfield;
    undefined2 field1_0x2;
} __attribute__((__packed__));
static_assert(sizeof(MKB(BoneAnim)) == 0x4);

typedef struct MKB(theme_light) MKB(theme_light), *MKB(Ptheme_light);

typedef short MKB(int16_t);

struct MKB(theme_light) { /* A struct used for each theme ID's lighting */
    float unk_float; /* Usually 0.6 */
    float light_group_r;
    float light_group_g;
    float light_group_b;
    float light_group_null;
    float unk_one_1;
    float unk_one_2;
    float unk_one_3;
    float unk_one_null;
    float unk_half_1;
    float unk_half_2;
    float unk_half_3;
    float unk_half_null;
    float light_param_r;
    float light_param_g;
    float light_param_b;
    MKB(int16_t) xa;
    MKB(int16_t) ya;
    float null;
} __attribute__((__packed__));
static_assert(sizeof(MKB(theme_light)) == 0x48);

typedef struct MKB(OptiGXChanSettings) MKB(OptiGXChanSettings), *MKB(POptiGXChanSettings);

typedef MKB(u8) MKB(GXBool);

enum {
    MKB(GX_SRC_REG)=0,
    MKB(GX_SRC_VTX)=1
};
typedef undefined4 MKB(GXColorSrc);

enum {
    MKB(GX_DF_NONE)=0,
    MKB(GX_DF_SIGN)=1,
    MKB(GX_DF_CLAMP)=2
};
typedef undefined4 MKB(GXDiffuseFn);

enum {
    MKB(GX_AF_SPEC)=0,
    MKB(GX_AF_SPOT)=1,
    MKB(GX_AF_NONE)=2
};
typedef undefined4 MKB(GXAttnFn);

struct MKB(OptiGXChanSettings) { /* Opti = For optimization */
    MKB(GXBool) enable;
    MKB(undefined) padding_0x1[0x3];
    MKB(GXColorSrc)  amb_src;
    MKB(GXColorSrc)  mat_src;
    MKB(u32) light_mask;
    MKB(GXDiffuseFn)  diff_fn;
    MKB(GXAttnFn)  attn_fn;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OptiGXChanSettings)) == 0x18);

typedef struct MKB(GSomeLightStruct) MKB(GSomeLightStruct), *MKB(PGSomeLightStruct);

struct MKB(GSomeLightStruct) {
    MKB(undefined) field_0x0[0x12b];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GSomeLightStruct)) == 0x12b);

typedef struct MKB(CmListEntry) MKB(CmListEntry), *MKB(PCmListEntry);

struct MKB(CmListEntry) {
    undefined4 field0_0x0;
    undefined4 g_stage_id;
    MKB(undefined) field_0x8[0x64];
} __attribute__((__packed__));
static_assert(sizeof(MKB(CmListEntry)) == 0x6c);

typedef struct MKB(NlBuffer) MKB(NlBuffer), *MKB(PNlBuffer);

struct MKB(NlBuffer) {
    int field0_0x0;
    MKB(undefined) field_0x4[0x14];
    int * * field21_0x18;
    MKB(undefined) field_0x1c[0x4];
    int * * field26_0x20;
    struct MKB(GXTexObj) * field27_0x24;
    MKB(undefined) field_0x28[0x10];
    int * * field44_0x38;
    MKB(undefined) field_0x3c[0x28];
    int * * field85_0x64;
} __attribute__((__packed__));
static_assert(sizeof(MKB(NlBuffer)) == 0x68);

typedef struct MKB(GraphicsInfo) MKB(GraphicsInfo), *MKB(PGraphicsInfo);

typedef struct MKB(GXFifoObj) MKB(GXFifoObj), *MKB(PGXFifoObj);

struct MKB(GraphicsInfo) { /* Is this the right size for the struct? maybe? -Crafted */
    void * active_framebuffer; /* A pointer to either framebuffer1 or framebuffer2 */
    void * framebuffer1;
    void * framebuffer2;
    MKB(u32) frame_count;
    MKB(u32) active_framebuffer_index; /* Either 0 or 1, representing framebuffer1 or framebuffer2 */
    struct MKB(GXFifoObj) * fifos[2];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GraphicsInfo)) == 0x1c);

struct MKB(GXFifoObj) {
    MKB(undefined) field_0x0[0x14];
    void * read_ptr;
    void * write_ptr;
    MKB(undefined) field_0x1c[0x64];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXFifoObj)) == 0x80);

enum {
    MKB(FONT32_ASCII)=0,
    MKB(FONT32_ASC_8x16)=1,
    MKB(FONT32_ASC_12x12)=2,
    MKB(FONT32_ASC_24x24)=3,
    MKB(FONT32_ASC_16x16P)=4,
    MKB(FONT32_ASC_16x16)=5,
    MKB(FONT32_DMY03)=6,
    MKB(FONT32_DMY04)=7,
    MKB(FONT32_DMY05)=8,
    MKB(FONT32_ASC_72x64)=9,
    MKB(FONT32_DMY06)=10,
    MKB(FONT32_DMY07)=11,
    MKB(FONT32_DMY08)=12,
    MKB(FONT32_DMY09)=13,
    MKB(FONT32_DMY10)=14,
    MKB(FONT32_DMY11)=15,
    MKB(FONT32_DMY12)=16,
    MKB(FONT32_DMY13)=17,
    MKB(FONT32_DMY14)=18,
    MKB(FONT32_DMY15)=19,
    MKB(FONT32_DMY16)=20,
    MKB(FONT32_ICON_SD)=21,
    MKB(FONT32_ICON_SD2)=22,
    MKB(FONT32_DMY20)=23,
    MKB(FONT32_DMY21)=24,
    MKB(FONT32_DMY22)=25,
    MKB(FONT32_DMY23)=26,
    MKB(FONT32_DMY_RNK00)=27,
    MKB(FONT32_DMY_RNK01)=28,
    MKB(FONT32_DMY_RNK02)=29,
    MKB(FONT32_DMY_RNK03)=30,
    MKB(FONT32_DMY_RNK04)=31,
    MKB(FONT32_RNK_32x32)=32,
    MKB(FONT32_RNK_NUM)=33,
    MKB(FONT32_MINI_RNK)=34,
    MKB(FONT32_SCORE_NUM)=35,
    MKB(FONT32_DMY_RNK0)=36,
    MKB(FONT32_DMY_RNK1)=37,
    MKB(FONT32_DMY_RNK2)=38,
    MKB(FONT32_DMY_RNK3)=39,
    MKB(FONT32_DMY_RNK4)=40,
    MKB(FONT32_DMY_RNK5)=41,
    MKB(FONT32_DMY_RNK6)=42,
    MKB(FONT32_DMY_RNK7)=43,
    MKB(FONT32_DMY_RNK8)=44,
    MKB(FONT32_DMY_RNK9)=45,
    MKB(FONT32_DMY_RNKa)=46,
    MKB(FONT32_DMY_RNKb)=47,
    MKB(FONT32_DMY_RNKc)=48,
    MKB(FONT32_DMY_RNKd)=49,
    MKB(FONT32_DMY_RNKe)=50,
    MKB(FONT32_DMY_RNKf)=51,
    MKB(FONT32_SEL_CTRL_PORT)=52,
    MKB(FONT32_SEL_CTRL_R)=53,
    MKB(FONT32_SEL_CTRL_B)=54,
    MKB(FONT32_SEL_CTRL_Y)=55,
    MKB(FONT32_SEL_CTRL_G)=56,
    MKB(FONT32_SEL_CTRL_SDW)=57,
    MKB(FONT32_DMY42)=58,
    MKB(FONT32_DMY43)=59,
    MKB(FONT32_BWL_SCORE)=60,
    MKB(FONT32_DMY50)=61,
    MKB(FONT32_DMY51)=62,
    MKB(FONT32_DMY52)=63,
    MKB(FONT32_DMY53)=64,
    MKB(FONT32_DMY54)=65,
    MKB(FONT32_DMY55)=66,
    MKB(FONT32_DMY56)=67,
    MKB(FONT32_DMY57)=68,
    MKB(FONT32_RAC_DMY01)=69,
    MKB(FONT32_RAC_PLAYER)=70,
    MKB(FONT32_RAC_RANK)=71,
    MKB(FONT32_RAC_LAP_MARK_NUM)=72,
    MKB(FONT32_RAC_LAP_NUM)=73,
    MKB(FONT32_RAC_TIME_NUM)=74,
    MKB(FONT32_RAC_LAP_MARK_NUM_S)=75,
    MKB(FONT32_RAC_DMY08)=76,
    MKB(FONT32_RAC_DMY09)=77,
    MKB(FONT32_RAC_DMY10)=78,
    MKB(FONT32_RAC_SPD_NUM)=79,
    MKB(FONT32_RAC_DMY12)=80,
    MKB(FONT32_RAC_DMY13)=81,
    MKB(FONT32_TGT_PLAYER)=82,
    MKB(FONT32_TGT_MSCORE)=83,
    MKB(FONT32_TGT_SCORE)=84,
    MKB(FONT32_TGT_SCORE_S)=85,
    MKB(FONT32_TGT_SPEED)=86,
    MKB(FONT32_TGT_ROUND)=87,
    MKB(FONT32_TGT_ALT)=88,
    MKB(FONT32_TGT_WIND)=89,
    MKB(FONT32_DMY71)=90,
    MKB(FONT32_DMY72)=91,
    MKB(FONT32_DMY73)=92,
    MKB(FONT32_NUM_NML_SCORE)=93,
    MKB(FONT32_NUM_NML_TIME)=94,
    MKB(FONT32_NUM_NML_TIME_S)=95,
    MKB(FONT32_NUM_NML_SPEED)=96,
    MKB(FONT32_NUM_NML_SARU)=97,
    MKB(FONT32_ASC_NML_D5)=98,
    MKB(FONT32_ASC_NML_D6)=99,
    MKB(FONT32_ASC_NML_D7)=100,
    MKB(FONT32_ASC_NML_D8)=101,
    MKB(FONT32_ASC_NML_D9)=102,
    MKB(FONT32_DMY79)=103,
    MKB(FONT32_DMY7a)=104,
    MKB(FONT32_DMY7b)=105,
    MKB(FONT32_DMY7c)=106,
    MKB(FONT32_DMY7d)=107,
    MKB(FONT32_DMY7e)=108,
    MKB(FONT32_DMY7f)=109,
    MKB(FONT32_BOA_TIME_NUM)=110,
    MKB(FONT32_BOA_DMY02)=111,
    MKB(FONT32_BOA_RANK)=112,
    MKB(FONT32_BOA_PLAYER)=113,
    MKB(FONT32_BOA_SPD_NUM)=114,
    MKB(FONT32_BOA_LAP_MARK_NUM)=115,
    MKB(FONT32_BOA_LAP_MARK_NUM_S)=116,
    MKB(FONT32_BOA_DMY08)=117,
    MKB(FONT32_BOA_DMY09)=118,
    MKB(FONT32_BOA_DMY10)=119,
    MKB(FONT32_BOA_LAP_NUM)=120,
    MKB(FONT32_BOA_DMY12)=121,
    MKB(FONT32_BOA_DMY13)=122,
    MKB(FONT32_BOA_DMY14)=123,
    MKB(FONT32_BOA_DMY15)=124,
    MKB(FONT32_BOA_DMY16)=125,
    MKB(FONT32_DOG_SPEED)=126,
    MKB(FONT32_DOG_SPEED_S)=127,
    MKB(FONT32_DOG_MSCORE)=128,
    MKB(FONT32_DOG_SCORE)=129,
    MKB(FONT32_DOG_SCORE_M)=130,
    MKB(FONT32_DOG_SCORE_S)=131,
    MKB(FONT32_DOG_ROUND)=132,
    MKB(FONT32_DOG_ALT)=133,
    MKB(FONT32_DOG_RESULT)=134,
    MKB(FONT32_DOG_TIME)=135,
    MKB(FONT32_DOG_DMY02)=136,
    MKB(FONT32_DOG_DMY03)=137,
    MKB(FONT32_DOG_DMY04)=138,
    MKB(FONT32_DOG_DMY05)=139,
    MKB(FONT32_JAP_TAG)=140,
    MKB(FONT32_JAP_DMY)=141,
    MKB(FONT32_JAP_24x24_2)=142,
    MKB(FONT32_JAP_24x24_2P)=143,
    MKB(FONT32_JAP_24x24_I)=144
};
typedef undefined4 MKB(Font32);

typedef struct MKB(g_someGameInfo) MKB(g_someGameInfo), *MKB(Pg_someGameInfo);

struct MKB(g_someGameInfo) {
    MKB(undefined) field_0x0[0x50];
} __attribute__((__packed__));
static_assert(sizeof(MKB(g_someGameInfo)) == 0x50);

typedef struct MKB(GSoundGroup) MKB(GSoundGroup), *MKB(PGSoundGroup);

struct MKB(GSoundGroup) { /* Has same length as GSoundGroupEntry and a fair bit of overlap - is this unioned with that? -Alice */
    MKB(undefined) field_0x0[0x4];
    char * field4_0x4;
    undefined2 field5_0x8;
    MKB(undefined) field_0xa[0x2];
    char * g_name;
    struct MKB(GSoundGroupEntry) field9_0x10[0]; /* vla? */
} __attribute__((__packed__));
static_assert(sizeof(MKB(GSoundGroup)) == 0x10);

typedef struct MKB(ScenInfo) MKB(ScenInfo), *MKB(PScenInfo);

struct MKB(ScenInfo) {
    MKB(ScenMode)  mode;
    MKB(ScenMode)  mode_request;
    undefined2 world;
    MKB(s16) world_stage;
    undefined4 flags;
    undefined2 next_world;
    MKB(undefined) field_0xe[2];
    undefined4 score;
    undefined4 bananas;
    undefined4 g_some_storymode_func_ptr;
    MKB(undefined) field_0x1c[6];
    undefined1 save_file_idx;
    MKB(undefined) field_0x23[1];
    undefined4 field19_0x24;
    undefined4 field20_0x28;
    undefined4 field21_0x2c;
    undefined4 field22_0x30;
    undefined4 field23_0x34;
} __attribute__((__packed__));
static_assert(sizeof(MKB(ScenInfo)) == 0x38);

enum {
    MKB(FONT16_ASCII)=0,
    MKB(FONT16_ASC_8x16)=1,
    MKB(FONT16_ASC_12x12)=2,
    MKB(FONT16_ASC_24x24)=3,
    MKB(FONT16_ASC_16x16P)=4,
    MKB(FONT16_ASC_16x16)=5,
    MKB(FONT16_DMY03)=6,
    MKB(FONT16_DMY04)=7,
    MKB(FONT16_DMY05)=8,
    MKB(FONT16_ASC_72x64)=9,
    MKB(FONT16_DMY06)=10,
    MKB(FONT16_DMY07)=11,
    MKB(FONT16_DMY08)=12,
    MKB(FONT16_DMY09)=13,
    MKB(FONT16_DMY10)=14,
    MKB(FONT16_DMY11)=15,
    MKB(FONT16_DMY12)=16,
    MKB(FONT16_DMY13)=17,
    MKB(FONT16_DMY14)=18,
    MKB(FONT16_DMY15)=19,
    MKB(FONT16_DMY16)=20,
    MKB(FONT16_ICON_SD)=21,
    MKB(FONT16_ICON_SD2)=22,
    MKB(FONT16_DMY20)=23,
    MKB(FONT16_DMY21)=24,
    MKB(FONT16_DMY22)=25,
    MKB(FONT16_DMY23)=26,
    MKB(FONT16_DMY_RNK00)=27,
    MKB(FONT16_DMY_RNK01)=28,
    MKB(FONT16_DMY_RNK02)=29,
    MKB(FONT16_DMY_RNK03)=30,
    MKB(FONT16_DMY_RNK04)=31,
    MKB(FONT16_RNK_32x32)=32,
    MKB(FONT16_RNK_NUM)=33,
    MKB(FONT16_MINI_RNK)=34,
    MKB(FONT16_SCORE_NUM)=35,
    MKB(FONT16_DMY_RNK0)=36,
    MKB(FONT16_DMY_RNK1)=37,
    MKB(FONT16_DMY_RNK2)=38,
    MKB(FONT16_DMY_RNK3)=39,
    MKB(FONT16_DMY_RNK4)=40,
    MKB(FONT16_DMY_RNK5)=41,
    MKB(FONT16_DMY_RNK6)=42,
    MKB(FONT16_DMY_RNK7)=43,
    MKB(FONT16_DMY_RNK8)=44,
    MKB(FONT16_DMY_RNK9)=45,
    MKB(FONT16_DMY_RNKa)=46,
    MKB(FONT16_DMY_RNKb)=47,
    MKB(FONT16_DMY_RNKc)=48,
    MKB(FONT16_DMY_RNKd)=49,
    MKB(FONT16_DMY_RNKe)=50,
    MKB(FONT16_DMY_RNKf)=51,
    MKB(FONT16_SEL_CTRL_PORT)=52,
    MKB(FONT16_SEL_CTRL_R)=53,
    MKB(FONT16_SEL_CTRL_B)=54,
    MKB(FONT16_SEL_CTRL_Y)=55,
    MKB(FONT16_SEL_CTRL_G)=56,
    MKB(FONT16_SEL_CTRL_SDW)=57,
    MKB(FONT16_DMY42)=58,
    MKB(FONT16_DMY43)=59,
    MKB(FONT16_BWL_SCORE)=60,
    MKB(FONT16_DMY50)=61,
    MKB(FONT16_DMY51)=62,
    MKB(FONT16_DMY52)=63,
    MKB(FONT16_DMY53)=64,
    MKB(FONT16_DMY54)=65,
    MKB(FONT16_DMY55)=66,
    MKB(FONT16_DMY56)=67,
    MKB(FONT16_DMY57)=68,
    MKB(FONT16_RAC_DMY01)=69,
    MKB(FONT16_RAC_PLAYER)=70,
    MKB(FONT16_RAC_RANK)=71,
    MKB(FONT16_RAC_LAP_MARK_NUM)=72,
    MKB(FONT16_RAC_LAP_NUM)=73,
    MKB(FONT16_RAC_TIME_NUM)=74,
    MKB(FONT16_RAC_LAP_MARK_NUM_S)=75,
    MKB(FONT16_RAC_DMY08)=76,
    MKB(FONT16_RAC_DMY09)=77,
    MKB(FONT16_RAC_DMY10)=78,
    MKB(FONT16_RAC_SPD_NUM)=79,
    MKB(FONT16_RAC_DMY12)=80,
    MKB(FONT16_RAC_DMY13)=81,
    MKB(FONT16_TGT_PLAYER)=82,
    MKB(FONT16_TGT_MSCORE)=83,
    MKB(FONT16_TGT_SCORE)=84,
    MKB(FONT16_TGT_SCORE_S)=85,
    MKB(FONT16_TGT_SPEED)=86,
    MKB(FONT16_TGT_ROUND)=87,
    MKB(FONT16_TGT_ALT)=88,
    MKB(FONT16_TGT_WIND)=89,
    MKB(FONT16_DMY71)=90,
    MKB(FONT16_DMY72)=91,
    MKB(FONT16_DMY73)=92,
    MKB(FONT16_NUM_NML_SCORE)=93,
    MKB(FONT16_NUM_NML_TIME)=94,
    MKB(FONT16_NUM_NML_TIME_S)=95,
    MKB(FONT16_NUM_NML_SPEED)=96,
    MKB(FONT16_NUM_NML_SARU)=97,
    MKB(FONT16_ASC_NML_D5)=98,
    MKB(FONT16_ASC_NML_D6)=99,
    MKB(FONT16_ASC_NML_D7)=100,
    MKB(FONT16_ASC_NML_D8)=101,
    MKB(FONT16_ASC_NML_D9)=102,
    MKB(FONT16_DMY79)=103,
    MKB(FONT16_DMY7a)=104,
    MKB(FONT16_DMY7b)=105,
    MKB(FONT16_DMY7c)=106,
    MKB(FONT16_DMY7d)=107,
    MKB(FONT16_DMY7e)=108,
    MKB(FONT16_DMY7f)=109,
    MKB(FONT16_BOA_TIME_NUM)=110,
    MKB(FONT16_BOA_DMY02)=111,
    MKB(FONT16_BOA_RANK)=112,
    MKB(FONT16_BOA_PLAYER)=113,
    MKB(FONT16_BOA_SPD_NUM)=114,
    MKB(FONT16_BOA_LAP_MARK_NUM)=115,
    MKB(FONT16_BOA_LAP_MARK_NUM_S)=116,
    MKB(FONT16_BOA_DMY08)=117,
    MKB(FONT16_BOA_DMY09)=118,
    MKB(FONT16_BOA_DMY10)=119,
    MKB(FONT16_BOA_LAP_NUM)=120,
    MKB(FONT16_BOA_DMY12)=121,
    MKB(FONT16_BOA_DMY13)=122,
    MKB(FONT16_BOA_DMY14)=123,
    MKB(FONT16_BOA_DMY15)=124,
    MKB(FONT16_BOA_DMY16)=125,
    MKB(FONT16_DOG_SPEED)=126,
    MKB(FONT16_DOG_SPEED_S)=127,
    MKB(FONT16_DOG_MSCORE)=128,
    MKB(FONT16_DOG_SCORE)=129,
    MKB(FONT16_DOG_SCORE_M)=130,
    MKB(FONT16_DOG_SCORE_S)=131,
    MKB(FONT16_DOG_ROUND)=132,
    MKB(FONT16_DOG_ALT)=133,
    MKB(FONT16_DOG_RESULT)=134,
    MKB(FONT16_DOG_TIME)=135,
    MKB(FONT16_DOG_DMY02)=136,
    MKB(FONT16_DOG_DMY03)=137,
    MKB(FONT16_DOG_DMY04)=138,
    MKB(FONT16_DOG_DMY05)=139,
    MKB(FONT16_JAP_TAG)=140,
    MKB(FONT16_JAP_DMY)=141,
    MKB(FONT16_JAP_24x24_2)=142,
    MKB(FONT16_JAP_24x24_2P)=143,
    MKB(FONT16_JAP_24x24_I)=144
};
typedef undefined2 MKB(Font16);

typedef struct MKB(MemCardInfo) MKB(MemCardInfo), *MKB(PMemCardInfo);

struct MKB(MemCardInfo) { /* Some struct that seems to hold per-memcard info; there are two statically allocated, one for each memory card slot (I think) */
    byte chan; /* Channel aka slot, 0 -> slot A, 1 -> slot B */
    byte field1_0x1;
    byte field2_0x2;
    MKB(undefined) field_0x3[0x5];
    word field8_0x8;
    word field9_0xa;
    dword field10_0xc;
    MKB(undefined) field_0x10[0x14];
    dword field31_0x24;
    MKB(undefined) field_0x28[0x8];
    dword field40_0x30;
    dword field41_0x34;
    dword field42_0x38;
    dword field43_0x3c;
    dword field44_0x40;
} __attribute__((__packed__));
static_assert(sizeof(MKB(MemCardInfo)) == 0x44);

typedef struct MKB(PadStatusGroup) MKB(PadStatusGroup), *MKB(PPadStatusGroup);

typedef struct MKB(PADStatus) MKB(PADStatus), *MKB(PPADStatus);

struct MKB(PADStatus) {
    MKB(PadDigitalInput)  button; /* Or-ed PAD_BUTTON_* and PAD_TRIGGER_* bits */
    MKB(s8) stickX; /* -128 <= stickX       <= 127 */
    MKB(s8) stickY; /* -128 <= stickY       <= 127 */
    MKB(s8) substickX; /* -128 <= substickX    <= 127 */
    MKB(s8) substickY; /* -128 <= substickY    <= 127 */
    MKB(u8) triggerLeft; /*    0 <= triggerLeft  <= 255 */
    MKB(u8) triggerRight; /*    0 <= triggerRight <= 255 */
    MKB(u8) analogA; /*    0 <= analogA      <= 255 */
    MKB(u8) analogB; /*    0 <= analogB      <= 255 */
    MKB(PadError)  err; /* one of PAD_ERR_* number */
    MKB(undefined) field_0xb[0x1];
} __attribute__((__packed__));
static_assert(sizeof(MKB(PADStatus)) == 0xc);

struct MKB(PadStatusGroup) { /* A set of PADStatus structs for a given controller, with each representing a different "filtered" version of the inputs */
    struct MKB(PADStatus) raw; /* The raw PADStatus read by PADRead() for the controller */
    struct MKB(PADStatus) prev_tick; /* The PADStatus from the previous frame */
    struct MKB(PADStatus) pressed; /* PADStatus representing the digital inputs that were just pressed this frame */
    struct MKB(PADStatus) released; /* PADStatus representing the digital inputs that were released this frame */
    struct MKB(PADStatus) repeated; /* PADStatus of digital inputs that "repeat" similar to holding a key down on a keyboard: bit is high on first press, then there's a pause of many frames, then the bit is high every 4 frames */
} __attribute__((__packed__));
static_assert(sizeof(MKB(PadStatusGroup)) == 0x3c);

enum { /* Some flags that get set during render loops, perhaps? */
    MKB(RENDERFLAG_NONE)=0,
    MKB(RENDERFLAG_DISP)=8,
    MKB(RENDERFLAG_MINIMAP)=1024
};
typedef undefined4 MKB(GRenderringFlags);

typedef struct MKB(AnalogInputGroup) MKB(AnalogInputGroup), *MKB(PAnalogInputGroup);

enum { /* Thresholded analog input bits used by SMB2 */
    MKB(PAI_LSTICK_LEFT)=1,
    MKB(PAI_LSTICK_RIGHT)=2,
    MKB(PAI_LSTICK_DOWN)=4,
    MKB(PAI_LSTICK_UP)=8,
    MKB(PAI_CSTICK_LEFT)=16,
    MKB(PAI_CSTICK_RIGHT)=32,
    MKB(PAI_CSTICK_DOWN)=64,
    MKB(PAI_CSTICK_UP)=128,
    MKB(PAI_LTRIG)=256,
    MKB(PAI_RTRIG)=512
};
typedef undefined2 MKB(PadAnalogInput);

struct MKB(AnalogInputGroup) { /* Consolidated thresholded analog input bitfields corresponding to the five groups of PadStatusGroup */
    MKB(PadAnalogInput)  raw;
    MKB(PadAnalogInput)  prev_tick;
    MKB(PadAnalogInput)  pressed;
    MKB(PadAnalogInput)  released;
    MKB(PadAnalogInput)  repeated;
} __attribute__((__packed__));
static_assert(sizeof(MKB(AnalogInputGroup)) == 0xa);

typedef struct MKB(CARDStat) MKB(CARDStat), *MKB(PCARDStat);

struct MKB(CARDStat) {
    char fileName[32];
    MKB(u32) length;
    MKB(u32) time;
    MKB(u8) gameName[4];
    MKB(u8) company[2];
    MKB(u8) bannerFormat;
    MKB(u8) field6_0x2f;
    MKB(u32) iconAddr;
    MKB(u16) iconFormat;
    MKB(u16) iconSpeed;
    MKB(u32) commentAddr;
    MKB(u32) offsetBanner;
    MKB(u32) offsetBannerTlut;
    MKB(u32) offsetIcon[8];
    MKB(u32) offsetIconTlut;
    MKB(u32) offsetData;
} __attribute__((__packed__));
static_assert(sizeof(MKB(CARDStat)) == 0x6c);

enum {
    MKB(NORMAL)=0,
    MKB(WIDESCREEN)=1,
    MKB(LETTERBOX)=2
};
typedef undefined1 MKB(WidescreenMode);

typedef struct MKB(SkeletonAnim) MKB(SkeletonAnim), *MKB(PSkeletonAnim);

struct MKB(SkeletonAnim) {
    short g_unk1;
    short bone_count;
} __attribute__((__packed__));
static_assert(sizeof(MKB(SkeletonAnim)) == 0x4);

typedef struct MKB(MemCardFile) MKB(MemCardFile), *MKB(PMemCardFile);

typedef struct MKB(CARDFileInfo) MKB(CARDFileInfo), *MKB(PCARDFileInfo);

struct MKB(CARDFileInfo) {
    MKB(s32) chan;
    MKB(s32) fileNo;
    MKB(s32) offset;
    MKB(s32) length;
    MKB(u16) iBlock;
} __attribute__((__packed__));
static_assert(sizeof(MKB(CARDFileInfo)) == 0x12);

struct MKB(MemCardFile) {
    MKB(s32) exi_channel_number;
    MKB(undefined) field_0x4[0xc];
    struct MKB(CARDFileInfo) gc_file_info; /* Created by retype action */
    MKB(undefined) field_0x22[0x2];
    char * file_name; /* Struct may be bigger? /shrug */
} __attribute__((__packed__));
static_assert(sizeof(MKB(MemCardFile)) == 0x28);

typedef struct MKB(ModeInfo) MKB(ModeInfo), *MKB(PModeInfo);

enum {
    MKB(BALLMODE_NONE)=0,
    MKB(BALLMODE_GOALED)=1,
    MKB(BALLMODE_OUT_OF_TIME_RINGOUT)=2,
    MKB(BALLMODE_FALLEN_OUT)=4,
    MKB(BALLMODE_FREEZE_TIMER)=8,
    MKB(BALLMODE_IN_REPLAY)=16,
    MKB(BALLMODE_IN_GOAL_ANIMATION)=32,
    MKB(BALLMODE_ON_BONUS_STAGE)=64,
    MKB(BALLMODE_IN_STAGE_LOADIN)=256,
    MKB(BALLMODE_CLEARED_BONUS_PERFECT)=512,
    MKB(BALLMODE_CLEARED_BONUS_BONUSFINISH_UNUSED)=1024,
    MKB(BALLMODE_IN_TUTORIAL_SEQUENCE)=2048,
    MKB(BALLMODE_ON_FINAL_STAGE)=4096,
    MKB(BALLMODE_OUT_OF_TIME_CONTINUE)=8192
};
typedef undefined4 MKB(G_BallMode);

enum {
    MKB(Blue)=0,
    MKB(Green)=1,
    MKB(Red)=2
};
typedef undefined1 MKB(GoalType);

struct MKB(ModeInfo) { /* I don't know what to call this, but there's some important global game info in here! -Complex */
    MKB(G_BallMode)  ball_mode; /* Correlates with the ball's 'mode' in the debug menu's ball display. Bonus stages have 0x40 set, final stages in a difficulty have 0x1000 set.  0x8 seems to stop the timer? -Crafted */
    MKB(s16) stage_time_frames_remaining;
    undefined2 stage_time_limit;
    undefined4 field3_0x8;
    MKB(s16) entered_goal_idx;
    undefined2 entered_goal_itemgroup_idx;
    struct MKB(Vec) g_ballVelAtGoal;
    undefined2 g_some_timer_frame_remaining_count;
    MKB(s16) attempt_count;
    undefined2 cm_course_stage_num; /* Current course stage num, updated immediately after completing stage */
    undefined2 g_some_stage_jump_distance;
    undefined4 bananas_remaining;
    MKB(u16) death_count;
    undefined2 continues_used;
    undefined2 g_next_stage_id2;
    MKB(s16) cm_stage_id; /* Current challenge mode stage id, updated immediately after finishing stage */
    undefined2 field16_0x30;
    undefined2 cm_next_stage_id;
    MKB(s16) g_selected_world_idx;
    undefined2 g_selected_world_stage_idx;
    MKB(GoalType)  entered_goal_type;
    MKB(undefined) field_0x39[0x3];
} __attribute__((__packed__));
static_assert(sizeof(MKB(ModeInfo)) == 0x3c);

typedef struct MKB(Event) MKB(Event), *MKB(PEvent);

struct MKB(Event) { /* A subset of continuously running game functionality that can be turned on or off independent of other Events */
    MKB(Status)  status;
    MKB(undefined) padding_0x1[0x3];
    char * name; /* The name of the event */
    void (* init_func)(void); /* Normally, called once when status is STAT_INIT, then status is set to STAT_NORMAL */
    void (* tick_func)(void); /* Normally, called every frame if status is STAT_NORMAL */
    void (* dest_func)(void); /* "destruct:" or "destroy" function; normally, called once when status is STAT_DEST, then status is set to STAT_NULL */
    MKB(u32) tick_time; /* For performance metrics (presumably this is what's seen in the debug overlay performance > event menu) */
} __attribute__((__packed__));
static_assert(sizeof(MKB(Event)) == 0x18);

typedef unsigned short    wchar16;
enum {
    MKB(EVENT_STAGE)=0,
    MKB(EVENT_WORLD)=1,
    MKB(EVENT_BALL)=2,
    MKB(EVENT_APE)=3,
    MKB(EVENT_STOBJ)=4,
    MKB(EVENT_ITEM)=5,
    MKB(EVENT_RECPLAY)=6,
    MKB(EVENT_OBJ_COLLISION)=7,
    MKB(EVENT_NAME_ENTRY)=8,
    MKB(EVENT_INFO)=9,
    MKB(EVENT_COURSE)=10,
    MKB(EVENT_VIBRATION)=11,
    MKB(EVENT_COMMEND)=12,
    MKB(EVENT_VIEW)=13,
    MKB(EVENT_EFFECT)=14,
    MKB(EVENT_MINIMAP)=15,
    MKB(EVENT_CAMERA)=16,
    MKB(EVENT_SPRITE)=17,
    MKB(EVENT_MOUSE)=18,
    MKB(EVENT_SOUND)=19,
    MKB(EVENT_BACKGROUND)=20,
    MKB(EVENT_REND_EFC)=21,
    MKB(EVENT_ADX)=22,
    MKB(EVENT_NONE)=4294967295
};
typedef undefined4 MKB(EventID);

enum {
    MKB(BANANA_SINGLE)=0,
    MKB(BANANA_BUNCH)=1
};
typedef undefined4 MKB(BananaType);

enum {
    MKB(DATA_SELECT_MENU)=0,
    MKB(NAME_ENTRY_MENU)=1,
    MKB(STAGE_SELECT_MENU)=2,
    MKB(RETURN_TO_MAIN_MENU)=3
};
typedef undefined1 MKB(StoryModeMenuState);

typedef struct MKB(__OutStrCtrl) MKB(__OutStrCtrl), *MKB(P__OutStrCtrl);

typedef ulong MKB(size_t);

struct MKB(__OutStrCtrl) {
    char * CharStr;
    MKB(size_t) MaxCharCount;
    MKB(size_t) CharsWritten;
} __attribute__((__packed__));
static_assert(sizeof(MKB(__OutStrCtrl)) == 0xc);

typedef struct MKB(OptiGXSettings) MKB(OptiGXSettings), *MKB(POptiGXSettings);

struct MKB(OptiGXSettings) { /* Opti = For optimization - I don't actually know how big this struct is -Crafted */
    MKB(undefined) field_0x0[0xc30];
    struct MKB(OptiGXChanSettings) chan_color0;
    struct MKB(OptiGXChanSettings) chan_color1;
    struct MKB(OptiGXChanSettings) chan_alpha0;
    struct MKB(OptiGXChanSettings) chan_alpha1;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OptiGXSettings)) == 0xc90);

typedef struct MKB(S32Vec) MKB(S32Vec), *MKB(PS32Vec);

struct MKB(S32Vec) {
    MKB(s32) x;
    MKB(s32) y;
} __attribute__((__packed__));
static_assert(sizeof(MKB(S32Vec)) == 0x8);

typedef struct MKB(StoryStagePreviewParam) MKB(StoryStagePreviewParam), *MKB(PStoryStagePreviewParam);

struct MKB(StoryStagePreviewParam) { /* Used for each stage preview on the Story Mode Stage Select screen */
    MKB(s16) stage_id; /* Only used when generating previews through Debug Mode */
    undefined2 field0x2; /* Seems to usually be 0x1000 or 0x1800 */
    float scale; /* Usually 1 with the exception of Entangled Path */
} __attribute__((__packed__));
static_assert(sizeof(MKB(StoryStagePreviewParam)) == 0x8);

enum {
    MKB(OF_G_SMTH_WITH_CAMERA)=2,
    MKB(OF_GAME_PAUSED)=8
};
typedef undefined4 MKB(GOtherFlags);

typedef struct MKB(Map) MKB(Map), *MKB(PMap);

struct MKB(Map) {
    int size;
    undefined4 field1_0x4;
    MKB(undefined) field_0x8[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(Map)) == 0xc);

typedef struct MKB(gDiscQueueItem) MKB(gDiscQueueItem), *MKB(PgDiscQueueItem);

struct MKB(gDiscQueueItem) {
    int status;
    int entrynum;
    undefined4 group;
} __attribute__((__packed__));
static_assert(sizeof(MKB(gDiscQueueItem)) == 0xc);

enum {
    MKB(MINIMAP_HIDDEN)=0,
    MKB(MINIMAP_SHRINK)=1,
    MKB(MINIMAP_SHOWN)=2,
    MKB(MINIMAP_EXPAND)=3,
    MKB(MINIMAP_INIT)=4
};
typedef undefined4 MKB(MinimapMode);

typedef longlong MKB(OSTime);

typedef struct MKB(DipSwitchesOld) MKB(DipSwitchesOld), *MKB(PDipSwitchesOld);

struct MKB(DipSwitchesOld) { /* Unused struct - maybe I'll use it once Ghidra supports bitfields a bit nicer in the decompilation */
    MKB(u32) DIP_DEBUG:1; /* Enables debug mode */
    MKB(u32) DIP_DISP:1;
    MKB(u32) DIP_STCOLI:1; /* Displays the level's collision and out of bounds areas */
    MKB(u32) DIP_TRIANGLE:1; /* Replaces the level model with a triangle */
    MKB(u32) DIP_TAIKEN:1;
    MKB(u32) DIP_TIME_STOP:1; /* Freezes the timer at 60 seconds */
    MKB(u32) DIP_NAMEENTRY:1;
    MKB(u32) DIP_FIX_WORLD:1; /* Disables stage tilting */
    MKB(u32) DIP_TEST_CAM:1; /* Free cam during gameplay */
    MKB(u32) DIP_NO_INTR:1;
    MKB(u32) DIP_CAPTURE:1;
    MKB(u32) DIP_PERF_ALWAYS:1; /* Related to the SDK's PERF profiling */
    MKB(u32) DIP_PLAY_STG_ALL:1; /* Unlock all */
    MKB(u32) DIP_PLAY_PNT_x10:1; /* 10x play point gain */
    MKB(u32) DIP_SARU_0:1;
    MKB(u32) DIP_SWITCH15:1;
    MKB(u32) DIP_ONLY24MB:1;
    MKB(u32) DIP_SHADOW_DISP:1; /* Display object shadows on the UI */
    MKB(u32) DIP_SPRITE_OFF:1; /* Hide most 2D graphics */
    MKB(u32) DIP_SNDREQ_DISP:1;
    MKB(u32) DIP_SE_VOL_RESET:1; /* Disable most sound effects */
    MKB(u32) DIP_APE_FACEDIR:1; /* Shows what the monkey is looking at */
    MKB(u32) DIP_SEL_ALL_CTRL:1; /* Control all 4 players at once with 1 controller */
    MKB(u32) DIP_APE_NUMBER:1; /* Display character id above each monkey's head */
    MKB(u32) DIP_1P_TEST_CAM:1;
    MKB(u32) DIP_PREVIEW_SAVE:1; /* Display a preview in the upper left of the screen */
    MKB(u32) DIP_AUTHOR_DEBUG:1; /* Show debug info during cutscenes */
    MKB(u32) DIP_SWITCH27:1;
    MKB(u32) DIP_SOA:1;
    MKB(u32) DIP_TEST_TEAM:1;
    MKB(u32) DIP_NO_MINIMAP:1; /* Hide minimap */
    MKB(u32) DIP_NO_STAGE:1; /* Hide stage */
} __attribute__((__packed__));
static_assert(sizeof(MKB(DipSwitchesOld)) == 0x4);

enum {
    MKB(LOCALE_ENGLISH)=0,
    MKB(LOCALE_GERMAN)=1,
    MKB(LOCALE_FRENCH)=2,
    MKB(LOCALE_SPANISH)=3,
    MKB(LOCALE_ITALIAN)=4,
    MKB(LOCALE_JAPANESE)=5
};
typedef undefined4 MKB(Locale);

enum { /* Abbreviated "ET" in the game */
    MKB(EFFECT_PAPERFRAG)=0,
    MKB(EFFECT_GET_BANANA)=1,
    MKB(EFFECT_COLI_PARTICLE)=2,
    MKB(EFFECT_HOLDING_BANANA)=3,
    MKB(EFFECT_LEVITATE)=4,
    MKB(EFFECT_COLISTAR_PARTICLE)=5,
    MKB(EFFECT_BGWAT_BUBBLE_BASE)=6,
    MKB(EFFECT_BGWAT_BUBBLE)=7,
    MKB(EFFECT_METEO)=8,
    MKB(EFFECT_METEO_FIX)=9,
    MKB(EFFECT_COLIFLASH)=10,
    MKB(EFFECT_BNS_STG_STAR)=11,
    MKB(EFFECT_BNS_STG_STAR_TAIL)=12,
    MKB(EFFECT_BGMST_GEN_CLOUD)=13,
    MKB(EFFECT_BGSTM_RAINRIPPLE)=14,
    MKB(EFFECT_NAMEENT_CODE)=15,
    MKB(EFFECT_GET_NAMEENT_CODE)=16,
    MKB(EFFECT_BGMST_WATER)=17,
    MKB(EFFECT_COMMENDFRAG)=18,
    MKB(EFFECT_ENDING_BALLFRAG)=19,
    MKB(EFFECT_BANANA_DROP)=20,
    MKB(EFFECT_BGEND_WATER)=21,
    MKB(EFFECT_BGBTMFIRE_PTCL)=22,
    MKB(EFFECT_PIYO)=23,
    MKB(EFFECT_DUMMY1)=24,
    MKB(EFFECT_DUMMY2)=25,
    MKB(EFFECT_DUMMY3)=26,
    MKB(EFFECT_DUMMY4)=27,
    MKB(EFFECT_DUMMY5)=28,
    MKB(EFFECT_DUMMY6)=29,
    MKB(EFFECT_DUMMY7)=30,
    MKB(EFFECT_DUMMY8)=31,
    MKB(EFFECT_DUMMY9)=32,
    MKB(EFFECT_DUMMYA)=33,
    MKB(EFFECT_DUMMYB)=34,
    MKB(EFFECT_DUMMYC)=35,
    MKB(EFFECT_DUMMYD)=36,
    MKB(EFFECT_DUMMYE)=37,
    MKB(EFFECT_DUMMYF)=38,
    MKB(EFFECT_TMP001)=39,
    MKB(EFFECT_TMP002)=40,
    MKB(EFFECT_TMP003)=41,
    MKB(EFFECT_TMP004)=42,
    MKB(EFFECT_TMP005)=43,
    MKB(EFFECT_TMP006)=44,
    MKB(EFFECT_TMP007)=45,
    MKB(EFFECT_TMP008)=46,
    MKB(EFFECT_TMP009)=47,
    MKB(EFFECT_TMP010)=48,
    MKB(EFFECT_TMP011)=49,
    MKB(EFFECT_TMP012)=50,
    MKB(EFFECT_TMP013)=51,
    MKB(EFFECT_TMP014)=52,
    MKB(EFFECT_TMP015)=53,
    MKB(EFFECT_TMP016)=54,
    MKB(EFFECT_TMP017)=55,
    MKB(EFFECT_TMP018)=56,
    MKB(EFFECT_TMP019)=57,
    MKB(EFFECT_TMP020)=58,
    MKB(EFFECT_TMP021)=59,
    MKB(EFFECT_TMP022)=60,
    MKB(EFFECT_TMP023)=61,
    MKB(EFFECT_TMP024)=62,
    MKB(EFFECT_TMP025)=63,
    MKB(EFFECT_TMP026)=64,
    MKB(EFFECT_TMP027)=65,
    MKB(EFFECT_TMP028)=66,
    MKB(EFFECT_TMP029)=67,
    MKB(EFFECT_TMP030)=68,
    MKB(EFFECT_TMP031)=69,
    MKB(EFFECT_TMP032)=70
};
typedef undefined2 MKB(EffectType);

typedef struct MKB(GApeAnim) MKB(GApeAnim), *MKB(PGApeAnim);

typedef struct MKB(g_thing) MKB(g_thing), *MKB(Pg_thing);

struct MKB(g_thing) {
    MKB(undefined) field_0x0[0x8];
    char * Name;
    MKB(undefined) field_0xc[0x1c];
} __attribute__((__packed__));
static_assert(sizeof(MKB(g_thing)) == 0x28);

struct MKB(GApeAnim) { /* Unknown length -Crafted */
    MKB(u8) field0_0x0;
    MKB(undefined) field_0x1[0x3];
    struct MKB(g_thing) * count;
    struct MKB(GApeAnim) * ape;
    MKB(s32) * field6_0xc; /* Ptr to some struct? */
    MKB(undefined) field_0x10[0xc];
    MKB(s32) g_either_motion_or_skel2;
    MKB(undefined) field_0x20[0x8];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GApeAnim)) == 0x28);

enum {
    MKB(GX_USAGE_NONE)=0,
    MKB(GX_USAGE_LOCKED_CACHE_ENABLED)=2,
    MKB(GX_USAGE_HAS_SECOND_FIFO)=4
};
typedef undefined4 MKB(GxUsageState);

enum {
    MKB(DSMS_DEFAULT)=0,
    MKB(DSMS_CHOOSE_DATA_TO_COPY)=1,
    MKB(DSMS_CHOOSE_WHERE_TO_COPY_TO)=2,
    MKB(DSMS_CONFIRM_COPY)=3,
    MKB(DSMS_CANCELLED_COPY)=4,
    MKB(DSMS_COPY_DATA)=5,
    MKB(DSMS_CHOOSE_DATA_TO_DELETE)=6,
    MKB(DSMS_CONFIRM_DELETE)=7,
    MKB(DSMS_CANCELLED_DELETE)=8,
    MKB(DSMS_DELETE_DATA)=9,
    MKB(DSMS_OPEN_DATA)=10,
    MKB(DSMS_RETURN_SELECTED)=11,
    MKB(DSMS_RETURN_TO_MAIN_GAME_SELECT)=12
};
typedef undefined1 MKB(DataSelectMenuState);

enum {
    MKB(EFFECT_NULL)=0,
    MKB(EFFECT_CAST_SHADOW)=1,
    MKB(EFFECT_RECEIVE_SHADOW)=2,
    MKB(EFFECT_UNKNOWN_3)=4,
    MKB(EFFECT_TRANSPARENCY_A)=8,
    MKB(EFFECT_TRANSPARENCY_B)=16,
    MKB(EFFECT_UNKNOWN_6)=32,
    MKB(EFFECT_UNKNOWN_7)=64,
    MKB(EFFECT_UNKNOWN_8)=128
};
typedef undefined4 MKB(StageModelEffectBitfield);

typedef struct MKB(Effect) MKB(Effect), *MKB(PEffect);

struct MKB(Effect) {
    word g_idx;
    short id;
    MKB(undefined) field_0x4[0x4];
    MKB(EffectType)  type;
    MKB(undefined) field_0xa[0x2];
    MKB(s32) field9_0xc;
    MKB(undefined) field_0x10[0x4];
    undefined2 g_ball_idx;
    MKB(s16) field15_0x16;
    MKB(undefined) field_0x18[0xc];
    struct MKB(Vec) g_scale;
    MKB(s32) g_pointer_to_some_struct;
    struct MKB(Vec) g_pos;
    struct MKB(Vec) g_some_vec;
    struct MKB(S16Vec) g_some_rot;
    MKB(undefined) field_0x52[0x6];
    struct MKB(Vec) g_prev_pos;
    MKB(undefined) field_0x64[0x28];
    struct MKB(Vec) g_some_vec2;
    struct MKB(Vec) g_some_vec3;
    MKB(undefined) field_0xa4[0xc];
} __attribute__((__packed__));
static_assert(sizeof(MKB(Effect)) == 0xb0);

enum {
    MKB(PMT_UNKNOWN0)=0,
    MKB(PMT_CHALLENGE)=1,
    MKB(PMT_PRACTICE)=2,
    MKB(PMT_UNKNOWN3)=3,
    MKB(PMT_BILLIARDS)=4,
    MKB(PMT_STORY_STAGE_SELECT)=5,
    MKB(PMT_STORY_PLAY)=6,
    MKB(PMT_UNKNOWN7)=7
};
typedef undefined4 MKB(PauseMenuType);

typedef struct MKB(ShadowReceive) MKB(ShadowReceive), *MKB(PShadowReceive);

struct MKB(ShadowReceive) {
    void * ptr1;
    MKB(u16) val1;
    MKB(u16) val2;
    void * ptr2;
} __attribute__((__packed__));
static_assert(sizeof(MKB(ShadowReceive)) == 0xc);

typedef struct MKB(ytgut) MKB(ytgut), *MKB(Pytgut);

struct MKB(ytgut) {
    MKB(undefined) field_0x0[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(ytgut)) == 0x4);

typedef struct MKB(GDialogStruct) MKB(GDialogStruct), *MKB(PGDialogStruct);

struct MKB(GDialogStruct) {
    undefined4 g_test_dialog_ptr;
    MKB(undefined) field_0x4[0x138];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GDialogStruct)) == 0x13c);

typedef struct MKB(PoolInfo) MKB(PoolInfo), *MKB(PPoolInfo);

struct MKB(PoolInfo) { /* Metadata and status info for lists of "tickable" objects like the sprite, effect, stobj, and item lists */
    dword len; /* The size of the object pool, aka the maximum number of non-empty objects the pool can hold */
    dword low_free_idx; /* The last known "low index" of an empty object in the pool. Reset at the start of the frame to the lowest free idx in the list, and increments as objects are allocated */
    dword upper_bound; /* Number of objects from the start of the list until the last non-empty object. Usually the list is iterated over from 0 to this value, checking each tickable if it's non-empty. Reset at the start of each frame to one past the last non-empty object in the pool, and increased if the low idx bumps into it */
    MKB(u8) * status_list; /* Byte array of same length as the pool, each byte corresponds to some status of the corresponding object. Usually either 0 for "empty slot" and either 1 or 2 for "active". This does not appear to be a Status like Events use. */
} __attribute__((__packed__));
static_assert(sizeof(MKB(PoolInfo)) == 0x10);

typedef struct MKB(SelMenuInfo) MKB(SelMenuInfo), *MKB(PSelMenuInfo);

struct MKB(SelMenuInfo) {
    MKB(u8) menu_stack_ptr;
    MKB(u8) menu_stack[10];
    MKB(undefined) field_0xb[0x3];
    undefined2 field5_0xe;
} __attribute__((__packed__));
static_assert(sizeof(MKB(SelMenuInfo)) == 0x10);

typedef struct MKB(SmWorldState) MKB(SmWorldState), *MKB(PSmWorldState);

struct MKB(SmWorldState) {
    MKB(u16) flags;
    MKB(undefined) field_0x2[0xe];
    MKB(u32) stages[10];
} __attribute__((__packed__));
static_assert(sizeof(MKB(SmWorldState)) == 0x38);

enum {
    MKB(PIID_LEFT)=0,
    MKB(PIID_RIGHT)=1,
    MKB(PIID_UP)=2,
    MKB(PIID_DOWN)=3,
    MKB(PIID_L)=4,
    MKB(PIID_R)=5,
    MKB(PIID_A)=6,
    MKB(PIID_B)=7,
    MKB(PIID_X)=8
};
typedef undefined4 MKB(PadInputID);

enum {
    MKB(PLAYBACK_FORWARD)=0,
    MKB(PLAYBACK_PAUSE)=1,
    MKB(PLAYBACK_BACKWARD)=2,
    MKB(PLAYBACK_FAST_FORWARD)=3,
    MKB(PLAYBACK_FAST_BACKWARD)=4
};
typedef undefined2 MKB(PlaybackState);

typedef struct MKB(OrdTblData) MKB(OrdTblData), *MKB(POrdTblData);

struct MKB(OrdTblData) {
    MKB(Mtx) matrices[5];
    struct MKB(OrdTblNode) * entries;
    MKB(s32) max_entries;
    float depth_offset;
    float min_depth;
    float max_depth;
    struct MKB(OrdTblNode) * last_entry;
    struct MKB(OrdTblNode) * first_entry;
    float depth_range;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OrdTblData)) == 0x110);

enum { /* Options for rendering text with font */
    MKB(TEXTDRAW_FLAG_BORDER)=536870912,
    MKB(TEXTDRAW_FLAG_DROP_SHADOW)=1073741824,
    MKB(TEXTDRAW_FLAG_PROPORTIONAL)=2147483648
};
typedef undefined4 MKB(TextDrawFlags);

typedef void MKB(_IO_lock_t);

typedef struct MKB(_IO_marker) MKB(_IO_marker), *MKB(P_IO_marker);

typedef struct MKB(_IO_FILE) MKB(_IO_FILE), *MKB(P_IO_FILE);

typedef long MKB(__off_t);

struct MKB(_IO_marker) {
    struct MKB(_IO_marker) * _next;
    struct MKB(_IO_FILE) * _sbuf;
    int _pos;
} __attribute__((__packed__));
static_assert(sizeof(MKB(_IO_marker)) == 0xc);

struct MKB(_IO_FILE) {
    int _flags;
    char * _IO_read_ptr;
    char * _IO_read_end;
    char * _IO_read_base;
    char * _IO_write_base;
    char * _IO_write_ptr;
    char * _IO_write_end;
    char * _IO_buf_base;
    char * _IO_buf_end;
    char * _IO_save_base;
    char * _IO_backup_base;
    char * _IO_save_end;
    struct MKB(_IO_marker) * _markers;
    struct MKB(_IO_FILE) * _chain;
    int _fileno;
    int _flags2;
    MKB(__off_t) _old_offset;
    ushort _cur_column;
    char _vtable_offset;
    char _shortbuf[1];
    MKB(_IO_lock_t) * _lock;
    void * __pad1;
    void * __pad2;
    void * __pad3;
    void * __pad4;
    MKB(size_t) __pad5;
    int _mode;
    char _unused2[15];
    MKB(undefined) padding_0x73[0x1];
} __attribute__((__packed__));
static_assert(sizeof(MKB(_IO_FILE)) == 0x74);

typedef double MKB(f64);

enum {
    MKB(__WORDSIZE)=32
};
typedef undefined8 MKB(define___WORDSIZE);

enum {
    MKB(__GLIBC_HAVE_LONG_LONG)=1
};
typedef undefined8 MKB(define___GLIBC_HAVE_LONG_LONG);

typedef struct MKB(_IO_FILE) MKB(__FILE);

typedef struct MKB(_IO_FILE) MKB(FILE);

typedef signed char MKB(int8_t);

typedef int MKB(bool_t);

typedef int MKB(int32_t);

typedef struct MKB(g_some_card_struct) MKB(g_some_card_struct), *MKB(Pg_some_card_struct);

typedef struct MKB(OSThreadQueue) MKB(OSThreadQueue), *MKB(POSThreadQueue);

typedef struct MKB(OSThread) MKB(OSThread), *MKB(POSThread);

typedef struct MKB(OSContext) MKB(OSContext), *MKB(POSContext);

typedef MKB(s32) MKB(OSPriority);

typedef struct MKB(OSThreadLink) MKB(OSThreadLink), *MKB(POSThreadLink);

typedef struct MKB(OSMutex) MKB(OSMutex), *MKB(POSMutex);

typedef struct MKB(OSMutexQueue) MKB(OSMutexQueue), *MKB(POSMutexQueue);

typedef struct MKB(OSMutexLink) MKB(OSMutexLink), *MKB(POSMutexLink);

struct MKB(OSContext) {
    MKB(u32) gpr[32]; /* General-purpose registers */
    MKB(u32) cr;
    MKB(u32) lr;
    MKB(u32) ctr;
    MKB(u32) xer;
    MKB(f64) fpr[32]; /* Floating-point registers */
    MKB(u32) fpscr_pad;
    MKB(u32) fpscr;
    MKB(u32) ssr0; /* Exception handling registers */
    MKB(u32) ssr1;
    MKB(u16) mode; /* Context mode - since UIMM is 16 bits in PPC */
    MKB(u16) state; /* OR-ed OS_CONTEXT_STATE_* */
    MKB(u32) gqr[8]; /* Place Gekko regs at the end so we have minimal changes to existing code */
    MKB(f64) psf[32];
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSContext)) == 0x2c4);

struct MKB(OSThreadQueue) {
    struct MKB(OSThread) * head;
    struct MKB(OSThread) * tail;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSThreadQueue)) == 0x8);

struct MKB(OSMutexQueue) {
    struct MKB(OSMutex) * head;
    struct MKB(OSMutex) * tail;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSMutexQueue)) == 0x8);

struct MKB(OSThreadLink) {
    struct MKB(OSThread) * next;
    struct MKB(OSThread) * prev;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSThreadLink)) == 0x8);

struct MKB(OSThread) {
    struct MKB(OSContext) context; /* register context */
    MKB(u16) state; /* OS_THREAD_STATE_* */
    MKB(u16) attr; /* OS_THREAD_ATTR_* */
    MKB(s32) suspend; /* suspended if the count is greater than zero */
    MKB(OSPriority) priority; /* effective scheduling priority */
    MKB(OSPriority) base; /* base scheduling priority */
    void * val; /* exit value */
    struct MKB(OSThreadQueue) * queue; /* queue thread is on */
    struct MKB(OSThreadLink) link; /* queue link */
    struct MKB(OSThreadQueue) * queueJoin; /* list of threads waiting for termination (join) */
    struct MKB(OSMutex) * mutex; /* mutex trying to lock */
    struct MKB(OSMutexQueue) queueMutex; /* list of mutexes owned */
    struct MKB(OSThreadLink) linkActive; /* list of all threads for debugging */
    MKB(u8) * stackBase; /* the thread's designated stack (high address) */
    MKB(u32) * stackEnd; /* last word of stack (low address) */
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSThread)) == 0x304);

struct MKB(OSMutexLink) {
    struct MKB(OSMutex) * next;
    struct MKB(OSMutex) * prev;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSMutexLink)) == 0x8);

struct MKB(OSMutex) {
    struct MKB(OSThreadQueue) queue;
    struct MKB(OSThread) * thread; /* the current owner */
    MKB(s32) count; /* lock count */
    struct MKB(OSMutexLink) link; /* for OSThread.queueMutex */
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSMutex)) == 0x18);

struct MKB(g_some_card_struct) {
    MKB(undefined) field_0x0[0x4];
    undefined4 field4_0x4;
    MKB(undefined) field_0x8[0x84];
    struct MKB(OSThreadQueue) field137_0x8c;
    MKB(undefined) field_0x94[0x4c];
    undefined4 field214_0xe0;
    MKB(undefined) field_0xe4[0x2c];
} __attribute__((__packed__));
static_assert(sizeof(MKB(g_some_card_struct)) == 0x110);

typedef struct MKB(gSceneData) MKB(gSceneData), *MKB(PgSceneData);

struct MKB(gSceneData) {
    char world_theme;
    char g_anim_flag;
    char ape_count;
    MKB(undefined) field_0x3[0x5];
    char LOD;
    char unknown;
    char scene_item_count;
    MKB(undefined) field_0xb[0x1];
    char * * field12_0xc;
} __attribute__((__packed__));
static_assert(sizeof(MKB(gSceneData)) == 0x10);

typedef struct MKB(StagedefBackgroundAnimHeader) MKB(StagedefBackgroundAnimHeader), *MKB(PStagedefBackgroundAnimHeader);

typedef struct MKB(StagedefAnimKeyframe) MKB(StagedefAnimKeyframe), *MKB(PStagedefAnimKeyframe);

enum {
    MKB(EASING_CONSTANT)=0,
    MKB(EASING_LINEAR)=1,
    MKB(EASING_SMOOTH)=2
};
typedef undefined4 MKB(Easing);

struct MKB(StagedefBackgroundAnimHeader) {
    MKB(undefined) field_0x0[0x4];
    float loop_point_seconds;
    MKB(undefined) field_0x8[0x8];
    MKB(u32) rot_x_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_x_keyframe_list;
    MKB(u32) rot_y_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_y_keyframe_list;
    MKB(u32) rot_z_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_z_keyframe_list;
    MKB(u32) pos_x_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_x_keyframe_list;
    MKB(u32) pos_y_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_y_keyframe_list;
    MKB(u32) pos_z_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_z_keyframe_list;
    MKB(undefined) field_0x40[0x10];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefBackgroundAnimHeader)) == 0x50);

struct MKB(StagedefAnimKeyframe) {
    MKB(Easing)  easing;
    float time;
    float value;
    float tangent_in;
    float tangent_out;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefAnimKeyframe)) == 0x14);

typedef struct MKB(StagedefEffectHeader) MKB(StagedefEffectHeader), *MKB(PStagedefEffectHeader);

typedef struct MKB(StagedefEffect1) MKB(StagedefEffect1), *MKB(PStagedefEffect1);

typedef struct MKB(StagedefEffect2) MKB(StagedefEffect2), *MKB(PStagedefEffect2);

typedef struct MKB(StagedefTextureScroll) MKB(StagedefTextureScroll), *MKB(PStagedefTextureScroll);

struct MKB(StagedefEffectHeader) {
    MKB(u32) fx1_keyframe_count;
    struct MKB(StagedefEffect1) * fx1_keyframe_list;
    MKB(u32) fx2_keyframe_count;
    struct MKB(StagedefEffect2) * fx2_keyframe_list;
    struct MKB(StagedefTextureScroll) * texture_scroll;
    MKB(undefined) field_0x14[0x1c];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefEffectHeader)) == 0x30);

struct MKB(StagedefEffect1) { /* Likely used for animated textures, such as the silhouettes in Night */
    dword field0_0x0;
    dword field1_0x4;
    dword field2_0x8;
    word field3_0xc;
    word field4_0xe;
    word field5_0x10;
    MKB(undefined) field_0x12[0x2];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefEffect1)) == 0x14);

struct MKB(StagedefEffect2) { /* May be used for the flames in Storm? */
    float field0_0x0;
    float field1_0x4;
    float field2_0x8;
    MKB(undefined) field_0xc[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefEffect2)) == 0x10);

struct MKB(StagedefTextureScroll) {
    struct MKB(Vec2d) speed;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefTextureScroll)) == 0x8);

typedef struct MKB(StagedefFogAnimHeader) MKB(StagedefFogAnimHeader), *MKB(PStagedefFogAnimHeader);

struct MKB(StagedefFogAnimHeader) {
    MKB(u32) start_distance_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * start_distance_keyframe_list;
    MKB(u32) end_distance_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * end_distance_keyframe_list;
    MKB(u32) red_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * red_keyframe_list;
    MKB(u32) green_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * green_keyframe_list;
    MKB(u32) blue_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * blue_keyframe_list;
    MKB(u32) unk_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * unk_keyframe_list;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefFogAnimHeader)) == 0x30);

typedef struct MKB(StagedefAnimHeader) MKB(StagedefAnimHeader), *MKB(PStagedefAnimHeader);

struct MKB(StagedefAnimHeader) {
    MKB(u32) rot_x_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_x_keyframe_list;
    MKB(u32) rot_y_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_y_keyframe_list;
    MKB(u32) rot_z_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_z_keyframe_list;
    MKB(u32) pos_x_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_x_keyframe_list;
    MKB(u32) pos_y_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_y_keyframe_list;
    MKB(u32) pos_z_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_z_keyframe_list;
    MKB(undefined) field_0x30[0x10];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefAnimHeader)) == 0x40);

typedef struct MKB(StagedefFileHeader) MKB(StagedefFileHeader), *MKB(PStagedefFileHeader);

typedef struct MKB(StagedefColiHeader) MKB(StagedefColiHeader), *MKB(PStagedefColiHeader);

typedef struct MKB(StagedefStart) MKB(StagedefStart), *MKB(PStagedefStart);

typedef struct MKB(StagedefFallout) MKB(StagedefFallout), *MKB(PStagedefFallout);

typedef struct MKB(StagedefGoal) MKB(StagedefGoal), *MKB(PStagedefGoal);

typedef struct MKB(StagedefBumper) MKB(StagedefBumper), *MKB(PStagedefBumper);

typedef struct MKB(StagedefJamabar) MKB(StagedefJamabar), *MKB(PStagedefJamabar);

typedef struct MKB(StagedefBanana) MKB(StagedefBanana), *MKB(PStagedefBanana);

typedef struct MKB(StagedefColiCone) MKB(StagedefColiCone), *MKB(PStagedefColiCone);

typedef struct MKB(StagedefColiSphere) MKB(StagedefColiSphere), *MKB(PStagedefColiSphere);

typedef struct MKB(StagedefColiCylinder) MKB(StagedefColiCylinder), *MKB(PStagedefColiCylinder);

typedef struct MKB(StagedefFalloutVolume) MKB(StagedefFalloutVolume), *MKB(PStagedefFalloutVolume);

typedef struct MKB(StagedefBackgroundModel) MKB(StagedefBackgroundModel), *MKB(PStagedefBackgroundModel);

typedef struct MKB(StagedefForegroundModel) MKB(StagedefForegroundModel), *MKB(PStagedefForegroundModel);

typedef struct MKB(StagedefReflectiveStageModel) MKB(StagedefReflectiveStageModel), *MKB(PStagedefReflectiveStageModel);

typedef struct MKB(StagedefStageModelInstance) MKB(StagedefStageModelInstance), *MKB(PStagedefStageModelInstance);

typedef struct MKB(StagedefStageModelPtrA) MKB(StagedefStageModelPtrA), *MKB(PStagedefStageModelPtrA);

typedef struct MKB(StagedefStageModelPtrB) MKB(StagedefStageModelPtrB), *MKB(PStagedefStageModelPtrB);

typedef struct MKB(StagedefButton) MKB(StagedefButton), *MKB(PStagedefButton);

typedef struct MKB(StagedefWormhole) MKB(StagedefWormhole), *MKB(PStagedefWormhole);

typedef struct MKB(StagedefFog) MKB(StagedefFog), *MKB(PStagedefFog);

typedef struct MKB(StagedefDynamicReflectionPlane) MKB(StagedefDynamicReflectionPlane), *MKB(PStagedefDynamicReflectionPlane);

typedef struct MKB(StagedefMystery3) MKB(StagedefMystery3), *MKB(PStagedefMystery3);

enum {
    MKB(ANIM_LOOP)=0,
    MKB(ANIM_PLAY_ONCE)=1,
    MKB(ANIM_SEESAW)=2
};
typedef undefined2 MKB(StagedefAnimType);

typedef struct MKB(StagedefColiTri) MKB(StagedefColiTri), *MKB(PStagedefColiTri);

typedef struct MKB(GStagedefBoundSphere) MKB(GStagedefBoundSphere), *MKB(PGStagedefBoundSphere);

typedef struct MKB(StagedefBackgroundAnim2Header) MKB(StagedefBackgroundAnim2Header), *MKB(PStagedefBackgroundAnim2Header);

typedef struct MKB(StagedefStageModel) MKB(StagedefStageModel), *MKB(PStagedefStageModel);

enum {
    MKB(GX_FOG_NONE)=0,
    MKB(GX_FOG_LIN)=2,
    MKB(GX_FOX_EXP)=4,
    MKB(GX_FOG_EXP2)=5,
    MKB(GX_FOG_REVEXP)=6,
    MKB(GX_FOG_REVEXP2)=7
};
typedef undefined1 MKB(GXFogType);

struct MKB(StagedefStageModelInstance) {
    struct MKB(StagedefStageModelPtrA) * stage_model_a;
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    undefined2 g_not_padding;
    struct MKB(Vec) scale;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefStageModelInstance)) == 0x24);

struct MKB(StagedefBumper) {
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    undefined2 padding;
    struct MKB(Vec) scale;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefBumper)) == 0x20);

struct MKB(StagedefReflectiveStageModel) {
    char * model_name;
    struct MKB(GmaModel) * g_model_header_ptr;
    MKB(undefined) field_0x8[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefReflectiveStageModel)) == 0xc);

struct MKB(StagedefFallout) {
    float y;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefFallout)) == 0x4);

struct MKB(StagedefButton) {
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    MKB(PlaybackState)  playback_state;
    MKB(u16) anim_group_id;
    undefined2 padding;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefButton)) == 0x18);

struct MKB(StagedefDynamicReflectionPlane) {
    char * model_name_ptr;
    struct MKB(Vec) pos;
    struct MKB(S16Vec) rot;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefDynamicReflectionPlane)) == 0x16);

struct MKB(StagedefStageModel) {
    MKB(undefined) field_0x0[0x4];
    char * model_name;
    uint some_flag;
    MKB(undefined) field_0xc[0x4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefStageModel)) == 0x10);

struct MKB(StagedefColiSphere) {
    struct MKB(Vec) position;
    float radius;
    undefined4 g_not_padding; /* Nullable */
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefColiSphere)) == 0x14);

struct MKB(StagedefColiCylinder) {
    struct MKB(Vec) position;
    float radius;
    float height;
    struct MKB(S16Vec) rotation;
    undefined2 g_not_padding;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefColiCylinder)) == 0x1c);

struct MKB(StagedefBackgroundModel) {
    uint g_model_flag;
    char * model_name;
    struct MKB(GmaModel) * gma_model; /* Created by retype action */
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    undefined2 padding;
    struct MKB(Vec) scale;
    struct MKB(StagedefBackgroundAnimHeader) * background_anim_header;
    struct MKB(StagedefBackgroundAnim2Header) * background_anim2_header;
    struct MKB(StagedefEffectHeader) * effect_header;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefBackgroundModel)) == 0x38);

struct MKB(StagedefStageModelPtrA) {
    MKB(StageModelEffectBitfield)  some_effect_bitflag;
    uint some_counter;
    struct MKB(StagedefStageModel) * stage_model;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefStageModelPtrA)) == 0xc);

struct MKB(StagedefStageModelPtrB) {
    struct MKB(StagedefStageModelPtrA) * stage_model_a;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefStageModelPtrB)) == 0x4);

struct MKB(StagedefBanana) {
    struct MKB(Vec) position;
    MKB(BananaType)  type;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefBanana)) == 0x10);

struct MKB(StagedefColiCone) {
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    undefined2 g_not_padding;
    struct MKB(Vec) scale;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefColiCone)) == 0x20);

struct MKB(StagedefJamabar) {
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    undefined2 padding;
    struct MKB(Vec) scale;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefJamabar)) == 0x20);

struct MKB(StagedefMystery3) {
    float field0_0x0;
    float field1_0x4;
    float field2_0x8;
    undefined2 field3_0xc;
    undefined2 field4_0xe;
    MKB(undefined) field_0x10[0x14];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefMystery3)) == 0x24);

struct MKB(StagedefFalloutVolume) {
    struct MKB(Vec) position;
    struct MKB(Vec) size;
    struct MKB(S16Vec) rotation;
    undefined2 padding;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefFalloutVolume)) == 0x20);

struct MKB(StagedefColiTri) {
    struct MKB(Vec) vert1;
    struct MKB(Vec) normal;
    struct MKB(S16Vec) rot_from_xy;
    undefined2 flags;
    struct MKB(Vec2d) vert2_delta; /* Before rotation is applied */
    struct MKB(Vec2d) vert3_delta;
    struct MKB(Vec2d) tangent;
    struct MKB(Vec2d) bitangent;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefColiTri)) == 0x40);

struct MKB(GStagedefBoundSphere) {
    MKB(undefined) field_0x0[0x4];
    float field4_0x4;
    float field5_0x8;
    float field6_0xc;
    float field7_0x10;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GStagedefBoundSphere)) == 0x14);

struct MKB(StagedefGoal) {
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    MKB(GoalType)  type;
    MKB(undefined) field_0x13[0x1];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefGoal)) == 0x14);

struct MKB(StagedefWormhole) {
    MKB(undefined) field_0x0[0x4];
    struct MKB(Vec) positon;
    struct MKB(S16Vec) rotation;
    undefined2 padding;
    struct MKB(StagedefWormhole) * destination;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefWormhole)) == 0x1c);

struct MKB(StagedefBackgroundAnim2Header) {
    MKB(undefined) field_0x0[0x4];
    float loop_point_seconds;
    MKB(u32) unk1_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * unk1_keyframe_list;
    MKB(u32) unk2_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * unk2_keyframe_list;
    MKB(u32) rot_x_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_x_keyframe_list;
    MKB(u32) rot_y_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_y_keyframe_list;
    MKB(u32) rot_z_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * rot_z_keyframe_list;
    MKB(u32) pos_x_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_x_keyframe_list;
    MKB(u32) pos_y_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_y_keyframe_list;
    MKB(u32) pos_z_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * pos_z_keyframe_list;
    MKB(u32) unk9_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * unk9_keyframe_list;
    MKB(u32) unk10_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * unk10_keyframe_list;
    MKB(u32) unk11_keyframe_count;
    struct MKB(StagedefAnimKeyframe) * unk11_keyframe_list;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefBackgroundAnim2Header)) == 0x60);

struct MKB(StagedefStart) {
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    undefined2 padding;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefStart)) == 0x14);

struct MKB(StagedefFileHeader) {
    MKB(u32) magic_number_a;
    MKB(u32) magic_number_b;
    MKB(u32) coli_header_count;
    struct MKB(StagedefColiHeader) * coli_header_list;
    struct MKB(StagedefStart) * start;
    struct MKB(StagedefFallout) * fallout;
    MKB(u32) goal_count;
    struct MKB(StagedefGoal) * goal_list;
    MKB(u32) bumper_count;
    struct MKB(StagedefBumper) * bumper_list;
    MKB(u32) jamabar_count;
    struct MKB(StagedefJamabar) * jamabar_list;
    MKB(u32) banana_count;
    struct MKB(StagedefBanana) * banana_list;
    MKB(u32) coli_cone_count;
    struct MKB(StagedefColiCone) * coli_cone_list;
    MKB(u32) coli_sphere_count;
    struct MKB(StagedefColiSphere) * coli_sphere_list;
    MKB(u32) coli_cylinder_count;
    struct MKB(StagedefColiCylinder) * coli_cylinder_list;
    MKB(u32) fallout_volume_count;
    struct MKB(StagedefFalloutVolume) * fallout_volume_list;
    MKB(u32) background_model_count;
    struct MKB(StagedefBackgroundModel) * background_model_list;
    MKB(u32) foreground_model_count;
    struct MKB(StagedefForegroundModel) * foreground_model_list;
    dword field26_0x68;
    dword field27_0x6c;
    MKB(u32) reflective_stage_model_count;
    struct MKB(StagedefReflectiveStageModel) * reflective_stage_model_list;
    MKB(undefined) field_0x78[0xc];
    MKB(u32) stage_model_instance_count;
    struct MKB(StagedefStageModelInstance) * stage_model_instance_list;
    MKB(u32) stage_model_a_count;
    struct MKB(StagedefStageModelPtrA) * stage_model_a_list;
    MKB(u32) stage_model_b_count;
    struct MKB(StagedefStageModelPtrB) * stage_model_b_list;
    MKB(undefined) field_0x9c[0xc];
    MKB(u32) button_count;
    struct MKB(StagedefButton) * button_list;
    struct MKB(StagedefFogAnimHeader) * fog_animation; /* Nullable */
    MKB(u32) wormhole_count;
    struct MKB(StagedefWormhole) * wormhole_list;
    struct MKB(StagedefFog) * fog; /* Nullable */
    MKB(undefined) field_0xc0[0x8];
    struct MKB(StagedefDynamicReflectionPlane) * dynamic_reflection_plane;
    MKB(undefined) field_0xcc[0x8];
    struct MKB(StagedefMystery3) * mystery3;
    MKB(undefined) field_0xd8[0x7c4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefFileHeader)) == 0x89c);

struct MKB(StagedefFog) {
    MKB(GXFogType)  type;
    MKB(undefined) field_0x1[0x3];
    float fog_start_distance;
    float fog_end_distance;
    struct MKB(Vec) color;
    MKB(undefined) field_0x18[0xc];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefFog)) == 0x24);

struct MKB(StagedefForegroundModel) {
    undefined4 field0_0x0; /* Unknown - typically 0x0000001F, sometimes 0x00000007 or 0x0000000F */
    char * model_name;
    struct MKB(GmaModel) * gma_model;
    struct MKB(Vec) position;
    struct MKB(S16Vec) rotation;
    undefined2 padding;
    struct MKB(Vec) scale;
    undefined4 field7_0x2c;
    void * background_anim2_header;
    void * field9_0x34;
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefForegroundModel)) == 0x38);

struct MKB(StagedefColiHeader) {
    struct MKB(Vec) origin; /* Center of rotation etc. */
    struct MKB(S16Vec) initial_rotation;
    MKB(StagedefAnimType)  anim_loop_type_and_seesaw;
    struct MKB(StagedefAnimHeader) * animation_header;
    struct MKB(Vec) conveyor_speed;
    struct MKB(StagedefColiTri) * coli_tri_list;
    MKB(s16) * * coli_tri_idxs;
    MKB(f32) coli_grid_start_x;
    MKB(f32) coli_grid_start_z;
    MKB(f32) coli_grid_step_x;
    MKB(f32) coli_grid_step_z;
    MKB(u32) coli_grid_cells_x;
    MKB(u32) coli_grid_cells_z;
    MKB(u32) goal_count;
    struct MKB(StagedefGoal) * goal_list;
    MKB(u32) bumper_count;
    struct MKB(StagedefBumper) * bumper_list;
    MKB(u32) jamabar_count;
    struct MKB(StagedefJamabar) * jamabar_list;
    MKB(u32) banana_count;
    struct MKB(StagedefBanana) * banana_list;
    MKB(u32) cone_collision_object_count;
    struct MKB(StagedefColiCone) * cone_collision_object_list;
    MKB(u32) sphere_collision_object_count;
    struct MKB(StagedefColiSphere) * sphere_collision_object_list;
    MKB(u32) cylinder_collision_object_count;
    struct MKB(StagedefColiCylinder) * cylinder_collision_object_list;
    MKB(u32) fallout_volume_count;
    struct MKB(StagedefFalloutVolume) * fallout_volume_list;
    MKB(u32) reflective_stage_model_count;
    struct MKB(StagedefReflectiveStageModel) * reflective_stage_model_list;
    MKB(u32) stage_model_instance_count;
    struct MKB(StagedefStageModelInstance) * stage_model_instance_list;
    MKB(u32) stage_model_b_count;
    struct MKB(StagedefStageModelPtrB) * stage_model_b_list;
    MKB(undefined) field_0x9c[0x8];
    MKB(u16) anim_group_id; /* For use with buttons - Must be non-zero if you want to assign a group ID */
    undefined2 g_not_padding;
    MKB(u32) button_count;
    struct MKB(StagedefButton) * button_list;
    MKB(undefined) field_0xb0[0x4];
    struct MKB(GStagedefBoundSphere) * g_bound_sphere;
    float seesaw_sensitivity; /* Higher is more sensitive, negative makes the seesaw reversed */
    float seesaw_friction; /* Lower is looser */
    float seesaw_spring; /* 0 prevents the seesaw from resetting */
    MKB(u32) wormhole_count;
    struct MKB(StagedefWormhole) * wormhole_list;
    MKB(u32) initial_playback_state; /* Should this be split into 2x padding bytes + PlaybackState enum? */
    float loop_start_sec;
    float loop_end_sec;
    struct MKB(StagedefTextureScroll) * texture_scroll;
    MKB(undefined) field_0xdc[0x3c0];
} __attribute__((__packed__));
static_assert(sizeof(MKB(StagedefColiHeader)) == 0x49c);

typedef void * MKB(__gnuc_va_list);

typedef MKB(__gnuc_va_list) MKB(va_list);

typedef struct MKB(GmaShape) MKB(GmaShape), *MKB(PGmaShape);

struct MKB(GmaShape) {
    uint g_some_bitflag;
    short g_some_short;
    short g_some_short2;
    short g_some_short3;
    short g_some_short4;
    byte g_some_flag;
    byte g_some_flag2;
    byte g_some_flag3;
    MKB(undefined) field_0xf[0x2];
    byte g_some_flag4;
    byte g_some_flag5;
    byte g_some_flag6;
    MKB(undefined) field_0x14[0x2];
    ushort g_some_ushort;
    MKB(undefined) field_0x18[0x4];
    uint g_some_uint4;
    MKB(undefined) field_0x20[0x8];
    int g_some_int;
    int g_some_int2;
    MKB(undefined) field_0x30[0xc];
    float g_some_float;
    uint g_some_uint5;
    MKB(undefined) field_0x44[0x1c];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GmaShape)) == 0x60);

typedef struct MKB(GmaTevLayer) MKB(GmaTevLayer), *MKB(PGmaTevLayer);

enum {
    MKB(TEXFLAG_UNK_BIT0)=1,
    MKB(TEXFLAG_UNK_BIT1)=2,
    MKB(TEXFLAG_REPEAT_U)=4,
    MKB(TEXFLAG_MIRROR_U)=8,
    MKB(TEXFLAG_REPEAT_V)=16,
    MKB(TEXFLAG_MIRROR_V)=32,
    MKB(TEXFLAG_UNK_BIT6)=64,
    MKB(TEXFLAG_UNK_BIT7)=128,
    MKB(TEXFLAG_ENABLE_MIPMAPS)=256,
    MKB(TEXFLAG_UNK_BIT9)=512,
    MKB(TEXFLAG_UNK_BIT10)=1024,
    MKB(TEXFLAG_G_NEAR)=2048,
    MKB(TEXFLAG_G_HEIGHTMAP_BLEND_GRAYSCALE)=4096,
    MKB(TEXFLAG_G_ALPHA_MASK)=8192,
    MKB(TEXFLAG_UNK_BIT14)=16384,
    MKB(TEXFLAG_UNK_BIT15)=32768,
    MKB(TEXFLAG_UNK_BIT16)=65536,
    MKB(TEXFLAG_ENABLE_UV_SCROLL)=131072,
    MKB(TEXFLAG_UNK_BIT18)=262144,
    MKB(TEXFLAG_UNK_BIT19)=524288,
    MKB(TEXFLAG_UNK_BIT20)=1048576,
    MKB(TEXFLAG_UNK_BIT21)=2097152,
    MKB(TEXFLAG_UNK_BIT22)=4194304,
    MKB(TEXFLAG_UNK_BIT23)=8388608
};
typedef undefined4 MKB(GmaTevLayerFlags);

enum {
    MKB(GX_ANISO_1)=0,
    MKB(GX_ANISO_2)=1,
    MKB(GX_ANISO_4)=2,
    MKB(GX_MAX_ANISOTROPY)=3
};
typedef undefined1 MKB(GXAnisotropy);

struct MKB(GmaTevLayer) {
    MKB(GmaTevLayerFlags)  flags;
    MKB(u16) tpl_texture_idx; /* Index into TPL for which texture is referenced */
    MKB(undefined) field_0x6[0x1];
    MKB(GXAnisotropy)  anisotropy; /* Level of anisotropic filtering to use */
    struct MKB(GXTexObj) * texobj;
    byte unk_0xC;
    byte g_is_swappable_texture; /* Boolean that indicates this texture may be swapped out at runtime. This is used for lap-related textures in F-Zero GX. */
    MKB(s16) tex_descriptor_idx; /* Texture descriptor index, matches its zero-indexed value in this array */
    MKB(undefined) field_0x10[0x10];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GmaTevLayer)) == 0x20);

typedef struct MKB(GmaVertexControlHeader) MKB(GmaVertexControlHeader), *MKB(PGmaVertexControlHeader);

struct MKB(GmaVertexControlHeader) { /* Also called "Model Type 1". This structure appears to outline various information relating to skinned and effective models only. A such, it is assumed that it is data for controlling the vertices. */
    MKB(s32) vertex_count;
    MKB(s32) vertex_control_type1_array_offset;
    MKB(s32) vertex_control_type2_array_offset;
    MKB(s32) vertex_control_type3_offset;
    MKB(s32) vertex_control_type4_offset;
    dword field5_0x14;
    MKB(undefined) field_0x18[0x8];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GmaVertexControlHeader)) == 0x20);

typedef struct MKB(Gma) MKB(Gma), *MKB(PGma);

struct MKB(Gma) { /* First 8 bytes of a GMA file. Following this is an arbitrary-length list of 8-byte GMA model entries */
    MKB(s32) model_count; /* The number of GCMF models in the file (including null entries) */
    MKB(s32) header_size; /* Size of header including FIFO padding before GCMF models begin. Also called "model base position" */
} __attribute__((__packed__));
static_assert(sizeof(MKB(Gma)) == 0x8);

typedef struct MKB(OSSectionInfo) MKB(OSSectionInfo), *MKB(POSSectionInfo);

struct MKB(OSSectionInfo) {
    MKB(u32) offset; /* Bit 31 is whether the section is executable */
    MKB(u32) size;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSSectionInfo)) == 0x8);

enum {
    MKB(GX_NONE)=0,
    MKB(GX_DIRECT)=1,
    MKB(GX_INDEX8)=2,
    MKB(GX_INDEX16)=3
};
typedef undefined4 MKB(GXAttrType);

enum {
    MKB(GX_VCACHE_MISS_ALL)=0,
    MKB(GX_VCACHE_MISS_POS)=1,
    MKB(GX_VCACHE_MISS_NRM)=2
};
typedef undefined4 MKB(GXEvent);

enum {
    MKB(GX_BM_NONE)=0,
    MKB(GX_BM_BLEND)=1,
    MKB(GX_BM_LOGIC)=2,
    MKB(GX_BM_SUBTRACT)=3,
    MKB(GX_MAX_BLENDMODE)=4
};
typedef undefined4 MKB(GXBlendMode);

enum {
    MKB(GX_READ_00)=0,
    MKB(GX_READ_FF)=1,
    MKB(GX_READ_NONE)=2
};
typedef undefined4 MKB(GXAlphaReadMode);

typedef union MKB(PPCWGPipe) MKB(PPCWGPipe), *MKB(PPPCWGPipe);

union MKB(PPCWGPipe) { /* PPC Write Gather Pipe. Original field names didn't have v_ prefix */
    MKB(u8) v_u8;
    MKB(u16) v_u16;
    MKB(u32) v_u32;
    MKB(s8) v_s8;
    MKB(s16) v_s16;
    MKB(s32) v_s32;
    MKB(f32) v_f32;
    MKB(f64) v_f64;
};
static_assert(sizeof(MKB(PPCWGPipe)) == 0x8);

typedef MKB(u32) MKB(OSTick);

typedef struct MKB(GXVtxAttrFmtList) MKB(GXVtxAttrFmtList), *MKB(PGXVtxAttrFmtList);

enum {
    MKB(GX_VA_PNMTXIDX)=0,
    MKB(GX_VA_TEX0MTXIDX)=1,
    MKB(GX_VA_TEX1MTXIDX)=2,
    MKB(GX_VA_TEX2MTXIDX)=3,
    MKB(GX_VA_TEX3MTXIDX)=4,
    MKB(GX_VA_TEX4MTXIDX)=5,
    MKB(GX_VA_TEX5MTXIDX)=6,
    MKB(GX_VA_TEX6MTXIDX)=7,
    MKB(GX_VA_TEX7MTXIDX)=8,
    MKB(GX_VA_POS)=9,
    MKB(GX_VA_NRM)=10,
    MKB(GX_VA_CLR0)=11,
    MKB(GX_VA_CLR1)=12,
    MKB(GX_VA_TEX0)=13,
    MKB(GX_VA_TEX1)=14,
    MKB(GX_VA_TEX2)=15,
    MKB(GX_VA_TEX3)=16,
    MKB(GX_VA_TEX4)=17,
    MKB(GX_VA_TEX5)=18,
    MKB(GX_VA_TEX6)=19,
    MKB(GX_VA_TEX7)=20,
    MKB(GX_POS_MTX_ARRAY)=21,
    MKB(GX_NRM_MTX_ARRAY)=22,
    MKB(GX_TEX_MTX_ARRAY)=23,
    MKB(GX_LIGHT_ARRAY)=24,
    MKB(GX_VA_NBT)=25,
    MKB(GX_VA_MAX_ATTR)=26,
    MKB(GX_VA_NULL)=255
};
typedef undefined4 MKB(GXAttr);

enum {
    MKB(GX_CLR_RGB)=0,
    MKB(GX_CLR_RGBA)=1,
    MKB(GX_NRM_NBT3)=2
};
typedef undefined4 MKB(GXCompCnt);

enum {
    MKB(GX_RGB565)=0,
    MKB(GX_RGB8)=1,
    MKB(GX_RGBX8)=2,
    MKB(GX_RGBA4)=3,
    MKB(GX_F32)=4,
    MKB(GX_RGBA8)=5
};
typedef undefined4 MKB(GXCompType);

struct MKB(GXVtxAttrFmtList) {
    MKB(GXAttr)  attr;
    MKB(GXCompCnt)  cnt;
    MKB(GXCompType)  type;
    MKB(u8) frac;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXVtxAttrFmtList)) == 0xd);

enum {
    MKB(GX_QUADS)=128,
    MKB(GX_TRIANGLES)=144,
    MKB(GX_TRIANGLESTRIP)=152,
    MKB(GX_TRIANGLEFAN)=160,
    MKB(GX_LINES)=168,
    MKB(GX_LINESTRIP)=176,
    MKB(GX_POINTS)=184
};
typedef undefined4 MKB(GXPrimitive);

enum {
    MKB(GX_TEV_KCSEL_1)=0,
    MKB(GX_TEV_KCSEL_7_8)=1,
    MKB(GX_TEV_KCSEL_3_4)=2,
    MKB(GX_TEV_KCSEL_5_8)=3,
    MKB(GX_TEV_KCSEL_1_2)=4,
    MKB(GX_TEV_KCSEL_3_8)=5,
    MKB(GX_TEV_KCSEL_1_4)=6,
    MKB(GX_TEV_KCSEL_1_8)=7,
    MKB(GX_TEV_KCSEL_K0)=12,
    MKB(GX_TEV_KCSEL_K1)=13,
    MKB(GX_TEV_KCSEL_K2)=14,
    MKB(GX_TEV_KCSEL_K3)=15,
    MKB(GX_TEV_KCSEL_K0_R)=16,
    MKB(GX_TEV_KCSEL_K1_R)=17,
    MKB(GX_TEV_KCSEL_K2_R)=18,
    MKB(GX_TEV_KCSEL_K3_R)=19,
    MKB(GX_TEV_KCSEL_K0_G)=20,
    MKB(GX_TEV_KCSEL_K1_G)=21,
    MKB(GX_TEV_KCSEL_K2_G)=22,
    MKB(GX_TEV_KCSEL_K3_G)=23,
    MKB(GX_TEV_KCSEL_K0_B)=24,
    MKB(GX_TEV_KCSEL_K1_B)=25,
    MKB(GX_TEV_KCSEL_K2_B)=26,
    MKB(GX_TEV_KCSEL_K3_B)=27,
    MKB(GX_TEV_KCSEL_K0_A)=28,
    MKB(GX_TEV_KCSEL_K1_A)=29,
    MKB(GX_TEV_KCSEL_K2_A)=30,
    MKB(GX_TEV_KCSEL_K3_A)=31
};
typedef undefined4 MKB(GXTevKColorSel);

typedef MKB(u32) MKB(OSModuleID);

enum {
    MKB(VI_TVMODE_NTSC_INT)=0,
    MKB(VI_TVMODE_NTSC_DS)=1,
    MKB(VI_TVMODE_NTSC_PROG)=2,
    MKB(VI_TVMODE_PAL_INT)=4,
    MKB(VI_TVMODE_PAL_DS)=5,
    MKB(VI_TVMODE_MPAL_INT)=8,
    MKB(VI_TVMODE_MPAL_DS)=9,
    MKB(VI_TVMODE_DEBUG_INT)=12,
    MKB(VI_TVMODE_DEBUG_PAL_INT)=16,
    MKB(VI_TVMODE_DEBUG_PAL_DS)=17,
    MKB(VI_TVMODE_EURGB60_INT)=20,
    MKB(VI_TVMODE_EURGB60_DS)=21
};
typedef undefined4 MKB(VITVMode);

typedef struct MKB(OSModuleInfo) MKB(OSModuleInfo), *MKB(POSModuleInfo);

typedef struct MKB(OSModuleLink) MKB(OSModuleLink), *MKB(POSModuleLink);

struct MKB(OSModuleLink) {
    struct MKB(OSModuleInfo) * next;
    struct MKB(OSModuleInfo) * prev;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSModuleLink)) == 0x8);

struct MKB(OSModuleInfo) { /* This matches the start of REL file header */
    MKB(OSModuleID) id; /* unique identifier for the module */
    struct MKB(OSModuleLink) link; /* doubly linked list of modules */
    MKB(u32) numSections; /* # of sections */
    MKB(u32) sectionInfoOffset; /* offset to section info table */
    MKB(u32) nameOffset; /* offset to module name */
    MKB(u32) nameSize; /* size of module name */
    MKB(u32) version; /* version number */
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSModuleInfo)) == 0x20);

enum {
    MKB(GX_TF_C4)=8,
    MKB(GX_TF_C8)=9,
    MKB(GX_TF_C14X2)=10
};
typedef undefined4 MKB(GXCITexFmt);

enum {
    MKB(GX_TG_POS)=0,
    MKB(GX_TG_NRM)=1,
    MKB(GX_TG_BINRM)=2,
    MKB(GX_TG_TANGENT)=3,
    MKB(GX_TG_TEX0)=4,
    MKB(GX_TG_TEX1)=5,
    MKB(GX_TG_TEX2)=6,
    MKB(GX_TG_TEX3)=7,
    MKB(GX_TG_TEX4)=8,
    MKB(GX_TG_TEX5)=9,
    MKB(GX_TG_TEX6)=10,
    MKB(GX_TG_TEX7)=11,
    MKB(GX_TG_TEXCOORD0)=12,
    MKB(GX_TG_TEXCOORD1)=13,
    MKB(GX_TG_TEXCOORD2)=14,
    MKB(GX_TG_TEXCOORD3)=15,
    MKB(GX_TG_TEXCOORD4)=16,
    MKB(GX_TG_TEXCOORD5)=17,
    MKB(GX_TG_TEXCOORD6)=18,
    MKB(GX_TG_COLOR0)=19,
    MKB(GX_TG_COLOR1)=20
};
typedef undefined4 MKB(GXTexGenSrc);

enum {
    MKB(GX_COLOR0)=0,
    MKB(GX_COLOR1)=1,
    MKB(GX_ALPHA0)=2,
    MKB(GX_ALPHA1)=3,
    MKB(GX_COLOR0A0)=4,
    MKB(GX_COLOR1A1)=5,
    MKB(GX_COLOR_ZERO)=6,
    MKB(GX_ALPHA_BUMP)=7,
    MKB(GX_ALPHA_BUMPN)=8,
    MKB(GX_COLOR_NULL)=255
};
typedef undefined4 MKB(GXChannelID);

enum {
    MKB(VI_XFBMODE_SF)=0,
    MKB(VI_XFBMODE_DF)=1
};
typedef undefined4 MKB(VIXFBMode);

typedef struct MKB(GXTlutRegion) MKB(GXTlutRegion), *MKB(PGXTlutRegion);

struct MKB(GXTlutRegion) {
    MKB(u32) dummy[4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXTlutRegion)) == 0x10);

enum {
    MKB(GX_CLAMP)=0,
    MKB(GX_REPEAT)=1,
    MKB(GX_MIRROR)=2,
    MKB(GX_MAX_TEXWRAPMODE)=3
};
typedef undefined4 MKB(GXTexWrapMode);

enum {
    MKB(GX_ITBA_OFF)=0,
    MKB(GX_ITBA_S)=1,
    MKB(GX_ITBA_T)=2,
    MKB(GX_ITBA_U)=3,
    MKB(GX_MAX_ITBALPHA)=4
};
typedef undefined4 MKB(GXIndTexAlphaSel);

enum {
    MKB(GX_TEXMAP0)=0,
    MKB(GX_TEXMAP1)=1,
    MKB(GX_TEXMAP2)=2,
    MKB(GX_TEXMAP3)=3,
    MKB(GX_TEXMAP4)=4,
    MKB(GX_TEXMAP5)=5,
    MKB(GX_TEXMAP6)=6,
    MKB(GX_TEXMAP7)=7,
    MKB(GX_MAX_TEXMAP)=8,
    MKB(GX_TEXMAP_NULL)=255,
    MKB(GX_TEX_DISABLE)=256
};
typedef undefined4 MKB(GXTexMapID);

enum {
    MKB(GX_VC_POS)=0,
    MKB(GX_VC_NRM)=1,
    MKB(GX_VC_CLR0)=2,
    MKB(GX_VC_CLR1)=3,
    MKB(GX_VC_TEX0)=4,
    MKB(GX_VC_TEX1)=5,
    MKB(GX_VC_TEX2)=6,
    MKB(GX_VC_TEX3)=7,
    MKB(GX_VC_TEX4)=8,
    MKB(GX_VC_TEX5)=9,
    MKB(GX_VC_TEX6)=10,
    MKB(GX_VC_TEX7)=11,
    MKB(GX_VC_ALL)=15
};
typedef undefined4 MKB(GXVCachePerf);

typedef MKB(f32) MKB(Mtx44)[4][4];

enum {
    MKB(GX_CS_SCALE_1)=0,
    MKB(GX_CS_SCALE_2)=1,
    MKB(GX_CS_SCALE_4)=2,
    MKB(GX_CS_DIVIDE_2)=3,
    MKB(GX_MAX_TEVSCALE)=4
};
typedef undefined4 MKB(GXTevScale);

typedef struct MKB(OSRel) MKB(OSRel), *MKB(POSRel);

struct MKB(OSRel) {
    MKB(u16) offset;
    MKB(u8) type;
    MKB(u8) section;
    MKB(u32) addend;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSRel)) == 0x8);

enum {
    MKB(GX_CLAMP_NONE)=0,
    MKB(GX_CLAMP_TOP)=1,
    MKB(GX_CLAMP_BOTTOM)=2
};
typedef undefined4 MKB(GXFBClamp);

typedef struct MKB(GXTexRegion) MKB(GXTexRegion), *MKB(PGXTexRegion);

struct MKB(GXTexRegion) {
    MKB(u32) dummy[4];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXTexRegion)) == 0x10);

enum {
    MKB(GX_TEV_ADD)=0,
    MKB(GX_TEV_SUB)=1,
    MKB(GX_TEV_COMP_R8_GT)=8,
    MKB(GX_TEV_COMP_R8_EQ)=9,
    MKB(GX_TEV_COMP_GR16_GT)=10,
    MKB(GX_TEV_COMP_GR16_EQ)=11,
    MKB(GX_TEV_COMP_BGR24_GT)=12,
    MKB(GX_TEV_COMP_BGR24_EQ)=13,
    MKB(GX_TEV_COMP_RGB8_GT)=14,
    MKB(GX_TEV_COMP_RGB8_EQ)=15,
    MKB(GX_TEV_COMP_A8_GT)=16,
    MKB(GX_TEV_COMP_A8_EQ)=17
};
typedef undefined4 MKB(GXTevOp);

typedef struct MKB(GXFogAdjTable) MKB(GXFogAdjTable), *MKB(PGXFogAdjTable);

struct MKB(GXFogAdjTable) {
    MKB(u16) dummy[10];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXFogAdjTable)) == 0x14);

enum {
    MKB(GX_ZT_DISABLE)=0,
    MKB(GX_ZT_ADD)=1,
    MKB(GX_ZT_REPLACE)=2,
    MKB(GX_MAX_ZTEXOP)=3
};
typedef undefined4 MKB(GXZTexOp);

typedef struct MKB(GXVtxDescList) MKB(GXVtxDescList), *MKB(PGXVtxDescList);

struct MKB(GXVtxDescList) {
    MKB(GXAttr)  attr;
    MKB(GXAttrType)  type;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXVtxDescList)) == 0x8);

enum {
    MKB(GX_TB_ZERO)=0,
    MKB(GX_TB_ADDHALF)=1,
    MKB(GX_TB_SUBHALF)=2,
    MKB(GX_MAX_TEVBIAS)=3
};
typedef undefined4 MKB(GXTevBias);

enum {
    MKB(GX_TEV_SWAP0)=0,
    MKB(GX_TEV_SWAP1)=1,
    MKB(GX_TEV_SWAP2)=2,
    MKB(GX_TEV_SWAP3)=3,
    MKB(GX_MAX_TEVSWAP)=4
};
typedef undefined4 MKB(GXTevSwapSel);

typedef struct MKB(ChunkInfo) MKB(ChunkInfo), *MKB(PChunkInfo);

struct MKB(ChunkInfo) { /* Header data for a heap chunk. The first 32 bytes of a heap chunk is dedicated to this metadata and padding, and the rest is the actual data in the chunk */
    struct MKB(ChunkInfo) * prev;
    struct MKB(ChunkInfo) * next;
    MKB(u32) size;
    MKB(u8) padding[20]; /* This is to make the data portion of the chunk 32-bit aligned */
} __attribute__((__packed__));
static_assert(sizeof(MKB(ChunkInfo)) == 0x20);

typedef struct MKB(GXLightObj) MKB(GXLightObj), *MKB(PGXLightObj);

struct MKB(GXLightObj) {
    MKB(u32) dummy[16];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXLightObj)) == 0x40);

enum {
    MKB(GX_TLUT_16)=1,
    MKB(GX_TLUT_32)=2,
    MKB(GX_TLUT_64)=4,
    MKB(GX_TLUT_128)=8,
    MKB(GX_TLUT_256)=16,
    MKB(GX_TLUT_512)=32,
    MKB(GX_TLUT_1K)=64,
    MKB(GX_TLUT_2K)=128,
    MKB(GX_TLUT_4K)=256,
    MKB(GX_TLUT_8K)=512,
    MKB(GX_TLUT_16K)=1024
};
typedef undefined4 MKB(GXTlutSize);

enum {
    MKB(GX_CC_CPREV)=0,
    MKB(GX_CC_APREV)=1,
    MKB(GX_CC_C0)=2,
    MKB(GX_CC_A0)=3,
    MKB(GX_CC_C1)=4,
    MKB(GX_CC_A1)=5,
    MKB(GX_CC_C2)=6,
    MKB(GX_CC_A2)=7,
    MKB(GX_CC_TEXC)=8,
    MKB(GX_CC_TEXA)=9,
    MKB(GX_CC_RASC)=10,
    MKB(GX_CC_RASA)=11,
    MKB(GX_CC_ONE)=12,
    MKB(GX_CC_HALF)=13,
    MKB(GX_CC_KONST)=14,
    MKB(GX_CC_ZERO)=15,
    MKB(GX_CC_TEXRRR)=16,
    MKB(GX_CC_TEXGGG)=17,
    MKB(GX_CC_TEXBBB)=18,
    MKB(GX_CC_QUARTER)=19
};
typedef undefined4 MKB(GXTevColorArg);

enum {
    MKB(GX_AOP_AND)=0,
    MKB(GX_AOP_OR)=1,
    MKB(GX_AOP_XOR)=2,
    MKB(GX_AOP_XNOR)=3,
    MKB(GX_MAX_ALPHAOP)=4
};
typedef undefined4 MKB(GXAlphaOp);

enum {
    MKB(GX_TO_ZERO)=0,
    MKB(GX_TO_SIXTEENTH)=1,
    MKB(GX_TO_EIGHTH)=2,
    MKB(GX_TO_FOURTH)=3,
    MKB(GX_TO_HALF)=4,
    MKB(GX_TO_ONE)=5,
    MKB(GX_MAX_TEXOFFSET)=6
};
typedef undefined4 MKB(GXTexOffset);

enum {
    MKB(GX_TEV_KASEL_1)=0,
    MKB(GX_TEV_KASEL_7_8)=1,
    MKB(GX_TEV_KASEL_3_4)=2,
    MKB(GX_TEV_KASEL_5_8)=3,
    MKB(GX_TEV_KASEL_1_2)=4,
    MKB(GX_TEV_KASEL_3_8)=5,
    MKB(GX_TEV_KASEL_1_4)=6,
    MKB(GX_TEV_KASEL_1_8)=7,
    MKB(GX_TEV_KASEL_K0_R)=16,
    MKB(GX_TEV_KASEL_K1_R)=17,
    MKB(GX_TEV_KASEL_K2_R)=18,
    MKB(GX_TEV_KASEL_K3_R)=19,
    MKB(GX_TEV_KASEL_K0_G)=20,
    MKB(GX_TEV_KASEL_K1_G)=21,
    MKB(GX_TEV_KASEL_K2_G)=22,
    MKB(GX_TEV_KASEL_K3_G)=23,
    MKB(GX_TEV_KASEL_K0_B)=24,
    MKB(GX_TEV_KASEL_K1_B)=25,
    MKB(GX_TEV_KASEL_K2_B)=26,
    MKB(GX_TEV_KASEL_K3_B)=27,
    MKB(GX_TEV_KASEL_K0_A)=28,
    MKB(GX_TEV_KASEL_K1_A)=29,
    MKB(GX_TEV_KASEL_K2_A)=30,
    MKB(GX_TEV_KASEL_K3_A)=31
};
typedef undefined4 MKB(GXTevKAlphaSel);

enum {
    MKB(GX_DA_OFF)=0,
    MKB(GX_DA_GENTLE)=1,
    MKB(GX_DA_MEDIUM)=2,
    MKB(GX_DA_STEEP)=3
};
typedef undefined4 MKB(GXDistAttnFn);

enum {
    MKB(GX_NEAR)=0,
    MKB(GX_LINEAR)=1,
    MKB(GX_NEAR_MIP_NEAR)=2,
    MKB(GX_LIN_MIP_NEAR)=3,
    MKB(GX_NEAR_MIP_LIN)=4,
    MKB(GX_LIN_MIP_LIN)=5
};
typedef undefined4 MKB(GXTexFilter);

enum {
    MKB(GX_CA_APREV)=0,
    MKB(GX_CA_A0)=1,
    MKB(GX_CA_A1)=2,
    MKB(GX_CA_A2)=3,
    MKB(GX_CA_TEXA)=4,
    MKB(GX_CA_RASA)=5,
    MKB(GX_CA_KONST)=6,
    MKB(GX_CA_ZERO)=7
};
typedef undefined4 MKB(GXTevAlphaArg);

typedef struct MKB(GXTlutObj) MKB(GXTlutObj), *MKB(PGXTlutObj);

struct MKB(GXTlutObj) {
    MKB(u32) dummy[3];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXTlutObj)) == 0xc);

enum {
    MKB(GX_VTXFMT0)=0,
    MKB(GX_VTXFMT1)=1,
    MKB(GX_VTXFMT2)=2,
    MKB(GX_VTXFMT3)=3,
    MKB(GX_VTXFMT4)=4,
    MKB(GX_VTXFMT5)=5,
    MKB(GX_VTXFMT6)=6,
    MKB(GX_VTXFMT7)=7,
    MKB(GX_MAX_VTXFMT)=8
};
typedef undefined4 MKB(GXVtxFmt);

enum {
    MKB(GX_NEVER)=0,
    MKB(GX_LESS)=1,
    MKB(GX_EQUAL)=2,
    MKB(GX_LEQUAL)=3,
    MKB(GX_GREATER)=4,
    MKB(GX_NEQUAL)=5,
    MKB(GX_GEQUAL)=6,
    MKB(GX_ALWAYS)=7
};
typedef undefined4 MKB(GXCompare);

enum {
    MKB(GX_PNMTX0)=0,
    MKB(GX_PNMTX1)=3,
    MKB(GX_PNMTX2)=6,
    MKB(GX_PNMTX3)=9,
    MKB(GX_PNMTX4)=12,
    MKB(GX_PNMTX5)=15,
    MKB(GX_PNMTX6)=18,
    MKB(GX_PNMTX7)=21,
    MKB(GX_PNMTX8)=24,
    MKB(GX_PNMTX9)=27
};
typedef undefined4 MKB(GXPosNrmMtx);

typedef struct MKB(OSCalendarTime) MKB(OSCalendarTime), *MKB(POSCalendarTime);

struct MKB(OSCalendarTime) {
    int sec; /* seconds after the minute [0, 61] */
    int min; /* minutes after the hour [0, 59] */
    int hour; /* hours since midnight [0, 23] */
    int mday; /* day of the month [1, 31] */
    int mon; /* month since January [0, 11] */
    int year; /* years in AD [1, ...] */
    int wday; /* days since Sunday [0, 6] */
    int yday; /* days since January 1 [0, 365] */
    int msec; /* milliseconds after the second [0,999] */
    int usec; /* microseconds after the millisecond [0,999] */
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSCalendarTime)) == 0x28);

enum {
    MKB(GX_PF_RGB8_Z24)=0,
    MKB(GX_PF_RGBA6_Z24)=1,
    MKB(GX_PF_RGB565_Z16)=2,
    MKB(GX_PF_Z24)=3,
    MKB(GX_PF_Y8)=4,
    MKB(GX_PF_U8)=5,
    MKB(GX_PF_V8)=6,
    MKB(GX_PF_YUV420)=7
};
typedef undefined4 MKB(GXPixelFmt);

typedef struct MKB(OSModuleHeader) MKB(OSModuleHeader), *MKB(POSModuleHeader);

struct MKB(OSModuleHeader) {
    struct MKB(OSModuleInfo) info;
    MKB(u32) bssSize;
    MKB(u32) relOffset;
    MKB(u32) impOffset;
    MKB(u32) impSize;
    MKB(u8) prologSection;
    MKB(u8) epilogSection;
    MKB(u8) unresolvedSection;
    MKB(u8) padding0;
    MKB(u32) prolog;
    MKB(u32) epilog;
    MKB(u32) unresolved;
    MKB(u32) align; /* REL versions >=2 only */
    MKB(u32) bssAlign; /* REL versions >=2 only */
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSModuleHeader)) == 0x48);

enum {
    MKB(GX_TL_IA8)=0,
    MKB(GX_TL_RGB565)=1,
    MKB(GX_TL_RGB5A3)=2,
    MKB(GX_MAX_TLUTFMT)=3
};
typedef undefined4 MKB(GXTlutFmt);

typedef struct MKB(ARQRequest) MKB(ARQRequest), *MKB(PARQRequest);

struct MKB(ARQRequest) {
    struct MKB(ARQRequest) * next; /* pointer to next in list */
    MKB(u32) owner; /* user enumerated owner id */
    MKB(u32) type; /* DMA direction */
    MKB(u32) priority; /* priority for transaction */
    MKB(u32) source; /* address of source */
    MKB(u32) dest; /* address of destination */
    MKB(u32) length; /* length in bytes */
    void (* callback)(MKB(u32)); /* user specified callback routine */
} __attribute__((__packed__));
static_assert(sizeof(MKB(ARQRequest)) == 0x20);

enum {
    MKB(GX_SP_OFF)=0,
    MKB(GX_SP_FLAT)=1,
    MKB(GX_SP_COS)=2,
    MKB(GX_SP_COS2)=3,
    MKB(GX_SP_SHARP)=4,
    MKB(GX_SP_RING1)=5,
    MKB(GX_SP_RING2)=6
};
typedef undefined4 MKB(GXSpotFn);

typedef struct MKB(HeapInfo) MKB(HeapInfo), *MKB(PHeapInfo);

struct MKB(HeapInfo) { /* OS heap info */
    MKB(u32) capacity;
    struct MKB(ChunkInfo) * first_free;
    struct MKB(ChunkInfo) * first_used;
} __attribute__((__packed__));
static_assert(sizeof(MKB(HeapInfo)) == 0xc);

enum {
    MKB(GX_CULL_NONE)=0,
    MKB(GX_CULL_FRONT)=1,
    MKB(GX_CULL_BACK)=2,
    MKB(GX_CULL_ALL)=3
};
typedef undefined4 MKB(GXCullMode);

enum {
    MKB(GX_CH_RED)=0,
    MKB(GX_CH_GREEN)=1,
    MKB(GX_CH_BLUE)=2,
    MKB(GX_CH_ALPHA)=3
};
typedef undefined4 MKB(GXTevColorChan);

enum {
    MKB(GX_ITM_OFF)=0,
    MKB(GX_ITM_0)=1,
    MKB(GX_ITM_1)=2,
    MKB(GX_ITM_2)=3,
    MKB(GX_ITM_S0)=5,
    MKB(GX_ITM_S1)=6,
    MKB(GX_ITM_S2)=7,
    MKB(GX_ITM_T0)=9,
    MKB(GX_ITM_T1)=10,
    MKB(GX_ITM_T2)=11
};
typedef undefined4 MKB(GXIndTexMtxID);

enum {
    MKB(GX_GM_1_0)=0,
    MKB(GX_GM_1_7)=1,
    MKB(GX_GM_2_2)=2
};
typedef undefined4 MKB(GXGamma);

typedef struct MKB(DVDDirEntry) MKB(DVDDirEntry), *MKB(PDVDDirEntry);

struct MKB(DVDDirEntry) {
    MKB(u32) entryNum;
    MKB(BOOL32) isDir;
    char * name;
} __attribute__((__packed__));
static_assert(sizeof(MKB(DVDDirEntry)) == 0xc);

enum {
    MKB(GX_TEVPREV)=0,
    MKB(GX_TEVREG0)=1,
    MKB(GX_TEVREG1)=2,
    MKB(GX_TEVREG2)=3,
    MKB(GX_MAX_TEVREG)=4
};
typedef undefined4 MKB(GXTevRegID);

enum {
    MKB(GX_TEXCACHE_32K)=0,
    MKB(GX_TEXCACHE_128K)=1,
    MKB(GX_TEXCACHE_512K)=2,
    MKB(GX_TEXCACHE_NONE)=3
};
typedef undefined4 MKB(GXTexCacheSize);

enum {
    MKB(GX_LIGHT_NULL)=0,
    MKB(GX_LIGHT0)=1,
    MKB(GX_LIGHT1)=2,
    MKB(GX_LIGHT2)=4,
    MKB(GX_LIGHT3)=8,
    MKB(GX_LIGHT4)=16,
    MKB(GX_LIGHT5)=32,
    MKB(GX_LIGHT6)=64,
    MKB(GX_LIGHT7)=128,
    MKB(GX_MAX_LIGHT)=256
};
typedef undefined4 MKB(GXLightID);

enum {
    MKB(GX_MTX3x4)=0,
    MKB(GX_MTX2x4)=1
};
typedef undefined4 MKB(GXTexMtxType);

enum {
    MKB(GX_BL_ZERO)=0,
    MKB(GX_BL_ONE)=1,
    MKB(GX_BL_SRCCLR)=2,
    MKB(GX_BL_INVSRCCLR)=3,
    MKB(GX_BL_SRCALPHA)=4,
    MKB(GX_BL_INVSRCALPHA)=5,
    MKB(GX_BL_DSTALPHA)=6,
    MKB(GX_BL_INVDSTALPHA)=7,
    MKB(GX_BL_DSTCLR)=8,
    MKB(GX_BL_INVDSTCLR)=9
};
typedef undefined4 MKB(GXBlendFactor);

enum {
    MKB(GX_ZC_LINEAR)=0,
    MKB(GX_ZC_NEAR)=1,
    MKB(GX_ZC_MID)=2,
    MKB(GX_ZC_FAR)=3
};
typedef undefined4 MKB(GXZFmt16);

enum {
    MKB(GX_ITB_NONE)=0,
    MKB(GX_ITB_S)=1,
    MKB(GX_ITB_T)=2,
    MKB(GX_ITB_ST)=3,
    MKB(GX_ITB_U)=4,
    MKB(GX_ITB_SU)=5,
    MKB(GX_ITB_TU)=6,
    MKB(GX_ITB_STU)=7,
    MKB(GX_MAX_ITBIAS)=8
};
typedef undefined4 MKB(GXIndTexBiasSel);

enum {
    MKB(GX_CLIP_ENABLE)=0,
    MKB(GX_CLIP_DISABLE)=1
};
typedef undefined4 MKB(GXClipMode);

enum {
    MKB(GX_ITF_8)=0,
    MKB(GX_ITF_5)=1,
    MKB(GX_ITF_4)=2,
    MKB(GX_ITF_3)=3,
    MKB(GX_MAX_ITFORMAT)=4
};
typedef undefined4 MKB(GXIndTexFormat);

enum {
    MKB(GX_COPY_PROGRESSIVE)=0,
    MKB(GX_COPY_INTLC_EVEN)=2,
    MKB(GX_COPY_INTLC_ODD)=3
};
typedef undefined4 MKB(GXCopyMode);

enum {
    MKB(GX_TEVSTAGE0)=0,
    MKB(GX_TEVSTAGE1)=1,
    MKB(GX_TEVSTAGE2)=2,
    MKB(GX_TEVSTAGE3)=3,
    MKB(GX_TEVSTAGE4)=4,
    MKB(GX_TEVSTAGE5)=5,
    MKB(GX_TEVSTAGE6)=6,
    MKB(GX_TEVSTAGE7)=7,
    MKB(GX_TEVSTAGE8)=8,
    MKB(GX_TEVSTAGE9)=9,
    MKB(GX_TEVSTAGE10)=10,
    MKB(GX_TEVSTAGE11)=11,
    MKB(GX_TEVSTAGE12)=12,
    MKB(GX_TEVSTAGE13)=13,
    MKB(GX_TEVSTAGE14)=14,
    MKB(GX_TEVSTAGE15)=15,
    MKB(GX_MAX_TEVSTAGE)=16
};
typedef undefined4 MKB(GXTevStageID);

enum {
    MKB(GX_PERF1_TEXELS)=0,
    MKB(GX_PERF1_TX_IDLE)=1,
    MKB(GX_PERF1_TX_REGS)=2,
    MKB(GX_PERF1_TX_MEMSTALL)=3,
    MKB(GX_PERF1_TC_CHECK1_2)=4,
    MKB(GX_PERF1_TC_CHECK3_4)=5,
    MKB(GX_PERF1_TC_CHECK5_6)=6,
    MKB(GX_PERF1_TC_CHECK7_8)=7,
    MKB(GX_PERF1_TC_MISS)=8,
    MKB(GX_PERF1_VC_ELEMQ_FULL)=9,
    MKB(GX_PERF1_VC_MISSQ_FULL)=10,
    MKB(GX_PERF1_VC_MEMREQ_FULL)=11,
    MKB(GX_PERF1_VC_STATUS7)=12,
    MKB(GX_PERF1_VC_MISSREP_FULL)=13,
    MKB(GX_PERF1_VC_STREAMBUF_LOW)=14,
    MKB(GX_PERF1_VC_ALL_STALLS)=15,
    MKB(GX_PERF1_VERTICES)=16,
    MKB(GX_PERF1_FIFO_REQ)=17,
    MKB(GX_PERF1_CALL_REQ)=18,
    MKB(GX_PERF1_VC_MISS_REQ)=19,
    MKB(GX_PERF1_CP_ALL_REQ)=20,
    MKB(GX_PERF1_CLOCKS)=21,
    MKB(GX_PERF1_NONE)=22
};
typedef undefined4 MKB(GXPerf1);

enum {
    MKB(GX_PERF0_VERTICES)=0,
    MKB(GX_PERF0_CLIP_VTX)=1,
    MKB(GX_PERF0_CLIP_CLKS)=2,
    MKB(GX_PERF0_XF_WAIT_IN)=3,
    MKB(GX_PERF0_XF_WAIT_OUT)=4,
    MKB(GX_PERF0_XF_XFRM_CLKS)=5,
    MKB(GX_PERF0_XF_LIT_CLKS)=6,
    MKB(GX_PERF0_XF_BOT_CLKS)=7,
    MKB(GX_PERF0_XF_REGLD_CLKS)=8,
    MKB(GX_PERF0_XF_REGRD_CLKS)=9,
    MKB(GX_PERF0_CLIP_RATIO)=10,
    MKB(GX_PERF0_TRIANGLES)=11,
    MKB(GX_PERF0_TRIANGLES_CULLED)=12,
    MKB(GX_PERF0_TRIANGLES_PASSED)=13,
    MKB(GX_PERF0_TRIANGLES_SCISSORED)=14,
    MKB(GX_PERF0_TRIANGLES_0TEX)=15,
    MKB(GX_PERF0_TRIANGLES_1TEX)=16,
    MKB(GX_PERF0_TRIANGLES_2TEX)=17,
    MKB(GX_PERF0_TRIANGLES_3TEX)=18,
    MKB(GX_PERF0_TRIANGLES_4TEX)=19,
    MKB(GX_PERF0_TRIANGLES_5TEX)=20,
    MKB(GX_PERF0_TRIANGLES_6TEX)=21,
    MKB(GX_PERF0_TRIANGLES_7TEX)=22,
    MKB(GX_PERF0_TRIANGLES_8TEX)=23,
    MKB(GX_PERF0_TRIANGLES_0CLR)=24,
    MKB(GX_PERF0_TRIANGLES_1CLR)=25,
    MKB(GX_PERF0_TRIANGLES_2CLR)=26,
    MKB(GX_PERF0_QUAD_0CVG)=27,
    MKB(GX_PERF0_QUAD_NON0CVG)=28,
    MKB(GX_PERF0_QUAD_1CVG)=29,
    MKB(GX_PERF0_QUAD_2CVG)=30,
    MKB(GX_PERF0_QUAD_3CVG)=31,
    MKB(GX_PERF0_QUAD_4CVG)=32,
    MKB(GX_PERF0_AVG_QUAD_CNT)=33,
    MKB(GX_PERF0_CLOCKS)=34,
    MKB(GX_PERF0_NONE)=35
};
typedef undefined4 MKB(GXPerf0);

typedef struct MKB(OSImportInfo) MKB(OSImportInfo), *MKB(POSImportInfo);

struct MKB(OSImportInfo) {
    MKB(OSModuleID) id;
    MKB(u32) offset;
} __attribute__((__packed__));
static_assert(sizeof(MKB(OSImportInfo)) == 0x8);

typedef struct MKB(GXRenderModeObj) MKB(GXRenderModeObj), *MKB(PGXRenderModeObj);

struct MKB(GXRenderModeObj) {
    MKB(VITVMode)  viTVmode;
    MKB(u16) fbWidth;
    MKB(u16) efbHeight;
    MKB(u16) xfbHeight;
    MKB(u16) viXOrigin;
    MKB(u16) viYOrigin;
    MKB(u16) viWidth;
    MKB(u16) viHeight;
    MKB(VIXFBMode)  xFBmode;
    MKB(u8) field_rendering;
    MKB(u8) aa;
    MKB(u8) sample_pattern[12][2];
    MKB(u8) vfilter[7];
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXRenderModeObj)) == 0x37);

enum {
    MKB(GX_XF_FLUSH_NONE)=0,
    MKB(GX_XF_FLUSH_SAFE)=8
};
typedef undefined4 MKB(GXXFFlushVal);

enum {
    MKB(GX_TLUT0)=0,
    MKB(GX_TLUT1)=1,
    MKB(GX_TLUT2)=2,
    MKB(GX_TLUT3)=3,
    MKB(GX_TLUT4)=4,
    MKB(GX_TLUT5)=5,
    MKB(GX_TLUT6)=6,
    MKB(GX_TLUT7)=7,
    MKB(GX_TLUT8)=8,
    MKB(GX_TLUT9)=9,
    MKB(GX_TLUT10)=10,
    MKB(GX_TLUT11)=11,
    MKB(GX_TLUT12)=12,
    MKB(GX_TLUT13)=13,
    MKB(GX_TLUT14)=14,
    MKB(GX_TLUT15)=15,
    MKB(GX_BIGTLUT0)=16,
    MKB(GX_BIGTLUT1)=17,
    MKB(GX_BIGTLUT2)=18,
    MKB(GX_BIGTLUT3)=19
};
typedef undefined4 MKB(GXTlut);

typedef MKB(u8) MKB(PERFId);

enum {
    MKB(GX_ITS_1)=0,
    MKB(GX_ITS_2)=1,
    MKB(GX_ITS_4)=2,
    MKB(GX_ITS_8)=3,
    MKB(GX_ITS_16)=4,
    MKB(GX_ITS_32)=5,
    MKB(GX_ITS_64)=6,
    MKB(GX_ITS_128)=7,
    MKB(GX_ITS_256)=8,
    MKB(GX_MAX_ITSCALE)=9
};
typedef undefined4 MKB(GXIndTexScale);

typedef MKB(s16) MKB(__OSInterrupt);

enum {
    MKB(GX_INDTEXSTAGE0)=0,
    MKB(GX_INDTEXSTAGE1)=1,
    MKB(GX_INDTEXSTAGE2)=2,
    MKB(GX_INDTEXSTAGE3)=3,
    MKB(GX_MAX_INDTEXSTAGE)=4
};
typedef undefined4 MKB(GXIndTexStageID);

typedef struct MKB(DVDDir) MKB(DVDDir), *MKB(PDVDDir);

struct MKB(DVDDir) {
    MKB(u32) entryNum;
    MKB(u32) location;
    MKB(u32) next;
} __attribute__((__packed__));
static_assert(sizeof(MKB(DVDDir)) == 0xc);

enum {
    MKB(GX_TG_MTX3x4)=0,
    MKB(GX_TG_MTX2x4)=1,
    MKB(GX_TG_BUMP0)=2,
    MKB(GX_TG_BUMP1)=3,
    MKB(GX_TG_BUMP2)=4,
    MKB(GX_TG_BUMP3)=5,
    MKB(GX_TG_BUMP4)=6,
    MKB(GX_TG_BUMP5)=7,
    MKB(GX_TG_BUMP6)=8,
    MKB(GX_TG_BUMP7)=9,
    MKB(GX_TG_SRTG)=10
};
typedef undefined4 MKB(GXTexGenType);

enum { /* Texture Environment "konstant" color register name. */
    MKB(GX_KCOLOR0)=0,
    MKB(GX_KCOLOR1)=1,
    MKB(GX_KCOLOR2)=2,
    MKB(GX_KCOLOR3)=3,
    MKB(GX_MAX_KCOLOR)=4
};
typedef undefined1 MKB(GXTevKColorID);

enum {
    MKB(GX_LO_CLEAR)=0,
    MKB(GX_LO_AND)=1,
    MKB(GX_LO_REVAND)=2,
    MKB(GX_LO_COPY)=3,
    MKB(GX_LO_INVAND)=4,
    MKB(GX_LO_NOOP)=5,
    MKB(GX_LO_XOR)=6,
    MKB(GX_LO_OR)=7,
    MKB(GX_LO_NOR)=8,
    MKB(GX_LO_EQUIV)=9,
    MKB(GX_LO_INV)=10,
    MKB(GX_LO_REVOR)=11,
    MKB(GX_LO_INVCOPY)=12,
    MKB(GX_LO_INVOR)=13,
    MKB(GX_LO_NAND)=14,
    MKB(GX_LO_SET)=15
};
typedef undefined4 MKB(GXLogicOp);

enum {
    MKB(GX_MT_NULL)=0,
    MKB(GX_MT_XF_FLUSH)=1,
    MKB(GX_MT_DL_SAVE_CONTEXT)=2
};
typedef undefined4 MKB(GXMiscToken);

enum {
    MKB(GX_PERSPECTIVE)=0,
    MKB(GX_ORTHOGRAPHIC)=1
};
typedef undefined4 MKB(GXProjectionType);

enum {
    MKB(GX_TC_LINEAR)=0,
    MKB(GX_TC_GE)=1,
    MKB(GX_TC_EQ)=2,
    MKB(GX_TC_LE)=3,
    MKB(GX_MAX_TEVCLAMPMODE)=4
};
typedef undefined4 MKB(GXTevClampMode);

typedef struct MKB(GXColorS10) MKB(GXColorS10), *MKB(PGXColorS10);

struct MKB(GXColorS10) {
    MKB(s16) r;
    MKB(s16) g;
    MKB(s16) b;
    MKB(s16) a;
} __attribute__((__packed__));
static_assert(sizeof(MKB(GXColorS10)) == 0x8);

enum {
    MKB(GX_ITW_OFF)=0,
    MKB(GX_ITW_256)=1,
    MKB(GX_ITW_128)=2,
    MKB(GX_ITW_64)=3,
    MKB(GX_ITW_32)=4,
    MKB(GX_ITW_16)=5,
    MKB(GX_ITW_0)=6,
    MKB(GX_MAX_ITWRAP)=7
};
typedef undefined4 MKB(GXIndTexWrap);

enum {
    MKB(GX_MODULATE)=0,
    MKB(GX_DECAL)=1,
    MKB(GX_BLEND)=2,
    MKB(GX_REPLACE)=3,
    MKB(GX_PASSCLR)=4
};
typedef undefined4 MKB(GXTevMode);

typedef MKB(u32) MKB(OSInterruptMask);

enum {
    MKB(GX_TEXMTX0)=30,
    MKB(GX_TEXMTX1)=33,
    MKB(GX_TEXMTX2)=36,
    MKB(GX_TEXMTX3)=39,
    MKB(GX_TEXMTX4)=42,
    MKB(GX_TEXMTX5)=45,
    MKB(GX_TEXMTX6)=48,
    MKB(GX_TEXMTX7)=51,
    MKB(GX_TEXMTX8)=54,
    MKB(GX_TEXMTX9)=57,
    MKB(GX_IDENTITY)=60
};
typedef undefined4 MKB(GXTexMtx);

enum {
    MKB(GX_PTTEXMTX0)=64,
    MKB(GX_PTTEXMTX1)=67,
    MKB(GX_PTTEXMTX2)=70,
    MKB(GX_PTTEXMTX3)=73,
    MKB(GX_PTTEXMTX4)=76,
    MKB(GX_PTTEXMTX5)=79,
    MKB(GX_PTTEXMTX6)=82,
    MKB(GX_PTTEXMTX7)=85,
    MKB(GX_PTTEXMTX8)=88,
    MKB(GX_PTTEXMTX9)=91,
    MKB(GX_PTTEXMTX10)=94,
    MKB(GX_PTTEXMTX11)=97,
    MKB(GX_PTTEXMTX12)=100,
    MKB(GX_PTTEXMTX13)=103,
    MKB(GX_PTTEXMTX14)=106,
    MKB(GX_PTTEXMTX15)=109,
    MKB(GX_PTTEXMTX16)=112,
    MKB(GX_PTTEXMTX17)=115,
    MKB(GX_PTTEXMTX18)=118,
    MKB(GX_PTTEXMTX19)=121,
    MKB(GX_PTIDENTITY)=125
};
typedef undefined4 MKB(GXPTTexMtx);

enum {
    MKB(GX_TEXCOORD0)=0,
    MKB(GX_TEXCOORD1)=1,
    MKB(GX_TEXCOORD2)=2,
    MKB(GX_TEXCOORD3)=3,
    MKB(GX_TEXCOORD4)=4,
    MKB(GX_TEXCOORD5)=5,
    MKB(GX_TEXCOORD6)=6,
    MKB(GX_TEXCOORD7)=7,
    MKB(GX_MAX_TEXCOORD)=8,
    MKB(GX_TEXCOORD_NULL)=255
};
typedef undefined4 MKB(GXTexCoordID);

typedef int (* MKB(__compar_fn_t))(void *, void *);

typedef uchar MKB(uint8_t);

typedef void *pointer;

#ifdef __cplusplus
extern "C" {
#endif
    /* Global data */
    extern char MKB(DVD_GAME_NAME)[4];
    extern char MKB(DVD_COMPANY)[2];
    extern MKB(u8) MKB(DVD_DISK_NUMBER);
    extern MKB(u8) MKB(DVD_GAME_VERSION);
    extern MKB(u8) MKB(DVD_STREAMING);
    extern MKB(u8) MKB(DVD_STREAMING_BUF_SIZE);
    extern MKB(u32) MKB(DVD_MAGIC_WORD);
    extern MKB(u32) MKB(MAGIC_WORD);
    extern MKB(u32) MKB(VERSION);
    extern MKB(size_t) MKB(PHYSICAL_MEMORY_SIZE);
    extern MKB(u32) MKB(CONSOLE_TYPE);
    extern void * MKB(ARENA_LO);
    extern void * MKB(ARENA_HI);
    extern undefined4 MKB(FST_LOCATION);
    extern undefined4 MKB(FST_MAX_LENGTH);
    extern MKB(u32) MKB(IS_DEBUGGER_PRESENT);
    extern undefined4 MKB(DEBUGGER_EXCEPTION_MASK);
    extern undefined4 MKB(DEBUGGER_EXCEPTION_HOOK_DESTINATION);
    extern undefined4 MKB(DEBUGGER_LR_TEMP);
    extern undefined4 MKB(current_os_context_physical);
    extern undefined4 MKB(previous_os_interrupt_mask);
    extern undefined4 MKB(current_os_interrupt_mask);
    extern MKB(u32) MKB(TV_MODE);
    extern MKB(size_t) MKB(ARAM_SIZE);
    extern undefined4 MKB(current_os_context_logical);
    extern undefined4 MKB(currentOsContextLogical);
    extern undefined4 MKB(default_os_thread);
    extern undefined4 MKB(active_thread_queue_head);
    extern undefined4 MKB(active_thread_queue_tail);
    extern undefined4 MKB(current_os_thread);
    extern undefined4 MKB(DEBUG_MONITOR_SIZE);
    extern undefined4 MKB(DEBUG_MONITOR_LOCATION);
    extern MKB(size_t) MKB(MEMORY_SIZE);
    extern undefined4 MKB(DVD_BI2_LOCATION);
    extern undefined4 MKB(BUS_CLOCK_SPEED);
    extern undefined4 MKB(CPU_CLOCK_SPEED);
    extern MKB(undefined) MKB(system_reset_exception_handler);
    extern MKB(undefined) MKB(machine_check_exception_handler);
    extern MKB(undefined) MKB(dsi_exception_handler);
    extern MKB(undefined) MKB(isi_exception_handler);
    extern MKB(undefined) MKB(external_interrupt_exception_handler);
    extern MKB(undefined) MKB(alignment_exception_handler);
    extern MKB(undefined) MKB(program_exception_handler);
    extern MKB(undefined) MKB(floating_point_unavailable_exception_handler);
    extern MKB(undefined) MKB(decrementer_exception_handler);
    extern MKB(undefined) MKB(system_call_exception_handler);
    extern MKB(undefined) MKB(trace_exception_handler);
    extern MKB(undefined) MKB(performance_monitor_interrupt_handler);
    extern MKB(undefined) MKB(instruction_address_breakpoint_exception_handler);
    extern MKB(undefined) MKB(system_management_interrupt_handler);
    extern MKB(undefined) MKB(thermal_management_interrupt_exception_handler);
    extern undefined4 MKB(osStringTablePtr);
    extern void  * MKB(switchdataD_80081a8c);
    extern struct MKB(FontDefinition) MKB(FONT_DEFINITIONS)[144];
    extern MKB(undefined) MKB(fullscreen_texture_buf);
    extern void  * MKB(switchdataD_80110c1c);
    extern MKB(undefined) * MKB(switchdataD_80111e20);
    extern void  * MKB(switchdataD_80111e3c);
    extern MKB(undefined) * MKB(switchdataD_80111e7c);
    extern MKB(undefined) * MKB(switchdataD_80111eb0);
    extern MKB(undefined) * MKB(switchdataD_80112104);
    extern MKB(undefined) * MKB(switchdataD_801125a0);
    extern MKB(undefined) * MKB(switchdataD_80112608);
    extern MKB(undefined) * MKB(switchdataD_80112670);
    extern MKB(undefined) * MKB(switchdataD_801126d8);
    extern MKB(undefined) * MKB(switchdataD_8011271c);
    extern MKB(undefined) * MKB(switchdataD_80112760);
    extern MKB(undefined) * MKB(switchdataD_801127a4);
    extern MKB(undefined) * MKB(switchdataD_801127c0);
    extern struct MKB(GXRenderModeObj) MKB(ntsc_interlaced_render_mode);
    extern struct MKB(GXRenderModeObj) MKB(ntsc_progressive_render_mode);
    extern struct MKB(GXRenderModeObj) MKB(mpal_interlaced_render_mode);
    extern struct MKB(GXRenderModeObj) MKB(pal_interlaced_render_mode);
    extern MKB(undefined) * MKB(switchdataD_80112948);
    extern MKB(undefined) * MKB(switchdataD_80112968);
    extern MKB(undefined) * MKB(switchdataD_80112a5c);
    extern MKB(undefined) * MKB(switchdataD_80112b50);
    extern MKB(undefined) * MKB(switchdataD_80112bd8);
    extern MKB(undefined) * MKB(switchdataD_80112c34);
    extern MKB(undefined) * MKB(switchdataD_80112e90);
    extern MKB(undefined) * MKB(switchdataD_00014130);
    extern MKB(undefined) * MKB(switchdataD_80113160);
    extern MKB(undefined) MKB(toneup_tab);
    extern MKB(undefined) MKB(tonedown_tab);
    extern undefined2 MKB(dspAttenuationTab);
    extern MKB(undefined) MKB(dspScale2IndexTab);
    extern undefined1 MKB(dspScale2IndexTab);
    extern MKB(undefined) MKB(dspDLSVolTab);
    extern MKB(undefined) MKB(musyx_vol_tab);
    extern MKB(undefined) MKB(pan_tab);
    extern MKB(undefined) MKB(pan_tab_dpl2);
    extern MKB(undefined) * MKB(switchdataD_80114110);
    extern MKB(undefined) * MKB(switchdataD_00012220);
    extern MKB(undefined) MKB(sndSintab);
    extern MKB(undefined) MKB(rsmpTab12khz);
    extern undefined4 MKB(g_some_new_thread_priority_1);
    extern undefined4 MKB(safe_thread_priority);
    extern undefined4 MKB(vsync_thread_priority);
    extern undefined4 MKB(mwidle_thread_priority);
    extern MKB(undefined) * MKB(switchdataD_801444c0);
    extern char MKB(ASCII_LOWERCASE)[256];
    extern char MKB(ASCII_UPPERCASE)[256];
    extern void  * MKB(switchdataD_80144920);
    extern void  * MKB(switchdataD_801449a4);
    extern MKB(undefined) * MKB(switchdataD_80144a28);
    extern MKB(undefined) * MKB(switchdataD_80144b08);
    extern void  * MKB(switchdataD_80144b50);
    extern MKB(undefined) * MKB(switchdataD_80144c20);
    extern MKB(undefined) * MKB(switchdataD_80144c3c);
    extern struct MKB(PadStatusGroup) MKB(pad_status_groups)[4];
    extern struct MKB(AnalogInputGroup) MKB(merged_analog_inputs);
    extern struct MKB(DigitalInputGroup) MKB(merged_digital_inputs);
    extern struct MKB(AnalogInputGroup) MKB(analog_inputs)[4];
    extern dword MKB(frames_since_last_input_change);
    extern undefined4 MKB(g_some_arq_request_src);
    extern MKB(undefined) MKB(g_aram_font_file_buffer);
    extern MKB(undefined) MKB(g_aram_font_source_location);
    extern struct MKB(ARQRequest) MKB(arq_task_struct);
    extern struct MKB(GXTexObj) MKB(sega_splash_tex);
    extern MKB(undefined) MKB(synthGlobalVariable);
    extern MKB(undefined) MKB(shdwRegs);
    extern struct MKB(GXFifoObj) MKB(default_GXFifoObj);
    extern undefined4 MKB(seqNote);
    extern undefined4 MKB(seqInstance);
    extern undefined2 MKB(seqMIDIPriority);
    extern undefined4 MKB(synthTicksPerSecond);
    extern undefined4 MKB(synthJobTable);
    extern undefined4 MKB(synthInfo);
    extern undefined4 MKB(synthMasterFader);
    extern undefined1 MKB(g_synthTrackVolume_or_synthVolume);
    extern undefined4 MKB(synthAuxACallback);
    extern undefined4 MKB(synthAuxBCallback);
    extern undefined1 MKB(synthITDDefault);
    extern MKB(undefined) MKB(inpAuxB);
    extern undefined4 MKB(vidList);
    extern undefined1 MKB(synth_last_started);
    extern undefined1 MKB(synth_last_fxstarted);
    extern MKB(undefined) MKB(voicePrioSortVoices);
    extern undefined1 MKB(voicePrioSortVoicesRoot);
    extern MKB(undefined) MKB(voicePrioSortRootList);
    extern undefined1 MKB(voiceList);
    extern undefined1 MKB(vs);
    extern undefined4 MKB(gs);
    extern undefined4 MKB(dspStudio);
    extern undefined4 MKB(dspHRTFOn);
    extern undefined4 MKB(startGroup);
    extern MKB(undefined) MKB(startListNum);
    extern MKB(undefined) MKB(runList);
    extern MKB(undefined) MKB(fx_lastNote);
    extern MKB(undefined) MKB(inpFXChannelDefaults);
    extern MKB(undefined) MKB(safe_thread_stack_base);
    extern MKB(undefined) MKB(vsync_thread_stack_base);
    extern MKB(undefined) MKB(mwidle_thread_OSThread);
    extern MKB(undefined) MKB(vsync_thread_OSThread);
    extern MKB(undefined) MKB(safe_thread_OSThread);
    extern struct MKB(GXRenderModeObj) MKB(overscan_adjusted_render_mode);
    extern struct MKB(GXFifoObj) MKB(additional_GXFifoObj);
    extern MKB(f32) MKB(projection_near_clip);
    extern MKB(f32) MKB(projection_far_clip);
    extern MKB(Mtx44) MKB(g_some_projection_matrix);
    extern undefined1 MKB(g_some_gmaflag_1);
    extern undefined1 MKB(g_some_gmaflag_2);
    extern undefined1 MKB(g_some_gmaflag_3);
    extern void  * MKB(INIT_REL_PATHS)[2];
    extern MKB(OSHeapHandle) MKB(currentHeap);
    extern undefined4 MKB(arena_lo);
    extern void  * MKB(g_some_internal_gx_struct);
    extern MKB(BOOL32) MKB(SHOULD_ALLOCATE_MEM_FROM_ARENA_HI);
    extern undefined4 MKB(rand_next_value);
    extern bool MKB(g_something_with_progressive_mode);
    extern MKB(u32) MKB(init_rel_index);
    extern struct MKB(RelBufferInfo) MKB(mainloop_rel_buffer_info);
    extern MKB(Locale)  MKB(locale);
    extern struct MKB(GmaBuffer) * MKB(init_common_gma);
    extern struct MKB(TplBuffer) * MKB(init_common_tpl);
    extern struct MKB(OptiGXSettings) * MKB(current_gx_settings);
    extern undefined1 MKB(dvd_cancel_async_complete_callback_flag);
    extern bool MKB(waiting_async_dvdread_callback_flag);
    extern undefined4 MKB(g_some_func_ptr3);
    extern undefined4 MKB(g_some_card_func_ptr);
    extern undefined4 MKB(g_some_func_ptr1);
    extern undefined4 MKB(g_some_OSTime);
    extern MKB(u32) (* MKB(console_reset_callback))(void);
    extern undefined4 MKB(aram_ptr);
    extern undefined4 MKB(aram_font_to_mram_font_spinlock);
    extern undefined4 MKB(g_some_arq_req_spinlock);
    extern undefined4 MKB(BootInfo);
    extern undefined4 MKB(BI2DebugFlag);
    extern undefined4 MKB(AreWeInitialized);
    extern undefined4 MKB(alloc_arena_start);
    extern undefined4 MKB(alloc_max_heaps);
    extern undefined4 MKB(post_OSInitAlloc_arena_start);
    extern undefined4 MKB(post_OSInitAlloc_arena_end_rounddown32);
    extern undefined4 MKB(arena_hi);
    extern undefined4 MKB(g_OSDispatchInterrupt_OSTime);
    extern undefined4 MKB(is_debugger_present_ptr);
    extern undefined4 MKB(retraceCount);
    extern undefined4 MKB(flushFlag);
    extern undefined4 MKB(PreCB);
    extern undefined4 MKB(PostCB);
    extern undefined4 MKB(encoderType);
    extern undefined2 MKB(displayOffsetH);
    extern undefined2 MKB(displayOffsetV);
    extern undefined4 MKB(showChangeMode);
    extern undefined4 MKB(shdwChanged);
    extern undefined4 MKB(g_arq_chunk_size);
    extern undefined4 MKB(CPUFifo);
    extern undefined4 MKB(GPFifo);
    extern undefined4 MKB(TokenCB);
    extern undefined4 MKB(DrawDoneCB);
    extern undefined1 MKB(g_has_finished_gx_work);
    extern struct MKB(OSThreadQueue) MKB(gx_thread_queue);
    extern undefined4 MKB(g_something_with_perf);
    extern undefined4 MKB(nextPublicID);
    extern undefined4 MKB(g_some_sound_alloc_memory_func_ptr);
    extern undefined4 MKB(g_OSTick_after_AIInitDMA);
    extern undefined4 MKB(g_are_threads_initialized);
    extern undefined4 MKB(g_some_safe_thread_counter);
    extern undefined4 MKB(g_safe_thread_loop_counter);
    extern undefined4 MKB(mwidle_thread_loop_counter);
    extern undefined4 MKB(g_some_new_thread_priority_2);
    extern undefined4 MKB(g_vsync_thread_finished);
    extern undefined4 MKB(g_vsync_thread_enabled);
    extern undefined4 MKB(g_mwidle_thread_finished);
    extern undefined4 MKB(g_mwidle_thread_enabled);
    extern undefined4 MKB(g_safe_thread_finished);
    extern undefined4 MKB(g_safe_thread_enabled);
    extern undefined4 MKB(g_something_that_resumes_safe_thread_funcptr);
    extern undefined4 MKB(g_something_that_suspends_safe_thread_funcptr);
    extern struct MKB(GXRenderModeObj) * MKB(current_render_mode);
    extern bool MKB(gx_draw_is_done);
    extern undefined4 MKB(OSTick_after_gx_start_new_frame);
    extern undefined4 MKB(gx_start_new_frame_retrace_count);
    extern undefined4 MKB(retraces_per_frame);
    extern MKB(u32) MKB(gx_fifo_use_size2);
    extern undefined4 MKB(g_some_gx_fifo_write_ptr);
    extern MKB(GxUsageState)  MKB(gx_usage);
    extern MKB(u32) MKB(gx_fifo_size);
    extern void * MKB(locked_cache_region);
    extern undefined4 MKB(old_arena_hi);
    extern undefined4 MKB(new_arena_hi);
    extern MKB(Mtx) * MKB(mtxa);
    extern void (* MKB(os_free_to_heap_func_ptr))(void *);
    extern void * (* MKB(os_alloc_from_heap_func_ptr))(MKB(u32));
    extern undefined4 MKB(g_some_empty_function_func_ptr);
    extern undefined4 MKB(g_some_dvd_close_func_ptr);
    extern undefined4 MKB(g_read_file_with_dvd_prio2_func_ptr);
    extern undefined4 MKB(g_get_dvd_file_size_func_ptr);
    extern MKB(BOOL32) (* MKB(g_some_dvd_open_func_ptr))(char *, struct MKB(DVDFileInfo) *);
    extern MKB(DVDFileInfo) * (* MKB(g_get_some_dvd_info_func_ptr))(void);
    extern struct MKB(GraphicsInfo) * MKB(graphics_info);
    extern struct MKB(GXColor) MKB(avdisp_post_add_color);
    extern struct MKB(GXColor) MKB(avdisp_post_mult_color);
    extern undefined4 MKB(avdisp_use_post_add_color);
    extern undefined4 MKB(avdisp_use_post_mult_color);
    extern undefined4 MKB(data_buffer);
    extern undefined8 MKB(i2fMagic);
    extern undefined4 MKB(value0_6);
    extern undefined4 MKB(value0_3);
    extern undefined8 MKB(i2fMagic);
    extern float MKB(FLOAT_ZERO);
    extern float MKB(FLOAT0);
    extern float MKB(FLOAT1);
    extern float MKB(FLOAT400);
    extern byte MKB(pausemenu_entry_counts)[8];
    extern undefined4 MKB(g_something_with_camera);
    extern double MKB(g_related_to_camera_turn_rate_and_stage_tilt);
    extern float MKB(camera_near_clip_z);
    extern float MKB(camera_far_clip_z);
    extern undefined4 MKB(g_camera_fov);
    extern double MKB(g_camera_turn_rate_scale);
    extern double MKB(camera_height);
    extern double MKB(camera_pivot_height);
    extern float MKB(camera_distance);
    extern undefined4 MKB(g_some_array_related_to_lights_2);
    extern undefined4 MKB(g_some_light_color);
    extern float MKB(FLOAT20000);
    extern float MKB(FLOAT0_2);
    extern float MKB(FLOAT480);
    extern float MKB(FLOAT640);
    extern MKB(BgmTrack)  MKB(g_bgm_id_lookup_table)[43];
    extern struct MKB(GSoundGroup) MKB(g_sound_group_GRPse04);
    extern struct MKB(GSoundGroupEntry) MKB(g_sound_group_GRPse04_items)[1463];
    extern struct MKB(GXColor) MKB(debugtext_bg_color);
    extern float MKB(MAX_GX_FIFO_BUF_SIZE);
    extern float MKB(visual_ball_size);
    extern float MKB(ball_friction);
    extern float MKB(epsilon);
    extern float MKB(g_goal_post_vertical_offset);
    extern float MKB(stage_tilt_multiplier);
    extern undefined8 MKB(g_itemgroup_anim_rate_post_dropin);
    extern float MKB(g_bg_animation_timescale);
    extern undefined4 MKB(bonus_stage_id_list);
    extern double MKB(jamabar_sensitivity);
    extern double MKB(jamabar_friction);
    extern double MKB(jamabar_lower_bound);
    extern float MKB(jamabar_lower_bound_2);
    extern MKB(undefined) MKB(g_some_font_value_lookup_table);
    extern char MKB(g_some_avtext_array)[344][2];
    extern MKB(undefined) MKB(BTM_SetDefaultLinkSuperTout);
    extern MKB(undefined) MKB(THPSimpleGetCurrentFrame);
    extern MKB(undefined) MKB(THPSimpleGetCurrentFrame);
    extern MKB(undefined) MKB(THPSimpleGetCurrentFrame);
    extern undefined1 MKB(ICE_FOG_THEME_ID);
    extern undefined1 MKB(ICE_FOG_TYPE);
    extern float MKB(ICE_FOG_START);
    extern float MKB(ICE_FOG_END);
    extern undefined1 MKB(ICE_FOG_RED);
    extern undefined1 MKB(ICE_FOG_GREEN);
    extern undefined1 MKB(ICE_FOG_BLUE);
    extern undefined8 MKB(divisor);
    extern undefined8 MKB(base);
    extern struct MKB(HeapConfig) MKB(heap_configs)[19];
    extern MKB(OSHeapHandle) MKB(main_heap);
    extern MKB(OSHeapHandle) MKB(stage_heap);
    extern MKB(OSHeapHandle) MKB(bg_heap);
    extern MKB(OSHeapHandle) MKB(chara_heap);
    extern MKB(OSHeapHandle) MKB(replay_heap);
    extern char MKB(empty_string)[1];
    extern MKB(undefined) * MKB(G_MAIN_MODE_ADDITIONAL_REL_PATHS)[8];
    extern void (* MKB(main_mode_funcs)[8])(void);
    extern void (* MKB(sub_mode_funcs)[265])(void);
    extern MKB(undefined) * MKB(MAIN_MODE_NAMES)[8];
    extern MKB(undefined) * MKB(SUB_MODE_NAMES)[265];
    extern struct MKB(RelBufferInfo) MKB(additional_rel_buffer_info);
    extern char * MKB(DEBUG_MENU_OPTION_NAMES)[7];
    extern MKB(undefined) * MKB(switchdataD_80370704);
    extern MKB(undefined) * MKB(switchdataD_80370758);
    extern MKB(undefined) * MKB(switchdataD_80370794);
    extern MKB(undefined) * MKB(switchdataD_803707c8);
    extern MKB(undefined) * MKB(switchdataD_80370848);
    extern struct MKB(Event) MKB(events)[23];
    extern MKB(undefined) * MKB(switchdataD_80370d6c);
    extern MKB(undefined) * MKB(switchdataD_80370ddc);
    extern void  * MKB(switchdataD_80370dfc);
    extern MKB(undefined) * MKB(switchdataD_80370e5c);
    extern MKB(undefined) * MKB(switchdataD_803727e4);
    extern MKB(undefined) * MKB(switchdataD_80372878);
    extern void  * MKB(switchdataD_80374aac);
    extern void  * MKB(g_some_submode_file_name_list);
    extern MKB(u8) MKB(g_active_players)[4];
    extern void (* MKB(camera_funcs)[93])(struct MKB(Camera) *, struct MKB(Ball) *);
    extern MKB(undefined) * MKB(switchdataD_80374e54);
    extern void  * MKB(switchdataD_80375030);
    extern void  * MKB(switchdataD_8037ed0c);
    extern MKB(undefined) * MKB(switchdataD_8037ed30);
    extern MKB(undefined) * MKB(switchdataD_8037ed54);
    extern void  * MKB(switchdataD_8037ed78);
    extern MKB(undefined) * MKB(switchdataD_8037edf8);
    extern struct MKB(theme_light) MKB(theme_lights)[41];
    extern char MKB(init_common_p_lz)[17];
    extern char MKB(init_common_lz)[15];
    extern struct MKB(BmpInfo) MKB(bmp_infos)[25];
    extern MKB(undefined) * MKB(bmp_tex_names);
    extern void  * MKB(switchdataD_803809d0);
    extern struct MKB(SpriteDrawRequest) MKB(ui_sprite_draw_req);
    extern undefined4 MKB(monkey_flags);
    extern void  * MKB(SEL_WORLD_PTR);
    extern MKB(undefined) MKB(g_some_music_status_array);
    extern MKB(undefined) * MKB(switchdataD_80391aa0);
    extern MKB(undefined) * MKB(switchdataD_80391ad8);
    extern MKB(undefined) * MKB(switchdataD_80391be8);
    extern MKB(undefined) * MKB(switchdataD_80391e70);
    extern undefined1 MKB(g_debug_textdraw_color);
    extern MKB(undefined) * MKB(switchdataD_8039b19c);
    extern MKB(undefined) * MKB(switchdataD_8039b280);
    extern void  * MKB(g_debug_text_buffers);
    extern void  * MKB(g_debug_color_bufers);
    extern char MKB(s_RateIntp)[12];
    extern char MKB(s_CalcIntp)[12];
    extern char MKB(s_Face_Dir)[12];
    extern char MKB(s_No_loop)[12];
    extern char * MKB(EVENT_STATUS_NAMES)[12];
    extern float MKB(physical_ball_size);
    extern float MKB(ball_accel);
    extern float MKB(ball_restitution);
    extern MKB(undefined) * MKB(ball_mode_funcs);
    extern MKB(undefined) * MKB(switchdataD_8039c560);
    extern MKB(undefined) * MKB(switchdataD_8039c5a4);
    extern void  * MKB(switchdataD_8039c5dc);
    extern struct MKB(GXColor) MKB(ball_colors)[10];
    extern MKB(undefined) * MKB(switchdataD_8039c838);
    extern void  * MKB(switchdataD_8039ccf8);
    extern undefined1 MKB(g_fog_type);
    extern MKB(s32) MKB(g_current_stage_id);
    extern MKB(s32) MKB(g_stage_id_to_load);
    extern undefined1 MKB(g_smb1_stage_list);
    extern char MKB(s_stage)[6];
    extern MKB(undefined) * MKB(LOCALE_STRINGS);
    extern struct MKB(GmaBuffer) * * MKB(g_loaded_gmas_list)[5];
    extern void  * MKB(goal_gma_model_names);
    extern undefined2 MKB(g_cached_button_itemgroup_count_stage);
    extern undefined4 MKB(cached_button_itemgroup_count);
    extern void (* MKB(effect_init_funcs)[71])(struct MKB(Effect) *);
    extern void (* MKB(effect_tick_funcs)[71])(struct MKB(Effect) *);
    extern void (* MKB(effect_disp_funcs)[71])(struct MKB(Effect) *);
    extern void (* MKB(effect_dest_funcs)[71])(struct MKB(Effect) *);
    extern MKB(WorldTheme)  MKB(world_theme);
    extern float MKB(g_bg_related_timer);
    extern struct MKB(GXColor) MKB(g_some_theme_color);
    extern undefined4 MKB(g_something_with_world_theme_2);
    extern undefined4 MKB(g_something_with_world_theme_3);
    extern MKB(s32) MKB(g_smth_for_drawing);
    extern undefined4 MKB(g_maybe_some_bg_effect_func_ptr);
    extern void * * MKB(g_some_bg_struct_ptr);
    extern undefined2 MKB(g_something_with_world_theme_4);
    extern undefined2 MKB(g_something_with_world_theme_5);
    extern struct MKB(Ape) * * MKB(BGApeTable);
    extern undefined4 MKB(g_smth_with_bg_models);
    extern struct MKB(GXColor) MKB(background_colors)[42];
    extern MKB(undefined) MKB(bg_init_funcs);
    extern MKB(undefined) MKB(bg_tick_funcs);
    extern MKB(undefined) MKB(bg_dest_funcs);
    extern void  * MKB(bg_disp_funcs);
    extern MKB(undefined) MKB(bg_item_coin_coli_funcs);
    extern char * MKB(g_bg_filename_list)[43];
    extern MKB(WorldTheme)  MKB(STAGE_WORLD_THEMES)[428];
    extern MKB(undefined) * MKB(switchdataD_8039f024);
    extern MKB(undefined) * MKB(switchdataD_8039f3f8);
    extern MKB(undefined) * MKB(switchdataD_8039f590);
    extern MKB(undefined) * MKB(switchdataD_8039f61c);
    extern MKB(undefined) * MKB(switchdataD_8039ff54);
    extern MKB(undefined) * MKB(switchdataD_803a0864);
    extern void  * MKB(cm_entry_if_funcs);
    extern void  * MKB(cm_entry_then_funcs);
    extern struct MKB(CourseCommand) MKB(beginner_noex_cm_entries)[31];
    extern struct MKB(CourseCommand) MKB(advanced_noex_cm_entries)[120];
    extern struct MKB(CourseCommand) MKB(expert_noex_cm_entries)[208];
    extern struct MKB(CourseCommand) MKB(beginner_ex_cm_entries)[35];
    extern struct MKB(CourseCommand) MKB(advanced_ex_cm_entries)[32];
    extern struct MKB(CourseCommand) MKB(expert_ex_cm_entries)[42];
    extern struct MKB(CourseCommand) MKB(master_noex_cm_entries)[35];
    extern struct MKB(CourseCommand) MKB(master_ex_cm_entries)[50];
    extern struct MKB(CourseCommand) MKB(g_cm_entry_list9)[16];
    extern struct MKB(CourseCommand) * MKB(cm_courses)[12];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world1)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world2)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world3)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world4)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world5)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world6)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world7)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world8)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world9)[10];
    extern struct MKB(SmStageInfo) MKB(sm_stage_infos_world10)[10];
    extern struct MKB(SmWorldInfo) MKB(sm_world_info)[10];
    extern MKB(undefined) * MKB(item_init_funcs);
    extern MKB(undefined) * MKB(item_tick_funcs);
    extern MKB(undefined) * MKB(item_disp_funcs);
    extern MKB(undefined) * MKB(item_coli_funcs);
    extern MKB(undefined) * MKB(item_dest_funcs);
    extern MKB(undefined) * MKB(item_replay_init_funcs);
    extern MKB(undefined) * MKB(item_debug_funcs);
    extern struct MKB(CoinType) MKB(coin_types)[5];
    extern MKB(undefined) * MKB(stobj_init_funcs);
    extern MKB(undefined) * MKB(stobj_tick_funcs);
    extern void (* MKB(stobj_disp_funcs)[14])(struct MKB(Stobj) *);
    extern MKB(undefined) * MKB(stobj_coli_funcs);
    extern MKB(undefined) * MKB(stobj_dest_funcs);
    extern MKB(undefined) * MKB(stobj_cb_f_funcs);
    extern MKB(undefined) * MKB(button_type_strs);
    extern short MKB(goal_LCD_small_common_gmatpl_idxes)[10];
    extern short MKB(goal_LCD_large_common_gmatpl_idxes)[10];
    extern void  * MKB(switchdataD_803a7210);
    extern void  * MKB(switchdataD_803a7234);
    extern MKB(undefined) * MKB(switchdataD_803a7258);
    extern MKB(undefined) * MKB(FONT_NAME_LIST);
    extern MKB(undefined) * MKB(FONT_STYLE_NAME_LIST);
    extern MKB(undefined) * MKB(switchdataD_803a8264);
    extern void  * MKB(switchdataD_803a83dc);
    extern void  * MKB(switchdataD_803a84b8);
    extern MKB(undefined) * MKB(switchdataD_803a86e8);
    extern MKB(undefined) * MKB(switchdataD_803a8770);
    extern MKB(undefined) * MKB(switchdataD_803a8794);
    extern MKB(undefined) * MKB(switchdataD_803a87b8);
    extern MKB(undefined) * MKB(switchdataD_803a87dc);
    extern void  * MKB(switchdataD_803a8800);
    extern void  * MKB(switchdataD_803a8894);
    extern MKB(undefined) * MKB(switchdataD_803a88cc);
    extern char MKB(PAUSEMENU_EXIT_GAME_STRING)[10];
    extern char * MKB(challenge_play_pausemenu_entries)[24];
    extern char * MKB(challenge_goal_pausemenu_entries)[24];
    extern char MKB(PAUSEMENU_STAGE_SELECT_STRING)[13];
    extern char * MKB(practice_play_pausemenu_entries)[36];
    extern char * MKB(practice_goal_pausemenu_entries)[36];
    extern char MKB(PAUSEMENU_GUIDE_STRING)[6];
    extern char * MKB(story_play_pausemenu_entries)[36];
    extern char * MKB(story_goal_pausemenu_entries)[36];
    extern char * * MKB(pausemenu_entry_pointers)[16];
    extern char MKB(PAUSEMENU_ON_STRING)[3];
    extern char MKB(PAUSEMENU_OFF_STRING)[4];
    extern char MKB(LOADIN_TEXT_ROUND)[9];
    extern char MKB(LOADIN_TEXT_WORLD)[12];
    extern char MKB(LOADIN_TEXT_MASTER_EX)[12];
    extern char MKB(LOADIN_TEXT_MASTER)[10];
    extern char MKB(LOADIN_TEXT_EXTRA)[9];
    extern char MKB(LOADIN_TEXT_STAGE)[9];
    extern char MKB(LOADIN_TEXT_BONUS_STAGE)[12];
    extern char MKB(LOADIN_TEXT_FINAL_ROUND)[12];
    extern char MKB(LOADIN_TEXT_FINAL_STAGE)[12];
    extern MKB(undefined) MKB(stage_name_tilde_fmt_string);
    extern char MKB(JUMP_TO_STAGE_STRING)[17];
    extern MKB(undefined) * MKB(switchdataD_803a96f8);
    extern char MKB(SPRITE_1UP_TEXT_STRING)[4];
    extern char MKB(GO_TO_THE_STAGE_STRING)[6];
    extern char MKB(POSTGOAL_SPRITES_STAGE_STRING)[15];
    extern MKB(undefined) * MKB(switchdataD_803a9e6c);
    extern void  * MKB(switchdataD_803a9ea4);
    extern MKB(undefined) * MKB(switchdataD_803a9ee8);
    extern MKB(undefined) * MKB(switchdataD_803aa0d4);
    extern void  * MKB(switchdataD_803bdd6c);
    extern void  * MKB(switchdataD_803bdda0);
    extern char MKB(HUD_TEXT_EX)[3];
    extern char MKB(HUD_TEXT_WORLD_STAGE_NO)[6];
    extern char MKB(HUD_TEXT_STAGE_NO)[3];
    extern char MKB(HUD_TEXT_STAGE_NAME)[3];
    extern char MKB(SPRITE_TEXT_FINAL_STAGE)[12];
    extern char MKB(sprite_banana_count_fmt_string)[5];
    extern MKB(MinimapMode)  MKB(g_minimap_mode);
    extern undefined4 MKB(minimap_zoom_level);
    extern undefined2 MKB(minimap_scale);
    extern undefined2 MKB(minimap_rotation);
    extern void  * MKB(switchdataD_803c0bc4);
    extern char MKB(s_cannot_read_file)[18];
    extern void  * MKB(switchdataD_803c12c8);
    extern MKB(undefined) * MKB(switchdataD_803c14b8);
    extern undefined1 MKB(g_last_used_memcard_slot_2);
    extern void  * MKB(g_some_card_funcs);
    extern void  * MKB(g_some_card_funcs2);
    extern MKB(undefined) * MKB(switchdataD_803ce82c);
    extern MKB(undefined) * MKB(switchdataD_803ce880);
    extern MKB(undefined) * MKB(switchdataD_803ce908);
    extern MKB(undefined) * MKB(switchdataD_803ce978);
    extern MKB(undefined) * MKB(switchdataD_803ce9f4);
    extern MKB(undefined) MKB(GXInitTexObjUserData);
    extern MKB(undefined) MKB(dGeomDisable);
    extern void  * MKB(switchdataD_803d1240);
    extern MKB(undefined) * MKB(RANKING_SMB1_MAIN_GAME_DEFAULT_NAMES)[3][5];
    extern undefined1 MKB(rumble_enabled_bitflag);
    extern void  * MKB(switchdataD_803d1624);
    extern void  * MKB(switchdataD_803d1c2c);
    extern MKB(undefined) * MKB(switchdataD_803d35f0);
    extern undefined4 MKB(g_mini_race_scene);
    extern undefined4 MKB(g_mini_target_scene);
    extern undefined4 MKB(g_mini_bowling_scene);
    extern undefined4 MKB(g_mini_billiards_scene);
    extern struct MKB(GComponentDef) MKB(g_aiai_lod0_skel_def2_list)[8];
    extern struct MKB(GComponentDef) MKB(g_aiai_lod1_skel_def2_list)[8];
    extern struct MKB(GComponentDef) MKB(g_aiai_lod3_lod4_skel_def2_list)[7];
    extern struct MKB(GComponentDefList) MKB(g_component_defs)[68];
    extern char * MKB(ape_lod_names)[8];
    extern MKB(undefined) * MKB(chara_names);
    extern MKB(undefined) * MKB(storymode_ape_enum);
    extern MKB(undefined) * MKB(ape_lod_enum);
    extern void  * MKB(g_mesh_type_name);
    extern MKB(undefined) * MKB(eye_string_table);
    extern MKB(undefined) * MKB(aiai_eye_model_names);
    extern char * * MKB(g_per_chara_blink_models)[17];
    extern MKB(undefined) * MKB(scene_names);
    extern char * MKB(ape_face_names)[9];
    extern MKB(undefined) * MKB(ape_face_enum_alt);
    extern MKB(undefined) * MKB(ape_face_expr_names);
    extern char MKB(s_ape)[4];
    extern MKB(undefined) * MKB(switchdataD_803d9220);
    extern char MKB(DIP_APENUM_FORMAT)[5];
    extern char MKB(s_null)[5];
    extern char MKB(s_HANDL_GHA)[10];
    extern char MKB(s_HANDL_PHA)[10];
    extern char MKB(s_HANDR_GHA)[10];
    extern char MKB(s_HANDR_PHA)[10];
    extern MKB(undefined) * MKB(RANKING_DEFAULT_NAMES);
    extern MKB(u32) MKB(RANKING_DEFAULT_SCORES)[20];
    extern MKB(u32) MKB(G_RANKING_DEFAULT_APE_IDS)[20];
    extern MKB(undefined) MKB(THPSimpleGetCurrentFrame);
    extern MKB(undefined) MKB(THPSimpleGetCurrentFrame);
    extern MKB(undefined) * MKB(switchdataD_803db7a0);
    extern MKB(undefined) * MKB(switchdataD_803db9ec);
    extern void  * MKB(switchdataD_803dca38);
    extern MKB(undefined) * MKB(switchdataD_803dca6c);
    extern MKB(undefined) * MKB(g_some_chara_anim_funcs);
    extern undefined4 MKB(g_active_scene_category);
    extern void  * MKB(switchdataD_803dd32c);
    extern MKB(undefined) * MKB(switchdataD_803dd37c);
    extern void  * MKB(switchdataD_803dd3c0);
    extern char * MKB(CUTSCENE_BIN_FILE_NAMES)[16];
    extern MKB(undefined) * MKB(switchdataD_803de11c);
    extern void  * MKB(switchdataD_803de8f8);
    extern MKB(undefined) * MKB(scene_filenames);
    extern MKB(undefined) * MKB(dialog_locale_suffix);
    extern void  * MKB(switchdataD_803ded34);
    extern MKB(undefined) * MKB(switchdataD_803ded7c);
    extern MKB(undefined) * MKB(switchdataD_803dedc4);
    extern void  * MKB(switchdataD_803dee0c);
    extern MKB(undefined) * MKB(switchdataD_803e7604);
    extern void  * MKB(switchdataD_803edb94);
    extern struct MKB(UnlockInfo) * MKB(unlock_info_ptr);
    extern undefined4 MKB(g_loaded_rel_count);
    extern struct MKB(RelBufferInfo) * MKB(g_loaded_rel_infos)[10];
    extern undefined4 MKB(frame_counter);
    extern undefined4 MKB(g_font_char_var_3);
    extern MKB(undefined) MKB(g_some_tpl_buffer);
    extern MKB(undefined) MKB(g_init_common_tpl_buf);
    extern MKB(undefined) MKB(g_some_nl_model_buffer_2);
    extern struct MKB(NlBuffer) * MKB(g_stage_nl_buf);
    extern struct MKB(NlBuffer) * * MKB(g_init_common_gma_buf);
    extern struct MKB(ScenInfo) MKB(scen_info);
    extern struct MKB(OrdTblData) * MKB(g_ord_tbl_stuff);
    extern struct MKB(OrdTblData) * MKB(ord_tbl_data);
    extern undefined4 MKB(is_more_than_24_mib);
    extern undefined4 MKB(g_some_buffer_ptr);
    extern undefined4 MKB(g_some_other_heap_lo);
    extern undefined4 MKB(g_some_other_heap_hi);
    extern undefined4 MKB(g_some_dead_heap_mem_lo);
    extern undefined4 MKB(g_some_dead_heap_mem_hi);
    extern undefined4 MKB(g_curr_heap_config_idx);
    extern undefined4 MKB(main_heap_size);
    extern undefined4 MKB(stage_heap_size);
    extern undefined4 MKB(bg_heap_size);
    extern undefined4 MKB(chara_heap_size);
    extern undefined4 MKB(replay_heap_size);
    extern MKB(MainMode)  MKB(main_mode);
    extern MKB(SubMode)  MKB(sub_mode);
    extern MKB(MainMode)  MKB(main_mode_request);
    extern MKB(SubMode)  MKB(sub_mode_request);
    extern MKB(s32) MKB(sub_mode_frame_counter);
    extern MKB(Difficulty)  MKB(curr_difficulty);
    extern MKB(ModeFlag)  MKB(mode_flags);
    extern undefined4 MKB(num_players);
    extern MKB(MainGameMode)  MKB(main_game_mode);
    extern undefined4 MKB(curr_player_idx);
    extern undefined4 MKB(mode_number_of_players);
    extern undefined2 MKB(g_has_started_a_game);
    extern undefined1 MKB(g_3player_camera_setting);
    extern bool MKB(stage_complete);
    extern MKB(WidescreenMode)  MKB(widescreen_mode);
    extern char * MKB(g_curr_main_mode_name);
    extern char * MKB(g_curr_sub_mode_name);
    extern undefined4 MKB(g_some_func_ptr_related_to_sub_mode2);
    extern void * MKB(sub_mode_destination);
    extern undefined4 MKB(test_draw_func_ptr);
    extern undefined1 MKB(g_repause_cooldown_counter);
    extern undefined4 MKB(g_related_to_pause_menu_input);
    extern undefined4 MKB(g_some_status_bitflag_maybe_pause_related);
    extern undefined4 MKB(g_current_focused_pause_menu_entry);
    extern undefined4 MKB(g_current_pause_menu_entry_count);
    extern MKB(PauseMenuType)  MKB(pausemenu_type);
    extern MKB(Status)  MKB(g_pause_status);
    extern undefined4 MKB(g_some_pausemenu_var);
    extern undefined1 MKB(g_some_pausemenu_var2);
    extern undefined4 MKB(g_some_pausemenu_var3);
    extern MKB(GRenderringFlags)  MKB(g_some_render_flag);
    extern struct MKB(Vec) MKB(g_mirror_pos1);
    extern struct MKB(Vec) MKB(g_some_scale_vec3);
    extern struct MKB(Vec) MKB(g_mirror_pos2);
    extern undefined4 MKB(g_mirror_pos3);
    extern undefined4 MKB(g_pausemenu_screenshot_requested);
    extern void * MKB(g_image_buffer_ptr);
    extern undefined2 MKB(g_pausemenu_screenshot_left_px);
    extern undefined2 MKB(g_pausemenu_screenshot_top_px);
    extern undefined2 MKB(g_pausemenu_screenshot_width_px);
    extern undefined2 MKB(g_pausemenu_screenshot_height_px);
    extern undefined4 MKB(pausemenu_scrot_texfmt);
    extern MKB(SubMode)  MKB(g_sub_mode3);
    extern undefined4 MKB(adv_demo_frame_counter);
    extern undefined1 MKB(g_focused_root_menu);
    extern undefined1 MKB(g_focused_partygame_menu);
    extern undefined1 MKB(g_focused_options_menu);
    extern undefined1 MKB(g_focused_maingame_menu);
    extern undefined1 MKB(selected_cm_difficulty2);
    extern undefined1 MKB(selected_cm_difficulty);
    extern undefined1 MKB(g_how_to_sprite_var_7);
    extern MKB(u8) MKB(selected_characters)[4];
    extern struct MKB(SelMenuInfo) MKB(sel_menu_info);
    extern MKB(s8) MKB(number_of_starting_monkeys);
    extern bool MKB(stageselect_is_storymode);
    extern MKB(s8) MKB(stageselect_course_idx)[2];
    extern MKB(s8) MKB(stageselect_course_stage_idx)[2][10];
    extern undefined1 MKB(stageselect_category_selected);
    extern undefined1 MKB(g_last_selected_bowling_difficulty);
    extern undefined4 MKB(g_auto_reload_setting);
    extern undefined1 MKB(bat_first_setting);
    extern undefined1 MKB(number_of_innings_setting);
    extern undefined1 MKB(baseball_com_level_setting);
    extern undefined4 MKB(menu_tick_func);
    extern undefined4 MKB(menu_draw_func);
    extern struct MKB(RelBufferInfo) MKB(g_some_sel_ngc_rel_buffer);
    extern undefined4 MKB(g_something_with_camera3);
    extern struct MKB(Camera) * MKB(g_current_camera);
    extern struct MKB(Camera) MKB(cameras)[5];
    extern float MKB(g_camera_aspect_ratio);
    extern undefined1 MKB(g_related_to_fov2);
    extern undefined4 MKB(g_minigame_camera_func);
    extern MKB(s16) MKB(g_camera_standstill_counters)[5];
    extern undefined4 MKB(g_active_light_group);
    extern undefined2 MKB(g_some_array_related_to_lights_1);
    extern undefined4 MKB(g_light_group_stack);
    extern MKB(u8) MKB(num_light_groups);
    extern undefined4 MKB(next_fifo_use_array_idx);
    extern MKB(u32) MKB(last_60_gx_fifo_use_sizes)[60];
    extern undefined4 MKB(tick_at_gx_finish_frame);
    extern MKB(u32) MKB(gx_fifo_use_size);
    extern MKB(BOOL32) MKB(g_video_mode_change_requested);
    extern struct MKB(ModeInfo) MKB(mode_info);
    extern undefined4 MKB(practice_best_score);
    extern undefined4 MKB(is_practice_score_new_best);
    extern struct MKB(GmaBuffer) * MKB(g_bg_gma);
    extern struct MKB(TplBuffer) * MKB(g_bg_tpl);
    extern undefined4 MKB(active_sprite_draw_req_count);
    extern undefined4 MKB(g_smth_with_sprite_draw_reqs_widescreen);
    extern struct MKB(TplBuffer) * MKB(bmp_com_tpl);
    extern undefined4 MKB(has_last_widescreen_scale);
    extern undefined2 MKB(last_widescreen_scale_pivot_x);
    extern struct MKB(SpriteDrawRequest) MKB(sprite_draw_req_buffer)[640];
    extern MKB(u32) MKB(g_profile_timer_start_times)[9];
    extern undefined4 MKB(g_something_with_perf_profiling);
    extern MKB(BOOL32) MKB(g_enable_perf);
    extern MKB(s32) MKB(g_some_draw_perf_var);
    extern undefined4 MKB(g_some_perf_time_elapsed);
    extern undefined4 MKB(some_time);
    extern undefined4 MKB(g_some_perf_timer_total_elapsed);
    extern undefined4 MKB(g_some_perf_timer1);
    extern undefined4 MKB(g_some_perf_timer2);
    extern undefined4 MKB(g_some_perf_timer3);
    extern undefined4 MKB(g_some_perf_timer4);
    extern undefined4 MKB(g_some_perf_timer5);
    extern undefined4 MKB(g_some_perf_timer6);
    extern undefined4 MKB(g_some_perf_timer7);
    extern undefined4 MKB(g_some_perf_timer8);
    extern undefined4 MKB(g_some_perf_timer9);
    extern undefined4 MKB(g_some_perf_timer10);
    extern undefined4 MKB(g_some_perf_timer11);
    extern undefined4 MKB(g_some_perf_timer12);
    extern undefined4 MKB(g_some_perf_timer14);
    extern undefined4 MKB(g_some_perf_timer13);
    extern undefined4 MKB(g_some_perf_timer15);
    extern undefined4 MKB(g_some_perf_timer17);
    extern undefined4 MKB(g_some_perf_timer18);
    extern undefined4 MKB(g_some_perf_timer16);
    extern undefined4 MKB(g_debug_sound_ram_usage);
    extern undefined4 MKB(g_debug_sound_aram_usage);
    extern MKB(s16) MKB(g_active_music_tracks)[10];
    extern undefined1 MKB(g_something_related_to_bgm_track_id);
    extern undefined1 MKB(g_some_music_related_counter);
    extern undefined4 MKB(current_bgm_volume);
    extern undefined4 MKB(g_something_with_sound5);
    extern undefined4 MKB(g_smth_with_sound);
    extern undefined4 MKB(g_player_id_for_sound);
    extern undefined4 MKB(g_debug_textdraw_pos_x_2);
    extern undefined4 MKB(g_debug_textdraw_pos_x_1);
    extern undefined4 MKB(g_debug_textdraw_pos_y);
    extern char MKB(g_debug_text_buffer_0)[1961];
    extern char MKB(g_debug_text_buffer_1)[1961];
    extern char MKB(g_debug_text_color_buffer_0)[1961];
    extern char MKB(g_debug_text_color_buffer_1)[1961];
    extern undefined2 MKB(g_debugtext_unknown7);
    extern undefined2 MKB(g_debugtext_unknown8);
    extern undefined2 MKB(g_debugtext_unknown9);
    extern undefined2 MKB(g_debugtext_unknown10);
    extern char MKB(g_debug_text_buffer_2)[1961];
    extern char MKB(g_debug_text_color_buffer_2)[1961];
    extern MKB(u16) MKB(os_font_encoding);
    extern MKB(DipSwitch)  MKB(dip_switches);
    extern MKB(GOtherFlags)  MKB(g_some_other_flags);
    extern undefined2 MKB(g_some_pad_idx);
    extern struct MKB(PoolInfo) MKB(ball_pool_info);
    extern struct MKB(PoolInfo) MKB(item_pool_info);
    extern struct MKB(PoolInfo) MKB(stobj_pool_info);
    extern struct MKB(PoolInfo) MKB(sprite_pool_info);
    extern struct MKB(PoolInfo) MKB(effect_pool_info);
    extern struct MKB(PoolInfo) MKB(g_some_pool_info2);
    extern struct MKB(PoolInfo) MKB(g_some_pool_info3);
    extern struct MKB(PoolInfo) MKB(camera_pool_info);
    extern MKB(u8) MKB(ball_status_list)[4];
    extern MKB(u8) MKB(item_status_list)[256];
    extern MKB(u8) MKB(stobj_status_list)[144];
    extern MKB(u8) MKB(sprite_status_list)[80];
    extern MKB(u8) MKB(effect_status_list)[512];
    extern MKB(u8) MKB(g_some_pool2_status_list)[16];
    extern MKB(u8) MKB(g_some_pool3_status_list)[16];
    extern MKB(u8) MKB(camera_status_list)[5];
    extern short * MKB(g_effect_flag_ptr);
    extern struct MKB(Ball) MKB(balls)[8];
    extern struct MKB(Ball) * MKB(current_ball);
    extern uint MKB(active_monkey_id)[4];
    extern undefined4 MKB(g_some_menu_func_1);
    extern MKB(u32) MKB(player_pad_map)[4];
    extern struct MKB(WorldInfo) MKB(world_infos)[4];
    extern struct MKB(Vec) MKB(g_gravity_dir);
    extern undefined4 MKB(current_world_info);
    extern int MKB(g_some_frame_counter);
    extern float MKB(g_some_frame_counter_float);
    extern undefined1 MKB(g_some_coli_nonsense1);
    extern undefined4 MKB(g_some_gameplay_flags);
    extern int * MKB(g_some_draw_var);
    extern struct MKB(Itemgroup) * MKB(itemgroups);
    extern struct MKB(StagedefFileHeader) * MKB(stagedef);
    extern undefined2 MKB(seesaw_count);
    extern undefined1 MKB(g_something_with_wl);
    extern struct MKB(TplBuffer) * MKB(stage_tpl);
    extern struct MKB(GmaBuffer) * MKB(stage_gma);
    extern undefined2 MKB(current_stage_id);
    extern char MKB(current_stage_name)[128];
    extern undefined4 MKB(worm_surface_gma_model);
    extern struct MKB(GmaModel) * MKB(wormhole_gma_model);
    extern struct MKB(GmaModel) * MKB(continue_gma_model);
    extern struct MKB(GmaModel) * MKB(goal_gma_models)[3];
    extern struct MKB(Itemgroup) * MKB(itemgroups_copy);
    extern undefined2 MKB(g_some_stage_id);
    extern undefined2 MKB(g_another_stage_id);
    extern MKB(s32) MKB(LOCALE_STGNAME_DVD_ENTRYNUMS)[6];
    extern undefined4 MKB(g_current_stagename_dvd_entry_number);
    extern undefined4 MKB(stage_name_list_file_size);
    extern int MKB(stage_name_offsets)[422];
    extern MKB(u8) MKB(g_some_32byte_lz_buffer)[32];
    extern undefined4 MKB(g_some_replay_data4);
    extern undefined4 MKB(replay_frames_remaining);
    extern struct MKB(Vec) MKB(replay_curr_ball_pos);
    extern struct MKB(Replay) * MKB(replay);
    extern undefined4 MKB(g_loaded_player_score);
    extern undefined2 MKB(next_effect_id);
    extern struct MKB(Effect) MKB(effects)[512];
    extern MKB(undefined) MKB(g_some_bg_mtx);
    extern MKB(Mtx) MKB(g_related_to_texture_UV_map);
    extern MKB(u8) MKB(cm_unlock_entries)[18];
    extern struct MKB(CourseCommand) * MKB(current_cm_entry);
    extern int MKB(stage_jump_distance);
    extern MKB(BOOL32) MKB(in_practice_mode);
    extern undefined2 MKB(g_stage_id_in_practice_mode);
    extern MKB(ModeFlag)  MKB(g_mode_flags2);
    extern undefined4 MKB(g_some_course_length);
    extern struct MKB(CmPlayerProgress) MKB(cm_player_progress)[12];
    extern struct MKB(SmWorldState) MKB(sm_state)[10];
    extern MKB(u8) MKB(storymode_unlock_entries)[13];
    extern undefined2 MKB(g_next_item_id);
    extern struct MKB(Item) MKB(items)[256];
    extern undefined2 MKB(next_stobj_id);
    extern struct MKB(Stobj) MKB(stobjs)[144];
    extern undefined4 MKB(bumper_lods);
    extern struct MKB(GoalTape) MKB(goaltapes)[16];
    extern struct MKB(GoalBag) MKB(goalbags)[16];
    extern struct MKB(GmaModel) * MKB(goal_LCD_small_numbers)[10];
    extern struct MKB(GmaModel) * MKB(goal_LCD_large_numbers)[10];
    extern struct MKB(Sprite) MKB(sprites)[80];
    extern struct MKB(GSomeSpriteStruct) MKB(g_some_sprite_structs)[82];
    extern undefined4 MKB(g_some_sprite_width);
    extern undefined4 MKB(g_some_sprite_height);
    extern undefined4 MKB(g_screenfade_flags);
    extern MKB(u32) MKB(g_screenfade_color);
    extern undefined4 MKB(g_screenfading1);
    extern undefined4 MKB(g_screenfading2);
    extern void * MKB(g_some_font_ptr_array)[512];
    extern MKB(s32) MKB(g_related_to_aram_font_pages)[145];
    extern float MKB(textdraw_pos_x);
    extern struct MKB(Vec2d) MKB(textdraw_pos);
    extern MKB(Font16)  MKB(textdraw_font);
    extern undefined4 MKB(textdraw_mul_color);
    extern undefined4 MKB(textdraw_add_color);
    extern undefined2 MKB(textdraw_rot_z);
    extern float MKB(textdraw_depth);
    extern float MKB(textdraw_width);
    extern float MKB(textdraw_height);
    extern undefined4 MKB(textdraw_alpha);
    extern MKB(TextDrawFlags)  MKB(textdraw_flags);
    extern undefined1 MKB(textdraw_alignment);
    extern undefined2 MKB(g_textdraw_smth_with_letter_spacing_and_alignment);
    extern undefined2 MKB(textdraw_drop_shadow_offset_x);
    extern undefined2 MKB(textdraw_drop_shadow_offset_y);
    extern float MKB(textdraw_shadow_alpha);
    extern undefined4 MKB(g_textdraw_unk5);
    extern undefined4 MKB(g_textdraw_unk6);
    extern undefined4 MKB(g_textdraw_unk7);
    extern undefined1 MKB(textdraw_style);
    extern struct MKB(Vec2d) MKB(textdraw_spacing);
    extern undefined2 MKB(g_textdraw_counter);
    extern undefined4 MKB(g_textdraw_unk8);
    extern undefined4 MKB(g_font_char_var_1);
    extern undefined4 MKB(g_font_char_var_2);
    extern undefined4 MKB(g_smth_with_how_to_sprite_mode);
    extern undefined1 MKB(g_how_to_sprite_var_3);
    extern undefined1 MKB(g_how_to_sprite_var_1);
    extern undefined1 MKB(g_how_to_sprite_var_4);
    extern undefined1 MKB(g_how_to_sprite_var_6);
    extern undefined1 MKB(g_how_to_sprite_var_8);
    extern undefined1 MKB(g_how_to_sprite_var_5);
    extern float MKB(g_how_to_bg_scale_x);
    extern float MKB(g_how_to_bg_scale_y);
    extern undefined1 MKB(g_how_to_sprite_var_2);
    extern MKB(u8) MKB(g_banana_disp_efc_req_count);
    extern MKB(undefined) MKB(g_banana_disp_efc_stack)[10];
    extern undefined4 MKB(global_ape_lod);
    extern undefined4 MKB(g_is_waiting_on_some_arq_request);
    extern struct MKB(gDiscQueueItem) MKB(discQueue)[128];
    extern undefined4 MKB(disc_queue_start);
    extern undefined4 MKB(disc_queue_end);
    extern undefined4 MKB(discQueueGroup);
    extern struct MKB(FileCacheEntry) MKB(file_cache_entries)[128];
    extern undefined4 MKB(g_last_filename_attempted_to_open);
    extern undefined4 MKB(g_aram_read_pending);
    extern undefined4 MKB(g_minigame_tick_func);
    extern undefined4 MKB(g_mini_draw_func_ptr);
    extern undefined4 MKB(shadow_req_capacity);
    extern struct MKB(ShadowReq) * MKB(next_shadow_req);
    extern struct MKB(ShadowReq) MKB(shadow_reqs)[64];
    extern MKB(undefined) MKB(g_haze_type);
    extern undefined1 MKB(g_override_clear_r);
    extern undefined1 MKB(g_override_clear_g);
    extern undefined1 MKB(g_override_clear_b);
    extern undefined4 MKB(g_some_flag_relating_to_pausing);
    extern struct MKB(MemCardInfo) MKB(memcard_infos)[2];
    extern undefined1 MKB(g_last_used_memcard_slot);
    extern undefined4 MKB(g_result_code);
    extern undefined1 MKB(g_some_memcard_var);
    extern undefined1 MKB(g_maybe_last_selected_memcard_slot);
    extern MKB(undefined) MKB(g_storymode_file_memcard_buffer);
    extern struct MKB(StoryModeSaveFile) MKB(storymode_save_files)[3];
    extern undefined1 MKB(g_storymode_unlock_entries_copy);
    extern undefined1 MKB(g_cm_unlock_entries_copy);
    extern bool MKB(autosave);
    extern MKB(undefined) MKB(g_some_string);
    extern struct MKB(GSomethingWithPadMotorsStruct) MKB(g_some_pad_motor_array1)[4];
    extern struct MKB(GSomethingWithPadMotorsStruct) MKB(g_some_pad_motor_array2)[4];
    extern MKB(OSHeapHandle) MKB(g_some_heap_handle);
    extern float MKB(g_avdisp_bound_sphere_scale);
    extern float MKB(g_avdisp_material_alpha);
    extern MKB(Mtx) MKB(avdisp_tex_mtx);
    extern struct MKB(Component) MKB(g_main_components)[17];
    extern struct MKB(Component) MKB(g_face_components)[17];
    extern struct MKB(Component) MKB(g_handr_components)[17];
    extern struct MKB(Component) MKB(g_tail_component);
    extern struct MKB(Component) MKB(g_handl_components)[17];
    extern void * MKB(common_anim_storage)[17];
    extern void * MKB(game_mal_storage)[17];
    extern struct MKB(ApeArchive) * MKB(skel_ska);
    extern void * MKB(common_mal_sizes)[17];
    extern int MKB(game_mal_sizes)[17];
    extern void * MKB(ape_tpls)[68];
    extern void * MKB(rep_tpl_table)[17];
    extern void * MKB(ape_gma_table)[68];
    extern struct MKB(ARCHandle) MKB(arc_file_handle);
    extern void * MKB(skl_arc);
    extern MKB(Mtx) MKB(g_common_ape_mtx);
    extern float MKB(g_ape_alphas)[64];
    extern int MKB(ape_ref_count_table)[136];
    extern struct MKB(Ape) * MKB(registered_apes)[64];
    extern undefined4 MKB(total_apes_registered);
    extern undefined4 MKB(g_ptr_to_something);
    extern undefined1 MKB(g_nameentry_state);
    extern undefined1 MKB(g_some_gift_menu_flags);
    extern bool MKB(g_nameentry_did_get_top_5);
    extern undefined4 MKB(scene_data);
    extern undefined4 MKB(dialog_tbl);
    extern undefined2 MKB(g_author_frame);
    extern undefined2 MKB(scene_length);
    extern undefined4 MKB(g_some_author_related_data);
    extern undefined4 MKB(g_author_scene);
    extern undefined4 MKB(g_new_main_mode_req);
    extern undefined4 MKB(g_new_sub_mode_req);
    extern undefined4 MKB(g_some_author_tick_func);
    extern undefined4 MKB(g_some_author_cutscene_timer);
    extern undefined4 MKB(g_some_author_cutscene_flag);
    extern undefined4 MKB(g_playpoint_msg_counter);
    extern struct MKB(UnlockInfo) MKB(unlock_info);
    extern float MKB(view_stage_aspect_ratio);
    extern void  * MKB(switchdataD_804ee064);
    extern MKB(undefined) * MKB(story_mode_funcs);
    extern struct MKB(StoryStagePreviewParam) MKB(MAIN_GAME_STORY_STAGE_PREVIEW_PARAMS)[100];
    extern undefined1 MKB(ape_story_select_anim_table);
    extern void  * MKB(switchdataD_804eee68);
    extern void  * MKB(switchdataD_804eee88);
    extern void  * MKB(g_unk_difficulty1);
    extern void  * MKB(switchdataD_804eef70);
    extern void  * MKB(switchdataD_804ef084);
    extern void  * MKB(switchdataD_804ef11c);
    extern MKB(undefined) MKB(CHAR_SPACE_3);
    extern MKB(undefined) MKB(CHAR_SPACE_2);
    extern undefined4 MKB(CHAR_A);
    extern undefined4 MKB(CHAR_B);
    extern undefined4 MKB(CHAR_C);
    extern undefined4 MKB(CHAR_D);
    extern undefined4 MKB(CHAR_E);
    extern undefined4 MKB(CHAR_F);
    extern undefined4 MKB(CHAR_G);
    extern undefined4 MKB(CHAR_H);
    extern undefined4 MKB(CHAR_I);
    extern undefined4 MKB(CHAR_J);
    extern undefined4 MKB(CHAR_K);
    extern undefined4 MKB(CHAR_L);
    extern undefined4 MKB(CHAR_M);
    extern undefined4 MKB(CHAR_N);
    extern undefined4 MKB(CHAR_O);
    extern undefined4 MKB(CHAR_P);
    extern undefined4 MKB(CHAR_Q);
    extern undefined4 MKB(CHAR_R);
    extern undefined4 MKB(CHAR_S);
    extern undefined4 MKB(CHAR_T);
    extern undefined4 MKB(CHAR_U);
    extern undefined4 MKB(CHAR_V);
    extern undefined4 MKB(CHAR_W);
    extern undefined4 MKB(CHAR_X);
    extern undefined4 MKB(CHAR_Y);
    extern undefined4 MKB(CHAR_Z);
    extern undefined4 MKB(CHAR_SPACE);
    extern undefined4 MKB(CHAR_a);
    extern undefined4 MKB(CHAR_b);
    extern undefined4 MKB(CHAR_c);
    extern undefined4 MKB(CHAR_d);
    extern undefined4 MKB(CHAR_e);
    extern undefined4 MKB(CHAR_f);
    extern undefined4 MKB(CHAR_g);
    extern undefined4 MKB(CHAR_h);
    extern undefined4 MKB(CHAR_i);
    extern undefined4 MKB(CHAR_j);
    extern undefined4 MKB(CHAR_k);
    extern undefined4 MKB(CHAR_l);
    extern undefined4 MKB(CHAR_m);
    extern undefined4 MKB(CHAR_n);
    extern undefined4 MKB(CHAR_o);
    extern undefined4 MKB(CHAR_p);
    extern undefined4 MKB(CHAR_q);
    extern undefined4 MKB(CHAR_r);
    extern undefined4 MKB(CHAR_s);
    extern undefined4 MKB(CHAR_t);
    extern undefined4 MKB(CHAR_u);
    extern undefined4 MKB(CHAR_v);
    extern undefined4 MKB(CHAR_x);
    extern undefined4 MKB(CHAR_y);
    extern undefined4 MKB(CHAR_z);
    extern undefined4 * * * MKB(nameentry_character_ptr_list);
    extern MKB(undefined) * MKB(DEFAULT_FILENAME);
    extern void  * MKB(DEFAULT_FILENAME_LOCALE_LIST);
    extern char MKB(continue_saved_game_text)[40];
    extern char MKB(start_game_from_beginning_text)[35];
    extern MKB(undefined) * MKB(switchdataD_804f0588);
    extern MKB(undefined) * MKB(switchdataD_804f0734);
    extern byte MKB(g_some_staff_roll_alignment_array);
    extern undefined4 MKB(g_staff_roll_big_banana_count_2);
    extern undefined4 MKB(staff_roll_item_percentage);
    extern undefined4 MKB(staff_roll_all_bananas_bonus);
    extern undefined4 MKB(staff_roll_all_big_bananas_bonus);
    extern undefined4 MKB(staff_roll_all_items_bonus);
    extern undefined4 MKB(staff_roll_all_letters_bonus);
    extern undefined4 MKB(staff_roll_no_letters_bonus);
    extern MKB(undefined) * MKB(switchdataD_805435f0);
    extern void  * MKB(switchdataD_80543614);
    extern void  * MKB(switchdataD_80543648);
    extern MKB(undefined) * MKB(switchdataD_80543690);
    extern undefined4 MKB(staff_roll_results_monkey_rating_color);
    extern MKB(undefined) MKB(staff_roll_results_monkey_rating_fmt_string);
    extern char * MKB(staff_roll_results_monkey_rating_buffer);
    extern MKB(undefined) * MKB(switchdataD_805437a4);
    extern MKB(undefined) * MKB(switchdataD_805437cc);
    extern MKB(undefined) * MKB(switchdataD_80543840);
    extern MKB(undefined) * MKB(switchdataD_80543868);
    extern char * * MKB(scen_stgname_buffer);
    extern undefined4 MKB(g_scen_stage_names_loaded);
    extern undefined4 MKB(g_are_story_select_sprites_visible);
    extern undefined2 MKB(g_amount_of_stages_per_world);
    extern undefined2 MKB(g_amount_of_beaten_stages_in_world);
    extern undefined4 MKB(g_difficulty_y_offset);
    extern undefined2 MKB(g_story_select_sprites_anim_state);
    extern undefined2 MKB(g_story_select_sprites_timer);
    extern float MKB(some_ape_float);
    extern float MKB(some_ape_float2);
    extern undefined4 MKB(g_some_story_select_sprite_timer);
    extern undefined4 MKB(g_some_story_select_sprite_timer_2);
    extern undefined4 MKB(g_some_story_select_sprite_initial_timer);
    extern float MKB(some_ape_float3);
    extern struct MKB(SpriteTex) MKB(g_storymode_preview_textures);
    extern MKB(StoryModeStageSelectState)  MKB(g_storymode_stageselect_state);
    extern undefined2 MKB(g_storymode_stageselect_framecounter);
    extern undefined4 MKB(g_some_storymode_stageid1);
    extern undefined4 MKB(g_some_storymode_stageid2);
    extern undefined4 MKB(g_some_storymode_stageid3);
    extern char * MKB(story_file_name)[6];
    extern undefined1 MKB(g_some_nameentry_length);
    extern undefined1 MKB(g_some_nameentry_flag);
    extern bool MKB(g_storymode_are_you_sure_answer);
    extern MKB(DataSelectMenuState)  MKB(data_select_menu_state);
    extern undefined1 MKB(story_file_select_state);
    extern undefined1 MKB(selected_story_file_idx);
    extern undefined1 MKB(g_dataselect_menu_framecounter);
    extern MKB(StoryModeMenuState)  MKB(storymode_menu_state);
    extern undefined4 MKB(staff_roll_final_score);
    extern undefined4 MKB(staff_roll_banana_count);
    extern undefined4 MKB(staff_roll_big_banana_count);
    extern undefined4 MKB(staff_roll_item_count);
    extern undefined4 MKB(staff_roll_hittable_object_count);
    extern undefined4 MKB(staff_roll_hit_percentage);
    extern undefined4 MKB(g_some_pausemenu_var4);
    extern undefined4 MKB(staff_roll_banana_percentage);
    extern struct MKB(Ape) * MKB(g_some_ape);
    extern undefined4 MKB(view_stage_camera_x);
    extern undefined4 MKB(view_stage_camera_y);
    extern undefined4 MKB(view_stage_camera_z);
    extern undefined2 MKB(view_stage_camera_rot_x);
    extern undefined2 MKB(view_stage_camera_rot_y);
    extern undefined2 MKB(view_stage_camera_rot_z);
    extern MKB(s16) MKB(view_stage_timer);
    extern undefined4 MKB(view_stage_camera_zoom);
    extern MKB(u16) MKB(view_stage_camera_horizontal_rotation);
    extern undefined2 MKB(view_stage_camera_vertical_pan);
    extern undefined2 MKB(view_stage_camera_horizontal_pan);
    extern MKB(undefined) * MKB(switchdataD_80574504);
    extern MKB(undefined) * MKB(switchdataD_80574614);
    extern void  * MKB(switchdataD_805747cc);
    extern void  * MKB(switchdataD_805748e8);
    extern void  * MKB(switchdataD_805749f8);
    extern void  * MKB(switchdataD_80574bb0);
    extern char MKB(CAN_PLAY_NUM_PARTY_GAMES_NEED_UNLOCK_STRING)[124];
    extern struct MKB(MenuEntry) MKB(menu_mode_select_entries);
    extern char MKB(CAN_PLAY_NUM_PARTY_GAMES_STRING)[49];
    extern MKB(undefined) MKB(menu_party_game_select_entries);
    extern char MKB(CAN_PURCHASE_PARTY_GAME_STRING)[93];
    extern char MKB(CANNOT_SELECT_PARTY_GAME_STRING)[84];
    extern MKB(undefined) MKB(menu_option_entries);
    extern char MKB(NUM_OF_PLAYERS_DESCRIPTION)[36];
    extern struct MKB(MenuEntry) MKB(menu_number_of_players_entries);
    extern char MKB(NUM_OF_PLAYERS_DESCRIPTION_PLAYPOINTS)[67];
    extern char MKB(NUM_OF_PLAYERS_DESCRIPTION_NO_PLAYPOINTS)[71];
    extern struct MKB(MenuEntry) MKB(menu_character_select_1_entries)[4];
    extern char MKB(CHARACTER_SELECT_DESCRIPTION_UNUSED)[76];
    extern struct MKB(MenuEntry) MKB(menu_character_select_2_entries)[4];
    extern char MKB(MENU_STORY_DESCRIPTION_TEXT)[151];
    extern char MKB(MENU_CHALLENGE_DESCRIPTION_TEXT)[135];
    extern struct MKB(MenuEntry) MKB(menu_main_game_select_entries)[3];
    extern MKB(undefined) MKB(menu_level_select_1_entries);
    extern MKB(undefined) MKB(menu_level_select_2_entries);
    extern MKB(undefined) MKB(menu_stage_select_entries);
    extern MKB(undefined) MKB(menu_gameplay_settings_entries);
    extern MKB(undefined) MKB(menu_race_mode_select_entries);
    extern MKB(undefined) MKB(menu_race_course_select_one_course_race_entries);
    extern struct MKB(MenuEntry) MKB(menu_race_course_select_time_trial_entries)[6];
    extern MKB(undefined) MKB(menu_race_gameplay_settings_one_course_race_entries);
    extern MKB(undefined) MKB(menu_race_gameplay_settings_grand_prix_entries);
    extern MKB(undefined) MKB(menu_fight_mode_select_entries);
    extern MKB(undefined) MKB(menu_fight_no_of_wins_entries);
    extern MKB(undefined) MKB(menu_fight_stage_select_entries);
    extern MKB(undefined) MKB(menu_fight_gameplay_settings_normal_entries);
    extern MKB(undefined) MKB(menu_fight_gameplay_settings_survival_entries);
    extern MKB(undefined) MKB(menu_target_empty_entries);
    extern MKB(undefined) MKB(menu_target_gameplay_settings_entries);
    extern MKB(undefined) MKB(menu_billiards_mode_select_entries);
    extern MKB(undefined) MKB(menu_billiards_rule_select_entries);
    extern MKB(undefined) MKB(menu_billiards_game_settings_multiplayer_entries);
    extern MKB(undefined) MKB(menu_billiards_game_settings_1p_entries);
    extern MKB(undefined) MKB(menu_bowling_mode_select_entries);
    extern MKB(undefined) MKB(menu_bowling_rule_select_entries);
    extern MKB(undefined) MKB(menu_bowling_level_select_entries);
    extern MKB(undefined) MKB(menu_golf_mode_select_entries);
    extern MKB(undefined) MKB(menu_golf_gameplay_settings_entries);
    extern MKB(undefined) MKB(menu_boat_mode_select_entries);
    extern MKB(undefined) MKB(menu_boat_course_select_one_course_race_entries);
    extern MKB(undefined) MKB(menu_boat_course_select_time_attack_entries);
    extern MKB(undefined) MKB(menu_boat_game_settings_one_course_race_entries);
    extern MKB(undefined) MKB(menu_boat_game_settings_grand_prix_entries);
    extern MKB(undefined) MKB(menu_shot_empty_entries);
    extern MKB(undefined) MKB(menu_shot_stage_select_entries);
    extern char MKB(SHOT_AUTO_RELOAD_OPTION_STRING)[12];
    extern char MKB(SHOT_AUTO_RELOAD_DESCRIPTION_STRING)[55];
    extern MKB(undefined) MKB(menu_shot_gameplay_settings_1p_entries);
    extern MKB(undefined) MKB(menu_shot_gameplay_settings_2p_entries);
    extern MKB(undefined) MKB(menu_shot_gameplay_settings_3p_entries);
    extern MKB(undefined) MKB(menu_shot_gameplay_settings_4p_entries);
    extern MKB(undefined) MKB(menu_dogfight_mode_select_entries);
    extern MKB(undefined) MKB(menu_dogfight_stage_select_entries);
    extern MKB(undefined) MKB(menu_dogfight_game_settings_normal_entries);
    extern MKB(undefined) MKB(menu_dogfight_game_settings_survival_entries);
    extern MKB(undefined) MKB(menu_soccer_mode_select_entries);
    extern MKB(undefined) MKB(menu_soccer_team_select_entries);
    extern MKB(undefined) MKB(menu_soccer_game_settings_no_com_entries);
    extern MKB(undefined) MKB(menu_soccer_game_settings_com_entries);
    extern MKB(undefined) MKB(menu_baseball_mode_select_entries);
    extern MKB(undefined) MKB(menu_baseball_character_select_entries);
    extern MKB(undefined) MKB(menu_baseball_stadium_select_entries);
    extern MKB(undefined) MKB(menu_baseball_game_settings_com_entries);
    extern MKB(undefined) MKB(menu_baseball_game_settings_no_com_entries);
    extern MKB(undefined) MKB(menu_tennis_mode_select_entries);
    extern MKB(undefined) MKB(menu_tennis_pair_select_2p_entries);
    extern MKB(undefined) MKB(menu_tennis_pair_select_3p_entries);
    extern MKB(undefined) MKB(menu_tennis_pair_select_4p_entries);
    extern MKB(undefined) MKB(menu_tennis_court_select_entries);
    extern MKB(undefined) MKB(menu_tennis_game_settings_com_entries);
    extern MKB(undefined) MKB(menu_tennis_game_settings_no_com_entries);
    extern MKB(undefined) MKB(menu_option_replay_entries);
    extern MKB(undefined) MKB(menu_option_play_points_entries);
    extern MKB(undefined) MKB(menu_option_gift_entries);
    extern MKB(undefined) MKB(menu_option_ranking_entries);
    extern MKB(undefined) MKB(menu_option_game_data_entries);
    extern MKB(undefined) MKB(menu_option_controller_entries);
    extern MKB(undefined) MKB(menu_option_screen_entries);
    extern struct MKB(MenuScreen) MKB(menu_screen_list)[88];
    extern void  * MKB(switchdataD_80580b70);
    extern MKB(undefined) MKB(menu_bmp_ids);
    extern char MKB(SPRITE_BUTTON_LESSON_TEXT)[7];
    extern char MKB(SPRITE_BUTTON_TUTORIAL_TEXT)[9];
    extern MKB(undefined) * MKB(switchdataD_805837fc);
    extern void  * MKB(switchdataD_80584584);
    extern MKB(undefined) * MKB(switchdataD_805846ac);
    extern MKB(MenuScreenID)  MKB(g_currently_visible_menu_screen);
    extern MKB(MenuScreenID)  MKB(g_next_menu_screen);
    extern undefined1 MKB(g_menu_transition_frametimer);
    extern byte MKB(g_character_selected);
    extern MKB(u32) MKB(g_menu_frame_count_until_input_locked);
    extern char * MKB(g_menu_description_text);
    extern undefined4 MKB(g_active_pads_bitfield);
    extern undefined1 MKB(g_language_id);
    extern undefined4 MKB(locked_menu_items);
    extern struct MKB(Ape) * MKB(menu_apes)[4];
    extern undefined4 MKB(g_menu_color_overlay_timer);
    extern MKB(undefined) MKB(g_something_with_preview_textures);
    extern undefined4 MKB(option_game_data_current_operation);
    extern undefined4 MKB(g_current_game_data_selection);
    extern undefined4 MKB(g_replay_stage_id_to_load);
    extern undefined1 MKB(g_gift_menu_cursor_pos);
    extern MKB(undefined) * MKB(switchdataD_80590050);
    extern void  * MKB(switchdataD_805b1fc6);
    extern MKB(undefined) MKB(s_bmp);
    extern MKB(undefined) MKB(s_ape_);
    extern void  * MKB(switchdataD_805b20d2);
    extern void  * MKB(switchdataD_805b22ae);
    extern void  * MKB(switchdataD_805b22ea);
    extern MKB(undefined) * MKB(switchdataD_8060470c);
    extern void  * MKB(switchdataD_80604780);
    extern void  * MKB(switchdataD_806047a0);
    extern void  * MKB(switchdataD_806047cc);
    extern MKB(undefined) * MKB(switchdataD_806047ec);
    extern MKB(undefined) * MKB(switchdataD_8060480c);
    extern MKB(undefined) * MKB(switchdataD_80604854);
    extern MKB(undefined) * MKB(switchdataD_80604b5c);
    extern void  * MKB(switchdataD_80605210);
    extern MKB(undefined) * MKB(test_mode_sub_mode_funcs);
    extern void  * MKB(switchdataD_80638fc4);
    extern MKB(undefined) * MKB(switchdataD_8063d714);
    extern void  * MKB(switchdataD_8063d738);
    extern MKB(undefined) * MKB(switchdataD_8063ec08);
    extern void  * MKB(switchdataD_80642514);
    extern MKB(undefined) * MKB(switchdataD_80642534);
    extern undefined4 MKB(g_debug_sound_fx_id);
    extern undefined1 MKB(g_debug_sound_fx_pan);
    extern undefined2 MKB(g_debug_sound_fx_pitch);
    extern undefined2 MKB(g_debug_sound_fx_doppler);
    extern undefined2 MKB(g_debug_sound_fx_mod);
    extern undefined1 MKB(g_debug_sound_fx_reverb);
    extern undefined1 MKB(g_debug_sound_fx_chorus);
    extern undefined1 MKB(g_debug_sound_ics);
    extern undefined2 MKB(g_debug_stream_se_id);
    extern undefined1 MKB(g_debug_sound_ics_volume);
    extern undefined1 MKB(g_debug_sound_ics_pan_L);
    extern undefined1 MKB(g_debug_sound_ics_pan_R);
    extern undefined2 MKB(g_debug_stream_bgm_id);
    extern undefined1 MKB(g_debug_sound_efc);
    extern MKB(Font16)  MKB(test_aram_font_type);
    extern undefined1 MKB(test_aram_font_proportional);
    extern undefined1 MKB(test_aram_font_style);
    extern undefined1 MKB(test_aram_font_page);
    extern undefined4 MKB(test_aram_font_char);
    extern MKB(u16) MKB(g_something_with_test_cutscenes);
    extern MKB(u16) MKB(test_scene_number);
    extern undefined2 MKB(g_test_dialog_category_idx);
    extern undefined2 MKB(g_test_dialog_data_idx);
    extern struct MKB(GDialogStruct) MKB(g_test_dialog_ptr);
    extern struct MKB(Ape) * MKB(g_test_ape_ptr);
    extern MKB(undefined) * MKB(switchdataD_8065c7a0);
    extern void  * MKB(switchdataD_80685ba0);
    extern void  * MKB(switchdataD_80686b64);
    extern void  * MKB(switchdataD_80686c20);
    extern undefined2 MKB(player_count);
    extern undefined4 MKB(race_frames_remaining);
    extern float MKB(golf_max_wind_velocity_divisor);
    extern MKB(undefined) * MKB(switchdataD_806c6ec4);
    extern MKB(undefined) * MKB(switchdataD_806c6ef8);
    extern MKB(undefined) * MKB(switchdataD_806c6fe0);
    extern MKB(undefined) * MKB(switchdataD_806c7010);
    extern MKB(undefined) * MKB(switchdataD_806c7058);
    extern MKB(undefined) * MKB(switchdataD_806c7418);
    extern MKB(undefined) * MKB(switchdataD_806c7434);
    extern MKB(undefined) * MKB(switchdataD_806c777c);
    extern MKB(undefined) * MKB(switchdataD_806cb4ac);
    extern MKB(undefined) * MKB(switchdataD_806cb6b8);
    extern void  * MKB(switchdataD_806cb6fc);
    extern void  * MKB(switchdataD_806cb784);
    extern undefined4 MKB(golf_wind_angle);
    extern float MKB(golf_wind_velocity);
    extern void  * MKB(switchdataD_806fd0d8);
    extern void  * MKB(switchdataD_806fd190);
    extern void  * MKB(switchdataD_80706bac);
    extern MKB(undefined) * MKB(switchdataD_8073c160);
    extern MKB(undefined) * MKB(switchdataD_8073c244);
    extern MKB(undefined) * MKB(switchdataD_8073c6dc);
    extern MKB(undefined) * MKB(switchdataD_8073c920);
    extern void  * MKB(switchdataD_807b8b94);
    extern void  * MKB(switchdataD_807b8bbc);
    extern MKB(undefined) * MKB(switchdataD_807b9ca8);
    extern void  * MKB(switchdataD_80801518);
    extern MKB(undefined) * MKB(switchdataD_80801724);
    extern MKB(undefined) * MKB(switchdataD_80804710);
    extern MKB(undefined) * MKB(switchdataD_808063ec);
    extern MKB(undefined) * MKB(switchdataD_8080640c);
    extern MKB(undefined) * MKB(switchdataD_8080642c);
    extern MKB(undefined) * MKB(switchdataD_8080644c);
    extern MKB(undefined) * MKB(switchdataD_808064ac);
    extern MKB(undefined) * MKB(switchdataD_8080667c);
    extern void  * MKB(switchdataD_80806734);
    extern MKB(undefined) * MKB(switchdataD_80806970);
    extern MKB(undefined) * MKB(switchdataD_808069a0);
    extern MKB(undefined) * MKB(switchdataD_808609e4);
    extern MKB(undefined) * MKB(switchdataD_80860a20);
    extern MKB(undefined) * MKB(switchdataD_80860a90);
    extern MKB(undefined) * MKB(switchdataD_80860ac0);
    extern MKB(undefined) * MKB(switchdataD_80860ae0);
    extern MKB(undefined) * MKB(switchdataD_80860c8c);
    extern MKB(undefined) * MKB(switchdataD_80860ca8);
    extern MKB(undefined) * MKB(switchdataD_80860cd8);
    extern MKB(undefined) * MKB(switchdataD_808ddd64);
    extern void  * MKB(switchdataD_808ddfa8);
    extern MKB(undefined) * MKB(switchdataD_808ddfc8);
    extern void  * MKB(switchdataD_809958d8);
    extern MKB(undefined) * MKB(switchdataD_80995924);
    extern void  * MKB(switchdataD_8099ca88);
    extern MKB(undefined) * MKB(switchdataD_8099cb74);
    extern MKB(undefined) * MKB(switchdataD_8099f914);
    extern MKB(undefined) * MKB(switchdataD_809e535c);
    extern MKB(undefined) * MKB(switchdataD_809e8b58);
    extern MKB(undefined) * MKB(switchdataD_809e8b78);
    extern MKB(undefined) * MKB(switchdataD_809eaa04);
    extern MKB(undefined) * MKB(switchdataD_809eaa28);
    extern MKB(undefined) * MKB(switchdataD_809eaa4c);
    extern MKB(undefined) * MKB(switchdataD_809eaa70);
    extern MKB(undefined) * MKB(switchdataD_809eaa8c);
    extern MKB(undefined) * MKB(switchdataD_809eaaac);
    extern MKB(undefined) * MKB(switchdataD_809eaaf4);
    extern MKB(undefined) * MKB(switchdataD_809eab18);
    extern MKB(undefined) * MKB(switchdataD_809eab34);
    extern MKB(undefined) * MKB(switchdataD_809eab50);
    extern MKB(undefined) * MKB(switchdataD_809eab70);
    extern MKB(undefined) * MKB(switchdataD_809eab8c);
    extern MKB(undefined) * MKB(switchdataD_809eabac);
    extern MKB(undefined) * MKB(switchdataD_809eabf4);
    extern MKB(undefined) * MKB(switchdataD_809eac2c);
    extern MKB(undefined) * MKB(switchdataD_809eac48);
    extern MKB(undefined) * MKB(switchdataD_809eadc4);
    extern void  * MKB(switchdataD_809eae20);
    extern void  * MKB(switchdataD_809eae94);
    extern void  * MKB(switchdataD_809f17e8);
    extern volatile undefined2 MKB(g_some_video_bitflag);
    extern volatile union MKB(PPCWGPipe) MKB(GXWGFifo);

    /* Function decls */
    void MKB(__check_pad3)(void);
    void MKB(dol_entry)(void);
    void MKB(__init_registers)(void);
    void MKB(__init_data)(void);
    void MKB(__init_hardware)(void);
    void MKB(__flush_cache)(uint param_1, int param_2);
    void * MKB(memset)(void * ptr, int value, MKB(size_t) num);
    void * MKB(__fill_mem)(void * ptr, int value, MKB(size_t) num);
    void * MKB(memcpy)(void * dest, void * src, MKB(size_t) count);
    int MKB(TRK_memset)(int param_1, byte param_2, uint param_3);
    void MKB(TRK_memcpy)(int param_1, int param_2, int param_3);
    void MKB(g_init_console)(void);
    void MKB(g_some_heap_init)(void);
    void MKB(load_run_main_loop_parent1)(void);
    uint MKB(read_and_set_os_string_table)(void);
    void MKB(load_main_loop_rel_and_run)(char * rel_filepath, struct MKB(RelBufferInfo) * buffer_info);
    int * MKB(unload_main_loop_rel)(int * mainLoopBufPtrs);
    void MKB(load_common_gma_tpl)(void);
    void MKB(set_init_rel_index)(MKB(u32) index);
    undefined4 MKB(g_clear_prev_GX_settings_something1)(void);
    void MKB(g_clear_prev_GX_settings_something2)(void);
    void MKB(GXSetCullMode_cached)(MKB(GXCullMode)  mode);
    void MKB(GXSetBlendMode_cached)(MKB(GXBlendMode)  type, MKB(GXBlendFactor)  src_factor, MKB(GXBlendFactor)  dst_factor, MKB(GXLogicOp)  op);
    void MKB(GXSetTevSwapModeTable_cached)(MKB(GXTevSwapSel)  id, MKB(GXTevColorChan)  red, MKB(GXTevColorChan)  green, MKB(GXTevColorChan)  blue, MKB(GXTevColorChan)  alpha);
    void MKB(g_GXSetTevSwapModeTable_wrapper)(MKB(GXTevSwapSel)  param_1, MKB(GXTevColorChan)  param_2, MKB(GXTevColorChan)  param_3, MKB(GXTevColorChan)  param_4, MKB(GXTevColorChan)  param_5);
    void MKB(GXSetTevSwapMode_cached)(MKB(GXTevStageID)  stage, MKB(GXTevSwapSel)  ras_sel, MKB(GXTevSwapSel)  tex_sel);
    void MKB(GXSetColorUpdate_cached)(MKB(GXBool) update_enable);
    void MKB(GXSetZCompLoc_cached)(MKB(GXBool) before_tex);
    void MKB(GXSetTevColorIn_cached)(MKB(GXTevStageID)  stage, MKB(GXTevColorArg)  a, MKB(GXTevColorArg)  b, MKB(GXTevColorArg)  c, MKB(GXTevColorArg)  d);
    void MKB(GXSetTevAlphaIn_cached)(MKB(GXTevStageID)  stage, MKB(GXTevAlphaArg)  a, MKB(GXTevAlphaArg)  b, MKB(GXTevAlphaArg)  c, MKB(GXTevAlphaArg)  d);
    void MKB(GXSetTevColorOp_cached)(MKB(GXTevStageID)  stage, MKB(GXTevOp)  op, MKB(GXTevBias)  bias, MKB(GXTevScale)  scale, MKB(GXBool) clamp, MKB(GXTevRegID)  out_reg);
    void MKB(GXSetTevAlphaOp_cached)(MKB(GXTevStageID)  stage, MKB(GXTevOp)  op, MKB(GXTevBias)  bias, MKB(GXTevScale)  scale, MKB(GXBool) clamp, MKB(GXTevRegID)  out_reg);
    void MKB(g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper)(MKB(GXTevStageID)  param_1, int param_2);
    void MKB(g_GXSetTevOrder_wrapper)(MKB(GXTevStageID)  param_1, MKB(GXTexCoordID)  param_2, MKB(GXTexCoordID)  param_3, MKB(GXChannelID)  param_4);
    void MKB(GXSetTevKColorSel_cached)(MKB(GXTevStageID)  stage, MKB(GXTevKColorSel)  sel);
    void MKB(GXSetTevKAlphaSel_cached)(MKB(GXTevStageID)  stage, MKB(GXTevKAlphaSel)  sel);
    void MKB(GXSetNumTevStages_cached)(MKB(u8) nStages);
    void MKB(GXSetTevKColor_cached)(MKB(GXTevKColorID)  id, struct MKB(GXColor) * color);
    void MKB(GXLoadTexObj_cached)(struct MKB(GXTexObj) * tex_obj, MKB(GXTexMapID)  tex_map_id);
    void MKB(g_something_with_GXSetProjectionv)(undefined4 * param_1, uint param_2);
    void MKB(GXSetNumIndStages_cached)(MKB(u8) nstages);
    void MKB(GXSetIndTexOrder_cached)(MKB(GXIndTexStageID)  ind_stage, MKB(GXTexCoordID)  tex_coord, MKB(GXTexMapID)  tex_map);
    void MKB(g_GXSetIndTexMtx_wrapper)(MKB(GXIndTexMtxID)  param_1, float param_2[2][3], char param_3);
    void MKB(GXSetTevIndirect_cached)(MKB(GXTevStageID)  tev_stage, MKB(GXIndTexStageID)  ind_stage, MKB(GXIndTexFormat)  format, MKB(GXIndTexBiasSel)  bias_sel, MKB(GXIndTexMtxID)  matrix_sel, MKB(GXIndTexWrap)  wrap_s, MKB(GXIndTexWrap)  wrap_t, MKB(GXBool) add_prev, MKB(GXBool) utc_lod, MKB(GXIndTexAlphaSel)  alpha_sel);
    void MKB(g_GXSetTevIndirect_zero_if_different)(MKB(GXTevStageID)  param_1);
    void MKB(GXSetScissor_cached)(MKB(u32) xOrig, MKB(u32) yOrig, MKB(u32) wd, MKB(u32) ht);
    void MKB(GXSetDispCopySrc_cached)(MKB(u16) left, MKB(u16) top, MKB(u16) wd, MKB(u16) ht);
    void MKB(GXSetTexCopySrc_cached)(MKB(u16) left, MKB(u16) top, MKB(u16) wd, MKB(u16) ht);
    void MKB(GXSetDispCopyDst_cached)(MKB(u16) wd, MKB(u16) ht);
    void MKB(GXSetTexCopyDst_cached)(MKB(u16) wd, MKB(u16) ht, MKB(GXTexFmt)  fmt, MKB(GXBool) mipmap);
    void MKB(GXSetCopyClear_cached)(struct MKB(GXColor) clear_clr, MKB(u32) clear_z);
    void MKB(GXSetTexCoordGen2_cached)(MKB(GXTexCoordID)  dst_coord, MKB(GXTexGenType)  func, MKB(GXTexGenSrc)  src_param, MKB(u32) mtx, MKB(GXBool) normalize, MKB(u32) postmtx);
    void MKB(GXSetNumTexGens_cached)(MKB(u8) nTexGens);
    void MKB(GXSetLineWidth_cached)(MKB(u8) width, MKB(GXTexOffset)  tex_offsets);
    void MKB(GXSetNumChans_cached)(MKB(u8) nChans);
    void MKB(opti_GXSetChanCtrl)(MKB(GXChannelID)  chan, MKB(GXBool) enable, MKB(GXColorSrc)  amb_src, MKB(GXColorSrc)  mat_src, MKB(u32) light_mask, MKB(GXDiffuseFn)  diff_fn, MKB(GXAttnFn)  attn_fn);
    void MKB(GXSetZMode_cached)(MKB(GXBool) compare_enable, MKB(GXCompare)  func, MKB(GXBool) update_enable);
    void MKB(g_read_something_from_prev_GX_settings)(undefined1 * param_1, undefined4 * param_2, undefined1 * param_3);
    void MKB(g_set_some_gx_settings)(void);
    void MKB(g_draw_something)(double param_1, double param_2, double param_3, ushort * param_4);
    void MKB(mark_finished_waiting_for_dvdread)(MKB(s32) result, struct MKB(DVDFileInfo) * file_info);
    MKB(u32) MKB(read_entire_file_using_dvdread_prio_async)(struct MKB(DVDFileInfo) * fileInfo, void * addr, MKB(s32) length, MKB(s32) offset);
    void MKB(dvd_cancel_async_callback)(int param_1);
    undefined4 MKB(call_dvd_cancel_async_and_wait_for_completion)(void * block);
    void MKB(g_something_with_disc_cover)(void);
    void MKB(g_idle_function)(int param_1);
    void MKB(g_set_some_func_ptr1)(undefined4 param_1);
    void MKB(g_set_some_card_func_ptr)(undefined4 param_1);
    void MKB(g_set_some_func_ptr3)(undefined4 param_1);
    void MKB(pad_init)(void);
    void MKB(g_read_inputs_and_maybe_reset_console)(void);
    void MKB(g_PADRead_and_handle_errors)(struct MKB(PADStatus) * pad_status);
    void MKB(g_reset_console)(struct MKB(PADStatus) * status, int param_2);
    void MKB(g_reset_console_wrapper)(struct MKB(PADStatus) * status);
    void MKB(g_smth_which_may_reset_console)(struct MKB(PADStatus) * status);
    void MKB(set_console_reset_callback)(MKB(u32) (* callback)(void));
    MKB(u32) (* MKB(get_console_reset_callback)(void))(void);
    void * MKB(OSAlloc)(MKB(u32) size);
    void MKB(OSFree)(void * ptr);
    void MKB(g_some_arq_request_callback)(void);
    void MKB(aram_font_to_mram_font_callback)(void);
    void MKB(g_load_aram_font)(void);
    void MKB(gARAMFont_DVDRead)(struct MKB(DVDFileInfo) * param_1, int param_2, int param_3);
    void MKB(g_some_arq_request)(void * source);
    void MKB(aram_font_to_mram_font)(MKB(Font8)  font, int g_char_idx, void * dest);
    void MKB(memcpy_handler)(void * dest, void * src, MKB(size_t) count);
    void MKB(g_something_with_video_progressive_mode)(void);
    void MKB(bootup_draw_sega_splash)(void);
    void MKB(init_sega_splash_tex)(struct MKB(GXTexObj) * tex_obj);
    void MKB(draw_sega_splash)(struct MKB(GXTexObj) * param_1);
    void MKB(draw_sega_splash_wrapper)(void);
    void MKB(PPCMfmsr)(void);
    void MKB(PPCMtmsr)(void);
    void MKB(PPCMfhid0)(void);
    void MKB(PPCMthid0)(void);
    void MKB(PPCMfl2cr)(void);
    void MKB(PPCMtl2cr)(void);
    void MKB(PPCMtdec)(void);
    void MKB(PPCSync)(void);
    void MKB(PPCHalt)(void);
    void MKB(PPCMtmmcr0)(void);
    void MKB(PPCMtmmcr1)(void);
    void MKB(PPCMfpmc1)(void);
    void MKB(PPCMtpmc1)(void);
    void MKB(PPCMtpmc2)(void);
    void MKB(PPCMfpmc3)(void);
    void MKB(PPCMtpmc3)(void);
    void MKB(PPCMfpmc4)(void);
    void MKB(PPCMtpmc4)(void);
    void MKB(PPCMfhid2)(void);
    void MKB(PPCMthid2)(void);
    void MKB(PPCMfwpar)(void);
    void MKB(PPCMtwpar)(void);
    void MKB(PPCDisableSpeculation)(void);
    void MKB(PPCSetFpNonIEEEMode)(void);
    int MKB(OSGetConsoleType)(void);
    void MKB(ClearArena)(void);
    void MKB(InquiryCallback)(undefined4 param_1, int param_2);
    void MKB(OSInit)(void);
    void MKB(OSExceptionInit)(void);
    undefined4 MKB(__OSDBINITSTART)(void);
    undefined4 MKB(__OSSetExceptionHandler)(uint param_1, undefined4 param_2);
    undefined4 MKB(__OSGetExceptionHandler)(uint param_1);
    undefined4 MKB(__OSPSInit)(void);
    uint MKB(__OSGetDIConfig)(void);
    void MKB(OSInitAlarm)(void);
    void MKB(OSCreateAlarm)(undefined4 * param_1);
    void MKB(InsertAlarm)(undefined4 * param_1, undefined4 param_2, uint param_3, uint param_4, undefined4 param_5);
    void MKB(OSSetAlarm)(undefined4 * param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5);
    void MKB(OSSetPeriodicAlarm)(undefined4 * param_1, undefined4 param_2, int param_3, uint param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7);
    void MKB(OSCancelAlarm)(int * param_1);
    undefined8 MKB(DecrementerExceptionCallback)(undefined4 param_1, struct MKB(OSContext) * param_2);
    MKB(ChunkInfo) * MKB(DLInsert)(struct MKB(ChunkInfo) * free_chunk_list, struct MKB(ChunkInfo) * new_free_chunk);
    void * MKB(OSAllocFromHeap)(MKB(OSHeapHandle) heap, MKB(u32) size);
    void MKB(OSFreeToHeap)(MKB(OSHeapHandle) heap, void * ptr);
    MKB(OSHeapHandle) MKB(OSSetCurrentHeap)(MKB(OSHeapHandle) heap);
    void * MKB(OSInitAlloc)(void * arena_start, void * arena_end, int max_heaps);
    MKB(OSHeapHandle) MKB(OSCreateHeap)(void * start, void * end);
    void MKB(OSDestroyHeap)(MKB(OSHeapHandle) heap);
    long MKB(OSCheckHeap)(MKB(OSHeapHandle) heap);
    void * MKB(OSGetArenaHi)(void);
    void * MKB(OSGetArenaLo)(void);
    void MKB(OSSetArenaHi)(void * newHi);
    void MKB(OSSetArenaLo)(void * newLo);
    void * MKB(OSAllocFromArenaLo)(MKB(u32) size, MKB(u32) align);
    void * MKB(OSAllocFromArenaHi)(MKB(u32) size, MKB(u32) align);
    void MKB(__OSInitAudioSystem)(void);
    void MKB(__OSStopAudioSystem)(void);
    uint MKB(DCEnable)(void);
    void MKB(DCInvalidateRange)(void * startAddr, MKB(u32) nBytes);
    void MKB(DCFlushRange)(void * startAddr, MKB(u32) nBytes);
    uint MKB(DCStoreRange)(uint param_1, int param_2);
    uint MKB(DCFlushRangeNoSync)(uint param_1, int param_2);
    uint MKB(DCStoreRangeNoSync)(uint param_1, int param_2);
    void MKB(ICInvalidateRange)(void * startAddr, MKB(u32) nBytes);
    uint MKB(ICFlashInvalidate)(void);
    uint MKB(ICEnable)(void);
    ulonglong MKB(__LCEnable)(void);
    void MKB(LCEnable)(void);
    ulonglong MKB(LCDisable)(void);
    void MKB(LCLoadBlocks)(void);
    uint MKB(LCStoreBlocks)(uint param_1);
    void MKB(LCQueueWait)(int param_1);
    void MKB(L2GlobalInvalidate)(void);
    void MKB(DMAErrorHandler)(undefined4 param_1, undefined4 * param_2);
    void MKB(__OSCacheInit)(void);
    undefined8 MKB(__OSLoadFPUContext)(undefined8 param_1, undefined4 param_2, int param_3);
    void MKB(__OSSaveFPUContext)(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, undefined4 param_9, undefined4 param_10, int param_11);
    void MKB(OSSetCurrentContext)(struct MKB(OSContext) * context);
    MKB(OSContext) * MKB(OSGetCurrentContext)(void);
    undefined4 MKB(OSSaveContext)(int param_1);
    undefined8 MKB(OSLoadContext)(int param_1);
    MKB(u32) MKB(OSGetStackPointer)(void);
    void MKB(OSClearContext)(struct MKB(OSContext) * context);
    void MKB(OSInitContext)(struct MKB(OSContext) * param_1, MKB(u32) param_2, MKB(u32) param_3);
    void MKB(OSDumpContext)(undefined4 * param_1);
    void MKB(__OSContextInit)(void);
    void MKB(OSReport)(char * msg, ...);
    void MKB(OSPanic)(char * file, int line, char * msg, ...);
    undefined4 MKB(OSSetErrorHandler)(uint param_1, undefined4 param_2);
    void MKB(Decode)(int param_1, int param_2);
    MKB(u16) MKB(OSGetFontEncode)(void);
    void MKB(ReadROM)(void * param_1, MKB(u32) param_2, int param_3);
    void MKB(ExpandFontSheet)(int param_1, uint param_2);
    bool MKB(OSInitFont)(int param_1);
    MKB(BOOL32) MKB(OSDisableInterrupts)(void);
    MKB(BOOL32) MKB(OSEnableInterrupts)(void);
    MKB(BOOL32) MKB(OSRestoreInterrupts)(MKB(BOOL32) enable);
    undefined4 MKB(__OSSetInterruptHandler)(short param_1, undefined4 param_2);
    void (* MKB(__OSGetInterruptHandler)(MKB(__OSInterrupt) interrupt))(MKB(__OSInterrupt), struct MKB(OSContext) *);
    void MKB(__OSInterruptInit)(void);
    MKB(OSInterruptMask) MKB(SetInterruptMask)(MKB(OSInterruptMask) mask);
    uint MKB(__OSMaskInterrupts)(uint param_1);
    MKB(OSInterruptMask) MKB(__OSUnmaskInterrupts)(MKB(OSInterruptMask) mask);
    undefined8 MKB(__OSDispatchInterrupt)(undefined4 param_1, int param_2);
    undefined8 MKB(ExternalInterruptHandler)(undefined4 param_1, undefined4 * param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(OSSetStringTable)(void * stringTable);
    undefined4 MKB(Relocate)(struct MKB(OSModuleHeader) * module1, struct MKB(OSModuleHeader) * module2);
    bool MKB(OSLink)(struct MKB(OSModuleHeader) * newModule, void * bss);
    undefined4 MKB(Undo)(int * param_1, int param_2);
    bool MKB(OSUnlink)(struct MKB(OSModuleInfo) * oldModule);
    void MKB(__OSModuleInit)(void);
    MKB(u32) MKB(OSGetConsoleSimulatedMemSize)(void);
    undefined4 MKB(OnReset)(int param_1);
    void MKB(MEMIntrruptHandler)(undefined4 param_1, undefined4 * param_2);
    uint MKB(RealMode)(void);
    void MKB(__OSInitMemoryProtection)(void);
    void MKB(__OSUnlockAllMutex)(int param_1);
    void MKB(Run)(void);
    void MKB(__OSReboot)(void);
    void MKB(OSRegisterResetFunction)(int param_1);
    void MKB(Reset)(undefined4 param_1);
    void MKB(__OSDoHotReset)(int param_1);
    bool MKB(OSResetSystem)(int param_1, int param_2, int param_3);
    uint MKB(OSGetResetCode)(void);
    void MKB(__OSResetSWInterruptHandler)(void);
    uint MKB(OSGetResetButtonState)(void);
    void MKB(OSGetResetSwitchState)(void);
    void MKB(WriteSramCallback)(void);
    uint MKB(WriteSram)(byte * param_1, int param_2, int param_3);
    void MKB(__OSInitSram)(void);
    undefined2 * MKB(__OSLockSram)(void);
    undefined2 * MKB(__OSLockSramEx)(void);
    uint MKB(UnlockSram)(int param_1, uint param_2);
    void MKB(__OSUnlockSram)(int param_1);
    void MKB(__OSUnlockSramEx)(int param_1);
    undefined4 MKB(__OSSyncSram)(void);
    uint MKB(__OSReadROM)(void * param_1, MKB(u32) param_2, int param_3);
    MKB(u32) MKB(OSGetSoundMode)(void);
    void MKB(OSSetSoundMode)(byte param_1);
    MKB(u32) MKB(OSGetProgressiveMode)(void);
    void MKB(OSSetProgressiveMode)(char param_1);
    undefined2 MKB(OSGetWirelessID)(int param_1);
    void MKB(OSSetWirelessID)(int param_1, short param_2);
    void MKB(__OSInitSystemCall)(void);
    void MKB(__OSThreadInit)(void);
    void MKB(OSInitThreadQueue)(struct MKB(OSThreadQueue) * queue);
    undefined4 MKB(OSGetCurrentThread)(void);
    int MKB(OSDisableScheduler)(void);
    int MKB(OSEnableScheduler)(void);
    void MKB(UnsetRun)(int param_1);
    int MKB(__OSGetEffectivePriority)(int param_1);
    undefined4 MKB(SetEffectivePriority)(int param_1, undefined4 param_2);
    MKB(OSContext) * MKB(SelectThread)(int param_1);
    void MKB(__OSReschedule)(void);
    undefined4 MKB(OSCreateThread)(struct MKB(OSThread) * thread, void * func, void * param, void * stackBase, MKB(u32) stackSize, MKB(OSPriority) priority, MKB(u16) attribute);
    void MKB(OSExitThread)(MKB(u32) param_1);
    void MKB(OSCancelThread)(struct MKB(OSContext) * param_1);
    int MKB(OSResumeThread)(int param_1);
    int MKB(OSSuspendThread)(int param_1);
    void MKB(OSSleepThread)(int * param_1);
    void MKB(OSWakeupThread)(int * param_1);
    undefined4 MKB(OSSetThreadPriority)(int param_1, int param_2);
    undefined4 MKB(OSGetThreadPriority)(int param_1);
    MKB(OSTime) MKB(OSGetTime)(void);
    MKB(OSTick) MKB(OSGetTick)(void);
    ulonglong MKB(__OSGetSystemTime)(void);
    undefined8 MKB(__OSTimeToSystemTime)(int param_1, uint param_2);
    void MKB(GetDates)(int param_1, int param_2);
    void MKB(OSTicksToCalendarTime)(MKB(OSTime) ticks, struct MKB(OSCalendarTime) * td);
    void MKB(init_cpp_wrapper)(void);
    void MKB(__init_cpp)(void);
    void MKB(ppc_halt_wrapper)(void);
    undefined4 MKB(SIBusy)(void);
    undefined4 MKB(SIIsChanBusy)(int param_1);
    uint MKB(CompleteTransfer)(void);
    void MKB(SIInterruptHandler)(undefined4 param_1, undefined4 param_2);
    bool MKB(SIEnablePollingInterrupt)(int param_1);
    undefined4 MKB(SIRegisterPollingHandler)(int param_1);
    undefined4 MKB(SIUnregisterPollingHandler)(int param_1);
    void MKB(SIInit)(void);
    undefined4 MKB(__SITransfer)(uint param_1, undefined4 * param_2, int param_3, undefined4 param_4, int param_5, int param_6);
    uint MKB(SIGetStatus)(int param_1);
    void MKB(SISetCommand)(int param_1, undefined4 param_2);
    void MKB(SITransferCommands)(void);
    uint MKB(SISetXY)(int param_1, int param_2);
    uint MKB(SIEnablePolling)(uint param_1);
    uint MKB(SIDisablePolling)(uint param_1);
    bool MKB(SIGetResponseRaw)(int param_1);
    int MKB(SIGetResponse)(int param_1, undefined4 * param_2);
    undefined4 MKB(SITransfer)(uint param_1, undefined4 * param_2, int param_3, undefined4 param_4, int param_5, int param_6, int param_7, uint param_8);
    void MKB(GetTypeCallback)(uint param_1, uint param_2);
    int MKB(SIGetType)(uint param_1);
    int MKB(SIGetTypeAsync)(uint param_1, MKB(undefined) * param_2);
    void MKB(SISetSamplingRate)(uint param_1);
    void MKB(SIRefreshSamplingRate)(void);
    void MKB(SetExiInterruptMask)(int param_1, int * param_2);
    undefined4 MKB(EXIImm)(int param_1, byte * param_2, int param_3, int param_4, undefined4 param_5);
    undefined4 MKB(EXIImmEx)(int param_1, byte * param_2, int param_3, int param_4);
    undefined4 MKB(EXIDma)(int param_1, uint param_2, undefined4 param_3, int param_4, undefined4 param_5);
    undefined4 MKB(EXISync)(int param_1);
    uint MKB(EXIClearInterrupts)(int param_1, int param_2, int param_3, int param_4);
    int MKB(EXISetExiCallback)(int param_1, int param_2);
    undefined4 MKB(__EXIProbe)(int param_1);
    int MKB(EXIProbe)(int param_1);
    undefined4 MKB(EXIProbeEx)(int param_1);
    undefined4 MKB(EXIAttach)(int param_1, undefined4 param_2);
    undefined4 MKB(EXIDetach)(int param_1);
    undefined4 MKB(EXISelect)(int param_1, int param_2, int param_3);
    undefined4 MKB(EXIDeselect)(int param_1);
    void MKB(TCIntrruptHandler)(short param_1, struct MKB(OSContext) * param_2);
    void MKB(EXIInit)(void);
    undefined4 MKB(EXILock)(int param_1, int param_2, int param_3);
    undefined4 MKB(EXIUnlock)(int param_1);
    undefined4 MKB(sndReadFlag)(int param_1);
    undefined4 MKB(EXIGetID)(int param_1, int param_2, byte * param_3);
    undefined4 MKB(InitializeUART)(void);
    undefined4 MKB(WriteUARTN)(byte * param_1, uint param_2);
    void MKB(DBInit)(void);
    void MKB(__DBExceptionDestinationAux)(void);
    void MKB(__DBExceptionDestination)(void);
    uint MKB(__DBIsExceptionMarked)(uint param_1);
    void MKB(DBPrintf)(void);
    undefined8 MKB(PSMTXIdentity)(short * param_1);
    undefined8 MKB(PSMTXCopy)(float * param_1, float * param_2);
    undefined8 MKB(PSMTXConcat)(float * param_1, float * param_2, float * param_3);
    undefined4 MKB(PSMTXInverse)(float * param_1, float * param_2);
    void MKB(PSMTXScale)(double param_1, double param_2, double param_3, float * param_4);
    void MKB(C_MTXLookAt)(MKB(Mtx) * mtx, struct MKB(Vec) * cam_pos, struct MKB(Vec) * cam_up, struct MKB(Vec) * target);
    void MKB(C_MTXFrustum)(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, float * param_7);
    void MKB(C_MTXPerspective)(MKB(Mtx44) * m, double fovy, double aspect, double n, double f);
    void MKB(C_MTXOrtho)(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, float * m);
    void MKB(PSVECAdd)(float * param_1, float * param_2, float * param_3);
    void MKB(PSVECSubtract)(float * param_1, float * param_2, float * param_3);
    void MKB(PSVECScale)(double param_1, float * param_2, float * param_3);
    undefined8 MKB(PSVECNormalize)(float * param_1, float * param_2);
    undefined8 MKB(PSVECCrossProduct)(float * param_1, float * param_2, float * param_3);
    void MKB(__DVDInitWA)(void);
    void MKB(AlarmHandlerForTimeout)(undefined4 param_1, struct MKB(OSContext) * param_2);
    void MKB(Read)(undefined4 param_1, uint param_2, uint param_3, undefined4 param_4);
    void MKB(SeekTwiceBeforeRead)(undefined4 param_1, undefined4 param_2, uint param_3, undefined4 param_4);
    undefined4 MKB(DVDLowRead)(undefined4 param_1, uint param_2, uint param_3, undefined4 param_4);
    undefined4 MKB(DVDLowSeek)(uint param_1, undefined4 param_2);
    undefined4 MKB(DVDLowWaitCoverClose)(undefined4 param_1);
    undefined4 MKB(DVDLowReadDiskID)(undefined4 param_1, undefined4 param_2);
    undefined4 MKB(DVDLowStopMotor)(undefined4 param_1);
    undefined4 MKB(DVDLowRequestError)(undefined4 param_1);
    undefined4 MKB(DVDLowInquiry)(undefined4 param_1, undefined4 param_2);
    undefined4 MKB(DVDLowAudioStream)(uint param_1, undefined4 param_2, uint param_3, undefined4 param_4);
    undefined4 MKB(DVDLowRequestAudioStatus)(uint param_1, undefined4 param_2);
    undefined4 MKB(DVDLowAudioBufferConfig)(int param_1, uint param_2, undefined4 param_3);
    void MKB(DVDLowReset)(void);
    undefined4 MKB(DVDLowBreak)(void);
    undefined4 MKB(DVDLowClearCallback)(void);
    void MKB(__DVDLowSetWAType)(undefined4 param_1, undefined4 param_2);
    void MKB(__DVDFSInit)(void);
    MKB(s32) MKB(DVDConvertPathToEntrynum)(char * fileName);
    MKB(BOOL32) MKB(DVDFastOpen)(MKB(s32) entrynum, struct MKB(DVDFileInfo) * fileInfo);
    MKB(BOOL32) MKB(DVDOpen)(char * fileName, struct MKB(DVDFileInfo) * fileInfo);
    MKB(BOOL32) MKB(DVDClose)(struct MKB(DVDFileInfo) * fileInfo);
    int MKB(entryToPath)(int param_1, int param_2, int param_3);
    bool MKB(DVDConvertEntrynumToPath)(MKB(s32) entry_num, char * out_path, MKB(size_t) max_path_size);
    undefined4 MKB(DVDGetCurrentDir)(int param_1, int param_2);
    void MKB(DVDChangeDir)(char * dirName);
    MKB(BOOL32) MKB(DVDReadAsyncPrio)(struct MKB(DVDFileInfo) * fileInfo, void * addr, MKB(s32) length, MKB(s32) offset, void (* callback)(MKB(s32), struct MKB(DVDFileInfo) *), MKB(s32) prio);
    void MKB(cbForReadAsync)(undefined4 param_1, int param_2);
    MKB(s32) MKB(DVDReadPrio)(struct MKB(DVDFileInfo) * fileInfo, void * addr, MKB(s32) length, MKB(s32) offset, MKB(s32) prio);
    void MKB(cbForCancelSync)(void);
    MKB(BOOL32) MKB(DVDOpenDir)(char * dirName, struct MKB(DVDDir) * dir);
    MKB(BOOL32) MKB(DVDReadDir)(struct MKB(DVDDir) * dir, struct MKB(DVDDirEntry) * dirent);
    void MKB(DVDPrepareStreamAsync)(undefined4 * param_1, uint param_2, uint param_3, undefined4 param_4);
    void MKB(cbForReadAsync)(undefined4 param_1, int param_2);
    void MKB(DVDInit)(void);
    void MKB(cbForStateError)(int param_1);
    void MKB(stateTimeout)(void);
    void MKB(stateGettingError)(void);
    undefined4 MKB(CategorizeError)(uint param_1);
    void MKB(cbForStateGettingError)(uint param_1);
    void MKB(cbForUnrecoveredError)(uint param_1);
    void MKB(cbForUnrecoveredErrorRetry)(uint param_1);
    void MKB(stateGettingError)(void);
    void MKB(cbForStateGoToRetry)(uint param_1);
    void MKB(stateCheckID)(void);
    void MKB(stateCheckID3)(void);
    void MKB(stateCheckID2)(void);
    void MKB(cbForStateCheckID1)(uint param_1);
    void MKB(cbForStateCheckID3)(uint param_1);
    void MKB(stateCoverClosed)(void);
    void MKB(cbForStateCoverClosed)(uint param_1);
    void MKB(stateGettingError)(void);
    void MKB(cbForStateMotorStopped)(void);
    void MKB(stateReady)(void);
    void MKB(stateBusy)(int param_1);
    undefined4 MKB(DVDReadAbsAsyncPrio)(undefined4 * param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, int param_6);
    undefined4 MKB(DVDReadAbsAsyncForBS)(undefined4 * param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5);
    undefined4 MKB(DVDReadDiskID)(undefined4 * param_1, undefined4 param_2, undefined4 param_3);
    undefined4 MKB(DVDPrepareStreamAbsAsync)(undefined4 * param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
    undefined4 MKB(DVDCancelStreamAsync)(undefined4 * param_1, undefined4 param_2);
    undefined4 MKB(DVDStopStreamAtEndAsync)(undefined4 * param_1, undefined4 param_2);
    undefined4 MKB(DVDGetStreamErrorStatusAsync)(undefined4 * param_1, undefined4 param_2);
    undefined4 MKB(DVDGetStreamPlayAddrAsync)(undefined4 * param_1, undefined4 param_2);
    undefined4 MKB(DVDInquiryAsync)(undefined4 * param_1, undefined4 param_2, undefined4 param_3);
    void MKB(DVDReset)(void);
    MKB(s32) MKB(DVDGetCommandBlockStatus)(struct MKB(DVDCommandBlock) * commandBlock);
    MKB(s32) MKB(DVDGetDriveStatus)(void);
    undefined4 MKB(DVDCancelAsync)(int * param_1, MKB(undefined) * param_2);
    MKB(s32) MKB(DVDCancel)(struct MKB(DVDCommandBlock) * block);
    void MKB(cbForCancelSync)(void);
    MKB(DVDDiskID) * MKB(DVDGetCurrentDiskID)(void);
    undefined4 MKB(DVDCheckDisk)(void);
    void MKB(__DVDPrepareResetAsync)(MKB(undefined) * param_1);
    void MKB(__DVDClearWaitingQueue)(void);
    undefined4 MKB(__DVDPushWaitingQueue)(int param_1, undefined4 * param_2);
    int * MKB(__DVDPopWaitingQueue)(void);
    undefined4 MKB(__DVDCheckWaitingQueue)(void);
    undefined4 MKB(__DVDDequeueWaitingQueue)(int * param_1);
    char MKB(ErrorCode2Num)(uint param_1);
    void MKB(__DVDStoreErrorCode)(uint param_1);
    void MKB(cb)(int param_1, undefined4 * param_2);
    void MKB(__fstLoad)(void);
    void MKB(__VIRetraceHandler)(undefined4 param_1, struct MKB(OSContext) * param_2);
    MKB(undefined) * MKB(getTiming)(undefined4 param_1);
    void MKB(__VIInit)(uint param_1);
    void MKB(VIInit)(void);
    void MKB(VIWaitForRetrace)(void);
    void MKB(setFbbRegs)(int param_1, uint * param_2, uint * param_3, uint * param_4, uint * param_5);
    void MKB(setVerticalRegs)(ushort param_1, ushort param_2, byte param_3, short param_4, short param_5, short param_6, short param_7, short param_8, int param_9);
    void MKB(VIConfigure)(struct MKB(GXRenderModeObj) * rm);
    void MKB(VIFlush)(void);
    void MKB(VISetNextFrameBuffer)(void * fb);
    void MKB(VISetBlack)(bool black);
    MKB(u32) MKB(VIGetRetraceCount)(void);
    undefined4 MKB(getCurrentFieldEvenOdd)(void);
    uint MKB(VIGetCurrentLine)(void);
    MKB(u32) MKB(VIGetTvFormat)(void);
    MKB(u32) MKB(VIGetDTVStatus)(void);
    void MKB(UpdateOrigin)(uint param_1);
    void MKB(PADOriginCallback)(undefined4 param_1, uint param_2);
    void MKB(PADProbeCallback)(undefined4 param_1, uint param_2);
    void MKB(PADTypeAndStatusCallback)(undefined4 param_1, uint param_2);
    undefined4 MKB(PADReset)(uint param_1);
    undefined4 MKB(PADRecalibrate)(uint param_1);
    undefined4 MKB(PADInit)(void);
    MKB(u32) MKB(PADRead)(struct MKB(PADStatus) * status);
    void MKB(PADControlAllMotors)(MKB(u32) * commandArray);
    void MKB(PADControlMotor)(uint param_1, uint param_2);
    void MKB(PADSetSpec)(int param_1);
    void MKB(SPEC0_MakeStatus)(undefined4 param_1, ushort * param_2, uint * param_3);
    void MKB(SPEC1_MakeStatus)(undefined4 param_1, ushort * param_2, uint * param_3);
    void MKB(SPEC2_MakeStatus)(int param_1, ushort * param_2, undefined4 * param_3);
    void MKB(PADSetAnalogMode)(int param_1);
    void MKB(SamplingHandler)(undefined4 param_1, struct MKB(OSContext) * param_2);
    undefined4 MKB(PADSetSamplingCallback)(int param_1);
    bool MKB(__PADDisableRecalibration)(int param_1);
    undefined4 MKB(AIRegisterDMACallback)(undefined4 param_1);
    void MKB(AIInitDMA)(undefined4 param_1, uint param_2);
    void MKB(AIStartDMA)(void);
    void MKB(AIStopDMA)(void);
    undefined4 MKB(AIGetStreamSampleCount)(void);
    void MKB(AIResetStreamSampleCount)(void);
    void MKB(AISetStreamTrigger)(undefined4 param_1);
    void MKB(AISetStreamPlayState)(uint param_1);
    uint MKB(AIGetStreamPlayState)(void);
    void MKB(AISetDSPSampleRate)(uint param_1);
    uint MKB(AIGetDSPSampleRate)(void);
    void MKB(__AI_set_stream_sample_rate)(uint param_1);
    uint MKB(AIGetStreamSampleRate)(void);
    void MKB(AISetStreamVolLeft)(uint param_1);
    uint MKB(AIGetStreamVolLeft)(void);
    void MKB(AISetStreamVolRight)(uint param_1);
    uint MKB(AIGetStreamVolRight)(void);
    void MKB(AIInit)(undefined4 param_1);
    void MKB(__AISHandler)(undefined4 param_1, struct MKB(OSContext) * param_2);
    void MKB(__AICallbackStackSwitch)(MKB(undefined) * param_1);
    void MKB(__AI_SRC_INIT)(void);
    ushort MKB(ARGetDMAStatus)(void);
    void MKB(ARStartDMA)(short param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
    undefined4 MKB(ARInit)(undefined4 param_1, undefined4 param_2);
    undefined4 MKB(ARGetBaseAddress)(void);
    void MKB(__ARHandler)(undefined4 param_1, struct MKB(OSContext) * param_2);
    void MKB(__ARChecksize)(void);
    void MKB(__ARQServiceQueueLo)(void);
    void MKB(empty_function)(void);
    void MKB(__ARQInterruptServiceRoutine)(void);
    void MKB(ARQInit)(void);
    void MKB(ARQPostRequest)(struct MKB(ARQRequest) * request, MKB(u32) owner, MKB(u32) type, MKB(u32) priority, MKB(u32) source, MKB(u32) dest, MKB(u32) length, void (* callback)(MKB(u32)));
    void MKB(ARQSetChunkSize)(uint param_1);
    undefined4 MKB(g_get_arq_chunk_size)(void);
    void MKB(hwExit)(void);
    void MKB(hwDisableIrq)(void);
    void MKB(salExitAi)(void);
    void MKB(hwEnableIrq)(void);
    void MKB(__AXOutQuit)(void);
    void MKB(salExitDspCtrl)(void);
    void MKB(salExitDsp)(void);
    ushort MKB(DSPCheckMailToDSP)(void);
    ushort MKB(DSPCheckMailFromDSP)(void);
    undefined4 MKB(DSPReadMailFromDSP)(void);
    void MKB(DSPSendMailToDSP)(undefined4 param_1);
    void MKB(DSPInit)(void);
    void MKB(DSPReset)(void);
    void MKB(DSPHalt)(void);
    ushort MKB(DSPGetDMAStatus)(void);
    undefined4 * MKB(DSPAddTask)(undefined4 * param_1);
    void MKB(__DSPHandler)(undefined4 param_1, struct MKB(OSContext) * param_2);
    void MKB(__DSP_exec_task)(int param_1, int * param_2);
    void MKB(__DSP_boot_task)(int param_1);
    void MKB(__DSP_insert_task)(int param_1);
    void MKB(__DSP_remove_task)(undefined4 * param_1);
    uint MKB(__DTKCallbackForFlush)(void);
    void MKB(DTKInit)(void);
    undefined4 MKB(DTKQueueTrack)(char * param_1, undefined4 * param_2, undefined4 param_3, undefined4 param_4);
    void MKB(DTKFlushTracks)(undefined4 param_1);
    void MKB(g_DTK_set_something)(undefined4 param_1);
    void MKB(DTKSetState)(int param_1);
    void MKB(DTKNextTrack)(void);
    void MKB(DTKPrevTrack)(void);
    undefined4 MKB(g_DTK_get_something1)(void);
    undefined4 MKB(g_GTK_get_something2)(void);
    void MKB(DTKSetVolume)(uint param_1, uint param_2);
    void MKB(empty_function)(void);
    void MKB(__CARDExtHandler)(int param_1);
    void MKB(__CARDExiHandler)(int param_1);
    void MKB(__CARDTxHandler)(int param_1);
    void MKB(__CARDUnlockedHandler)(int param_1);
    undefined4 MKB(__CARDEnableInterrupt)(int param_1, int param_2);
    undefined4 MKB(__CARDReadStatus)(int param_1, byte * param_2);
    undefined4 MKB(__CARDClearStatus)(int param_1);
    void MKB(TimeoutHandler)(MKB(undefined) * param_1);
    undefined4 MKB(Retry)(int param_1);
    void MKB(UnlockedCallback)(int param_1, int param_2);
    undefined4 MKB(__CARDStart)(int param_1, int param_2, int param_3);
    int MKB(__CARDReadSegment)(int param_1, int param_2);
    int MKB(__CARDWritePage)(int param_1, int param_2);
    int MKB(__CARDEraseSector)(int param_1, uint param_2, int param_3);
    void MKB(CARDInit)(void);
    void MKB(__CARDSetDiskID)(MKB(undefined) * param_1);
    undefined4 MKB(__CARDGetControlBlock)(int param_1, undefined4 * param_2);
    int MKB(__CARDPutControlBlock)(int * param_1, int param_2);
    MKB(s32) MKB(CARDGetResultCode)(int param_1);
    int MKB(CARDFreeBlocks)(int param_1, int * param_2, int * param_3);
    undefined4 MKB(OnReset)(int param_1);
    uint MKB(bitrev)(uint param_1);
    undefined4 MKB(ReadArrayUnlock)(int param_1, uint param_2, byte * param_3, int param_4, int param_5);
    uint MKB(DummyLen)(void);
    undefined4 MKB(__CARDUnlock)(int param_1, uint * param_2);
    void MKB(InitCallback)(int param_1);
    void MKB(DoneCallback)(int param_1);
    void MKB(BlockReadCallback)(int param_1, int param_2);
    int MKB(__CARDRead)(int param_1, undefined4 param_2, uint param_3, undefined4 param_4, undefined4 param_5);
    void MKB(BlockWriteCallback)(int param_1, int param_2);
    int MKB(__CARDWrite)(int param_1, undefined4 param_2, uint param_3, undefined4 param_4, undefined4 param_5);
    undefined4 MKB(CARDGetXferredBytes)(int param_1);
    undefined4 MKB(__CARDGetFatBlock)(int param_1);
    void MKB(WriteCallback)(int param_1, int param_2);
    void MKB(EraseCallback)(int param_1, int param_2);
    undefined4 MKB(__CARDAllocBlock)(int param_1, uint param_2, undefined4 param_3);
    undefined4 MKB(__CARDFreeBlock)(int param_1, ushort param_2, undefined4 param_3);
    void MKB(__CARDUpdateFatBlock)(int param_1, short * param_2, undefined4 param_3);
    undefined4 MKB(__CARDGetDirBlock)(int param_1);
    void MKB(WriteCallback)(int param_1, int param_2);
    void MKB(EraseCallback)(int param_1, int param_2);
    int MKB(__CARDUpdateDir)(int param_1, undefined4 param_2);
    void MKB(__CARDCheckSum)(ushort * param_1, uint param_2, short * param_3, short * param_4);
    undefined4 MKB(VerifyID)(int param_1);
    int MKB(VerifyDir)(int param_1, uint * param_2);
    int MKB(VerifyFAT)(int param_1, uint * param_2);
    int MKB(__CARDVerify)(int param_1);
    int MKB(CARDCheckExAsync)(int param_1, undefined4 * param_2, MKB(undefined) * param_3);
    void MKB(CARDCheckAsync)(int param_1, MKB(undefined) * param_2);
    undefined4 MKB(IsCard)(char * param_1);
    MKB(s32) MKB(CARDProbeEx)(MKB(s32) chan, MKB(s32) * memSize, MKB(s32) * sectorSize);
    int MKB(DoMount)(int param_1);
    void MKB(__CARDMountCallback)(int param_1, int param_2);
    MKB(s32) MKB(CARDMountAsync)(MKB(s32) chan, void * workArea, void * detachCallback, void * attachCallback);
    void MKB(DoUnmount)(int param_1, undefined4 param_2);
    MKB(s32) MKB(CARDUnmount)(MKB(s32) chan);
    void MKB(FormatCallback)(int param_1, int param_2);
    int MKB(__CARDFormatRegionAsync)(int param_1, ushort param_2, MKB(undefined) * param_3);
    void MKB(CARDFormatAsync)(int param_1, MKB(undefined) * param_2);
    undefined4 MKB(__CARDCompareFileName)(int param_1, char * param_2);
    undefined4 MKB(__CARDAccess)(int param_1, char * param_2);
    undefined4 MKB(__CARDIsPublic)(char * param_1);
    undefined4 MKB(__CARDGetFileNo)(int * param_1, char * param_2, int * param_3);
    MKB(s32) MKB(CARDOpen)(MKB(s32) chan, char * fileName, struct MKB(CARDFileInfo) * fileInfo);
    undefined4 MKB(return_0)(void);
    void MKB(CreateCallbackFat)(int param_1, int param_2);
    void MKB(CARDCreateAsync)(MKB(s32) chan, char * fileName, MKB(u32) size, struct MKB(CARDFileInfo) * fileInfo, void * callback);
    int MKB(__CARDSeek)(int * param_1, int param_2, uint param_3, undefined4 * param_4);
    void MKB(ReadCallback)(int param_1, int param_2);
    MKB(s32) MKB(CARDReadAsync)(struct MKB(CARDFileInfo) * fileInfo, void * buf, MKB(s32) length, MKB(s32) offset, void * callback);
    void MKB(WriteCallback)(int param_1, int param_2);
    void MKB(EraseCallback)(int param_1, int param_2);
    MKB(s32) MKB(CARDWriteAsync)(struct MKB(CARDFileInfo) * fileInfo, void * buf, MKB(s32) length, MKB(s32) offset, void * callback);
    void MKB(DeleteCallback)(int param_1, int param_2);
    void MKB(CARDFastDeleteAsync)(MKB(s32) chan, MKB(s32) fileNo, void * callback);
    int MKB(CARDDeleteAsync)(int param_1, char * param_2, MKB(undefined) * param_3);
    void MKB(UpdateIconOffsets)(int param_1, int param_2);
    MKB(s32) MKB(CARDGetStatus)(MKB(s32) chan, MKB(s32) fileNo, struct MKB(CARDStat) * stat);
    int MKB(CARDSetStatusAsync)(int param_1, int param_2, int param_3, undefined4 param_4);
    int MKB(CARDRenameAsync)(int param_1, char * param_2, char * param_3, undefined4 param_4);
    int MKB(CARDGetSerialNo)(int param_1, uint * param_2);
    MKB(GXFifoObj) * MKB(GXInit)(void * base, MKB(u32) size);
    void MKB(__GXInitGX)(void);
    void MKB(GXInitFifoBase)(struct MKB(GXFifoObj) * fifo, void * base, MKB(u32) size);
    void MKB(GXInitFifoPtrs)(struct MKB(GXFifoObj) * fifo, void * read_ptr, void * write_ptr);
    void MKB(GXInitFifoLimits)(struct MKB(GXFifoObj) * fifo, MKB(u32) hi_water_mark, MKB(u32) lo_water_mark);
    void MKB(GXSetCPUFifo)(struct MKB(GXFifoObj) * fifo);
    void MKB(GXSetGPFifo)(struct MKB(GXFifoObj) * fifo);
    void MKB(GXSaveCPUFifo)(struct MKB(GXFifoObj) * fifo);
    void MKB(__GXSaveCPUFifoAux)(int * param_1);
    void MKB(GXGetGPStatus)(MKB(GXBool) * overhi, MKB(GXBool) * underlow, MKB(GXBool) * readIdle, MKB(GXBool) * cmdIdle, MKB(GXBool) * brkpt);
    void MKB(GXGetFifoPtrs)(struct MKB(GXFifoObj) * fifo, void * * read_ptr, void * * write_ptr);
    void MKB(__GXFifoInit)(void);
    void MKB(__GXFifoReadEnable)(void);
    void MKB(__GXFifoReadDisable)(void);
    void MKB(__GXFifoLink)(char param_1);
    void MKB(__GXWriteFifoIntEnable)(uint param_1, uint param_2);
    void MKB(__GXWriteFifoIntReset)(uint param_1, uint param_2);
    MKB(GXFifoObj) * MKB(GXGetCPUFifo)(void);
    MKB(GXFifoObj) * MKB(GXGetGPFifo)(void);
    void MKB(__GXXfVtxSpecs)(void);
    void MKB(GXSetVtxDesc)(MKB(GXAttr)  attr, MKB(GXAttrType)  type);
    void MKB(GXSetVtxDescv)(struct MKB(GXVtxDescList) * param_1);
    void MKB(__GXSetVCD)(void);
    void MKB(g_GX_internal_unknown1)(void);
    void MKB(GXGetVtxDesc)(MKB(GXAttr)  attr, MKB(GXAttrType)  * type);
    void MKB(GXGetVtxDescv)(struct MKB(GXVtxDescList) * attr_list);
    void MKB(GXClearVtxDesc)(void);
    void MKB(GXSetVtxAttrFmt)(MKB(GXVtxFmt)  vtxfmt, MKB(GXAttr)  attr, MKB(GXCompCnt)  cnt, MKB(GXCompType)  type, MKB(u8) frac);
    void MKB(GXSetVtxAttrFmtv)(MKB(GXVtxFmt)  vtxfmt, struct MKB(GXVtxAttrFmtList) * list);
    void MKB(__GXSetVAT)(void);
    void MKB(GXGetVtxAttrFmt)(MKB(GXVtxFmt)  vtxfmt, MKB(GXAttr)  attr, MKB(GXCompCnt)  * cnt, MKB(GXCompType)  * type, MKB(u8) * frac);
    void MKB(GXGetVtxAttrFmtv)(MKB(GXVtxFmt)  vtxfmt, struct MKB(GXVtxAttrFmtList) * list);
    void MKB(GXSetArray)(MKB(GXAttr)  attr, void * base_ptr, MKB(u8) stride);
    void MKB(GXInvalidateVtxCache)(void);
    void MKB(GXSetTexCoordGen2)(MKB(GXTexCoordID)  dst_coord, MKB(GXTexGenType)  func, MKB(GXTexGenSrc)  src_param, MKB(u32) mtx, MKB(GXBool) normalize, MKB(u32) postmtx);
    void MKB(GXSetNumTexGens)(MKB(u8) nTexGens);
    void MKB(GXSetMisc)(MKB(GXMiscToken)  token, MKB(u32) value);
    void MKB(GXFlush)(void);
    void MKB(GXResetWriteGatherPipe)(void);
    void MKB(GXSetDrawSync)(MKB(u16) token);
    void MKB(GXSetDrawDone)(void);
    void MKB(GXDrawDone)(void);
    void MKB(GXPixModeSync)(void);
    void MKB(GXPokeAlphaMode)(MKB(GXCompare)  func, MKB(u8) threshold);
    void MKB(GXPokeAlphaRead)(MKB(GXAlphaReadMode)  mode);
    void MKB(GXPokeAlphaUpdate)(MKB(GXBool) update_enable);
    void MKB(GXPokeBlendMode)(MKB(GXBlendMode)  type, MKB(GXBlendFactor)  src_factor, MKB(GXBlendFactor)  dst_factor, MKB(GXLogicOp)  op);
    void MKB(GXPokeColorUpdate)(MKB(GXBool) update_enable);
    void MKB(GXPokeDstAlpha)(MKB(GXBool) enable, MKB(u8) alpha);
    void MKB(GXPokeDither)(MKB(GXBool) dither);
    void MKB(GXPokeZMode)(MKB(GXBool) compare_enable, MKB(GXCompare)  func, MKB(GXBool) update_enable);
    void MKB(GXPeekZ)(MKB(u16) x, MKB(u16) y, MKB(u32) * z);
    undefined4 MKB(GXSetDrawSyncCallback)(undefined4 param_1);
    void MKB(GXTokenInterruptHandler)(MKB(__OSInterrupt) interrupt, struct MKB(OSContext) * context);
    undefined4 MKB(GXSetDrawDoneCallback)(undefined4 param_1);
    void MKB(GXFinishInterruptHandler)(MKB(__OSInterrupt) interrupt, struct MKB(OSContext) * context);
    void MKB(__GXPEInit)(void);
    void MKB(__GXSetDirtyState)(undefined4 param_1, undefined4 param_2, uint param_3);
    void MKB(GXBegin)(MKB(GXPrimitive)  type, MKB(GXVtxFmt)  vtxfmt, MKB(u16) nverts);
    void MKB(__GXSendFlushPrim)(void);
    void MKB(GXSetLineWidth)(MKB(u8) width, MKB(GXTexOffset)  tex_offsets);
    void MKB(GXSetPointSize)(MKB(u8) size, MKB(GXTexOffset)  tex_offsets);
    void MKB(GXEnableTexOffsets)(MKB(GXTexCoordID)  coord, MKB(GXBool) line_enable, MKB(GXBool) point_enable);
    void MKB(GXSetCullMode)(MKB(GXCullMode)  mode);
    void MKB(GXSetCoPlanar)(MKB(GXBool) enable);
    void MKB(__GXSetGenMode)(void);
    void MKB(GXAdjustForOverscan)(struct MKB(GXRenderModeObj) * rmin, struct MKB(GXRenderModeObj) * rmout, MKB(u16) hor, MKB(u16) ver);
    void MKB(GXSetDispCopySrc)(MKB(u16) left, MKB(u16) top, MKB(u16) wd, MKB(u16) ht);
    void MKB(GXSetTexCopySrc)(MKB(u16) left, MKB(u16) top, MKB(u16) wd, MKB(u16) ht);
    void MKB(GXSetDispCopyDst)(MKB(u16) wd, MKB(u16) ht);
    void MKB(GXSetTexCopyDst)(MKB(u16) wd, MKB(u16) ht, MKB(GXTexFmt)  fmt, MKB(GXBool) mipmap);
    void MKB(GXSetDispCopyFrame2Field)(MKB(GXCopyMode)  mode);
    void MKB(GXSetCopyClamp)(MKB(GXFBClamp)  clamp);
    MKB(u32) MKB(GXSetDispCopyYScale)(MKB(undefined) yscale);
    void MKB(GXSetCopyClear)(struct MKB(GXColor) clear_clr, MKB(u32) clear_z);
    void MKB(GXSetCopyFilter)(MKB(GXBool) aa, MKB(undefined) sample_pattern, MKB(GXBool) vf, MKB(u8) vfilter[7]);
    void MKB(GXSetDispCopyGamma)(MKB(GXGamma)  gamma);
    void MKB(GXCopyDisp)(void * dest, MKB(GXBool) clear);
    void MKB(GXCopyTex)(void * dest, MKB(GXBool) clear);
    void MKB(GXClearBoundingBox)(void);
    void MKB(GXInitLightAttnK)(struct MKB(GXLightObj) * lt_obj, float k0, float k1, float k2);
    void MKB(GXInitLightSpot)(struct MKB(GXLightObj) * lt_obj, float cutoff, MKB(GXSpotFn)  spot_func);
    void MKB(GXInitLightDistAttn)(struct MKB(GXLightObj) * lt_obj, float ref_distance, float ref_brightness, MKB(GXDistAttnFn)  dist_func);
    void MKB(GXInitLightPos)(struct MKB(GXLightObj) * lt_obj, float x, float y, float z);
    void MKB(GXGetLightPos)(struct MKB(GXLightObj) * lt_obj, float * x, float * y, float * z);
    void MKB(GXInitLightDir)(struct MKB(GXLightObj) * lt_obj, float nx, float ny, float nz);
    void MKB(GXInitLightColor)(struct MKB(GXLightObj) * lt_obj, struct MKB(GXColor) color);
    void MKB(GXGetLightColor)(struct MKB(GXLightObj) * lt_obj, struct MKB(GXColor) * color);
    void MKB(GXLoadLightObjImm)(struct MKB(GXLightObj) * lt_obj, MKB(GXLightID)  light);
    void MKB(GXSetChanAmbColor)(MKB(GXChannelID)  chan, struct MKB(GXColor) amb_color);
    void MKB(GXSetChanMatColor)(MKB(GXChannelID)  chan, struct MKB(GXColor) mat_color);
    void MKB(GXSetNumChans)(MKB(u8) nChans);
    void MKB(GXSetChanCtrl)(MKB(GXChannelID)  chan, MKB(GXBool) enable, MKB(GXColorSrc)  amb_src, MKB(GXColorSrc)  mat_src, MKB(GXLightID)  light_mask, MKB(GXDiffuseFn)  diff_fn, MKB(GXAttnFn)  attn_fn);
    MKB(u32) MKB(GXGetTexBufferSize)(MKB(u16) width, MKB(u16) height, MKB(u32) format, MKB(GXBool) mipmap, MKB(u8) max_lod);
    void MKB(__GetImageTileCount)(int param_1, ushort param_2, ushort param_3, int * param_4, int * param_5, undefined4 * param_6);
    void MKB(GXInitTexObj)(struct MKB(GXTexObj) * obj, void * image_ptr, MKB(u16) width, MKB(u16) height, MKB(GXTexFmt)  format, MKB(GXTexWrapMode)  wrap_s, MKB(GXTexWrapMode)  wrap_t, MKB(GXBool) mipmap);
    void MKB(GXInitTexObjLOD)(struct MKB(GXTexObj) * obj, MKB(GXTexFilter)  min_filt, MKB(GXTexFilter)  mag_filt, float min_lod, float max_lod, float lod_bias, MKB(GXBool) bias_clamp, MKB(GXBool) do_edge_lod, MKB(GXAnisotropy)  max_aniso);
    void MKB(GXInitTexObjData)(struct MKB(GXTexObj) * obj, void * image_ptr);
    void MKB(GXInitTexObjWrapMode)(struct MKB(GXTexObj) * obj, MKB(GXTexWrapMode)  wrap_s, MKB(GXTexWrapMode)  wrap_t);
    void MKB(GXGetTexObjAll)(struct MKB(GXTexObj) * obj, void * * image_ptr, MKB(u16) * width, MKB(u16) * height, MKB(GXTexFmt)  * format, MKB(GXTexWrapMode)  * wrap_s, MKB(GXTexWrapMode)  * wrap_t, MKB(GXBool) * mipmap);
    void * MKB(GXGetTexObjData)(struct MKB(GXTexObj) * obj);
    MKB(u16) MKB(GXGetTexObjWidth)(struct MKB(GXTexObj) * obj);
    MKB(u16) MKB(GXGetTexObjHeight)(struct MKB(GXTexObj) * obj);
    MKB(GXTexFmt) MKB(GXGetTexObjFmt)(struct MKB(GXTexObj) * obj);
    MKB(GXTexWrapMode) MKB(GXGetTexObjWrapS)(struct MKB(GXTexObj) * obj);
    MKB(GXTexWrapMode) MKB(GXGetTexObjWrapT)(struct MKB(GXTexObj) * obj);
    MKB(GXBool) MKB(GXGetTexObjMipMap)(struct MKB(GXTexObj) * obj);
    void MKB(g_GX_something)(uint * param_1, uint * param_2, uint * param_3, float * param_4, float * param_5, float * param_6, byte * param_7, undefined1 * param_8, uint * param_9);
    void MKB(GXLoadTexObjPreLoaded)(struct MKB(GXTexObj) * obj, struct MKB(GXTexRegion) * region, MKB(GXTexMapID)  id);
    void MKB(GXLoadTexObj)(struct MKB(GXTexObj) * obj, MKB(GXTexMapID)  id);
    void MKB(GXInitTexCacheRegion)(struct MKB(GXTexRegion) * region, MKB(GXBool) is_32b_mipmap, MKB(u32) tmem_even, MKB(GXTexCacheSize)  size_even, MKB(u32) tmem_odd, MKB(GXTexCacheSize)  size_odd);
    void MKB(GXInitTlutRegion)(struct MKB(GXTlutRegion) * region, MKB(u32) tmem_addr, MKB(GXTlutSize)  tlut_size);
    void MKB(GXInvalidateTexAll)(void);
    MKB(GXTexRegion) * (* MKB(GXSetTexRegionCallback)(MKB(GXTexRegion) * (* f)(struct MKB(GXTexObj) *, MKB(GXTexMapID) )))(struct MKB(GXTexObj) *, MKB(GXTexMapID) );
    MKB(GXTlutRegion) (* MKB(GXSetTlutRegionCallback)(MKB(GXTlutRegion) (* f)(MKB(u32))))(MKB(u32));
    void MKB(__SetSURegs)(int param_1, int param_2);
    void MKB(__GXSetSUTexRegs)(void);
    void MKB(__GXSetTmemConfig)(int param_1);
    void MKB(GXSetTevIndirect)(MKB(GXTevStageID)  tev_stage, MKB(GXIndTexStageID)  ind_stage, MKB(GXIndTexFormat)  format, MKB(GXIndTexBiasSel)  bias_sel, MKB(GXIndTexMtxID)  matrix_sel, MKB(GXIndTexWrap)  wrap_s, MKB(GXIndTexWrap)  wrap_t, MKB(GXBool) add_prev, MKB(GXBool) utc_lod, MKB(GXIndTexAlphaSel)  alpha_sel);
    void MKB(GXSetIndTexMtx)(MKB(GXIndTexMtxID)  mtx_sel, float offset_mtx[2][3], MKB(s8) scale_exp);
    void MKB(GXSetIndTexCoordScale)(MKB(GXIndTexStageID)  ind_stage, MKB(GXIndTexScale)  scale_s, MKB(GXIndTexScale)  scale_t);
    void MKB(GXSetIndTexOrder)(MKB(GXIndTexStageID)  ind_stage, MKB(GXTexCoordID)  tex_coord, MKB(GXTexMapID)  tex_map);
    void MKB(GXSetNumIndStages)(MKB(u8) nstages);
    void MKB(GXSetTevDirect)(MKB(GXTevStageID)  tev_stage);
    void MKB(__GXUpdateBPMask)(undefined4 param_1, undefined4 param_2, uint param_3);
    void MKB(__GXFlushTextureState)(void);
    void MKB(GXSetTevOp)(MKB(GXTevStageID)  id, MKB(GXTevMode)  mode);
    void MKB(GXSetTevColorIn)(MKB(GXTevStageID)  stage, MKB(GXTevColorArg)  a, MKB(GXTevColorArg)  b, MKB(GXTevColorArg)  c, MKB(GXTevColorArg)  d);
    void MKB(GXSetTevAlphaIn)(MKB(GXTevStageID)  stage, MKB(GXTevAlphaArg)  a, MKB(GXTevAlphaArg)  b, MKB(GXTevAlphaArg)  c, MKB(GXTevAlphaArg)  d);
    void MKB(GXSetTevColorOp)(MKB(GXTevStageID)  stage, MKB(GXTevOp)  op, MKB(GXTevBias)  bias, MKB(GXTevScale)  scale, MKB(GXBool) clamp, MKB(GXTevRegID)  out_reg);
    void MKB(GXSetTevAlphaOp)(MKB(GXTevStageID)  stage, MKB(GXTevOp)  op, MKB(GXTevBias)  bias, MKB(GXTevScale)  scale, MKB(GXBool) clamp, MKB(GXTevRegID)  out_reg);
    void MKB(GXSetTevColor)(MKB(GXTevRegID)  id, struct MKB(GXColor) color);
    void MKB(GXSetTevKColor)(MKB(GXTevKColorID)  id, struct MKB(GXColor) color);
    void MKB(GXSetTevKColorSel)(MKB(GXTevStageID)  stage, MKB(GXTevKColorSel)  sel);
    void MKB(GXSetTevKAlphaSel)(MKB(GXTevStageID)  stage, MKB(GXTevKAlphaSel)  sel);
    void MKB(GXSetTevSwapMode)(MKB(GXTevStageID)  stage, MKB(GXTevSwapSel)  ras_sel, MKB(GXTevSwapSel)  tex_sel);
    void MKB(GXSetTevSwapModeTable)(MKB(GXTevSwapSel)  id, MKB(GXTevColorChan)  red, MKB(GXTevColorChan)  green, MKB(GXTevColorChan)  blue, MKB(GXTevColorChan)  alpha);
    void MKB(GXSetAlphaCompare)(MKB(GXCompare)  comp0, MKB(u8) ref0, MKB(GXAlphaOp)  op, MKB(GXCompare)  comp1, MKB(u8) ref1);
    void MKB(GXSetZTexture)(MKB(GXZTexOp)  op, MKB(GXTexFmt)  fmt, MKB(u32) bias);
    void MKB(GXSetTevOrder)(MKB(GXTevStageID)  stage, MKB(GXTexCoordID)  coord, MKB(GXTexMapID)  map, MKB(GXChannelID)  color);
    void MKB(GXSetNumTevStages)(MKB(u8) nStages);
    void MKB(GXSetFog)(MKB(GXFogType)  type, float startz, float endz, float nearz, float farz, struct MKB(GXColor) color);
    void MKB(GXSetFogRangeAdj)(MKB(GXBool) enable, MKB(u16) center, struct MKB(GXFogAdjTable) * table);
    void MKB(GXSetBlendMode)(MKB(GXBlendMode)  type, MKB(GXBlendFactor)  src_factor, MKB(GXBlendFactor)  dst_factor, MKB(GXLogicOp)  op);
    void MKB(GXSetColorUpdate)(MKB(GXBool) update_enable);
    void MKB(GXSetAlphaUpdate)(MKB(GXBool) update_enable);
    void MKB(GXSetZMode)(MKB(GXBool) compare_enable, MKB(GXCompare)  func, MKB(GXBool) update_enable);
    void MKB(GXSetZCompLoc)(MKB(GXBool) before_tex);
    void MKB(GXSetPixelFmt)(MKB(GXPixelFmt)  pix_fmt, MKB(GXZFmt16)  z_fmt);
    void MKB(GXSetDither)(MKB(GXBool) dither);
    void MKB(GXSetDstAlpha)(MKB(GXBool) enable, MKB(u8) alpha);
    void MKB(GXSetFieldMask)(MKB(GXBool) odd_mask, MKB(GXBool) even_mask);
    void MKB(GXSetFieldMode)(MKB(GXBool) field_mode, MKB(GXBool) half_aspect_ratio);
    void MKB(GXDrawCylinder)(MKB(u8) numEdges);
    void MKB(GXDrawSphere)(MKB(u8) numMajor, MKB(u8) numMinor);
    void MKB(GXDrawCubeFace)(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, int param_4_00, int param_5_00);
    void MKB(GXDrawCube)(void);
    void MKB(__GXSetRange)(void);
    void MKB(GXBeginDisplayList)(void * list, MKB(u32) size);
    MKB(u32) MKB(GXEndDisplayList)(void);
    void MKB(GXCallDisplayList)(void * list, MKB(u32) nbytes);
    void MKB(GXSetProjection)(MKB(f32) mtx[4][4], MKB(GXProjectionType)  type);
    void MKB(GXSetProjectionv)(MKB(f32) * ptr);
    void MKB(GXGetProjectionv)(float * ptr);
    undefined8 MKB(WriteMTXPS4x3)(float * param_1, float * param_2);
    void MKB(WriteMTXPS3x3from3x4)(float * param_1, float * param_2);
    undefined8 MKB(WriteMTXPS4x2)(float * param_1, float * param_2);
    void MKB(GXLoadPosMtxImm)(float mtxPtr[3][4], MKB(u32) id);
    void MKB(GXLoadNrmMtxImm)(float mtxPtr[3][4], MKB(u32) id);
    void MKB(GXSetCurrentMtx)(MKB(u32) id);
    void MKB(GXLoadTexMtxImm)(float * mtxPtr, MKB(u32) id, MKB(GXTexMtxType)  type);
    void MKB(GXSetViewportJitter)(float xOrig, float yOrig, float wd, float ht, float nearZ, float farZ, MKB(u32) field);
    void MKB(GXSetViewport)(float xOrig, float yOrig, float wd, float ht, float nearZ, float farZ);
    void MKB(GXGetViewportv)(float * vp);
    void MKB(GXSetScissor)(MKB(u32) xOrig, MKB(u32) yOrig, MKB(u32) wd, MKB(u32) ht);
    void MKB(GXGetScissor)(MKB(u32) * xOrig, MKB(u32) * yOrig, MKB(u32) * wd, MKB(u32) * ht);
    void MKB(GXSetScissorBoxOffset)(MKB(s32) xoffset, MKB(s32) yoffset);
    void MKB(GXSetClipMode)(MKB(GXClipMode)  mode);
    void MKB(__GXSetMatrixIndex)(int param_1);
    void MKB(GXSetGPMetric)(MKB(GXPerf0)  perf0, MKB(GXPerf1)  perf1);
    void MKB(GXClearGPMetric)(void);
    void MKB(GXInitXfRasMetric)(void);
    void MKB(GXReadXfRasMetric)(undefined4 * param_1, undefined4 * param_2, undefined4 * param_3, undefined4 * param_4);
    undefined4 MKB(ARCInitHandle)(void * arc_data, struct MKB(ARCHandle) * out_arc_handle);
    MKB(BOOL32) MKB(ARCOpen)(struct MKB(ARCHandle) * arc_handle, void * filebuf, struct MKB(ArcFileInfo) * arcFileInfo);
    uint MKB(arc_path_to_entrynum)(struct MKB(ARCHandle) * arcHandle, char * file);
    void MKB(arc_get_dir)(int param_1, int param_2, int param_3);
    void * MKB(ARCGetStartAddrInMem)(struct MKB(ArcFileInfo) * af);
    undefined4 MKB(arcGetLength)(struct MKB(ArcFileInfo) * fileInfo);
    undefined4 MKB(return_1)(void);
    void MKB(g_some_perf_init_func)(void);
    void MKB(PERFGetAllMemMetrics)(int param_1, int param_2);
    int MKB(PERFInit)(int param_1, uint param_2, uint param_3, MKB(undefined) * param_4, undefined4 param_5, undefined4 param_6);
    void MKB(PERFSetEvent)(uint param_1, undefined4 param_2, undefined4 param_3);
    void MKB(PERFStartFrame)(void);
    void MKB(PERFEndFrame)(void);
    void MKB(PERFEventStart)(MKB(PERFId) id);
    void MKB(PERFEventEnd)(uint param_1);
    void MKB(PERFStartAutoSample)(void);
    void MKB(PERFEndAutoSample)(void);
    void MKB(PERFStartAutoSampling)(double param_1);
    void MKB(PERFStopAutoSampling)(void);
    void MKB(__PERFDrawInit)(undefined4 param_1);
    void MKB(PERFPreDraw)(void);
    void MKB(DrawBWBar)(int param_1);
    void MKB(DrawKey)(void);
    void MKB(PERFDumpScreen)(void);
    void MKB(PERFPostDraw)(void);
    void MKB(g_set_something_with_perf)(undefined4 value);
    void MKB(ResetNotes)(int param_1);
    undefined4 * MKB(AllocateNote)(undefined4 param_1, byte param_2);
    undefined4 MKB(HandleNotes)(void);
    void MKB(KeyOffNotes)(void);
    void MKB(seqFreeKeyOffNote)(int * param_1);
    uint MKB(GetPublicId)(int param_1);
    uint MKB(seqGetPrivateId)(uint param_1);
    void MKB(empty_function)(void);
    uint MKB(seqStartPlay)(int param_1, int param_2, int param_3, int * param_4, uint * param_5, undefined1 param_6, undefined2 param_7);
    void MKB(HandleMasterTrack)(uint param_1);
    void MKB(StartPause)(int * param_1);
    void MKB(seqPause)(uint param_1);
    void MKB(seqStop)(uint param_1);
    void MKB(seqKillInstancesByGroupID)(short param_1);
    void MKB(seqSpeed)(uint param_1, undefined2 param_2);
    void MKB(seqContinue)(uint param_1);
    void MKB(seqMute)(uint param_1, undefined4 param_2, undefined4 param_3);
    void MKB(seqVolume)(uint param_1, uint param_2, uint param_3, byte param_4);
    void MKB(seqCrossFade)(uint * param_1, uint * param_2, char param_3);
    byte * MKB(GetStreamValue)(byte * param_1, ushort * param_2, short * param_3);
    int MKB(GenerateNextTrackEvent)(byte param_1);
    void MKB(InsertGlobalEvent)(int param_1, undefined4 * param_2);
    int MKB(HandleEvent)(int param_1, byte param_2, uint * param_3);
    void MKB(InitTrackEvents)(void);
    void MKB(InitTrackEventsSection)(uint param_1);
    undefined4 MKB(HandleTrackEvents)(uint param_1, undefined4 param_2);
    void MKB(seqHandle)(int param_1);
    int MKB(seqInit)(void);
    void MKB(synthSetBpm)(int param_1, byte param_2, uint param_3);
    undefined4 MKB(synthGetTicksPerSecond)(int param_1);
    void MKB(synthInitPortamento)(int param_1);
    uint MKB(do_voice_portamento)(byte param_1, char param_2, char param_3, int param_4, undefined4 * param_5);
    uint MKB(StartLayer)(undefined2 param_1, int param_2, undefined4 param_3, undefined4 param_4, uint param_5, byte param_6, uint param_7, uint param_8, byte param_9, undefined1 param_10, ushort param_11, undefined2 param_12, int param_13, undefined1 param_14, undefined1 param_15, int param_16);
    int * MKB(StartKeymap)(undefined2 param_1, short param_2, undefined4 param_3, undefined4 param_4, uint param_5, byte param_6, uint param_7, uint param_8, byte param_9, undefined1 param_10, ushort param_11, undefined2 param_12, uint param_13, undefined1 param_14, undefined1 param_15, int param_16);
    int * MKB(synthStartSound)(uint param_1, int param_2, undefined4 param_3, uint param_4, byte param_5, uint param_6, uint param_7, uint param_8, undefined1 param_9, ushort param_10, undefined2 param_11, undefined1 param_12, short param_13, undefined1 param_14, int param_15);
    void MKB(synthAddJob)(int * param_1, int * param_2, uint param_3);
    void MKB(synthStartSynthJobHandling)(int * param_1);
    void MKB(synthForceLowPrecisionUpdate)(int * param_1);
    void MKB(synthKeyStateUpdate)(int * param_1);
    void MKB(HandleJobQueue)(int * param_1, MKB(undefined) * param_2);
    void MKB(HandleFaderTermination)(int param_1);
    void MKB(synthHandle)(uint param_1);
    int * MKB(synthFXStart)(undefined2 param_1, byte param_2, uint param_3, undefined1 param_4, uint param_5);
    undefined4 MKB(synthFXSetCtrl)(uint param_1, byte param_2, byte param_3);
    undefined4 MKB(synthFXSetCtrl14)(uint param_1, byte param_2, uint param_3);
    void MKB(synthFXCloneMidiSetup)(int param_1, int param_2);
    undefined4 MKB(synthSendKeyOff)(uint param_1);
    void MKB(synthVolume)(uint param_1, uint param_2, uint param_3, undefined1 param_4, undefined4 param_5);
    undefined4 MKB(synthIsFadeOutActive)(uint param_1);
    void MKB(synthSetMusicVolumeType)(uint param_1, undefined1 param_2);
    void MKB(synthInit)(undefined4 param_1, int param_2);
    void MKB(sndSeqSpeed)(uint param_1, undefined2 param_2);
    void MKB(sndSeqContinue)(uint param_1);
    void MKB(sndSeqMute)(uint param_1, undefined4 param_2, undefined4 param_3);
    void MKB(sndSeqVolume)(uint param_1, uint param_2, uint param_3, byte param_4);
    undefined2 MKB(seqGetMIDIPriority)(uint param_1, uint param_2);
    undefined4 MKB(g_call_synthFXSetCtrl_with_irq_disabled)(uint param_1, byte param_2, byte param_3);
    undefined4 MKB(g_call_synthFXSetCtrl14_with_irq_disabled)(uint param_1, byte param_2, uint param_3);
    undefined4 MKB(sndFXKeyOff)(uint param_1);
    int * MKB(sndFXStartEx)(undefined2 param_1, byte param_2, uint param_3, byte param_4);
    int * MKB(sndFXStartParaInfo)(undefined2 param_1, byte param_2, uint param_3, byte param_4, byte * param_5);
    uint MKB(sndFXCheck)(uint param_1);
    void MKB(sndVolume)(uint param_1, uint param_2, uint param_3);
    void MKB(sndMasterVolume)(uint param_1, uint param_2, char param_3, char param_4);
    void MKB(sndSetAuxProcessingCallbacks)(uint param_1, int param_2, undefined4 param_3, char param_4, uint param_5, int param_6, undefined4 param_7, char param_8, uint param_9);
    void MKB(synthActivateStudio)(uint param_1, undefined1 param_2, undefined4 param_3);
    void MKB(synthDeactivateStudio)(uint param_1);
    void MKB(synthAddStudioInput)(uint param_1, byte * param_2);
    void MKB(synthRemoveStudioInput)(uint param_1, int param_2);
    void MKB(streamInit)(void);
    void MKB(streamHandle)(void);
    void MKB(streamCorrectLoops)(void);
    void MKB(streamKill)(int param_1);
    int MKB(GetPrivateIndex)(int param_1);
    void MKB(sndStreamARAMUpdate)(int param_1, uint param_2, uint param_3, uint param_4, uint param_5);
    void MKB(CheckOutputMode)(undefined1 * param_1, undefined1 * param_2);
    int MKB(sndStreamAllocEx)(undefined1 param_1, undefined4 param_2, int param_3, undefined4 param_4, undefined1 param_5, undefined1 param_6, undefined1 param_7, undefined1 param_8, undefined1 param_9, undefined1 param_10, uint param_11, undefined4 param_12, undefined4 param_13, undefined2 * param_14);
    uint MKB(sndStreamAllocLength)(int param_1, uint param_2);
    void MKB(sndStreamADPCMParameter)(int param_1, undefined2 * param_2);
    void MKB(sndStreamFrq)(int param_1, undefined4 param_2);
    void MKB(sndStreamFree)(int param_1);
    undefined4 MKB(sndStreamActivate)(int param_1);
    void MKB(sndStreamDeactivate)(int param_1);
    undefined4 MKB(dataInsertKeymap)(ushort param_1, undefined4 param_2);
    undefined4 MKB(dataRemoveKeymap)(short param_1);
    undefined4 MKB(dataInsertLayer)(ushort param_1, undefined4 param_2, undefined2 param_3);
    undefined4 MKB(dataRemoveLayer)(short param_1);
    undefined4 MKB(dataInsertCurve)(ushort param_1, undefined4 param_2);
    undefined4 MKB(dataRemoveCurve)(short param_1);
    undefined4 MKB(dataInsertSDir)(short * param_1, undefined4 param_2);
    undefined4 MKB(dataRemoveSDir)(short * param_1);
    undefined4 MKB(dataAddSampleReference)(short param_1);
    undefined4 MKB(dataRemoveSampleReference)(short param_1);
    undefined4 MKB(dataInsertFX)(short param_1, int param_2, uint param_3);
    undefined4 MKB(dataRemoveFX)(short param_1);
    undefined4 MKB(dataInsertMacro)(uint param_1, undefined4 param_2);
    undefined4 MKB(dataRemoveMacro)(uint param_1);
    undefined4 MKB(dataGetMacro)(uint param_1);
    undefined4 MKB(dataGetSample)(undefined2 param_1, undefined4 * param_2);
    undefined4 MKB(dataGetCurve)(undefined2 param_1);
    undefined4 MKB(dataGetKeymap)(undefined2 param_1);
    undefined4 MKB(dataGetLayer)(undefined2 param_1, undefined2 * param_2);
    int MKB(dataGetFX)(undefined2 param_1);
    void MKB(dataInit)(undefined4 param_1, int param_2);
    undefined4 MKB(mcmdWait)(int param_1, uint * param_2);
    void MKB(mcmdLoop)(int param_1, uint * param_2);
    void MKB(mcmdPlayMacro)(int param_1, uint * param_2);
    void MKB(mcmdStartSample)(int * param_1, uint * param_2);
    void MKB(mcmdVibrato)(int param_1, uint * param_2);
    void MKB(DoSetPitch)(int param_1);
    void MKB(mcmdSetADSR)(int param_1, uint * param_2);
    void MKB(mcmdSetPitchADSR)(int param_1, undefined4 * param_2);
    void MKB(DoPanningSetup)(int param_1, uint * param_2, uint param_3);
    uint MKB(TranslateVolume)(uint param_1, short param_2);
    void MKB(DoEnvelopeCalculation)(int param_1, uint * param_2, int param_3);
    void MKB(mcmdRandomKey)(int param_1, uint * param_2);
    void MKB(SelectSource)(int param_1, int param_2, uint * param_3, undefined4 param_4, uint param_5, uint param_6, uint param_7);
    void MKB(mcmdPortamento)(int param_1, uint * param_2);
    uint MKB(varGet32)(int param_1, int param_2, uint param_3);
    int MKB(varGet)(int param_1, int param_2, uint param_3);
    void MKB(varSet32)(int param_1, int param_2, uint param_3, undefined4 param_4);
    void MKB(mcmdVarCalculation)(int param_1, uint * param_2, byte param_3);
    void MKB(mcmdSendMessage)(int param_1, uint * param_2);
    void MKB(mcmdSetKeyGroup)(int param_1, uint * param_2);
    void MKB(macHandleActive)(int * param_1);
    void MKB(macHandle)(uint param_1);
    void MKB(macSampleEndNotify)(int * param_1);
    int MKB(macSetExternalKeyoff)(int * param_1);
    void MKB(macSetPedalState)(int * param_1, int param_2);
    void MKB(TimeQueueAdd)(int param_1);
    void MKB(UnYieldMacro)(int * param_1, int param_2);
    void MKB(macMakeActive)(int * param_1);
    void MKB(macMakeInactive)(int param_1, int param_2);
    uint MKB(macStart)(uint param_1, byte param_2, byte param_3, short param_4, byte param_5, undefined1 param_6, undefined1 param_7, uint param_8, byte param_9, undefined1 param_10, ushort param_11, undefined1 param_12, byte param_13, undefined1 param_14, undefined1 param_15, int param_16);
    void MKB(macInit)(void);
    int MKB(vidInit)(void);
    undefined4 * MKB(get_vidlist)(uint param_1);
    void MKB(vidRemoveVoiceReferences)(int param_1);
    undefined4 MKB(vidMakeRoot)(int param_1);
    uint MKB(vidMakeNew)(int param_1, int param_2);
    undefined4 MKB(vidGetInternalId)(uint param_1);
    void MKB(voiceRemovePriority)(int param_1);
    void MKB(voiceSetPriority)(int param_1, byte param_2);
    uint MKB(voiceAllocate)(byte param_1, byte param_2, short param_3, char param_4);
    void MKB(voiceFree)(int param_1);
    void MKB(synthInitAllocationAids)(void);
    uint MKB(voiceBlock)(byte param_1);
    void MKB(voiceUnblock)(int param_1);
    void MKB(voiceKill)(int param_1);
    undefined4 MKB(voiceKillSound)(uint param_1);
    void MKB(synthKillVoicesByMacroReferences)(ushort * param_1);
    undefined4 MKB(voiceIsLastStarted)(int param_1);
    void MKB(empty_function)(void);
    void MKB(voiceSetLastStarted)(int param_1);
    void MKB(voiceResetLastStarted)(int param_1);
    void MKB(voiceInitLastStarted)(void);
    int MKB(sndPitchUpOne)(uint param_1);
    void MKB(sndGetPitch)(uint param_1, uint param_2);
    void MKB(adsrConvertTimeCents)(void);
    undefined4 MKB(salChangeADSRState)(char * param_1);
    void MKB(g_mcmdWaitMs_or_adsrSetup)(char * param_1);
    undefined4 MKB(adsrStartRelease)(char * param_1, uint param_2);
    undefined4 MKB(adsrRelease)(byte * param_1);
    undefined4 MKB(adsrHandle)(char * param_1, undefined2 * param_2, short * param_3);
    undefined4 MKB(adsrHandleLowPrecision)(char * param_1, undefined2 * param_2, short * param_3);
    void MKB(vsInit)(void);
    uint MKB(vsSampleStartNotify)(byte param_1);
    void MKB(vsSampleEndNotify)(uint param_1);
    void MKB(vsUpdateBuffer)(int param_1, uint param_2);
    void MKB(vsSampleUpdates)(void);
    void MKB(dataInitStack)(void);
    void MKB(InsertData)(uint param_1, int * param_2, byte param_3, int param_4);
    void MKB(ScanIDList)(ushort * param_1, int * param_2, byte param_3, int param_4);
    void MKB(ScanIDListReverse)(ushort * param_1, int * param_2, byte param_3, int param_4);
    undefined4 MKB(sndPushGroup)(int * param_1, short param_2, undefined4 param_3, int * param_4, int * param_5);
    undefined4 MKB(sndPopGroup)(void);
    uint MKB(seqPlaySong)(short param_1, short param_2, int * param_3, uint * param_4, char param_5, undefined1 param_6);
    void MKB(sndSeqPlayEx)(short param_1, short param_2, int * param_3, uint * param_4, undefined1 param_5);
    undefined4 MKB(salInitDspCtrl)(byte param_1, byte param_2, int param_3);
    void MKB(salInitHRTFBuffer)(void);
    void MKB(salActivateStudio)(uint param_1, undefined1 param_2, undefined4 param_3);
    void MKB(salDeactivateStudio)(uint param_1);
    undefined4 MKB(salCheckVolErrorAndResetDelta)(undefined2 * param_1, undefined2 * param_2, short * param_3, short param_4, int param_5, ushort param_6);
    void MKB(HandleDepopVoice)(int param_1, int * param_2);
    void MKB(SortVoices)(int param_1, int param_2, int param_3);
    void MKB(salBuildCommandList)(undefined4 param_1, uint param_2);
    undefined4 MKB(salSynthSendMessage)(int param_1, undefined4 param_2);
    void MKB(salActivateVoice)(int param_1, byte param_2);
    void MKB(salDeactivateVoice)(int param_1);
    undefined4 MKB(salAddStudioInput)(int param_1, byte * param_2);
    undefined4 MKB(salRemoveStudioInput)(int param_1, int param_2);
    void MKB(salHandleAuxProcessing)(void);
    void MKB(salCalcVolume)(double param_1, double param_2, double param_3, char param_4, float * param_5, uint param_6, uint param_7, int param_8, int param_9);
    void MKB(UpdateRoomDistances)(void);
    void MKB(CheckRoomStatus)(void);
    void MKB(CheckDoorStatus)(void);
    void MKB(CalcEmitter)(int param_1, float * param_2, float * param_3, float * param_4, float * param_5, float * param_6);
    void MKB(SetFXParameters)(double param_1, double param_2, undefined8 param_3, double param_4, double param_5, int param_6);
    void MKB(EmitterShutdown)(int * param_1);
    void MKB(s3dKillEmitterByFXID)(short * param_1, int param_2);
    void MKB(AddRunningEmitter)(double param_1, int param_2);
    undefined4 MKB(AddStartingEmitter)(double param_1, double param_2, double param_3, double param_4, double param_5, int param_6);
    void MKB(StartContinousEmitters)(void);
    void MKB(s3dHandle)(void);
    void MKB(s3dInit)(uint param_1);
    int MKB(sndInit)(byte param_1, undefined1 param_2, undefined1 param_3, byte param_4, uint param_5, int param_6);
    void MKB(salApplyMatrix)(float * param_1, float * param_2, float * param_3);
    void MKB(salNormalizeVector)(float * param_1);
    void MKB(inpSetGlobalMIDIDirtyFlag)(uint param_1, uint param_2, uint param_3);
    void MKB(inpSetMidiCtrl)(byte param_1, byte param_2, byte param_3, byte param_4);
    void MKB(inpSetMidiCtrl14)(byte param_1, byte param_2, byte param_3, uint param_4);
    void MKB(inpResetMidiCtrl)(uint param_1, uint param_2, int param_3);
    uint MKB(inpGetMidiCtrl)(uint param_1, uint param_2, uint param_3);
    undefined4 MKB(return_0)(void);
    MKB(undefined) * MKB(inpGetChannelDefaults)(uint param_1, uint param_2);
    void MKB(inpResetChannelDefaults)(uint param_1, uint param_2);
    void MKB(inpAddCtrl)(int param_1, uint param_2, undefined4 param_3, byte param_4, int param_5);
    void MKB(inpFXCopyCtrl)(uint param_1, int param_2, int param_3);
    void MKB(inpSetMidiLastNote)(uint param_1, uint param_2, undefined1 param_3);
    undefined1 MKB(inpGetMidiLastNote)(uint param_1, uint param_2);
    uint MKB(_GetInputValue)(int param_1, byte * param_2, uint param_3, uint param_4);
    void MKB(inpInit)(int param_1);
    uint MKB(inpTranslateExCtrl)(uint param_1);
    undefined4 MKB(return_0x82)(void);
    undefined4 MKB(return_0xa0)(void);
    undefined4 MKB(return_0xa1)(void);
    undefined4 MKB(return_0x83)(void);
    undefined4 MKB(return_0x84)(void);
    undefined4 MKB(return_0xa2)(void);
    undefined4 MKB(return_0xa3)(void);
    undefined4 MKB(return_0xa4)(void);
    uint MKB(inpGetExCtrl)(int param_1, uint param_2);
    void MKB(inpSetExCtrl)(int param_1, uint param_2, short param_3);
    uint MKB(sndRand)(void);
    int MKB(sndSin)(uint param_1);
    int MKB(sndBSearch)(undefined4 param_1, int param_2, int param_3, int param_4, MKB(undefined) * param_5);
    void MKB(sndConvertMs)(int * param_1);
    void MKB(sndConvertTicks)(uint * param_1, int param_2);
    uint MKB(sndConvert2Ms)(uint param_1);
    void MKB(snd_handle_irq)(void);
    undefined4 MKB(hwInit)(undefined4 * param_1, byte param_2, byte param_3, uint param_4);
    void MKB(hwSetTimeOffset)(undefined1 param_1);
    undefined1 MKB(WPADGetDpdSensitivity)(void);
    bool MKB(hwIsActive)(int param_1);
    void MKB(hwSetPriority)(int param_1, undefined4 param_2);
    void MKB(hwInitSamplePlayback)(int param_1, undefined2 param_2, undefined4 * param_3, int param_4, undefined4 param_5, undefined4 param_6, int param_7, char param_8);
    void MKB(hwBreak)(int param_1);
    void MKB(hwSetADSR)(int param_1, uint * param_2, byte param_3);
    void MKB(hwSetVirtualSampleLoopBuffer)(int param_1, undefined4 param_2, undefined4 param_3);
    undefined1 MKB(hwGetVirtualSampleState)(int param_1);
    undefined1 MKB(hwGetVirtualSampleState)(int param_1);
    undefined2 MKB(hwGetSampleID)(int param_1);
    void MKB(hwSetStreamLoopPS)(int param_1, undefined1 param_2);
    void MKB(hwStart)(int param_1, byte param_2);
    void MKB(hwKeyOff)(int param_1);
    void MKB(hwSetPitch)(int param_1, ushort param_2);
    void MKB(hwSetSRCType)(int param_1, uint param_2);
    void MKB(hwSetPolyPhaseFilter)(int param_1, uint param_2);
    void MKB(hwSetITDMode)(int param_1, char param_2);
    void MKB(hwSetVolume)(double param_1, double param_2, double param_3, int param_4, char param_5, uint param_6, uint param_7);
    void MKB(hwOff)(int param_1);
    void MKB(hwSetAUXProcessingCallbacks)(uint param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5);
    int MKB(hwGetPos)(int param_1);
    void MKB(hwFlushStream)(int param_1, uint param_2, int param_3, uint param_4, undefined4 param_5, undefined4 param_6);
    void MKB(hwInitStream)(int param_1);
    void MKB(hwExitStream)(uint param_1);
    void MKB(hwGetStreamPlayBuffer)(uint param_1);
    void MKB(hwTransAddr)(void);
    void MKB(hwFrq2Pitch)(undefined4 param_1);
    void MKB(hwInitSampleMem)(undefined4 param_1, int param_2);
    void MKB(hwSaveSample)(int * param_1, int * param_2);
    void MKB(hwRemoveSample)(int param_1, undefined4 param_2);
    void MKB(hwSyncSampleMem)(void);
    void MKB(empty_function)(void);
    void MKB(sndSetHooks)(undefined4 * param_1);
    undefined4 MKB(hwGetVirtualSampleID)(int param_1);
    uint MKB(hwVoiceInStartup)(int param_1);
    void MKB(aramUploadData)(undefined4 param_1, undefined4 param_2, undefined4 param_3, int param_4, undefined4 param_5, undefined4 param_6);
    void MKB(aramSyncTransferQueue)(void);
    void MKB(aramInit)(int param_1);
    void MKB(aramGetZeroBuffer)(void);
    int MKB(aramStoreData)(void * param_1, int param_2);
    void MKB(aramRemoveData)(undefined4 param_1, int param_2);
    void MKB(InitStreamBuffers)(void);
    uint MKB(aramAllocateStreamBuffer)(int param_1);
    undefined4 MKB(aramGetStreamBufferAddress)(uint param_1, undefined4 * param_2);
    void MKB(aramFreeStreamBuffer)(uint param_1);
    void MKB(salCallback)(void);
    void MKB(Destroy)(void);
    bool MKB(salInitAi)(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    void MKB(salStartAi)(void);
    int MKB(salAiGetDest)(void);
    undefined4 MKB(salInitDsp)(void);
    void MKB(salCtrlDsp)(undefined4 param_1);
    uint MKB(salGetStartDelay)(void);
    void MKB(hwInitIrq)(void);
    void MKB(hwEnableIrq)(void);
    void MKB(hwDisableIrq)(void);
    void MKB(hwIRQEnterCritical)(void);
    void MKB(hwIRQLeaveCritical)(void);
    void MKB(salMalloc)(void);
    void MKB(salFree)(void);
    undefined4 MKB(sndAuxCallbackUpdateSettingsReverbHI)(void * param_1);
    void MKB(sndAuxCallbackPrepareReverbHI)(void * param_1);
    undefined4 MKB(g_something_with_sound8_wrapper)(int param_1);
    undefined4 MKB(ReverbHICreate)(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, void * param_7);
    undefined4 MKB(ReverbHIModify)(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, void * param_7);
    void MKB(DoCrossTalk)(double param_1, double param_2, uint * param_3, uint * param_4);
    void MKB(HandleReverb)(uint * param_1, int param_2, int param_3);
    void MKB(ReverbHICallback)(uint * param_1, uint * param_2, uint * param_3, int param_4);
    void MKB(ReverbHIFree)(int param_1);
    void MKB(do_src1)(undefined4 * param_1);
    void MKB(do_src2)(undefined4 * param_1);
    undefined4 MKB(sndAuxCallbackUpdateSettingsChorus)(int param_1);
    undefined4 MKB(sndAuxCallbackPrepareChorus)(int * param_1);
    undefined4 MKB(sndAuxCallbackShutdownChorus)(void);
    void * MKB(g_GXGetTexObjUserData_dup1_maybe_not)(struct MKB(GXTexObj) * obj);
    undefined4 MKB(SEQGetState)(int param_1);
    MKB(u32) MKB(g_GXGetTexObjTlut_maybe_not)(struct MKB(GXTexObj) * obj);
    void * MKB(g_GXGetTexObjUserData_dup2_maybe_not)(struct MKB(GXTexObj) * obj);
    MKB(GXTexFmt) MKB(g_GXGetTexObjFmt_dup2)(struct MKB(GXTexObj) * obj);
    void MKB(PPCMtwpar)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_GXInitTexObjUserData_dup1)(int param_1, undefined4 param_2);
    void MKB(g_GXInitTexObjUserData_dup2)(int param_1, undefined4 param_2);
    void MKB(g_GXInitTexObjUserData_dup3)(int param_1, undefined4 param_2);
    void MKB(g_GXInitTexObjUserData_dup4)(int param_1, undefined4 param_2);
    MKB(GXTexFmt) MKB(g_GXGetTexObjFmt_dup3)(struct MKB(GXTexObj) * obj);
    void MKB(g_adxt_set_out_pan)(int param_1, int param_2, int param_3);
    void MKB(g_GXInitTexObjUserData_dup5)(int param_1, undefined4 param_2);
    void MKB(empty_function)(void);
    void MKB(IFFifoAlloc)(int param_1);
    void MKB(g_create_some_threads)(undefined4 * g_optional_initial_thread_priorities);
    void MKB(mwidle_thread_entrypoint)(void * param);
    void MKB(vsync_thread_entrypoint)(void * param);
    void MKB(safe_thread_entrypoint)(void * param);
    void MKB(g_adxm_goto_mwidle_border)(void);
    void MKB(g_something_that_suspends_safe_thread)(void);
    void MKB(g_something_that_resumes_safe_thread)(void);
    void MKB(empty_function)(void);
    undefined4 MKB(return_0)(void);
    void MKB(g_some_printf_function_0)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(empty_function)(void);
    void MKB(g_cvFsAddDev)(char * param_1, MKB(undefined) * param_2);
    undefined4 MKB(math_init)(void);
    float MKB(CHUNK__math_rsqrt)(float param_1, float param_2);
    float MKB(math_sqrt)(float param_1);
    float MKB(math_rsqrt)(double param_1);
    float MKB(math_sqrt_rsqrt)(double param_1, float * param_2);
    double MKB(g_math_unknown1)(double param_1, double param_2);
    double MKB(g_math_unknown2)(double param_1);
    float MKB(math_sin)(MKB(s16) angle);
    void MKB(math_sin_cos_v)(MKB(s16) param_1, float * param_2);
    void MKB(g_math_sin_cos_wrapper)(MKB(s16) angle, float * out_sin, float * out_cos);
    void MKB(CHUNK__math_sin_cos)(MKB(s16) angle);
    void MKB(math_sin_cos)(MKB(s16) angle, float * out_sin, float * out_cos);
    float MKB(math_tan)(MKB(s16) param_1);
    MKB(s16) MKB(math_atan2)(double param_1, double param_2);
    int MKB(math_atan)(double param_1);
    int MKB(CHUNK__math_atan)(void);
    int MKB(g_math_unk3)(double param_1);
    int MKB(g_math_unk4)(double param_1);
    float MKB(vec_dot_normalized_safe)(struct MKB(Vec) * vec1, struct MKB(Vec) * vec2);
    double MKB(CHUNK__vec_dot_normalized_safe)(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6);
    void MKB(ray_scale)(float scale, struct MKB(Vec) * ray_start, struct MKB(Vec) * ray_end, struct MKB(Vec) * out_ray_end);
    void MKB(vec_set_len)(float len, struct MKB(Vec) * src, struct MKB(Vec) * dest);
    float MKB(vec_normalize_len)(struct MKB(Vec) * vec);
    float MKB(vec_dot_normalized)(struct MKB(Vec) * vec1, struct MKB(Vec) * vec2);
    undefined8 MKB(mtxa_from_identity)(void);
    void MKB(mtx_from_identity)(MKB(Mtx) * mtx);
    undefined8 MKB(mtxa_sq_from_identity)(void);
    void MKB(mtxa_from_translate)(struct MKB(Vec) * translate);
    void MKB(mtxa_from_translate_xyz)(float x, float y, float z);
    void MKB(mtxa_from_rotate_x)(short angle);
    void MKB(mtxa_from_rotate_y)(short angle);
    void MKB(mtxa_from_rotate_z)(short angle);
    void MKB(mtxa_from_mtxb_translate)(struct MKB(Vec) * vec);
    double MKB(mtxa_from_mtxb_translate_xyz)(double param_1, double param_2, double param_3);
    void MKB(mtxa_normalize_basis)(void);
    undefined8 MKB(mtxa_push)(void);
    void MKB(mtxa_pop)(void);
    void MKB(mtxa_to_mtx)(MKB(Mtx) * mtx);
    void MKB(mtxa_from_mtx)(MKB(Mtx) * mtx);
    undefined8 MKB(mtxa_peek)(void);
    void MKB(mtxa_sq_to_mtx)(MKB(Mtx) * mtx);
    void MKB(mtxa_sq_from_mtx)(MKB(Mtx) * mtx);
    undefined8 MKB(mtxa_from_mtxb)(void);
    undefined8 MKB(mtxa_to_mtxb)(void);
    void MKB(mtx_copy)(MKB(Mtx) * src, MKB(Mtx) * dest);
    void MKB(mtxa_invert)(void);
    void MKB(mtxa_rigid_invert)(void);
    void MKB(mtxa_mult_right)(MKB(Mtx) * mtx);
    void MKB(mtxa_mult_left)(MKB(Mtx) * mtx);
    void MKB(mtxa_from_mtxb_mult_mtx)(MKB(Mtx) * mtx);
    void MKB(mtx_mult)(MKB(Mtx) * mtx1, MKB(Mtx) * mtx2, MKB(Mtx) * dest);
    void MKB(mtxa_translate)(struct MKB(Vec) * vec);
    void MKB(mtxa_translate_xyz)(float x, float y, float z);
    void MKB(mtxa_translate_neg)(struct MKB(Vec) * vec);
    void MKB(mtxa_translate_neg_xyz)(float x, float y, float z);
    void MKB(mtxa_scale)(struct MKB(Vec) * vec);
    void MKB(mtxa_scale_s)(float scale);
    void MKB(mtxa_scale_xyz)(float x, float y, float z);
    void MKB(mtxa_tf_point)(struct MKB(Vec) * src, struct MKB(Vec) * dest);
    void MKB(mtxa_tf_vec)(struct MKB(Vec) * src, struct MKB(Vec) * dest);
    void MKB(mtxa_tf_point_xyz)(float x, float y, float z, struct MKB(Vec) * dest);
    void MKB(mtxa_tf_vec_xyz)(float x, float y, float z, struct MKB(Vec) * param_4);
    void MKB(mtxa_rigid_inv_tf_point)(struct MKB(Vec) * src, struct MKB(Vec) * dst);
    void MKB(mtxa_rigid_inv_tf_point_xyz)(float x, float y, float z, struct MKB(Vec) * dst);
    void MKB(mtxa_rigid_inv_tf_tl)(struct MKB(Vec) * dst);
    void MKB(mtxa_rigid_inv_tf_vec)(struct MKB(Vec) * src, struct MKB(Vec) * dst);
    void MKB(mtxa_rigid_inv_tf_vec_xyz)(float x, float y, float z, struct MKB(Vec) * dst);
    void MKB(CHUNK__mtxa_rigid_inv_tf_vec_xyz)(float x, float y, float z, undefined4 param_4, struct MKB(Vec) * dst, void * mtxa);
    void MKB(mtxa_rotate_x)(MKB(s16) angle);
    void MKB(mtxa_rotate_x_sin_cos)(float sin_x_angle, float cos_x_angle);
    void MKB(mtxa_rotate_y)(MKB(s16) angle);
    void MKB(mtxa_rotate_y_sin_cos)(float sin_y_angle, float cos_y_angle);
    void MKB(mtxa_rotate_z)(MKB(s16) angle);
    void MKB(mtxa_rotate_z_sin_cos)(float sin_z_angle, float cos_z_angle);
    void MKB(mtxa_from_quat)(struct MKB(Quat) * quat);
    void MKB(quat_mult)(struct MKB(Quat) * dest, struct MKB(Quat) * quat1, struct MKB(Quat) * quat2);
    undefined8 MKB(g_math_smth1)(float * param_1);
    void MKB(g_math_unk6)(float * param_1);
    void MKB(g_math_unk7)(double param_1, struct MKB(Quat) * param_2, float * param_3, float * param_4);
    void MKB(g_math_unk8)(double param_1, struct MKB(Quat) * param_2, float * param_3, float * param_4);
    void MKB(mtxa_to_quat)(struct MKB(Quat) * out_quat);
    void MKB(quat_from_axis_angle)(struct MKB(Quat) * quat, struct MKB(Vec) * axis, int angle);
    void MKB(g_math_unk9_smth_w_quats)(double param_1, struct MKB(Quat) * param_2, struct MKB(Vec) * param_3);
    void MKB(quat_to_axis_angle)(struct MKB(Quat) * quat, undefined4 out_axis);
    void MKB(quat_normalize)(struct MKB(Quat) * quat);
    void MKB(quat_from_dirs)(struct MKB(Quat) * out_quat, struct MKB(Vec) * start, struct MKB(Vec) * end);
    void MKB(quat_slerp)(float t, struct MKB(Quat) * dest, struct MKB(Quat) * quat1, struct MKB(Quat) * quat2);
    void MKB(g_math_quat_smth2)(struct MKB(Quat) * dst, struct MKB(Quat) * quat1, struct MKB(Quat) * quat2);
    void MKB(g_math_unk10)(struct MKB(Vec) * param_1, struct MKB(Vec) * param_2);
    void MKB(g_math_unk11)(struct MKB(Vec) * param_1, struct MKB(Vec) * param_2);
    void MKB(ray_to_euler)(struct MKB(Vec) * param_1, struct MKB(Vec) * param_2, struct MKB(S16Vec) * param_3);
    void MKB(ray_to_euler_xy)(float * param_1, float * param_2, MKB(s16) * param_3, MKB(s16) * param_4);
    void MKB(vec_to_euler)(struct MKB(Vec) * vec, struct MKB(S16Vec) * rot);
    void MKB(vec_to_euler_xy)(struct MKB(Vec) * vec, MKB(s16) * out_x_rot, MKB(s16) * out_y_rot);
    void MKB(g_math_unk12)(struct MKB(Vec) * param_1, float * param_2);
    void MKB(g_math_unk13)(undefined4 param_1, undefined4 param_2, struct MKB(Vec) * param_3);
    void MKB(mtxa_to_euler_yxz)(MKB(s16) * rot_y, MKB(s16) * rot_x, MKB(s16) * rot_z);
    void MKB(mtxa_to_euler)(struct MKB(S16Vec) rot);
    undefined8 MKB(g_math_unk14)(short * param_1, MKB(s16) * param_2, MKB(s16) * param_3);
    void MKB(g_math_unk15)(double param_1, struct MKB(Vec) * param_2, struct MKB(Vec) * param_3);
    void MKB(g_math_unk16)(float param_1, undefined4 param_2, undefined4 param_3);
    void MKB(g_math_unk17)(double param_1, double param_2, struct MKB(Vec) * param_3, struct MKB(Vec) * param_4);
    void MKB(g_math_unk18)(double param_1, double param_2, struct MKB(Vec) * param_3, struct MKB(Vec) * param_4);
    void MKB(g_math_unk19)(double param_1, double param_2, struct MKB(Vec) * param_3, struct MKB(Vec) * param_4);
    void MKB(g_init_console_gx)(void);
    void MKB(gx_start_new_frame)(void);
    void MKB(gp_wait)(void);
    void MKB(video_finish_frame)(void);
    MKB(u32) MKB(get_gx_fifo_use_size2)(void);
    void MKB(g_gx_reset_z_and_alpha_compare_modes)(void);
    int MKB(swap_gx_cpu_fifo)(void);
    void MKB(wait_for_gp_read_idle)(void);
    void MKB(gx_draw_done_callback_func)(void);
    void MKB(g_set_gx_vtx_formats)(void);
    void MKB(init_loched_cache)(MKB(BOOL32) use_locked_cache);
    void * MKB(g_something_with_locked_cache)(uint param_1);
    void MKB(g_make_arena_simulate_24mb_console)(void);
    void MKB(auto_set_render_mode)(MKB(BOOL32) use_progressive_for_ntsc);
    void MKB(g_init_gx)(MKB(BOOL32) make_second_fifo, MKB(u32) fifo_size, int g_something_with_alpha);
    void MKB(g_init_locked_cache_mtx_stack)(int matrix_stack, int param_2);
    void * MKB(allocate_mem_from_arena)(int size);
    void MKB(g_set_some_func_ptrs2)(undefined4 * param_1);
    void MKB(g_set_some_dvd_func_ptrs)(void);
    void * (* MKB(set_alloc_from_heap_func_ptr)(void * (* new_func)(MKB(u32))))(MKB(u32));
    void (* MKB(set_free_to_heap_func_ptr)(void (* new_func)(void *)))(void *);
    void MKB(reset_heap_alloc_func_ptrs)(void);
    int * MKB(g_load_raw_gma)(char * gma_file_name, struct MKB(TplBuffer) * tpl);
    MKB(GmaBuffer) * MKB(g_load_gma)(char * file_path, struct MKB(TplBuffer) * tpl);
    void MKB(g_something_freeing_heap)(int * param_1);
    MKB(TplBuffer) * MKB(g_load_tpl)(char * tpl_file_path);
    void MKB(g_something_freeing_heap_2)(void * param_1);
    void MKB(g_panic_in_avload)(char * param_1, undefined4 param_2);
    void * MKB(alloc_from_set_heap)(MKB(u32) size);
    void MKB(g_free_mem_to_heap_via_func_ptr)(void * mem);
    void MKB(g_init_gma_texture)(struct MKB(GmaTevLayer) * tex_desc, struct MKB(TplTextureHeader) * tpl_texture, struct MKB(TplBuffer) * tpl);
    void MKB(os_alloc_from_heap_wrapper)(MKB(u32) size);
    void MKB(os_free_to_heap_wrapper)(void * ptr);
    MKB(DVDFileInfo) * MKB(g_get_some_dvd_file_info)(void);
    MKB(BOOL32) MKB(dvd_open_wrapper)(char * file_name, struct MKB(DVDFileInfo) * file_info);
    MKB(s32) MKB(dvd_read_with_prio2)(struct MKB(DVDFileInfo) * fileInfo, void * addr, MKB(s32) length, MKB(s32) offset);
    MKB(BOOL32) MKB(dvd_close_wrapper)(struct MKB(DVDFileInfo) * file_info);
    void MKB(empty_function)(void);
    void MKB(g_something_with_fog)(double param_1, double param_2, int param_3);
    void MKB(g_something_with_raster_state)(int param_1);
    void MKB(g_set_some_projection_matrix)(double fov, double aspect_ratio, double near_clip, double far_clip);
    void MKB(g_smth_with_projection_mtx)(float * param_1);
    void MKB(gx_begin_display_list_wrapper)(void * list, MKB(u32) size);
    MKB(u32) MKB(gx_end_display_list_wrapper)(void);
    void MKB(g_zero_something)(void);
    void MKB(g_something_with_GX_vtx_desc)(uint param_1);
    void MKB(empty_function)(void);
    void MKB(load_gx_pos_nrm_mtx)(MKB(Mtx) * mtx, int id);
    undefined8 MKB(g_smth_with_gpu_and_locked_cache_mtx)(void);
    void MKB(g_something_with_texture_scroll_3)(undefined4 param_1, MKB(Mtx) * param_2);
    void MKB(avdisp_set_post_mult_color)(double red, double green, double blue, double alpha);
    void MKB(avdisp_set_post_add_color)(double red, double green, double blue, double alpha);
    void MKB(g_avdisp_set_ambient)(double g_red, double g_green, double g_blue);
    void MKB(g_some_GmaSomeStruct_func7)(struct MKB(GmaShape) * gma_struct, MKB(GXTevStageID)  * param_2);
    double MKB(g_some_GmaSomeStruct_func)(struct MKB(GmaShape) * param_1);
    double MKB(g_avdisp_smth_with_tex_materials)(struct MKB(GmaModel) * gma_header, struct MKB(GmaShape) * gma_struct);
    void MKB(g_some_GmaSomeStruct_func3)(struct MKB(GmaShape) * shape, int g_some_gma_value);
    void MKB(g_something_with_GmaSomeStruct_GXBlendFactor)(struct MKB(GmaShape) * gma_struct);
    void MKB(g_some_GmaSomeStruct_func5_GXSetChanCtrl)(struct MKB(GmaShape) * gma_struct);
    void MKB(g_some_GmaSomeStruct_func6)(struct MKB(GmaShape) * gma_struct, int param_2);
    undefined8 MKB(g_some_GmaSomeStruct_func5)(struct MKB(GmaShape) * gma_struct);
    void MKB(g_some_GmaSomeStruct_func4)(struct MKB(GmaShape) * param_1);
    void MKB(g_free_some_memory)(void);
    MKB(Mtx) * MKB(draw_poly)(int param_1, undefined4 * param_2);
    uint MKB(pointer_range_advance)(byte * g_frame_pointer, int * toset);
    void MKB(g_avdisp_draw_model_now1)(struct MKB(GmaModel) * model);
    void MKB(g_avdisp_draw_model_now2)(struct MKB(GmaModel) * model);
    void MKB(g_avdisp_draw_model_now3)(struct MKB(GmaModel) * model);
    void MKB(g_write_to_gfx_fifos)(struct MKB(GmaShape) * param_1, undefined4 param_2);
    int * MKB(avdisp_draw_deformable_shape)(uint * param_1, int param_2, struct MKB(GmaShape) * gma_struct, int param_4, struct MKB(GmaModel) * gma_header);
    undefined8 MKB(g_avdisp_smth_with_transforms)(struct MKB(GmaShape) * vert_control);
    void MKB(avdisp_draw_deformable_model)(struct MKB(GmaModel) * model, struct MKB(GmaShape) * first_shape, int param_3);
    void MKB(g_avdisp_draw_deformable_model2)(struct MKB(GmaModel) * model, struct MKB(GmaShape) * shape, int param_3, int param_4);
    MKB(GmaShape) * MKB(avdisp_draw_static_shape_now)(struct MKB(GmaModel) * model, struct MKB(GmaShape) * shape, struct MKB(GmaTevLayer) * tev_layers);
    void MKB(g_some_GmaSomeStruct_func2)(void * param_1);
    void MKB(g_iteratively_multiply_model_matrices)(struct MKB(GmaModel) * model);
    void MKB(g_avdisp_func8)(int param_1);
    void MKB(g_maybe_something_with_normals)(int param_1);
    void MKB(g_init_gma)(struct MKB(GmaBuffer) * gma_buffer, struct MKB(Gma) * gma_header, struct MKB(TplBuffer) * tpl);
    int MKB(g_init_gma_model_materials)(struct MKB(GmaModel) * model, struct MKB(TplBuffer) * tpl, struct MKB(GXTexObj) * texobj_array);
    void MKB(g_memcpy_using_locked_cache)(void * dest, void * curr_src_1_1_1_1_1_1_1_1_1_1_1_1_1_1_1, MKB(size_t) count);
    void MKB(g_something_with_locked_cache_2)(void * param_1, uint param_2, uint param_3);
    void MKB(memcpy2)(void * dest, void * src, MKB(size_t) count);
    int * MKB(__va_arg)(char * param_1, int param_2);
    void MKB(__destroy_global_chain)(void);
    int MKB(__cvt_fp2unsigned)(double param_1);
    void MKB(_savefpr_14)(void);
    void MKB(_savefpr_15)(void);
    void MKB(_savefpr_16)(void);
    void MKB(_savefpr_17)(void);
    void MKB(_savefpr_18)(void);
    void MKB(_savefpr_19)(void);
    void MKB(_savefpr_20)(void);
    void MKB(_savefpr_21)(void);
    void MKB(_savefpr_22)(void);
    void MKB(_savefpr_23)(void);
    void MKB(_savefpr_24)(void);
    void MKB(_savefpr_25)(void);
    void MKB(_savefpr_26)(void);
    void MKB(_savefpr_27)(void);
    void MKB(_savefpr_28)(void);
    void MKB(_savefpr_29)(void);
    void MKB(_savefpr_30)(void);
    void MKB(_savefpr_31)(void);
    void MKB(_restfpr_14)(void);
    void MKB(_restfpr_15)(void);
    void MKB(_restfpr_16)(void);
    void MKB(_restfpr_17)(void);
    void MKB(_restfpr_18)(void);
    void MKB(_restfpr_19)(void);
    void MKB(_restfpr_20)(void);
    void MKB(_restfpr_21)(void);
    void MKB(_restfpr_22)(void);
    void MKB(_restfpr_23)(void);
    void MKB(_restfpr_24)(void);
    void MKB(_restfpr_25)(void);
    void MKB(_restfpr_26)(void);
    void MKB(_restfpr_27)(void);
    void MKB(_restfpr_28)(void);
    void MKB(_restfpr_29)(void);
    void MKB(_restfpr_30)(void);
    void MKB(_restfpr_31)(void);
    void MKB(_savegpr_14)(void);
    void MKB(_savegpr_15)(void);
    void MKB(_savegpr_16)(void);
    void MKB(_savegpr_17)(void);
    void MKB(_savegpr_18)(void);
    void MKB(_savegpr_19)(void);
    void MKB(_savegpr_20)(void);
    void MKB(_savegpr_21)(void);
    void MKB(_savegpr_22)(void);
    void MKB(_savegpr_23)(void);
    void MKB(_savegpr_24)(void);
    void MKB(_savegpr_25)(void);
    void MKB(_savegpr_26)(void);
    void MKB(_savegpr_27)(void);
    void MKB(_savegpr_28)(void);
    void MKB(_savegpr_29)(void);
    void MKB(_savegpr_30)(void);
    void MKB(_savegpr_31)(void);
    void MKB(_restgpr_14)(void);
    void MKB(_restgpr_15)(void);
    void MKB(_restgpr_16)(void);
    void MKB(_restgpr_17)(void);
    void MKB(_restgpr_18)(void);
    void MKB(_restgpr_19)(void);
    void MKB(_restgpr_20)(void);
    void MKB(_restgpr_21)(void);
    void MKB(_restgpr_22)(void);
    void MKB(_restgpr_23)(void);
    void MKB(_restgpr_24)(void);
    void MKB(_restgpr_25)(void);
    void MKB(_restgpr_26)(void);
    void MKB(_restgpr_27)(void);
    void MKB(_restgpr_28)(void);
    void MKB(_restgpr_29)(void);
    void MKB(_restgpr_30)(void);
    void MKB(_restgpr_31)(void);
    undefined8 MKB(__div2u)(uint param_1, uint param_2, int param_3, uint param_4);
    undefined8 MKB(__div2i)(uint param_1, uint param_2, uint param_3, uint param_4);
    undefined8 MKB(__mod2u)(uint param_1, uint param_2, int param_3, uint param_4);
    void MKB(empty_function)(void);
    undefined8 MKB(__mod2i)(uint param_1, uint param_2, int param_3, uint param_4);
    undefined8 MKB(__shl2i)(int param_1, uint param_2, int param_3);
    undefined8 MKB(__shr2u)(uint param_1, uint param_2, int param_3);
    undefined8 MKB(__shr2i)(int param_1, uint param_2, uint param_3);
    double MKB(__cvt_sll_flt)(uint param_1, uint param_2);
    undefined8 MKB(__cvt_dbl_usll)(ulonglong param_1);
    undefined4 MKB(OSGetStackPointer)(void);
    void MKB(__fini_cpp_exceptions)(void);
    void MKB(__init_cpp_exceptions)(void);
    void MKB(__unregister_fragment)(int param_1);
    undefined4 MKB(__register_fragment)(undefined4 param_1, undefined4 param_2);
    void MKB(free)(int * * __ptr);
    void MKB(deallocate_from_fixed_pools)(undefined4 * param_1, int * param_2, uint param_3);
    uint * MKB(soft_allocate_from_var_pools)(int * param_1, int param_2, uint * param_3);
    void MKB(SubBlock_merge_next)(uint * param_1, uint * param_2);
    void MKB(Block_link)(int param_1, uint * param_2);
    undefined4 MKB(__flush_all)(void);
    void MKB(__close_all)(void);
    uint * MKB(__find_unopened_file)(void);
    void MKB(__num2dec)(double param_1_00, int param_2, char * param_3);
    void MKB(__num2dec_internal)(double param_1, char * param_2);
    uint MKB(__equals_dec)(int param_1, int param_2);
    void MKB(__two_exp)(undefined4 * param_1, ushort param_2);
    void MKB(__timesdec)(undefined1 * param_1, int param_2, int param_3);
    void MKB(__ull2dec)(undefined1 * param_1, undefined4 param_2, int param_3, int param_4);
    int MKB(__count_trailing_zerol)(uint param_1);
    int MKB(__flush_buffer)(undefined4 * param_1, undefined4 * param_2);
    void MKB(__prep_buffer)(int param_1);
    int MKB(g_very_similar_to_puts)(char * str);
    uint MKB(__put_char)(uint param_1, MKB(FILE) * param_2);
    int MKB(to_lower)(int c);
    MKB(size_t) MKB(__fwrite)(void * buffer, MKB(size_t) size, MKB(size_t) count, MKB(FILE) * stream);
    MKB(size_t) MKB(__fwrite2)(void * buffer, MKB(size_t) size, MKB(size_t) count, MKB(FILE) * stream);
    int MKB(fflush)(MKB(FILE) * __stream);
    int MKB(fclose)(MKB(FILE) * __stream);
    int MKB(_fseek)(MKB(FILE) * __stream, long __off, int __whence);
    int MKB(ftell)(int __stream);
    uint MKB(wcstombs)(int __s, ushort * __pwcs, uint __n);
    uint MKB(unicode_to_UTF8)(int param_1, uint param_2);
    int MKB(memcmp)(void * __s1, void * __s2, MKB(size_t) __n);
    char * MKB(__memrchr)(int param_1, char param_2, int param_3);
    void * MKB(memchr)(void * __s, int __c, MKB(size_t) __n);
    void * MKB(memmove)(void * __dest, void * __src, MKB(size_t) __n);
    void MKB(__copy_longs_rev_unaligned)(int param_1, int param_2, uint param_3);
    void MKB(__copy_longs_unaligned)(int param_1, int param_2, uint param_3);
    void MKB(__copy_longs_rev_aligned)(int param_1, int param_2, uint param_3);
    void MKB(__copy_longs_aligned)(int param_1, int param_2, uint param_3);
    void MKB(__sinit_AILoader_cpp)(void);
    int MKB(sprintf)(char * buffer, char * format, ...);
    MKB(s32) MKB(vsprintf)(char * out_str, char * format_str, MKB(va_list) args);
    int MKB(vprintf)(char * format, MKB(va_list) args);
    int MKB(printf)(char * format, ...);
    void * MKB(__StringWrite)(struct MKB(__OutStrCtrl) * osc, char * Buffer, MKB(size_t) NumChars);
    MKB(FILE) * MKB(__FileWrite)(MKB(FILE) * File, char * Buffer, MKB(size_t) NumChars);
    int MKB(__pformatter)(void (* WriteProc)(void *, char *, MKB(size_t)), void * WriteProcArg, char * format_str, MKB(va_list) arg, int is_secure);
    byte * MKB(float2str)(double param_1, int param_2, int param_3);
    void MKB(round_decimal)(int param_1, int param_2);
    char * MKB(double2hex)(double param_1, int param_2, int param_3);
    char * MKB(longlong2str)(uint param_1, int param_2, int param_3, char * param_4);
    char * MKB(long2str)(uint param_1, int param_2, char * param_3);
    char * MKB(parse_format)(int param_1, char * param_2, uint * param_3);
    void MKB(qsort)(void * __base, MKB(size_t) __nmemb, MKB(size_t) __size, MKB(__compar_fn_t) __compar);
    void MKB(srand)(MKB(u32) seed);
    int MKB(rand)(void);
    uint MKB(__StringRead)(int * param_1, uint param_2, int param_3);
    char * MKB(strstr)(char * __haystack, char * __needle);
    char * MKB(strchr)(char * __s, int __c);
    int MKB(strncmp)(char * __s1, char * __s2, MKB(size_t) __n);
    int MKB(strcmp)(char * __s1, char * __s2);
    char * MKB(strncat)(char * __dest, char * __src, MKB(size_t) __n);
    void MKB(strcat)(char * dest, char * src);
    char * MKB(strncpy)(char * __dest, char * __src, MKB(size_t) __n);
    char * MKB(strcpy)(char * __dest, char * __src);
    MKB(size_t) MKB(strlen)(char * __s);
    double MKB(atof)(char * __nptr);
    int MKB(atoi)(char * __nptr);
    uint MKB(__strtoul)(uint param_1, uint param_2, MKB(undefined) * param_3, undefined4 param_4, int * param_5, undefined4 * param_6, undefined4 * param_7);
    undefined4 MKB(return_0)(void);
    undefined4 MKB(__write_console)(undefined4 param_1, byte * param_2, uint * param_3);
    int MKB(fwide)(MKB(FILE) * stream, int mode);
    double MKB(__ieee754_acos)(double __x);
    double MKB(pow_internal)(double x, double y);
    uint MKB(__ieee754_rem_pio2)(double param_1, double * param_2);
    double MKB(__kernel_cos)(double param_1, double param_2);
    uint MKB(__kernel_rem_pio2)(double * param_1, double * param_2, int param_3, int param_4, int param_5, int param_6);
    double MKB(__kernel_sin)(double param_1, double param_2, int param_3);
    double MKB(__kernel_tan)(double param_1, double param_2, uint param_3);
    double MKB(atan)(double __x);
    double MKB(copysign)(double __x, double __y);
    double MKB(cos)(double __x);
    double MKB(floor)(double __x);
    double MKB(frexp)(double x, int * exponent);
    double MKB(ldexp)(double __x, int __exponent);
    double MKB(modf)(double __x, double * __iptr);
    double MKB(sin)(double __x);
    double MKB(tan)(double __x);
    float MKB(acosf)(float __x);
    double MKB(pow)(double x, double y);
    float MKB(floorf)(float __x);
    float MKB(powf)(float __x);
    float MKB(tanf)(float __x);
    float MKB(sinf)(float __x);
    float MKB(cosf)(float __x);
    void MKB(TRKNubMainLoop)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(TRKDestructEvent)(int param_1);
    void MKB(TRKConstructEvent)(undefined1 * param_1, undefined1 param_2);
    undefined4 MKB(TRKPostEvent)(int param_1);
    undefined4 MKB(TRKGetNextEvent)(int param_1);
    undefined4 MKB(TRKInitializeEventQueue)(void);
    void MKB(TRKNubWelcome)(void);
    undefined4 MKB(TRKTerminateNub)(void);
    int MKB(TRKInitializeNub)(void);
    void MKB(TRKMessageSend)(int param_1);
    void MKB(TRKReadBuffer_ui32)(int param_1, undefined1 * param_2, int param_3);
    void MKB(TRKReadBuffer_ui8)(int param_1, int param_2, int param_3);
    int MKB(TRKReadBuffer1_ui64)(int param_1, undefined1 * param_2);
    int MKB(TRKReadBuffer1_ui32)(int param_1, undefined1 * param_2);
    int MKB(TRKReadBuffer1_ui16)(int param_1, undefined1 * param_2);
    undefined4 MKB(TRKReadBuffer1_ui8)(int param_1, int param_2);
    void MKB(TRKAppendBuffer_ui32)(int param_1, undefined4 * param_2, int param_3);
    int MKB(TRKAppendBuffer_ui8)(int param_1, undefined1 * param_2, int param_3);
    void MKB(TRKAppendBuffer1_ui64)(int param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
    void MKB(TRKAppendBuffer1_ui32)(int param_1, undefined4 param_2);
    void MKB(TRKAppendBuffer1_ui16)(int param_1, undefined2 param_2);
    undefined4 MKB(TRKReadBuffer)(int param_1, int param_2, uint param_3);
    undefined4 MKB(TRKAppendBuffer)(int param_1, undefined1 * param_2, uint param_3);
    undefined4 MKB(TRKSetBufferPosition)(int param_1, uint param_2);
    void MKB(TRKResetBuffer)(int param_1, char param_2);
    void MKB(TRKReleaseBuffer)(int param_1);
    MKB(undefined) * MKB(TRKGetBuffer)(int param_1);
    int MKB(TRKGetFreeBuffer)(int * param_1, undefined4 * param_2);
    undefined4 MKB(TRKInitializeMessageBuffers)(void);
    undefined4 MKB(TRKTerminateSerialHandler)(void);
    undefined4 MKB(TRKInitializeSerialHandler)(void);
    void MKB(TRKProcessInput)(undefined4 param_1);
    void MKB(TRKGetInput)(void);
    int MKB(TRKTestForPacket)(void);
    void MKB(usr_put_initialize)(void);
    void MKB(usr_puts_serial)(char * param_1);
    undefined4 MKB(TRKDispatchMessage)(int param_1);
    undefined4 MKB(TRKTargetStop)(void);
    void MKB(TRKDoSetOption)(int param_1);
    void MKB(TRKDoStop)(int param_1);
    void MKB(TRKDoStep)(int param_1);
    void MKB(TRKDoContinue)(int param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(TRKDoFlushCache)(int param_1);
    void MKB(TRKDoWriteRegisters)(int param_1);
    void MKB(TRKDoReadRegisters)(int param_1);
    void MKB(TRKDoWriteMemory)(int param_1);
    void MKB(TRKDoReadMemory)(int param_1);
    void MKB(TRKDoCPUType)(int param_1);
    void MKB(TRKDoSupportMask)(int param_1);
    void MKB(TRKDoVersions)(int param_1);
    undefined4 MKB(TRKDoReset)(int param_1);
    int MKB(TRKDoDisconnect)(int param_1);
    void MKB(TRKDoConnect)(int param_1);
    void MKB(TRKDoUnsupported)(int param_1);
    void MKB(TRKStandardACK)(int param_1, undefined1 param_2, undefined1 param_3);
    void MKB(SetTRKConnected)(undefined4 param_1);
    int MKB(HandlePositionFileSupportRequest)(undefined4 param_1, undefined4 * param_2, undefined1 param_3, undefined1 * param_4);
    int MKB(HandleCloseFileSupportRequest)(undefined4 param_1, undefined1 * param_2);
    int MKB(HandleOpenFileSupportRequest)(char * param_1, undefined1 param_2, undefined4 * param_3, undefined1 * param_4);
    int MKB(TRKRequestSend)(int param_1, int * param_2, uint param_3, int param_4, int param_5);
    int MKB(TRKSuppAccessFile)(int param_1, int param_2, uint * param_3, char * param_4, int param_5, int param_6);
    undefined4 MKB(return_0)(void);
    undefined4 MKB(return_0)(void);
    undefined4 MKB(return_0)(void);
    int MKB(TRKDoNotifyStopped)(char param_1);
    void MKB(TRK_flush_cache)(uint param_1, int param_2);
    void MKB(TRK_fill_mem)(int param_1, byte param_2, uint param_3);
    void MKB(__TRK_get_MSR)(void);
    void MKB(__TRK_set_MSR)(void);
    void MKB(TRK_ppc_memcpy)(int param_1, int param_2, int param_3);
    undefined4 MKB(TRKPPCAccessFPRegister)(undefined4 * param_1, uint param_2, int param_3);
    undefined4 MKB(TRKTargetStop)(void);
    undefined4 MKB(TRKTargetStopped)(void);
    undefined4 MKB(TRKTargetFlushCache)(undefined4 param_1, uint param_2, uint param_3);
    int MKB(TRKTargetSupportRequest)(void);
    undefined4 MKB(TRKTargetGetPC)(void);
    undefined4 MKB(TRKTargetStepOutOfRange)(undefined4 param_1, undefined4 param_2, int param_3);
    undefined4 MKB(TRKTargetSingleStep)(int param_1, int param_2);
    void MKB(TRKTargetAddExceptionInfo)(int param_1);
    void MKB(TRKTargetAddStopInfo)(int param_1);
    int MKB(TRKTargetInterrupt)(byte * param_1);
    undefined4 MKB(TRKTargetCPUType)(undefined1 * param_1);
    undefined4 MKB(TRKTargetSupportMask)(undefined1 * param_1);
    undefined4 MKB(TRKTargetVersions)(undefined1 * param_1);
    int MKB(TRKTargetAccessExtended2)(uint param_1, uint param_2, int param_3, int * param_4, int param_5);
    uint MKB(TRKTargetAccessExtended1)(uint param_1, uint param_2, int param_3, int * param_4, int param_5);
    int MKB(TRKTargetAccessFP)(uint param_1, uint param_2, int param_3, int * param_4, int param_5);
    undefined4 MKB(TRKTargetAccessDefault)(int param_1, uint param_2, int param_3, int * param_4, int param_5);
    int MKB(TRKTargetAccessMemory)(int param_1, uint param_2, int * param_3, undefined4 param_4, int param_5);
    int MKB(TRKValidMemory32)(uint param_1, int param_2, uint param_3);
    void MKB(g_twi_0)(void);
    void MKB(g_twi_0)(void);
    void MKB(g_twi_0)(void);
    void MKB(g_twi_0)(void);
    undefined8 MKB(InitMetroTRK)(undefined4 param_1, undefined4 param_2, int param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    undefined4 MKB(TRKInitializeTarget)(void);
    uint MKB(TRKTargetTranslate)(uint param_1);
    void MKB(EnableMetroTRKInterrupts)(void);
    void MKB(TRKSaveExtended1Block)(void);
    void MKB(TRKRestoreExtended1Block)(void);
    undefined4 MKB(TRKTargetCPUMinorType)(void);
    void MKB(TRK_main)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    undefined4 MKB(TRKLoadContext)(undefined4 * param_1, int param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(empty_function)(void);
    void MKB(TRK_board_display)(char * param_1);
    void MKB(UnreserveEXI2Port)(void);
    void MKB(UnreserveEXI2Port)(void);
    int MKB(TRKReadUARTPoll)(undefined1 * param_1);
    undefined4 MKB(WriteUART1)(undefined1 param_1);
    int MKB(WriteUARTFlush)(void);
    void MKB(UnreserveEXI2Port)(void);
    undefined4 MKB(TRKInitializeIntDrivenUART)(void);
    void MKB(InitMetroTRKCommTable)(int param_1);
    void MKB(TRKEXICallBack)(undefined4 param_1, undefined4 * param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    undefined4 MKB(TRKTargetContinue)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(SetUseSerialIO)(undefined1 param_1);
    undefined4 MKB(__position_file)(void);
    undefined4 MKB(__close_file)(void);
    undefined4 MKB(__write_file)(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    undefined4 MKB(__write_file)(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    undefined4 MKB(__TRK_write_console)(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    undefined4 MKB(__TRK_write_console)(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    undefined4 MKB(DBWrite)(undefined4 * param_1, uint param_2);
    undefined4 MKB(DBRead)(undefined4 * param_1, int param_2);
    uint MKB(DBQueryData)(void);
    void MKB(DBInitInterrupts)(void);
    void MKB(DBInitComm)(undefined4 * param_1, undefined4 param_2);
    void MKB(DBGHandler)(short param_1);
    void MKB(MWCallback)(void);
    uint MKB(DBGReadStatus)(byte * param_1);
    uint MKB(DBGWrite)(uint param_1, undefined4 * param_2, int param_3);
    uint MKB(DBGRead)(uint param_1, undefined4 * param_2, int param_3);
    uint MKB(DBGReadStatus)(byte * param_1);
    undefined4 MKB(DBGEXIImm)(byte * param_1, int param_2, int param_3);
    undefined4 MKB(return_0)(void);
    undefined4 MKB(return_0)(void);
    void MKB(load_additional_rel)(char * rel_filepath, struct MKB(RelBufferInfo) * rel_buffer_ptrs);
    void MKB(g_purge_rel)(struct MKB(RelBufferInfo) * rel_buffer_info);
    void MKB(g_purge_rel2)(void);
    void MKB(main_loop_prolog)(void);
    void MKB(main_loop_epilog)(void);
    void MKB(main_loop_unlinked_func)(void);
    void MKB(g_something_with_sound6_and_stop_pad_motors_callback)(void);
    void MKB(g_something_with_card_callback)(void);
    void MKB(g_something_with_sound11_callback)(void);
    void MKB(revert_console_reset_callback)(void);
    uint MKB(console_reset_callback_impl)(void);
    void MKB(g_init_some_stuff)(void);
    void MKB(cd_to_dvd_root2)(void);
    void MKB(cd_to_dvd_root)(void);
    void MKB(g_create_initial_main_heap)(void);
    void MKB(g_remake_initial_main_heap_somehow)(void);
    void MKB(g_setup_and_create_game_heaps)(void);
    void MKB(g_something_with_sound7_and_game_heaps)(int param_1);
    void MKB(create_game_heaps)(int heap_config_idx);
    void MKB(destroy_game_heaps)(void);
    void * MKB(alloc_from_heap_or_panic)(MKB(OSHeapHandle) heap, MKB(u32) size, char * file, int line);
    void MKB(g_set_some_initial_state)(void);
    void MKB(mode_tick)(void);
    undefined4 MKB(g_something_with_title_debug_menu)(void);
    void MKB(handle_start_button_with_debug_mode)(void);
    void MKB(handle_start_button_no_debug_mode)(void);
    void MKB(smd_null)(void);
    void MKB(set_sub_mode_destination)(void * func);
    void MKB(call_then_reset_sub_mode_destination)(void);
    uint MKB(get_next_player_idx)(void);
    int MKB(g_get_next_stage_id)(void);
    void MKB(construct_pause_menu_sprite)(struct MKB(Sprite) * pause_sprite);
    void MKB(check_pause_menu_input)(struct MKB(Sprite) * pause_sprite);
    void MKB(handle_paused_input_in_menu)(struct MKB(Sprite) * pause_sprite);
    void MKB(handle_paused_input_out_of_menu)(struct MKB(Sprite) * pause_sprite);
    void MKB(pause_game)(void);
    void MKB(init_events)(void);
    void MKB(tick_events)(void);
    void MKB(event_init)(MKB(EventID)  event_id);
    void MKB(event_dest)(MKB(EventID)  event_id);
    void MKB(event_freeze)(MKB(EventID)  event_id);
    void MKB(event_restart)(MKB(EventID)  event_id);
    void MKB(dest_all_events)(void);
    void MKB(polydisp_main)(void);
    void MKB(draw_func_handler)(void);
    void MKB(adv_draw_func_handler)(void);
    void MKB(game_main_draw_func)(void);
    void MKB(g_draw_world_child)(void);
    void MKB(game_extra_draw_func)(void);
    void MKB(game_result_draw_func)(void);
    void MKB(handle_test_camera)(void);
    void MKB(g_set_clear_color)(void);
    void MKB(g_smth_with_bg_color_drawing)(struct MKB(GXColor) param_1);
    void MKB(g_draw_func_init)(void);
    void MKB(draw_stage_shadows)(void);
    void MKB(g_reset_shadow_state)(void);
    void MKB(fade_color_base_default)(void);
    void MKB(take_pausemenu_screenshot)(void * out_image_buffer, undefined4 src_left_px, undefined4 src_top_px, short width_px, short height_px, MKB(GXTexFmt)  fmt);
    void MKB(init_pausemenu_screenshot_texobj)(struct MKB(GXTexObj) * param_1);
    void MKB(g_draw_pausemenu_screenshot)(struct MKB(GXTexObj) * tex);
    uint MKB(md_adv_func)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(smd_adv_logo_init)(void);
    void MKB(smd_adv_logo_tick)(void);
    void MKB(smd_adv_demo_init)(void);
    void MKB(smd_adv_demo_tick)(void);
    void MKB(test_mode_sub_mode_handler)(void);
    void MKB(smd_adv_movie_return)(void);
    void MKB(g_smth_with_adv_sprites_and_stage_loading)(MKB(s32) param_1);
    void MKB(smd_adv_title_init)(void);
    void MKB(smd_adv_title_reinit)(void);
    void MKB(smd_adv_title_tick)(void);
    void MKB(smd_adv_info_init)(void);
    void MKB(smd_adv_info_tick)(void);
    void MKB(caseD_f3)(void);
    void MKB(smd_adv_game_ready_init)(void);
    void MKB(smd_adv_game_ready_tick)(void);
    void MKB(smd_adv_game_play_init)(void);
    void MKB(smd_adv_game_play_tick)(void);
    void MKB(smd_adv_ranking_init)(void);
    void MKB(smd_adv_ranking_tick)(void);
    undefined4 MKB(return_0x2c)(void);
    void MKB(smd_adv_start_init)(void);
    void MKB(smd_adv_start_tick)(void);
    void MKB(g_init_some_globals_for_adv)(void);
    void MKB(smd_adv_prog_init)(void);
    void MKB(smd_adv_prog_tick)(void);
    void MKB(smd_adv_first_logo_init)(void);
    void MKB(smd_adv_first_logo_tick)(void);
    void MKB(g_maybe_draw_sega_splash)(void);
    void MKB(g_something_with_loading_stages_on_gamestart)(void);
    void MKB(g_something_w_adv_demo_init)(void);
    undefined4 MKB(adv_demo_tick_function)(void);
    void MKB(g_camera_func30_child)(struct MKB(Camera) * camera);
    void MKB(create_demo_mes_sprite)(void);
    void MKB(sprite_demo_mes_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_demo_mes_disp)(struct MKB(Sprite) * param_1);
    void MKB(create_demo_mask_sprites)(void);
    void MKB(sprite_demo_mask_banana_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_set_lots_of_initial_state)(void);
    uint MKB(md_sel_func)(void);
    void MKB(smd_sel_ngc_dest)(void);
    void MKB(g_return_to_sel_ngc)(undefined4 param_1);
    void MKB(g_load_stage_for_menu_bg)(char param_1, int param_2);
    void MKB(g_set_some_sel_ngc_global_func_ptrs)(void * func1, void * func2, void * func3, void * func4);
    void MKB(smd_sel_ngc_init)(void);
    void MKB(smd_sel_ngc_tick)(void);
    void MKB(g_sel_draw_func_handler)(void);
    void MKB(g_sel_ngc_sub_mode_destination)(void);
    void MKB(init_cameras)(void);
    void MKB(event_camera_init)(void);
    void MKB(event_camera_tick)(void);
    void MKB(g_some_camera_parent_func)(void);
    void MKB(event_camera_dest)(void);
    void MKB(enable_camera)(int camera_idx);
    void MKB(disable_all_cameras)(void);
    void MKB(g_something_setting_camera_status)(void);
    void MKB(set_camera_view_settings)(float viewport_pos_x, float viewport_pos_y, float viewport_size_x, float viewport_size_y, undefined4 camera_idx);
    void MKB(g_something_with_camera2)(int player_number);
    void MKB(g_some_camera_setup_function)(void);
    void MKB(g_smth_with_camera_and_reflective_objects)(int param_1);
    void MKB(g_something_with_cameras)(void);
    void MKB(g_set_mode_of_all_cameras_optionally)(MKB(u8) param_1);
    void MKB(g_set_camera_mode)(uint param_1, byte mode);
    void MKB(g_camera_func27)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func28)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func29)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func30)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func54)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func55)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func43)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func12)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func13)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func52)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func53)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func80)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func81_mainmenu)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func87)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func88)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func10)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func11_spinin)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func38)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func0)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func1)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func2)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func3_setup)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func62)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func4_fallout)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func5)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func6)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func7)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func8)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func9)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func14)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func15_goal)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func16)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func17)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func18)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func19)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func20)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func21)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func22_23_24_timeover)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func25)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func26)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func31)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func32)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func33)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func34)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func35)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func36)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func37)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func39)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func40)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func41)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func42)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func44)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func45)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func46)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func47)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func48)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func49)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func50)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func51)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func56)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func57)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func63)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func66)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func68)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func69)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func70)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func71)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func72)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func73)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_default_func)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_main_camera_function_func75)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func76_gameplay)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func82)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func83)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func84)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func85)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func91)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func92)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    bool MKB(g_is_sphere_visible)(double radius, struct MKB(Vec) * center);
    bool MKB(g_is_sphere_visible_scaled)(double radius, double g_scale, struct MKB(Vec) * center);
    void MKB(g_init_lights)(void);
    void MKB(g_something_to_do_with_lights)(void);
    undefined4 MKB(g_some_stage_init_func)(void * param_1);
    void MKB(g_set_active_light_group)(int g_smth_with_lights);
    undefined4 MKB(get_active_light_group)(void);
    void MKB(push_light_group)(void);
    void MKB(pop_light_group)(void);
    void MKB(g_set_light_color_maybe)(double param_1, double param_2, double param_3, int param_4);
    void MKB(g_smth_with_avdisp_globals)(double param_1, double param_2, double param_3);
    void MKB(g_something_with_gx_modes_and_compare)(void);
    void MKB(gx_finish_frame_and_reset_z_and_alpha_compare_modes)(void);
    void MKB(g_something_with_graphics_processor2)(void);
    void MKB(g_something_with_video_this_also_finishes_a_frame)(void);
    void MKB(gp_wait_wrapper)(void);
    MKB(u32) MKB(get_gx_fifo_use_size)(void);
    void MKB(g_reset_cm_course)(void);
    void MKB(event_info_init)(void);
    void MKB(event_info_tick)(void);
    void MKB(event_info_dest)(void);
    void MKB(g_reset_mode_info_stuff)(void);
    bool MKB(did_ball_enter_goal)(struct MKB(Ball) * ball, int * out_stage_goal_idx, int * out_itemgroup_id, byte * out_goal_flags);
    MKB(BOOL32) MKB(did_ball_enter_wormhole)(struct MKB(Ball) * ball, int * out_wormhole_idx);
    void MKB(g_set_goaled)(void);
    void MKB(sprite_bonus_plus000_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_bonus_banana_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_ranking_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_ranking_disp)(void);
    void MKB(create_ranking_sprite)(struct MKB(Ball) * ball);
    void MKB(g_something_with_goals_and_ball)(struct MKB(Ball) * ball, int goal_idx, int ig_idx, MKB(undefined) param_4);
    MKB(BOOL32) MKB(did_ball_fallout)(struct MKB(Ball) * ball);
    void MKB(g_something_playing_soundeffects)(struct MKB(Ball) * ball);
    void MKB(g_load_init_common)(void);
    void MKB(g_load_minigame_gma_tpl)(int param_1);
    void MKB(g_something_with_loading_wl_tpls)(int world_number);
    void MKB(g_load_efcmdl_gmatpl)(void);
    void MKB(load_efcmdl_files_from_disc)(void);
    void MKB(g_something_freeing_heap_parent)(void);
    void MKB(process_inputs)(void);
    void MKB(threshold_analog_inputs)(void);
    void MKB(g_calc_frames_since_last_input_change)(void);
    void MKB(merge_inputs)(void);
    void MKB(load_bmp_com)(void);
    MKB(TplBuffer) * MKB(load_bmp)(char * filepath);
    void MKB(load_bmp_by_id_child)(int g_idx);
    void MKB(free_bmp_by_id)(int id);
    void MKB(free_nl2ngc_tpl_buf_to_heap)(struct MKB(TplBuffer) * param_1);
    void MKB(set_bmp_com_tpl_pointer)(int param_1);
    void MKB(g_zero_some_sprite_related_state)(void);
    void MKB(g_some_sprite_reset_gx_state_func)(void);
    void MKB(draw_ui)(void);
    void MKB(g_draw_sprite_requests_from_buffer)(void);
    MKB(u16) MKB(g_get_bmp_width)(uint param_1);
    MKB(u16) MKB(g_get_bmp_height)(uint param_1);
    void MKB(init_ui_element_sprite_with_defaults)(void);
    void MKB(set_ui_element_sprite_pos)(double x, double y);
    void MKB(set_ui_element_sprite_depth)(double depth);
    void MKB(g_set_ui_element_sprite_flags)(MKB(SpriteDrawReqFlags)  param_1);
    void MKB(set_ui_element_sprite_scale)(double x, double y);
    void MKB(set_ui_element_sprite_mult_color_from_u32)(MKB(u32) param_1);
    void MKB(set_ui_element_sprite_mult_color)(int param_1, uint param_2, uint param_3, uint param_4);
    void MKB(set_ui_element_sprite_add_color_from_u32)(MKB(u32) param_1);
    void MKB(set_ui_element_sprite_add_color)(int param_1, uint param_2, uint param_3, uint param_4);
    void MKB(set_ui_element_sprite_alpha)(double alpha);
    void MKB(set_ui_element_sprite_rot_z)(MKB(s32) param_1);
    void MKB(g_set_some_ui_element_sprite_flag)(MKB(SpriteDrawReqFlags)  param_1);
    void MKB(draw_ui_element)(MKB(s32) texture_id);
    void MKB(draw_ui_box)(uint g_texture_id);
    void MKB(draw_ui_box_only_borders)(uint param_1);
    void MKB(perf_init_timer)(int timer_id);
    uint MKB(perf_stop_timer)(int timer_id);
    void MKB(g_init_perf_profiling)(void);
    void MKB(g_perf_finish_frame)(void);
    void MKB(g_perf_start_frame)(void);
    void MKB(g_reset_draw_perf_counters)(void);
    void MKB(g_printf_sound_error)(MKB(BOOL32) is_error, char * origin, undefined4 g_error_format, ...);
    void * MKB(try_alloc_memory_for_sound)(MKB(u32) size);
    void MKB(g_OSFree_wrapper)(void * param_1);
    void MKB(init_sound)(void);
    void MKB(g_something_with_music_fading)(void);
    void MKB(hwExit_wrapper)(void);
    void MKB(ReadMusyXData)(MKB(undefined) param_1);
    void MKB(g_something_calls_sndPopGroup)(void);
    void MKB(g_something_calls_sndPopGroup_2)(void);
    void MKB(event_sound_init)(void);
    void MKB(event_sound_tick)(void);
    void MKB(event_sound_dest)(void);
    double MKB(g_smth_called_by_event_tick_sound_1)(char param_1, short param_2);
    void MKB(g_smth_called_by_event_tick_sound_2)(void);
    void MKB(g_set_smth_with_sound)(undefined1 param_1);
    void MKB(g_smth_calls_sndFXStartParaInfo)(struct MKB(GSoundCue) * cue);
    int MKB(g_something_with_volume)(uint * param_1, int param_2, int param_3);
    undefined4 MKB(g_something_calls_sndFXKeyOff)(uint param_1, int param_2, int param_3);
    int MKB(SoundReq)(uint g_some_id);
    int MKB(SoundReqDirect)(uint sfx_id);
    int MKB(SoundReqID)(uint g_sfx_id, int soundreq_arg);
    void MKB(call_SoundReqID_arg_0)(MKB(u32) g_sfx_id);
    void MKB(call_SoundReqID_arg_1)(uint g_sfx_id);
    void MKB(call_SoundReqID_arg_2)(uint g_sfx_id);
    void MKB(call_SoundReqID_arg_1_copy)(uint param_1);
    void MKB(SoundOff)(uint param_1);
    void MKB(SoundVol)(uint param_1, byte param_2);
    void MKB(SoundPan)(uint param_1, char param_2, char param_3);
    void MKB(SoundPitch)(uint param_1, uint param_2);
    void MKB(SoundDop)(uint param_1, uint param_2);
    void MKB(SoundMod)(uint param_1, uint param_2);
    void MKB(SoundRev)(uint param_1, byte param_2);
    void MKB(SoundCho)(uint param_1, byte param_2);
    bool MKB(SoundOffID)(int param_1);
    void MKB(SoundVolID)(int param_1, byte param_2);
    void MKB(SoundPanID)(int param_1, char param_2, char param_3);
    void MKB(SoundRevID)(int param_1, byte param_2);
    void MKB(SoundChoID)(int param_1, byte param_2);
    uint MKB(SoundCheckStateID)(int param_1);
    void MKB(SoundIcsReq)(uint param_1, byte param_2, char param_3);
    void MKB(g_fade_track_volume)(MKB(u32) volume, MKB(u8) param_2);
    int MKB(get_smgr_port)(char param_1, int param_2, char param_3, short * param_4);
    void MKB(g_something_with_bgm)(void);
    void MKB(g_crossfade_music)(void);
    void MKB(SoftStreamSEReq)(char param_1, int sfx_id, MKB(u32) sfx_volume, uint sfx_pan_L, uint sfx_pan_R);
    void MKB(empty_function)(void);
    void MKB(call_SoftStreamSEReq_arg_0)(int sfx_id, uint sfx_volume, uint sfx_pan_L, uint sfx_pan_R);
    void MKB(g_stop_music_sound_debug)(int param_1);
    void MKB(g_some_dvd_read_async_sound_callback2)(undefined4 param_1, struct MKB(DVDCommandBlock) * param_2);
    MKB(s32) MKB(SoftStreamStart)(MKB(u32) g_looping_state, MKB(BgmTrack)  g_bgm_id, MKB(u32) param_3);
    void MKB(empty_function)(void);
    void MKB(g_SoftStreamStart_with_some_defaults)(MKB(BgmTrack)  param_1);
    void MKB(g_SoftStreamStart_with_some_defaults_2)(MKB(BgmTrack)  param_1);
    void MKB(g_another_SoftStreamStart_wrapper_of_some_sort)(MKB(BgmTrack)  param_1, MKB(u32) param_2);
    undefined4 MKB(play_track_and_fade_out_other_tracks)(undefined4 param_1, undefined4 param_2, byte volume);
    MKB(s32) MKB(g_smth_related_to_music)(MKB(s32) param_1_00, MKB(s32) param_2_00, MKB(s32) param_3_00, MKB(u8) param_4, MKB(u8) param_5);
    void MKB(g_handle_world_bgm)(MKB(u32) g_volume);
    void MKB(g_something_with_stopping_music_or_sfx)(MKB(BgmTrack)  param_1);
    int MKB(g_maybe_related_to_music_crossfading)(int param_1);
    undefined4 MKB(g_check_current_track)(MKB(BgmTrack)  track_id);
    void MKB(g_maybe_smth_with_music)(MKB(s32) param_1, MKB(u8) param_2);
    void MKB(g_change_music_volume)(MKB(s32) param_1, MKB(s32) param_2, MKB(u8) volume);
    void MKB(SoundEffectInit)(void);
    void MKB(g_something_with_sound9)(void);
    void MKB(g_snd_dbg_call_sndAuxCallbackupdateSettingsReverbHI_1)(undefined4 * param_1);
    void MKB(g_snd_dbg_call_sndAuxCallbackUpdateSettingsChorus_2)(undefined4 * param_1);
    void MKB(g_snd_dbg_call_sndAuxCallbackUpdateSettingsReverbHI_2)(void);
    void MKB(g_snd_dbg_call_sndAuxCallbackUpdateSettingsChorus_1)(void);
    void MKB(g_set_some_sound_data)(void);
    void MKB(g_something_with_sound6)(void);
    void MKB(g_something_with_sound11)(void);
    void MKB(empty_function)(void);
    uint MKB(g_something_with_sound4)(undefined1 * param_1, undefined4 param_2, undefined1 param_3, undefined1 param_4, undefined1 param_5, undefined1 param_6);
    int MKB(g_smgr_allocate_stereo)(undefined4 param_1, undefined4 param_2, undefined1 param_3, undefined1 param_4, char * param_5, char * param_6, char param_7);
    void MKB(g_some_dvd_read_async_sound_callback)(MKB(s32) result, struct MKB(DVDFileInfo) * file_info);
    void MKB(event_adx_init)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(event_adx_tick)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(event_adx_dest)(void);
    void MKB(g_adx_error_call_back)(undefined4 param_1, char * param_2);
    void MKB(g_draw_ingame_debug_menu)(uint * param_1, int param_2);
    void MKB(g_draw_debug_window_boundaries)(uint * param_1);
    void MKB(draw_debugtext)(void);
    void MKB(window_init)(void);
    void MKB(g_something_with_replays3)(void);
    void MKB(debug_textdraw_set_pos)(uint g_x_pos, uint g_y_pos);
    void MKB(debug_window_printf)(MKB(undefined) param_1, char * format, ...);
    void MKB(debug_textdraw_set_color)(MKB(uint8_t) color);
    void MKB(debug_textdraw_put)(int buffer_idx, char * string);
    void MKB(g_set_up_debugtext_buffer)(void);
    undefined4 MKB(debug_textdraw_vsprintf_buf_0)(char * fmt, ...);
    void MKB(debug_textdraw_vsprintf_buf_1)(char * format, ...);
    void MKB(debug_textdraw_put_buf_1)(char * text);
    undefined4 MKB(g_some_printf_function_2)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, char * param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(draw_debugtext_char_en)(MKB(u32) x, MKB(u32) y, char ch, MKB(u8) color_rrggbbaa);
    void MKB(draw_debugtext_char_jp)(MKB(u32) x, MKB(u32) y, byte * ch, uint color_unknown_format);
    void MKB(draw_debugtext_window_bg)(double x1, double y1, double x2, double y2);
    bool MKB(g_is_sjis_and_more_than_24_mib)(void);
    void MKB(g_debug_print_ape)(int param_1, undefined4 param_2, int param_3);
    void MKB(debug_overlay_draw_fifo)(void);
    void MKB(unset_all_dip_switches)(void);
    void MKB(g_check_debug_mode_input)(void);
    void MKB(pool_init)(void);
    void MKB(pool_tick)(void);
    void MKB(pool_update_idxs_of_all_pools)(void);
    void MKB(pool_update_idxs)(struct MKB(PoolInfo) * tlm, MKB(EventID)  event_filter);
    void MKB(g_do_nothing)(void);
    int MKB(pool_alloc)(struct MKB(PoolInfo) * tlm, MKB(u8) status);
    void MKB(pool_clear)(struct MKB(PoolInfo) * info);
    void MKB(g_set_some_draw_values)(float param_1, float param_2, float param_3);
    void MKB(nl2ngc)(struct MKB(NlBuffer) * * out_nl_buffer, undefined4 out_tpl_buffer, char * p_lz_path, char * lz_path);
    undefined4 MKB(g_nl2ngc_gma_func)(struct MKB(NlBuffer) * nl_buf);
    void MKB(g_nl2ngc_tpl_func)(struct MKB(NlBuffer) * param_1, struct MKB(TplBuffer) * param_2);
    void MKB(g_nl_model_draw_func)(struct MKB(GmaModel) * param_1);
    void MKB(g_some_draw_func5)(int * param_1);
    void MKB(nl2ngc_set_fog_params)(double param_1, double param_2, undefined4 param_3);
    void MKB(nl2ngc_set_fog_color)(MKB(u8) r, MKB(u8) g, MKB(u8) b);
    void MKB(empty_function)(void);
    void MKB(g_ape_upright)(struct MKB(Ape) * ball);
    void MKB(g_move_ape_into_ball)(struct MKB(Ape) * ape);
    void MKB(g_default_ape_draw)(struct MKB(Ape) * ape, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(empty_function)(void);
    void MKB(event_ball_init)(void);
    void MKB(g_call_maybe_sets_number_of_starting_monkeys)(struct MKB(Ball) * ball);
    void MKB(create_stage_player)(byte player_index, MKB(BallMode)  physicsmode, byte ape_id, byte color_idx, uint LOD, void * draw_func, int g_variant);
    void MKB(g_create_stage_player_wrapper)(byte player_index, MKB(BallMode)  physicsMode, byte ape_id, byte param_4, MKB(ApeLOD)  LOD, void * draw_func);
    void MKB(challenge_mode_init)(void);
    void MKB(event_ball_tick)(void);
    void MKB(event_ball_dest)(void);
    void MKB(add_bananas)(int bananas_to_add);
    void MKB(g_something_with_translating_items)(double param_1, int param_2, struct MKB(Vec) * param_position);
    void MKB(g_maybe_sets_number_of_starting_monkeys)(struct MKB(Ball) * ball);
    void MKB(g_reset_ball)(struct MKB(Ball) * in_ball);
    void MKB(g_competition_mode_respawn_start)(struct MKB(Ball) * ball);
    void MKB(ball_physics_g_something_w_postgoal_slowdown)(struct MKB(Ball) * param_1);
    void MKB(ball_physics_g_something_w_postgoal_blast_up)(struct MKB(Ball) * param_1);
    void MKB(g_ball_mode_play_replay)(struct MKB(Ball) * ball);
    void MKB(g_handle_competition_mode_respawn)(struct MKB(Ball) * ball);
    void MKB(ball_physics_g_something_w_poastgoal_slowdown_blast_up)(struct MKB(Ball) * param_1);
    void MKB(ball_physics_g_something_w_postgoal_blast_up2)(struct MKB(Ball) * ball);
    void MKB(g_move_and_collide)(struct MKB(Ball) * ball, struct MKB(PhysicsBall) * physicsBall);
    void MKB(g_apply_ball_velocity)(struct MKB(Ball) * ball);
    void MKB(handle_ball_stage_coli)(struct MKB(Ball) * ball, struct MKB(PhysicsBall) * physicsball);
    void MKB(position_ball)(struct MKB(Ball) * ball, struct MKB(PhysicsBall) * phys_ball);
    void MKB(set_ball_properties)(struct MKB(Ball) * ball, int constants_idx);
    void MKB(ball_collision_stars)(struct MKB(Ball) * ball);
    void MKB(init_physicsball_from_ball)(struct MKB(Ball) * ball, struct MKB(PhysicsBall) * physicsball);
    void MKB(apply_physicsball_to_ball)(struct MKB(Ball) * ball, struct MKB(PhysicsBall) * physicsball);
    void MKB(g_ball_ape_rotation)(struct MKB(Ball) * ball);
    void MKB(spawn_postgoal_ball_sparkle)(void);
    void MKB(g_some_ballfunc)(struct MKB(Ball) * param_1);
    void MKB(ball_sounds_gameplay)(struct MKB(Ball) * ball);
    MKB(G_BallMode) * MKB(ball_movement_sparks)(struct MKB(Ball) * ball);
    void MKB(set_visual_scale)(struct MKB(Ball) * ball);
    void MKB(draw_storysel_ball_clear_half)(MKB(Mtx) * mtx, int lod);
    void MKB(g_draw_ball_and_ape)(void);
    void MKB(draw_ball_shadow)(void);
    void MKB(collide_ball_with_stage)(struct MKB(PhysicsBall) * physicsball, struct MKB(StagedefFileHeader) * stagedef);
    short * MKB(tri_coli_grid_lookup)(float x, float z, struct MKB(StagedefColiHeader) * coli_header);
    void MKB(collide_ball_with_tri_face)(struct MKB(PhysicsBall) * physicsball, struct MKB(StagedefColiTri) * tri);
    void MKB(collide_ball_with_tri_edges)(struct MKB(PhysicsBall) * physball, struct MKB(StagedefColiTri) * tri);
    void MKB(collide_ball_with_tri_edge)(struct MKB(PhysicsBall) * param_1, struct MKB(Vec) * param_2, struct MKB(Vec) * param_3, float * param_4);
    void MKB(collide_ball_with_tri_vertices)(struct MKB(PhysicsBall) * physball, struct MKB(StagedefColiTri) * tri);
    undefined8 MKB(collide_ball_with_tri_vertex)(struct MKB(PhysicsBall) * param_1, float * param_2, float * param_3);
    void MKB(g_some_jamabar_coli_func)(struct MKB(PhysicsBall) * physicsball, float * param_2);
    void MKB(collide_ball_with_cylinder)(struct MKB(PhysicsBall) * physball, struct MKB(StagedefColiCylinder) * cylinder);
    void MKB(stcoli_sub10)(struct MKB(PhysicsBall) * param_1, struct MKB(Vec) * param_2);
    void MKB(collide_ball_with_sphere)(struct MKB(PhysicsBall) * param_1, struct MKB(StagedefColiSphere) * param_2);
    void MKB(collide_ball_with_cone)(struct MKB(PhysicsBall) * param_1, struct MKB(StagedefColiCone) * param_2);
    void MKB(collide_ball_with_plane)(struct MKB(PhysicsBall) * physicsball, struct MKB(ColiPlane) * plane);
    MKB(BOOL32) MKB(line_intersects_rect)(struct MKB(Vec) * lineStart, struct MKB(Vec) * lineEnd, struct MKB(Rect) * rect);
    void MKB(stobj_jamabar_child_coli)(struct MKB(PhysicsBall) * physicsball, struct MKB(Stobj) * stobj);
    void MKB(raycast_stage_down)(struct MKB(Vec) * origin, struct MKB(RaycastHit) * out_hit, struct MKB(Vec) * out_vel_at_point);
    MKB(BOOL32) MKB(raycast_tri)(struct MKB(Vec) * line_origin, struct MKB(Vec) * line_dir, struct MKB(StagedefColiTri) * tri);
    MKB(BOOL32) MKB(raycast_cone)(struct MKB(Vec) * line_origin, undefined4 line_dir, struct MKB(StagedefColiCone) * cone, struct MKB(Vec) * out_hit_pos, struct MKB(Vec) * out_hit_normal);
    MKB(BOOL32) MKB(raycast_sphere)(struct MKB(Vec) * line_origin, struct MKB(Vec) * line_dir, struct MKB(StagedefColiSphere) * sphere, struct MKB(Vec) * out_hit_pos, struct MKB(Vec) * out_hit_normal);
    MKB(BOOL32) MKB(raycast_cylinder)(undefined4 line_origin, undefined4 line_dir, struct MKB(StagedefColiCylinder) * cylinder, undefined4 out_hit_pos, undefined4 out_hit_normal);
    uint MKB(collide_ball_with_goal)(struct MKB(PhysicsBall) * physicsball, struct MKB(StagedefGoal) * goal_def);
    void MKB(g_goal_coli_func1)(struct MKB(PhysicsBall) * param_1, struct MKB(Vec) * param_2);
    void MKB(g_goal_coli_func2)(struct MKB(PhysicsBall) * param_1, struct MKB(Vec) * param_2);
    void MKB(stcoli_sub25)(struct MKB(PhysicsBall) * param_1, int param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(g_draw_stage_collision)(void);
    void MKB(stcoli_sub27)(int param_1);
    void MKB(stcoli_sub28)(struct MKB(Vec) * param_1);
    void MKB(stcoli_sub29)(float * param_1, float * param_2, float * param_3, float * param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(tf_physicsball_by_mtxa)(struct MKB(PhysicsBall) * physicsball1, struct MKB(PhysicsBall) * physicsball2);
    void MKB(inv_tf_physicsball_by_mtxa)(struct MKB(PhysicsBall) * src_physicsball, struct MKB(PhysicsBall) * dest_physicsball);
    void MKB(tf_physball_to_itemgroup_space)(struct MKB(PhysicsBall) * physicsball, int dest_ig_idx);
    uint MKB(g_is_ball_in_ig_coli_range)(struct MKB(Itemgroup) * ig_list, struct MKB(StagedefColiHeader) * coli_header_list, undefined4 param_3, struct MKB(Vec) * physicsball_x);
    MKB(BOOL32) MKB(g_ball_ig_bound_sphere_overlap)(struct MKB(Itemgroup) * ig_anim, struct MKB(StagedefColiHeader) * ig_def, struct MKB(Vec) * physicsball_pos);
    void MKB(event_world_init)(void);
    void MKB(event_world_tick)(void);
    void MKB(event_world_dest)(void);
    double MKB(evaluate_stagedef_keyframe)(double g_anim_frame, int count, struct MKB(StagedefAnimKeyframe) * keyframe);
    void MKB(event_stage_init)(void);
    void MKB(event_stage_tick)(void);
    void MKB(event_stage_dest)(void);
    float MKB(advance_itemgroup_anim)(struct MKB(Itemgroup) * itemgroup, struct MKB(StagedefColiHeader) * colis_header);
    void MKB(g_advance_stage_animation)(void);
    void MKB(g_transform_some_itemgroup_vec)(void);
    MKB(GmaModel) * MKB(get_GmaBuffer_entry)(struct MKB(GmaBuffer) * buffer, char * name);
    void MKB(g_stgname_init)(void);
    void MKB(init_itemgroups)(void);
    void MKB(empty_function)(void);
    void MKB(load_stage)(int stage_id);
    void MKB(unload_stage)(void);
    void MKB(queue_stage_load)(uint stage_number);
    void MKB(g_load_stage_gma_tpl_lz)(MKB(u32) stage_id);
    undefined4 MKB(g_handle_bonus_wave_collision)(float * param_1, float * param_2, float * param_3);
    MKB(WorldTheme) MKB(get_stage_world_theme)(int stage_id);
    MKB(WorldTheme) MKB(get_stage_world_theme)(int stage_id);
    int MKB(g_some_draw_func2)(int param_1);
    int MKB(g_some_draw_func4)(double param_1, int param_2);
    void MKB(g_smth_with_stage_anim_groups)(int anim_group_id, uint param_2);
    MKB(BOOL32) MKB(g_smth_with_buttons)(int anim_group_id, uint param_2);
    void MKB(g_init_smth_with_seesaws)(void);
    void MKB(collide_ball_with_seesaws)(undefined2 param_1, undefined2 g_ball_idx, struct MKB(PhysicsBall) * param_3);
    undefined4 MKB(get_seesaw_replay_state_size)(struct MKB(SeesawInfo) * seesaw_info);
    void MKB(g_smth_with_stage_fog)(double param_1);
    bool MKB(is_stage_id_not_for_party_game)(int stage_id);
    void MKB(seesaw_init)(struct MKB(SeesawInfo) * seesaw_info);
    void MKB(seesaw_dest)(void);
    void MKB(seesaw_tick)(struct MKB(SeesawInfo) * seesaw_info);
    void MKB(seesaw_coli)(struct MKB(SeesawInfo) * seesaw_info, int param_2);
    void MKB(g_seesaw_func5)(int param_1, undefined4 * param_2);
    void MKB(g_seesaw_replay_smth)(double param_1, short * param_2, int param_3);
    void MKB(g_init_smth_with_stage_models)(void);
    void MKB(g_draw_stage_models)(void);
    void MKB(g_draw_goalpost_models)(void);
    void MKB(g_draw_wormhole_models)(void);
    void MKB(g_draw_generator_models)(void);
    void MKB(g_draw_start_position_marker)(void);
    void MKB(g_draw_stage)(void);
    void MKB(g_draw_collision_triangles)(void);
    bool MKB(is_stage_id_348_revolution)(void);
    void MKB(g_special_handler_for_st348_revolution)(void);
    void MKB(g_handle_hardcoded_special_case_stages)(void);
    void MKB(g_special_load_for_st348)(void);
    void MKB(g_smth_hardcoded_for_meltingpot)(void);
    void MKB(g_smth_hardcoded_for_totalitarianism_and_meltingpot)(void);
    void MKB(g_smth_hardcoded_for_st340)(void);
    void MKB(g_smth_hardcoded_for_trainworm_and_nintendo)(void);
    void MKB(g_init_shadows)(void);
    void MKB(g_draw_goal_and_wormhole_shadows)(void);
    int MKB(g_something_with_shadow_cast)(void);
    void MKB(g_load_stgname_file)(int locale_index);
    undefined4 MKB(g_load_stgname_dvd_entrynum)(MKB(s32) stgname_dvd_entrynum);
    int MKB(g_get_scen_stgname_buffer_size)(void);
    bool MKB(g_queue_stage_name_load)(void);
    undefined4 MKB(g_read_storymode_select_stage_names_from_dvd)(int * param_1);
    char * MKB(read_stage_name_from_dvd)(int stage_id, char * out_stage_name, int out_stage_name_buf_size);
    void MKB(load_stage_models)(void);
    void MKB(init_seesaws)(void);
    void MKB(load_stagedef)(MKB(u32) stage_id);
    void MKB(g_free_stage)(void);
    void MKB(g_something_with_replays5)(void);
    void MKB(event_recplay_init)(void);
    void MKB(event_recplay_tick)(void);
    void MKB(event_recplay_dest)(void);
    void MKB(g_something_with_recplay2)(void);
    void MKB(g_something_with_recplay3)(int param_1);
    undefined4 MKB(g_smth_with_replay_playback)(int param_1);
    bool MKB(is_itemgroup_playable)(MKB(u32) itemgroup_idx);
    MKB(u32) MKB(get_playable_itemgroup_count)(void);
    MKB(u32) MKB(compute_playable_itemgroup_count)(void);
    void MKB(g_smth_with_wormhole_replays)(int ball_idx);
    void MKB(g_get_replay_frame_data)(double replay_frames_remaining, int param_2, struct MKB(Vec) * position_out);
    void MKB(g_smth_with_seesaws_and_replays)(int param_1, uint param_2, uint param_3, void * param_4);
    void MKB(g_something_with_stage_world_themes)(int param_1);
    void MKB(g_something_with_replays4)(void);
    double MKB(g_something_with_replays6)(int param_1);
    void MKB(g_something_with_replays)(int param_1, void * param_2);
    void MKB(g_something_with_bundled_replays)(void);
    undefined4 MKB(g_LoadRecFile)(int param_1, int param_2);
    undefined4 MKB(g_something_with_replays2)(int param_1);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_something_with_score)(void);
    void MKB(g_advance_replay2)(double replay_frames_remaining, float * param_2);
    short MKB(g_get_replay_time)(void);
    uint MKB(rle_encode)(void * input, void * output, uint inputSize);
    uint MKB(get_compressed_replay_size)(void);
    uint MKB(compress_replay)(void * outCompressedReplay);
    int MKB(g_smth_with_cmp_recplay)(void * param_1);
    undefined4 MKB(g_some_replay_func5)(void);
    MKB(Replay) * MKB(init_replay)(struct MKB(Replay) * replay, int stage_time_limit);
    undefined4 MKB(init_replay_playable_ig_and_seesaw_state)(struct MKB(Replay) * replay, short * param_2, int param_3, MKB(u32) playableIgAndSeesawSize);
    MKB(Replay) * MKB(create_replay)(int stage_time_limit);
    void MKB(convert_replay_ptrs_to_offsets)(struct MKB(Replay) * replay, struct MKB(Replay) * outReplay);
    void MKB(convert_replay_offsets_to_ptrs)(struct MKB(Replay) * replay);
    void MKB(increment_score)(int g_some_flag, int score);
    int MKB(get_goal_score)(uint * g_jump_dist_ptr, int * g_score_ptr);
    void MKB(event_effect_init)(void);
    void MKB(event_effect_tick)(void);
    void MKB(event_effect_dest)(void);
    void MKB(g_draw_effects)(void);
    int MKB(spawn_effect)(struct MKB(Effect) * effect);
    void MKB(overwrite_effect_funcs)(MKB(EffectType)  effect_type, void (** new_effect_funcs)(struct MKB(Effect) *));
    void MKB(effect_default_init)(void);
    void MKB(effect_default_tick)(void);
    void MKB(effect_default_disp)(void);
    void MKB(effect_default_dest)(struct MKB(Effect) * effect);
    void MKB(effect_paperfrag_init)(struct MKB(Effect) * effect);
    void MKB(effect_paperfrag_tick)(short * param_1);
    void MKB(effect_paperfrag_disp)(int param_1);
    void MKB(effect_paperfrag_dest)(struct MKB(Effect) * effect);
    void MKB(effect_get_banana_init)(struct MKB(Effect) * effect);
    void MKB(effect_get_banana_tick)(struct MKB(Effect) * effect);
    void MKB(effect_get_banana_disp)(int param_1);
    void MKB(effect_get_banana_dest)(struct MKB(Effect) * effect);
    void MKB(effect_particle_init_coli)(int param_1);
    void MKB(effect_particle_tick_coli)(int param_1);
    void MKB(effect_particle_disp_coli)(int param_1);
    void MKB(effect_particle_dest_coli)(struct MKB(Effect) * effect);
    void MKB(effect_holding_banana_init)(struct MKB(Effect) * effect);
    void MKB(effect_holding_banana_tick)(struct MKB(Effect) * effect);
    void MKB(effect_holding_banana_disp)(struct MKB(Effect) * effect);
    void MKB(effect_holding_banana_dest)(struct MKB(Effect) * effect);
    void MKB(effect_levitate_init)(struct MKB(Effect) * effect);
    void MKB(effect_levitate_tick)(short * param_1);
    void MKB(effect_levitate_disp)(int param_1);
    void MKB(effect_levitate_dest)(struct MKB(Effect) * effect);
    void MKB(effect_colistar_particle_init)(struct MKB(Effect) * effect);
    void MKB(effect_colistar_particle_tick)(int param_1);
    void MKB(effect_colistar_particle_disp)(int param_1);
    void MKB(effect_colistar_particle_dest)(struct MKB(Effect) * effect);
    void MKB(effect_bgwat_bubble_base_init)(struct MKB(Effect) * effect);
    void MKB(effect_bgwat_bubble_base_tick)(int param_1);
    void MKB(effect_bgwat_bubble_base_disp)(void);
    void MKB(effect_bgwat_bubble_base_dest)(struct MKB(Effect) * effect);
    void MKB(effect_bgwat_bubble_init)(struct MKB(Effect) * effect);
    void MKB(effect_bgwat_bubble_tick)(int param_1);
    void MKB(effect_bgwat_bubble_disp)(int param_1);
    void MKB(effect_bgwat_bubble_dest)(struct MKB(Effect) * effect);
    void MKB(effect_meteo_init)(struct MKB(Effect) * effect);
    void MKB(effect_meteo_tick)(short * param_1);
    void MKB(effect_meteo_disp)(int param_1);
    void MKB(effect_meteo_dest)(struct MKB(Effect) * effect);
    void MKB(effect_meteo_fix_init)(struct MKB(Effect) * effect);
    void MKB(effect_meteo_fix_tick)(int param_1);
    void MKB(effect_meteo_fix_disp)(int param_1);
    void MKB(effect_meteo_fix_dest)(struct MKB(Effect) * effect);
    void MKB(effect_coliflash_init)(struct MKB(Effect) * effect);
    void MKB(effect_coliflash_tick)(int param_1);
    void MKB(effect_coliflash_disp)(int param_1);
    void MKB(effect_coliflash_dest)(struct MKB(Effect) * effect);
    void MKB(effect_bns_stg_star_init)(struct MKB(Effect) * effect);
    void MKB(effect_bns_stg_star_tick)(int param_1);
    void MKB(effect_bns_stg_star_disp)(int param_1);
    void MKB(effect_bns_stg_star_dest)(struct MKB(Effect) * effect);
    void MKB(effect_bns_stg_star_tail_init)(int param_1);
    void MKB(effect_bns_stg_star_tail_tick)(struct MKB(Effect) * effect);
    void MKB(effect_bns_stg_star_tail_disp)(int param_1);
    void MKB(effect_bns_stg_star_tail_dest)(struct MKB(Effect) * effect);
    void MKB(effect_bgmst_gen_cloud_init)(int param_1);
    void MKB(effect_bgmst_gen_cloud_tick)(ushort * param_1);
    void MKB(effect_bgmst_gen_cloud_disp)(int param_1);
    void MKB(effect_bgmst_gen_cloud_dest)(struct MKB(Effect) * effect);
    uint MKB(effect_bgstm_rainripple_init)(int param_1);
    void MKB(effect_bgstm_rainripple_tick)(int param_1);
    void MKB(effect_bgstm_rainripple_disp)(struct MKB(Effect) * effect);
    void MKB(effect_bgstm_rainripple_dest)(struct MKB(Effect) * effect);
    void MKB(effect_bgmst_water_init)(void);
    void MKB(effect_bgmst_water_tick)(void);
    void MKB(effect_bgmst_water_disp)(int param_1);
    void MKB(effect_bgmst_water_dest)(struct MKB(Effect) * effect);
    void MKB(effect_commendfrag_init)(int param_1);
    void MKB(effect_commendfrag_tick)(short * param_1);
    void MKB(effect_commendfrag_disp)(int param_1);
    void MKB(effect_commendfrag_dest)(struct MKB(Effect) * effect);
    void MKB(effect_banana_drop_init)(int param_1);
    void MKB(effect_banana_drop_tick)(int param_1);
    void MKB(effect_banana_drop_disp)(int param_1);
    void MKB(effect_banana_drop_dest)(struct MKB(Effect) * effect);
    void MKB(effect_bgend_water_init)(void);
    void MKB(effect_bgend_water_tick)(void);
    void MKB(effect_bgend_water_disp)(int param_1);
    void MKB(effect_bgend_water_dest)(struct MKB(Effect) * effect);
    void MKB(effect_bgbtmfire_ptcl_init)(int param_1);
    void MKB(effect_bgbtmfire_ptcl_tick)(int param_1);
    void MKB(effect_bgbtmfire_ptcl_disp)(int param_1);
    void MKB(effect_bgbtmfire_ptcl_dest)(struct MKB(Effect) * effect);
    void MKB(effect_piyo_init)(int param_1);
    void MKB(effect_piyo_tick)(short * param_1, int param_2);
    void MKB(effect_piyo_disp)(int param_1, int param_2);
    void MKB(effect_piyo_dest)(struct MKB(Effect) * effect);
    void MKB(event_background_init)(void);
    void MKB(event_background_tick)(void);
    void MKB(event_background_dest)(void);
    void MKB(g_maybe_some_bg_effect_handler)(void);
    void MKB(g_draw_bg)(void);
    void MKB(g_something_with_world_themes)(void);
    void MKB(g_something_with_init_or_maybe_leftover_from_smb1)(int param_1);
    void MKB(g_something_with_stage_world_themes2)(int theme_id);
    void MKB(g_draw_bg_model_shadows)(void);
    void MKB(call_item_coin_coli_func_for_cur_world_theme)(void);
    void MKB(bg_init_base)(void);
    void MKB(bg_tick_base)(void);
    void MKB(bg_dest_base)(void);
    void MKB(bg_disp_base)(void);
    void MKB(bg_item_coin_coli_base)(void);
    void MKB(g_handle_bg_fg_model_anim)(struct MKB(StagedefBackgroundModel) * bg_model_ptr, int bg_model_count);
    void MKB(g_render_foreground_objects)(MKB(Mtx) * g_stage_tilt_mtx, struct MKB(StagedefForegroundModel) * fg_model, int foreground_model_count);
    void MKB(g_something_with_texture_scroll)(struct MKB(StagedefTextureScroll) * tex_scroll);
    void MKB(bg_nig_init)(void);
    void MKB(bg_nig_tick)(void);
    void MKB(bg_nig_dest)(void);
    void MKB(bg_nig_disp)(void);
    void MKB(bg_nig_item_coin_coli)(void);
    void MKB(bg_ice2_init)(void);
    void MKB(bg_ice2_tick)(void);
    void MKB(bg_ice2_dest)(void);
    void MKB(bg_ice2_disp)(void);
    void MKB(bg_ice2_item_coin_coli)(void);
    void MKB(bg_bil_init)(void);
    void MKB(bg_bil_tick)(void);
    void MKB(bg_bil_dest)(void);
    void MKB(bg_bil_disp)(void);
    void MKB(bg_bil_item_coin_coli)(void);
    void MKB(bg_gol_init)(void);
    void MKB(bg_gol_tick)(void);
    void MKB(bg_gol_dest)(void);
    void MKB(bg_gol_disp)(void);
    void MKB(bg_gol_item_coin_coli)(void);
    void MKB(bg_bow_init)(void);
    void MKB(bg_bow_tick)(void);
    void MKB(bg_bow_dest)(void);
    void MKB(bg_bow_disp)(void);
    void MKB(bg_bow_item_coin_coli)(void);
    void MKB(bg_futsal_init)(void);
    void MKB(bg_futsal_tick)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(bg_futsal_dest)(void);
    void MKB(bg_futsal_disp)(void);
    void MKB(bg_futsal_item_coin_coli)(void);
    void MKB(bg_electric_init)(void);
    void MKB(bg_electric_tick)(void);
    void MKB(bg_electric_dest)(void);
    void MKB(bg_electric_disp)(void);
    void MKB(bg_electric_item_coin_coli)(void);
    void MKB(bg_boat_init)(void);
    void MKB(bg_boat_tick)(void);
    void MKB(bg_boat_dest)(void);
    void MKB(bg_boat_disp)(void);
    void MKB(bg_boat_item_coin_coli)(void);
    void MKB(bg_park_init)(void);
    void MKB(bg_park_tick)(void);
    void MKB(bg_park_dest)(void);
    void MKB(bg_park_disp)(void);
    void MKB(bg_park_item_coin_coli)(void);
    void MKB(g_smth_with_bg_model_names)(struct MKB(GmaModelEntry) * model, void * g_some_func);
    void MKB(g_init_stagedef_bg_fg_models)(struct MKB(StagedefBackgroundModel) * model, int count, struct MKB(GSomeBgStruct) * some_bg_struct, void * g_effect_func);
    MKB(BOOL32) MKB(is_game_paused_and_in_view_stage)(void);
    int MKB(g_smth_with_challenge_mode_var_and_3p)(void);
    void MKB(bg_jun_init)(void);
    void MKB(bg_jun_tick)(void);
    void MKB(bg_jun_dest)(void);
    void MKB(bg_jun_disp)(void);
    void MKB(bg_jun_item_coin_coli)(void);
    void MKB(bg_snd_init)(void);
    void MKB(bg_snd_tick)(void);
    void MKB(bg_snd_dest)(void);
    void MKB(bg_snd_disp)(void);
    void MKB(bg_snd_item_coin_coli)(void);
    void MKB(bg_wat_init)(void);
    void MKB(bg_wat_tick)(void);
    void MKB(bg_wat_dest)(void);
    void MKB(bg_wat_disp)(void);
    void MKB(bg_wat_item_coin_coli)(void);
    undefined4 MKB(g_set_bgwat_bubble_effect)(int param_1, uint * param_2);
    void MKB(bg_spa_init)(void);
    void MKB(bg_spa_tick)(void);
    void MKB(bg_spa_dest)(void);
    void MKB(bg_spa_disp)(void);
    void MKB(bg_spa_item_coin_coli)(void);
    void MKB(bg_sun_init)(void);
    void MKB(bg_sun_tick)(void);
    void MKB(bg_sun_dest)(void);
    void MKB(bg_sun_disp)(void);
    void MKB(bg_sun_item_coin_coli)(void);
    void MKB(bg_bns_init)(void);
    void MKB(bg_bns_tick)(void);
    void MKB(bg_bns_dest)(void);
    void MKB(bg_bns_disp)(void);
    void MKB(bg_bns_item_coin_coli)(int param_1);
    void MKB(bg_stm_init)(void);
    void MKB(bg_stm_tick)(void);
    void MKB(bg_stm_dest)(void);
    void MKB(bg_stm_disp)(void);
    void MKB(bg_stm_item_coin_coli)(void);
    void MKB(bg_mst_init)(void);
    void MKB(bg_mst_tick)(void);
    void MKB(bg_mst_dest)(void);
    void MKB(bg_mst_disp)(void);
    void MKB(bg_mst_item_coin_coli)(void);
    void MKB(bg_pil_init)(void);
    void MKB(bg_pil_tick)(void);
    void MKB(bg_pil_dest)(void);
    void MKB(bg_pil_disp)(void);
    void MKB(bg_pil_item_coin_coli)(void);
    void MKB(bg_end_init)(void);
    void MKB(bg_end_tick)(void);
    void MKB(bg_end_dest)(void);
    void MKB(bg_end_disp)(void);
    void MKB(bg_end_item_coin_coli)(void);
    void MKB(bg_lava_init)(void);
    void MKB(bg_lava_tick)(void);
    void MKB(bg_lava_dest)(void);
    void MKB(bg_lava_disp)(void);
    void MKB(bg_lava_item_coin_coli)(void);
    void MKB(g_draw_lava_particles)(int param_1);
    void MKB(g_something_with_stage_heap_and_lava_theme)(int * param_1);
    void MKB(bg_wat2_init)(void);
    void MKB(bg_wat2_tick)(void);
    void MKB(bg_wat2_dest)(void);
    void MKB(bg_wat2_disp)(void);
    void MKB(bg_wat2_item_coin_coli)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(wat2_draw_caustics)(void * some_pointer);
    void MKB(bg_pil2_init)(void);
    void MKB(bg_pil2_tick)(void);
    void MKB(bg_pil2_dest)(void);
    void MKB(bg_pil2_disp)(void);
    void MKB(bg_pil2_item_coin_coli)(void);
    void MKB(bg_spa2_init)(void);
    void MKB(bg_spa2_tick)(void);
    void MKB(bg_spa2_dest)(void);
    void MKB(bg_spa2_disp)(void);
    void MKB(bg_spa2_item_coin_coli)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(bg_bil2_init)(void);
    void MKB(bg_bil2_tick)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(bg_bil2_dest)(void);
    void MKB(bg_bil2_disp)(void);
    void MKB(bg_bil2_item_coin_coli)(void);
    void MKB(bg_pot_init)(void);
    void MKB(bg_pot_tick)(void);
    void MKB(bg_pot_dest)(void);
    void MKB(bg_pot_disp)(void);
    void MKB(bg_pot_item_coin_coli)(void);
    void MKB(bg_pot_fgt_init)(void);
    void MKB(bg_pot_fgt_tick)(void);
    void MKB(bg_pot_fgt_dest)(void);
    void MKB(bg_pot_fgt_disp)(void);
    void MKB(bg_pot_fgt_item_coin_coli)(void);
    void MKB(empty_function)(void);
    void MKB(g_something_with_stage_heap_and_pot_theme)(int param_1);
    void MKB(bg_park_init)(void);
    void MKB(bg_park_tick)(void);
    void MKB(bg_park_dest)(void);
    void MKB(bg_park_disp)(void);
    void MKB(bg_park_item_coin_coli)(void);
    void MKB(bg_whale_init)(void);
    void MKB(bg_whale_tick)(void);
    void MKB(bg_whale_dest)(void);
    void MKB(bg_whale_disp)(void);
    void MKB(bg_whale_item_coin_coli)(void);
    void MKB(g_something_with_stage_heap_and_whale_theme)(int param_1);
    short * MKB(g_something_with_stage_heap_and_whale_theme2)(int param_1);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(bg_bubble_init)(void);
    void MKB(bg_bubble_tick)(void);
    void MKB(bg_bubble_dest)(void);
    void MKB(bg_bubble_disp)(void);
    void MKB(bg_bubble_item_coin_coli)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(bg_gear_init)(void);
    void MKB(bg_gear_author_tick)(void);
    void MKB(bg_gear_author_dest)(void);
    void MKB(bg_gear_author_disp)(void);
    void MKB(bg_gear_item_coin_coli)(void);
    void MKB(g_something_with_stage_heap_and_gear_theme)(int param_1);
    void MKB(bg_jun2_init)(void);
    void MKB(bg_jun2_tick)(void);
    void MKB(bg_jun2_dest)(void);
    void MKB(bg_jun2_disp)(void);
    void MKB(bg_jun2_item_coin_coli)(void);
    void MKB(g_load_ape_bg)(void);
    void MKB(bg_bns2_init)(void);
    void MKB(bg_bns2_tick)(void);
    void MKB(bg_bns2_dest)(void);
    void MKB(bg_bns2_disp)(void);
    void MKB(bg_bns2_item_coin_coli)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(bg_bow2_author_init)(void);
    void MKB(bg_bow2_author_tick)(void);
    void MKB(bg_bow2_author_dest)(void);
    void MKB(bg_bow2_author_disp)(void);
    void MKB(bg_bow2_author_item_coin_coli)(void);
    void MKB(clear_unlocked_cm_stages)(void);
    void MKB(init_course)(void);
    void MKB(event_course_init)(void);
    void MKB(event_course_tick)(void);
    void MKB(event_course_dest)(void);
    bool MKB(is_stage_complete)(struct MKB(CourseCommand) * entry);
    bool MKB(entered_goal_has_type)(struct MKB(CourseCommand) * entry);
    bool MKB(did_beat_stage_faster_than)(struct MKB(CourseCommand) * entry);
    void MKB(calc_stage_jump_distance)(struct MKB(CourseCommand) * entry);
    void MKB(clear_next_cm_stage_id2)(struct MKB(CourseCommand) * entry);
    void MKB(clear_next_cm_stage_id)(struct MKB(CourseCommand) * entry);
    MKB(s32) MKB(get_current_cm_stage_time_limit)(void);
    MKB(u32) MKB(g_update_cm_course)(MKB(Difficulty)  difficulty, MKB(s32) course_stage_num, MKB(ModeFlag)  mode_flags);
    int MKB(calc_course_idx)(MKB(Difficulty)  difficulty, MKB(ModeFlag)  mode_flags);
    int MKB(g_get_some_difficulty_count_2)(int param_1, MKB(ModeFlag)  mode_flags);
    bool MKB(g_are_on_final_course_level)(int difficulty_id, int course_stage, uint difficulty_flags);
    bool MKB(is_bonus_stage)(int stage_id);
    void MKB(g_something_with_cm_entries_practice_mode)(void);
    undefined4 MKB(g_smth_with_cm_entries_in_main_menu)(int param_1, int param_2, uint param_3);
    void MKB(empty_function)(void);
    void MKB(update_cm_unlocked_levels)(MKB(Difficulty)  difficulty, int param_2, MKB(ModeFlag)  mode_flags);
    void MKB(clear_cm_player_progress)(void);
    void MKB(update_course_progress)(void);
    void MKB(g_something_with_cm_player_progress)(void);
    void MKB(sprite_debug_course_display_disp)(int param_1);
    void MKB(g_save_cm_unlock_entries)(void);
    void MKB(g_load_cm_unlock_entries)(void);
    void MKB(clear_sm_state)(void);
    int MKB(get_world_stage_count)(int world);
    int MKB(get_world_unbeaten_stage_count)(int world);
    int MKB(get_story_mode_stage_id)(int world_idx, int world_stage_idx);
    int MKB(get_story_mode_stage_difficulty)(int world, int stage);
    uint MKB(is_storymode_stage_beaten)(int world, int world_stage);
    MKB(BOOL32) MKB(g_is_storymode_stage_unlocked)(int world, int world_stage);
    void MKB(unlock_storymode_stage)(int world, int world_stage);
    void MKB(init_sm_state)(int world, int param_2);
    void MKB(clear_unlocked_storymode_stages)(void);
    void MKB(g_save_storymode_unlock_entries)(void);
    void MKB(g_load_storymode_unlock_entries)(void);
    int MKB(get_storymode_stage_time_limit)(int world, int world_stage);
    void MKB(event_item_init)(void);
    void MKB(event_item_tick)(void);
    void MKB(event_item_dest)(void);
    void MKB(draw_items)(void);
    int MKB(spawn_item)(struct MKB(Item) * item);
    void MKB(draw_item_shadows)(void);
    void MKB(init_coli_header_items)(struct MKB(StagedefColiHeader) * coli_header, int coli_header_count);
    void MKB(item_default_init)(void);
    void MKB(item_default_tick)(void);
    void MKB(item_default_disp)(void);
    void MKB(item_default_coli)(void);
    void MKB(item_default_dest)(void);
    void MKB(item_default_replay_init)(void);
    void MKB(item_debug_default)(void);
    void MKB(item_coin_init)(struct MKB(Item) * item);
    void MKB(item_coin_tick)(struct MKB(Item) * item);
    void MKB(item_coin_disp)(struct MKB(Item) * item);
    void MKB(item_coin_coli)(struct MKB(Item) * item, struct MKB(PhysicsBall) * phys_ball);
    void MKB(item_coin_dest)(void);
    void MKB(item_coin_replay_init)(struct MKB(Item) * item);
    void MKB(item_debug_coin)(struct MKB(Item) * item);
    MKB(GmaModel) * MKB(g_something_with_coins)(undefined4 * param_1);
    void MKB(event_stobj_collision_init)(void);
    void MKB(event_stobj_collision_tick)(void);
    void MKB(event_stobj_collision_dest)(void);
    void MKB(event_stobj_collision_child_tick)(void);
    uint MKB(g_some_item_collision_check)(double ball_scale, double item_scale, struct MKB(Vec) * ball_prev_pos, struct MKB(Vec) * ball_pos, struct MKB(Vec) * item_pos_copy, struct MKB(Vec) * item_pos);
    void MKB(g_something_with_item_coli)(double param_1, double param_2, struct MKB(Vec) * param_3, struct MKB(Vec) * param_4, struct MKB(Vec) * param_5);
    void MKB(event_stobj_init)(void);
    void MKB(event_stobj_tick)(void);
    void MKB(event_stobj_dest)(void);
    void MKB(g_draw_stobjs)(void);
    void MKB(g_stobj_sound_something)(void);
    int MKB(spawn_stobj)(struct MKB(Stobj) * stobj);
    void MKB(g_something_with_bumper_LODs)(void);
    void MKB(load_bumper_stobjs)(struct MKB(StagedefColiHeader) * coli_header_list, int coli_header_count);
    void MKB(load_jamabar_stobjs)(struct MKB(StagedefColiHeader) * coli_header_list, int coli_header_count);
    void MKB(load_button_stobjs)(struct MKB(StagedefColiHeader) * coli_header_list, int coli_header_count);
    void MKB(stobj_bumper_init)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_bumper_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_cb_f)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_bgspecial_init)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_bgspecial_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_bgspecial_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_bgspecial_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_bumper_bgspecial_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_bumper_bgspecial_cb_f)(struct MKB(Stobj) * stobj);
    void MKB(stobj_jamabar_init)(struct MKB(Stobj) * stobj);
    void MKB(stobj_jamabar_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_jamabar_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_jamabar_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_jamabar_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_jamabar_cb_f)(struct MKB(Stobj) * stobj);
    void MKB(stobj_default_init)(void);
    void MKB(stobj_default_tick)(void);
    void MKB(stobj_default_disp)(void);
    void MKB(stobj_default_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_default_dest)(void);
    void MKB(stobj_default_cb_f)(void);
    void MKB(stobj_button_init)(struct MKB(Stobj) * stobj);
    void MKB(stobj_button_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_button_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_button_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_button_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_button_cb_f)(struct MKB(Stobj) * stobj);
    void MKB(load_goaltape_and_goalbag_stobjs)(struct MKB(StagedefColiHeader) * coli_header_list, int coli_header_count);
    void MKB(stobj_goaltape_init)(struct MKB(Stobj) * stobj);
    double MKB(stobj_goaltape_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goaltape_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goaltape_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_goaltape_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goaltape_cb_f)(void);
    double MKB(stobj_goalbag_init)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goalbag_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goalbag_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goalbag_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_goalbag_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goalbag_cb_f)(struct MKB(Stobj) * stobj);
    double MKB(stobj_goalbag_exmaster_init)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goalbag_exmaster_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goalbag_exmaster_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goalbag_exmaster_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_goalbag_exmaster_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_goalbag_exmaster_cb_f)(struct MKB(Stobj) * stobj);
    void MKB(g_something_with_goals_and_physicsball)(int goal_idx, struct MKB(PhysicsBall) * physicsball);
    float MKB(g_get_sphere_camera_zdist_clamped)(float radius, struct MKB(Vec) * origin);
    void MKB(load_returngate_stobjs)(struct MKB(StagedefColiHeader) * coli_header_list, int coli_header_count);
    void MKB(stobj_returngate_init)(struct MKB(Stobj) * stobj);
    void MKB(stobj_returngate_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_returngate_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_returngate_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_returngate_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_returngate_cb_f)(struct MKB(Stobj) * stobj);
    void MKB(event_sprite_init)(void);
    void MKB(event_sprite_tick)(void);
    void MKB(event_sprite_dest)(void);
    void MKB(g_smth_with_drawing_all_sprites)(int param_1);
    void MKB(draw_sprite)(struct MKB(Sprite) * sprite);
    void MKB(load_bmp_by_id)(int param_1);
    void MKB(call_free_bmp_by_id)(int param_1);
    void MKB(g_something_with_iteratively_freeing_memory)(void);
    MKB(Sprite) * MKB(create_sprite)(void);
    MKB(Sprite) * MKB(create_linked_sprite)(struct MKB(Sprite) * sprite);
    void MKB(destroy_sprite_with_unique_id)(MKB(SpriteUniqueID)  unique_id);
    void MKB(dest_all_sprites)(void);
    MKB(Sprite) * MKB(get_sprite_with_unique_id)(MKB(SpriteUniqueID)  unique_id);
    void MKB(calc_sprite_bounds)(struct MKB(Sprite) * sprite, MKB(s32) * left, MKB(s32) * top, MKB(s32) * right, MKB(s32) * bottom);
    void MKB(g_get_font_char_width)(char * character, MKB(Font32)  font, struct MKB(FontDefinition) * g_font_struct);
    double MKB(g_get_font_char_width_scaling)(char * character, MKB(Font32)  font);
    void MKB(textdraw_reset)(void);
    void MKB(textdraw_set_font)(MKB(Font32)  font_type);
    void MKB(textdraw_set_mul_color)(uint param_1);
    void MKB(textdraw_set_add_color)(undefined4 param_1);
    void MKB(textdraw_set_rot_z)(undefined2 param_1);
    void MKB(textdraw_set_depth)(float param_1);
    void MKB(textdraw_set_scale)(float width, float height);
    void MKB(textdraw_set_alpha)(double alpha);
    void MKB(textdraw_set_flags)(MKB(TextDrawFlags)  param_1);
    void MKB(textdraw_clear_flags)(uint param_1);
    void MKB(textdraw_set_alignment)(MKB(SpriteAlignment)  alignment);
    void MKB(textdraw_set_drop_shadow_with_params)(float alpha, MKB(s16) offset_x, MKB(s16) offset_y);
    void MKB(textdraw_set_drop_shadow)(void);
    void MKB(textdraw_clear_drop_shadow)(void);
    void MKB(textdraw_set_border)(void);
    void MKB(textdraw_clear_border)(void);
    void MKB(g_textdraw_set_unk6)(double param_1);
    void MKB(g_textdraw_set_unk7)(undefined4 param_1);
    void MKB(textdraw_set_font_style)(MKB(FontStyle)  style);
    void MKB(textdraw_set_spacing)(float x, float y);
    void MKB(g_textdraw_set_counter)(undefined2 param_1);
    void MKB(textdraw_set_pos_for_aram_font_test)(double x, double y);
    void MKB(textdraw_set_pos)(float x, float y);
    void MKB(textdraw_put_char)(char ch);
    void MKB(textdraw_print)(char * string);
    void MKB(textdraw_printf)(char * format, ...);
    void MKB(draw_text_sprite)(struct MKB(Sprite) * sprite);
    void MKB(draw_bmp_sprite)(struct MKB(Sprite) * sprite);
    void MKB(draw_texture_sprite)(struct MKB(Sprite) * sprite);
    void MKB(textdraw_print_and_fit_to_width)(double width, char * string);
    void MKB(textdraw_printf_and_fit_to_width)(double g_width, char * format, ...);
    int MKB(g_get_font_def_aram_flag)(int param_1);
    void MKB(g_get_string_sprite_width_2)(char * param_1);
    void MKB(g_call_get_string_sprite_width_3_discard_result)(char * param_1);
    MKB(f32) MKB(textdraw_get_pixel_width_of_string_as_f32)(char * string);
    float MKB(textdraw_get_pixel_width_of_string)(char * str);
    double MKB(textdraw_get_pixel_height_of_string)(char * param_1);
    void MKB(g_smth_with_fonts_chara_load_wrapper_discard_result)(char * param_1);
    int MKB(textdraw_get_line_count_of_string)(char * str);
    void MKB(g_smth_with_screen_fading)(void);
    void MKB(fade_screen_to_color)(uint flags, MKB(u32) color, uint frames);
    undefined4 MKB(draw_sprite_draw_request)(struct MKB(SpriteDrawRequest) * request);
    void MKB(set_ui_widescreen_scale_mtx)(uint pivot_x);
    void MKB(reset_ui_widescreen_scale_mtx)(void);
    void MKB(g_something_loading_fonts)(void);
    void MKB(g_load_specific_font)(MKB(Font32)  font);
    void MKB(g_draw_sprite_draw_request_unbuffered)(struct MKB(SpriteDrawRequest) * request);
    uint MKB(g_parse_avtext_non_alphanumeric)(MKB(u32) next_two_chars_as_uint);
    undefined4 MKB(g_parse_avtext_other_codes)(char * string, ushort * next_two_chars);
    undefined4 MKB(parse_avtext_color_codes)(char * string, struct MKB(SpriteDrawRequest) * sprite_draw_req);
    uint MKB(g_some_avtext_array_lookup)(ushort next_two_chars, float some_float, short * float_as_short_ptr);
    double MKB(textdraw_print_internal_func_2)(double param_1, double param_2, int param_3, short param_4);
    double MKB(textdraw_print_internal_func_3)(int param_1, int param_2, short param_3, int param_4);
    int MKB(g_get_tex_id)(undefined4 param_1, ushort param_2, ushort * param_3, int param_4);
    void MKB(textdraw_print_internal_func_1)(byte * string);
    float MKB(textdraw_chara_load)(char * string, MKB(BOOL32) stop_on_newline, MKB(TextdrawCharaLoadReturnParameter)  return_parameter);
    double MKB(g_get_string_sprite_width_3)(char * param_1);
    MKB(f32) MKB(textdraw_get_pixel_width_of_string_as_f32_child)(char * string);
    float MKB(textdraw_get_pixel_width_of_string_child)(char * string);
    int MKB(g_smth_with_fonts_chara_load_wrapper)(char * param_1);
    int MKB(textdraw_get_line_count_of_string_child)(char * param_1);
    void MKB(g_display_playpoint_or_gift_message_child)(int param_1, int param_2, int * param_3);
    void MKB(g_smth_with_playpoint_or_gift_msg)(int param_1, char * param_2);
    void MKB(g_some_printf_function_4)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, int param_9, char * param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(create_pausemenu_sprite)(void);
    void MKB(g_something_with_pause_menu_and_game_over_sprite)(undefined4 param_1, struct MKB(Sprite) * sprite);
    void MKB(sprite_pausemenu_disp)(struct MKB(Sprite) * sprite);
    void MKB(create_titlescreen_press_start_sprites)(int param_1);
    void MKB(sprite_titlescreen_press_start_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_sprite_copyright_text_disp)(struct MKB(Sprite) * param_1);
    void MKB(g_create_smd_adv_logo_sprites)(void);
    void MKB(sprite_logo_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_copyright_text_sprites)(int param_1);
    void MKB(sprite_copyright_text_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_title_sprite)(MKB(s32) param_1);
    void MKB(sprite_title_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_title_disp)(struct MKB(Sprite) * sprite);
    void MKB(g_create_some_controls_description_sprite)(void);
    void MKB(create_stage_loadin_text_sprites)(void);
    void MKB(sprite_loadin_stage_name_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_loadin_stage_name_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_player_num_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_player_num_and_ready_sprites)(MKB(s32) param_1);
    void MKB(sprite_ready_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_bonus_stage_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_final_stage_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_final_stage_disp)(struct MKB(Sprite) * sprite);
    void MKB(create_go_sprite)(MKB(s32) param_1);
    void MKB(sprite_go_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_go_disp)(struct MKB(Sprite) * sprite);
    void MKB(create_goal_and_jump_to_stage_sprites)(MKB(s32) g_num_frames);
    void MKB(sprite_goal_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_jump_to_stage_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_jump_to_stage_duplicate_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_fallout_or_bonus_finish_sprite)(MKB(s32) param_1);
    void MKB(sprite_fallout_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_time_over_sprite)(MKB(s32) param_1);
    void MKB(sprite_time_over_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_bonus_finish_or_perfect_sprite)(void);
    void MKB(create_bonus_finish_sprite)(void);
    void MKB(sprite_bonus_finish_or_perfect_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_continue_sprites)(void);
    void MKB(sprite_continue_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_continues_remaining_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_continue_yes_or_no_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_create_some_game_over_and_player_num_sprites)(MKB(s32) param_1);
    void MKB(create_game_over_letter_sprite)(MKB(s32) param_1, uint x, uint y, MKB(s32) param_4, char * letter);
    void MKB(sprite_game_over_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_1up_sprite)(MKB(s32) param_1);
    void MKB(sprite_1up_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(empty_function)(void);
    void MKB(create_replay_sprite)(short param_1);
    void MKB(sprite_replay_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_go_to_extra_or_master_stages_sprites)(void);
    void MKB(g_sprite_extra_all_mask_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_sprite_go_to_the_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_sprite_go_to_the_disp)(struct MKB(Sprite) * sprite);
    void MKB(create_postgoal_score_sprites)(int param_1, int param_2, uint param_3);
    void MKB(sprite_clear_score_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_warp_bonus_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_time_bonus_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_stage_score_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_goal_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_make_some_continue_screen_sprites)(void);
    void MKB(create_save_sprite)(void);
    void MKB(sprite_save_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_save_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_monkey_counter_icon_disp)(struct MKB(Sprite) * sprite);
    undefined4 MKB(g_smth_with_get_active_monkey_icon)(int param_1, int param_2, int param_3);
    void MKB(create_how_to_sprite)(void);
    void MKB(sprite_how_to_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_how_to_sprite_draw_controller_tooltips)(int param_1, int param_2, struct MKB(SpriteDrawRequest) * param_3);
    void MKB(g_how_to_sprite_draw_rules_page)(char param_1, struct MKB(Sprite) * sprite, struct MKB(SpriteDrawRequest) * req, char param_4, char param_5, char param_6, char param_7);
    void MKB(sprite_how_to_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_how_to_dest)(void);
    void MKB(g_how_to_sprite_mtx_setup_stuff)(void);
    void MKB(g_how_to_sprite_mtx_cleanup)(void);
    void MKB(create_hud_sprites)(void);
    void MKB(create_score_sprites)(float pos_x, float pos_y);
    void MKB(create_timer_sprites)(float param_1, float param_2);
    void MKB(create_speed_sprites)(float param_1, float param_2);
    void MKB(create_hud_stage_name_sprites)(float param_1, float param_2);
    void MKB(create_banana_counter_sprites)(double param_1, double param_2);
    void MKB(create_monkey_counter_sprites)(double x, double y);
    void MKB(create_final_stage_sprite)(void);
    void MKB(create_debug_course_display_sprites)(void);
    void MKB(sprite_score_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_score_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_timer_ball_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_speed_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_current_stage_display_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_current_stage_display_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_hud_stage_name_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_banana_icon_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_banana_icon_shadow_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_banana_count_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_banana_disp_efc_req)(char bananas_collected);
    void MKB(sprite_monkey_counter_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_hud_player_num_disp)(struct MKB(Sprite) * sprite);
    void MKB(event_minimap_init)(void);
    void MKB(event_minimap_tick)(void);
    void MKB(event_minimap_dest)(void);
    void MKB(set_minimap_mode)(MKB(MinimapMode)  minimap_mode);
    float MKB(g_lava_bumper_func1)(void);
    void MKB(toggle_minimap_zoom)(void);
    void MKB(g_draw_minimap)(void);
    void MKB(g_smth_with_rendefc_reflective_height)(double height);
    MKB(OrdTblNode) * MKB(ord_tbl_get_entry_for_pos)(struct MKB(Vec) * pos);
    void MKB(ord_tbl_draw_nodes)(void);
    void * MKB(ord_tbl_alloc_node)(MKB(u32) size);
    void MKB(g_replay_screen_stage_name)(void);
    void MKB(empty_function)(void);
    void MKB(set_global_ape_LOD_1)(void);
    void MKB(empty_function)(void);
    undefined4 MKB(return_0)(void);
    MKB(Ape) * MKB(load_ape_gameplay_id_only)(int monkey_id);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(set_global_LOD)(int lod);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_ape_anim_head)(struct MKB(Ape) * ape, struct MKB(Vec) * ape_facedir_point, int flags);
    void MKB(g_smth_with_ape_rotation)(struct MKB(Ape) * ape, struct MKB(Vec) * vec_in);
    void MKB(empty_function)(void);
    uint MKB(decompress_lz)(byte * in_compressed, byte * out_decompressed);
    void MKB(g_some_arq_callback)(MKB(u32) pointerToARQRequest);
    void MKB(load_disc_queue)(void);
    MKB(BOOL32) MKB(g_something_with_dvd)(MKB(s32) entry_num, undefined4 * param_2);
    MKB(BOOL32) MKB(file_open)(char * file_path, struct MKB(File) * file);
    MKB(BOOL32) MKB(file_close)(struct MKB(File) * file);
    void MKB(ARQPostRequest_on_finish)(void);
    MKB(u32) MKB(file_read)(struct MKB(File) * file, void * buffer, MKB(u32) length, int offset);
    int MKB(file_size)(struct MKB(File) * file);
    void MKB(g_some_dvd_callback)(MKB(s32) result, struct MKB(DVDFileInfo) * fileInfo);
    void MKB(g_something_with_dvd2)(uint param_1, int param_2);
    int MKB(add_one_wrap_if_over127)(int num);
    bool MKB(disc_queue_load)(int entrynum);
    MKB(BOOL32) MKB(queue_disc_read)(char * file_name);
    void MKB(get_load_group_status)(void);
    void MKB(disc_queue_status)(int identifier);
    int MKB(disc_queue_distance)(int identifier);
    int MKB(g_get_debug_level_select_loading_left_asterisks)(void);
    undefined4 MKB(g_swapDiscQueueGroup)(undefined4 newValue);
    void MKB(g_fill_some_memory_with_0x0_and_0xff)(void * ptr);
    void MKB(g_some_shadow_draw_func)(struct MKB(ShadowReceive) * shadow_receive);
    undefined4 MKB(g_check_some_condition)(ushort param_1);
    void MKB(debug_draw_shadow_textures)(void);
    uint MKB(md_mini_func)(void);
    void MKB(g_set_minigame_specific_funcs)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
    void MKB(g_smd_mini_generic_init)(void);
    void MKB(g_smd_mini_generic_tick)(void);
    void MKB(g_mini_draw_func_handler)(void);
    void MKB(smd_mini_select_init)(void);
    void MKB(smd_mini_select_tick)(void);
    void MKB(smd_mini_ending_init)(void);
    void MKB(smd_mini_ending_tick)(void);
    void MKB(mini_ending_draw_func)(void);
    void MKB(smd_mini_s_roll_init)(void);
    void MKB(smd_mini_s_roll_tick)(void);
    void MKB(mini_nameentry_draw_func)(void);
    void MKB(init_shadow_reqs)(void);
    MKB(BOOL32) MKB(submit_shadow_req)(struct MKB(ShadowReq) * shadow);
    void MKB(draw_shadow_reqs)(void);
    void MKB(event_mouse_init)(void);
    void MKB(event_mouse_tick)(void);
    void MKB(event_mouse_dest)(void);
    void MKB(g_smth_with_drawing_text_sprites)(void);
    void MKB(event_rend_efc_init)(void);
    void MKB(event_rend_efc_tick)(void);
    void MKB(event_rend_efc_dest)(void);
    void MKB(g_something_with_rend_efc)(uint param_1);
    void MKB(g_init_rendefc_for_stage)(void);
    void MKB(g_smth_with_loading_reflective_stgobjs)(undefined1 * param_1);
    void MKB(g_smth_calling_reflective_obj_draw_hdlr)(int g_some_flag, int * param_2);
    void MKB(g_reflective_object_draw_handler)(int g_some_flag, int * g_some_ptr);
    void MKB(g_reflective_object_draw_handler_2)(undefined4 param_1, int param_2);
    void MKB(g_some_rendefc_func_1)(undefined1 * param_1);
    void MKB(g_smth_with_pil2_ref)(undefined1 * param_1);
    void MKB(g_something_with_stage_heap_and_target_theme)(int param_1);
    void MKB(g_some_rendefc_func_2)(int param_1, int param_2);
    void MKB(g_smth_with_reflective_models)(undefined4 param_1, int param_2);
    void MKB(g_smth_with_wormhole_surfaces)(undefined1 * param_1);
    void MKB(fog_main)(void);
    void MKB(g_set_something3)(char param_1);
    void MKB(smd_mini_commend_init)(void);
    void MKB(smd_mini_commend_tick)(void);
    void MKB(g_mini_commend_draw_func)(void);
    void MKB(g_something_with_rotation_not_aiai)(struct MKB(Ape) * ape);
    void MKB(g_something_with_drawing_apes)(void);
    uint MKB(g_check_flag_related_to_pausing)(void);
    void MKB(event_commend_init)(void);
    void MKB(event_commend_tick)(double param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8, undefined4 param_9);
    void MKB(event_commend_dest)(void);
    void MKB(load_commend_stage)(void);
    undefined4 MKB(return_1)(void);
    void MKB(empty_function)(void);
    void MKB(g_something_with_card13)(void);
    void MKB(g_some_replay_func4)(byte param_1, byte param_2, byte * param_3);
    void MKB(g_something_with_card3)(void);
    void MKB(print_card_submode_error)(byte * param_1);
    void MKB(g_memcard_func_1)(struct MKB(MemCardInfo) * info);
    void MKB(empty_function)(void);
    void MKB(g_memcard_func_2)(struct MKB(MemCardInfo) * info);
    void MKB(empty_function)(void);
    void MKB(mount_memory_card)(byte * param_1);
    void MKB(g_memcard_func_4)(struct MKB(MemCardInfo) * info);
    void MKB(g_something_free_card_blocks)(struct MKB(MemCardFile) * param_1);
    void MKB(g_open_card_file)(struct MKB(MemCardFile) * file);
    void MKB(something_that_calls_CARDCreateAsync)(byte * param_1);
    void MKB(g_calls_CARDWriteAsync)(byte * param_1);
    void MKB(g_related_to_memcard_rw)(byte * param_1);
    void MKB(g_something_with_card12)(byte * param_1);
    void MKB(g_something_with_card)(void);
    void MKB(g_something_with_card10)(char * param_1);
    void MKB(g_some_printf_function_5)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(g_something_with_card11)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(g_something_with_card9)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(g_something_with_card5)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(g_something_with_card4)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(g_something_with_card8)(void);
    void MKB(g_something_with_card6)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    byte MKB(g_check_some_memcard_field)(void);
    undefined4 MKB(g_get_result_code)(void);
    undefined1 MKB(g_get_some_memcard_var)(void);
    void MKB(g_sprintf_memcard_error)(int param_1, char * param_2);
    undefined1 MKB(g_get_last_used_memcard_slot)(void);
    void MKB(g_save_game_data)(void);
    void MKB(g_some_loading_function)(void);
    void MKB(smd_game_over_save_child)(void);
    void MKB(g_something_with_card2)(int card_chan, int param_2);
    void * MKB(g_some_replay_func)(undefined4 * param_1, uint * param_2);
    undefined4 MKB(g_some_replay_func2)(byte * param_1);
    char * MKB(g_some_replay_func3)(byte * param_1, char * param_2);
    uint MKB(g_something_with_fonts)(void);
    void MKB(g_some_printf_function_6)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, undefined4 param_10, undefined4 param_11, char * param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(smd_mini_ranking_init)(void);
    void MKB(smd_mini_ranking_tick)(void);
    void MKB(g_load_preview_texture)(struct MKB(SpriteTex) * sprite_tex, char * file_path, undefined4 param_3, MKB(u16) width, MKB(u16) height, MKB(GXTexFmt)  format);
    void MKB(g_some_dvd_callback2)(MKB(s32) result, struct MKB(DVDFileInfo) * file_info);
    void MKB(g_load_sprite_tex_from_dvd)(struct MKB(SpriteTex) * sprite_tex);
    void MKB(g_maybe_save_previews)(void);
    void MKB(g_something_with_smb1_ranking_main_game_defaults_wrapper)(void);
    void MKB(event_name_entry_init)(void);
    void MKB(event_name_entry_tick)(void);
    void MKB(event_name_entry_dest)(void);
    void MKB(g_something_with_name_entry)(void);
    void MKB(g_something_with_smb1_ranking_main_game_defaults)(void);
    void MKB(empty_function)(void);
    void MKB(effect_nameent_code_init)(int param_1);
    void MKB(effect_nameent_code_tick)(short * param_1);
    void MKB(effect_nameent_code_disp)(int param_1);
    void MKB(effect_nameent_code_dest)(struct MKB(Effect) * effect);
    void MKB(effect_get_nameent_code_init)(int param_1);
    void MKB(effect_get_nameent_code_tick)(short * param_1);
    void MKB(effect_get_nameent_code_disp)(int param_1);
    void MKB(effect_get_nameent_code_dest)(struct MKB(Effect) * effect);
    void MKB(stobj_nameent_btn_init)(struct MKB(Stobj) * stobj);
    void MKB(stobj_nameent_btn_tick)(struct MKB(Stobj) * stobj);
    void MKB(stobj_nameent_btn_disp)(struct MKB(Stobj) * stobj);
    void MKB(stobj_nameent_btn_coli)(struct MKB(Stobj) * stobj, struct MKB(PhysicsBall) * physicsball);
    void MKB(stobj_nameent_btn_dest)(struct MKB(Stobj) * stobj);
    void MKB(stobj_nameent_btn_cb_f)(struct MKB(Stobj) * stobj);
    void MKB(event_vibration_init)(void);
    void MKB(event_vibration_tick)(void);
    void MKB(event_vibration_dest)(void);
    void MKB(g_rumble_controller)(int controller_idx, int param_2, int param_3);
    void MKB(g_alloc_memory_for_ending)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_set_effect_ending_ballfrag)(void);
    void MKB(g_clear_effect_ending_ballfrag)(void);
    void MKB(effect_ending_ballfrag_init)(int param_1);
    void MKB(effect_ending_ballfrag_tick)(int param_1);
    void MKB(effect_ending_ballfrag_disp)(int param_1);
    void MKB(effect_ending_ballfrag_dest)(struct MKB(Effect) * effect);
    void MKB(g_init_something_with_heaps)(void);
    void MKB(g_free_some_memory2)(void);
    void MKB(avdisp_draw_model_unculled_sort_auto)(struct MKB(GmaModel) * model);
    void MKB(avdisp_draw_model_culled_sort_auto)(struct MKB(GmaModel) * model);
    void MKB(avdisp_draw_model_unculled_sort_never)(struct MKB(GmaModel) * gma_model);
    void MKB(avdisp_draw_model_culled_sort_never)(struct MKB(GmaModel) * model);
    void MKB(avdisp_draw_model_unculled_sort_always)(struct MKB(GmaModel) * model);
    void MKB(avdisp_draw_model_culled_sort_always)(struct MKB(GmaModel) * gma_model);
    void MKB(g_some_ord_node_func1)(int param_1);
    void MKB(g_some_ord_node_func2)(int param_1);
    void MKB(avdisp_set_scale_factor)(float scale);
    void MKB(call_g_avdisp_set_ambient)(double param_1, double param_2, double param_3);
    void MKB(avdisp_set_alpha)(float param_1);
    void MKB(avdisp_set_z_mode)(MKB(GXBool) compare_enable, MKB(GXCompare)  func, MKB(GXBool) update_enable);
    undefined4 MKB(g_something_with_texture_scroll_2)(int param_1);
    void MKB(set_post_mult_color)(double param_1, double param_2, double param_3, double param_4);
    void MKB(g_stores_doubles2)(double param_1, double param_2, double param_3, double param_4);
    void MKB(avdisp_set_fog_params)(double param_1, double param_2, undefined1 param_3);
    void MKB(avdisp_set_fog_color)(MKB(u8) r, MKB(u8) g, MKB(u8) b);
    void MKB(g_yet_another_unk_draw_func)(undefined4 param_1);
    void MKB(g_avdisp_reset_alpha_and_bound_sphere_scale)(void);
    MKB(DVDFileInfo) * MKB(g_get_some_dvd_file_info2)(void);
    void MKB(empty_function)(void);
    void * MKB(alloc_from_current_heap)(MKB(u32) size);
    void MKB(free_from_current_heap)(void * ptr);
    undefined8 MKB(teleport_through_wormhole)(int ball_idx, int wormhole_idx);
    MKB(f32) * MKB(g_compute_wormhole_tf)(int wormhole_idx, MKB(f32) * param_2);
    void MKB(g_mtx_mult_right)(MKB(Mtx) * ret, MKB(Mtx) * param_2);
    void MKB(apply_wormhole_tf_to_mtx)(MKB(Mtx) * src_tf, MKB(Mtx) * wormhole_tf);
    void MKB(apply_wormhole_tf_to_vec)(struct MKB(Vec) * vec, MKB(Mtx) * wormhole_tf);
    void MKB(g_apply_wormhole_pos_to_camera)(struct MKB(Vec) * camera_pos, struct MKB(Vec) * camera_vel, MKB(Mtx) * mtx);
    void MKB(apply_wormhole_tf_to_quat)(struct MKB(Quat) * quat, MKB(Mtx) * wormhole_tf);
    void MKB(empty_function)(void);
    void MKB(init_component_state)(struct MKB(SkeletonState) * component_state, struct MKB(BoneState) * bone_states, struct MKB(SkeletonDef) * skl);
    void MKB(g_init_some_handl_component_stuff)(struct MKB(SkeletonState) * component_state, undefined4 * param_2);
    void MKB(g_init_mta)(struct MKB(SkeletonState) * skeleton_state, struct MKB(ApeArchive) * mta);
    void MKB(g_some_ape_anim_init_wrapper)(void * param_1, char * param_2);
    void MKB(g_init_skeleton_animation)(struct MKB(SkeletonState) * skeleton_state, void * param_2, short g_anim_id);
    void MKB(g_unk_ape_anim1)(undefined4 param_1, MKB(undefined) param_2, uint param_3, uint param_4, uint param_5);
    void MKB(g_init_bone_state_subA)(struct MKB(GSkeletonStateSubA) * param_1);
    void MKB(g_unk_ape_anim2)(double g_speed, struct MKB(Ape) * ape);
    void MKB(g_set_more_ape_state)(undefined2 * param_1, undefined2 * param_2);
    void MKB(g_smth_with_quat_slerp)(ushort * param_1);
    void MKB(ape_archive_offsets_to_pointers)(struct MKB(ApeArchive) * g_ska);
    uint MKB(g_table_index)(MKB(undefined) param1, char * str);
    void MKB(g_load_ape_mtas)(MKB(ApeCharacter)  chara, MKB(undefined) scene_id);
    void MKB(g_init_mtas)(struct MKB(Ape) * ape, MKB(ApeCharacter)  chara, MKB(undefined) scene_id);
    void MKB(g_something_freeing_chara_heap_3)(int * param_1);
    void MKB(g_load_ape_mals)(struct MKB(Ape) * ape, MKB(ApeCharacter)  chara, MKB(undefined) scene_id);
    void MKB(event_ape_init)(void);
    void MKB(event_ape_tick)(void);
    void MKB(event_ape_dest)(void);
    MKB(Ape) * MKB(load_ape)(MKB(ApeCharacter)  chara, MKB(ApeLOD)  ape_lod, MKB(undefined) scene_id, MKB(undefined) deform_type);
    MKB(Ape) * MKB(g_load_ape_variant0)(MKB(ApeCharacter)  chara, MKB(ApeLOD)  lod, MKB(undefined) scene_id);
    MKB(Ape) * MKB(load_ape_gameplay)(MKB(ApeCharacter)  chara, undefined4 lod, int g_ape_variant);
    MKB(Ape) * MKB(g_load_ape_gameplay_variant0)(MKB(ApeCharacter)  chara, MKB(ApeLOD)  lod);
    void MKB(draw_ape_subroutine)(int param_1);
    void MKB(g_draw_ape)(double param_1, struct MKB(Ape) * ape);
    void MKB(draw_ape)(struct MKB(Ape) * ape);
    void MKB(g_something_with_freeing_chara_heap_ape)(struct MKB(Ape) * ape);
    void MKB(ape_default_anim)(struct MKB(Ape) * ape);
    void MKB(g_ape_anim_body)(struct MKB(Ape) * ape, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(g_load_mal_files_from_disc)(int chara_index, int LOD, int s_mal, int game_index);
    void MKB(g_something_freeing_chara_heap_4)(int param_1, int param_2, int param_3);
    void MKB(g_something_freeing_chara_heap_2)(int param_1, int param_2);
    void MKB(assign_ape_assets)(struct MKB(Ape) * ape, MKB(ApeLOD)  lod);
    MKB(BoneState) * MKB(g_get_ape_anim_smth)(struct MKB(Ape) * ape, int index);
    double MKB(body_frame_add72)(struct MKB(Ape) * ape);
    char * MKB(get_ape_scene_name)(struct MKB(Ape) * ape);
    int MKB(get_ape_anim_flags)(struct MKB(Ape) * ape, undefined4 param_2, int param_3);
    void MKB(preload_ape)(MKB(ApeCharacter)  chara, MKB(ApeLOD)  lod, int s_mal, int scene_id);
    void MKB(g_queue_ape_mta_mal_load)(int scene_id, MKB(ApeCharacter)  chara);
    uint MKB(g_something_with_comparing_strings)(undefined4 param_1, char * param_2);
    void MKB(register_ape)(struct MKB(Ape) * ape);
    void MKB(unregister_ape)(struct MKB(Ape) * ape);
    MKB(Ape) * MKB(get_ape_by_idx)(int param_1);
    void MKB(dip_apenum_draw)(struct MKB(Ape) * ape);
    char * MKB(get_ape_chara_name)(struct MKB(Ape) * ape);
    char * MKB(get_ape_lod_string)(struct MKB(Ape) * ape);
    char * MKB(g_get_ape_skinning_or_stitching_name)(struct MKB(Ape) * ape);
    char * MKB(get_ape_face_expr_name)(struct MKB(Ape) * ape);
    int MKB(seek)(char * param_1);
    void MKB(assign_ape_models)(struct MKB(Ape) * ape);
    MKB(SomeApeState) * MKB(g_get_structure_from_ape_common_mal_struct)(struct MKB(Mal) * common_mal);
    void MKB(g_something_freeing_heap_3)(void * param_1);
    int MKB(g_get_ape_flag)(struct MKB(Ape) * ape, undefined4 g_something_with_game, int param_3);
    undefined4 MKB(g_alloc_some_ape_state_0x8)(struct MKB(SomeApeState) * g_some_ape_state, int chara_idx);
    void MKB(g_ape_anim_unk3)(struct MKB(Ape) * ape);
    void MKB(g_set_ape_dance_state1)(int param_1, int param_2, undefined2 param_3, int param_4, undefined4 param_5, undefined4 param_6, int param_7);
    void MKB(g_set_ape_dance_state2)(struct MKB(SomeApeState) * g_some_ape_state, uint param_2, uint param_3, int param_4, int param_5, undefined4 param_6, undefined4 param_7, int param_8);
    void MKB(g_ape_anim_unk4)(struct MKB(Ape) * ape, int param_2, short * param_3);
    int MKB(g_some_ape_anim_func2)(int param_1, int param_2, undefined4 param_3, undefined4 param_4, int param_5);
    void MKB(g_set_ape_stuff_in_chara_heap)(undefined2 * param_1, undefined2 * param_2);
    void MKB(g_something_with_GXPeekZ)(void);
    void MKB(empty_function)(void);
    void MKB(g_init_rankings_to_defaults_wrapper)(void);
    void MKB(g_init_rankings_to_defaults)(void);
    void MKB(g_smth_with_ending_course_2)(void);
    void MKB(g_NameEntry2_InitFirst)(void);
    void MKB(g_nameentry2_init)(void);
    void MKB(game_nameentry_draw_func)(void);
    void MKB(ape_assignment)(void);
    void MKB(sprite_rank_tick)(void);
    void MKB(sprite_rank_disp)(void);
    void MKB(g_something_with_name_entry_get_course)(int param_1);
    void MKB(g_some_ape_anim_func1)(struct MKB(Ape) * ape);
    void MKB(set_ape_anim)(struct MKB(Ape) * ape, MKB(undefined) animationType);
    void MKB(g_some_set_ape_anim)(struct MKB(Ape) * ape, MKB(undefined) chara_anim_type, undefined2 param_3);
    void MKB(run_anim_funcs)(struct MKB(Ape) * ape);
    void MKB(load_scene_files)(int scene_category);
    uint MKB(load_scene_data)(int scene_category);
    void MKB(g_parse_scene_data)(int * param_1);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_something_freeing_heap_4)(void);
    void MKB(g_something_with_cutscenes_func2)(void);
    undefined4 MKB(g_something_with_cutscenes_func)(MKB(u16) g_something_cutscene);
    void MKB(g_dump_scene_info)(void);
    void MKB(g_something_to_do_with_soft_fx)(undefined4 param_1, undefined4 param_2, int param_3);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    MKB(OSHeapHandle) * MKB(load_dialog_tbl)(int scene_id);
    undefined4 MKB(g_smth_with_dialog2)(int * param_1, int param_2);
    void MKB(load_scene_sound)(char param_1_00);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(smd_author_select_init)(void);
    void MKB(smd_author_select_tick)(void);
    void MKB(g_print_author_debug_data)(void);
    void MKB(smd_author_play_init)(void);
    void MKB(smd_author_play_tick)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(smd_author_play_story_init)(void);
    void MKB(smd_author_play_ending_init)(void);
    void MKB(smd_author_play_from_sel)(void);
    void MKB(smd_author_play_ret_sel)(void);
    uint MKB(md_author_func)(void);
    void MKB(author_draw_func)(void);
    void MKB(g_assign_new_main_and_sub_mode_for_play_tick)(MKB(MainMode)  param_1, MKB(SubMode)  param_2);
    void MKB(g_set_some_author_tick_func)(undefined4 param_1);
    undefined4 MKB(g_get_some_author_related_data)(void);
    undefined4 MKB(g_get_author_scene)(void);
    int MKB(g_get_author_scene_max)(void);
    undefined2 MKB(g_get_author_frame)(void);
    undefined2 MKB(g_get_author_frame_max)(void);
    void MKB(g_some_author_cutscene_related_func)(void);
    void MKB(g_load_stageselect_after_cutscene)(void);
    void MKB(g_preload_next_stage_files)(int param_1, int param_2, int param_3);
    void MKB(clear_unlock_info)(void);
    void MKB(unlock_master)(void);
    byte MKB(is_master_unlocked)(void);
    void MKB(empty_function)(void);
    void MKB(g_set_unlockables_status)(void);
    void MKB(g_handle_goal)(void);
    void MKB(empty_function)(void);
    void MKB(compare_play_points_with_99999_after_exit_game)(void);
    dword MKB(get_play_point_count)(void);
    void MKB(g_display_playpoint_or_gift_message)(double g_x_pos, double g_y_pos, undefined1 param_3);
    void MKB(g_playpoint_or_gift_msg_disp)(undefined8 param_1_00, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, int * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    bool MKB(is_able_to_unlock_party_game)(void);
    void MKB(unlock_party_game)(int party_game);
    bool MKB(is_able_to_unlock_gift)(void);
    void MKB(g_something_with_assinging_unlockables)(void);
    void MKB(unlock_movie)(int movie);
    void MKB(unlock_staff_credits_game)(void);
    uint MKB(are_all_gifts_unlocked)(void);
    uint MKB(g_check_if_partygame_unlocked)(int param_1);
    int MKB(get_num_of_unlocked_party_games)(void);
    bool MKB(are_all_party_games_unlocked)(void);
    void MKB(set_unlocked_monkeys_to_three)(void);
    byte MKB(get_num_unlocked_monkeys)(void);
    bool MKB(are_all_extra_monkeys_unlocked)(void);
    void MKB(g_set_movie_as_unlocked)(int param_1);
    uint MKB(g_is_movie_unlocked)(int param_1);
    bool MKB(are_all_movies_unlocked)(void);
    byte MKB(is_staff_credits_game_unlocked)(void);
    undefined4 MKB(return_5)(void);
    undefined4 MKB(return_0)(void);
    void MKB(main_game_rel_prolog)(void);
    void MKB(main_game_rel_epilog)(void);
    void MKB(main_game_rel_unlinked)(void);
    uint MKB(md_game_func)(void);
    void MKB(smd_game_first_init)(void);
    void MKB(smd_game_restart_init)(void);
    void MKB(smd_game_ready_init)(void);
    void MKB(smd_game_ready_tick)(void);
    void MKB(smd_game_play_init)(void);
    void MKB(smd_game_play_tick)(void);
    void MKB(smd_game_goal_init)(void);
    void MKB(smd_game_goal_tick)(void);
    void MKB(smd_game_goal_replay_init)(void);
    void MKB(smd_game_goal_replay_tick)(void);
    void MKB(smd_game_continue_init)(void);
    void MKB(smd_game_continue_tick)(void);
    void MKB(smd_game_timeover_init)(void);
    void MKB(smd_game_timeover_tick)(void);
    void MKB(smd_game_ringout_init)(void);
    void MKB(smd_game_ringout_tick)(void);
    void MKB(smd_game_bonus_clear_init)(void);
    void MKB(smd_game_bonus_clear_tick)(void);
    void MKB(smd_game_first_tick)(void);
    void MKB(smd_game_over_init)(void);
    void MKB(smd_game_over_point_init)(void);
    void MKB(smd_game_over_point_tick)(void);
    void MKB(smd_game_scenario_init)(void);
    void MKB(smd_game_scenario_tick)(void);
    void MKB(g_some_dest_function_handler)(void);
    void MKB(smd_game_scenario_return)(void);
    void MKB(smd_game_retry_init)(void);
    void MKB(smd_game_retry_tick)(void);
    void MKB(smd_game_over_save)(void);
    void MKB(smd_game_over_dest)(void);
    void MKB(smd_game_nameentry_ready_init)(void);
    void MKB(smd_game_nameentry_ready_tick)(void);
    void MKB(smd_game_nameentry_init)(void);
    void MKB(smd_game_nameentry_tick)(void);
    void MKB(smd_game_roll_init)(void);
    void MKB(smd_game_roll_tick)(undefined4 param_1, undefined4 param_2_00, short param_3);
    void MKB(smd_game_extra_init)(void);
    void MKB(smd_game_extra_tick)(void);
    void MKB(smd_game_result_init)(void);
    void MKB(smd_game_result_tick)(void);
    void MKB(smd_game_result_menu)(void);
    void MKB(smd_game_intr_sel_init)(void);
    void MKB(smd_game_intr_sel_tick)(void);
    void MKB(smd_game_sugg_save_init)(void);
    void MKB(smd_game_sugg_save_tick)(void);
    int MKB(get_next_stage_id)(void);
    uint MKB(g_decrement_active_ball_monkey_count)(void);
    void MKB(g_smth_with_ending_course)(void);
    void MKB(smd_game_force_exit_init)(void);
    void MKB(smd_game_force_exit_tick)(void);
    void MKB(smd_game_scenscnplay_return)(void);
    void MKB(smd_game_force_over_init)(void);
    void MKB(smd_game_force_over_tick)(void);
    void MKB(g_maybe_some_goal_func)(void);
    void MKB(g_preload_stage_or_results)(void);
    void MKB(g_challenge_mode_start)(struct MKB(Ball) * ball);
    void MKB(challenge_mode_physics)(struct MKB(Ball) * ball);
    void MKB(apply_velocity)(struct MKB(Ball) * ball);
    void MKB(g_smth_with_mystery_3)(struct MKB(Ball) * ball);
    void MKB(g_camera_func89)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(g_camera_func90)(struct MKB(Camera) * camera, struct MKB(Ball) * ball);
    void MKB(smd_game_scenario_init_child)(void);
    void MKB(smd_game_scenario_tick_child)(void);
    void MKB(g_some_storymode_dest_function)(void);
    void MKB(game_scenario_draw_func)(void);
    undefined4 MKB(get_storymode_score)(void);
    void MKB(set_storymode_score)(int value);
    undefined4 MKB(get_storymode_banana_count)(void);
    void MKB(set_storymode_bananas)(int banana_count);
    void MKB(g_preload_ape_model_for_stageselect)(void);
    void MKB(g_save_storymode_progress)(void * param_1);
    int MKB(get_current_storymode_stage_time_limit)(void);
    int MKB(get_storymode_stage_time_limit_wrapper)(int world, int world_stage);
    void MKB(g_some_scenario_init_func_1)(void);
    void MKB(g_some_storymode_mode_handler)(void);
    void MKB(g_get_storymode_playtime_frames)(void);
    void MKB(dmd_scen_select_init)(void);
    void MKB(dmd_scen_select_main)(void);
    void MKB(dmd_scen_1st_init)(void);
    void MKB(dmd_scen_return_init)(void);
    void MKB(dmd_scen_newgame_init)(void);
    void MKB(dmd_scen_newgame_main)(void);
    void MKB(dmd_scen_loadgame_init)(void);
    void MKB(dmd_scen_loadgame_main)(void);
    void MKB(dmd_scen_loadgame_restore)(void);
    void MKB(dmd_scen_sceneplay_init)(void);
    void MKB(dmd_scen_sceneplay_main)(void);
    void MKB(dmd_scen_scnplay_return)(void);
    void MKB(dmd_scen_scnplay_return_loading)(void);
    void MKB(dmd_scen_sel_world_init)(void);
    void MKB(dmd_scen_sel_world_next)(void);
    void MKB(dmd_scen_sel_floor_init)(void);
    void MKB(dmd_scen_sel_floor_main)(void);
    void MKB(dmd_scen_game_init)(void);
    void MKB(dmd_scen_game_main)(void);
    void MKB(dmd_scen_game_clear_init)(void);
    void MKB(dmd_scen_game_clear_main)(void);
    void MKB(dmd_scen_nameentry_init)(void);
    void MKB(dmd_scen_nameentry_main)(void);
    void MKB(dmd_scen_game_over_init)(void);
    void MKB(dmd_scen_game_over_main)(void);
    void MKB(dmd_scen_entry_init)(void);
    void MKB(dmd_scen_entry_main)(void);
    void MKB(g_some_storymode_mode_func)(void);
    MKB(StoryModeSaveFile) * MKB(get_current_storymode_save_file)(void);
    void MKB(g_init_scen_stage_name_buffer)(void);
    void MKB(g_free_scen_stage_name_buffer)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_create_storymode_select_sprites)(uint param_1);
    void MKB(g_related_to_loading_story_stageselect)(uint param_1);
    void MKB(sprite_story_stage_select_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_story_stage_select_disp)(struct MKB(Sprite) * sprite);
    void MKB(g_handle_story_clear_stage_balls)(short param_1);
    void MKB(sprite_clear_floors_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_clear_floors_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_decoration_bar_tick)(MKB(u8) * param_1, struct MKB(Sprite) * param_2);
    void MKB(sprite_decoration_bar_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_world_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_world_disp)(struct MKB(Sprite) * sprite);
    void MKB(g_animate_story_select_sprites)(undefined2 param_1);
    void MKB(g_sprite_world_info_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_sprite_world_info_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_scen_stagesel_stage_name_tick)(MKB(u8) * param_1, struct MKB(Sprite) * sprite);
    void MKB(sprite_scen_stagesel_stage_name_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_story_difficulty_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_story_difficulty_disp)(struct MKB(Sprite) * sprite);
    void MKB(g_draw_story_stage_preview_ball)(undefined2 param_1);
    void MKB(g_sprite_story_stage_info_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(g_sprite_story_stage_info_disp)(struct MKB(Sprite) * sprite);
    void MKB(g_preload_all_story_preview_images)(void);
    void MKB(g_draw_now_loading_text)(void);
    uint MKB(g_get_storymode_next_world)(void);
    void * MKB(g_check_if_game_over_or_after_credits_sequence)(int param_1, void * param_2);
    void MKB(empty_function)(void);
    void MKB(g_some_scenario_init_func_3)(void);
    void MKB(g_handle_storymode_stageselect_state)(void);
    void MKB(dmd_scen_sel_floor_init_child)(void);
    void MKB(g_set_some_storymode_stageids)(void);
    void MKB(g_load_new_storymode_stage)(int param_1);
    void MKB(g_some_scenario_init_func_2)(void);
    void MKB(empty_function)(void);
    void MKB(g_smth_with_storymode)(void);
    void MKB(sprite_story_file_nameentry_disp)(void);
    void MKB(create_story_file_nameentry_sprite)(void);
    void MKB(dmd_scen_entry_main_child)(void);
    void MKB(g_get_story_file_name)(char * param_1);
    void MKB(draw_storymode_save_file)(struct MKB(StoryModeSaveFile) * saveFile, int index);
    void MKB(g_related_to_loading_dataselect_menu)(void);
    void MKB(dmd_scen_loadgame_main_child)(void);
    int MKB(get_storymode_menu_state)(void);
    void MKB(staff_roll_init)(void);
    void MKB(g_smth_with_staff_roll_init_7)(undefined4 param_1, undefined4 param_2, short param_3);
    void MKB(game_roll_draw_func)(void);
    void MKB(smd_staff_roll_dest)(void);
    void MKB(g_smth_with_staff_roll_init_1)(void);
    void MKB(g_smth_with_staff_roll_init_2)(void);
    void MKB(g_staff_roll_name_align)(void);
    void MKB(g_staff_roll_banana_set)(undefined2 * param_1);
    void MKB(g_smth_with_staff_roll_init_4)(void);
    void MKB(g_smth_with_staff_roll_init_3)(short * param_1);
    void MKB(g_smth_with_staff_roll_init_6)(void);
    void MKB(staff_roll_results_draw_final_score)(void);
    void MKB(staff_roll_results_draw_monkey_rating)(void);
    void MKB(staff_roll_results_draw_banana_count)(void);
    void MKB(staff_roll_results_draw_big_banana_count)(void);
    void MKB(staff_roll_results_draw_item_count)(void);
    void MKB(staff_roll_results_draw_letter_count)(void);
    void MKB(g_smth_with_staff_roll_init_5)(void);
    void MKB(game_continue_draw_func)(void);
    void MKB(event_view_init)(void);
    void MKB(event_view_tick)(void);
    void MKB(event_view_dest)(void);
    void MKB(g_something_with_view_stage2)(void);
    void MKB(draw_reflective_objects_in_viewstage)(void);
    void MKB(g_call_advance_stage_animation)(void);
    void MKB(g_increment_bg_related_timer)(void);
    void MKB(view_stage_draw_bananas)(void);
    void MKB(g_smth_with_rendering_models_for_reflective_surfaces)(void);
    void MKB(g_draw_stobjs_in_viewstage)(void);
    void MKB(g_something_with_view_stage_camera)(struct MKB(Camera) * camera);
    void MKB(sel_ngc_prolog)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(sel_ngc_epilog)(void);
    void MKB(sel_ngc_unlinked_func)(void);
    uint MKB(g_b_button_handler_for_sel_ngc)(void);
    bool MKB(did_any_pad_press_input)(MKB(PadInputID)  input_id);
    void MKB(g_create_main_menu)(void);
    void MKB(g_initialize_sel_ngc)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(g_reinitialize_sel_ngc)(void);
    undefined4 MKB(g_called_when_fading_to_storymode)(void);
    void MKB(g_sel_ngc_item_selected)(int play_menu_selction_sound);
    void MKB(g_b_button_pressed_on_menu)(int param_1_00);
    MKB(BOOL32) MKB(g_is_screen_in_screen_stack)(byte g_screen_id);
    void MKB(menu_tick)(void);
    void MKB(g_something_with_menus)(int param_1);
    void MKB(menu_mode_select_tick)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(menu_party_game_select_tick)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(menu_option_tick)(void);
    void MKB(menu_return_to_title)(void);
    void MKB(menu_number_of_players_tick)(void);
    void MKB(menu_character_select_tick)(void);
    void MKB(menu_gameplay_settings_tick)(void);
    void MKB(menu_main_game_select_tick)(void);
    void MKB(menu_level_select_tick)(void);
    void MKB(g_get_some_challenge_mode_difficulty_info)(int param_1);
    void MKB(g_get_some_category_count)(int flag, int * g_out_1, int * g_out_2);
    void MKB(menu_stage_select_tick)(void);
    void MKB(menu_start_story_mode)(void);
    void MKB(menu_start_challenge_mode)(void);
    void MKB(menu_start_practice_mode)(void);
    void MKB(menu_race_mode_select_tick)(void);
    void MKB(menu_race_course_select_tick)(void);
    void MKB(menu_start_race)(void);
    void MKB(menu_fight_mode_select_tick)(void);
    void MKB(menu_fight_no_of_wins_tick)(void);
    void MKB(menu_fight_stage_select_tick)(void);
    void MKB(menu_start_fight)(void);
    void MKB(menu_start_target)(void);
    void MKB(menu_billiards_mode_select_tick)(void);
    void MKB(menu_billiards_rule_select_tick)(void);
    void MKB(menu_start_billiards)(void);
    void MKB(menu_bowling_mode_select_tick)(void);
    void MKB(menu_bowling_rule_select_tick)(void);
    void MKB(menu_bowling_level_select_tick)(void);
    void MKB(menu_start_bowling)(void);
    void MKB(menu_golf_mode_select_tick)(void);
    void MKB(menu_start_golf)(void);
    void MKB(menu_boat_mode_select_entries_tick)(void);
    void MKB(menu_boat_course_select_tick)(void);
    void MKB(menu_start_boat)(void);
    void MKB(menu_shot_stage_select_tick)(void);
    void MKB(menu_start_shot)(void);
    void MKB(menu_dogfight_mode_select_tick)(void);
    void MKB(menu_dogfight_stage_select_tick)(void);
    void MKB(menu_start_dogfight)(void);
    void MKB(menu_soccer_mode_select_tick)(void);
    void MKB(menu_start_soccer)(void);
    void MKB(menu_baseball_mode_select_tick)(void);
    void MKB(menu_baseball_stadium_select_tick)(void);
    void MKB(menu_start_baseball)(void);
    void MKB(menu_tennis_mode_select_tick)(void);
    void MKB(menu_tennis_pair_select_tick)(void);
    void MKB(menu_tennis_court_select_tick)(void);
    void MKB(menu_start_tennis)(void);
    void MKB(menu_option_replay_tick)(void);
    void MKB(menu_option_play_points_tick)(void);
    void MKB(menu_option_gift_tick)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, MKB(u32) param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(menu_option_ranking_tick)(void);
    void MKB(menu_option_game_data_tick)(void);
    void MKB(menu_option_controller_tick)(void);
    void MKB(menu_option_screen_tick)(void);
    char * MKB(g_handle_starting_monkeys_count)(int param_1, int param_2);
    void MKB(create_main_menu_sprites)(void);
    void MKB(g_display_game_settings_sprite)(void);
    void MKB(create_practice_mode_stage_select_sprite)(void);
    void MKB(sprite_title_str_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_title_str_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_info_str_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_info_str_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_start_mask_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_menu_basic_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_menu_basic_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_menu_basic_stack_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_menu_basic_title_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_menu_basic_title_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_game_settings_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_game_settings_disp)(struct MKB(Sprite) * param_1);
    void MKB(sprite_practice_stage_select_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_practice_stage_select_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_fight_stage_select_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_fight_stage_select_disp)(struct MKB(Sprite) * param_1);
    void MKB(sprite_button_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_button_disp)(struct MKB(Sprite) * param_1);
    void MKB(g_blink_selected_menu_item)(void);
    void MKB(g_something_with_menus5)(void);
    void MKB(g_set_some_option_menu_sprite_param)(void);
    void MKB(g_load_select_apes)(void);
    void MKB(g_load_some_apes)(void);
    void MKB(menu_draw_background)(void);
    void MKB(g_draw_menu_bg_overlay)(struct MKB(GXColor) * param_1, MKB(GXTexFmt)  param_2);
    void MKB(free_some_apes)(void);
    void MKB(g_draw_stage_0x9f_for_menu_bg)(void);
    void MKB(g_draw_stage_0x8a_for_menu_bg)(void);
    void MKB(g_draw_stage_for_menu_bg_2)(void);
    void MKB(sprite_practice_stage_preview_tick)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(sprite_practice_stage_preview_disp)(struct MKB(Sprite) * sprite);
    void MKB(sprite_practice_stage_preview_mask_disp)(MKB(u8) * status, struct MKB(Sprite) * sprite);
    void MKB(create_practice_mode_preview_sprites)(void);
    void MKB(create_fight_stage_preview_sprites)(void);
    void MKB(sprite_gamedata_disp)(int param_1);
    void MKB(create_gamedata_sprite)(void);
    void MKB(menu_option_game_data_tick_child)(void);
    void MKB(destroy_gamedata_sprite)(void);
    void MKB(g_draw_controller_sprites)(void);
    void MKB(g_draw_gift_menu)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, MKB(u32) param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(empty_function)(void);
    void MKB(sel_stage_unlinked_func)(void);
    void MKB(g_references_420_03)(void);
    void MKB(bowling_rel_prolog)(void);
    void MKB(bowling_rel_epilog)(void);
    void MKB(bowling_rel_unlinked)(void);
    void MKB(g_init_bowling)(void);
    void MKB(g_bowl_drawFunc)(struct MKB(Ape) * param_1, int param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(g_load_missing_apes)(void);
    void MKB(g_load_commend_apes)(void);
    void MKB(g_load_selthing)(void);
    MKB(BOOL32) MKB(ballid_matches_chara)(MKB(u32) chara_index);
    void MKB(mini_billiards_unlinked_func)(void);
    void MKB(g_references_420_04)(void);
    void MKB(sample_unlinked_func)(void);
    void MKB(test_mode_prolog)(void);
    void MKB(test_mode_epilog)(void);
    void MKB(test_mode_unlinked_func)(void);
    void MKB(smd_test_select_init)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(smd_test_select_main)(void);
    undefined2 * MKB(test_select_draw_func)(void);
    void MKB(smd_test_dip_init)(void);
    void MKB(smd_test_dip_main)(void);
    void MKB(smd_test_input_init)(void);
    void MKB(smd_test_input_main)(void);
    void MKB(smd_test_sound_init)(void);
    void MKB(smd_test_sound_main)(void);
    void MKB(test_sound_draw_func)(void);
    void MKB(smd_test_adx_init)(void);
    void MKB(smd_test_adx_main)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, undefined4 param_10, uint * param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(smd_test_blur_init)(void);
    void MKB(smd_test_blur_main)(void);
    void MKB(test_blur_draw_func)(void);
    void MKB(smd_test_shadow_init)(void);
    void MKB(smd_test_shadow_main)(void);
    void MKB(smd_test_focus_init)(void);
    void MKB(smd_test_focus_main)(void);
    void MKB(smd_test_indirect_init)(void);
    void MKB(smd_test_indirect_main)(void);
    void MKB(smd_test_nlobject_init)(void);
    void MKB(smd_test_nlobject_main)(void);
    void MKB(smd_test_bitmap_init)(void);
    void MKB(smd_test_bitmap_main)(void);
    void MKB(test_bitmap_draw_func)(void);
    void MKB(smd_test_font_init)(void);
    void MKB(smd_test_font_main)(void);
    void MKB(test_font_draw_func)(void);
    void MKB(smd_test_aram_font_init)(void);
    void MKB(smd_test_aram_font_main)(void);
    void MKB(test_aram_font_draw_func)(void);
    void MKB(smd_test_motion_init)(void);
    void MKB(smd_test_motion_main)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(smd_test_newmotion_init)(void);
    void MKB(smd_test_newmotion_main)(undefined4 param_1, undefined4 param_2, int param_3, int param_4, int param_5, undefined4 param_6, int param_7, int param_8);
    void MKB(smd_test_advreplay_init)(void);
    void MKB(smd_test_model_init)(void);
    void MKB(smd_test_model_main)(void);
    void MKB(test_model_draw_func)(void);
    void MKB(smd_test_newmodel_init)(void);
    void MKB(smd_test_newmodel_main)(void);
    void MKB(test_newmodel_draw_func)(void);
    void MKB(smd_test_pattern_init)(void);
    void MKB(smd_test_pattern_main)(void);
    void MKB(test_pattern_draw_func)(void);
    void MKB(smd_test_romfont_init)(void);
    void MKB(smd_test_romfont_main)(void);
    void MKB(smd_test_prerend_init)(void);
    void MKB(smd_test_prerend_main)(void);
    void MKB(test_prerend_draw_func)(void);
    void MKB(smd_test_preview_init)(void);
    void MKB(smd_test_preview_main)(void);
    void MKB(smd_test_replay_init)(void);
    void MKB(smd_test_replay_main)(void);
    void MKB(smd_test_scene_play_init)(void);
    void MKB(smd_test_scene_play_main)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(test_scene_play_draw_func)(void);
    void MKB(smd_test_dialog_init)(void);
    void MKB(smd_test_dialog_main)(undefined4 param_1, undefined4 param_2, undefined2 * param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void MKB(test_dialog_draw_func)(void);
    void MKB(smd_test_difficulty_init)(void);
    void MKB(smd_test_difficulty_main)(void);
    void MKB(test_difficulty_draw_func)(void);
    void MKB(smd_test_chk_repsize_init)(void);
    void MKB(smd_test_chk_repsize_main)(void);
    void MKB(test_draw_func_handler)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void MKB(smd_test_shadow_main_child)(void);
    void MKB(g_test_shadow_draw_func_1)(void);
    void MKB(g_test_shadow_draw_func_2)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(smd_test_focus_main_child)(void);
    void MKB(test_focus_draw_func)(void);
    void MKB(empty_function)(void);
    void MKB(smd_test_indirect_main_child)(void);
    void MKB(test_indirect_draw_func)(void);
    void MKB(empty_function)(void);
    void MKB(g_some_printf_function_7)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, char * param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    undefined8 MKB(g_test_motion_main_draw_func)(void);
    void MKB(test_adx_draw_func)(void);
    void MKB(test_newmotion_draw_func)(void);
    void MKB(g_something_freeing_something_from_main_heap_2)(void);
    void MKB(g_ape_anim_unk5)(undefined4 * param_1, char * param_2, int * param_3);
    void MKB(g_ape_anim_unk6)(int param_1);
    uint MKB(g_ape_anim_unk7)(int param_1, undefined4 param_2, int param_3);
    void MKB(g_ape_anim_unk8)(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, int param_5);
    void MKB(option_prolog)(void);
    void MKB(option_epilog)(void);
    void MKB(option_unlinked_func)(void);
    void MKB(option_draw_func)(void);
    void MKB(g_references_420_06)(int param_1, short param_2);
    void MKB(race2_rel_prolog)(void);
    void MKB(race2_rel_epilog)(void);
    void MKB(race2_rel_unresolved)(void);
    void MKB(race2_tick)(void);
    void MKB(g_load_race)(void);
    void MKB(g_draw_race_timer_ui)(void);
    void MKB(g_print_race_time_remaining)(undefined4 param_1, int param_2);
    void MKB(golf2_unlinked_func)(void);
    void MKB(g_golf_init)(void);
    void MKB(g_smth_with_golf_wind_velocity)(double wind_velocity);
    void MKB(empty_function)(void);
    void MKB(mini_fight2_unlinked_func)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_references_420_07)(double param_1, double param_2, int param_3);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(pilot2_unlinked_func)(void);
    void MKB(g_load_pilot)(void);
    void MKB(empty_function)(void);
    void MKB(g_some_printf_function_8)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(g_some_printf_function_9)(double param_1, double param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, uint param_9, undefined4 param_10, char * param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(item_pilot_init)(struct MKB(Item) * item);
    void MKB(item_pilot_tick)(struct MKB(Item) * item);
    void MKB(item_pilot_disp)(struct MKB(Item) * item);
    void MKB(item_pilot_coli)(int param_1, int param_2);
    void MKB(item_pilot_dest)(void);
    void MKB(item_pilot_replay_init)(void);
    void MKB(item_debug_pilot)(void);
    void MKB(boat_unlinked_func)(void);
    void MKB(g_load_boat)(void);
    void MKB(shooting_unlinked_func)(void);
    void MKB(g_read_something_for_shooting_from_dvd)(char * param_1, undefined4 * param_2, int * param_3);
    void MKB(g_read_something_for_shooting_2)(undefined4 * param_1, char * param_2, int * param_3);
    void MKB(mini_futsal_unlinked_func)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(dogfight_unlinked_func)(void);
    void MKB(g_load_dog)(void);
    void MKB(g_some_printf_function_10)(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void MKB(item_dogfight_init)(struct MKB(Item) * item);
    void MKB(item_dogfight_tick)(struct MKB(Item) * item);
    void MKB(item_dogfight_disp)(struct MKB(Item) * item);
    void MKB(item_dogfight_coli)(int param_1, int param_2);
    void MKB(item_dogfight_dest)(void);
    void MKB(item_dogfight_replay_init)(void);
    void MKB(item_debug_dogfight)(void);
    void MKB(g_baseball_prolog)(void);
    void MKB(baseball_unlinked_func)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    undefined4 MKB(g_baseball_game_loop)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_load_baseball)(byte player_id, MKB(BallMode)  param_2, undefined1 param_3, byte param_4, byte param_5, uint param_6, void * param_7, int param_8);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(empty_function)(void);
    void MKB(g_baseball_init_seesaws)(void);
    void MKB(empty_function)(void);
    undefined4 MKB(g_get_sprite_work)(int * param_1, int param_2);
    void MKB(empty_function)(void);
    void MKB(g_tennis_prolog)(void);
    void MKB(tennis_unlinked_func)(void);
    double MKB(something_with_distance_sq)(float * param_1, float * param_2);
    void MKB(GXResetOverflowCount)(int param_1, undefined4 param_2);
    void MKB(ttyClearProperty)(int param_1, undefined4 param_2);
    void MKB(exoption_prolog)(void);
    void MKB(exoption_epilog)(void);
    void MKB(exoption_unlinked_func)(void);
    void MKB(exoption_draw_func)(void);
    void MKB(create_replay_hud_sprites)(void);
    void MKB(create_replay_stage_name_sprites)(float x, float y);
#ifdef __cplusplus
} // extern "C"
} // namespace mkb
#endif

#undef MKB_CONCAT
#undef MKB_CONCAT_IMPL
#undef MKB
