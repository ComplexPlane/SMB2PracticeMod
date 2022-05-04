#pragma once

typedef unsigned char   undefined;

#ifndef __cplusplus
typedef unsigned char    bool;
#endif
typedef unsigned char    byte;
typedef unsigned int    dword;
typedef long long    longlong;
typedef unsigned long long    qword;
typedef unsigned char    uchar;
typedef unsigned int    uint;
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
typedef struct SeesawState SeesawState, *PSeesawState;

typedef float f32;

typedef f32 Mtx[3][4];

struct SeesawState {
    float angle; /* Current rotation angle as a float, in range [-0x8000, 0x7FFF] similar to s16 rotation angles */
    float prev_angle;
    float angle_vel;
    float sensitivity; /* Created by retype action */
    float friction; /* Created by retype action */
    float spring; /* Created by retype action */
    Mtx inv_transform;
    Mtx transform;
} __attribute__((__packed__));

enum { /* The character associated with an Ape */
    APE_AIAI=0,
    APE_MEEMEE=1,
    APE_BABY=2,
    APE_GONGON=3,
    APE_MADH=4,
    APE_KOBUN=5,
    APE_MALE=6,
    APE_FEMALE=7,
    APE_JIJI=8,
    APE_BABA=9,
    APE_MADSPA=10,
    APE_MADFRG=11,
    APE_MADNKD=12,
    APE_ROB=13,
    APE_WHALE=14,
    APE_MADCRS=15,
    APE_KOBSPA=16
};
typedef undefined4 ApeCharacter;

enum { /* When we're in Master, current difficulty is still 0x2 */
    DIFF_BEGINNER=0,
    DIFF_ADVANCED=1,
    DIFF_EXPERT=2
};
typedef undefined4 Difficulty;

typedef struct CmEntry CmEntry, *PCmEntry;

enum { /* See cmEntryFormat.txt notes */
    CMET_IF=0,
    CMET_THEN=1,
    CMET_INFO=2,
    CMET_END=3
};
typedef undefined1 CmEntryType;

typedef uchar u8;

typedef uint uint32_t;

typedef uint32_t u32;

struct CmEntry { /* Challenge Mode Entry, see cmEntryFormat.txt by TwixNinja in notes for more info */
    CmEntryType  type;
    u8 arg;
    undefined field2_0x2[2];
    u32 value;
    undefined field4_0x8[20];
} __attribute__((__packed__));

enum {
    G_STORY_LEVEL_SELECT_NO_BACKGROUND=0,
    G_LAVA_NO_BACKGROUND=1,
    JUNGLE1=2,
    WATER1=3,
    NIGHT=4,
    SKY=5,
    SPACE1=6,
    SAND=7,
    ICE=8,
    STORM=9,
    BONUS1=10,
    TARGET1=11,
    G_CONTINUE_A=12,
    G_CONTINUE_B=13,
    G_FIGHT2=14,
    MASTER=15,
    G_ENDING=16,
    LAVA=17,
    WATER2=18,
    JUNGLE2=19,
    PARK=20,
    POT=21,
    SPACE2=22,
    ELECTRIC=23,
    GEAR=24,
    BUBBLY=25,
    BONUS2=26,
    SOCCER=27,
    BOWLING2=28,
    TARGET2=29,
    BILLIARDS2=30,
    BOAT=31,
    WHALE=32,
    GOLF=33,
    POT_FIGHT=34,
    VILLAGE_STORY=35,
    BUBBLY_STORY=36,
    PARK_STORY=37,
    GEAR_STORY=38,
    UNDERWATER_STORY=39,
    WATER_SURFACE_STORY=40,
    BOWLING_STORY=41
};
typedef undefined1 WorldTheme;

typedef struct GoalTape GoalTape, *PGoalTape;

typedef long s32;

struct GoalTape { /* Extra goaltape-specific data pointed to by goaltape StageObjects */
    undefined field_0x0[0x10];
    s32 g_timer_frames_remaining;
    s32 field17_0x14;
    float field18_0x18;
    undefined field_0x1c[0x17c];
} __attribute__((__packed__));

typedef struct MenuScreen MenuScreen, *PMenuScreen;

typedef struct MenuEntry MenuEntry, *PMenuEntry;

enum {
    MODE_SELECT_MENU=0,
    PARTY_GAME_SELECT_MENU=1,
    OPTIONS_MENU=2,
    RETURN_TO_TITLE_SCREEN=3,
    NUMBER_OF_PLAYERS_MENU=4,
    CHARACTER_SELECT_MENU=5,
    CHARACTER_SELECT_GAMEWILLBEGIN_MENU=6,
    MAIN_GAME_SELECT_MENU=7,
    LEVEL_SELECT_NOMASTER_MENU=8,
    LEVEL_SELECT_WITHMASTER_MENU=9,
    PRACTICE_STAGE_SELECT_MENU=10,
    CHALLENGE_GAMEPLAY_SETTINGS_MENU=11,
    STORY_MODE_SELECTED=12,
    CHALLENGE_DIFFICULTY_SELECTED=13,
    MONKEY_RACE_MODE_SELECT_MENU=15,
    MONKEY_RACE_COURSE_SELECT_MENU=16,
    REPLAY_MENU=80,
    PLAY_POINTS_MENU=81,
    GIFT_MENU=82,
    RANKING_MENU=83,
    GAME_DATA_MENU=84,
    CONTROLLERS_MENU=85,
    SCREEN_MENU=86,
    SOUND_MENU=87
};
typedef undefined1 MenuScreenID;

struct MenuScreen {
    struct MenuEntry * menu_entries; /* Nullable */
    void * g_enter_submenu_func;
    u32 g_some_bitflag; /* 0x40 repositions stuff and makes stuff up/down controls */
    undefined field_0xc[0x4];
} __attribute__((__packed__));

struct MenuEntry {
    MenuScreenID  next_screen_id;
    undefined padding_0x1[0x3];
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

typedef struct BmpInfo BmpInfo, *PBmpInfo;

typedef int BOOL32;

typedef struct TplBuffer TplBuffer, *PTplBuffer;

typedef int OSHeapHandle;

typedef struct TplTextureHeader TplTextureHeader, *PTplTextureHeader;

typedef struct GXTexObj GXTexObj, *PGXTexObj;

enum {
    GX_TF_I4=0,
    GX_TF_I8=1,
    GX_TF_IA4=2,
    GX_TF_IA8=3,
    GX_TF_RGB565=4,
    GX_TF_RGB5A3=5,
    GX_TF_RGBA8=6,
    GX_TF_CMPR=14,
    GX_CTF_R4=15,
    GX_CTF_RA4=16,
    GX_CTF_RA8=17,
    GX_CTF_YUVA8=18,
    GX_CTF_A8=19,
    GX_CTF_R8=20,
    GX_CTF_G8=21,
    GX_CTF_B8=22,
    GX_CTF_RG8=23,
    GX_CTF_GB8=24,
    GX_TF_Z8=25,
    GX_TF_Z16=26,
    GX_TF_Z24X8=27,
    GX_CTF_Z4=28,
    GX_CTF_Z8M=29,
    GX_CTF_Z8L=30,
    GX_CTF_Z16L=31,
    GX_TF_A8=32
};
typedef undefined4 GXTexFmt;

typedef ushort u16;

struct GXTexObj {
    undefined field_0x0[0x14];
    GXTexFmt  format; /* Created by retype action */
    undefined field_0x18[0x8];
} __attribute__((__packed__));

struct TplBuffer { /* Buffer allocated for TPL files (with 32 extra bytes at the beginning compared to the on-disc TPL file). Amusement Vision TPL is different than standard Gamecube TPL */
    s32 texture_count;
    struct TplTextureHeader * texture_headers;
    void * raw_tpl_buffer; /* Pointer to the raw TPL data loaded from disc */
    struct GXTexObj * texobjs; /* Array of texobjs, one for each texture in the TPL */
    undefined field_0x10[0x10];
} __attribute__((__packed__));

struct BmpInfo { /* Bitmap info. Corresponds to a loaded TPL in bmp/ */
    BOOL32 is_loaded;
    char * filepath;
    char * category;
    undefined field_0xc[0x4];
    struct TplBuffer * tpl;
    OSHeapHandle heap;
} __attribute__((__packed__));

struct TplTextureHeader {
    GXTexFmt  format;
    dword data_offset;
    u16 width;
    u16 height;
    u16 mipmap_count;
    u16 always_0x1234;
} __attribute__((__packed__));

typedef struct CoinType CoinType, *PCoinType;

typedef short s16;

typedef struct S16Vec S16Vec, *PS16Vec;

struct S16Vec { /* Often used for rotations */
    s16 x;
    s16 y;
    s16 z;
} __attribute__((__packed__));

struct CoinType {
    s16 banana_count;
    s16 g_score_increment_flag;
    s16 g_score_value;
    struct S16Vec angular_velocity;
    undefined field_0xc[0x8];
} __attribute__((__packed__));

typedef struct GMotionData GMotionData, *PGMotionData;

struct GMotionData {
    undefined field_0x0[0x4c];
    float progress;
    float length;
} __attribute__((__packed__));

enum { /* NULL, INIT, NORMAL, and DEST, and FREEZE seem to be the most common */
    STAT_NULL=0,
    STAT_INIT=1,
    STAT_NORMAL=2,
    STAT_DEST=3,
    STAT_FREEZE=4,
    STAT_INIT_FIRST=5,
    STAT_GOAL_INIT=6,
    STAT_GOAL=7,
    STAT_RINGOUT_INIT=8,
    STAT_RINGOUT=9,
    STAT_MINI_MODE_0_INIT=10,
    STAT_MINI_MODE_0=11
};
typedef undefined1 Status;

typedef struct Vec Vec, *PVec;

struct Vec {
    float x;
    float y;
    float z;
} __attribute__((__packed__));

typedef struct GSomeBgStruct GSomeBgStruct, *PGSomeBgStruct;

struct GSomeBgStruct {
    u32 some_flag;
    char * bg_model_list;
    u32 field2_0x8;
    u32 * some_ptr_2;
    u32 field4_0x10;
    u32 * some_ptr_3;
    u32 field6_0x18;
    u32 * some_ptr_4;
    u32 field8_0x20;
    u32 * some_ptr_5;
    u32 some_counter;
    u32 field11_0x2c;
} __attribute__((__packed__));

enum { /* Sourced from 0x80370244 in Ghidra - I'm guessing SMD stands for Sub MoDe here? / Also I added SMD_INVALID, it's `-1` -Crafted */
    SMD_ADV_TOP=0,
    SMD_ADV_MOVIE_INIT=1,
    SMD_ADV_MOVIE_RETURN=2,
    SMD_ADV_LOGO_INIT=3,
    SMD_ADV_LOGO_MAIN=4,
    SMD_ADV_WARNING_INIT=5,
    SMD_ADV_WARNING_MAIN=6,
    SMD_ADV_DEMO_INIT=7,
    SMD_ADV_DEMO_MAIN=8,
    SMD_ADV_START_INIT=9,
    SMD_ADV_START_MAIN=10,
    SMD_ADV_RANKING_INIT=11,
    SMD_ADV_RANKING_MAIN=12,
    SMD_ADV_INFO_INIT=13,
    SMD_ADV_INFO_MAIN=14,
    SMD_ADV_GAME_READY_INIT=15,
    SMD_ADV_GAME_READY_MAIN=16,
    SMD_ADV_GAME_PLAY_INIT=17,
    SMD_ADV_GAME_PLAY_MAIN=18,
    SMD_ADV_TITLE_INIT=19,
    SMD_ADV_TITLE_MAIN=20,
    SMD_ADV_TITLE_REINIT=21,
    SMD_ADV_PROG_INIT=22,
    SMD_ADV_PROG_MAIN=23,
    SMD_ADV_FIRST_LOGO_INIT=24,
    SMD_ADV_FIRST_LOGO_MAIN=25,
    SMD_ADV_DMY8=26,
    SMD_ADV_BOTTOM=27,
    SMD_SEL_TOP=28,
    SMD_SEL_STAGE_INIT=29,
    SMD_SEL_STAGE_MAIN=30,
    SMD_SEL_NGC_INIT=31,
    SMD_SEL_NGC_MAIN=32,
    SMD_SEL_NGC_REINIT=33,
    SMD_SEL_NGC_DEST=34,
    SMD_SEL_DMY1=35,
    SMD_SEL_DMY2=36,
    SMD_SEL_DMY3=37,
    SMD_SEL_DMY4=38,
    SMD_SEL_DMY5=39,
    SMD_SEL_DMY6=40,
    SMD_SEL_DMY7=41,
    SMD_SEL_DMY8=42,
    SMD_SEL_DMY9=43,
    SMD_SEL_DMYa=44,
    SMD_SEL_DMYb=45,
    SMD_SEL_BOTTOM=46,
    SMD_GAME_TOP=47,
    SMD_GAME_READY_INIT=48,
    SMD_GAME_READY_MAIN=49,
    SMD_GAME_PLAY_INIT=50,
    SMD_GAME_PLAY_MAIN=51,
    SMD_GAME_GOAL_INIT=52,
    SMD_GAME_GOAL_MAIN=53,
    SMD_GAME_GOAL_REPLAY_INIT=54,
    SMD_GAME_GOAL_REPLAY_MAIN=55,
    SMD_GAME_TIMEOVER_INIT=56,
    SMD_GAME_TIMEOVER_MAIN=57,
    SMD_GAME_RINGOUT_INIT=58,
    SMD_GAME_RINGOUT_MAIN=59,
    SMD_GAME_FIRST_INIT=60,
    SMD_GAME_OVER_INIT=61,
    SMD_GAME_OVER_MAIN=62,
    SMD_GAME_NAMEENTRY_INIT=63,
    SMD_GAME_NAMEENTRY_MAIN=64,
    SMD_GAME_CONTINUE_INIT=65,
    SMD_GAME_CONTINUE_MAIN=66,
    SMD_GAME_RESTART_INIT=67,
    SMD_GAME_NAMEENTRY_READY_INIT=68,
    SMD_GAME_NAMEENTRY_READY_MAIN=69,
    SMD_GAME_BONUS_CLEAR_INIT=70,
    SMD_GAME_BONUS_CLEAR_MAIN=71,
    SMD_GAME_ENDING_INIT=72,
    SMD_GAME_ENDING_MAIN=73,
    SMD_GAME_EXTRA_INIT=74,
    SMD_GAME_EXTRA_WAIT=75,
    SMD_GAME_RESULT_INIT=76,
    SMD_GAME_RESULT_MAIN=77,
    SMD_GAME_RESULT_MENU=78,
    SMD_GAME_INTR_SEL_INIT=79,
    SMD_GAME_INTR_SEL_MAIN=80,
    SMD_GAME_OVER_SAVE=81,
    SMD_GAME_OVER_DEST=82,
    SMD_GAME_ROLL_INIT=83,
    SMD_GAME_ROLL_MAIN=84,
    SMD_GAME_OVER_POINT_INIT=85,
    SMD_GAME_OVER_POINT_MAIN=86,
    SMD_GAME_SCENARIO_INIT=87,
    SMD_GAME_SCENARIO_MAIN=88,
    SMD_GAME_SCENARIO_RETURN=89,
    SMD_GAME_RETRY_INIT=90,
    SMD_GAME_RETRY_MAIN=91,
    SMD_GAME_FORCE_EXIT_INIT=92,
    SMD_GAME_FORCE_EXIT_MAIN=93,
    SMD_GAME_SCENSCNPLAY_RETURN=94,
    SMD_GAME_FORCE_OVER_INIT=95,
    SMD_GAME_FORCE_OVER_MAIN=96,
    SMD_GAME_SUGG_SAVE_INIT=97,
    SMD_GAME_SUGG_SAVE_MAIN=98,
    SMD_GAME_DMY3=99,
    SMD_GAME_DMY4=100,
    SMD_GAME_DMY5=101,
    SMD_GAME_DMY6=102,
    SMD_GAME_DMY7=103,
    SMD_GAME_DMY8=104,
    SMD_GAME_DMY9=105,
    SMD_GAME_DMYA=106,
    SMD_GAME_DMYB=107,
    SMD_GAME_DMYC=108,
    SMD_GAME_DMYD=109,
    SMD_GAME_DMYE=110,
    SMD_GAME_DMYF=111,
    SMD_GAME_BOTTOM=112,
    SMD_TEST_TOP=113,
    SMD_TEST_SELECT_INIT=114,
    SMD_TEST_SELECT_MAIN=115,
    SMD_TEST_INPUT_INIT=116,
    SMD_TEST_INPUT_MAIN=117,
    SMD_TEST_DIP_INIT=118,
    SMD_TEST_DIP_MAIN=119,
    SMD_TEST_SOUND_INIT=120,
    SMD_TEST_SOUND_MAIN=121,
    SMD_TEST_BLUR_INIT=122,
    SMD_TEST_BLUR_MAIN=123,
    SMD_TEST_SHADOW_INIT=124,
    SMD_TEST_SHADOW_MAIN=125,
    SMD_TEST_FOCUS_INIT=126,
    SMD_TEST_FOCUS_MAIN=127,
    SMD_TEST_INDIRECT_INIT=128,
    SMD_TEST_INDIRECT_MAIN=129,
    SMD_TEST_NLOBJPUT_INIT=130,
    SMD_TEST_NLOBJPUT_MAIN=131,
    SMD_TEST_BITMAP_INIT=132,
    SMD_TEST_BITMAP_MAIN=133,
    SMD_TEST_FONT_INIT=134,
    SMD_TEST_FONT_MAIN=135,
    SMD_TEST_REPLAY_INIT=136,
    SMD_TEST_REPLAY_MAIN=137,
    SMD_TEST_MOTION_INIT=138,
    SMD_TEST_MOTION_MAIN=139,
    SMD_TEST_ADVREPLAY_INIT=140,
    SMD_TEST_MODEL_INIT=141,
    SMD_TEST_MODEL_MAIN=142,
    SMD_TEST_NEWMODEL_INIT=143,
    SMD_TEST_NEWMODEL_MAIN=144,
    SMD_TEST_PATTERN_INIT=145,
    SMD_TEST_PATTERN_MAIN=146,
    SMD_TEST_ROMFONT_INIT=147,
    SMD_TEST_ROMFONT_MAIN=148,
    SMD_TEST_PREREND_INIT=149,
    SMD_TEST_PREREND_MAIN=150,
    SMD_TEST_PREVIEW_INIT=151,
    SMD_TEST_PREVIEW_MAIN=152,
    SMD_TEST_ADX_INIT=153,
    SMD_TEST_ADX_MAIN=154,
    SMD_TEST_NEWMOTION_INIT=155,
    SMD_TEST_NEWMOTION_MAIN=156,
    SMD_TEST_ARAM_FONT_INIT=157,
    SMD_TEST_ARAM_FONT_MAIN=158,
    SMD_TEST_SCENE_PLAY_INIT=159,
    SMD_TEST_SCENE_PLAY_MAIN=160,
    SMD_TEST_DIALOG_INIT=161,
    SMD_TEST_DIALOG_MAIN=162,
    SMD_TEST_DIFFICULTY_INIT=163,
    SMD_TEST_DIFFICULTY_MAIN=164,
    SMD_TEST_CHK_REPSIZE_INIT=165,
    SMD_TEST_CHK_REPSIZE_MAIN=166,
    SMD_TEST_BOTTOM=167,
    SMD_MINI_TOP=168,
    SMD_MINI_SELECT_INIT=169,
    SMD_MINI_SELECT_MAIN=170,
    SMD_MINI_BOWLING_INIT=171,
    SMD_MINI_BOWLING_MAIN=172,
    SMD_MINI_BILLIARDS_INIT=173,
    SMD_MINI_BILLIARDS_MAIN=174,
    SMD_MINI_SHOOTING_INIT=175,
    SMD_MINI_SHOOTING_MAIN=176,
    SMD_MINI_REL_SAMPLE_INIT=177,
    SMD_MINI_REL_SAMPLE_MAIN=178,
    SMD_MINI_COMMEND_INIT=179,
    SMD_MINI_COMMEND_MAIN=180,
    SMD_MINI_RANKING_INIT=181,
    SMD_MINI_RANKING_MAIN=182,
    SMD_MINI_ENDING_INIT=183,
    SMD_MINI_ENDING_MAIN=184,
    SMD_MINI_RACE2_INIT=185,
    SMD_MINI_RACE2_MAIN=186,
    SMD_MINI_GOLF2_INIT=187,
    SMD_MINI_GOLF2_MAIN=188,
    SMD_MINI_FIGHT2_INIT=189,
    SMD_MINI_FIGHT2_MAIN=190,
    SMD_MINI_PILOT2_INIT=191,
    SMD_MINI_PILOT2_MAIN=192,
    SMD_MINI_BOAT_INIT=193,
    SMD_MINI_BOAT_MAIN=194,
    SMD_MINI_S_ROLL_INIT=195,
    SMD_MINI_S_ROLL_MAIN=196,
    SMD_MINI_FUTSAL_INIT=197,
    SMD_MINI_FUTSAL_MAIN=198,
    SMD_MINI_DOGFIGHT_INIT=199,
    SMD_MINI_DOGFIGHT_MAIN=200,
    SMD_MINI_BASEBALL_INIT=201,
    SMD_MINI_BASEBALL_MAIN=202,
    SMD_MINI_TENNIS_INIT=203,
    SMD_MINI_TENNIS_MAIN=204,
    SMD_MINI_DMY9=205,
    SMD_MINI_DMY10=206,
    SMD_MINI_DMY11=207,
    SMD_MINI_DMY12=208,
    SMD_MINI_BOTTOM=209,
    SMD_OPTION_TOP=210,
    SMD_OPTION_SELECT_INIT=211,
    SMD_OPTION_SELECT_MAIN=212,
    SMD_OPTION_SOUND_INIT=213,
    SMD_OPTION_SOUND_MAIN=214,
    SMD_OPTION_PAD_INIT=215,
    SMD_OPTION_PAD_MAIN=216,
    SMD_OPTION_MEMCARD_INIT=217,
    SMD_OPTION_MEMCARD_MAIN=218,
    SMD_OPTION_POINT_INIT=219,
    SMD_OPTION_POINT_MAIN=220,
    SMD_OPTION_RANKING_INIT=221,
    SMD_OPTION_RANKING_MAIN=222,
    SMD_OPTION_AUTOSAVE_INIT=223,
    SMD_OPTION_AUTOSAVE_MAIN=224,
    SMD_OPTION_REPLAY_INIT=225,
    SMD_OPTION_REPLAY_MAIN=226,
    SMD_OPTION_SEPARATE_INIT=227,
    SMD_OPTION_SEPARATE_MAIN=228,
    SMD_OPTION_REPLAY_PLAY_INIT=229,
    SMD_OPTION_REPLAY_PLAY_MAIN=230,
    SMD_OPTION_PAD_MENU_INIT=231,
    SMD_OPTION_PAD_MENU_MAIN=232,
    SMD_OPTION_PAD_VIB_INIT=233,
    SMD_OPTION_PAD_VIB_MAIN=234,
    SMD_OPTION_DMY0=235,
    SMD_OPTION_DMY1=236,
    SMD_OPTION_DMY2=237,
    SMD_OPTION_DMY3=238,
    SMD_OPTION_DMY4=239,
    SMD_OPTION_DMY5=240,
    SMD_OPTION_DMY6=241,
    SMD_OPTION_DMY7=242,
    SMD_OPTION_BOTTOM=243,
    SMD_AUTHOR_TOP=244,
    SMD_AUTHOR_SELECT_INIT=245,
    SMD_AUTHOR_SELECT_MAIN=246,
    SMD_AUTHOR_PLAY_INIT=247,
    SMD_AUTHOR_PLAY_MAIN=248,
    SMD_AUTHOR_PLAY_STORY_INIT=249,
    SMD_AUTHOR_PLAY_FROM_SEL=250,
    SMD_AUTHOR_PLAY_RET_SEL=251,
    SMD_AUTHOR_PLAY_ENDING_INIT=252,
    SMD_AUTHOR_DMY6=253,
    SMD_AUTHOR_DMY7=254,
    SMD_AUTHOR_BOTTOM=255,
    SMD_EXOPT_TOP=256,
    SMD_EXOPT_REPLAY_LOAD_INIT=257,
    SMD_EXOPT_REPLAY_LOAD_MAIN=258,
    SMD_EXOPT_REPLAY_INIT=259,
    SMD_EXOPT_REPLAY_MAIN=260,
    SMD_EXOPT_DMY5=261,
    SMD_EXOPT_DMY6=262,
    SMD_EXOPT_DMY7=263,
    SMD_EXOPT_BOTTOM=264,
    SMD_INVALID=4294967295
};
typedef undefined4 SubMode;

enum { /* I made MD_INVALID, it's `-1` -Crafted */
    MD_ADV=0,
    MD_SEL=1,
    MD_GAME=2,
    MD_TEST=3,
    MD_MINI=4,
    MD_OPTION=5,
    MD_AUTHOR=6,
    MD_EXOPT=7,
    MD_INVALID=4294967295
};
typedef undefined4 MainMode;

enum { /* I added DIP_NONE -Crafted */
    DIP_NONE=0,
    DIP_DEBUG=1,
    DIP_DISP=2,
    DIP_STCOLI=4,
    DIP_TRIANGLE=8,
    DIP_TAIKEN=16,
    DIP_TIME_STOP=32,
    DIP_NAMEENTRY=64,
    DIP_FIX_WORLD=128,
    DIP_TEST_CAM=256,
    DIP_NO_INTR=512,
    DIP_CAPTURE=1024,
    DIP_PERF_ALWAYS=2048,
    DIP_PLAY_STG_ALL=4096,
    DIP_PLAY_PNT_x10=8192,
    DIP_SARU_0=16384,
    DIP_SWITCH15=32768,
    DIP_ONLY24MB=65536,
    DIP_SHADOW_DISP=131072,
    DIP_SPRITE_OFF=262144,
    DIP_SNDREQ_DISP=524288,
    DIP_SE_VOL_RESET=1048576,
    DIP_APE_FACEDIR=2097152,
    DIP_SEL_ALL_CTRL=4194304,
    DIP_APE_NUMBER=8388608,
    DIP_1P_TEST_CAM=16777216,
    DIP_PREVIEW_SAVE=33554432,
    DIP_AUTHOR_DEBUG=67108864,
    DIP_SWITCH27=134217728,
    DIP_SOA=268435456,
    DIP_TEST_TEAM=536870912,
    DIP_NO_MINIMAP=1073741824,
    DIP_NO_STAGE=2147483648
};
typedef undefined4 DipSwitch;

typedef struct Replay Replay, *PReplay;

struct Replay { /* Unknown size atm */
    undefined field_0x0[0x4];
    u8 difficulty;
    u8 floorNum;
    u8 monkey;
    undefined field_0x7[0x9];
    u32 field16_0x10;
    undefined field_0x14[0x4];
    s16 g_stageTimeLimit1;
    s16 g_duration;
    undefined field_0x1c[0x2];
    s16 goalId;
    undefined field_0x20[0x4];
    struct Vec g_ballVelAtGoal;
    undefined field_0x30[0x4];
    u32 size;
    s16 g_currFrame;
    s16 g_stageTimeLimit2;
    struct Vec initBallPos;
    u8 * ballDeltaXLo;
    u8 * ballDeltaXHi;
    u8 * ballDeltaYLo;
    u8 * ballDeltaYHi;
    u8 * ballDeltaZLo;
    u8 * ballDeltaZHi;
    u8 * ballRotXLo;
    u8 * ballRotXHi;
    u8 * ballRotYLo;
    u8 * ballRotYHi;
    u8 * ballRotZLo;
    u8 * ballRotZHi;
    u8 * sparkDirX;
    u8 * sparkDirY;
    u8 * sparkDirZ;
    u8 * sparkIntensity;
    u8 * coliFlags0;
    u8 * coliFlags1;
    u8 * coliFlags2;
    u8 * coliFlags3;
    u8 * stageTiltXLo;
    u8 * stageTiltXHi;
    u8 * stageTiltZLo;
    u8 * stageTiltZHi;
    u8 field63_0xa8[4];
    s16 playableItemgroupCount;
    s16 seesawCount;
    u8 * playableItemgroupAnimFrames;
    u8 * seesawState;
} __attribute__((__packed__));

typedef struct Ape Ape, *PApe;

typedef struct SKLRoot SKLRoot, *PSKLRoot;

typedef struct ArcFileInfo ArcFileInfo, *PArcFileInfo;

typedef struct SKLFile SKLFile, *PSKLFile;

typedef struct GApeAnim GApeAnim, *PGApeAnim;

typedef struct GmaBuffer GmaBuffer, *PGmaBuffer;

enum {
    GAME_COMMON=0,
    GAME_MAIN=1,
    GAME_DUMMY1=2,
    GAME_DUMMY2=3,
    GAME_DUMMY3=4,
    GAME_DUMMY4=5,
    GAME_DUMMY5=6,
    GAME_DUMMY6=7,
    GAME_DUMMY7=8,
    GAME_DUMMY8=9,
    GAME_DUMMY9=10,
    GAME_DUMMY10=11,
    GAME_RACE=12,
    GAME_FGT=13,
    GAME_PILOT=14,
    GAME_GOLF=15,
    GAME_BOWL=16,
    GAME_BILL=17,
    GAME_BOAT=18,
    GAME_DOGF=19,
    GAME_TENNIS=20,
    GAME_BBALL=21,
    GAME_GUNS=22,
    GAME_ADV=23,
    GAME_SEL=24,
    GAME_COMMEND=25,
    GAME_SCENE1A=26,
    GAME_SCENE1B=27,
    GAME_SCENE2=28,
    GAME_SCENE3=29,
    GAME_SCENE4=30,
    GAME_SCENE5=31,
    GAME_SCENE6=32,
    GAME_SCENE7=33,
    GAME_SCENE8=34,
    GAME_SCENE9=35,
    GAME_SCENE10=36,
    GAME_LASTSCENE=37,
    GAME_ENDING1=38,
    GAME_ENDING2=39,
    GAME_ENDING3=40,
    GAME_ENDING4=41
};
typedef undefined2 ApeGame;

enum {
    FACE_BASE=0,
    FACE_CLOSEEYE=1,
    FACE_SMILE=2,
    FACE_PISTOFF=3,
    FACE_SOB=4,
    FACE_SURPRISE=5,
    FACE_TALK=6,
    FACE_SMILETALK=7,
    FACE_PISTOFFTALK=8,
    FACE_SOBTALK=9
};
typedef undefined2 ApeFace;

typedef struct Quat Quat, *PQuat;

enum {
    LOD_HIGH=0,
    LOD_MIDDLE=1,
    LOD_LOW=2,
    LOD_LOWEST=3
};
typedef undefined4 ApeLOD;

typedef struct SKLInfo SKLInfo, *PSKLInfo;

typedef struct gFloats gFloats, *PgFloats;

typedef struct ARCHandle ARCHandle, *PARCHandle;

typedef struct SKLBone SKLBone, *PSKLBone;

typedef struct g_thing g_thing, *Pg_thing;

typedef struct GmaModelEntry GmaModelEntry, *PGmaModelEntry;

typedef struct GmaModelHeader GmaModelHeader, *PGmaModelHeader;

enum { /* Per-GMA model attributes */
    GCMF_ATTR_16BIT=1,
    GCMF_ATTR_STITCHING_MODEL=4,
    GCMF_ATTR_SKIN_MODEL=8,
    GCMF_ATTR_EFFECTIVE_MODEL=16
};
typedef undefined4 GcmfAttributes;

typedef signed char s8;

struct Quat {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} __attribute__((__packed__));

struct gFloats {
    short field0_0x0;
    short field1_0x2;
    short field2_0x4;
    short field3_0x6;
    struct Vec vec3;
} __attribute__((__packed__));

struct SKLRoot {
    ushort bone_count;
    ushort g_flag; /* Created by retype action */
    float g_some_float;
    struct SKLInfo * bones;
    struct Quat rotation1;
    struct Quat rotation2;
    char * g_bone_name;
    struct Vec translation;
    undefined field_0x3c[0x4];
    struct gFloats float_thing1;
    undefined field_0x54[0x4];
    struct gFloats float_thing2;
    undefined field_0x6c[0x4];
    struct SKLInfo * gBoneData;
    undefined field_0x74[0x100];
} __attribute__((__packed__));

struct SKLBone {
    short parentNumber;
    short g_maybe_flag;
    struct Quat Rotation1;
    struct Quat Rotation2;
    struct Vec Translation;
} __attribute__((__packed__));

struct SKLFile {
    short bone_count;
    undefined2 padding;
    struct SKLBone bone_section;
} __attribute__((__packed__));

struct ArcFileInfo {
    struct ARCHandle * Handle;
    int startoffset;
    int length;
} __attribute__((__packed__));

struct g_thing {
    undefined field_0x0[0x8];
    char * Name;
    undefined field_0xc[0x1c];
} __attribute__((__packed__));

struct GmaBuffer { /* Represents the first 32 bytes of buffer allocated for loaded GMA files. The first 32 bytes are extra; not part of the original GMA file */
    s32 model_count;
    void * model_list_ptr; /* Pointer to the first model (after GMA header) */
    struct GmaModelEntry * model_entries;
    dword model_names_ptr;
    dword g_initially_zero;
    undefined field_0x14[0xc];
} __attribute__((__packed__));

struct SKLInfo {
    char * bone_name;
    short g_flag;
    short parentNumber;
    struct Vec vec3f1;
    float vec3f1_len; /* Created by retype action */
    struct Vec vec3f1copy;
    float g_float;
    Mtx g_mtx1;
    undefined1 g_mtx2; /* Created by retype action */
    undefined field_0x59[0x2f];
    Mtx matrix; /* Created by retype action */
    struct Vec vec3f3; /* Created by retype action */
    undefined field_0xc4[0xc];
    float g_float2;
    undefined field_0xd4[0xa0];
} __attribute__((__packed__));

struct GmaModelHeader { /* Also known as a GCMF (GameCube Model Format?) */
    char gcmf_magic[4]; /* Just the string "GCMF" */
    GcmfAttributes  attrs; /* Also called "section flags" */
    struct Vec origin; /* Also the center of the bounding sphere */
    float bounding_sphere_radius;
    u16 texture_count; /* In F-Zero GX this is "texture count" */
    u16 material_count; /* In F-Zero GX this is "material count" */
    u16 translucid_material_count; /* In F-Zero GX this is "translucid material count" */
    s8 transform_mtx_count;
    undefined field_0x1f[0x1];
    s32 model_header_size; /* Memory size of this structure in bytes, including texture description array and transform matrices. */
    struct GXTexObj * texobjs; /* Array of texobjs, one for each texture in the model */
    s8 default_mtx_indices[8]; /* Default (root?) indices into Transform Matrix array */
    undefined field_0x30[0x18];
    struct GXTexObj * g_some_texobjs_ptr;
    undefined field_0x4c[0x8];
    void * g_some_ptr;
    undefined field_0x58[0x8];
} __attribute__((__packed__));

struct ARCHandle { /* I don't actually know the struct contents in the slightest */
    void * arc_data;
    void * g_first_node_offset;
    s32 c;
    s32 d;
    s32 e;
    s32 f;
    s32 g;
} __attribute__((__packed__));

struct GApeAnim { /* Unknown length -Crafted */
    u8 field0_0x0;
    undefined field_0x1[0x3];
    struct g_thing * count;
    struct GApeAnim * ape;
    s32 * field6_0xc; /* Ptr to some struct? */
    undefined field_0x10[0xc];
    s32 g_either_motion_or_skel2;
    undefined field_0x20[0x8];
} __attribute__((__packed__));

struct Ape {
    struct SKLRoot * g_maybe_sk;
    void * common_mal; /* Was originally ushort -Crafted */
    void * game_mal; /* It's not, a lot of these labels were confusingly wrong?? - bomb Is this even a float? -Crafted */
    struct SKLRoot * frame_ptr;
    struct ArcFileInfo * arc_location;
    float g_some_ptr;
    struct SKLRoot * faceSKLStorage;
    struct SKLFile * faceSKLFile;
    void * faceMTA;
    struct SKLRoot * handRSKLStorage;
    struct SKLFile * handRSKLFile;
    void * handrMTA;
    struct SKLRoot * handLSKLStorage;
    struct SKLFile * handLSKLFile;
    void * handlMTA;
    struct SKLRoot * tailSKLStorage;
    struct SKLFile * tailSKLFile;
    struct GApeAnim * animLengthBytes; /* Created by retype action */
    struct SKLRoot * g_some_skl;
    struct GmaBuffer * GMABuffer;
    struct TplBuffer * TPLBuffer;
    undefined2 field21_0x54;
    undefined field_0x56[0x2];
    undefined2 field24_0x58;
    undefined field_0x5a[0x2];
    void * anim_func; /* Created by retype action */
    float float_0x60;
    undefined field_0x64[0x10];
    ApeGame  game; /* Created by retype action */
    char g_ape_variant;
    undefined field_0x77[0x1];
    int g_smth_with_game; /* Created by retype action */
    undefined4 field49_0x7c;
    undefined2 field50_0x80;
    undefined field_0x82[0x2];
    short field53_0x84;
    u8 chara_anim_type; /* Some value that changes the type of animation the character is doing. From standing still, to walking, to the "I lost" state, etc */
    byte g_anim_step;
    int ape_total_index; /* Something to do with character spinning post-goal */
    ushort g_handr_short;
    ushort g_handl_short;
    ApeFace  face;
    undefined field_0x92[0x6];
    float g_handl_float;
    float g_handr_float;
    ushort some_flag;
    ushort some_flag_1;
    undefined field_0xa4[0x4];
    int g_some_length1;
    int field75_0xac;
    float field76_0xb0;
    undefined4 field77_0xb4;
    undefined4 field78_0xb8;
    undefined4 field79_0xbc;
    int field80_0xc0;
    void * g_frames1[40];
    void * field82_0x164;
    void * field83_0x168;
    void * field84_0x16c;
    void * field85_0x170;
    void * field86_0x174;
    void * field87_0x178;
    void * field88_0x17c;
    void * field89_0x180;
    void * field90_0x184;
    void * field91_0x188;
    void * field92_0x18c;
    void * field93_0x190;
    void * field94_0x194;
    void * field95_0x198;
    void * field96_0x19c;
    void * field97_0x1a0;
    void * field98_0x1a4;
    void * field99_0x1a8;
    void * field100_0x1ac;
    void * field101_0x1b0;
    void * field102_0x1b4;
    void * field103_0x1b8;
    void * field104_0x1bc;
    undefined field_0x1c0[0x1c];
    void * expression_models[9];
    undefined field_0x200[0x4];
    void * HandL_GHA_model;
    void * HandL_PHA_model;
    void * HandR_GHA_model;
    void * HANDR_PHA_model;
    float float_0x214;
    short field143_0x218;
    undefined field_0x21a[0x2];
    char flag2;
    undefined field_0x21d[0x3];
    uint flag3;
    uint flag4;
    undefined field_0x228[0x14];
    undefined4 field172_0x23c;
    undefined4 field173_0x240;
    int character;
    u32 flag1;
    undefined4 field176_0x24c;
    undefined4 field177_0x250;
    undefined4 field178_0x254;
    struct Vec pos;
    struct Vec some_vec3;
    undefined4 field181_0x270;
    undefined4 field182_0x274;
    undefined4 field183_0x278;
    undefined4 field184_0x27c;
    float scale;
    undefined field_0x284[0x4];
    struct Quat chara_rotation;
    int ape_index;
    undefined4 field192_0x29c;
    ApeLOD  lod;
    undefined4 field194_0x2a4;
    float g_smth_w_velocity1;
    float g_smth_w_velocity2;
    float g_smth_w_velocity3;
    float g_smth_w_velocity4;
    undefined4 field199_0x2b8;
    int color_index;
    undefined field_0x2c0[0x8];
    char ball_idx; /* Created by retype action */
    byte camera_visible_bitmask;
    undefined field_0x2ca[0x2];
    float field213_0x2cc;
    float field214_0x2d0;
    float field215_0x2d4;
    float field216_0x2d8;
    float field217_0x2dc;
    float field218_0x2e0;
    float field219_0x2e4;
    float field220_0x2e8;
    float field221_0x2ec;
} __attribute__((__packed__));

struct GmaModelEntry {
    struct GmaModelHeader * model;
    char * name;
} __attribute__((__packed__));

typedef struct Camera Camera, *PCamera;

typedef struct Vec2d Vec2d, *PVec2d;

struct Vec2d {
    float x;
    float y;
} __attribute__((__packed__));

struct Camera {
    struct Vec pos; /* Position of the camera */
    struct Vec pivot; /* Called 'intr' in the debug menu. The point which the camera looks at - close to the monkey in normal play, but can also be modified with the C-stick in test camera. Camera pose is actually controled by pos and rot - pivot is an intermediate value */
    struct S16Vec rot; /* Rotation of the camera. Called 'ang' in the debug menu */
    u8 mode; /* One byte representing camera mode (1 (dec) = SMB 1 style camera, 75 (dec) = SMB 2 style camera, and there's a lot more (everything from 1 to about 105 - a few are duplicates. Each value  corresponds to a different function in the camera function table */
    u8 submode; /* Called 'SUB' in the debug menu. Not sure of the purpose */
    float g_some_float;
    u16 g_some_bitflags;
    u8 g_some_flag; /* Set to '2' in a fallout state, set to 1 during spin-in */
    u8 g_some_flag2;
    float g_some_float2;
    float g_some_float3;
    u16 fov; /* Field of view of the camera, called 'pers' in the debug menu */
    u16 next_fov; /* Next field of view - when the camera FOV changes through some event (goal replay, new level start), this value is taken and used for the FOV */
    float aspect_ratio; /* Aspect ratio of the camera */
    float fov_tangent; /* Tangent of (fov/32768)*(pi/2) */
    float fov_cotangent; /* Cotangent of (fov/32768)*(pi/2) */
    float start_draw_distance; /* Relative to camera position */
    float end_draw_distance; /* Relative to camera position */
    struct Vec2d viewport_pos;
    struct Vec2d viewport_size;
    u16 g_some_counter1;
    u16 g_some_short;
    struct Vec g_initial_pivot; /* The pivot is set to this point at the beginning of the spin-in sequence, and approaches g_dest_intr */
    float g_spinin_value_1; /* Affects something with camera spin-in */
    float g_spinin_value_2; /* Affects something with camera spin-in */
    undefined2 field25_0x70;
    undefined2 field26_0x72;
    undefined2 field27_0x74;
    undefined2 field28_0x76;
    undefined field_0x78[0x4];
    struct Vec g_final_pivot; /* The pivot moves towards this point, and reaches it at the end of the spin-in sequence */
    undefined field_0x88[0x8];
    undefined2 field42_0x90;
    undefined2 field43_0x92;
    undefined field_0x94[0x8];
    struct Vec vel;
    struct Vec pivot_vel;
    struct Vec g_pos_desire; /* Something to do with camera rotation interpolation? */
    s16 g_pivot_x_rot;
    undefined field_0xc2[0x52];
    s16 g_y_rot_vel;
    undefined field_0x116[0x2];
    undefined1 g_some_goal_idx; /* Created by retype action */
    undefined field_0x119[0x1b];
    struct Vec g_some_vec5;
    struct Vec g_some_vec6;
    Mtx g_some_mtx1;
    Mtx g_some_mtx2;
    Mtx g_some_mtx3;
    Mtx g_some_mtx4;
    u32 idx;
    undefined field_0x210[0x7c];
} __attribute__((__packed__));

typedef struct Rect Rect, *PRect;

struct Rect {
    struct Vec pos;
    struct S16Vec rot;
    undefined field_0x12[0xe];
    float width;
    float height;
} __attribute__((__packed__));

enum {
    CHALLENGE_MODE=0,
    COMPETITION_MODE=1,
    PRACTICE_MODE=2,
    G_RETRY_W_NO_SHADOW=3,
    G_NO_RETRY_VIEWSTAGE_INFLIFE=4,
    G_NO_RETRY_VIEWSTAGE_INFLIFE_NOCOUNTER=5,
    G_NO_PAUSING_NOCOUNTER=6,
    G_NO_FALLOUT_SCREAM=7,
    G_NO_FALLOUT_SCREAM_CRYING=8,
    G_NO_FALLOUT_SCREEN_CHEERING=9,
    G_NO_SHADOWS_CRASH_ON_FALLOUT=10,
    G_SPAWN_OPPOSITE_OF_STARTPOS=11,
    G_SPAWN_ORIGIN=12,
    G_DANCE_CONTINUE_SCREEN=13,
    STORY_MODE=14,
    G_H_POSE_CONTINUE_SCREEN=15
};
typedef undefined4 MainGameMode;

typedef struct Sprite Sprite, *PSprite;

enum { /* How to render the sprite if a disp() function is not provided */
    SPRT_TEXT=0,
    SPRT_BMP=1,
    SPRT_TEXTURE=2
};
typedef undefined1 SpriteType;

enum {
    FONT_ASCII=0,
    FONT_ASC_8x16=1,
    FONT_ASC_12x12=2,
    FONT_ASC_24x24=3,
    FONT_ASC_16x16P=4,
    FONT_ASC_16x16=5,
    FONT_DMY03=6,
    FONT_DMY04=7,
    FONT_DMY05=8,
    FONT_ASC_72x64=9,
    FONT_DMY06=10,
    FONT_DMY07=11,
    FONT_DMY08=12,
    FONT_DMY09=13,
    FONT_DMY10=14,
    FONT_DMY11=15,
    FONT_DMY12=16,
    FONT_DMY13=17,
    FONT_DMY14=18,
    FONT_DMY15=19,
    FONT_DMY16=20,
    FONT_ICON_SD=21,
    FONT_ICON_SD2=22,
    FONT_DMY20=23,
    FONT_DMY21=24,
    FONT_DMY22=25,
    FONT_DMY23=26,
    FONT_DMY_RNK00=27,
    FONT_DMY_RNK01=28,
    FONT_DMY_RNK02=29,
    FONT_DMY_RNK03=30,
    FONT_DMY_RNK04=31,
    FONT_RNK_32x32=32,
    FONT_RNK_NUM=33,
    FONT_MINI_RNK=34,
    FONT_SCORE_NUM=35,
    FONT_DMY_RNK0=36,
    FONT_DMY_RNK1=37,
    FONT_DMY_RNK2=38,
    FONT_DMY_RNK3=39,
    FONT_DMY_RNK4=40,
    FONT_DMY_RNK5=41,
    FONT_DMY_RNK6=42,
    FONT_DMY_RNK7=43,
    FONT_DMY_RNK8=44,
    FONT_DMY_RNK9=45,
    FONT_DMY_RNKa=46,
    FONT_DMY_RNKb=47,
    FONT_DMY_RNKc=48,
    FONT_DMY_RNKd=49,
    FONT_DMY_RNKe=50,
    FONT_DMY_RNKf=51,
    FONT_SEL_CTRL_PORT=52,
    FONT_SEL_CTRL_R=53,
    FONT_SEL_CTRL_B=54,
    FONT_SEL_CTRL_Y=55,
    FONT_SEL_CTRL_G=56,
    FONT_SEL_CTRL_SDW=57,
    FONT_DMY42=58,
    FONT_DMY43=59,
    FONT_BWL_SCORE=60,
    FONT_DMY50=61,
    FONT_DMY51=62,
    FONT_DMY52=63,
    FONT_DMY53=64,
    FONT_DMY54=65,
    FONT_DMY55=66,
    FONT_DMY56=67,
    FONT_DMY57=68,
    FONT_RAC_DMY01=69,
    FONT_RAC_PLAYER=70,
    FONT_RAC_RANK=71,
    FONT_RAC_LAP_MARK_NUM=72,
    FONT_RAC_LAP_NUM=73,
    FONT_RAC_TIME_NUM=74,
    FONT_RAC_LAP_MARK_NUM_S=75,
    FONT_RAC_DMY08=76,
    FONT_RAC_DMY09=77,
    FONT_RAC_DMY10=78,
    FONT_RAC_SPD_NUM=79,
    FONT_RAC_DMY12=80,
    FONT_RAC_DMY13=81,
    FONT_TGT_PLAYER=82,
    FONT_TGT_MSCORE=83,
    FONT_TGT_SCORE=84,
    FONT_TGT_SCORE_S=85,
    FONT_TGT_SPEED=86,
    FONT_TGT_ROUND=87,
    FONT_TGT_ALT=88,
    FONT_TGT_WIND=89,
    FONT_DMY71=90,
    FONT_DMY72=91,
    FONT_DMY73=92,
    FONT_NUM_NML_SCORE=93,
    FONT_NUM_NML_TIME=94,
    FONT_NUM_NML_TIME_S=95,
    FONT_NUM_NML_SPEED=96,
    FONT_NUM_NML_SARU=97,
    FONT_ASC_NML_D5=98,
    FONT_ASC_NML_D6=99,
    FONT_ASC_NML_D7=100,
    FONT_ASC_NML_D8=101,
    FONT_ASC_NML_D9=102,
    FONT_DMY79=103,
    FONT_DMY7a=104,
    FONT_DMY7b=105,
    FONT_DMY7c=106,
    FONT_DMY7d=107,
    FONT_DMY7e=108,
    FONT_DMY7f=109,
    FONT_BOA_TIME_NUM=110,
    FONT_BOA_DMY02=111,
    FONT_BOA_RANK=112,
    FONT_BOA_PLAYER=113,
    FONT_BOA_SPD_NUM=114,
    FONT_BOA_LAP_MARK_NUM=115,
    FONT_BOA_LAP_MARK_NUM_S=116,
    FONT_BOA_DMY08=117,
    FONT_BOA_DMY09=118,
    FONT_BOA_DMY10=119,
    FONT_BOA_LAP_NUM=120,
    FONT_BOA_DMY12=121,
    FONT_BOA_DMY13=122,
    FONT_BOA_DMY14=123,
    FONT_BOA_DMY15=124,
    FONT_BOA_DMY16=125,
    FONT_DOG_SPEED=126,
    FONT_DOG_SPEED_S=127,
    FONT_DOG_MSCORE=128,
    FONT_DOG_SCORE=129,
    FONT_DOG_SCORE_M=130,
    FONT_DOG_SCORE_S=131,
    FONT_DOG_ROUND=132,
    FONT_DOG_ALT=133,
    FONT_DOG_RESULT=134,
    FONT_DOG_TIME=135,
    FONT_DOG_DMY02=136,
    FONT_DOG_DMY03=137,
    FONT_DOG_DMY04=138,
    FONT_DOG_DMY05=139,
    FONT_JAP_TAG=140,
    FONT_JAP_DMY=141,
    FONT_JAP_24x24_2=142,
    FONT_JAP_24x24_2P=143,
    FONT_JAP_24x24_I=144
};
typedef undefined1 Font8;

typedef struct Rgb24 Rgb24, *PRgb24;

typedef struct SpriteTex SpriteTex, *PSpriteTex;

typedef struct DVDFileInfo DVDFileInfo, *PDVDFileInfo;

typedef struct DVDCommandBlock DVDCommandBlock, *PDVDCommandBlock;

typedef struct DVDDiskID DVDDiskID, *PDVDDiskID;

struct Rgb24 {
    u8 red;
    u8 green;
    u8 blue;
} __attribute__((__packed__));

struct Sprite {
    SpriteType  type; /* Whether it's visible or not? */
    Font8  font; /* Seems to affect the font size/type on the pause menu? */
    u8 index;
    undefined1 field3_0x3;
    struct Vec2d pos;
    struct Rgb24 mult_color;
    Font8  g_probably_not_font; /* Is this actually a font? Or is it some kind of ID? On the pause menu sprite, the monkey head won't track the menu selection unless it's "4" */
    s16 g_counter; /* At least in the press start/select text sprites, this is used as some kind of counter when ticking */
    s16 field8_0x12;
    undefined field_0x14[0xc];
    float field21_0x20;
    undefined4 field22_0x24;
    undefined field_0x28[0x4];
    struct SpriteTex * tex;
    void (* dest_func)(struct Sprite *);
    void (* tick_func)(u8 *, struct Sprite *);
    void (* disp_func)(struct Sprite *);
    undefined2 bmp;
    undefined field_0x3e[0x2];
    float width;
    float height;
    float depth;
    s32 para1; /* Arbitrary int param 1 */
    s32 para2; /* Arbitrary int param 2 */
    s32 para3; /* Arbitrary int param 3 */
    f32 fpara1; /* Arbitrary float param 1 */
    f32 fpara2; /* Arbitrary float param 2 */
    f32 fpara3; /* Arbitrary float param 3 */
    struct Sprite * prev_sprite;
    struct Sprite * next_sprite;
    undefined4 field45_0x6c;
    undefined4 field46_0x70;
    undefined4 field47_0x74;
    undefined4 field48_0x78;
    undefined field_0x7c[0x4];
    float alpha; /* called trnsl in game? */
    struct Rgb24 add_color;
    undefined field_0x87[0x1];
    u32 g_flags1;
    u32 g_flags2;
    f32 u1;
    f32 v1;
    f32 u2;
    f32 v2;
    char text[48]; /* If this sprite displays text, this is what it shows, otherwise this is usually just an identifier name */
} __attribute__((__packed__));

struct DVDCommandBlock {
    struct DVDCommandBlock * next;
    struct DVDCommandBlock * prev;
    u32 command;
    s32 state;
    u32 offset;
    u32 length;
    void * addr;
    u32 currTransferSize;
    u32 transferredSize;
    struct DVDDiskID * id;
    void (* callback)(s32, struct DVDCommandBlock *);
    void * userData;
} __attribute__((__packed__));

struct DVDFileInfo { /* Not entirely sure about this one... I've filled in some fields that only exist within `#ifdef MACOS` blocks according to the GC SDK on my end. Leaving them out seems wrong if I look at DVDFastOpen. Also I haven't put *all* MACOS fields in there since I don't know the size of IOParam (from a Mac header). */
    struct DVDCommandBlock cb;
    u32 startAddr;
    u32 length;
    struct DVDFileInfo * next;
    void (* callback)(s32, struct DVDFileInfo *);
} __attribute__((__packed__));

struct DVDDiskID {
    char gameName[4];
    char company[2];
    u8 diskNumber;
    u8 gameVersion;
    u8 streaming;
    u8 streamingBufSize; /* 0 = default */
    u8 padding[22]; /* 0's are stored */
} __attribute__((__packed__));

struct SpriteTex {
    u8 field0_0x0;
    s8 field1_0x1;
    u16 field2_0x2;
    s32 tex_index;
    struct GXTexObj texobj;
    void * tex_data;
    u32 tex_data_size;
    u16 width;
    u16 height;
    GXTexFmt  format;
    OSHeapHandle heap;
    struct DVDFileInfo dvd_file;
} __attribute__((__packed__));

typedef struct HeapConfig HeapConfig, *PHeapConfig;

struct HeapConfig { /* Set of sizes for game heaps */
    u32 stage_heap_size;
    u32 bg_heap_size;
    u32 chara_heap_size;
    u32 replay_heap_size;
    u32 flags;
} __attribute__((__packed__));

typedef struct GSomethingWithPadMotorsStruct GSomethingWithPadMotorsStruct, *PGSomethingWithPadMotorsStruct;

struct GSomethingWithPadMotorsStruct {
    undefined1 a;
    undefined field_0x1[0x1];
    undefined2 b;
} __attribute__((__packed__));

typedef struct SpriteDrawRequest SpriteDrawRequest, *PSpriteDrawRequest;

enum {
    SDRF_G_SCREENFADE_RELATED=262144,
    SDRF_FLIP_X=524288,
    SDRF_FLIP_Y=1048576,
    SDRF_G_ID_FORMAT_RELATED=4194304,
    SDRF_G_X_SCALE_RELATED=16777216
};
typedef undefined4 SpriteDrawReqFlags;

struct SpriteDrawRequest { /* Used by Sprite disp() functions to render a texture on the screen, sometimes multiple times per disp() call to render multiple things on-screen per Sprite object */
    s32 id; /* At least some of the time: bits 16-24 are category, 24-31 are id in the category. Sometimes used as just an ID with no category? */
    struct Vec pos;
    struct Vec2d scale;
    float u1; /* First texture coordinate, U component */
    float v1; /* First texture coordinate, V component */
    float u2; /* Second texture coordinate, U component */
    float v2; /* Second texture coordinate, V component */
    s32 rot_z; /* For some reason this is 32-bit instead of s16? */
    float alpha;
    s32 g_unk1;
    SpriteDrawReqFlags  flags;
    u32 mult_color; /* RGB24 multiply blend color */
    u32 add_color; /* RGB24 add blend color */
    s16 g_some_x_value;
    undefined field_0x42[0xe];
} __attribute__((__packed__));

typedef struct CmPlayerProgress CmPlayerProgress, *PCmPlayerProgress;

typedef struct CmStage CmStage, *PCmStage;

struct CmStage { /* Stage in Challenge Mode. Stage course number is what's shown in bottom left of screen */
    s32 stage_course_num;
    s32 stage_id;
} __attribute__((__packed__));

struct CmPlayerProgress { /* Seems to be one of these per player, not sure what they are exactly yet */
    struct CmStage curr_stage;
    struct CmStage next_stages[3]; /* Appears to track the "next" stage for the player in the first element (assuming a blue goal is taken). The latter two elements are never updated, and I don't think the first element is even used for anything */
    s16 field2_0x20;
    s16 g_next_stage_idx;
} __attribute__((__packed__));

enum { /* These are normally just #defines in the SDK's PAD library. Also these are supposed to be signed */
    PAD_ERR_NONE=0,
    PAD_ERR_TRANSFER=253,
    PAD_ERR_NOT_READY=254,
    PAD_ERR_NO_CONTROLLER=255
};
typedef undefined1 PadError;

typedef struct GoalBag GoalBag, *PGoalBag;

typedef struct Stobj Stobj, *PStobj;

typedef struct PhysicsBall PhysicsBall, *PPhysicsBall;

enum {
    STOBJ_BUMPER=0,
    STOBJ_JAMABAR=1,
    STOBJ_GOALTAPE=2,
    STOBJ_GOALBAG=3,
    STOBJ_GOALBAG_EXMASTER=4,
    STOBJ_MF_PNL_BUMPER=5,
    STOBJ_MF_PNL_ELECTRAP=6,
    STOBJ_MF_BULLET_TEST=7,
    STOBJ_MF_BOX=8,
    STOBJ_BUMPER_BGSPECIAL=9,
    STOBJ_NAMEENT_BTN=10,
    STOBJ_BUTTON=11,
    STOBJ_RETURNGATE=12
};
typedef undefined2 StobjType;

struct GoalBag { /* Extra goalbag-specific state pointed to by goalbag StageObjects */
    undefined field_0x0[0x4];
    undefined4 field4_0x4;
    undefined4 field5_0x8;
    struct Stobj * stobj; /* Created by retype action */
    struct Vec g_pos;
    undefined field_0x1c[0x8];
    undefined4 field16_0x24;
} __attribute__((__packed__));

struct PhysicsBall { /* A representation of a Ball with just the physics/collision-related info */
    dword flags;
    struct Vec pos;
    struct Vec prev_pos;
    struct Vec vel;
    float radius;
    float acceleration;
    float restitution;
    dword g_jerk;
    undefined field_0x38[0xc];
    struct Vec g_some_vec;
    undefined field_0x50[0x4];
    dword field25_0x54;
    float field26_0x58;
    dword itemgroup_idx; /* The itemgroup that this PhysicsBall is relative to, aka in the local space of */
} __attribute__((__packed__));

struct Stobj { /* A "stage object" which is one of a: bumper, jamabar, goaltape, party ball, wormhole, and others. */
    word idx;
    short id;
    StobjType  type;
    undefined field_0x6[0x2];
    uint g_some_bitflag;
    short g_mode;
    short g_counter;
    struct Vec g_model_origin;
    struct Vec position;
    struct Vec position_2; /* Copy of position? */
    float bounding_sphere_radius; /* Has something to do w/ collision */
    void (* coli_func)(struct Stobj *, struct PhysicsBall *);
    struct Vec scale;
    float field14_0x48;
    float field15_0x4c;
    float field16_0x50;
    struct GmaModelHeader * g_visual_model;
    struct Vec g_some_position; /* Has something to do w/ position */
    struct Vec velocity;
    struct S16Vec rot;
    short field21_0x76;
    short field22_0x78;
    undefined field_0x7a[0x2];
    struct Vec g_prev_pos;
    struct S16Vec g_prev_rot;
    undefined field_0x8e[0x2];
    float field29_0x90;
    float field30_0x94;
    float field31_0x98;
    float field32_0x9c;
    s8 itemgroup_idx;
    undefined field_0xa1[0x3];
    void * extra_data; /* Extra stobj-type-specific data, such as switch stagedef header for switches or goaltape struct for goaltapes. Maybe worth making a union */
    struct Vec g_some_pos2;
    struct Vec g_local_position;
    struct Vec g_local_velocity;
} __attribute__((__packed__));

typedef struct SeesawInfo SeesawInfo, *PSeesawInfo;

struct SeesawInfo { /* Allocated on the heap for an itemgroup if it's a seesaw. Points to another struct also allocated on the heap that contains physics state info like current rotation angle. */
    short itemgroup_idx;
    word g_type; /* Seems to always be zero, but the way it's set seems to indicate there may have been more than one type of seesaw at some point */
    struct SeesawState * state;
    void * init_func;
    void * dest_func;
    void * tick_func;
    void * coli_func;
    void * g_some_func5;
    void * g_replay_func;
} __attribute__((__packed__));

enum {
    DATA_SELECT=0,
    STAGE_SELECT_PRE_INTRO_SEQUENCE=1,
    STAGE_SELECT_INTRO_SEQUENCE=2,
    STAGE_SELECT_IDLE=4,
    STAGE_SELECTED=6
};
typedef undefined2 StoryModeStageSelectState;

typedef struct GCachedFileEntry GCachedFileEntry, *PGCachedFileEntry;

struct GCachedFileEntry {
    undefined field_0x0[0x4];
    int dvd_entrynum;
    struct DVDCommandBlock * next; /* Created by retype action */
    struct DVDCommandBlock * prev;
} __attribute__((__packed__));

enum {
    MF_NONE=0,
    MF_0x1=1,
    MF_0x2=2,
    MF_0x4=4,
    MF_PLAYING_EXTRA_COURSE=8,
    MF_PLAYING_MASTER_NOEX_COURSE=16,
    MF_0x20=32,
    MF_ADDITIONAL_REL_LOADED=512,
    MF_0x2000=8192,
    MF_OPTION_MODE=262144,
    MF_G_STOP_GAME_LOOP=2097152,
    MF_0x400000=4194304,
    MF_PLAYING_MASTER_EX_COURSE=8388608,
    MF_G_PLAYING_MASTER_COURSE=33554432
};
typedef undefined4 ModeFlag;

typedef struct GSomeSoundStruct GSomeSoundStruct, *PGSomeSoundStruct;

struct GSomeSoundStruct {
    short g_sfx_id;
    short g_sfx_rid;
    char field2_0x4;
    byte g_sfx_pan;
    char g_sfx_span;
    char g_sfx_grp;
    int g_player_id;
} __attribute__((__packed__));

typedef struct GSomeSpriteStruct GSomeSpriteStruct, *PGSomeSpriteStruct;

struct GSomeSpriteStruct {
    struct Sprite * g_some_sprite;
    struct GSomeSpriteStruct * g_prev;
    struct GSomeSpriteStruct * g_next;
} __attribute__((__packed__));

typedef struct Item Item, *PItem;

enum {
    ITEM_COIN=0,
    ITEM_FGT_BANANA=1,
    ITEM_MINI_RACE=2,
    ITEM_FGT_POWER=3,
    ITEM_FGT_PUNCH=4,
    ITEM_PILOT=5,
    ITEM_DOGFIGHT=6,
    ITEM_TMP001=7,
    ITEM_TMP002=8,
    ITEM_TMP003=9,
    ITEM_TMP004=10,
    ITEM_TMP005=11,
    ITEM_TMP006=12,
    ITEM_TMP007=13,
    ITEM_TMP008=14,
    ITEM_TMP009=15,
    ITEM_TMP010=16,
    ITEM_TMP011=17,
    ITEM_TMP012=18,
    ITEM_TMP013=19,
    ITEM_TMP014=20,
    ITEM_TMP015=21,
    ITEM_TMP016=22
};
typedef undefined2 ItemType;

struct Item { /* Represents an item that can be picked up by the player. These are bananas in main game, but can be other pickups in the party games. Are "tickable" like events, effects, etc. */
    word index;
    short id;
    ItemType  type;
    s16 coin_type;
    u32 g_some_bitfield;
    undefined2 field5_0xc;
    undefined2 g_some_flag;
    short field7_0x10;
    undefined2 g_some_flag2;
    float scale;
    float field10_0x18;
    undefined * g_something_with_gma_model;
    struct Vec position;
    struct Vec velocity;
    struct S16Vec rotation;
    struct S16Vec angular_velocity;
    struct Vec g_position_copy;
    struct S16Vec g_rotation_copy;
    undefined field_0x56[0x2];
    void (* item_coli_func)(struct Item *, struct PhysicsBall *); /* Created by retype action */
    u8 itemgroup_idx;
    undefined field_0x5d[0x1];
    s16 g_some_frame_counter;
    struct Vec * g_some_vec3f_ptr;
    undefined4 field25_0x64;
    struct GmaModelHeader * model_ptr;
    float field27_0x6c;
    s16 negative_y_rotation;
    undefined field_0x72[0x2];
    float field31_0x74;
    float g_something_with_shadow_disp;
    struct Vec shadow_scale;
    float shadow_intensity;
    struct Vec g_position_copy_2;
    u32 g_some_flag_2;
    struct Vec shadow_position;
    struct Vec g_something_with_shadows;
} __attribute__((__packed__));

enum { /* Background music tracks by ID */
    BGM_SEL_WORLD=0,
    BGM_CAVE=1,
    BGM_SEL_FUTSAL=2,
    BGM_FUTSAL=3,
    BGM_WATER=4,
    BGM_SEL_DEF=5,
    BGM_NABE=6,
    BGM_JUNGLE=7,
    BGM_BOAT=8,
    BGM_BILI=9,
    BGM_SEL_BOWLING=10,
    BGM_SEL_GOLF=11,
    BGM_CONTINUE=12,
    BGM_OVER=13,
    BGM_FIGHT=14,
    BGM_SEL_FIGHT=15,
    BGM_COLONY=16,
    BGM_BILL_FOUL=17,
    BGM_BILL_LOSE=18,
    BGM_BILL_SENKOU=19,
    BGM_BILL_WIN=20,
    BGM_BOWL_MISS=21,
    BGM_BOWL_NORMAL=22,
    BGM_BOWL_PERFECT=23,
    BGM_BOWL_SPARE=24,
    BGM_BOWL_STRIKE=25,
    BGM_BOWL_TURKEY=26,
    BGM_FIGHT_WIN=27,
    BGM_GOLF_BIRDIE=28,
    BGM_GOLF_BOGIE=29,
    BGM_GOLF_GIVEUP1=30,
    BGM_GOLF_HOLEIN1=31,
    BGM_GOLF_OB=32,
    BGM_GOLF_PAR=33,
    BGM_RACE_LOSE=34,
    BGM_RACE_WIN=35,
    BGM_TAG_MISS=36,
    BGM_TAG_SCORE_HI=37,
    BGM_TAG_SCORE_NML=38,
    BGM_TAG_SCORE_SP=39,
    BGM_AVLOGO=40,
    BGM_10MIN1=41,
    BGM_BONUS=42,
    BGM_BOWLING=43,
    BGM_10MIN2=44,
    BGM_SEL_BASEBALL=45,
    BGM_SEL_BOAT=46,
    BGM_SEL_TARGET=47,
    BGM_ENTRY=48,
    BGM_HYOSHO=49,
    BGM_ROLL=50,
    BGM_S1A_SNG1=51,
    BGM_S1A_SNG2=52,
    BGM_S1A_SNG3=53,
    BGM_TITLE=54,
    BGM_S1A_SNG4=55,
    BGM_FIGHT_VIEW=56,
    BGM_GOLF_VIEW=57,
    BGM_SF=58,
    BGM_SEL_TENNIS=59,
    BGM_TARGET=60,
    BGM_YUENCHI=61,
    BGM_SEL_RACE=62,
    BGM_SENTAKU=63,
    BGM_KUJIRA=64,
    BGM_RACE=65,
    BGM_GOLF=66,
    BGM_SEL_BILI=67,
    BGM_GREP=68,
    BGM_RACE_ADVAN=69,
    BGM_CLOCKTOWER=70,
    BGM_BOAT_ADVAN=71,
    BGM_SEL_SHOT=72,
    BGM_RACE_EXPERT=73,
    BGM_SEL_DOG=74,
    BGM_BOAT_EXPERT=75,
    BGM_BOAT_LOSE=76,
    BGM_BOAT_WIN=77,
    BGM_SHOT=78,
    BGM_SHOT_CLEAR=79,
    BGM_SHOT_LOSE=80,
    BGM_DOG=81,
    BGM_SHOT_ADVAN=82,
    BGM_SHOT_BOSS=83,
    BGM_DOG_WIN=84,
    BGM_DOG_LOSE=85,
    BGM_FUTSAL_GOAL=86,
    BGM_POINT=87,
    BGM_SHOT_EXPERT=88,
    BGM_GOLF_EAGLE=89,
    BGM_SEL_CLEAR=90,
    BGM_SCEN_ENDING=91,
    BGM_S3_SNG1=92,
    BGM_S3_SNG2=93,
    BGM_S3_SNG3=94,
    BGM_ADV=95,
    BGM_S3_SNG4=96,
    BGM_S3_SNG5=97,
    BGM_S3_SNG6=98,
    BGM_S3_SNG7=99,
    BGM_S3_SNG8=100,
    BGM_SCEN_ENDING4=101,
    BGM_S1B_SNG1=102,
    BGM_S1B_SNG2=103,
    BGM_S1B_SNG3=104,
    BGM_S2_SNG1=105,
    BGM_S2_SNG2=106,
    BGM_S2_SNG3=107,
    BGM_S2_SNG4=108,
    BGM_S2_SNG5=109,
    BGM_S2_SNG6=110,
    BGM_S2_SNG7=111,
    BGM_S2_SNG8=112,
    BGM_S4_SNG1=113,
    BGM_S4_SNG2=114,
    BGM_S4_SNG3=115,
    BGM_S4_SNG4=116,
    BGM_S4_SNG5=117,
    BGM_S4_SNG6=118,
    BGM_S4_SNG7=119,
    BGM_S6_SNG1=120,
    BGM_S6_SNG2=121,
    BGM_S6_SNG3=122,
    BGM_S5_SNG1=123,
    BGM_S5_SNG2=124,
    BGM_S5_SNG3=125,
    BGM_S5_SNG4=126,
    BGM_S5_SNG5=127,
    BGM_S5_SNG6=128,
    BGM_S5_SNG7=129,
    BGM_S7_SNG1=130,
    BGM_S7_SNG2=131,
    BGM_S7_SNG3=132,
    BGM_S7_SNG4=133,
    BGM_S7_SNG5=134,
    BGM_S7_SNG6=135,
    BGM_SLAST_SNG1=136,
    BGM_SLAST_SNG2=137,
    BGM_S8_SNG1=138,
    BGM_S8_SNG2=139,
    BGM_S9_SNG1=140,
    BGM_S9_SNG2=141,
    BGM_S10_SNG1=142,
    BGM_S10_SNG2=143,
    BGM_S10_SNG3=144,
    BGM_S10_SNG4=145,
    BGM_S10_SNG5=146
};
typedef undefined2 BgmTrack;

typedef struct RelBufferInfo RelBufferInfo, *PRelBufferInfo;

struct RelBufferInfo {
    void * rel_buffer;
    void * bss_buffer;
} __attribute__((__packed__));

typedef struct UnlockInfo UnlockInfo, *PUnlockInfo;

struct UnlockInfo { /* Info about which stuff in the game is unlocked, such as master, number of monkeys, party games, etc. Which story mode / challenge mode stages are unlocked are stored separately however. */
    byte master_unlocked;
    byte monkeys; /* Number of monkeys (lives) unlocked, aka the max you can use in challenge mode */
    undefined field_0x2[0x1];
    byte staff_credits_game_unlocked;
    dword play_points; /* Seems to function as "current number of play points" when you don't have everything unlocked, and "highest play points record" when you do */
    dword newest_play_point_record;
    undefined field_0xc[0x2];
    word movies; /* One bit for each of the 12 movies */
    dword party_games; /* Bitfield; normally 0x0001b600 when all are unlocked, and not zero when only base games are unlocked */
    word g_movies_watched; /* Determines whether unlocked movies are grayed out; does this mean "movie watched"? */
    undefined field_0x16[0x6e];
} __attribute__((__packed__));

typedef struct Ball Ball, *PBall;

enum {
    PHYS_MODE_G_GOALED=5,
    PHYS_MODE_SLOWDOWN_BLASTOFF=6,
    PHYS_MODE_NORMAL=32
};
typedef undefined1 BallMode;

enum { /* Flags that mostly affect ball physics and controls */
    PHYS_NONE=0,
    PHYS_ON_GROUND=1,
    PHYS_UNK_0x2=2,
    PHYS_UNK_0x4=4,
    PHYS_UNK_0x8=8,
    PHYS_UNK_0x10=16,
    PHYS_UNK_0x20=32,
    PHYS_UNK_0x40=64,
    PHYS_UNK_0x80=128,
    PHYS_DISABLE_GRAVITY=256,
    PHYS_BLAST_UP=512,
    PHYS_SLOW_DOWN=1024,
    PHYS_G_DISABLE_GOAL_DETECTION=2048,
    PHYS_DISABLE_CONTROLS=4096,
    PHYS_G_SECRET_POSTGOAL_DANCE=8192,
    PHYS_UNK_0x4000=16384
};
typedef undefined4 BallPhysFlags;

typedef struct RaycastHit RaycastHit, *PRaycastHit;

struct RaycastHit {
    u16 geom_flags; /* Bit 0 is set if the hit is valid. Also OR-d with some flags from the tri/sphere/whatever the line trace hit */
    undefined field_0x2[0x2];
    struct Vec pos; /* Position of ray-geometry intersection */
    struct Vec normal; /* Geometry normal at point of ray-geometry intersection */
} __attribute__((__packed__));

struct Ball {
    Status  status; /* Actually called just "STAT" in the debug menu */
    undefined field_0x1[0x1];
    u8 monkey_count; /* Life counter */
    BallMode  mode; /* Ball's mode, which determines how its physics behave */
    struct Vec pos;
    struct Vec prev_pos;
    struct Vec vel; /* Velocity/speed */
    struct S16Vec visual_rot;
    u8 idx; /* The index of the ball in the ball pool, aka 0 for the first ball, 1 for the second */
    undefined field_0x2f[0x1];
    Mtx model_transform;
    struct S16Vec visual_rot_vel;
    short padding2;
    float ball_size;
    float gravity;
    float restitution;
    float visual_scale;
    s32 banana_count;
    s32 score;
    s32 frames_since_spawn;
    undefined field_0x84[0xe];
    s16 g_monkey_angle; /* Seems to be related to the direction the monkey is facing */
    undefined field_0x94[0x4];
    u32 g_effect_flags;
    BallPhysFlags  phys_flags; /* Some more flags related to ball state? The lowest-order bit may represent "is ball touching the ground" and I believe if affects the physics */
    struct Quat g_monkey_rotation; /* Rotation of the monkey inside the ball? */
    struct Quat g_ball_rotation; /* Rotation of the ball itself? */
    struct Vec some_vec3;
    float some_length;
    Mtx ball_transform_copy;
    float speed;
    struct Ape * ape;
    int field48_0x108;
    struct Vec ape_facedir_point; /* The point of interest that the monkey looks at (goal, banana, etc) */
    float something_with_ape_facedir; /* Approaches 1 the closer you are to the point of interest */
    struct Vec g_last_collision_normal; /* Maybe inverse of the normal of the last triangle collided with? */
    undefined field_0x128[0x4];
    dword g_race_flags;
    short g_other_counter;
    undefined2 field58_0x132;
    s16 field59_0x134;
    s16 g_something_timer; /* Created by retype action */
    undefined field_0x138[0x4];
    float hardest_coli_speed;
    int g_banana_count_copy;
    undefined field_0x144[0x8];
    float physical_ball_radius;
    undefined field_0x150[0x4];
    undefined1 field80_0x154;
    undefined1 field81_0x155;
    u8 g_ball_color_index; /* Ball color by player index: 0 red, 1 blue, 2 yellow, 3 green, etc */
    undefined1 field83_0x157;
    undefined field_0x158[0x2];
    short g_some_counter;
    struct Vec g_some_pos1;
    undefined field_0x168[0x10];
    u8 g_some_game_flag; /* Some values make the ball return to the start position */
    u8 g_some_model_flag; /* Changes the ball model to lower poly variants, also apparantly the Dole blimp? */
    undefined field_0x17a[0x2];
    struct RaycastHit raycast_down_hit; /* Result of raycast downward from ball's origin with the stage */
    undefined field_0x198[0x18];
} __attribute__((__packed__));

typedef struct Itemgroup Itemgroup, *PItemgroup;

struct Itemgroup { /* Contains the current animation-related state of each item group in a stage (each thing corresponding to a collision header in the stagedef) */
    dword playback_state; /* Corresponding to the switch playback type which is controlling the item group, see PlaybackState */
    dword anim_frame;
    struct Vec position;
    struct Vec prev_position;
    struct S16Vec rotation;
    struct S16Vec prev_rotation;
    Mtx transform;
    Mtx prev_transform;
    undefined field_0x8c[0x10];
    struct SeesawInfo * seesaw_info;
} __attribute__((__packed__));

typedef struct StoryModeSaveFile StoryModeSaveFile, *PStoryModeSaveFile;

struct StoryModeSaveFile {
    undefined field0_0x0[4];
    BOOL32 is_valid;
    char file_name[13];
    u8 current_world;
    u8 field4_0x16;
    undefined field_0x17[0x1];
    u32 playtime_in_frames;
    u32 score;
    u32 bananas;
    u8 num_beaten_stages_in_current_world;
    u8 beaten_stage_indices[10];
    undefined field11_0x2f[85];
} __attribute__((__packed__));

typedef struct GBone GBone, *PGBone;

struct GBone {
    struct SKLFile * SKLData;
    void * MTAData;
    void * UnknownPtr;
} __attribute__((__packed__));

typedef struct RankingEntry RankingEntry, *PRankingEntry;

struct RankingEntry {
    char * name;
    undefined field_0x4[0x4];
    u32 score; /* Created by retype action */
    u32 g_ape_id;
    undefined field_0x10[0x4];
    undefined4 field11_0x14;
    undefined field_0x18[0x4];
} __attribute__((__packed__));

typedef struct GTableEntry GTableEntry, *PGTableEntry;

struct GTableEntry {
    undefined field_0x0[0x3];
    byte field3_0x3;
    undefined field_0x4[0x8];
} __attribute__((__packed__));

typedef struct OptiGXChanSettings OptiGXChanSettings, *POptiGXChanSettings;

typedef u8 GXBool;

enum {
    GX_SRC_REG=0,
    GX_SRC_VTX=1
};
typedef undefined4 GXColorSrc;

enum {
    GX_DF_NONE=0,
    GX_DF_SIGN=1,
    GX_DF_CLAMP=2
};
typedef undefined4 GXDiffuseFn;

enum {
    GX_AF_SPEC=0,
    GX_AF_SPOT=1,
    GX_AF_NONE=2
};
typedef undefined4 GXAttnFn;

struct OptiGXChanSettings { /* Opti = For optimization */
    GXBool enable;
    undefined padding_0x1[0x3];
    GXColorSrc  amb_src;
    GXColorSrc  mat_src;
    u32 light_mask;
    GXDiffuseFn  diff_fn;
    GXAttnFn  attn_fn;
} __attribute__((__packed__));

typedef struct CmListEntry CmListEntry, *PCmListEntry;

struct CmListEntry {
    undefined4 field0_0x0;
    undefined4 g_stage_id;
    undefined field_0x8[0x64];
} __attribute__((__packed__));

typedef struct GraphicsInfo GraphicsInfo, *PGraphicsInfo;

typedef struct GXFifoObj GXFifoObj, *PGXFifoObj;

struct GraphicsInfo { /* Is this the right size for the struct? maybe? -Crafted */
    void * active_framebuffer; /* A pointer to either framebuffer1 or framebuffer2 */
    void * framebuffer1;
    void * framebuffer2;
    u32 frame_count;
    u32 active_framebuffer_index; /* Either 0 or 1, representing framebuffer1 or framebuffer2 */
    struct GXFifoObj * fifos[2];
} __attribute__((__packed__));

struct GXFifoObj {
    undefined field_0x0[0x14];
    void * read_ptr;
    void * write_ptr;
    undefined field_0x1c[0x64];
} __attribute__((__packed__));

enum {
    FONT32_ASCII=0,
    FONT32_ASC_8x16=1,
    FONT32_ASC_12x12=2,
    FONT32_ASC_24x24=3,
    FONT32_ASC_16x16P=4,
    FONT32_ASC_16x16=5,
    FONT32_DMY03=6,
    FONT32_DMY04=7,
    FONT32_DMY05=8,
    FONT32_ASC_72x64=9,
    FONT32_DMY06=10,
    FONT32_DMY07=11,
    FONT32_DMY08=12,
    FONT32_DMY09=13,
    FONT32_DMY10=14,
    FONT32_DMY11=15,
    FONT32_DMY12=16,
    FONT32_DMY13=17,
    FONT32_DMY14=18,
    FONT32_DMY15=19,
    FONT32_DMY16=20,
    FONT32_ICON_SD=21,
    FONT32_ICON_SD2=22,
    FONT32_DMY20=23,
    FONT32_DMY21=24,
    FONT32_DMY22=25,
    FONT32_DMY23=26,
    FONT32_DMY_RNK00=27,
    FONT32_DMY_RNK01=28,
    FONT32_DMY_RNK02=29,
    FONT32_DMY_RNK03=30,
    FONT32_DMY_RNK04=31,
    FONT32_RNK_32x32=32,
    FONT32_RNK_NUM=33,
    FONT32_MINI_RNK=34,
    FONT32_SCORE_NUM=35,
    FONT32_DMY_RNK0=36,
    FONT32_DMY_RNK1=37,
    FONT32_DMY_RNK2=38,
    FONT32_DMY_RNK3=39,
    FONT32_DMY_RNK4=40,
    FONT32_DMY_RNK5=41,
    FONT32_DMY_RNK6=42,
    FONT32_DMY_RNK7=43,
    FONT32_DMY_RNK8=44,
    FONT32_DMY_RNK9=45,
    FONT32_DMY_RNKa=46,
    FONT32_DMY_RNKb=47,
    FONT32_DMY_RNKc=48,
    FONT32_DMY_RNKd=49,
    FONT32_DMY_RNKe=50,
    FONT32_DMY_RNKf=51,
    FONT32_SEL_CTRL_PORT=52,
    FONT32_SEL_CTRL_R=53,
    FONT32_SEL_CTRL_B=54,
    FONT32_SEL_CTRL_Y=55,
    FONT32_SEL_CTRL_G=56,
    FONT32_SEL_CTRL_SDW=57,
    FONT32_DMY42=58,
    FONT32_DMY43=59,
    FONT32_BWL_SCORE=60,
    FONT32_DMY50=61,
    FONT32_DMY51=62,
    FONT32_DMY52=63,
    FONT32_DMY53=64,
    FONT32_DMY54=65,
    FONT32_DMY55=66,
    FONT32_DMY56=67,
    FONT32_DMY57=68,
    FONT32_RAC_DMY01=69,
    FONT32_RAC_PLAYER=70,
    FONT32_RAC_RANK=71,
    FONT32_RAC_LAP_MARK_NUM=72,
    FONT32_RAC_LAP_NUM=73,
    FONT32_RAC_TIME_NUM=74,
    FONT32_RAC_LAP_MARK_NUM_S=75,
    FONT32_RAC_DMY08=76,
    FONT32_RAC_DMY09=77,
    FONT32_RAC_DMY10=78,
    FONT32_RAC_SPD_NUM=79,
    FONT32_RAC_DMY12=80,
    FONT32_RAC_DMY13=81,
    FONT32_TGT_PLAYER=82,
    FONT32_TGT_MSCORE=83,
    FONT32_TGT_SCORE=84,
    FONT32_TGT_SCORE_S=85,
    FONT32_TGT_SPEED=86,
    FONT32_TGT_ROUND=87,
    FONT32_TGT_ALT=88,
    FONT32_TGT_WIND=89,
    FONT32_DMY71=90,
    FONT32_DMY72=91,
    FONT32_DMY73=92,
    FONT32_NUM_NML_SCORE=93,
    FONT32_NUM_NML_TIME=94,
    FONT32_NUM_NML_TIME_S=95,
    FONT32_NUM_NML_SPEED=96,
    FONT32_NUM_NML_SARU=97,
    FONT32_ASC_NML_D5=98,
    FONT32_ASC_NML_D6=99,
    FONT32_ASC_NML_D7=100,
    FONT32_ASC_NML_D8=101,
    FONT32_ASC_NML_D9=102,
    FONT32_DMY79=103,
    FONT32_DMY7a=104,
    FONT32_DMY7b=105,
    FONT32_DMY7c=106,
    FONT32_DMY7d=107,
    FONT32_DMY7e=108,
    FONT32_DMY7f=109,
    FONT32_BOA_TIME_NUM=110,
    FONT32_BOA_DMY02=111,
    FONT32_BOA_RANK=112,
    FONT32_BOA_PLAYER=113,
    FONT32_BOA_SPD_NUM=114,
    FONT32_BOA_LAP_MARK_NUM=115,
    FONT32_BOA_LAP_MARK_NUM_S=116,
    FONT32_BOA_DMY08=117,
    FONT32_BOA_DMY09=118,
    FONT32_BOA_DMY10=119,
    FONT32_BOA_LAP_NUM=120,
    FONT32_BOA_DMY12=121,
    FONT32_BOA_DMY13=122,
    FONT32_BOA_DMY14=123,
    FONT32_BOA_DMY15=124,
    FONT32_BOA_DMY16=125,
    FONT32_DOG_SPEED=126,
    FONT32_DOG_SPEED_S=127,
    FONT32_DOG_MSCORE=128,
    FONT32_DOG_SCORE=129,
    FONT32_DOG_SCORE_M=130,
    FONT32_DOG_SCORE_S=131,
    FONT32_DOG_ROUND=132,
    FONT32_DOG_ALT=133,
    FONT32_DOG_RESULT=134,
    FONT32_DOG_TIME=135,
    FONT32_DOG_DMY02=136,
    FONT32_DOG_DMY03=137,
    FONT32_DOG_DMY04=138,
    FONT32_DOG_DMY05=139,
    FONT32_JAP_TAG=140,
    FONT32_JAP_DMY=141,
    FONT32_JAP_24x24_2=142,
    FONT32_JAP_24x24_2P=143,
    FONT32_JAP_24x24_I=144
};
typedef undefined4 Font32;

typedef struct g_someGameInfo g_someGameInfo, *Pg_someGameInfo;

struct g_someGameInfo {
    undefined field_0x0[0x50];
} __attribute__((__packed__));

enum {
    DMD_SCEN_1ST_INIT=0,
    DMD_SCEN_RETURN_INIT=1,
    DMD_SCEN_NEWGAME_INIT=2,
    DMD_SCEN_NEWGAME_MAIN=3,
    DMD_SCEN_LOADGAME_INIT=4,
    DMD_SCEN_LOADGAME_MAIN=5,
    DMD_SCEN_SCENEPLAY_INIT=6,
    DMD_SCEN_SCENEPLAY_MAIN=7,
    DMD_SCEN_SEL_WORLD_INIT=8,
    DMD_SCEN_SEL_WORLD_NEXT=9,
    DMD_SCEN_SEL_FLOOR_INIT=10,
    DMD_SCEN_SEL_FLOOR_MAIN=11,
    DMD_SCEN_GAME_INIT=12,
    DMD_SCEN_GAME_MAIN=13,
    DMD_SCEN_GAME_CLEAR_INIT=14,
    DMD_SCEN_GAME_CLEAR_MAIN=15,
    DMD_SCEN_GAME_OVER_INIT=16,
    DMD_SCEN_GAME_OVER_MAIN=17,
    DMD_SCEN_SELECT_INIT=18,
    DMD_SCEN_SELECT_MAIN=19,
    DMD_SCEN_ENTRY_INIT=20,
    DMD_SCEN_ENTRY_MAIN=21,
    DMD_SCEN_SCNPLAY_RETURN=22,
    DMD_SCEN_SCNPLAY_RETURN_LOADING=23,
    DMD_SCEN_LOADGAME_RESTORE=24,
    DMD_SCEN_NAMEENTRY_INIT=25,
    DMD_SCEN_NAMEENTRY_MAIN=26
};
typedef undefined2 StoryModeFunc;

typedef struct MemCardInfo MemCardInfo, *PMemCardInfo;

struct MemCardInfo { /* Some struct that seems to hold per-memcard info; there are two statically allocated, one for each memory card slot (I think) */
    byte chan; /* Channel aka slot, 0 -> slot A, 1 -> slot B */
    byte field1_0x1;
    byte field2_0x2;
    undefined field_0x3[0x5];
    word field8_0x8;
    word field9_0xa;
    dword field10_0xc;
    undefined field_0x10[0x14];
    dword field31_0x24;
    undefined field_0x28[0x8];
    dword field40_0x30;
    dword field41_0x34;
    dword field42_0x38;
    dword field43_0x3c;
    dword field44_0x40;
} __attribute__((__packed__));

typedef struct GSomeFileStruct GSomeFileStruct, *PGSomeFileStruct;

struct GSomeFileStruct {
    BOOL32 dvd_entrynum;
    struct DVDFileInfo dvdFileInfo;
} __attribute__((__packed__));

typedef struct PadStatusGroup PadStatusGroup, *PPadStatusGroup;

typedef struct PADStatus PADStatus, *PPADStatus;

enum { /* These are normally just #defines in the SDK's PAD library */
    PAD_BUTTON_LEFT=1,
    PAD_BUTTON_RIGHT=2,
    PAD_BUTTON_DOWN=4,
    PAD_BUTTON_UP=8,
    PAD_TRIGGER_Z=16,
    PAD_TRIGGER_R=32,
    PAD_TRIGGER_L=64,
    PAD_BUTTON_A=256,
    PAD_BUTTON_B=512,
    PAD_BUTTON_X=1024,
    PAD_BUTTON_Y=2048,
    PAD_BUTTON_START=4096
};
typedef undefined2 PadDigitalInput;

struct PADStatus {
    PadDigitalInput  button; /* Or-ed PAD_BUTTON_* and PAD_TRIGGER_* bits */
    s8 stickX; /* -128 <= stickX       <= 127 */
    s8 stickY; /* -128 <= stickY       <= 127 */
    s8 substickX; /* -128 <= substickX    <= 127 */
    s8 substickY; /* -128 <= substickY    <= 127 */
    u8 triggerLeft; /*    0 <= triggerLeft  <= 255 */
    u8 triggerRight; /*    0 <= triggerRight <= 255 */
    u8 analogA; /*    0 <= analogA      <= 255 */
    u8 analogB; /*    0 <= analogB      <= 255 */
    PadError  err; /* one of PAD_ERR_* number */
    undefined field_0xb[0x1];
} __attribute__((__packed__));

struct PadStatusGroup { /* A set of PADStatus structs for a given controller, with each representing a different "filtered" version of the inputs */
    struct PADStatus raw; /* The raw PADStatus read by PADRead() for the controller */
    struct PADStatus prev_tick; /* The PADStatus from the previous frame */
    struct PADStatus pressed; /* PADStatus representing the digital inputs that were just pressed this frame */
    struct PADStatus released; /* PADStatus representing the digital inputs that were released this frame */
    struct PADStatus repeated; /* PADStatus of digital inputs that "repeat" similar to holding a key down on a keyboard: bit is high on first press, then there's a pause of many frames, then the bit is high every 4 frames */
} __attribute__((__packed__));

typedef struct DigitalInputGroup DigitalInputGroup, *PDigitalInputGroup;

struct DigitalInputGroup { /* Consolidated bitfields for digital button inputs corresponding to the five PadStatusGroup types */
    PadDigitalInput  raw;
    PadDigitalInput  prev_tick;
    PadDigitalInput  pressed;
    PadDigitalInput  released;
    PadDigitalInput  repeated;
} __attribute__((__packed__));

typedef struct AnalogInputGroup AnalogInputGroup, *PAnalogInputGroup;

enum { /* Thresholded analog input bits used by SMB2 */
    PAI_LSTICK_LEFT=1,
    PAI_LSTICK_RIGHT=2,
    PAI_LSTICK_DOWN=4,
    PAI_LSTICK_UP=8,
    PAI_CSTICK_LEFT=16,
    PAI_CSTICK_RIGHT=32,
    PAI_CSTICK_DOWN=64,
    PAI_CSTICK_UP=128,
    PAI_LTRIG=256,
    PAI_RTRIG=512
};
typedef undefined2 PadAnalogInput;

struct AnalogInputGroup { /* Consolidated thresholded analog input bitfields corresponding to the five groups of PadStatusGroup */
    PadAnalogInput  raw;
    PadAnalogInput  prev_tick;
    PadAnalogInput  pressed;
    PadAnalogInput  released;
    PadAnalogInput  repeated;
} __attribute__((__packed__));

typedef struct CARDStat CARDStat, *PCARDStat;

struct CARDStat {
    char fileName[32];
    u32 length;
    u32 time;
    u8 gameName[4];
    u8 company[2];
    u8 bannerFormat;
    u8 field6_0x2f;
    u32 iconAddr;
    u16 iconFormat;
    u16 iconSpeed;
    u32 commentAddr;
    u32 offsetBanner;
    u32 offsetBannerTlut;
    u32 offsetIcon[8];
    u32 offsetIconTlut;
    u32 offsetData;
} __attribute__((__packed__));

typedef struct MemCardFile MemCardFile, *PMemCardFile;

typedef struct CARDFileInfo CARDFileInfo, *PCARDFileInfo;

struct CARDFileInfo {
    s32 chan;
    s32 fileNo;
    s32 offset;
    s32 length;
    u16 iBlock;
} __attribute__((__packed__));

struct MemCardFile {
    s32 exi_channel_number;
    undefined field_0x4[0xc];
    struct CARDFileInfo gc_file_info; /* Created by retype action */
    undefined field_0x22[0x2];
    char * file_name; /* Struct may be bigger? /shrug */
} __attribute__((__packed__));

typedef struct ModeInfo ModeInfo, *PModeInfo;

enum {
    BALLMODE_NONE=0,
    BALLMODE_GOALED=1,
    BALLMODE_OUT_OF_TIME_RINGOUT=2,
    BALLMODE_FALLEN_OUT=4,
    BALLMODE_FREEZE_TIMER=8,
    BALLMODE_IN_REPLAY=16,
    BALLMODE_IN_GOAL_ANIMATION=32,
    BALLMODE_ON_BONUS_STAGE=64,
    BALLMODE_IN_STAGE_LOADIN=256,
    BALLMODE_CLEARED_BONUS_PERFECT=512,
    BALLMODE_CLEARED_BONUS_BONUSFINISH_UNUSED=1024,
    BALLMODE_IN_TUTORIAL_SEQUENCE=2048,
    BALLMODE_ON_FINAL_STAGE=4096,
    BALLMODE_OUT_OF_TIME_CONTINUE=8192
};
typedef undefined4 G_BallMode;

enum {
    Blue=0,
    Green=1,
    Red=2
};
typedef undefined1 GoalType;

struct ModeInfo { /* I don't know what to call this, but there's some important global game info in here! -Complex */
    G_BallMode  g_ball_mode; /* Correlates with the ball's 'mode' in the debug menu's ball display. Bonus stages have 0x40 set, final stages in a difficulty have 0x1000 set.  0x8 seems to stop the timer? -Crafted */
    s16 stage_time_frames_remaining;
    undefined2 stage_time_limit;
    undefined4 field3_0x8;
    s16 entered_goal_idx;
    undefined2 field5_0xe;
    struct Vec g_ballVelAtGoal;
    undefined2 g_some_timer_frame_remaining_count;
    undefined2 field8_0x1e;
    undefined2 cm_course_stage_num; /* Current course stage num, updated immediately after completing stage */
    undefined2 g_some_stage_jump_distance;
    undefined4 bananas_remaining;
    undefined2 field12_0x28;
    undefined2 field13_0x2a;
    undefined2 g_next_stage_id2;
    s16 cm_stage_id; /* Current challenge mode stage id, updated immediately after finishing stage */
    undefined2 field16_0x30;
    undefined2 cm_next_stage_id;
    s16 g_selected_world_idx;
    undefined2 g_selected_world_stage_idx;
    GoalType  entered_goal_type;
    undefined field_0x39[0x3];
} __attribute__((__packed__));

typedef struct Event Event, *PEvent;

struct Event { /* A subset of continuously running game functionality that can be turned on or off independent of other Events */
    Status  status;
    undefined padding_0x1[0x3];
    char * name; /* The name of the event */
    void (* init_func)(void); /* Normally, called once when status is STAT_INIT, then status is set to STAT_NORMAL */
    void (* tick_func)(void); /* Normally, called every frame if status is STAT_NORMAL */
    void (* dest_func)(void); /* "destruct:" or "destroy" function; normally, called once when status is STAT_DEST, then status is set to STAT_NULL */
    u32 tick_time; /* For performance metrics (presumably this is what's seen in the debug overlay performance > event menu) */
} __attribute__((__packed__));

typedef unsigned short    wchar16;
enum {
    EVENT_STAGE=0,
    EVENT_WORLD=1,
    EVENT_BALL=2,
    EVENT_APE=3,
    EVENT_STOBJ=4,
    EVENT_ITEM=5,
    EVENT_RECPLAY=6,
    EVENT_OBJ_COLLISION=7,
    EVENT_NAME_ENTRY=8,
    EVENT_INFO=9,
    EVENT_COURSE=10,
    EVENT_VIBRATION=11,
    EVENT_COMMEND=12,
    EVENT_VIEW=13,
    EVENT_EFFECT=14,
    EVENT_MINIMAP=15,
    EVENT_CAMERA=16,
    EVENT_SPRITE=17,
    EVENT_MOUSE=18,
    EVENT_SOUND=19,
    EVENT_BACKGROUND=20,
    EVENT_REND_EFC=21,
    EVENT_ADX=22,
    EVENT_NONE=4294967295
};
typedef undefined4 EventID;

enum {
    BANANA_SINGLE=0,
    BANANA_BUNCH=1
};
typedef undefined4 BananaType;

enum {
    DATA_SELECT_MENU=0,
    NAME_ENTRY_MENU=1,
    STAGE_SELECT_MENU=2,
    RETURN_TO_MAIN_MENU=3
};
typedef undefined1 StoryModeMenuState;

typedef struct __OutStrCtrl __OutStrCtrl, *P__OutStrCtrl;

typedef ulong size_t;

struct __OutStrCtrl {
    char * CharStr;
    size_t MaxCharCount;
    size_t CharsWritten;
} __attribute__((__packed__));

typedef struct OptiGXSettings OptiGXSettings, *POptiGXSettings;

struct OptiGXSettings { /* Opti = For optimization - I don't actually know how big this struct is -Crafted */
    undefined field_0x0[0xc30];
    struct OptiGXChanSettings chan_color0;
    struct OptiGXChanSettings chan_color1;
    struct OptiGXChanSettings chan_alpha0;
    struct OptiGXChanSettings chan_alpha1;
} __attribute__((__packed__));

typedef struct S32Vec S32Vec, *PS32Vec;

struct S32Vec {
    s32 x;
    s32 y;
} __attribute__((__packed__));

enum {
    OF_G_SMTH_WITH_CAMERA=2,
    OF_GAME_PAUSED=8
};
typedef undefined4 GOtherFlags;

typedef struct Map Map, *PMap;

struct Map {
    int size;
    undefined4 field1_0x4;
    undefined field_0x8[0x4];
} __attribute__((__packed__));

typedef struct gDiscQueueItem gDiscQueueItem, *PgDiscQueueItem;

struct gDiscQueueItem {
    int status;
    int entrynum;
    undefined4 group;
} __attribute__((__packed__));

enum {
    MINIMAP_HIDDEN=0,
    MINIMAP_SHRINK=1,
    MINIMAP_SHOWN=2,
    MINIMAP_EXPAND=3,
    MINIMAP_INIT=4
};
typedef undefined4 MinimapMode;

typedef struct DipSwitchesOld DipSwitchesOld, *PDipSwitchesOld;

struct DipSwitchesOld { /* Unused struct - maybe I'll use it once Ghidra supports bitfields a bit nicer in the decompilation */
    u32 DIP_DEBUG:1; /* Enables debug mode */
    u32 DIP_DISP:1;
    u32 DIP_STCOLI:1; /* Displays the level's collision and out of bounds areas */
    u32 DIP_TRIANGLE:1; /* Replaces the level model with a triangle */
    u32 DIP_TAIKEN:1;
    u32 DIP_TIME_STOP:1; /* Freezes the timer at 60 seconds */
    u32 DIP_NAMEENTRY:1;
    u32 DIP_FIX_WORLD:1; /* Disables stage tilting */
    u32 DIP_TEST_CAM:1; /* Free cam during gameplay */
    u32 DIP_NO_INTR:1;
    u32 DIP_CAPTURE:1;
    u32 DIP_PERF_ALWAYS:1; /* Related to the SDK's PERF profiling */
    u32 DIP_PLAY_STG_ALL:1; /* Unlock all */
    u32 DIP_PLAY_PNT_x10:1; /* 10x play point gain */
    u32 DIP_SARU_0:1;
    u32 DIP_SWITCH15:1;
    u32 DIP_ONLY24MB:1;
    u32 DIP_SHADOW_DISP:1; /* Display object shadows on the UI */
    u32 DIP_SPRITE_OFF:1; /* Hide most 2D graphics */
    u32 DIP_SNDREQ_DISP:1;
    u32 DIP_SE_VOL_RESET:1; /* Disable most sound effects */
    u32 DIP_APE_FACEDIR:1; /* Shows what the monkey is looking at */
    u32 DIP_SEL_ALL_CTRL:1; /* Control all 4 players at once with 1 controller */
    u32 DIP_APE_NUMBER:1; /* Display character id above each monkey's head */
    u32 DIP_1P_TEST_CAM:1;
    u32 DIP_PREVIEW_SAVE:1; /* Display a preview in the upper left of the screen */
    u32 DIP_AUTHOR_DEBUG:1; /* Show debug info during cutscenes */
    u32 DIP_SWITCH27:1;
    u32 DIP_SOA:1;
    u32 DIP_TEST_TEAM:1;
    u32 DIP_NO_MINIMAP:1; /* Hide minimap */
    u32 DIP_NO_STAGE:1; /* Hide stage */
} __attribute__((__packed__));

enum {
    LOCALE_ENGLISH=0,
    LOCALE_GERMAN=1,
    LOCALE_FRENCH=2,
    LOCALE_SPANISH=3,
    LOCALE_ITALIAN=4,
    LOCALE_JAPANESE=5
};
typedef undefined4 Locale;

enum { /* Abbreviated "ET" in the game */
    EFFECT_PAPERFRAG=0,
    EFFECT_GET_BANANA=1,
    EFFECT_COLI_PARTICLE=2,
    EFFECT_HOLDING_BANANA=3,
    EFFECT_LEVITATE=4,
    EFFECT_COLISTAR_PARTICLE=5,
    EFFECT_BGWAT_BUBBLE_BASE=6,
    EFFECT_BGWAT_BUBBLE=7,
    EFFECT_METEO=8,
    EFFECT_METEO_FIX=9,
    EFFECT_COLIFLASH=10,
    EFFECT_BNS_STG_STAR=11,
    EFFECT_BNS_STG_STAR_TAIL=12,
    EFFECT_BGMST_GEN_CLOUD=13,
    EFFECT_BGSTM_RAINRIPPLE=14,
    EFFECT_NAMEENT_CODE=15,
    EFFECT_GET_NAMEENT_CODE=16,
    EFFECT_BGMST_WATER=17,
    EFFECT_COMMENDFRAG=18,
    EFFECT_ENDING_BALLFRAG=19,
    EFFECT_BANANA_DROP=20,
    EFFECT_BGEND_WATER=21,
    EFFECT_BGBTMFIRE_PTCL=22,
    EFFECT_PIYO=23,
    EFFECT_DUMMY1=24,
    EFFECT_DUMMY2=25,
    EFFECT_DUMMY3=26,
    EFFECT_DUMMY4=27,
    EFFECT_DUMMY5=28,
    EFFECT_DUMMY6=29,
    EFFECT_DUMMY7=30,
    EFFECT_DUMMY8=31,
    EFFECT_DUMMY9=32,
    EFFECT_DUMMYA=33,
    EFFECT_DUMMYB=34,
    EFFECT_DUMMYC=35,
    EFFECT_DUMMYD=36,
    EFFECT_DUMMYE=37,
    EFFECT_DUMMYF=38,
    EFFECT_TMP001=39,
    EFFECT_TMP002=40,
    EFFECT_TMP003=41,
    EFFECT_TMP004=42,
    EFFECT_TMP005=43,
    EFFECT_TMP006=44,
    EFFECT_TMP007=45,
    EFFECT_TMP008=46,
    EFFECT_TMP009=47,
    EFFECT_TMP010=48,
    EFFECT_TMP011=49,
    EFFECT_TMP012=50,
    EFFECT_TMP013=51,
    EFFECT_TMP014=52,
    EFFECT_TMP015=53,
    EFFECT_TMP016=54,
    EFFECT_TMP017=55,
    EFFECT_TMP018=56,
    EFFECT_TMP019=57,
    EFFECT_TMP020=58,
    EFFECT_TMP021=59,
    EFFECT_TMP022=60,
    EFFECT_TMP023=61,
    EFFECT_TMP024=62,
    EFFECT_TMP025=63,
    EFFECT_TMP026=64,
    EFFECT_TMP027=65,
    EFFECT_TMP028=66,
    EFFECT_TMP029=67,
    EFFECT_TMP030=68,
    EFFECT_TMP031=69,
    EFFECT_TMP032=70
};
typedef undefined2 EffectType;

enum {
    GX_USAGE_NONE=0,
    GX_USAGE_LOCKED_CACHE_ENABLED=2,
    GX_USAGE_HAS_SECOND_FIFO=4
};
typedef undefined4 GxUsageState;

enum {
    DSMS_DEFAULT=0,
    DSMS_CHOOSE_DATA_TO_COPY=1,
    DSMS_CHOOSE_WHERE_TO_COPY_TO=2,
    DSMS_CONFIRM_COPY=3,
    DSMS_CANCELLED_COPY=4,
    DSMS_COPY_DATA=5,
    DSMS_CHOOSE_DATA_TO_DELETE=6,
    DSMS_CONFIRM_DELETE=7,
    DSMS_CANCELLED_DELETE=8,
    DSMS_DELETE_DATA=9,
    DSMS_OPEN_DATA=10,
    DSMS_RETURN_SELECTED=11,
    DSMS_RETURN_TO_MAIN_GAME_SELECT=12
};
typedef undefined1 DataSelectMenuState;

enum {
    EFFECT_NULL=0,
    EFFECT_CAST_SHADOW=1,
    EFFECT_RECEIVE_SHADOW=2,
    EFFECT_UNKNOWN_3=4,
    EFFECT_TRANSPARENCY_A=8,
    EFFECT_TRANSPARENCY_B=16,
    EFFECT_UNKNOWN_6=32,
    EFFECT_UNKNOWN_7=64,
    EFFECT_UNKNOWN_8=128
};
typedef undefined4 StageModelEffectBitfield;

typedef struct Effect Effect, *PEffect;

struct Effect {
    word g_idx;
    short id;
    undefined field_0x4[0x4];
    EffectType  type;
    undefined field_0xa[0x2];
    s32 field9_0xc;
    undefined field_0x10[0x4];
    undefined2 g_ball_idx;
    s16 field15_0x16;
    undefined field_0x18[0xc];
    float field28_0x24;
    undefined4 field29_0x28;
    float field30_0x2c;
    s32 g_pointer_to_some_struct;
    struct Vec g_pos;
    struct Vec g_some_vec;
    struct S16Vec g_some_rot;
    undefined field_0x52[0x6];
    struct Vec g_prev_pos;
    undefined field_0x64[0x28];
    struct Vec g_some_vec2;
    struct Vec g_some_vec3;
    undefined field_0xa4[0xc];
} __attribute__((__packed__));

enum {
    PMT_UNKNOWN0=0,
    PMT_CHALLENGE=1,
    PMT_PRACTICE=2,
    PMT_UNKNOWN3=3,
    PMT_UNKNOWN4=4,
    PMT_STORY_STAGE_SELECT=5,
    PMT_STORY_PLAY=6,
    PMT_UNKNOWN7=7
};
typedef undefined4 PauseMenuType;

typedef struct ytgut ytgut, *Pytgut;

struct ytgut {
    undefined field_0x0[0x4];
} __attribute__((__packed__));

typedef struct PoolInfo PoolInfo, *PPoolInfo;

struct PoolInfo { /* Metadata and status info for lists of "tickable" objects like the sprite, effect, stobj, and item lists */
    dword len; /* The size of the object pool, aka the maximum number of non-empty objects the pool can hold */
    dword low_free_idx; /* The last known "low index" of an empty object in the pool. Reset at the start of the frame to the lowest free idx in the list, and increments as objects are allocated */
    dword upper_bound; /* Number of objects from the start of the list until the last non-empty object. Usually the list is iterated over from 0 to this value, checking each tickable if it's non-empty. Reset at the start of each frame to one past the last non-empty object in the pool, and increased if the low idx bumps into it */
    u8 * status_list; /* Byte array of same length as the pool, each byte corresponds to some status of the corresponding object. Usually either 0 for "empty slot" and either 1 or 2 for "active". This does not appear to be a Status like Events use. */
} __attribute__((__packed__));

enum {
    PIID_LEFT=0,
    PIID_RIGHT=1,
    PIID_UP=2,
    PIID_DOWN=3,
    PIID_L=4,
    PIID_R=5,
    PIID_A=6,
    PIID_B=7,
    PIID_X=8
};
typedef undefined4 PadInputID;

enum {
    PLAYBACK_FORWARD=0,
    PLAYBACK_PAUSE=1,
    PLAYBACK_BACKWARD=2,
    PLAYBACK_FAST_FORWARD=3,
    PLAYBACK_FAST_BACKWARD=4
};
typedef undefined2 PlaybackState;

typedef void _IO_lock_t;

typedef struct _IO_marker _IO_marker, *P_IO_marker;

typedef struct _IO_FILE _IO_FILE, *P_IO_FILE;

typedef long __off_t;

struct _IO_marker {
    struct _IO_marker * _next;
    struct _IO_FILE * _sbuf;
    int _pos;
} __attribute__((__packed__));

struct _IO_FILE {
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
    struct _IO_marker * _markers;
    struct _IO_FILE * _chain;
    int _fileno;
    int _flags2;
    __off_t _old_offset;
    ushort _cur_column;
    char _vtable_offset;
    char _shortbuf[1];
    _IO_lock_t * _lock;
    void * __pad1;
    void * __pad2;
    void * __pad3;
    void * __pad4;
    size_t __pad5;
    int _mode;
    char _unused2[15];
    undefined padding_0x73[0x1];
} __attribute__((__packed__));

typedef double f64;

#define _M_IX86 500

#define _USE_ATTRIBUTES_FOR_SAL 0

#define _WIN32_WINNT 1024

#define _USE_DECLSPECS_FOR_SAL 0

#define _WIN32_WINDOWS 1024

#define WINVER 1536

#define _MSC_VER 9090

#define _INTEGRAL_MAX_BITS 32

typedef struct _IO_FILE __FILE;

typedef struct _IO_FILE FILE;

typedef int bool_t;

typedef signed char int8_t;

typedef int int32_t;

typedef struct gSceneData gSceneData, *PgSceneData;

struct gSceneData {
    char world_theme;
    char g_anim_flag;
    char ape_count;
    undefined field_0x3[0x5];
    char LOD;
    char unknown;
    char scene_item_count;
    undefined field_0xb[0x1];
    char * * field12_0xc;
} __attribute__((__packed__));

typedef struct StagedefBackgroundAnimHeader StagedefBackgroundAnimHeader, *PStagedefBackgroundAnimHeader;

typedef struct StagedefAnimKeyframe StagedefAnimKeyframe, *PStagedefAnimKeyframe;

enum {
    EASING_CONSTANT=0,
    EASING_LINEAR=1,
    EASING_SMOOTH=2
};
typedef undefined4 Easing;

struct StagedefBackgroundAnimHeader {
    undefined field_0x0[0x4];
    float loop_point_seconds;
    undefined field_0x8[0x8];
    u32 rot_x_keyframe_count;
    struct StagedefAnimKeyframe * rot_x_keyframe_list;
    u32 rot_y_keyframe_count;
    struct StagedefAnimKeyframe * rot_y_keyframe_list;
    u32 rot_z_keyframe_count;
    struct StagedefAnimKeyframe * rot_z_keyframe_list;
    u32 pos_x_keyframe_count;
    struct StagedefAnimKeyframe * pos_x_keyframe_list;
    u32 pos_y_keyframe_count;
    struct StagedefAnimKeyframe * pos_y_keyframe_list;
    u32 pos_z_keyframe_count;
    struct StagedefAnimKeyframe * pos_z_keyframe_list;
    undefined field_0x40[0x10];
} __attribute__((__packed__));

struct StagedefAnimKeyframe {
    Easing  easing;
    float time;
    float value;
    undefined field_0xc[0x8];
} __attribute__((__packed__));

typedef struct StagedefEffectHeader StagedefEffectHeader, *PStagedefEffectHeader;

typedef struct StagedefEffect1 StagedefEffect1, *PStagedefEffect1;

typedef struct StagedefEffect2 StagedefEffect2, *PStagedefEffect2;

typedef struct StagedefTextureScroll StagedefTextureScroll, *PStagedefTextureScroll;

struct StagedefEffectHeader {
    u32 fx1_keyframe_count;
    struct StagedefEffect1 * fx1_keyframe_list;
    u32 fx2_keyframe_count;
    struct StagedefEffect2 * fx2_keyframe_list;
    struct StagedefTextureScroll * texture_scroll;
    undefined field_0x14[0x1c];
} __attribute__((__packed__));

struct StagedefEffect1 { /* Likely used for animated textures, such as the silhouettes in Night */
    dword field0_0x0;
    dword field1_0x4;
    dword field2_0x8;
    word field3_0xc;
    word field4_0xe;
    word field5_0x10;
    undefined field_0x12[0x2];
} __attribute__((__packed__));

struct StagedefEffect2 { /* May be used for the flames in Storm? */
    float field0_0x0;
    float field1_0x4;
    float field2_0x8;
    undefined field_0xc[0x4];
} __attribute__((__packed__));

struct StagedefTextureScroll {
    struct Vec2d speed;
} __attribute__((__packed__));

typedef struct StagedefFogAnimHeader StagedefFogAnimHeader, *PStagedefFogAnimHeader;

struct StagedefFogAnimHeader {
    u32 start_distance_keyframe_count;
    struct StagedefAnimKeyframe * start_distance_keyframe_list;
    u32 end_distance_keyframe_count;
    struct StagedefAnimKeyframe * end_distance_keyframe_list;
    u32 red_keyframe_count;
    struct StagedefAnimKeyframe * red_keyframe_list;
    u32 green_keyframe_count;
    struct StagedefAnimKeyframe * green_keyframe_list;
    u32 blue_keyframe_count;
    struct StagedefAnimKeyframe * blue_keyframe_list;
    u32 unk_keyframe_count;
    struct StagedefAnimKeyframe * unk_keyframe_list;
} __attribute__((__packed__));

typedef struct StagedefAnimHeader StagedefAnimHeader, *PStagedefAnimHeader;

struct StagedefAnimHeader {
    u32 rot_x_keyframe_count;
    struct StagedefAnimKeyframe * rot_x_keyframe_list;
    u32 rot_y_keyframe_count;
    struct StagedefAnimKeyframe * rot_y_keyframe_list;
    u32 rot_z_keyframe_count;
    struct StagedefAnimKeyframe * rot_z_keyframe_list;
    u32 pos_x_keyframe_count;
    struct StagedefAnimKeyframe * pos_x_keyframe_list;
    u32 pos_y_keyframe_count;
    struct StagedefAnimKeyframe * pos_y_keyframe_list;
    u32 pos_z_keyframe_count;
    struct StagedefAnimKeyframe * pos_z_keyframe_list;
    undefined field_0x30[0x10];
} __attribute__((__packed__));

typedef struct StagedefFileHeader StagedefFileHeader, *PStagedefFileHeader;

typedef struct StagedefColiHeader StagedefColiHeader, *PStagedefColiHeader;

typedef struct StagedefStart StagedefStart, *PStagedefStart;

typedef struct StagedefFallout StagedefFallout, *PStagedefFallout;

typedef struct StagedefGoal StagedefGoal, *PStagedefGoal;

typedef struct StagedefBumper StagedefBumper, *PStagedefBumper;

typedef struct StagedefJamabar StagedefJamabar, *PStagedefJamabar;

typedef struct StagedefBanana StagedefBanana, *PStagedefBanana;

typedef struct StagedefColiCone StagedefColiCone, *PStagedefColiCone;

typedef struct StagedefColiSphere StagedefColiSphere, *PStagedefColiSphere;

typedef struct StagedefColiCylinder StagedefColiCylinder, *PStagedefColiCylinder;

typedef struct StagedefFalloutVolume StagedefFalloutVolume, *PStagedefFalloutVolume;

typedef struct StagedefBackgroundModel StagedefBackgroundModel, *PStagedefBackgroundModel;

typedef struct StagedefForegroundModel StagedefForegroundModel, *PStagedefForegroundModel;

typedef struct StagedefReflectiveStageModel StagedefReflectiveStageModel, *PStagedefReflectiveStageModel;

typedef struct StagedefStageModelInstance StagedefStageModelInstance, *PStagedefStageModelInstance;

typedef struct StagedefStageModelPtrA StagedefStageModelPtrA, *PStagedefStageModelPtrA;

typedef struct StagedefStageModelPtrB StagedefStageModelPtrB, *PStagedefStageModelPtrB;

typedef struct StagedefButton StagedefButton, *PStagedefButton;

typedef struct StagedefWormhole StagedefWormhole, *PStagedefWormhole;

typedef struct StagedefFog StagedefFog, *PStagedefFog;

typedef struct StagedefDynamicReflectionPlane StagedefDynamicReflectionPlane, *PStagedefDynamicReflectionPlane;

typedef struct StagedefMystery3 StagedefMystery3, *PStagedefMystery3;

enum {
    ANIM_LOOP=0,
    ANIM_PLAY_ONCE=1,
    ANIM_SEESAW=2
};
typedef undefined2 StagedefAnimType;

typedef struct StagedefColiTri StagedefColiTri, *PStagedefColiTri;

typedef struct StagedefMystery5 StagedefMystery5, *PStagedefMystery5;

typedef struct StagedefBackgroundAnim2Header StagedefBackgroundAnim2Header, *PStagedefBackgroundAnim2Header;

typedef struct StagedefStageModel StagedefStageModel, *PStagedefStageModel;

enum {
    GX_FOG_NONE=0,
    GX_FOG_LIN=2,
    GX_FOX_EXP=4,
    GX_FOG_EXP2=5,
    GX_FOG_REVEXP=6,
    GX_FOG_REVEXP2=7
};
typedef undefined1 GXFogType;

struct StagedefStageModelInstance {
    struct StagedefStageModelPtrA * stage_model_a;
    struct Vec position;
    struct S16Vec rotation;
    undefined2 g_not_padding;
    struct Vec scale;
} __attribute__((__packed__));

struct StagedefBumper {
    struct Vec position;
    struct S16Vec rotation;
    undefined2 padding;
    struct Vec scale;
} __attribute__((__packed__));

struct StagedefReflectiveStageModel {
    char * model_name;
    struct GmaModelHeader * g_model_header_ptr;
    undefined field_0x8[0x4];
} __attribute__((__packed__));

struct StagedefFallout {
    float y;
} __attribute__((__packed__));

struct StagedefButton {
    struct Vec position;
    struct S16Vec rotation;
    PlaybackState  playback_state;
    u16 anim_group_id;
    undefined2 padding;
} __attribute__((__packed__));

struct StagedefDynamicReflectionPlane {
    char * model_name_ptr;
    struct Vec pos;
    struct S16Vec rot;
} __attribute__((__packed__));

struct StagedefStageModel {
    undefined field_0x0[0x4];
    char * model_name;
    uint some_flag;
    undefined field_0xc[0x4];
} __attribute__((__packed__));

struct StagedefColiSphere {
    struct Vec position;
    float radius;
    undefined4 g_not_padding; /* Nullable */
} __attribute__((__packed__));

struct StagedefColiCylinder {
    struct Vec position;
    float radius;
    float height;
    struct S16Vec rotation;
    undefined2 g_not_padding;
} __attribute__((__packed__));

struct StagedefBackgroundModel {
    undefined field_0x0[0x4];
    char * model_name;
    struct GmaModelHeader * gma_model; /* Created by retype action */
    struct Vec position;
    struct S16Vec rotation;
    undefined2 padding;
    struct Vec scale;
    struct StagedefBackgroundAnimHeader * background_anim_header;
    struct StagedefBackgroundAnim2Header * background_anim2_header;
    struct StagedefEffectHeader * effect_header;
} __attribute__((__packed__));

struct StagedefStageModelPtrA {
    StageModelEffectBitfield  some_effect_bitflag;
    uint some_counter;
    struct StagedefStageModel * stage_model;
} __attribute__((__packed__));

struct StagedefStageModelPtrB {
    struct StagedefStageModelPtrA * stage_model_a;
} __attribute__((__packed__));

struct StagedefBanana {
    struct Vec position;
    BananaType  type;
} __attribute__((__packed__));

struct StagedefMystery5 {
    undefined field_0x0[0x4];
    float field4_0x4;
    float field5_0x8;
    float field6_0xc;
    float field7_0x10;
} __attribute__((__packed__));

struct StagedefColiCone {
    struct Vec position;
    struct S16Vec rotation;
    undefined2 g_not_padding;
    struct Vec scale;
} __attribute__((__packed__));

struct StagedefJamabar {
    struct Vec position;
    struct S16Vec rotation;
    undefined2 padding;
    struct Vec scale;
} __attribute__((__packed__));

struct StagedefMystery3 {
    float field0_0x0;
    float field1_0x4;
    float field2_0x8;
    undefined2 field3_0xc;
    undefined2 field4_0xe;
    undefined field_0x10[0x14];
} __attribute__((__packed__));

struct StagedefFalloutVolume {
    struct Vec position;
    struct Vec size;
    struct S16Vec rotation;
    undefined2 padding;
} __attribute__((__packed__));

struct StagedefColiTri {
    struct Vec vert1;
    struct Vec normal;
    struct S16Vec rot_from_xy;
    undefined2 flags;
    struct Vec2d vert2_delta; /* Before rotation is applied */
    struct Vec2d vert3_delta;
    struct Vec2d tangent;
    struct Vec2d bitangent;
} __attribute__((__packed__));

struct StagedefGoal {
    struct Vec position;
    struct S16Vec rotation;
    GoalType  type;
    undefined field_0x13[0x1];
} __attribute__((__packed__));

struct StagedefWormhole {
    undefined field_0x0[0x4];
    struct Vec positon;
    struct S16Vec rotation;
    undefined2 padding;
    struct StagedefWormhole * destination;
} __attribute__((__packed__));

struct StagedefBackgroundAnim2Header {
    undefined field_0x0[0x4];
    float loop_point_seconds;
    u32 unk1_keyframe_count;
    struct StagedefAnimKeyframe * unk1_keyframe_list;
    u32 unk2_keyframe_count;
    struct StagedefAnimKeyframe * unk2_keyframe_list;
    u32 rot_x_keyframe_count;
    struct StagedefAnimKeyframe * rot_x_keyframe_list;
    u32 rot_y_keyframe_count;
    struct StagedefAnimKeyframe * rot_y_keyframe_list;
    u32 rot_z_keyframe_count;
    struct StagedefAnimKeyframe * rot_z_keyframe_list;
    u32 pos_x_keyframe_count;
    struct StagedefAnimKeyframe * pos_x_keyframe_list;
    u32 pos_y_keyframe_count;
    struct StagedefAnimKeyframe * pos_y_keyframe_list;
    u32 pos_z_keyframe_count;
    struct StagedefAnimKeyframe * pos_z_keyframe_list;
    u32 unk9_keyframe_count;
    struct StagedefAnimKeyframe * unk9_keyframe_list;
    u32 unk10_keyframe_count;
    struct StagedefAnimKeyframe * unk10_keyframe_list;
    u32 unk11_keyframe_count;
    struct StagedefAnimKeyframe * unk11_keyframe_list;
} __attribute__((__packed__));

struct StagedefStart {
    struct Vec position;
    struct S16Vec rotation;
    undefined2 padding;
} __attribute__((__packed__));

struct StagedefFileHeader {
    u32 magic_number_a;
    u32 magic_number_b;
    u32 coli_header_count;
    struct StagedefColiHeader * coli_header_list;
    struct StagedefStart * start;
    struct StagedefFallout * fallout;
    u32 goal_count;
    struct StagedefGoal * goal_list;
    u32 bumper_count;
    struct StagedefBumper * bumper_list;
    u32 jamabar_count;
    struct StagedefJamabar * jamabar_list;
    u32 banana_count;
    struct StagedefBanana * banana_list;
    u32 coli_cone_count;
    struct StagedefColiCone * coli_cone_list;
    u32 coli_sphere_count;
    struct StagedefColiSphere * coli_sphere_list;
    u32 coli_cylinder_count;
    struct StagedefColiCylinder * coli_cylinder_list;
    u32 fallout_volume_count;
    struct StagedefFalloutVolume * fallout_volume_list;
    u32 background_model_count;
    struct StagedefBackgroundModel * background_model_list;
    u32 foreground_model_count;
    struct StagedefForegroundModel * foreground_model_list;
    dword field26_0x68;
    dword field27_0x6c;
    u32 reflective_stage_model_count;
    struct StagedefReflectiveStageModel * reflective_stage_model_list;
    undefined field_0x78[0xc];
    u32 stage_model_instance_count;
    struct StagedefStageModelInstance * stage_model_instance_list;
    u32 stage_model_a_count;
    struct StagedefStageModelPtrA * stage_model_a_list;
    u32 stage_model_b_count;
    struct StagedefStageModelPtrB * stage_model_b_list;
    undefined field_0x9c[0xc];
    u32 button_count;
    struct StagedefButton * button_list;
    struct StagedefFogAnimHeader * fog_animation; /* Nullable */
    u32 wormhole_count;
    struct StagedefWormhole * wormhole_list;
    struct StagedefFog * fog; /* Nullable */
    undefined field_0xc0[0x8];
    struct StagedefDynamicReflectionPlane * dynamic_reflection_plane;
    undefined field_0xcc[0x8];
    struct StagedefMystery3 * mystery3;
    undefined field_0xd8[0x7c4];
} __attribute__((__packed__));

struct StagedefFog {
    GXFogType  type;
    undefined field_0x1[0x3];
    float fog_start_distance;
    float fog_end_distance;
    struct Vec color;
    undefined field_0x18[0xc];
} __attribute__((__packed__));

struct StagedefForegroundModel {
    undefined4 field0_0x0; /* Unknown - typically 0x0000001F, sometimes 0x00000007 or 0x0000000F */
    char * model_name;
    struct GmaModelHeader * gma_model;
    struct Vec position;
    struct S16Vec rotation;
    undefined2 padding;
    struct Vec scale;
    undefined4 field7_0x2c;
    void * background_anim2_header;
    void * field9_0x34;
} __attribute__((__packed__));

struct StagedefColiHeader {
    struct Vec origin; /* Center of rotation etc. */
    struct S16Vec initial_rotation;
    StagedefAnimType  anim_loop_type_and_seesaw;
    struct StagedefAnimHeader * animation_header;
    struct Vec conveyor_speed;
    struct StagedefColiTri * coli_tri_list;
    s16 * * coli_tri_idxs;
    f32 coli_grid_start_x;
    f32 coli_grid_start_z;
    f32 coli_grid_step_x;
    f32 coli_grid_step_z;
    u32 coli_grid_cells_x;
    u32 coli_grid_cells_z;
    u32 goal_count;
    struct StagedefGoal * goal_list;
    u32 bumper_count;
    struct StagedefBumper * bumper_list;
    u32 jamabar_count;
    struct StagedefJamabar * jamabar_list;
    u32 banana_count;
    struct StagedefBanana * banana_list;
    u32 cone_collision_object_count;
    struct StagedefColiCone * cone_collision_object_list;
    u32 sphere_collision_object_count;
    struct StagedefColiSphere * sphere_collision_object_list;
    u32 cylinder_collision_object_count;
    struct StagedefColiCylinder * cylinder_collision_object_list;
    u32 fallout_volume_count;
    struct StagedefFalloutVolume * fallout_volume_list;
    u32 reflective_stage_model_count;
    struct StagedefReflectiveStageModel * reflective_stage_model_list;
    u32 stage_model_instance_count;
    struct StagedefStageModelInstance * stage_model_instance_list;
    u32 stage_model_b_count;
    struct StagedefStageModelPtrB * stage_model_b_list;
    undefined field_0x9c[0x8];
    u16 anim_group_id; /* For use with buttons - Must be non-zero if you want to assign a group ID */
    undefined2 g_not_padding;
    u32 button_count;
    struct StagedefButton * button_list;
    undefined field_0xb0[0x4];
    struct StagedefMystery5 * mystery5;
    float seesaw_sensitivity; /* Higher is more sensitive, negative makes the seesaw reversed */
    float seesaw_friction; /* Lower is looser */
    float seesaw_spring; /* 0 prevents the seesaw from resetting */
    u32 wormhole_count;
    struct StagedefWormhole * wormhole_list;
    u32 initial_playback_state; /* Should this be split into 2x padding bytes + PlaybackState enum? */
    undefined field_0xd0[0x4];
    float anim_loop_point_seconds;
    struct StagedefTextureScroll * texture_scroll;
    undefined field_0xdc[0x3c0];
} __attribute__((__packed__));

typedef void * __gnuc_va_list;

typedef __gnuc_va_list va_list;

typedef struct GmaVertexControlHeader GmaVertexControlHeader, *PGmaVertexControlHeader;

struct GmaVertexControlHeader { /* Also called "Model Type 1". This structure appears to outline various information relating to skinned and effective models only. A such, it is assumed that it is data for controlling the vertices. */
    s32 vertex_count;
    s32 vertex_control_type1_array_offset;
    s32 vertex_control_type2_array_offset;
    s32 vertex_control_type3_offset;
    s32 vertex_control_type4_offset;
    dword field5_0x14;
    undefined field_0x18[0x8];
} __attribute__((__packed__));

typedef struct GmaHeader GmaHeader, *PGmaHeader;

struct GmaHeader { /* First 8 bytes of a GMA file. Following this is an arbitrary-length list of 8-byte GMA model entries */
    s32 model_count; /* The number of GCMF models in the file (including null entries) */
    s32 header_size; /* Size of header including FIFO padding before GCMF models begin. Also called "model base position" */
} __attribute__((__packed__));

typedef struct GmaSomeStruct GmaSomeStruct, *PGmaSomeStruct;

struct GmaSomeStruct {
    uint g_some_bitflag;
    short g_some_short;
    short g_some_short2;
    short g_some_short3;
    short g_some_short4;
    byte g_some_flag;
    byte g_some_flag2;
    byte g_some_flag3;
    undefined field_0xf[0x2];
    byte g_some_flag4;
    byte g_some_flag5;
    byte g_some_flag6;
    undefined field_0x14[0x2];
    ushort g_some_ushort;
    undefined field_0x18[0x4];
    uint g_some_uint4;
    undefined field_0x20[0x8];
    int g_some_int;
    int g_some_int2;
    undefined field_0x30[0xc];
    float g_some_float;
    uint g_some_uint5;
    undefined field_0x44[0x1c];
} __attribute__((__packed__));

typedef struct GmaTextureDescriptor GmaTextureDescriptor, *PGmaTextureDescriptor;

enum {
    TEXFLAG_UNK_BIT0=1,
    TEXFLAG_UNK_BIT1=2,
    TEXFLAG_REPEAT_U=4,
    TEXFLAG_MIRROR_U=8,
    TEXFLAG_REPEAT_V=16,
    TEXFLAG_MIRROR_V=32,
    TEXFLAG_UNK_BIT6=64,
    TEXFLAG_UNK_BIT7=128,
    TEXFLAG_ENABLE_MIPMAPS=256,
    TEXFLAG_UNK_BIT9=512,
    TEXFLAG_UNK_BIT10=1024,
    TEXFLAG_G_NEAR=2048,
    TEXFLAG_G_HEIGHTMAP_BLEND_GRAYSCALE=4096,
    TEXFLAG_G_ALPHA_MASK=8192,
    TEXFLAG_UNK_BIT14=16384,
    TEXFLAG_UNK_BIT15=32768,
    TEXFLAG_UNK_BIT16=65536,
    TEXFLAG_ENABLE_UV_SCROLL=131072,
    TEXFLAG_UNK_BIT18=262144,
    TEXFLAG_UNK_BIT19=524288,
    TEXFLAG_UNK_BIT20=1048576,
    TEXFLAG_UNK_BIT21=2097152,
    TEXFLAG_UNK_BIT22=4194304,
    TEXFLAG_UNK_BIT23=8388608
};
typedef undefined4 GmaTextureFlags;

enum {
    GX_ANISO_1=0,
    GX_ANISO_2=1,
    GX_ANISO_4=2,
    GX_MAX_ANISOTROPY=3
};
typedef undefined1 GXAnisotropy;

struct GmaTextureDescriptor {
    GmaTextureFlags  flags;
    u16 tpl_texture_idx; /* Index into TPL for which texture is referenced */
    undefined field_0x6[0x1];
    GXAnisotropy  anisotropy; /* Level of anisotropic filtering to use */
    struct GXTexObj * texobj;
    byte unk_0xC;
    byte g_is_swappable_texture; /* Boolean that indicates this texture may be swapped out at runtime. This is used for lap-related textures in F-Zero GX. */
    s16 tex_descriptor_idx; /* Texture descriptor index, matches its zero-indexed value in this array */
    undefined field_0x10[0x10];
} __attribute__((__packed__));

typedef struct OSSectionInfo OSSectionInfo, *POSSectionInfo;

struct OSSectionInfo {
    u32 offset; /* Bit 31 is whether the section is executable */
    u32 size;
} __attribute__((__packed__));

typedef struct OSThreadLink OSThreadLink, *POSThreadLink;

typedef struct OSThread OSThread, *POSThread;

typedef struct OSContext OSContext, *POSContext;

typedef s32 OSPriority;

typedef struct OSThreadQueue OSThreadQueue, *POSThreadQueue;

typedef struct OSMutex OSMutex, *POSMutex;

typedef struct OSMutexQueue OSMutexQueue, *POSMutexQueue;

typedef struct OSMutexLink OSMutexLink, *POSMutexLink;

struct OSThreadLink {
    struct OSThread * next;
    struct OSThread * prev;
} __attribute__((__packed__));

struct OSMutexLink {
    struct OSMutex * next;
    struct OSMutex * prev;
} __attribute__((__packed__));

struct OSThreadQueue {
    struct OSThread * head;
    struct OSThread * tail;
} __attribute__((__packed__));

struct OSMutex {
    struct OSThreadQueue queue;
    struct OSThread * thread; /* the current owner */
    s32 count; /* lock count */
    struct OSMutexLink link; /* for OSThread.queueMutex */
} __attribute__((__packed__));

struct OSContext {
    u32 gpr[32]; /* General-purpose registers */
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fpr[32]; /* Floating-point registers */
    u32 fpscr_pad;
    u32 fpscr;
    u32 ssr0; /* Exception handling registers */
    u32 ssr1;
    u16 mode; /* Context mode - since UIMM is 16 bits in PPC */
    u16 state; /* OR-ed OS_CONTEXT_STATE_* */
    u32 gqr[8]; /* Place Gekko regs at the end so we have minimal changes to existing code */
    f64 psf[32];
} __attribute__((__packed__));

struct OSMutexQueue {
    struct OSMutex * head;
    struct OSMutex * tail;
} __attribute__((__packed__));

struct OSThread {
    struct OSContext context; /* register context */
    u16 state; /* OS_THREAD_STATE_* */
    u16 attr; /* OS_THREAD_ATTR_* */
    s32 suspend; /* suspended if the count is greater than zero */
    OSPriority priority; /* effective scheduling priority */
    OSPriority base; /* base scheduling priority */
    void * val; /* exit value */
    struct OSThreadQueue * queue; /* queue thread is on */
    struct OSThreadLink link; /* queue link */
    struct OSThreadQueue * queueJoin; /* list of threads waiting for termination (join) */
    struct OSMutex * mutex; /* mutex trying to lock */
    struct OSMutexQueue queueMutex; /* list of mutexes owned */
    struct OSThreadLink linkActive; /* list of all threads for debugging */
    u8 * stackBase; /* the thread's designated stack (high address) */
    u32 * stackEnd; /* last word of stack (low address) */
} __attribute__((__packed__));

enum {
    GX_NONE=0,
    GX_DIRECT=1,
    GX_INDEX8=2,
    GX_INDEX16=3
};
typedef undefined4 GXAttrType;

enum {
    GX_VCACHE_MISS_ALL=0,
    GX_VCACHE_MISS_POS=1,
    GX_VCACHE_MISS_NRM=2
};
typedef undefined4 GXEvent;

enum {
    GX_BM_NONE=0,
    GX_BM_BLEND=1,
    GX_BM_LOGIC=2,
    GX_BM_SUBTRACT=3,
    GX_MAX_BLENDMODE=4
};
typedef undefined4 GXBlendMode;

enum {
    GX_READ_00=0,
    GX_READ_FF=1,
    GX_READ_NONE=2
};
typedef undefined4 GXAlphaReadMode;

typedef union PPCWGPipe PPCWGPipe, *PPPCWGPipe;

union PPCWGPipe { /* PPC Write Gather Pipe. Original field names didn't have v_ prefix */
    u8 v_u8;
    u16 v_u16;
    u32 v_u32;
    s8 v_s8;
    s16 v_s16;
    s32 v_s32;
    f32 v_f32;
    f64 v_f64;
};

typedef u32 OSTick;

typedef struct GXVtxAttrFmtList GXVtxAttrFmtList, *PGXVtxAttrFmtList;

enum {
    GX_VA_PNMTXIDX=0,
    GX_VA_TEX0MTXIDX=1,
    GX_VA_TEX1MTXIDX=2,
    GX_VA_TEX2MTXIDX=3,
    GX_VA_TEX3MTXIDX=4,
    GX_VA_TEX4MTXIDX=5,
    GX_VA_TEX5MTXIDX=6,
    GX_VA_TEX6MTXIDX=7,
    GX_VA_TEX7MTXIDX=8,
    GX_VA_POS=9,
    GX_VA_NRM=10,
    GX_VA_CLR0=11,
    GX_VA_CLR1=12,
    GX_VA_TEX0=13,
    GX_VA_TEX1=14,
    GX_VA_TEX2=15,
    GX_VA_TEX3=16,
    GX_VA_TEX4=17,
    GX_VA_TEX5=18,
    GX_VA_TEX6=19,
    GX_VA_TEX7=20,
    GX_POS_MTX_ARRAY=21,
    GX_NRM_MTX_ARRAY=22,
    GX_TEX_MTX_ARRAY=23,
    GX_LIGHT_ARRAY=24,
    GX_VA_NBT=25,
    GX_VA_MAX_ATTR=26,
    GX_VA_NULL=255
};
typedef undefined4 GXAttr;

enum {
    GX_CLR_RGB=0,
    GX_CLR_RGBA=1,
    GX_NRM_NBT3=2
};
typedef undefined4 GXCompCnt;

enum {
    GX_RGB565=0,
    GX_RGB8=1,
    GX_RGBX8=2,
    GX_RGBA4=3,
    GX_F32=4,
    GX_RGBA8=5
};
typedef undefined4 GXCompType;

struct GXVtxAttrFmtList {
    GXAttr  attr;
    GXCompCnt  cnt;
    GXCompType  type;
    u8 frac;
} __attribute__((__packed__));

enum {
    GX_QUADS=128,
    GX_TRIANGLES=144,
    GX_TRIANGLESTRIP=152,
    GX_TRIANGLEFAN=160,
    GX_LINES=168,
    GX_LINESTRIP=176,
    GX_POINTS=184
};
typedef undefined4 GXPrimitive;

enum {
    GX_TEV_KCSEL_1=0,
    GX_TEV_KCSEL_7_8=1,
    GX_TEV_KCSEL_3_4=2,
    GX_TEV_KCSEL_5_8=3,
    GX_TEV_KCSEL_1_2=4,
    GX_TEV_KCSEL_3_8=5,
    GX_TEV_KCSEL_1_4=6,
    GX_TEV_KCSEL_1_8=7,
    GX_TEV_KCSEL_K0=12,
    GX_TEV_KCSEL_K1=13,
    GX_TEV_KCSEL_K2=14,
    GX_TEV_KCSEL_K3=15,
    GX_TEV_KCSEL_K0_R=16,
    GX_TEV_KCSEL_K1_R=17,
    GX_TEV_KCSEL_K2_R=18,
    GX_TEV_KCSEL_K3_R=19,
    GX_TEV_KCSEL_K0_G=20,
    GX_TEV_KCSEL_K1_G=21,
    GX_TEV_KCSEL_K2_G=22,
    GX_TEV_KCSEL_K3_G=23,
    GX_TEV_KCSEL_K0_B=24,
    GX_TEV_KCSEL_K1_B=25,
    GX_TEV_KCSEL_K2_B=26,
    GX_TEV_KCSEL_K3_B=27,
    GX_TEV_KCSEL_K0_A=28,
    GX_TEV_KCSEL_K1_A=29,
    GX_TEV_KCSEL_K2_A=30,
    GX_TEV_KCSEL_K3_A=31
};
typedef undefined4 GXTevKColorSel;

typedef u32 OSModuleID;

enum {
    VI_TVMODE_NTSC_INT=0,
    VI_TVMODE_NTSC_DS=1,
    VI_TVMODE_NTSC_PROG=2,
    VI_TVMODE_PAL_INT=4,
    VI_TVMODE_PAL_DS=5,
    VI_TVMODE_MPAL_INT=8,
    VI_TVMODE_MPAL_DS=9,
    VI_TVMODE_DEBUG_INT=12,
    VI_TVMODE_DEBUG_PAL_INT=16,
    VI_TVMODE_DEBUG_PAL_DS=17,
    VI_TVMODE_EURGB60_INT=20,
    VI_TVMODE_EURGB60_DS=21
};
typedef undefined4 VITVMode;

typedef struct OSModuleInfo OSModuleInfo, *POSModuleInfo;

typedef struct OSModuleLink OSModuleLink, *POSModuleLink;

struct OSModuleLink {
    struct OSModuleInfo * next;
    struct OSModuleInfo * prev;
} __attribute__((__packed__));

struct OSModuleInfo { /* This matches the start of REL file header */
    OSModuleID id; /* unique identifier for the module */
    struct OSModuleLink link; /* doubly linked list of modules */
    u32 numSections; /* # of sections */
    u32 sectionInfoOffset; /* offset to section info table */
    u32 nameOffset; /* offset to module name */
    u32 nameSize; /* size of module name */
    u32 version; /* version number */
} __attribute__((__packed__));

enum {
    GX_TF_C4=8,
    GX_TF_C8=9,
    GX_TF_C14X2=10
};
typedef undefined4 GXCITexFmt;

enum {
    GX_TG_POS=0,
    GX_TG_NRM=1,
    GX_TG_BINRM=2,
    GX_TG_TANGENT=3,
    GX_TG_TEX0=4,
    GX_TG_TEX1=5,
    GX_TG_TEX2=6,
    GX_TG_TEX3=7,
    GX_TG_TEX4=8,
    GX_TG_TEX5=9,
    GX_TG_TEX6=10,
    GX_TG_TEX7=11,
    GX_TG_TEXCOORD0=12,
    GX_TG_TEXCOORD1=13,
    GX_TG_TEXCOORD2=14,
    GX_TG_TEXCOORD3=15,
    GX_TG_TEXCOORD4=16,
    GX_TG_TEXCOORD5=17,
    GX_TG_TEXCOORD6=18,
    GX_TG_COLOR0=19,
    GX_TG_COLOR1=20
};
typedef undefined4 GXTexGenSrc;

typedef struct GXColor GXColor, *PGXColor;

struct GXColor { /* A generic color structure used by various GX API functions. */
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} __attribute__((__packed__));

enum {
    GX_COLOR0=0,
    GX_COLOR1=1,
    GX_ALPHA0=2,
    GX_ALPHA1=3,
    GX_COLOR0A0=4,
    GX_COLOR1A1=5,
    GX_COLOR_ZERO=6,
    GX_ALPHA_BUMP=7,
    GX_ALPHA_BUMPN=8,
    GX_COLOR_NULL=255
};
typedef undefined4 GXChannelID;

enum {
    VI_XFBMODE_SF=0,
    VI_XFBMODE_DF=1
};
typedef undefined4 VIXFBMode;

typedef struct GXTlutRegion GXTlutRegion, *PGXTlutRegion;

struct GXTlutRegion {
    u32 dummy[4];
} __attribute__((__packed__));

enum {
    GX_CLAMP=0,
    GX_REPEAT=1,
    GX_MIRROR=2,
    GX_MAX_TEXWRAPMODE=3
};
typedef undefined4 GXTexWrapMode;

enum {
    GX_ITBA_OFF=0,
    GX_ITBA_S=1,
    GX_ITBA_T=2,
    GX_ITBA_U=3,
    GX_MAX_ITBALPHA=4
};
typedef undefined4 GXIndTexAlphaSel;

enum {
    GX_TEXMAP0=0,
    GX_TEXMAP1=1,
    GX_TEXMAP2=2,
    GX_TEXMAP3=3,
    GX_TEXMAP4=4,
    GX_TEXMAP5=5,
    GX_TEXMAP6=6,
    GX_TEXMAP7=7,
    GX_MAX_TEXMAP=8,
    GX_TEXMAP_NULL=255,
    GX_TEX_DISABLE=256
};
typedef undefined4 GXTexMapID;

enum {
    GX_VC_POS=0,
    GX_VC_NRM=1,
    GX_VC_CLR0=2,
    GX_VC_CLR1=3,
    GX_VC_TEX0=4,
    GX_VC_TEX1=5,
    GX_VC_TEX2=6,
    GX_VC_TEX3=7,
    GX_VC_TEX4=8,
    GX_VC_TEX5=9,
    GX_VC_TEX6=10,
    GX_VC_TEX7=11,
    GX_VC_ALL=15
};
typedef undefined4 GXVCachePerf;

typedef f32 Mtx44[4][4];

enum {
    GX_CS_SCALE_1=0,
    GX_CS_SCALE_2=1,
    GX_CS_SCALE_4=2,
    GX_CS_DIVIDE_2=3,
    GX_MAX_TEVSCALE=4
};
typedef undefined4 GXTevScale;

typedef struct OSRel OSRel, *POSRel;

struct OSRel {
    u16 offset;
    u8 type;
    u8 section;
    u32 addend;
} __attribute__((__packed__));

enum {
    GX_CLAMP_NONE=0,
    GX_CLAMP_TOP=1,
    GX_CLAMP_BOTTOM=2
};
typedef undefined4 GXFBClamp;

typedef struct GXTexRegion GXTexRegion, *PGXTexRegion;

struct GXTexRegion {
    u32 dummy[4];
} __attribute__((__packed__));

enum {
    GX_TEV_ADD=0,
    GX_TEV_SUB=1,
    GX_TEV_COMP_R8_GT=8,
    GX_TEV_COMP_R8_EQ=9,
    GX_TEV_COMP_GR16_GT=10,
    GX_TEV_COMP_GR16_EQ=11,
    GX_TEV_COMP_BGR24_GT=12,
    GX_TEV_COMP_BGR24_EQ=13,
    GX_TEV_COMP_RGB8_GT=14,
    GX_TEV_COMP_RGB8_EQ=15,
    GX_TEV_COMP_A8_GT=16,
    GX_TEV_COMP_A8_EQ=17
};
typedef undefined4 GXTevOp;

typedef struct GXFogAdjTable GXFogAdjTable, *PGXFogAdjTable;

struct GXFogAdjTable {
    u16 dummy[10];
} __attribute__((__packed__));

enum {
    GX_ZT_DISABLE=0,
    GX_ZT_ADD=1,
    GX_ZT_REPLACE=2,
    GX_MAX_ZTEXOP=3
};
typedef undefined4 GXZTexOp;

typedef struct GXVtxDescList GXVtxDescList, *PGXVtxDescList;

struct GXVtxDescList {
    GXAttr  attr;
    GXAttrType  type;
} __attribute__((__packed__));

enum {
    GX_TB_ZERO=0,
    GX_TB_ADDHALF=1,
    GX_TB_SUBHALF=2,
    GX_MAX_TEVBIAS=3
};
typedef undefined4 GXTevBias;

enum {
    GX_TEV_SWAP0=0,
    GX_TEV_SWAP1=1,
    GX_TEV_SWAP2=2,
    GX_TEV_SWAP3=3,
    GX_MAX_TEVSWAP=4
};
typedef undefined4 GXTevSwapSel;

typedef struct ChunkInfo ChunkInfo, *PChunkInfo;

struct ChunkInfo { /* Header data for a heap chunk. The first 32 bytes of a heap chunk is dedicated to this metadata and padding, and the rest is the actual data in the chunk */
    struct ChunkInfo * prev;
    struct ChunkInfo * next;
    u32 size;
    u8 padding[20]; /* This is to make the data portion of the chunk 32-bit aligned */
} __attribute__((__packed__));

typedef struct GXLightObj GXLightObj, *PGXLightObj;

struct GXLightObj {
    u32 dummy[16];
} __attribute__((__packed__));

enum {
    GX_TLUT_16=1,
    GX_TLUT_32=2,
    GX_TLUT_64=4,
    GX_TLUT_128=8,
    GX_TLUT_256=16,
    GX_TLUT_512=32,
    GX_TLUT_1K=64,
    GX_TLUT_2K=128,
    GX_TLUT_4K=256,
    GX_TLUT_8K=512,
    GX_TLUT_16K=1024
};
typedef undefined4 GXTlutSize;

enum {
    GX_CC_CPREV=0,
    GX_CC_APREV=1,
    GX_CC_C0=2,
    GX_CC_A0=3,
    GX_CC_C1=4,
    GX_CC_A1=5,
    GX_CC_C2=6,
    GX_CC_A2=7,
    GX_CC_TEXC=8,
    GX_CC_TEXA=9,
    GX_CC_RASC=10,
    GX_CC_RASA=11,
    GX_CC_ONE=12,
    GX_CC_HALF=13,
    GX_CC_KONST=14,
    GX_CC_ZERO=15,
    GX_CC_TEXRRR=16,
    GX_CC_TEXGGG=17,
    GX_CC_TEXBBB=18,
    GX_CC_QUARTER=19
};
typedef undefined4 GXTevColorArg;

enum {
    GX_AOP_AND=0,
    GX_AOP_OR=1,
    GX_AOP_XOR=2,
    GX_AOP_XNOR=3,
    GX_MAX_ALPHAOP=4
};
typedef undefined4 GXAlphaOp;

enum {
    GX_TO_ZERO=0,
    GX_TO_SIXTEENTH=1,
    GX_TO_EIGHTH=2,
    GX_TO_FOURTH=3,
    GX_TO_HALF=4,
    GX_TO_ONE=5,
    GX_MAX_TEXOFFSET=6
};
typedef undefined4 GXTexOffset;

enum {
    GX_TEV_KASEL_1=0,
    GX_TEV_KASEL_7_8=1,
    GX_TEV_KASEL_3_4=2,
    GX_TEV_KASEL_5_8=3,
    GX_TEV_KASEL_1_2=4,
    GX_TEV_KASEL_3_8=5,
    GX_TEV_KASEL_1_4=6,
    GX_TEV_KASEL_1_8=7,
    GX_TEV_KASEL_K0_R=16,
    GX_TEV_KASEL_K1_R=17,
    GX_TEV_KASEL_K2_R=18,
    GX_TEV_KASEL_K3_R=19,
    GX_TEV_KASEL_K0_G=20,
    GX_TEV_KASEL_K1_G=21,
    GX_TEV_KASEL_K2_G=22,
    GX_TEV_KASEL_K3_G=23,
    GX_TEV_KASEL_K0_B=24,
    GX_TEV_KASEL_K1_B=25,
    GX_TEV_KASEL_K2_B=26,
    GX_TEV_KASEL_K3_B=27,
    GX_TEV_KASEL_K0_A=28,
    GX_TEV_KASEL_K1_A=29,
    GX_TEV_KASEL_K2_A=30,
    GX_TEV_KASEL_K3_A=31
};
typedef undefined4 GXTevKAlphaSel;

enum {
    GX_DA_OFF=0,
    GX_DA_GENTLE=1,
    GX_DA_MEDIUM=2,
    GX_DA_STEEP=3
};
typedef undefined4 GXDistAttnFn;

enum {
    GX_NEAR=0,
    GX_LINEAR=1,
    GX_NEAR_MIP_NEAR=2,
    GX_LIN_MIP_NEAR=3,
    GX_NEAR_MIP_LIN=4,
    GX_LIN_MIP_LIN=5
};
typedef undefined4 GXTexFilter;

enum {
    GX_CA_APREV=0,
    GX_CA_A0=1,
    GX_CA_A1=2,
    GX_CA_A2=3,
    GX_CA_TEXA=4,
    GX_CA_RASA=5,
    GX_CA_KONST=6,
    GX_CA_ZERO=7
};
typedef undefined4 GXTevAlphaArg;

typedef struct GXTlutObj GXTlutObj, *PGXTlutObj;

struct GXTlutObj {
    u32 dummy[3];
} __attribute__((__packed__));

enum {
    GX_VTXFMT0=0,
    GX_VTXFMT1=1,
    GX_VTXFMT2=2,
    GX_VTXFMT3=3,
    GX_VTXFMT4=4,
    GX_VTXFMT5=5,
    GX_VTXFMT6=6,
    GX_VTXFMT7=7,
    GX_MAX_VTXFMT=8
};
typedef undefined4 GXVtxFmt;

enum {
    GX_NEVER=0,
    GX_LESS=1,
    GX_EQUAL=2,
    GX_LEQUAL=3,
    GX_GREATER=4,
    GX_NEQUAL=5,
    GX_GEQUAL=6,
    GX_ALWAYS=7
};
typedef undefined4 GXCompare;

enum {
    GX_PNMTX0=0,
    GX_PNMTX1=3,
    GX_PNMTX2=6,
    GX_PNMTX3=9,
    GX_PNMTX4=12,
    GX_PNMTX5=15,
    GX_PNMTX6=18,
    GX_PNMTX7=21,
    GX_PNMTX8=24,
    GX_PNMTX9=27
};
typedef undefined4 GXPosNrmMtx;

enum {
    GX_PF_RGB8_Z24=0,
    GX_PF_RGBA6_Z24=1,
    GX_PF_RGB565_Z16=2,
    GX_PF_Z24=3,
    GX_PF_Y8=4,
    GX_PF_U8=5,
    GX_PF_V8=6,
    GX_PF_YUV420=7
};
typedef undefined4 GXPixelFmt;

typedef struct OSModuleHeader OSModuleHeader, *POSModuleHeader;

struct OSModuleHeader {
    struct OSModuleInfo info;
    u32 bssSize;
    u32 relOffset;
    u32 impOffset;
    u32 impSize;
    u8 prologSection;
    u8 epilogSection;
    u8 unresolvedSection;
    u8 padding0;
    u32 prolog;
    u32 epilog;
    u32 unresolved;
    u32 align; /* REL versions >=2 only */
    u32 bssAlign; /* REL versions >=2 only */
} __attribute__((__packed__));

enum {
    GX_TL_IA8=0,
    GX_TL_RGB565=1,
    GX_TL_RGB5A3=2,
    GX_MAX_TLUTFMT=3
};
typedef undefined4 GXTlutFmt;

typedef struct ARQRequest ARQRequest, *PARQRequest;

struct ARQRequest {
    struct ARQRequest * next; /* pointer to next in list */
    u32 owner; /* user enumerated owner id */
    u32 type; /* DMA direction */
    u32 priority; /* priority for transaction */
    u32 source; /* address of source */
    u32 dest; /* address of destination */
    u32 length; /* length in bytes */
    void (* callback)(u32); /* user specified callback routine */
} __attribute__((__packed__));

enum {
    GX_SP_OFF=0,
    GX_SP_FLAT=1,
    GX_SP_COS=2,
    GX_SP_COS2=3,
    GX_SP_SHARP=4,
    GX_SP_RING1=5,
    GX_SP_RING2=6
};
typedef undefined4 GXSpotFn;

typedef struct HeapInfo HeapInfo, *PHeapInfo;

struct HeapInfo { /* OS heap info */
    u32 capacity;
    struct ChunkInfo * first_free;
    struct ChunkInfo * first_used;
} __attribute__((__packed__));

enum {
    GX_CULL_NONE=0,
    GX_CULL_FRONT=1,
    GX_CULL_BACK=2,
    GX_CULL_ALL=3
};
typedef undefined4 GXCullMode;

enum {
    GX_CH_RED=0,
    GX_CH_GREEN=1,
    GX_CH_BLUE=2,
    GX_CH_ALPHA=3
};
typedef undefined4 GXTevColorChan;

enum {
    GX_ITM_OFF=0,
    GX_ITM_0=1,
    GX_ITM_1=2,
    GX_ITM_2=3,
    GX_ITM_S0=5,
    GX_ITM_S1=6,
    GX_ITM_S2=7,
    GX_ITM_T0=9,
    GX_ITM_T1=10,
    GX_ITM_T2=11
};
typedef undefined4 GXIndTexMtxID;

enum {
    GX_GM_1_0=0,
    GX_GM_1_7=1,
    GX_GM_2_2=2
};
typedef undefined4 GXGamma;

typedef struct DVDDirEntry DVDDirEntry, *PDVDDirEntry;

struct DVDDirEntry {
    u32 entryNum;
    BOOL32 isDir;
    char * name;
} __attribute__((__packed__));

enum {
    GX_TEVPREV=0,
    GX_TEVREG0=1,
    GX_TEVREG1=2,
    GX_TEVREG2=3,
    GX_MAX_TEVREG=4
};
typedef undefined4 GXTevRegID;

enum {
    GX_TEXCACHE_32K=0,
    GX_TEXCACHE_128K=1,
    GX_TEXCACHE_512K=2,
    GX_TEXCACHE_NONE=3
};
typedef undefined4 GXTexCacheSize;

enum {
    GX_LIGHT_NULL=0,
    GX_LIGHT0=1,
    GX_LIGHT1=2,
    GX_LIGHT2=4,
    GX_LIGHT3=8,
    GX_LIGHT4=16,
    GX_LIGHT5=32,
    GX_LIGHT6=64,
    GX_LIGHT7=128,
    GX_MAX_LIGHT=256
};
typedef undefined4 GXLightID;

enum {
    GX_MTX3x4=0,
    GX_MTX2x4=1
};
typedef undefined4 GXTexMtxType;

enum {
    GX_BL_ZERO=0,
    GX_BL_ONE=1,
    GX_BL_SRCCLR=2,
    GX_BL_INVSRCCLR=3,
    GX_BL_SRCALPHA=4,
    GX_BL_INVSRCALPHA=5,
    GX_BL_DSTALPHA=6,
    GX_BL_INVDSTALPHA=7,
    GX_BL_DSTCLR=8,
    GX_BL_INVDSTCLR=9
};
typedef undefined4 GXBlendFactor;

enum {
    GX_ZC_LINEAR=0,
    GX_ZC_NEAR=1,
    GX_ZC_MID=2,
    GX_ZC_FAR=3
};
typedef undefined4 GXZFmt16;

enum {
    GX_ITB_NONE=0,
    GX_ITB_S=1,
    GX_ITB_T=2,
    GX_ITB_ST=3,
    GX_ITB_U=4,
    GX_ITB_SU=5,
    GX_ITB_TU=6,
    GX_ITB_STU=7,
    GX_MAX_ITBIAS=8
};
typedef undefined4 GXIndTexBiasSel;

enum {
    GX_CLIP_ENABLE=0,
    GX_CLIP_DISABLE=1
};
typedef undefined4 GXClipMode;

enum {
    GX_ITF_8=0,
    GX_ITF_5=1,
    GX_ITF_4=2,
    GX_ITF_3=3,
    GX_MAX_ITFORMAT=4
};
typedef undefined4 GXIndTexFormat;

enum {
    GX_COPY_PROGRESSIVE=0,
    GX_COPY_INTLC_EVEN=2,
    GX_COPY_INTLC_ODD=3
};
typedef undefined4 GXCopyMode;

enum {
    GX_TEVSTAGE0=0,
    GX_TEVSTAGE1=1,
    GX_TEVSTAGE2=2,
    GX_TEVSTAGE3=3,
    GX_TEVSTAGE4=4,
    GX_TEVSTAGE5=5,
    GX_TEVSTAGE6=6,
    GX_TEVSTAGE7=7,
    GX_TEVSTAGE8=8,
    GX_TEVSTAGE9=9,
    GX_TEVSTAGE10=10,
    GX_TEVSTAGE11=11,
    GX_TEVSTAGE12=12,
    GX_TEVSTAGE13=13,
    GX_TEVSTAGE14=14,
    GX_TEVSTAGE15=15,
    GX_MAX_TEVSTAGE=16
};
typedef undefined4 GXTevStageID;

enum {
    GX_PERF1_TEXELS=0,
    GX_PERF1_TX_IDLE=1,
    GX_PERF1_TX_REGS=2,
    GX_PERF1_TX_MEMSTALL=3,
    GX_PERF1_TC_CHECK1_2=4,
    GX_PERF1_TC_CHECK3_4=5,
    GX_PERF1_TC_CHECK5_6=6,
    GX_PERF1_TC_CHECK7_8=7,
    GX_PERF1_TC_MISS=8,
    GX_PERF1_VC_ELEMQ_FULL=9,
    GX_PERF1_VC_MISSQ_FULL=10,
    GX_PERF1_VC_MEMREQ_FULL=11,
    GX_PERF1_VC_STATUS7=12,
    GX_PERF1_VC_MISSREP_FULL=13,
    GX_PERF1_VC_STREAMBUF_LOW=14,
    GX_PERF1_VC_ALL_STALLS=15,
    GX_PERF1_VERTICES=16,
    GX_PERF1_FIFO_REQ=17,
    GX_PERF1_CALL_REQ=18,
    GX_PERF1_VC_MISS_REQ=19,
    GX_PERF1_CP_ALL_REQ=20,
    GX_PERF1_CLOCKS=21,
    GX_PERF1_NONE=22
};
typedef undefined4 GXPerf1;

enum {
    GX_PERF0_VERTICES=0,
    GX_PERF0_CLIP_VTX=1,
    GX_PERF0_CLIP_CLKS=2,
    GX_PERF0_XF_WAIT_IN=3,
    GX_PERF0_XF_WAIT_OUT=4,
    GX_PERF0_XF_XFRM_CLKS=5,
    GX_PERF0_XF_LIT_CLKS=6,
    GX_PERF0_XF_BOT_CLKS=7,
    GX_PERF0_XF_REGLD_CLKS=8,
    GX_PERF0_XF_REGRD_CLKS=9,
    GX_PERF0_CLIP_RATIO=10,
    GX_PERF0_TRIANGLES=11,
    GX_PERF0_TRIANGLES_CULLED=12,
    GX_PERF0_TRIANGLES_PASSED=13,
    GX_PERF0_TRIANGLES_SCISSORED=14,
    GX_PERF0_TRIANGLES_0TEX=15,
    GX_PERF0_TRIANGLES_1TEX=16,
    GX_PERF0_TRIANGLES_2TEX=17,
    GX_PERF0_TRIANGLES_3TEX=18,
    GX_PERF0_TRIANGLES_4TEX=19,
    GX_PERF0_TRIANGLES_5TEX=20,
    GX_PERF0_TRIANGLES_6TEX=21,
    GX_PERF0_TRIANGLES_7TEX=22,
    GX_PERF0_TRIANGLES_8TEX=23,
    GX_PERF0_TRIANGLES_0CLR=24,
    GX_PERF0_TRIANGLES_1CLR=25,
    GX_PERF0_TRIANGLES_2CLR=26,
    GX_PERF0_QUAD_0CVG=27,
    GX_PERF0_QUAD_NON0CVG=28,
    GX_PERF0_QUAD_1CVG=29,
    GX_PERF0_QUAD_2CVG=30,
    GX_PERF0_QUAD_3CVG=31,
    GX_PERF0_QUAD_4CVG=32,
    GX_PERF0_AVG_QUAD_CNT=33,
    GX_PERF0_CLOCKS=34,
    GX_PERF0_NONE=35
};
typedef undefined4 GXPerf0;

typedef struct OSImportInfo OSImportInfo, *POSImportInfo;

struct OSImportInfo {
    OSModuleID id;
    u32 offset;
} __attribute__((__packed__));

typedef struct GXRenderModeObj GXRenderModeObj, *PGXRenderModeObj;

struct GXRenderModeObj {
    VITVMode  viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    VIXFBMode  xFBmode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
} __attribute__((__packed__));

enum {
    GX_XF_FLUSH_NONE=0,
    GX_XF_FLUSH_SAFE=8
};
typedef undefined4 GXXFFlushVal;

enum {
    GX_TLUT0=0,
    GX_TLUT1=1,
    GX_TLUT2=2,
    GX_TLUT3=3,
    GX_TLUT4=4,
    GX_TLUT5=5,
    GX_TLUT6=6,
    GX_TLUT7=7,
    GX_TLUT8=8,
    GX_TLUT9=9,
    GX_TLUT10=10,
    GX_TLUT11=11,
    GX_TLUT12=12,
    GX_TLUT13=13,
    GX_TLUT14=14,
    GX_TLUT15=15,
    GX_BIGTLUT0=16,
    GX_BIGTLUT1=17,
    GX_BIGTLUT2=18,
    GX_BIGTLUT3=19
};
typedef undefined4 GXTlut;

typedef u8 PERFId;

enum {
    GX_ITS_1=0,
    GX_ITS_2=1,
    GX_ITS_4=2,
    GX_ITS_8=3,
    GX_ITS_16=4,
    GX_ITS_32=5,
    GX_ITS_64=6,
    GX_ITS_128=7,
    GX_ITS_256=8,
    GX_MAX_ITSCALE=9
};
typedef undefined4 GXIndTexScale;

typedef struct OSCalendarTime OSCalendarTime, *POSCalendarTime;

struct OSCalendarTime {
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

typedef s16 __OSInterrupt;

enum {
    GX_INDTEXSTAGE0=0,
    GX_INDTEXSTAGE1=1,
    GX_INDTEXSTAGE2=2,
    GX_INDTEXSTAGE3=3,
    GX_MAX_INDTEXSTAGE=4
};
typedef undefined4 GXIndTexStageID;

typedef struct DVDDir DVDDir, *PDVDDir;

struct DVDDir {
    u32 entryNum;
    u32 location;
    u32 next;
} __attribute__((__packed__));

enum {
    GX_TG_MTX3x4=0,
    GX_TG_MTX2x4=1,
    GX_TG_BUMP0=2,
    GX_TG_BUMP1=3,
    GX_TG_BUMP2=4,
    GX_TG_BUMP3=5,
    GX_TG_BUMP4=6,
    GX_TG_BUMP5=7,
    GX_TG_BUMP6=8,
    GX_TG_BUMP7=9,
    GX_TG_SRTG=10
};
typedef undefined4 GXTexGenType;

enum { /* Texture Environment "konstant" color register name. */
    GX_KCOLOR0=0,
    GX_KCOLOR1=1,
    GX_KCOLOR2=2,
    GX_KCOLOR3=3,
    GX_MAX_KCOLOR=4
};
typedef undefined1 GXTevKColorID;

enum {
    GX_LO_CLEAR=0,
    GX_LO_AND=1,
    GX_LO_REVAND=2,
    GX_LO_COPY=3,
    GX_LO_INVAND=4,
    GX_LO_NOOP=5,
    GX_LO_XOR=6,
    GX_LO_OR=7,
    GX_LO_NOR=8,
    GX_LO_EQUIV=9,
    GX_LO_INV=10,
    GX_LO_REVOR=11,
    GX_LO_INVCOPY=12,
    GX_LO_INVOR=13,
    GX_LO_NAND=14,
    GX_LO_SET=15
};
typedef undefined4 GXLogicOp;

enum {
    GX_MT_NULL=0,
    GX_MT_XF_FLUSH=1,
    GX_MT_DL_SAVE_CONTEXT=2
};
typedef undefined4 GXMiscToken;

enum {
    GX_PERSPECTIVE=0,
    GX_ORTHOGRAPHIC=1
};
typedef undefined4 GXProjectionType;

enum {
    GX_TC_LINEAR=0,
    GX_TC_GE=1,
    GX_TC_EQ=2,
    GX_TC_LE=3,
    GX_MAX_TEVCLAMPMODE=4
};
typedef undefined4 GXTevClampMode;

typedef struct GXColorS10 GXColorS10, *PGXColorS10;

struct GXColorS10 {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} __attribute__((__packed__));

enum {
    GX_ITW_OFF=0,
    GX_ITW_256=1,
    GX_ITW_128=2,
    GX_ITW_64=3,
    GX_ITW_32=4,
    GX_ITW_16=5,
    GX_ITW_0=6,
    GX_MAX_ITWRAP=7
};
typedef undefined4 GXIndTexWrap;

enum {
    GX_MODULATE=0,
    GX_DECAL=1,
    GX_BLEND=2,
    GX_REPLACE=3,
    GX_PASSCLR=4
};
typedef undefined4 GXTevMode;

typedef u32 OSInterruptMask;

enum {
    GX_TEXMTX0=30,
    GX_TEXMTX1=33,
    GX_TEXMTX2=36,
    GX_TEXMTX3=39,
    GX_TEXMTX4=42,
    GX_TEXMTX5=45,
    GX_TEXMTX6=48,
    GX_TEXMTX7=51,
    GX_TEXMTX8=54,
    GX_TEXMTX9=57,
    GX_IDENTITY=60
};
typedef undefined4 GXTexMtx;

enum {
    GX_PTTEXMTX0=64,
    GX_PTTEXMTX1=67,
    GX_PTTEXMTX2=70,
    GX_PTTEXMTX3=73,
    GX_PTTEXMTX4=76,
    GX_PTTEXMTX5=79,
    GX_PTTEXMTX6=82,
    GX_PTTEXMTX7=85,
    GX_PTTEXMTX8=88,
    GX_PTTEXMTX9=91,
    GX_PTTEXMTX10=94,
    GX_PTTEXMTX11=97,
    GX_PTTEXMTX12=100,
    GX_PTTEXMTX13=103,
    GX_PTTEXMTX14=106,
    GX_PTTEXMTX15=109,
    GX_PTTEXMTX16=112,
    GX_PTTEXMTX17=115,
    GX_PTTEXMTX18=118,
    GX_PTTEXMTX19=121,
    GX_PTIDENTITY=125
};
typedef undefined4 GXPTTexMtx;

enum {
    GX_TEXCOORD0=0,
    GX_TEXCOORD1=1,
    GX_TEXCOORD2=2,
    GX_TEXCOORD3=3,
    GX_TEXCOORD4=4,
    GX_TEXCOORD5=5,
    GX_TEXCOORD6=6,
    GX_TEXCOORD7=7,
    GX_MAX_TEXCOORD=8,
    GX_TEXCOORD_NULL=255
};
typedef undefined4 GXTexCoordID;

typedef uchar uint8_t;

typedef void *pointer;

#ifdef __cplusplus
extern "C" {
#endif
    /* Global data */
    extern char DVD_GAME_NAME[4];
    extern char DVD_COMPANY[2];
    extern u8 DVD_DISK_NUMBER;
    extern u8 DVD_GAME_VERSION;
    extern u8 DVD_STREAMING;
    extern u8 DVD_STREAMING_BUF_SIZE;
    extern u32 DVD_MAGIC_WORD;
    extern u32 MAGIC_WORD;
    extern u32 VERSION;
    extern size_t PHYSICAL_MEMORY_SIZE;
    extern u32 CONSOLE_TYPE;
    extern void * ARENA_LO;
    extern void * ARENA_HI;
    extern undefined4 FST_LOCATION;
    extern undefined4 FST_MAX_LENGTH;
    extern u32 IS_DEBUGGER_PRESENT;
    extern undefined4 DEBUGGER_EXCEPTION_MASK;
    extern undefined4 DEBUGGER_EXCEPTION_HOOK_DESTINATION;
    extern undefined4 DEBUGGER_LR_TEMP;
    extern undefined4 current_os_context_physical;
    extern undefined4 previous_os_interrupt_mask;
    extern undefined4 current_os_interrupt_mask;
    extern u32 TV_MODE;
    extern size_t ARAM_SIZE;
    extern undefined4 current_os_context_logical;
    extern undefined4 currentOsContextLogical;
    extern undefined4 default_os_thread;
    extern undefined4 active_thread_queue_head;
    extern undefined4 active_thread_queue_tail;
    extern undefined4 current_os_thread;
    extern undefined4 DEBUG_MONITOR_SIZE;
    extern undefined4 DEBUG_MONITOR_LOCATION;
    extern size_t MEMORY_SIZE;
    extern undefined4 DVD_BI2_LOCATION;
    extern undefined4 BUS_CLOCK_SPEED;
    extern undefined4 CPU_CLOCK_SPEED;
    extern undefined system_reset_exception_handler;
    extern undefined machine_check_exception_handler;
    extern undefined dsi_exception_handler;
    extern undefined isi_exception_handler;
    extern undefined external_interrupt_exception_handler;
    extern undefined alignment_exception_handler;
    extern undefined program_exception_handler;
    extern undefined floating_point_unavailable_exception_handler;
    extern undefined decrementer_exception_handler;
    extern undefined system_call_exception_handler;
    extern undefined trace_exception_handler;
    extern undefined performance_monitor_interrupt_handler;
    extern undefined instruction_address_breakpoint_exception_handler;
    extern undefined system_management_interrupt_handler;
    extern undefined thermal_management_interrupt_exception_handler;
    extern undefined4 osStringTablePtr;
    extern pointer switchdataD_80081a8c;
    extern undefined g_some_texture_data;
    extern pointer switchdataD_80110c1c;
    extern undefined * switchdataD_80111e20;
    extern pointer switchdataD_80111e3c;
    extern undefined * switchdataD_80111e7c;
    extern undefined * switchdataD_80111eb0;
    extern undefined * switchdataD_80112104;
    extern undefined * switchdataD_801125a0;
    extern undefined * switchdataD_80112608;
    extern undefined * switchdataD_80112670;
    extern undefined * switchdataD_801126d8;
    extern undefined * switchdataD_8011271c;
    extern undefined * switchdataD_80112760;
    extern undefined * switchdataD_801127a4;
    extern undefined * switchdataD_801127c0;
    extern struct GXRenderModeObj ntsc_interlaced_render_mode;
    extern struct GXRenderModeObj ntsc_progressive_render_mode;
    extern struct GXRenderModeObj mpal_interlaced_render_mode;
    extern struct GXRenderModeObj pal_interlaced_render_mode;
    extern undefined * switchdataD_80112948;
    extern undefined * switchdataD_80112968;
    extern undefined * switchdataD_80112a5c;
    extern undefined * switchdataD_80112b50;
    extern undefined * switchdataD_80112bd8;
    extern undefined * switchdataD_80112c34;
    extern undefined * switchdataD_80112e90;
    extern undefined * switchdataD_00014130;
    extern undefined * switchdataD_80113160;
    extern undefined toneup_tab;
    extern undefined tonedown_tab;
    extern undefined2 dspAttenuationTab;
    extern undefined dspScale2IndexTab;
    extern undefined1 dspScale2IndexTab;
    extern undefined dspDLSVolTab;
    extern undefined musyx_vol_tab;
    extern undefined pan_tab;
    extern undefined pan_tab_dpl2;
    extern undefined * switchdataD_80114110;
    extern undefined * switchdataD_00012220;
    extern undefined sndSintab;
    extern undefined rsmpTab12khz;
    extern undefined4 g_some_new_thread_priority_1;
    extern undefined4 safe_thread_priority;
    extern undefined4 vsync_thread_priority;
    extern undefined4 mwidle_thread_priority;
    extern undefined stdout;
    extern undefined * switchdataD_801444c0;
    extern char ASCII_LOWERCASE[256];
    extern char ASCII_UPPERCASE[256];
    extern pointer switchdataD_80144920;
    extern pointer switchdataD_801449a4;
    extern undefined * switchdataD_80144a28;
    extern undefined * switchdataD_80144b08;
    extern pointer switchdataD_80144b50;
    extern undefined * switchdataD_80144c20;
    extern undefined * switchdataD_80144c3c;
    extern struct PadStatusGroup pad_status_groups[4];
    extern struct AnalogInputGroup merged_analog_inputs;
    extern struct DigitalInputGroup merged_digital_inputs;
    extern struct AnalogInputGroup analog_inputs[4];
    extern dword frames_since_last_input_change;
    extern struct GXTexObj sega_splash_tex;
    extern undefined synthGlobalVariable;
    extern undefined shdwRegs;
    extern struct GXFifoObj default_GXFifoObj;
    extern undefined4 seqNote;
    extern undefined4 seqInstance;
    extern undefined2 seqMIDIPriority;
    extern undefined4 synthTicksPerSecond;
    extern undefined4 synthJobTable;
    extern undefined4 synthInfo;
    extern undefined4 synthMasterFader;
    extern undefined1 g_synthTrackVolume_or_synthVolume;
    extern undefined4 synthAuxACallback;
    extern undefined4 synthAuxBCallback;
    extern undefined1 synthITDDefault;
    extern undefined inpAuxB;
    extern undefined4 vidList;
    extern undefined1 synth_last_started;
    extern undefined1 synth_last_fxstarted;
    extern undefined voicePrioSortVoices;
    extern undefined1 voicePrioSortVoicesRoot;
    extern undefined voicePrioSortRootList;
    extern undefined1 voiceList;
    extern undefined1 vs;
    extern undefined4 gs;
    extern undefined4 dspStudio;
    extern undefined4 dspHRTFOn;
    extern undefined4 startGroup;
    extern undefined startListNum;
    extern undefined runList;
    extern undefined fx_lastNote;
    extern undefined inpFXChannelDefaults;
    extern undefined safe_thread_stack_base;
    extern undefined vsync_thread_stack_base;
    extern undefined mwidle_thread_OSThread;
    extern undefined vsync_thread_OSThread;
    extern undefined safe_thread_OSThread;
    extern struct GXRenderModeObj overscan_adjusted_render_mode;
    extern struct GXFifoObj additional_GXFifoObj;
    extern f32 projection_near_clip;
    extern f32 projection_far_clip;
    extern Mtx44 g_some_projection_matrix;
    extern undefined1 g_some_gmaflag_1;
    extern undefined1 g_some_gmaflag_2;
    extern undefined1 g_some_gmaflag_3;
    extern pointer INIT_REL_PATHS[2];
    extern OSHeapHandle currentHeap;
    extern undefined4 arena_lo;
    extern pointer gx;
    extern BOOL32 SHOULD_ALLOCATE_MEM_FROM_ARENA_HI;
    extern undefined4 rand_next_value;
    extern bool g_something_with_progressive_mode;
    extern u32 init_rel_index;
    extern struct RelBufferInfo mainloop_rel_buffer_info;
    extern Locale  locale;
    extern struct GmaBuffer * init_common_gma;
    extern struct TplBuffer * init_common_tpl;
    extern struct OptiGXSettings * current_gx_settings;
    extern undefined1 dvd_cancel_async_complete_callback_flag;
    extern bool waiting_async_dvdread_callback_flag;
    extern undefined4 g_some_func_ptr3;
    extern undefined4 g_some_card_func_ptr;
    extern undefined4 g_some_func_ptr1;
    extern undefined4 g_some_OSTime;
    extern u32 (* console_reset_callback)(void);
    extern undefined4 BootInfo;
    extern undefined4 BI2DebugFlag;
    extern undefined4 AreWeInitialized;
    extern undefined4 alloc_arena_start;
    extern undefined4 alloc_max_heaps;
    extern undefined4 post_OSInitAlloc_arena_start;
    extern undefined4 post_OSInitAlloc_arena_end_rounddown32;
    extern undefined4 arena_hi;
    extern undefined4 g_OSDispatchInterrupt_OSTime;
    extern undefined4 is_debugger_present_ptr;
    extern undefined4 retraceCount;
    extern undefined4 flushFlag;
    extern undefined4 PreCB;
    extern undefined4 PostCB;
    extern undefined4 encoderType;
    extern undefined2 displayOffsetH;
    extern undefined2 displayOffsetV;
    extern undefined4 showChangeMode;
    extern undefined4 shdwChanged;
    extern undefined4 CPUFifo;
    extern undefined4 GPFifo;
    extern undefined4 TokenCB;
    extern undefined4 DrawDoneCB;
    extern undefined1 g_has_finished_gx_work;
    extern struct OSThreadQueue gx_thread_queue;
    extern undefined4 g_something_with_perf;
    extern undefined4 nextPublicID;
    extern undefined4 g_some_sound_alloc_memory_func_ptr;
    extern undefined4 g_OSTick_after_AIInitDMA;
    extern undefined4 g_are_threads_initialized;
    extern undefined4 g_some_safe_thread_counter;
    extern undefined4 g_safe_thread_loop_counter;
    extern undefined4 mwidle_thread_loop_counter;
    extern undefined4 g_some_new_thread_priority_2;
    extern undefined4 g_vsync_thread_finished;
    extern undefined4 g_vsync_thread_enabled;
    extern undefined4 g_mwidle_thread_finished;
    extern undefined4 g_mwidle_thread_enabled;
    extern undefined4 g_safe_thread_finished;
    extern undefined4 g_safe_thread_enabled;
    extern undefined4 g_something_that_resumes_safe_thread_funcptr;
    extern undefined4 g_something_that_suspends_safe_thread_funcptr;
    extern struct GXRenderModeObj * current_render_mode;
    extern bool gx_draw_is_done;
    extern undefined4 OSTick_after_gx_start_new_frame;
    extern undefined4 gx_start_new_frame_retrace_count;
    extern undefined4 retraces_per_frame;
    extern u32 gx_fifo_use_size2;
    extern undefined4 g_some_gx_fifo_write_ptr;
    extern GxUsageState  gx_usage;
    extern u32 gx_fifo_size;
    extern void * locked_cache_region;
    extern undefined4 old_arena_hi;
    extern undefined4 new_arena_hi;
    extern Mtx * mtxa;
    extern void (* os_free_to_heap_func_ptr)(void *);
    extern void * (* os_alloc_from_heap_func_ptr)(u32);
    extern undefined4 g_some_empty_function_func_ptr;
    extern undefined4 g_some_dvd_close_func_ptr;
    extern undefined4 g_read_file_with_dvd_prio2_func_ptr;
    extern undefined4 g_get_dvd_file_size_func_ptr;
    extern BOOL32 (* g_some_dvd_open_func_ptr)(char *, struct DVDFileInfo *);
    extern DVDFileInfo * (* g_get_some_dvd_info_func_ptr)(void);
    extern struct GraphicsInfo * graphics_info;
    extern undefined4 g_motion;
    extern undefined4 data_buffer;
    extern undefined8 i2fMagic;
    extern undefined4 value0_6;
    extern undefined4 value0_3;
    extern undefined8 i2fMagic;
    extern float FLOAT_ZERO;
    extern float FLOAT0;
    extern float FLOAT1;
    extern float FLOAT400;
    extern undefined4 g_something_with_camera;
    extern double g_related_to_camera_turn_rate_and_stage_tilt;
    extern float camera_near_clip_z;
    extern float camera_far_clip_z;
    extern undefined4 g_camera_fov;
    extern double g_camera_turn_rate_scale;
    extern double camera_height;
    extern double camera_pivot_height;
    extern float camera_distance;
    extern undefined4 g_some_light_color;
    extern float FLOAT20000;
    extern float FLOAT0_2;
    extern float FLOAT480;
    extern float FLOAT640;
    extern BgmTrack  g_bgm_id_lookup_table[42];
    extern struct GXColor debugtext_bg_color;
    extern float MAX_GX_FIFO_BUF_SIZE;
    extern float visual_ball_size;
    extern float ball_friction;
    extern float epsilon;
    extern float g_goal_post_vertical_offset;
    extern undefined8 g_itemgroup_anim_rate_post_dropin;
    extern float g_bg_animation_timescale;
    extern undefined4 g_bonus_stage_list;
    extern double jamabar_sensitivity;
    extern double jamabar_friction;
    extern double jamabar_lower_bound;
    extern float jamabar_lower_bound_2;
    extern undefined BTM_SetDefaultLinkSuperTout;
    extern undefined THPSimpleGetCurrentFrame;
    extern undefined THPSimpleGetCurrentFrame;
    extern undefined THPSimpleGetCurrentFrame;
    extern undefined1 G_ICE_THEME_ID;
    extern undefined8 divisor;
    extern undefined8 base;
    extern struct HeapConfig heap_configs[19];
    extern OSHeapHandle main_heap;
    extern OSHeapHandle stage_heap;
    extern OSHeapHandle bg_heap;
    extern OSHeapHandle chara_heap;
    extern OSHeapHandle replay_heap;
    extern char empty_string[1];
    extern undefined * G_MAIN_MODE_ADDITIONAL_REL_PATHS[8];
    extern void (* main_mode_funcs[8])(void);
    extern void (* sub_mode_funcs[265])(void);
    extern undefined * MAIN_MODE_NAMES[8];
    extern undefined * SUB_MODE_NAMES[265];
    extern struct RelBufferInfo additional_rel_buffer_info;
    extern undefined * DEBUG_MENU_OPTION_NAMES[7];
    extern undefined * switchdataD_80370704;
    extern undefined * switchdataD_80370758;
    extern undefined * switchdataD_80370794;
    extern undefined * switchdataD_803707c8;
    extern undefined * switchdataD_80370848;
    extern struct Event events[23];
    extern undefined * switchdataD_80370d6c;
    extern undefined * switchdataD_80370ddc;
    extern pointer switchdataD_80370dfc;
    extern undefined * switchdataD_80370e5c;
    extern undefined * switchdataD_803727e4;
    extern undefined * switchdataD_80372878;
    extern pointer switchdataD_80374aac;
    extern u8 g_active_players[4];
/* Attempting output of invalid function pointer type declaration: CameraFunc *[93] * */    extern void (* camera_funcs)(struct Camera *, struct Ball *);
    extern pointer g_some_camera_func_table2;
    extern undefined * g_some_camera_func_table3;
    extern undefined * g_some_camera_func_table4;
    extern undefined * switchdataD_80374e54;
    extern pointer switchdataD_80375030;
    extern pointer switchdataD_8037ed0c;
    extern undefined * switchdataD_8037ed30;
    extern undefined * switchdataD_8037ed54;
    extern pointer switchdataD_8037ed78;
    extern undefined * switchdataD_8037edf8;
    extern struct BmpInfo bmp_infos[25];
    extern undefined * bmp_tex_names;
    extern pointer switchdataD_803809d0;
    extern struct SpriteDrawRequest g_some_sprite_related_obj;
    extern undefined4 monkey_flags;
    extern undefined * switchdataD_80391aa0;
    extern undefined * switchdataD_80391ad8;
    extern undefined * switchdataD_80391be8;
    extern undefined * switchdataD_80391e70;
    extern undefined1 g_fifo_use_color;
    extern undefined * switchdataD_8039b19c;
    extern undefined * switchdataD_8039b280;
    extern char * EVENT_STATUS_NAMES[12];
    extern float physical_ball_size;
    extern float ball_accel;
    extern float ball_restitution;
    extern undefined * ball_mode_funcs;
    extern undefined * switchdataD_8039c560;
    extern undefined * switchdataD_8039c5a4;
    extern pointer switchdataD_8039c5dc;
    extern undefined * switchdataD_8039c838;
    extern pointer switchdataD_8039ccf8;
    extern undefined1 g_fog_type;
    extern s32 g_current_stage_id;
    extern s32 g_stage_id_to_load;
    extern undefined1 g_smb1_stage_list;
    extern undefined * LOCALE_STRINGS;
    extern struct GmaBuffer * * g_loaded_gmas_list[5];
    extern pointer goal_gma_model_names;
    extern undefined2 g_cached_button_itemgroup_count_stage;
    extern undefined4 cached_button_itemgroup_count;
    extern void (* effect_init_funcs[71])(struct Effect *);
    extern void (* effect_tick_funcs[71])(struct Effect *);
    extern void (* effect_disp_funcs[71])(struct Effect *);
    extern void (* effect_dest_funcs[71])(struct Effect *);
    extern WorldTheme  world_theme;
    extern struct GXColor g_something_with_world_theme_1;
    extern undefined4 g_something_with_world_theme_2;
    extern undefined4 g_something_with_world_theme_3;
    extern s32 g_smth_for_drawing;
    extern undefined2 g_something_with_world_theme_4;
    extern undefined2 g_something_with_world_theme_5;
    extern struct Ape * * BGApeTable;
    extern undefined bg_init_funcs;
    extern undefined bg_tick_funcs;
    extern undefined bg_dest_funcs;
    extern pointer bg_disp_funcs;
    extern undefined bg_item_coin_coli_funcs;
    extern char * g_bg_filename_list[43];
    extern WorldTheme  STAGE_WORLD_THEMES[428];
    extern undefined * switchdataD_8039f024;
    extern undefined * switchdataD_8039f3f8;
    extern undefined * switchdataD_8039f590;
    extern undefined * switchdataD_8039f61c;
    extern undefined * switchdataD_8039ff54;
    extern undefined * switchdataD_803a0864;
    extern pointer cm_entry_if_funcs;
    extern pointer cm_entry_then_funcs;
    extern struct CmEntry beginner_noex_cm_entries[31];
    extern struct CmEntry advanced_noex_cm_entries[120];
    extern struct CmEntry expert_noex_cm_entries[208];
    extern struct CmEntry beginner_ex_cm_entries[35];
    extern struct CmEntry advanced_ex_cm_entries[32];
    extern struct CmEntry expert_ex_cm_entries[42];
    extern struct CmEntry master_noex_cm_entries[35];
    extern struct CmEntry master_ex_cm_entries[50];
    extern struct CmEntry g_cm_entry_list9[16];
    extern undefined * cm_courses;
    extern pointer g_some_cm_entry_table2;
    extern undefined * g_some_cm_entry_table3;
    extern undefined * item_init_funcs;
    extern undefined * item_tick_funcs;
    extern undefined * item_disp_funcs;
    extern undefined * item_coli_funcs;
    extern undefined * item_dest_funcs;
    extern undefined * item_replay_init_funcs;
    extern undefined * item_debug_funcs;
    extern struct CoinType coin_types[5];
    extern undefined * stobj_init_funcs;
    extern undefined * stobj_tick_funcs;
    extern void (* stobj_disp_funcs[14])(struct Stobj *);
    extern undefined * stobj_coli_funcs;
    extern undefined * stobj_dest_funcs;
    extern undefined * stobj_cb_f_funcs;
    extern undefined * button_type_strs;
    extern pointer switchdataD_803a7210;
    extern pointer switchdataD_803a7234;
    extern undefined * switchdataD_803a7258;
    extern undefined * switchdataD_803a8264;
    extern pointer switchdataD_803a83dc;
    extern pointer switchdataD_803a84b8;
    extern undefined * switchdataD_803a86e8;
    extern undefined * switchdataD_803a8770;
    extern undefined * switchdataD_803a8794;
    extern undefined * switchdataD_803a87b8;
    extern undefined * switchdataD_803a87dc;
    extern pointer switchdataD_803a8800;
    extern pointer switchdataD_803a8894;
    extern undefined * switchdataD_803a88cc;
    extern char LOADIN_TEXT_ROUND[9];
    extern char LOADIN_TEXT_WORLD[12];
    extern char LOADIN_TEXT_MASTER_EX[12];
    extern char LOADIN_TEXT_MASTER[10];
    extern char LOADIN_TEXT_EXTRA[9];
    extern char LOADIN_TEXT_STAGE[9];
    extern char LOADIN_TEXT_BONUS_STAGE[12];
    extern char LOADIN_TEXT_FINAL_ROUND[12];
    extern char LOADIN_TEXT_FINAL_STAGE[12];
    extern undefined * switchdataD_803a96f8;
    extern undefined * switchdataD_803a9e6c;
    extern pointer switchdataD_803a9ea4;
    extern undefined * switchdataD_803a9ee8;
    extern undefined * switchdataD_803aa0d4;
    extern pointer switchdataD_803bdd6c;
    extern pointer switchdataD_803bdda0;
    extern char HUD_TEXT_EX[3];
    extern char HUD_TEXT_WORLD_STAGE_NO[6];
    extern char HUD_TEXT_STAGE_NO[3];
    extern char HUD_TEXT_STAGE_NAME[3];
    extern MinimapMode  g_minimap_mode;
    extern undefined4 minimap_zoom_level;
    extern undefined2 minimap_scale;
    extern undefined2 minimap_rotation;
    extern pointer switchdataD_803c0bc4;
    extern pointer switchdataD_803c12c8;
    extern undefined * switchdataD_803c14b8;
    extern pointer g_some_card_funcs;
    extern pointer g_some_card_funcs2;
    extern undefined * switchdataD_803ce82c;
    extern undefined * switchdataD_803ce880;
    extern undefined * switchdataD_803ce908;
    extern undefined * switchdataD_803ce978;
    extern undefined * switchdataD_803ce9f4;
    extern undefined GXInitTexObjUserData;
    extern undefined dGeomDisable;
    extern pointer switchdataD_803d1240;
    extern undefined * RANKING_SMB1_MAIN_GAME_DEFAULT_NAMES[3][5];
    extern undefined1 rumble_enabled_bitflag;
    extern pointer switchdataD_803d1624;
    extern pointer switchdataD_803d1c2c;
    extern undefined * switchdataD_803d35f0;
    extern undefined * postfix_table;
    extern undefined * postfix_table_s;
    extern undefined * ape_name_enum;
    extern undefined * storymode_ape_enum;
    extern undefined * ape_lod_enum;
    extern undefined * eye_string_table;
    extern char * * eye_name_table[17];
    extern undefined * game_name_enum;
    extern char * enum_face_types[9];
    extern undefined * ape_face_enum_alt;
    extern undefined * ape_face_enum_monkey;
    extern undefined * switchdataD_803d9220;
    extern char DIP_APENUM_FORMAT[5];
    extern undefined * RANKING_DEFAULT_NAMES;
    extern u32 RANKING_DEFAULT_SCORES[20];
    extern u32 G_RANKING_DEFAULT_APE_IDS[20];
    extern undefined THPSimpleGetCurrentFrame;
    extern undefined THPSimpleGetCurrentFrame;
    extern undefined * switchdataD_803db7a0;
    extern undefined * switchdataD_803db9ec;
    extern pointer switchdataD_803dca38;
    extern undefined * switchdataD_803dca6c;
    extern undefined * g_some_chara_anim_funcs;
    extern pointer switchdataD_803dd32c;
    extern undefined * switchdataD_803dd37c;
    extern pointer switchdataD_803dd3c0;
    extern char * CUTSCENE_BIN_FILE_NAMES[16];
    extern undefined * switchdataD_803de11c;
    extern pointer switchdataD_803de8f8;
    extern pointer switchdataD_803ded34;
    extern undefined * switchdataD_803ded7c;
    extern undefined * switchdataD_803dedc4;
    extern pointer switchdataD_803dee0c;
    extern undefined * switchdataD_803e7604;
    extern pointer switchdataD_803edb94;
    extern struct UnlockInfo * unlock_info_ptr;
    extern undefined4 g_loaded_rel_count;
    extern struct RelBufferInfo * g_loaded_rel_infos[10];
    extern undefined4 frame_counter;
    extern StoryModeFunc  g_storymode_mode;
    extern StoryModeFunc  active_story_mode_func;
    extern undefined2 curr_world;
    extern undefined2 selected_storymode_stage;
    extern undefined2 g_storymode_next_world;
    extern undefined4 storymode_score;
    extern undefined4 storymode_bananas;
    extern undefined4 g_some_storymode_func_ptr;
    extern undefined1 curr_storymode_save_file_idx;
    extern Mtx * g_maybe_related_to_stage_tilt;
    extern undefined4 is_more_than_24_mib;
    extern undefined4 g_some_buffer_ptr;
    extern undefined4 g_some_other_heap_lo;
    extern undefined4 g_some_other_heap_hi;
    extern undefined4 g_some_dead_heap_mem_lo;
    extern undefined4 g_some_dead_heap_mem_hi;
    extern undefined4 g_curr_heap_config_idx;
    extern undefined4 main_heap_size;
    extern undefined4 stage_heap_size;
    extern undefined4 bg_heap_size;
    extern undefined4 chara_heap_size;
    extern undefined4 replay_heap_size;
    extern MainMode  main_mode;
    extern SubMode  sub_mode;
    extern MainMode  main_mode_request;
    extern SubMode  sub_mode_request;
    extern s32 sub_mode_frame_counter;
    extern Difficulty  curr_difficulty;
    extern ModeFlag  mode_flags;
    extern undefined4 num_players;
    extern MainGameMode  main_game_mode;
    extern undefined4 curr_player_idx;
    extern undefined4 g_set_when_enter_cm1;
    extern undefined2 g_set_when_enter_cm2;
    extern undefined1 g_3player_camera_setting;
    extern bool stage_complete;
    extern undefined1 g_related_to_fov1;
    extern char * g_curr_main_mode_name;
    extern char * g_curr_sub_mode_name;
    extern undefined4 g_some_func_ptr_related_to_sub_mode2;
    extern undefined4 g_some_func_ptr_related_to_sub_mode;
    extern undefined1 g_repause_cooldown_counter;
    extern undefined4 g_some_bitflag2;
    extern undefined4 g_current_focused_pause_menu_entry;
    extern undefined4 g_current_pause_menu_entry_count;
    extern PauseMenuType  pausemenu_type;
    extern Status  g_pause_status;
    extern undefined4 g_some_other_flags;
    extern struct Vec g_mirror_pos1;
    extern struct Vec g_some_scale_vec3;
    extern struct Vec g_mirror_pos2;
    extern undefined4 g_mirror_pos3;
    extern undefined4 g_pausemenu_screenshot_requested;
    extern void * g_image_buffer_ptr;
    extern undefined2 g_pausemenu_screenshot_left_px;
    extern undefined2 g_pausemenu_screenshot_top_px;
    extern undefined2 g_pausemenu_screenshot_width_px;
    extern undefined2 g_pausemenu_screenshot_height_px;
    extern undefined4 pausemenu_scrot_texfmt;
    extern SubMode  g_sub_mode3;
    extern undefined4 adv_demo_frame_counter;
    extern undefined1 g_focused_root_menu;
    extern undefined1 g_focused_partygame_menu;
    extern undefined1 g_focused_options_menu;
    extern undefined1 g_focused_maingame_menu;
    extern undefined1 selected_cm_difficulty2;
    extern undefined1 selected_cm_difficulty;
    extern u8 selected_characters[4];
    extern byte menu_stack_ptr;
    extern undefined1 g_menu_stack[10];
    extern s8 number_of_starting_monkeys;
    extern bool stageselect_is_storymode;
    extern s8 stageselect_course_idx[2];
    extern s8 stageselect_course_stage_idx[2][10];
    extern undefined4 g_some_func_ptr4;
    extern undefined4 g_something_with_camera3;
    extern struct Camera * g_current_camera;
    extern struct Camera cameras[5];
    extern float g_camera_aspect_ratio;
    extern undefined1 g_related_to_fov2;
    extern undefined4 g_minigame_camera_func;
    extern s16 g_camera_standstill_counters[5];
    extern undefined4 g_maybe_something_with_lights;
    extern undefined4 g_light_group_stack;
    extern u8 num_light_groups;
    extern undefined4 next_fifo_use_array_idx;
    extern u32 last_60_gx_fifo_use_sizes[60];
    extern undefined4 tick_at_gx_finish_frame;
    extern u32 gx_fifo_use_size;
    extern BOOL32 g_video_mode_change_requested;
    extern struct ModeInfo mode_info;
    extern struct GmaBuffer * g_bg_gma;
    extern struct TplBuffer * g_bg_tpl;
    extern undefined4 g_something_related_to_sprites_probably;
    extern undefined g_some_buf_related_to_sprites;
    extern u32 g_profile_timer_start_times[9];
    extern undefined4 g_something_with_perf_profiling;
    extern BOOL32 g_enable_perf;
    extern s32 g_some_draw_perf_var;
    extern undefined4 g_some_perf_time_elapsed;
    extern undefined4 some_time;
    extern undefined4 g_some_perf_timer_total_elapsed;
    extern undefined4 g_some_perf_timer1;
    extern undefined4 g_some_perf_timer2;
    extern undefined4 g_some_perf_timer3;
    extern undefined4 g_some_perf_timer4;
    extern undefined4 g_some_perf_timer5;
    extern undefined4 g_some_perf_timer6;
    extern undefined4 g_some_perf_timer7;
    extern undefined4 g_some_perf_timer8;
    extern undefined4 g_some_perf_timer9;
    extern undefined4 g_some_perf_timer10;
    extern undefined4 g_some_perf_timer11;
    extern undefined4 g_some_perf_timer12;
    extern undefined4 g_some_perf_timer14;
    extern undefined4 g_some_perf_timer13;
    extern undefined4 g_some_perf_timer15;
    extern undefined4 g_some_perf_timer17;
    extern undefined4 g_some_perf_timer18;
    extern undefined4 g_some_perf_timer16;
    extern undefined4 g_debug_sound_ram_usage;
    extern undefined4 g_debug_sound_aram_usage;
    extern undefined2 g_active_music_tracks[10];
    extern undefined1 g_something_related_to_bgm_track_id;
    extern undefined1 g_some_music_related_counter;
    extern undefined4 current_bgm_volume;
    extern undefined4 g_something_with_sound5;
    extern undefined4 g_smth_with_sound;
    extern undefined4 g_player_id_for_sound;
    extern char g_debugtext_unknown_buf1[1961];
    extern char g_debugtext_unknown_buf2[1961];
    extern char g_debugtext_unknown_buf3[1961];
    extern char g_debugtext_colorbuf1[1961];
    extern undefined2 g_debugtext_unknown7;
    extern undefined2 g_debugtext_unknown8;
    extern undefined2 g_debugtext_unknown9;
    extern undefined2 g_debugtext_unknown10;
    extern char g_debugtext_unknown_buf5[1961];
    extern char g_debugtext_colorbuf2[1961];
    extern u16 os_font_encoding;
    extern DipSwitch  dip_switches;
    extern GOtherFlags  g_some_other_flags;
    extern undefined2 g_some_pad_idx;
    extern struct PoolInfo ball_pool_info;
    extern struct PoolInfo item_pool_info;
    extern struct PoolInfo stobj_pool_info;
    extern struct PoolInfo sprite_pool_info;
    extern struct PoolInfo effect_pool_info;
    extern struct PoolInfo g_some_pool_info2;
    extern struct PoolInfo g_some_pool_info3;
    extern struct PoolInfo camera_pool_info;
    extern u8 ball_status_list[4];
    extern u8 item_status_list[256];
    extern u8 stobj_status_list[144];
    extern u8 sprite_status_list[80];
    extern u8 effect_status_list[512];
    extern u8 g_some_pool2_status_list[16];
    extern u8 g_some_pool3_status_list[16];
    extern u8 camera_status_list[5];
    extern short * g_effect_flag_ptr;
    extern struct Ball balls[8];
    extern struct Ball * current_ball;
    extern uint active_monkey_id[4];
    extern u32 player_pad_map[4];
    extern undefined2 g_related_to_stage_tilt1;
    extern undefined2 g_related_to_stage_tilt2;
    extern struct Vec g_gravity_dir;
    extern undefined4 g_world_state;
    extern int g_some_frame_counter;
    extern float g_some_frame_counter_float;
    extern struct Itemgroup * itemgroups;
    extern struct StagedefFileHeader * stagedef;
    extern undefined2 seesaw_count;
    extern undefined1 g_something_with_wl;
    extern undefined4 stage_tpl;
    extern struct GmaBuffer * stage_gma;
    extern undefined2 current_stage_id;
    extern char g_current_stage_name[128];
    extern undefined4 worm_surface_gma_model;
    extern struct GmaModelHeader * wormhole_gma_model;
    extern struct GmaModelHeader * continue_gma_model;
    extern struct GmaModelHeader * goal_gma_models[3];
    extern struct Itemgroup * itemgroups_copy;
    extern undefined2 g_some_stage_id;
    extern undefined2 g_another_stage_id;
    extern s32 LOCALE_STGNAME_DVD_ENTRYNUMS[6];
    extern undefined4 g_some_dvd_entry_num;
    extern undefined4 g_something_with_stgname4_maybe;
    extern u8 g_some_32byte_lz_buffer[32];
    extern undefined4 g_some_replay_data4;
    extern undefined4 replay_frames_remaining;
    extern struct Vec replay_curr_ball_pos;
    extern struct Replay * replay;
    extern undefined g_loaded_player_score;
    extern undefined next_effect_id;
    extern struct Effect effects[512];
    extern Mtx g_related_to_texture_UV_map;
    extern undefined1 cm_unlock_entries[18];
    extern struct CmEntry * current_cm_entry;
    extern int stage_jump_distance;
    extern BOOL32 in_practice_mode;
    extern undefined2 g_stage_id_in_practice_mode;
    extern ModeFlag  g_mode_flags2;
    extern undefined4 g_some_course_length;
    extern struct CmPlayerProgress cm_player_progress[12];
    extern undefined1 storymode_unlock_entries[13];
    extern undefined2 g_next_item_id;
    extern struct Item items[256];
    extern undefined2 next_stobj_id;
    extern struct Stobj stobjs[144];
    extern struct GoalTape goaltapes[16];
    extern struct GoalBag goalbags[16];
    extern undefined4 g_some_goalbag1;
    extern undefined4 g_some_goalbag2;
    extern struct Sprite sprites[80];
    extern struct GSomeSpriteStruct g_some_sprite_structs[82];
    extern undefined4 g_some_sprite_width;
    extern undefined4 g_some_sprite_height;
    extern undefined4 g_screenfade_flags;
    extern u32 g_screenfade_color;
    extern undefined4 g_screenfading1;
    extern undefined4 g_screenfading2;
    extern undefined4 g_something_with_fonts3[512];
    extern undefined2 g_font_type;
    extern undefined4 g_some_data_with_font_drawing2;
    extern undefined4 g_some_data_with_font_drawing3;
    extern undefined2 g_some_data_with_font_drawing4;
    extern float g_some_data_with_font_drawing_depth;
    extern float g_some_data_with_font_drawing_6;
    extern float g_some_data_with_font_drawing_7;
    extern undefined4 g_some_data_with_font_drawing_8;
    extern undefined4 g_some_data_with_font_drawing_9;
    extern undefined1 g_some_data_with_font_drawing_10;
    extern undefined2 g_some_data_with_font_drawing_11;
    extern undefined2 g_some_data_with_font_drawing_12;
    extern undefined2 g_some_data_with_font_drawing_13;
    extern undefined4 g_some_data_with_font_drawing_14;
    extern undefined4 g_some_data_with_font_drawing_15;
    extern undefined4 g_some_data_with_font_drawing_16;
    extern undefined4 g_some_data_with_font_drawing_17;
    extern undefined1 g_some_data_with_font_drawing_18;
    extern undefined4 g_some_data_with_font_drawing_19;
    extern undefined4 g_some_data_with_font_drawing_20;
    extern undefined2 g_some_data_with_font_drawing_21;
    extern undefined4 g_some_data_with_font_drawing_22;
    extern u8 g_banana_disp_efc_req_count;
    extern undefined g_banana_disp_efc_stack[10];
    extern undefined4 global_ape_lod;
    extern undefined4 g_is_waiting_on_some_arq_request;
    extern struct gDiscQueueItem discQueue[128];
    extern undefined4 disc_queue_start;
    extern undefined4 disc_queue_end;
    extern undefined4 discQueueGroup;
    extern struct GCachedFileEntry g_cached_file_entries[128];
    extern undefined4 g_last_filename_attempted_to_open;
    extern undefined4 g_minigame_tick_func;
    extern undefined1 g_haze_type;
    extern struct MemCardInfo memcard_infos[2];
    extern struct StoryModeSaveFile storymode_save_files[3];
    extern undefined1 g_storymode_unlock_entries_copy[13];
    extern undefined1 g_cm_unlock_entries_copy[18];
    extern undefined g_some_string;
    extern struct GSomethingWithPadMotorsStruct g_some_pad_motor_array1[4];
    extern struct GSomethingWithPadMotorsStruct g_some_pad_motor_array2[4];
    extern OSHeapHandle g_some_heap_handle;
    extern undefined main_mta_storage;
    extern struct GBone face_bones[16];
    extern struct GBone handR_bones[16];
    extern struct GBone tailBone;
    extern struct GBone handLbones[16];
    extern void * common_anim_storage[17];
    extern void * game_mal_storage[17];
    extern struct ArcFileInfo * arc_location;
    extern void * common_mal_sizes[17];
    extern int game_mal_sizes[17];
    extern void * ape_tpls[68];
    extern void * rep_tpl_table[17];
    extern void * ape_gma_table[68];
    extern struct ARCHandle ape_skl_arc_handle;
    extern void * ape_skl_arc_data;
    extern Mtx g_common_mtx;
    extern float g_some_floats[64];
    extern undefined ape_ref_count_table;
    extern struct Ape * registered_apes[64];
    extern undefined4 total_apes_registered;
    extern undefined4 g_ptr_to_something;
    extern undefined4 g_something_with_cutscenes3;
    extern undefined2 g_author_frame;
    extern undefined2 g_author_frame_max;
    extern undefined4 g_some_author_related_data;
    extern undefined4 g_author_scene;
    extern undefined4 g_new_main_mode_req;
    extern undefined4 g_new_sub_mode_req;
    extern undefined4 g_some_author_tick_func;
    extern undefined4 g_some_author_cutscene_timer;
    extern undefined4 g_some_author_cutscene_flag;
    extern struct UnlockInfo unlock_info;
    extern pointer switchdataD_804ee064;
    extern undefined * story_mode_funcs;
    extern pointer switchdataD_804eee68;
    extern pointer switchdataD_804eee88;
    extern pointer switchdataD_804eef70;
    extern pointer switchdataD_804ef084;
    extern pointer switchdataD_804ef11c;
    extern undefined CHAR_SPACE_3;
    extern undefined CHAR_SPACE_2;
    extern undefined4 CHAR_A;
    extern undefined4 CHAR_B;
    extern undefined4 CHAR_C;
    extern undefined4 CHAR_D;
    extern undefined4 CHAR_E;
    extern undefined4 CHAR_F;
    extern undefined4 CHAR_G;
    extern undefined4 CHAR_H;
    extern undefined4 CHAR_I;
    extern undefined4 CHAR_J;
    extern undefined4 CHAR_K;
    extern undefined4 CHAR_L;
    extern undefined4 CHAR_M;
    extern undefined4 CHAR_N;
    extern undefined4 CHAR_O;
    extern undefined4 CHAR_P;
    extern undefined4 CHAR_Q;
    extern undefined4 CHAR_R;
    extern undefined4 CHAR_S;
    extern undefined4 CHAR_T;
    extern undefined4 CHAR_U;
    extern undefined4 CHAR_V;
    extern undefined4 CHAR_W;
    extern undefined4 CHAR_X;
    extern undefined4 CHAR_Y;
    extern undefined4 CHAR_Z;
    extern undefined4 CHAR_SPACE;
    extern undefined4 CHAR_a;
    extern undefined4 CHAR_b;
    extern undefined4 CHAR_c;
    extern undefined4 CHAR_d;
    extern undefined4 CHAR_e;
    extern undefined4 CHAR_f;
    extern undefined4 CHAR_g;
    extern undefined4 CHAR_h;
    extern undefined4 CHAR_i;
    extern undefined4 CHAR_j;
    extern undefined4 CHAR_k;
    extern undefined4 CHAR_l;
    extern undefined4 CHAR_m;
    extern undefined4 CHAR_n;
    extern undefined4 CHAR_o;
    extern undefined4 CHAR_p;
    extern undefined4 CHAR_q;
    extern undefined4 CHAR_r;
    extern undefined4 CHAR_s;
    extern undefined4 CHAR_t;
    extern undefined4 CHAR_u;
    extern undefined4 CHAR_v;
    extern undefined4 CHAR_x;
    extern undefined4 CHAR_y;
    extern undefined4 CHAR_z;
    extern undefined4 * * * nameentry_character_ptr_list;
    extern undefined * DEFAULT_FILENAME;
    extern pointer DEFAULT_FILENAME_LOCALE_LIST;
    extern char continue_saved_game_text[40];
    extern char start_game_from_beginning_text[35];
    extern undefined * switchdataD_804f0588;
    extern undefined * switchdataD_804f0734;
    extern undefined * switchdataD_805435f0;
    extern pointer switchdataD_80543614;
    extern pointer switchdataD_80543648;
    extern undefined * switchdataD_80543690;
    extern undefined * switchdataD_805437a4;
    extern undefined * switchdataD_805437cc;
    extern undefined * switchdataD_80543840;
    extern undefined * switchdataD_80543868;
    extern float some_ape_float;
    extern float some_ape_float2;
    extern undefined4 some_ape_float3;
    extern struct SpriteTex g_storymode_preview_textures;
    extern StoryModeStageSelectState  g_storymode_stageselect_state;
    extern undefined2 g_storymode_stageselect_framecounter;
    extern undefined4 g_some_storymode_stageid1;
    extern undefined4 g_some_storymode_stageid2;
    extern undefined4 g_some_storymode_stageid3;
    extern char * story_file_name[6];
    extern undefined1 g_some_nameentry_length;
    extern undefined1 g_some_nameentry_flag;
    extern bool g_storymode_are_you_sure_answer;
    extern DataSelectMenuState  data_select_menu_state;
    extern undefined1 story_file_select_state;
    extern undefined1 selected_story_file_idx;
    extern undefined1 g_dataselect_menu_framecounter;
    extern StoryModeMenuState  storymode_menu_state;
    extern struct Ape * g_some_ape;
    extern undefined4 view_stage_camera_x;
    extern undefined4 view_stage_camera_y;
    extern undefined4 view_stage_camera_z;
    extern undefined2 view_stage_camera_rot_x;
    extern undefined2 view_stage_camera_rot_y;
    extern undefined2 view_stage_camera_rot_z;
    extern undefined * switchdataD_80574504;
    extern undefined * switchdataD_80574614;
    extern pointer switchdataD_805747cc;
    extern pointer switchdataD_805748e8;
    extern pointer switchdataD_805749f8;
    extern pointer switchdataD_80574bb0;
    extern struct MenuEntry main_menu_list;
    extern char CAN_PLAY_NUM_PARTY_GAMES_STRING[49];
    extern undefined party_game_menu_list;
    extern char CAN_PURCHASE_PARTY_GAME_STRING[93];
    extern char CANNOT_SELECT_PARTY_GAME_STRING[84];
    extern undefined options_menu_list;
    extern struct MenuEntry main_game_menu_list[3];
    extern struct MenuScreen menu_screen_list[87];
    extern pointer switchdataD_80580b70;
    extern undefined * switchdataD_805837fc;
    extern pointer switchdataD_80584584;
    extern undefined * switchdataD_805846ac;
    extern MenuScreenID  g_currently_visible_menu_screen;
    extern MenuScreenID  g_next_menu_screen;
    extern undefined1 g_menu_transition_frametimer;
    extern undefined1 g_character_selected;
    extern char * g_menu_description_text;
    extern undefined4 g_active_pads_bitfield;
    extern undefined1 g_language_id;
    extern undefined4 locked_menu_items;
    extern struct Ape * menu_apes[4];
    extern undefined4 g_menu_color_overlay_timer;
    extern undefined g_something_with_preview_textures;
    extern undefined * switchdataD_80590050;
    extern pointer switchdataD_805b1fc6;
    extern pointer switchdataD_805b20d2;
    extern pointer switchdataD_805b22ae;
    extern pointer switchdataD_805b22ea;
    extern undefined * switchdataD_8060470c;
    extern pointer switchdataD_80604780;
    extern pointer switchdataD_806047a0;
    extern pointer switchdataD_806047cc;
    extern undefined * switchdataD_806047ec;
    extern undefined * switchdataD_8060480c;
    extern undefined * switchdataD_80604854;
    extern undefined * switchdataD_80604b5c;
    extern pointer switchdataD_80605210;
    extern undefined * test_mode_sub_mode_funcs;
    extern pointer switchdataD_80638fc4;
    extern undefined * switchdataD_8063d714;
    extern pointer switchdataD_8063d738;
    extern undefined * switchdataD_8063ec08;
    extern pointer switchdataD_80642514;
    extern undefined * switchdataD_80642534;
    extern undefined4 g_debug_sound_fx_id;
    extern undefined1 g_debug_sound_fx_pan;
    extern undefined2 g_debug_sound_fx_pitch;
    extern undefined2 g_debug_sound_fx_doppler;
    extern undefined2 g_debug_sound_fx_mod;
    extern undefined1 g_debug_sound_fx_reverb;
    extern undefined1 g_debug_sound_fx_chorus;
    extern undefined1 g_debug_sound_ics;
    extern undefined2 g_debug_stream_se_id;
    extern undefined1 g_debug_sound_ics_volume;
    extern undefined1 g_debug_sound_ics_pan_L;
    extern undefined1 g_debug_sound_ics_pan_R;
    extern undefined2 g_debug_stream_bgm_id;
    extern undefined1 g_debug_sound_efc;
    extern u16 g_something_with_cutscenes2;
    extern u16 g_something_with_cutscenes;
    extern undefined * switchdataD_8065c7a0;
    extern pointer switchdataD_80685ba0;
    extern pointer switchdataD_80686b64;
    extern pointer switchdataD_80686c20;
    extern undefined2 player_count;
    extern undefined4 race_frames_remaining;
    extern undefined * switchdataD_806c6ec4;
    extern undefined * switchdataD_806c6ef8;
    extern undefined * switchdataD_806c6fe0;
    extern undefined * switchdataD_806c7010;
    extern undefined * switchdataD_806c7058;
    extern undefined * switchdataD_806c7418;
    extern undefined * switchdataD_806c7434;
    extern undefined * switchdataD_806c777c;
    extern undefined * switchdataD_806cb4ac;
    extern undefined * switchdataD_806cb6b8;
    extern pointer switchdataD_806cb6fc;
    extern pointer switchdataD_806cb784;
    extern pointer switchdataD_806fd0d8;
    extern pointer switchdataD_806fd190;
    extern pointer switchdataD_80706bac;
    extern undefined * switchdataD_8073c160;
    extern undefined * switchdataD_8073c244;
    extern undefined * switchdataD_8073c6dc;
    extern undefined * switchdataD_8073c920;
    extern pointer switchdataD_807b8b94;
    extern pointer switchdataD_807b8bbc;
    extern undefined * switchdataD_807b9ca8;
    extern pointer switchdataD_80801518;
    extern undefined * switchdataD_80801724;
    extern undefined * switchdataD_80804710;
    extern undefined * switchdataD_808063ec;
    extern undefined * switchdataD_8080640c;
    extern undefined * switchdataD_8080642c;
    extern undefined * switchdataD_8080644c;
    extern undefined * switchdataD_808064ac;
    extern undefined * switchdataD_8080667c;
    extern pointer switchdataD_80806734;
    extern undefined * switchdataD_80806970;
    extern undefined * switchdataD_808069a0;
    extern undefined * switchdataD_808609e4;
    extern undefined * switchdataD_80860a20;
    extern undefined * switchdataD_80860a90;
    extern undefined * switchdataD_80860ac0;
    extern undefined * switchdataD_80860ae0;
    extern undefined * switchdataD_80860c8c;
    extern undefined * switchdataD_80860ca8;
    extern undefined * switchdataD_80860cd8;
    extern undefined * switchdataD_808ddd64;
    extern pointer switchdataD_808ddfa8;
    extern undefined * switchdataD_808ddfc8;
    extern pointer switchdataD_809958d8;
    extern undefined * switchdataD_80995924;
    extern pointer switchdataD_8099ca88;
    extern undefined * switchdataD_8099cb74;
    extern undefined * switchdataD_8099f914;
    extern undefined * switchdataD_809e535c;
    extern undefined * switchdataD_809e8b58;
    extern undefined * switchdataD_809e8b78;
    extern undefined * switchdataD_809eaa04;
    extern undefined * switchdataD_809eaa28;
    extern undefined * switchdataD_809eaa4c;
    extern undefined * switchdataD_809eaa70;
    extern undefined * switchdataD_809eaa8c;
    extern undefined * switchdataD_809eaaac;
    extern undefined * switchdataD_809eaaf4;
    extern undefined * switchdataD_809eab18;
    extern undefined * switchdataD_809eab34;
    extern undefined * switchdataD_809eab50;
    extern undefined * switchdataD_809eab70;
    extern undefined * switchdataD_809eab8c;
    extern undefined * switchdataD_809eabac;
    extern undefined * switchdataD_809eabf4;
    extern undefined * switchdataD_809eac2c;
    extern undefined * switchdataD_809eac48;
    extern undefined * switchdataD_809eadc4;
    extern pointer switchdataD_809eae20;
    extern pointer switchdataD_809eae94;
    extern pointer switchdataD_809f17e8;
    extern volatile undefined2 g_some_video_bitflag;
    extern volatile union PPCWGPipe GXWGFifo;

    /* Function decls */
    void __check_pad3(void);
    void dol_entry(void);
    void __init_registers(void);
    void __init_data(void);
    void __init_hardware(void);
    void __flush_cache(uint param_1, int param_2);
    void * memset(void * ptr, int value, size_t num);
    void * __fill_mem(void * ptr, int value, size_t num);
    void * memcpy(void * dest, void * src, size_t count);
    int TRK_memset(int param_1, uint param_2, uint param_3);
    void TRK_memcpy(int param_1, int param_2, int param_3);
    void g_init_console(void);
    void g_some_heap_init(void);
    void load_run_main_loop_parent1(void);
    uint read_and_set_os_string_table(void);
    void load_main_loop_rel_and_run(char * rel_filepath, struct RelBufferInfo * buffer_info);
    OSModuleInfo * * unload_main_loop_rel(struct OSModuleInfo * * mainLoopBufPtrs);
    void load_common_gma_tpl(void);
    void set_init_rel_index(u32 index);
    undefined4 g_clear_prev_GX_settings_something1(void);
    void g_clear_prev_GX_settings_something2(void);
    void GXSetCullMode_cached(GXCullMode  mode);
    void GXSetBlendMode_cached(GXBlendMode  type, GXBlendFactor  src_factor, GXBlendFactor  dst_factor, GXLogicOp  op);
    void GXSetTevSwapModeTable_cached(GXTevSwapSel  id, GXTevColorChan  red, GXTevColorChan  green, GXTevColorChan  blue, GXTevColorChan  alpha);
    void g_GXSetTevSwapModeTable_wrapper(GXTevSwapSel  param_1, GXTevColorChan  param_2, GXTevColorChan  param_3, GXTevColorChan  param_4, GXTevColorChan  param_5);
    void GXSetTevSwapMode_cached(GXTevStageID  stage, GXTevSwapSel  ras_sel, GXTevSwapSel  tex_sel);
    void GXSetColorUpdate_cached(GXBool update_enable);
    void GXSetZCompLoc_cached(GXBool before_tex);
    void GXSetTevColorIn_cached(GXTevStageID  stage, GXTevColorArg  a, GXTevColorArg  b, GXTevColorArg  c, GXTevColorArg  d);
    void GXSetTevAlphaIn_cached(GXTevStageID  stage, GXTevAlphaArg  a, GXTevAlphaArg  b, GXTevAlphaArg  c, GXTevAlphaArg  d);
    void GXSetTevColorOp_cached(GXTevStageID  stage, GXTevOp  op, GXTevBias  bias, GXTevScale  scale, GXBool clamp, GXTevRegID  out_reg);
    void GXSetTevAlphaOp_cached(GXTevStageID  stage, GXTevOp  op, GXTevBias  bias, GXTevScale  scale, GXBool clamp, GXTevRegID  out_reg);
    void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(GXTevStageID  param_1, int param_2);
    void g_GXSetTevOrder_wrapper(GXTevStageID  param_1, GXTexCoordID  param_2, GXTexCoordID  param_3, GXChannelID  param_4);
    void GXSetTevKColorSel_cached(GXTevStageID  stage, GXTevKColorSel  sel);
    void GXSetTevKAlphaSel_cached(GXTevStageID  stage, GXTevKAlphaSel  sel);
    void GXSetNumTevStages_cached(u8 nStages);
    void GXSetTevKColor_cached(GXTevKColorID  id, struct GXColor * color);
    void GXLoadTexObj_cached(struct GXTexObj * tex_obj, GXTexMapID  tex_map_id);
    void g_something_with_GXSetProjectionv(undefined4 * param_1, uint param_2);
    void GXSetNumIndStages_cached(u8 nstages);
    void GXSetIndTexOrder_cached(GXIndTexStageID  ind_stage, GXTexCoordID  tex_coord, GXTexMapID  tex_map);
    void g_GXSetIndTexMtx_wrapper(GXIndTexMtxID  param_1, float param_2[2][3], char param_3);
    void GXSetTevIndirect_cached(GXTevStageID  tev_stage, GXIndTexStageID  ind_stage, GXIndTexFormat  format, GXIndTexBiasSel  bias_sel, GXIndTexMtxID  matrix_sel, GXIndTexWrap  wrap_s, GXIndTexWrap  wrap_t, GXBool add_prev, GXBool utc_lod, GXIndTexAlphaSel  alpha_sel);
    void g_GXSetTevIndirect_zero_if_different(GXTevStageID  param_1);
    void GXSetScissor_cached(u32 xOrig, u32 yOrig, u32 wd, u32 ht);
    void GXSetDispCopySrc_cached(u16 left, u16 top, u16 wd, u16 ht);
    void GXSetTexCopySrc_cached(u16 left, u16 top, u16 wd, u16 ht);
    void GXSetDispCopyDst_cached(u16 wd, u16 ht);
    void GXSetTexCopyDst_cached(u16 wd, u16 ht, GXTexFmt  fmt, GXBool mipmap);
    void GXSetCopyClear_cached(struct GXColor clear_clr, u32 clear_z);
    void GXSetTexCoordGen2_cached(GXTexCoordID  dst_coord, GXTexGenType  func, GXTexGenSrc  src_param, u32 mtx, GXBool normalize, u32 postmtx);
    void GXSetNumTexGens_cached(u8 nTexGens);
    void GXSetLineWidth_cached(u8 width, GXTexOffset  tex_offsets);
    void GXSetNumChans_cached(u8 nChans);
    void opti_GXSetChanCtrl(GXChannelID  chan, GXBool enable, GXColorSrc  amb_src, GXColorSrc  mat_src, u32 light_mask, GXDiffuseFn  diff_fn, GXAttnFn  attn_fn);
    void GXSetZMode_cached(GXBool compare_enable, GXCompare  func, GXBool update_enable);
    void g_read_something_from_prev_GX_settings(undefined * param_1, undefined4 * param_2, undefined * param_3);
    void g_set_some_gx_settings(void);
    void g_draw_something(double param_1, double param_2, double param_3, ushort * param_4);
    void mark_finished_waiting_for_dvdread(s32 result, struct DVDFileInfo * file_info);
    u32 read_entire_file_using_dvdread_prio_async(struct DVDFileInfo * fileInfo, void * addr, s32 length, s32 offset);
    void dvd_cancel_async_callback(int param_1);
    undefined4 call_dvd_cancel_async_and_wait_for_completion(void * block);
    void g_something_with_disc_cover(void);
    void g_idle_function(int param_1);
    void g_set_some_func_ptr1(undefined4 param_1);
    void g_set_some_card_func_ptr(undefined4 param_1);
    void g_set_some_func_ptr3(undefined4 param_1);
    void pad_init(void);
    void g_read_inputs_and_maybe_reset_console(void);
    void g_PADRead_and_handle_errors(struct PADStatus * pad_status);
    void g_reset_console(struct PADStatus * status, int param_2);
    void g_reset_console_wrapper(struct PADStatus * status);
    void g_smth_which_may_reset_console(struct PADStatus * status);
    void set_console_reset_callback(u32 (* callback)(void));
    u32 (* get_console_reset_callback(void))(void);
    void * OSAlloc(u32 size);
    void OSFree(void * ptr);
    void g_load_aram_font(void);
    void gARAMFont_DVDRead(struct DVDFileInfo * param_1, int param_2, int param_3);
    void g_fancy_memcpy(void * dest, void * src, size_t count);
    void g_something_with_video_progressive_mode(void);
    void bootup_draw_sega_splash(void);
    void init_sega_splash_tex(struct GXTexObj * tex_obj);
    void draw_sega_splash(struct GXTexObj * param_1);
    void draw_sega_splash_wrapper(void);
    void PPCMfmsr(void);
    void PPCMtmsr(void);
    void PPCMfhid0(void);
    void PPCMthid0(void);
    void PPCMfl2cr(void);
    void PPCMtl2cr(void);
    void PPCMtdec(void);
    void PPCSync(void);
    void PPCHalt(void);
    void PPCMtmmcr0(void);
    void PPCMtmmcr1(void);
    void PPCMfpmc1(void);
    void PPCMtpmc1(void);
    void PPCMtpmc2(void);
    void PPCMfpmc3(void);
    void PPCMtpmc3(void);
    void PPCMfpmc4(void);
    void PPCMtpmc4(void);
    void PPCMfhid2(void);
    void PPCMthid2(void);
    void PPCMfwpar(void);
    void PPCMtwpar(void);
    void PPCDisableSpeculation(void);
    void PPCSetFpNonIEEEMode(void);
    int OSGetConsoleType(void);
    void ClearArena(void);
    void InquiryCallback(undefined4 param_1, int param_2);
    void OSInit(void);
    void OSExceptionInit(void);
    undefined4 __OSDBINITSTART(void);
    undefined4 __OSSetExceptionHandler(uint param_1, undefined4 param_2);
    undefined4 __OSGetExceptionHandler(uint param_1);
    undefined4 __OSPSInit(void);
    uint __OSGetDIConfig(void);
    void OSInitAlarm(void);
    void OSCreateAlarm(undefined4 * param_1);
    void InsertAlarm(undefined4 * param_1, undefined4 param_2, uint param_3, uint param_4, undefined4 param_5);
    void OSSetAlarm(undefined4 * param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5);
    void OSSetPeriodicAlarm(undefined4 * param_1, undefined4 param_2, int param_3, uint param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7);
    void OSCancelAlarm(int * param_1);
    undefined8 DecrementerExceptionCallback(undefined4 param_1, struct OSContext * param_2);
    ChunkInfo * DLInsert(struct ChunkInfo * free_chunk_list, struct ChunkInfo * new_free_chunk);
    void * OSAllocFromHeap(OSHeapHandle heap, u32 size);
    void OSFreeToHeap(OSHeapHandle heap, void * ptr);
    OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);
    void * OSInitAlloc(void * arena_start, void * arena_end, int max_heaps);
    OSHeapHandle OSCreateHeap(void * start, void * end);
    void OSDestroyHeap(OSHeapHandle heap);
    long OSCheckHeap(OSHeapHandle heap);
    void * OSGetArenaHi(void);
    void * OSGetArenaLo(void);
    void OSSetArenaHi(void * newHi);
    void OSSetArenaLo(void * newLo);
    void * OSAllocFromArenaLo(u32 size, u32 align);
    void * OSAllocFromArenaHi(u32 size, u32 align);
    void __OSInitAudioSystem(void);
    void __OSStopAudioSystem(void);
    uint DCEnable(void);
    void DCInvalidateRange(void * startAddr, u32 nBytes);
    void DCFlushRange(void * startAddr, u32 nBytes);
    uint DCStoreRange(uint param_1, int param_2);
    uint DCFlushRangeNoSync(uint param_1, int param_2);
    uint DCStoreRangeNoSync(uint param_1, int param_2);
    void ICInvalidateRange(void * startAddr, u32 nBytes);
    uint ICFlashInvalidate(void);
    uint ICEnable(void);
    ulonglong __LCEnable(void);
    void LCEnable(void);
    ulonglong LCDisable(void);
    void LCLoadBlocks(void);
    uint LCStoreBlocks(uint param_1);
    void LCQueueWait(int param_1);
    void L2GlobalInvalidate(void);
    void DMAErrorHandler(undefined4 param_1, undefined4 * param_2);
    void __OSCacheInit(void);
    void __OSLoadFPUContext(undefined8 param_1, undefined4 param_2, int param_3);
    void __OSSaveFPUContext(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, undefined4 param_10, int param_11);
    void OSSetCurrentContext(struct OSContext * context);
    OSContext * OSGetCurrentContext(void);
    undefined4 OSSaveContext(int param_1);
    undefined8 OSLoadContext(int param_1);
    u32 OSGetStackPointer(void);
    void OSClearContext(struct OSContext * context);
    void OSInitContext(struct OSContext * param_1, u32 param_2, u32 param_3);
    void OSDumpContext(undefined4 * param_1);
    void __OSContextInit(void);
    void OSReport(char * msg, ...);
    void OSPanic(char * file, int line, char * msg, ...);
    undefined4 OSSetErrorHandler(uint param_1, undefined4 param_2);
    void Decode(int param_1, int param_2);
    u16 OSGetFontEncode(void);
    void ReadROM(void * param_1, u32 param_2, int param_3);
    void ExpandFontSheet(int param_1, uint param_2);
    bool OSInitFont(int param_1);
    BOOL32 OSDisableInterrupts(void);
    BOOL32 OSEnableInterrupts(void);
    BOOL32 OSRestoreInterrupts(BOOL32 enable);
    undefined4 __OSSetInterruptHandler(short param_1, undefined4 param_2);
    void (* __OSGetInterruptHandler(__OSInterrupt interrupt))(__OSInterrupt, struct OSContext *);
    void __OSInterruptInit(void);
    OSInterruptMask SetInterruptMask(OSInterruptMask mask);
    uint __OSMaskInterrupts(uint param_1);
    OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask mask);
    undefined8 __OSDispatchInterrupt(undefined4 param_1, int param_2);
    undefined8 ExternalInterruptHandler(undefined4 param_1, undefined4 * param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void empty_function(void);
    void empty_function(void);
    void OSSetStringTable(void * stringTable);
    undefined4 Relocate(struct OSModuleHeader * module1, struct OSModuleHeader * module2);
    bool OSLink(struct OSModuleHeader * newModule, void * bss);
    undefined4 Undo(int * param_1, int param_2);
    bool OSUnlink(struct OSModuleInfo * oldModule);
    void __OSModuleInit(void);
    u32 OSGetConsoleSimulatedMemSize(void);
    undefined4 OnReset(int param_1);
    void MEMIntrruptHandler(undefined4 param_1, undefined4 * param_2);
    uint RealMode(void);
    void __OSInitMemoryProtection(void);
    void __OSUnlockAllMutex(int param_1);
    void Run(void);
    void __OSReboot(void);
    void OSRegisterResetFunction(int param_1);
    void Reset(undefined4 param_1);
    void __OSDoHotReset(int param_1);
    void OSResetSystem(int param_1, int param_2, int param_3);
    uint OSGetResetCode(void);
    void __OSResetSWInterruptHandler(void);
    uint OSGetResetButtonState(void);
    void OSGetResetSwitchState(void);
    void WriteSramCallback(void);
    uint WriteSram(byte * param_1, int param_2, int param_3);
    void __OSInitSram(void);
    undefined2 * __OSLockSram(void);
    undefined2 * __OSLockSramEx(void);
    uint UnlockSram(int param_1, uint param_2);
    void __OSUnlockSram(int param_1);
    void __OSUnlockSramEx(int param_1);
    undefined4 __OSSyncSram(void);
    uint __OSReadROM(void * param_1, u32 param_2, int param_3);
    u32 OSGetSoundMode(void);
    void OSSetSoundMode(uint param_1);
    u32 OSGetProgressiveMode(void);
    void OSSetProgressiveMode(uint param_1);
    undefined2 OSGetWirelessID(int param_1);
    void OSSetWirelessID(int param_1, short param_2);
    void __OSInitSystemCall(void);
    void __OSThreadInit(void);
    void OSInitThreadQueue(struct OSThreadQueue * queue);
    undefined4 OSGetCurrentThread(void);
    int OSDisableScheduler(void);
    int OSEnableScheduler(void);
    void UnsetRun(int param_1);
    int __OSGetEffectivePriority(int param_1);
    undefined4 SetEffectivePriority(int param_1, undefined4 param_2);
    OSContext * SelectThread(int param_1);
    void __OSReschedule(void);
    undefined4 OSCreateThread(struct OSThread * thread, void * func, void * param, void * stackBase, u32 stackSize, OSPriority priority, u16 attribute);
    void OSExitThread(u32 param_1);
    void OSCancelThread(struct OSContext * param_1);
    int OSResumeThread(int param_1);
    int OSSuspendThread(int param_1);
    void OSSleepThread(int * param_1);
    void OSWakeupThread(int * param_1);
    undefined4 OSSetThreadPriority(int param_1, int param_2);
    undefined4 OSGetThreadPriority(int param_1);
    void OSGetTime(void);
    OSTick OSGetTick(void);
    ulonglong __OSGetSystemTime(void);
    undefined8 __OSTimeToSystemTime(int param_1, uint param_2);
    void GetDates(int param_1, int param_2);
    void OSTicksToCalendarTime(uint param_1, uint param_2, int * param_3);
    void init_cpp_wrapper(void);
    void __init_cpp(void);
    void ppc_halt_wrapper(void);
    undefined4 SIBusy(void);
    undefined4 SIIsChanBusy(int param_1);
    uint CompleteTransfer(void);
    void SIInterruptHandler(undefined4 param_1, undefined4 param_2);
    bool SIEnablePollingInterrupt(int param_1);
    undefined4 SIRegisterPollingHandler(int param_1);
    undefined4 SIUnregisterPollingHandler(int param_1);
    void SIInit(void);
    undefined4 __SITransfer(uint param_1, undefined4 * param_2, int param_3, undefined4 param_4, int param_5, int param_6);
    uint SIGetStatus(int param_1);
    void SISetCommand(int param_1, undefined4 param_2);
    void SITransferCommands(void);
    uint SISetXY(int param_1, int param_2);
    uint SIEnablePolling(uint param_1);
    uint SIDisablePolling(uint param_1);
    bool SIGetResponseRaw(int param_1);
    int SIGetResponse(int param_1, undefined4 * param_2);
    undefined4 SITransfer(uint param_1, undefined4 * param_2, int param_3, undefined4 param_4, int param_5, int param_6, uint param_7, uint param_8);
    void GetTypeCallback(uint param_1, uint param_2);
    int SIGetType(uint param_1);
    int SIGetTypeAsync(uint param_1, undefined * param_2);
    void SISetSamplingRate(uint param_1);
    void SIRefreshSamplingRate(void);
    void SetExiInterruptMask(int param_1, int * param_2);
    undefined4 EXIImm(int param_1, byte * param_2, int param_3, int param_4, undefined4 param_5);
    undefined4 EXIImmEx(int param_1, byte * param_2, int param_3, int param_4);
    undefined4 EXIDma(int param_1, uint param_2, undefined4 param_3, int param_4, undefined4 param_5);
    undefined4 EXISync(int param_1);
    uint EXIClearInterrupts(int param_1, int param_2, int param_3, int param_4);
    int EXISetExiCallback(int param_1, int param_2);
    undefined4 __EXIProbe(int param_1);
    int EXIProbe(int param_1);
    undefined4 EXIProbeEx(int param_1);
    undefined4 EXIAttach(int param_1, undefined4 param_2);
    undefined4 EXIDetach(int param_1);
    undefined4 EXISelect(int param_1, int param_2, int param_3);
    undefined4 EXIDeselect(int param_1);
    void TCIntrruptHandler(short param_1, struct OSContext * param_2);
    void EXIInit(void);
    undefined4 EXILock(int param_1, int param_2, int param_3);
    undefined4 EXIUnlock(int param_1);
    undefined4 sndReadFlag(int param_1);
    undefined4 EXIGetID(int param_1, int param_2, undefined4 * param_3);
    undefined4 InitializeUART(void);
    undefined4 WriteUARTN(byte * param_1, uint param_2);
    void DBInit(void);
    void __DBExceptionDestinationAux(void);
    void __DBExceptionDestination(void);
    uint __DBIsExceptionMarked(uint param_1);
    void DBPrintf(void);
    undefined8 PSMTXIdentity(int param_1);
    undefined8 PSMTXCopy(int param_1, int param_2);
    undefined8 PSMTXConcat(int param_1, int param_2, int param_3);
    undefined4 PSMTXInverse(int param_1, int param_2);
    void PSMTXScale(double param_1, double param_2, double param_3, float * param_4);
    void C_MTXLookAt(Mtx * mtx, struct Vec * cam_pos, struct Vec * cam_up, struct Vec * target);
    void C_MTXFrustum(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, float * param_7);
    void C_MTXPerspective(Mtx44 * m, double fovy, double aspect, double n, double f);
    void C_MTXOrtho(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, float * m);
    void PSVECAdd(int param_1, int param_2, int param_3);
    void PSVECSubtract(int param_1, int param_2, int param_3);
    void PSVECScale(undefined8 param_1, int param_2, int param_3);
    undefined8 PSVECNormalize(int param_1, int param_2);
    undefined8 PSVECCrossProduct(int param_1, int param_2, int param_3);
    void __DVDInitWA(void);
    void AlarmHandlerForTimeout(undefined4 param_1, struct OSContext * param_2);
    void Read(undefined4 param_1, uint param_2, uint param_3, undefined4 param_4);
    void SeekTwiceBeforeRead(undefined4 param_1, undefined4 param_2, uint param_3, undefined4 param_4);
    undefined4 DVDLowRead(undefined4 param_1, uint param_2, uint param_3, undefined4 param_4);
    undefined4 DVDLowSeek(uint param_1, undefined4 param_2);
    undefined4 DVDLowWaitCoverClose(undefined4 param_1);
    undefined4 DVDLowReadDiskID(undefined4 param_1, undefined4 param_2);
    undefined4 DVDLowStopMotor(undefined4 param_1);
    undefined4 DVDLowRequestError(undefined4 param_1);
    undefined4 DVDLowInquiry(undefined4 param_1, undefined4 param_2);
    undefined4 DVDLowAudioStream(uint param_1, undefined4 param_2, uint param_3, undefined4 param_4);
    undefined4 DVDLowRequestAudioStatus(uint param_1, undefined4 param_2);
    undefined4 DVDLowAudioBufferConfig(int param_1, uint param_2, undefined4 param_3);
    void DVDLowReset(void);
    undefined4 DVDLowBreak(void);
    undefined4 DVDLowClearCallback(void);
    void __DVDLowSetWAType(undefined4 param_1, undefined4 param_2);
    void __DVDFSInit(void);
    s32 DVDConvertPathToEntrynum(char * fileName);
    BOOL32 DVDFastOpen(s32 entrynum, struct DVDFileInfo * fileInfo);
    BOOL32 DVDOpen(char * fileName, struct DVDFileInfo * fileInfo);
    BOOL32 DVDClose(struct DVDFileInfo * fileInfo);
    int entryToPath(int param_1, int param_2, int param_3);
    bool DVDConvertEntrynumToPath(s32 entry_num, char * out_path, size_t max_path_size);
    undefined4 DVDGetCurrentDir(int param_1, int param_2);
    void DVDChangeDir(char * dirName);
    BOOL32 DVDReadAsyncPrio(struct DVDFileInfo * fileInfo, void * addr, s32 length, s32 offset, void (* callback)(s32, struct DVDFileInfo *), s32 prio);
    void cbForReadAsync(undefined4 param_1, int param_2);
    s32 DVDReadPrio(struct DVDFileInfo * fileInfo, void * addr, s32 length, s32 offset, s32 prio);
    void cbForCancelSync(void);
    BOOL32 DVDOpenDir(char * dirName, struct DVDDir * dir);
    BOOL32 DVDReadDir(struct DVDDir * dir, struct DVDDirEntry * dirent);
    void DVDPrepareStreamAsync(undefined4 * param_1, uint param_2, uint param_3, undefined4 param_4);
    void cbForReadAsync(undefined4 param_1, int param_2);
    void DVDInit(void);
    void cbForStateError(int param_1);
    void stateTimeout(void);
    void stateGettingError(void);
    undefined4 CategorizeError(uint param_1);
    void cbForStateGettingError(uint param_1);
    void cbForUnrecoveredError(uint param_1);
    void cbForUnrecoveredErrorRetry(uint param_1);
    void stateGettingError(void);
    void cbForStateGoToRetry(uint param_1);
    void stateCheckID(void);
    void stateCheckID3(void);
    void stateCheckID2(void);
    void cbForStateCheckID1(uint param_1);
    void cbForStateCheckID3(uint param_1);
    void stateCoverClosed(void);
    void cbForStateCoverClosed(uint param_1);
    void stateGettingError(void);
    void cbForStateMotorStopped(void);
    void stateReady(void);
    void stateBusy(int param_1);
    undefined4 DVDReadAbsAsyncPrio(undefined4 * param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, int param_6);
    undefined4 DVDReadAbsAsyncForBS(undefined4 * param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5);
    undefined4 DVDReadDiskID(undefined4 * param_1, undefined4 param_2, undefined4 param_3);
    undefined4 DVDPrepareStreamAbsAsync(undefined4 * param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
    undefined4 DVDCancelStreamAsync(undefined4 * param_1, undefined4 param_2);
    undefined4 DVDStopStreamAtEndAsync(undefined4 * param_1, undefined4 param_2);
    undefined4 DVDGetStreamErrorStatusAsync(undefined4 * param_1, undefined4 param_2);
    undefined4 DVDGetStreamPlayAddrAsync(undefined4 * param_1, undefined4 param_2);
    undefined4 DVDInquiryAsync(undefined4 * param_1, undefined4 param_2, undefined4 param_3);
    void DVDReset(void);
    s32 DVDGetCommandBlockStatus(struct DVDCommandBlock * commandBlock);
    s32 DVDGetDriveStatus(void);
    undefined4 DVDCancelAsync(int * param_1, undefined * param_2);
    s32 DVDCancel(struct DVDCommandBlock * block);
    void cbForCancelSync(void);
    DVDDiskID * DVDGetCurrentDiskID(void);
    undefined4 DVDCheckDisk(void);
    void __DVDPrepareResetAsync(undefined * param_1);
    void __DVDClearWaitingQueue(void);
    undefined4 __DVDPushWaitingQueue(int param_1, undefined4 * param_2);
    int * * __DVDPopWaitingQueue(void);
    undefined4 __DVDCheckWaitingQueue(void);
    undefined4 __DVDDequeueWaitingQueue(int * param_1);
    char ErrorCode2Num(uint param_1);
    void __DVDStoreErrorCode(uint param_1);
    void cb(int param_1, undefined4 * param_2);
    void __fstLoad(void);
    void __VIRetraceHandler(undefined4 param_1, struct OSContext * param_2);
    undefined * getTiming(undefined4 param_1);
    void __VIInit(uint param_1);
    void VIInit(void);
    void VIWaitForRetrace(void);
    void setFbbRegs(int param_1, uint * param_2, uint * param_3, uint * param_4, uint * param_5);
    void setVerticalRegs(uint param_1, ushort param_2, byte param_3, short param_4, short param_5, short param_6, short param_7, short param_8, int param_9);
    void VIConfigure(struct GXRenderModeObj * rm);
    void VIFlush(void);
    void VISetNextFrameBuffer(void * fb);
    void VISetBlack(bool black);
    u32 VIGetRetraceCount(void);
    undefined4 getCurrentFieldEvenOdd(void);
    uint VIGetCurrentLine(void);
    u32 VIGetTvFormat(void);
    u32 VIGetDTVStatus(void);
    void UpdateOrigin(uint param_1);
    void PADOriginCallback(undefined4 param_1, uint param_2);
    void PADProbeCallback(undefined4 param_1, uint param_2);
    void PADTypeAndStatusCallback(undefined4 param_1, uint param_2);
    undefined4 PADReset(uint param_1);
    undefined4 PADRecalibrate(uint param_1);
    undefined4 PADInit(void);
    u32 PADRead(struct PADStatus * status);
    void PADControlAllMotors(u32 * commandArray);
    void PADControlMotor(uint param_1, uint param_2);
    void PADSetSpec(int param_1);
    void SPEC0_MakeStatus(undefined4 param_1, ushort * param_2, uint * param_3);
    void SPEC1_MakeStatus(undefined4 param_1, ushort * param_2, uint * param_3);
    void SPEC2_MakeStatus(int param_1, ushort * param_2, undefined4 * param_3);
    void PADSetAnalogMode(int param_1);
    void SamplingHandler(undefined4 param_1, struct OSContext * param_2);
    undefined4 PADSetSamplingCallback(int param_1);
    bool __PADDisableRecalibration(int param_1);
    undefined4 AIRegisterDMACallback(undefined4 param_1);
    void AIInitDMA(undefined4 param_1, uint param_2);
    void AIStartDMA(void);
    void AIStopDMA(void);
    undefined4 AIGetStreamSampleCount(void);
    void AIResetStreamSampleCount(void);
    void AISetStreamTrigger(undefined4 param_1);
    void AISetStreamPlayState(uint param_1);
    uint AIGetStreamPlayState(void);
    void AISetDSPSampleRate(uint param_1);
    uint AIGetDSPSampleRate(void);
    void __AI_set_stream_sample_rate(uint param_1);
    uint AIGetStreamSampleRate(void);
    void AISetStreamVolLeft(uint param_1);
    uint AIGetStreamVolLeft(void);
    void AISetStreamVolRight(uint param_1);
    uint AIGetStreamVolRight(void);
    void AIInit(undefined4 param_1);
    void __AISHandler(undefined4 param_1, struct OSContext * param_2);
    void __AICallbackStackSwitch(undefined * param_1);
    void __AI_SRC_INIT(void);
    ushort ARGetDMAStatus(void);
    void ARStartDMA(int param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
    undefined4 ARInit(undefined4 param_1, undefined4 param_2);
    undefined4 ARGetBaseAddress(void);
    void __ARHandler(undefined4 param_1, struct OSContext * param_2);
    void __ARChecksize(void);
    void __ARQServiceQueueLo(void);
    void empty_function(void);
    void __ARQInterruptServiceRoutine(void);
    void ARQInit(void);
    void ARQPostRequest(struct ARQRequest * request, u32 owner, u32 type, u32 priority, u32 source, u32 dest, u32 length, void (* callback)(u32));
    void ARQSetChunkSize(uint param_1);
    void hwExit(void);
    void hwDisableIrq(void);
    void salExitAi(void);
    void hwEnableIrq(void);
    void __AXOutQuit(void);
    void salExitDspCtrl(void);
    void salExitDsp(void);
    ushort DSPCheckMailToDSP(void);
    ushort DSPCheckMailFromDSP(void);
    undefined4 DSPReadMailFromDSP(void);
    void DSPSendMailToDSP(undefined4 param_1);
    void DSPInit(void);
    void DSPReset(void);
    void DSPHalt(void);
    ushort DSPGetDMAStatus(void);
    undefined4 * DSPAddTask(undefined4 * param_1);
    void __DSPHandler(undefined4 param_1, struct OSContext * param_2);
    void __DSP_exec_task(int param_1, int * param_2);
    void __DSP_boot_task(int param_1);
    void __DSP_insert_task(int param_1);
    void __DSP_remove_task(undefined4 * param_1);
    uint __DTKCallbackForFlush(void);
    void DTKInit(void);
    undefined4 DTKQueueTrack(char * param_1, undefined4 * param_2, undefined4 param_3, undefined4 param_4);
    void DTKFlushTracks(undefined4 param_1);
    void g_DTK_set_something(undefined4 param_1);
    void DTKSetState(int param_1);
    void DTKNextTrack(void);
    void DTKPrevTrack(void);
    undefined4 g_DTK_get_something1(void);
    undefined4 g_GTK_get_something2(void);
    void DTKSetVolume(uint param_1, uint param_2);
    void empty_function(void);
    void __CARDExtHandler(int param_1);
    void __CARDExiHandler(int param_1);
    void __CARDTxHandler(int param_1);
    void __CARDUnlockedHandler(int param_1);
    undefined4 __CARDEnableInterrupt(int param_1, int param_2);
    undefined4 __CARDReadStatus(int param_1, byte * param_2);
    undefined4 __CARDClearStatus(int param_1);
    void TimeoutHandler(undefined * param_1);
    undefined4 Retry(int param_1);
    void UnlockedCallback(int param_1, int param_2);
    undefined4 __CARDStart(int param_1, int param_2, int param_3);
    int __CARDReadSegment(int param_1, int param_2);
    int __CARDWritePage(int param_1, int param_2);
    int __CARDEraseSector(int param_1, uint param_2, int param_3);
    void CARDInit(void);
    void __CARDSetDiskID(undefined * param_1);
    undefined4 __CARDGetControlBlock(int param_1, int * * param_2);
    int __CARDPutControlBlock(int * param_1, int param_2);
    s32 CARDGetResultCode(int param_1);
    int CARDFreeBlocks(int param_1, int * param_2, int * param_3);
    undefined4 OnReset(int param_1);
    uint bitrev(uint param_1);
    undefined4 ReadArrayUnlock(int param_1, uint param_2, byte * param_3, int param_4, int param_5);
    uint DummyLen(void);
    undefined4 __CARDUnlock(int param_1, uint * param_2);
    void InitCallback(int param_1);
    void DoneCallback(int param_1);
    void BlockReadCallback(int param_1, int param_2);
    int __CARDRead(int param_1, undefined4 param_2, uint param_3, undefined4 param_4, undefined4 param_5);
    void BlockWriteCallback(int param_1, int param_2);
    int __CARDWrite(int param_1, undefined4 param_2, uint param_3, undefined4 param_4, undefined4 param_5);
    undefined4 CARDGetXferredBytes(int param_1);
    undefined4 __CARDGetFatBlock(int param_1);
    void WriteCallback(int param_1, int param_2);
    void EraseCallback(int param_1, int param_2);
    undefined4 __CARDAllocBlock(int param_1, uint param_2, undefined4 param_3);
    undefined4 __CARDFreeBlock(int param_1, ushort param_2, undefined4 param_3);
    void __CARDUpdateFatBlock(int param_1, short * param_2, undefined4 param_3);
    undefined4 __CARDGetDirBlock(int param_1);
    void WriteCallback(int param_1, int param_2);
    void EraseCallback(int param_1, int param_2);
    int __CARDUpdateDir(int param_1, undefined4 param_2);
    void __CARDCheckSum(ushort * param_1, uint param_2, short * param_3, short * param_4);
    undefined4 VerifyID(int param_1);
    int VerifyDir(int param_1, uint * param_2);
    int VerifyFAT(int param_1, uint * param_2);
    int __CARDVerify(int param_1);
    int CARDCheckExAsync(int param_1, undefined4 * param_2, undefined * param_3);
    void CARDCheckAsync(int param_1, undefined * param_2);
    undefined4 IsCard(char * param_1);
    s32 CARDProbeEx(s32 chan, s32 * memSize, s32 * sectorSize);
    int DoMount(int param_1);
    void __CARDMountCallback(int param_1, int param_2);
    s32 CARDMountAsync(s32 chan, void * workArea, void * detachCallback, void * attachCallback);
    void DoUnmount(int param_1, undefined4 param_2);
    s32 CARDUnmount(s32 chan);
    void FormatCallback(int param_1, int param_2);
    int __CARDFormatRegionAsync(int param_1, ushort param_2, undefined * param_3);
    void CARDFormatAsync(int param_1, undefined * param_2);
    undefined4 __CARDCompareFileName(int param_1, char * param_2);
    undefined4 __CARDAccess(int param_1, char * param_2);
    undefined4 __CARDIsPublic(char * param_1);
    undefined4 __CARDGetFileNo(int * param_1, char * param_2, int * param_3);
    s32 CARDOpen(s32 chan, char * fileName, struct CARDFileInfo * fileInfo);
    undefined4 return_0(void);
    void CreateCallbackFat(int param_1, int param_2);
    void CARDCreateAsync(s32 chan, char * fileName, u32 size, struct CARDFileInfo * fileInfo, void * callback);
    int __CARDSeek(int * param_1, int param_2, uint param_3, int * * param_4);
    void ReadCallback(int param_1, int param_2);
    s32 CARDReadAsync(struct CARDFileInfo * fileInfo, void * buf, s32 length, s32 offset, void * callback);
    void WriteCallback(int param_1, int param_2);
    void EraseCallback(int param_1, int param_2);
    s32 CARDWriteAsync(struct CARDFileInfo * fileInfo, void * buf, s32 length, s32 offset, void * callback);
    void DeleteCallback(int param_1, int param_2);
    void CARDFastDeleteAsync(s32 chan, s32 fileNo, void * callback);
    int CARDDeleteAsync(int param_1, char * param_2, undefined * param_3);
    void UpdateIconOffsets(int param_1, int param_2);
    s32 CARDGetStatus(s32 chan, s32 fileNo, struct CARDStat * stat);
    int CARDSetStatusAsync(int param_1, int param_2, int param_3, undefined4 param_4);
    int CARDRenameAsync(int param_1, char * param_2, char * param_3, undefined4 param_4);
    int CARDGetSerialNo(int param_1, uint * param_2);
    GXFifoObj * GXInit(void * base, u32 size);
    void __GXInitGX(void);
    void GXInitFifoBase(struct GXFifoObj * fifo, void * base, u32 size);
    void GXInitFifoPtrs(struct GXFifoObj * fifo, void * read_ptr, void * write_ptr);
    void GXInitFifoLimits(struct GXFifoObj * fifo, u32 hi_water_mark, u32 lo_water_mark);
    void GXSetCPUFifo(struct GXFifoObj * fifo);
    void GXSetGPFifo(struct GXFifoObj * fifo);
    void GXSaveCPUFifo(struct GXFifoObj * fifo);
    void __GXSaveCPUFifoAux(int * param_1);
    void GXGetGPStatus(GXBool * overhi, GXBool * underlow, GXBool * readIdle, GXBool * cmdIdle, GXBool * brkpt);
    void GXGetFifoPtrs(struct GXFifoObj * fifo, void * * read_ptr, void * * write_ptr);
    void __GXFifoInit(void);
    void __GXFifoReadEnable(void);
    void __GXFifoReadDisable(void);
    void __GXFifoLink(char param_1);
    void __GXWriteFifoIntEnable(uint param_1, uint param_2);
    void __GXWriteFifoIntReset(uint param_1, uint param_2);
    GXFifoObj * GXGetCPUFifo(void);
    GXFifoObj * GXGetGPFifo(void);
    void __GXXfVtxSpecs(void);
    void GXSetVtxDesc(GXAttr  attr, GXAttrType  type);
    void GXSetVtxDescv(struct GXVtxDescList * param_1);
    void __GXSetVCD(void);
    void g_GX_internal_unknown1(void);
    void GXGetVtxDesc(GXAttr  attr, GXAttrType  * type);
    void GXGetVtxDescv(struct GXVtxDescList * attr_list);
    void GXClearVtxDesc(void);
    void GXSetVtxAttrFmt(GXVtxFmt  vtxfmt, GXAttr  attr, GXCompCnt  cnt, GXCompType  type, u8 frac);
    void GXSetVtxAttrFmtv(GXVtxFmt  vtxfmt, struct GXVtxAttrFmtList * list);
    void __GXSetVAT(void);
    void GXGetVtxAttrFmt(GXVtxFmt  vtxfmt, GXAttr  attr, GXCompCnt  * cnt, GXCompType  * type, u8 * frac);
    void GXGetVtxAttrFmtv(GXVtxFmt  vtxfmt, struct GXVtxAttrFmtList * list);
    void GXSetArray(GXAttr  attr, void * base_ptr, u8 stride);
    void GXInvalidateVtxCache(void);
    void GXSetTexCoordGen2(GXTexCoordID  dst_coord, GXTexGenType  func, GXTexGenSrc  src_param, u32 mtx, GXBool normalize, u32 postmtx);
    void GXSetNumTexGens(u8 nTexGens);
    void GXSetMisc(GXMiscToken  token, u32 value);
    void GXFlush(void);
    void GXResetWriteGatherPipe(void);
    void GXSetDrawSync(u16 token);
    void GXSetDrawDone(void);
    void GXDrawDone(void);
    void GXPixModeSync(void);
    void GXPokeAlphaMode(GXCompare  func, u8 threshold);
    void GXPokeAlphaRead(GXAlphaReadMode  mode);
    void GXPokeAlphaUpdate(GXBool update_enable);
    void GXPokeBlendMode(GXBlendMode  type, GXBlendFactor  src_factor, GXBlendFactor  dst_factor, GXLogicOp  op);
    void GXPokeColorUpdate(GXBool update_enable);
    void GXPokeDstAlpha(GXBool enable, u8 alpha);
    void GXPokeDither(GXBool dither);
    void GXPokeZMode(GXBool compare_enable, GXCompare  func, GXBool update_enable);
    void GXPeekZ(u16 x, u16 y, u32 * z);
    undefined4 GXSetDrawSyncCallback(undefined4 param_1);
    void GXTokenInterruptHandler(__OSInterrupt interrupt, struct OSContext * context);
    undefined4 GXSetDrawDoneCallback(undefined4 param_1);
    void GXFinishInterruptHandler(__OSInterrupt interrupt, struct OSContext * context);
    void __GXPEInit(void);
    void __GXSetDirtyState(undefined4 param_1, undefined4 param_2, uint param_3);
    void GXBegin(GXPrimitive  type, GXVtxFmt  vtxfmt, u16 nverts);
    void __GXSendFlushPrim(void);
    void GXSetLineWidth(u8 width, GXTexOffset  tex_offsets);
    void GXSetPointSize(u8 size, GXTexOffset  tex_offsets);
    void GXEnableTexOffsets(GXTexCoordID  coord, GXBool line_enable, GXBool point_enable);
    void GXSetCullMode(GXCullMode  mode);
    void GXSetCoPlanar(GXBool enable);
    void __GXSetGenMode(void);
    void GXAdjustForOverscan(struct GXRenderModeObj * rmin, struct GXRenderModeObj * rmout, u16 hor, u16 ver);
    void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht);
    void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht);
    void GXSetDispCopyDst(u16 wd, u16 ht);
    void GXSetTexCopyDst(u16 wd, u16 ht, GXTexFmt  fmt, GXBool mipmap);
    void GXSetDispCopyFrame2Field(GXCopyMode  mode);
    void GXSetCopyClamp(GXFBClamp  clamp);
    u32 GXSetDispCopyYScale(undefined yscale);
    void GXSetCopyClear(struct GXColor clear_clr, u32 clear_z);
    void GXSetCopyFilter(GXBool aa, undefined sample_pattern, GXBool vf, u8 vfilter[7]);
    void GXSetDispCopyGamma(GXGamma  gamma);
    void GXCopyDisp(void * dest, GXBool clear);
    void GXCopyTex(void * dest, GXBool clear);
    void GXClearBoundingBox(void);
    void GXInitLightAttnK(struct GXLightObj * lt_obj, float k0, float k1, float k2);
    void GXInitLightSpot(struct GXLightObj * lt_obj, float cutoff, GXSpotFn  spot_func);
    void GXInitLightDistAttn(struct GXLightObj * lt_obj, float ref_distance, float ref_brightness, GXDistAttnFn  dist_func);
    void GXInitLightPos(struct GXLightObj * lt_obj, float x, float y, float z);
    void GXGetLightPos(struct GXLightObj * lt_obj, float * x, float * y, float * z);
    void GXInitLightDir(struct GXLightObj * lt_obj, float nx, float ny, float nz);
    void GXInitLightColor(struct GXLightObj * lt_obj, struct GXColor color);
    void GXGetLightColor(struct GXLightObj * lt_obj, struct GXColor * color);
    void GXLoadLightObjImm(struct GXLightObj * lt_obj, GXLightID  light);
    void GXSetChanAmbColor(GXChannelID  chan, struct GXColor amb_color);
    void GXSetChanMatColor(GXChannelID  chan, struct GXColor mat_color);
    void GXSetNumChans(u8 nChans);
    void GXSetChanCtrl(GXChannelID  chan, GXBool enable, GXColorSrc  amb_src, GXColorSrc  mat_src, GXLightID  light_mask, GXDiffuseFn  diff_fn, GXAttnFn  attn_fn);
    u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, GXBool mipmap, u8 max_lod);
    void __GetImageTileCount(int param_1, ushort param_2, ushort param_3, int * param_4, int * param_5, undefined4 * param_6);
    void GXInitTexObj(struct GXTexObj * obj, void * image_ptr, u16 width, u16 height, GXTexFmt  format, GXTexWrapMode  wrap_s, GXTexWrapMode  wrap_t, GXBool mipmap);
    void GXInitTexObjLOD(struct GXTexObj * obj, GXTexFilter  min_filt, GXTexFilter  mag_filt, float min_lod, float max_lod, float lod_bias, GXBool bias_clamp, GXBool do_edge_lod, GXAnisotropy  max_aniso);
    void GXInitTexObjData(struct GXTexObj * obj, void * image_ptr);
    void GXInitTexObjWrapMode(struct GXTexObj * obj, GXTexWrapMode  wrap_s, GXTexWrapMode  wrap_t);
    void GXGetTexObjAll(struct GXTexObj * obj, void * * image_ptr, u16 * width, u16 * height, GXTexFmt  * format, GXTexWrapMode  * wrap_s, GXTexWrapMode  * wrap_t, GXBool * mipmap);
    void * GXGetTexObjData(struct GXTexObj * obj);
    u16 GXGetTexObjWidth(struct GXTexObj * obj);
    u16 GXGetTexObjHeight(struct GXTexObj * obj);
    GXTexFmt GXGetTexObjFmt(struct GXTexObj * obj);
    GXTexWrapMode GXGetTexObjWrapS(struct GXTexObj * obj);
    GXTexWrapMode GXGetTexObjWrapT(struct GXTexObj * obj);
    GXBool GXGetTexObjMipMap(struct GXTexObj * obj);
    void g_GX_something(uint * param_1, uint * param_2, uint * param_3, float * param_4, float * param_5, float * param_6, byte * param_7, undefined * param_8, uint * param_9);
    void GXLoadTexObjPreLoaded(struct GXTexObj * obj, struct GXTexRegion * region, GXTexMapID  id);
    void GXLoadTexObj(struct GXTexObj * obj, GXTexMapID  id);
    void GXInitTexCacheRegion(struct GXTexRegion * region, GXBool is_32b_mipmap, u32 tmem_even, GXTexCacheSize  size_even, u32 tmem_odd, GXTexCacheSize  size_odd);
    void GXInitTlutRegion(struct GXTlutRegion * region, u32 tmem_addr, GXTlutSize  tlut_size);
    void GXInvalidateTexAll(void);
    GXTexRegion * (* GXSetTexRegionCallback(GXTexRegion * (* f)(struct GXTexObj *, GXTexMapID )))(struct GXTexObj *, GXTexMapID );
    GXTlutRegion (* GXSetTlutRegionCallback(GXTlutRegion (* f)(u32)))(u32);
    void __SetSURegs(int param_1, int param_2);
    void __GXSetSUTexRegs(void);
    void __GXSetTmemConfig(int param_1);
    void GXSetTevIndirect(GXTevStageID  tev_stage, GXIndTexStageID  ind_stage, GXIndTexFormat  format, GXIndTexBiasSel  bias_sel, GXIndTexMtxID  matrix_sel, GXIndTexWrap  wrap_s, GXIndTexWrap  wrap_t, GXBool add_prev, GXBool utc_lod, GXIndTexAlphaSel  alpha_sel);
    void GXSetIndTexMtx(GXIndTexMtxID  mtx_sel, float offset_mtx[2][3], s8 scale_exp);
    void GXSetIndTexCoordScale(GXIndTexStageID  ind_stage, GXIndTexScale  scale_s, GXIndTexScale  scale_t);
    void GXSetIndTexOrder(GXIndTexStageID  ind_stage, GXTexCoordID  tex_coord, GXTexMapID  tex_map);
    void GXSetNumIndStages(u8 nstages);
    void GXSetTevDirect(GXTevStageID  tev_stage);
    void __GXUpdateBPMask(undefined4 param_1, undefined4 param_2, uint param_3);
    void __GXFlushTextureState(void);
    void GXSetTevOp(GXTevStageID  id, GXTevMode  mode);
    void GXSetTevColorIn(GXTevStageID  stage, GXTevColorArg  a, GXTevColorArg  b, GXTevColorArg  c, GXTevColorArg  d);
    void GXSetTevAlphaIn(GXTevStageID  stage, GXTevAlphaArg  a, GXTevAlphaArg  b, GXTevAlphaArg  c, GXTevAlphaArg  d);
    void GXSetTevColorOp(GXTevStageID  stage, GXTevOp  op, GXTevBias  bias, GXTevScale  scale, GXBool clamp, GXTevRegID  out_reg);
    void GXSetTevAlphaOp(GXTevStageID  stage, GXTevOp  op, GXTevBias  bias, GXTevScale  scale, GXBool clamp, GXTevRegID  out_reg);
    void GXSetTevColor(GXTevRegID  id, struct GXColor color);
    void GXSetTevKColor(GXTevKColorID  id, struct GXColor color);
    void GXSetTevKColorSel(GXTevStageID  stage, GXTevKColorSel  sel);
    void GXSetTevKAlphaSel(GXTevStageID  stage, GXTevKAlphaSel  sel);
    void GXSetTevSwapMode(GXTevStageID  stage, GXTevSwapSel  ras_sel, GXTevSwapSel  tex_sel);
    void GXSetTevSwapModeTable(GXTevSwapSel  id, GXTevColorChan  red, GXTevColorChan  green, GXTevColorChan  blue, GXTevColorChan  alpha);
    void GXSetAlphaCompare(GXCompare  comp0, u8 ref0, GXAlphaOp  op, GXCompare  comp1, u8 ref1);
    void GXSetZTexture(GXZTexOp  op, GXTexFmt  fmt, u32 bias);
    void GXSetTevOrder(GXTevStageID  stage, GXTexCoordID  coord, GXTexMapID  map, GXChannelID  color);
    void GXSetNumTevStages(u8 nStages);
    void GXSetFog(GXFogType  type, float startz, float endz, float nearz, float farz, struct GXColor color);
    void GXSetFogRangeAdj(GXBool enable, u16 center, struct GXFogAdjTable * table);
    void GXSetBlendMode(GXBlendMode  type, GXBlendFactor  src_factor, GXBlendFactor  dst_factor, GXLogicOp  op);
    void GXSetColorUpdate(GXBool update_enable);
    void GXSetAlphaUpdate(GXBool update_enable);
    void GXSetZMode(GXBool compare_enable, GXCompare  func, GXBool update_enable);
    void GXSetZCompLoc(GXBool before_tex);
    void GXSetPixelFmt(GXPixelFmt  pix_fmt, GXZFmt16  z_fmt);
    void GXSetDither(GXBool dither);
    void GXSetDstAlpha(GXBool enable, u8 alpha);
    void GXSetFieldMask(GXBool odd_mask, GXBool even_mask);
    void GXSetFieldMode(GXBool field_mode, GXBool half_aspect_ratio);
    void GXDrawCylinder(u8 numEdges);
    void GXDrawSphere(u8 numMajor, u8 numMinor);
    void GXDrawCubeFace(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, int param_4_00, int param_5_00);
    void GXDrawCube(void);
    void __GXSetRange(void);
    void GXBeginDisplayList(void * list, u32 size);
    u32 GXEndDisplayList(void);
    void GXCallDisplayList(void * list, u32 nbytes);
    void GXSetProjection(f32 mtx[4][4], GXProjectionType  type);
    void GXSetProjectionv(f32 * ptr);
    void GXGetProjectionv(float * ptr);
    undefined8 WriteMTXPS4x3(int param_1, undefined4 param_2);
    void WriteMTXPS3x3from3x4(int param_1, undefined4 * param_2);
    undefined8 WriteMTXPS4x2(int param_1, undefined4 param_2);
    void GXLoadPosMtxImm(float mtxPtr[3][4], u32 id);
    void GXLoadNrmMtxImm(float mtxPtr[3][4], u32 id);
    void GXSetCurrentMtx(u32 id);
    void GXLoadTexMtxImm(float * mtxPtr, u32 id, GXTexMtxType  type);
    void GXSetViewportJitter(float xOrig, float yOrig, float wd, float ht, float nearZ, float farZ, u32 field);
    void GXSetViewport(float xOrig, float yOrig, float wd, float ht, float nearZ, float farZ);
    void GXGetViewportv(float * vp);
    void GXSetScissor(u32 xOrig, u32 yOrig, u32 wd, u32 ht);
    void GXGetScissor(u32 * xOrig, u32 * yOrig, u32 * wd, u32 * ht);
    void GXSetScissorBoxOffset(s32 xoffset, s32 yoffset);
    void GXSetClipMode(GXClipMode  mode);
    void __GXSetMatrixIndex(int param_1);
    void GXSetGPMetric(GXPerf0  perf0, GXPerf1  perf1);
    void GXClearGPMetric(void);
    void GXInitXfRasMetric(void);
    void GXReadXfRasMetric(undefined4 * param_1, undefined4 * param_2, undefined4 * param_3, undefined4 * param_4);
    undefined4 ARCInitHandle(void * arc_data, struct ARCHandle * out_arc_handle);
    BOOL32 ARCOpen(struct ARCHandle * arc_handle, char * file, struct ArcFileInfo * arcFileInfo);
    uint arc_path_to_entrynum(struct ARCHandle * arcHandle, char * file);
    void arc_get_dir(int param_1, int param_2, int param_3);
    int ARCGetStartAddrInMem(int * * param_1);
    undefined4 arcGetLength(struct ArcFileInfo * fileInfo);
    undefined4 return_1(void);
    void g_some_perf_init_func(void);
    void PERFGetAllMemMetrics(int param_1, int param_2);
    int PERFInit(int param_1, uint param_2, uint param_3, undefined * param_4, undefined4 param_5, undefined4 param_6);
    void PERFSetEvent(uint param_1, undefined4 param_2, undefined4 param_3);
    void PERFStartFrame(void);
    void PERFEndFrame(void);
    void PERFEventStart(PERFId id);
    void PERFEventEnd(uint param_1);
    void PERFStartAutoSample(void);
    void PERFEndAutoSample(void);
    void PERFStartAutoSampling(double param_1);
    void PERFStopAutoSampling(void);
    void __PERFDrawInit(undefined4 param_1);
    void PERFPreDraw(void);
    void DrawBWBar(int param_1);
    void DrawKey(void);
    void PERFDumpScreen(void);
    void PERFPostDraw(void);
    void g_set_something_with_perf(undefined4 value);
    void ResetNotes(int param_1);
    undefined4 * AllocateNote(undefined4 param_1, byte param_2);
    undefined4 HandleNotes(void);
    void KeyOffNotes(void);
    void seqFreeKeyOffNote(int * param_1);
    uint GetPublicId(int param_1);
    uint seqGetPrivateId(uint param_1);
    void empty_function(void);
    uint seqStartPlay(int * param_1, int * param_2, int param_3, int * param_4, uint * param_5, undefined param_6, undefined2 param_7);
    void HandleMasterTrack(uint param_1);
    void StartPause(int * param_1);
    void seqPause(uint param_1);
    void seqStop(uint param_1);
    void seqKillInstancesByGroupID(short param_1);
    void seqSpeed(uint param_1, undefined2 param_2);
    void seqContinue(uint param_1);
    void seqMute(uint param_1, undefined4 param_2, undefined4 param_3);
    void seqVolume(uint param_1, uint param_2, uint param_3, byte param_4);
    void seqCrossFade(uint * param_1, uint * param_2, char param_3);
    byte * GetStreamValue(byte * param_1, ushort * param_2, short * param_3);
    int GenerateNextTrackEvent(byte param_1);
    void InsertGlobalEvent(int param_1, undefined4 * param_2);
    int HandleEvent(int param_1, byte param_2, uint * param_3);
    void InitTrackEvents(void);
    void InitTrackEventsSection(uint param_1);
    undefined4 HandleTrackEvents(uint param_1, undefined4 param_2);
    void seqHandle(int param_1);
    int seqInit(void);
    void synthSetBpm(int param_1, byte param_2, uint param_3);
    undefined4 synthGetTicksPerSecond(int param_1);
    void synthInitPortamento(int param_1);
    int * do_voice_portamento(byte param_1, char param_2, char param_3, int param_4, undefined4 * param_5);
    int * StartLayer(undefined2 param_1, int param_2, undefined4 param_3, undefined4 param_4, uint param_5, byte param_6, uint param_7, uint param_8, byte param_9, undefined param_10, ushort param_11, undefined2 param_12, int param_13, undefined param_14, undefined param_15, int param_16);
    int * StartKeymap(undefined2 param_1, short param_2, undefined4 param_3, undefined4 param_4, uint param_5, byte param_6, uint param_7, uint param_8, byte param_9, undefined param_10, ushort param_11, undefined2 param_12, uint param_13, undefined param_14, undefined param_15, int param_16);
    int * synthStartSound(uint param_1, int param_2, undefined4 param_3, uint param_4, byte param_5, uint param_6, uint param_7, uint param_8, undefined param_9, ushort param_10, undefined2 param_11, undefined param_12, short param_13, undefined param_14, int param_15);
    void synthAddJob(int * param_1, int * param_2, uint param_3);
    void synthStartSynthJobHandling(int * param_1);
    void synthForceLowPrecisionUpdate(int * param_1);
    void synthKeyStateUpdate(int * param_1);
    void HandleJobQueue(int * param_1, undefined * param_2);
    void HandleFaderTermination(int param_1);
    void synthHandle(uint param_1);
    int * synthFXStart(undefined2 param_1, byte param_2, uint param_3, undefined param_4, int param_5);
    undefined4 synthFXSetCtrl(uint param_1, byte param_2, byte param_3);
    undefined4 synthFXSetCtrl14(uint param_1, byte param_2, uint param_3);
    void synthFXCloneMidiSetup(int param_1, int param_2);
    undefined4 synthSendKeyOff(uint param_1);
    void synthVolume(uint param_1, uint param_2, uint param_3, undefined param_4, undefined4 param_5);
    undefined4 synthIsFadeOutActive(uint param_1);
    void synthSetMusicVolumeType(uint param_1, undefined param_2);
    void synthInit(undefined4 param_1, int param_2);
    void sndSeqSpeed(uint param_1, undefined2 param_2);
    void sndSeqContinue(uint param_1);
    void sndSeqMute(uint param_1, undefined4 param_2, undefined4 param_3);
    void sndSeqVolume(uint param_1, uint param_2, uint param_3, byte param_4);
    undefined2 seqGetMIDIPriority(uint param_1, uint param_2);
    undefined4 g_call_synthFXSetCtrl_with_irq_disabled(uint param_1, byte param_2, byte param_3);
    undefined4 g_call_synthFXSetCtrl14_with_irq_disabled(uint param_1, byte param_2, uint param_3);
    undefined4 sndFXKeyOff(uint param_1);
    int * sndFXStartEx(undefined2 param_1, byte param_2, uint param_3, byte param_4);
    int * sndFXStartParaInfo(undefined2 param_1, byte param_2, uint param_3, byte param_4, byte * param_5);
    uint sndFXCheck(uint param_1);
    void sndVolume(uint param_1, uint param_2, uint param_3);
    void sndMasterVolume(uint param_1, uint param_2, char param_3, char param_4);
    void sndSetAuxProcessingCallbacks(uint param_1, int param_2, undefined4 param_3, char param_4, uint param_5, int param_6, undefined4 param_7, char param_8, uint param_9);
    void synthActivateStudio(uint param_1, undefined param_2, undefined4 param_3);
    void synthDeactivateStudio(uint param_1);
    void synthAddStudioInput(uint param_1, byte * param_2);
    void synthRemoveStudioInput(uint param_1, int param_2);
    void streamInit(void);
    void streamHandle(void);
    void streamCorrectLoops(void);
    void streamKill(int param_1);
    int GetPrivateIndex(int param_1);
    void sndStreamARAMUpdate(int param_1, uint param_2, uint param_3, uint param_4, uint param_5);
    void CheckOutputMode(undefined * param_1, undefined * param_2);
    int sndStreamAllocEx(undefined param_1, undefined4 param_2, int param_3, undefined4 param_4, undefined param_5, undefined param_6, undefined param_7, undefined param_8, undefined param_9, undefined param_10, uint param_11, undefined4 param_12, undefined4 param_13, undefined2 * param_14);
    uint sndStreamAllocLength(int param_1, uint param_2);
    void sndStreamADPCMParameter(int param_1, undefined2 * param_2);
    void sndStreamFrq(int param_1, undefined4 param_2);
    void sndStreamFree(int param_1);
    undefined4 sndStreamActivate(int param_1);
    void sndStreamDeactivate(int param_1);
    undefined4 dataInsertKeymap(ushort param_1, undefined4 param_2);
    undefined4 dataRemoveKeymap(short param_1);
    undefined4 dataInsertLayer(ushort param_1, undefined4 param_2, undefined2 param_3);
    undefined4 dataRemoveLayer(short param_1);
    undefined4 dataInsertCurve(ushort param_1, undefined4 param_2);
    undefined4 dataRemoveCurve(short param_1);
    undefined4 dataInsertSDir(short * param_1, undefined4 param_2);
    undefined4 dataRemoveSDir(short * param_1);
    undefined4 dataAddSampleReference(short param_1);
    undefined4 dataRemoveSampleReference(short param_1);
    undefined4 dataInsertFX(short param_1, int param_2, uint param_3);
    undefined4 dataRemoveFX(short param_1);
    undefined4 dataInsertMacro(uint param_1, undefined4 param_2);
    undefined4 dataRemoveMacro(uint param_1);
    undefined4 dataGetMacro(uint param_1);
    undefined4 dataGetSample(undefined2 param_1, undefined4 * param_2);
    undefined4 dataGetCurve(undefined2 param_1);
    undefined4 dataGetKeymap(undefined2 param_1);
    undefined4 dataGetLayer(undefined2 param_1, undefined2 * param_2);
    int dataGetFX(undefined2 param_1);
    void dataInit(undefined4 param_1, int param_2);
    undefined4 mcmdWait(int param_1, uint * param_2);
    void mcmdLoop(int param_1, uint * param_2);
    void mcmdPlayMacro(int param_1, uint * param_2);
    void mcmdStartSample(int * param_1, uint * param_2);
    void mcmdVibrato(int param_1, uint * param_2);
    void DoSetPitch(int param_1);
    void mcmdSetADSR(int param_1, uint * param_2);
    void mcmdSetPitchADSR(int param_1, undefined4 * param_2);
    void DoPanningSetup(int param_1, uint * param_2, uint param_3);
    uint TranslateVolume(uint param_1, short param_2);
    void DoEnvelopeCalculation(int param_1, uint * param_2, int param_3);
    void mcmdRandomKey(int param_1, uint * param_2);
    void SelectSource(int param_1, int param_2, uint * param_3, undefined4 param_4, uint param_5, uint param_6, uint param_7);
    void mcmdPortamento(int param_1, uint * param_2);
    uint varGet32(int param_1, int param_2, uint param_3);
    int varGet(int param_1, int param_2, uint param_3);
    void varSet32(int param_1, int param_2, uint param_3, undefined4 param_4);
    void mcmdVarCalculation(int param_1, uint * param_2, byte param_3);
    void mcmdSendMessage(int param_1, uint * param_2);
    void mcmdSetKeyGroup(int param_1, uint * param_2);
    void macHandleActive(int * param_1);
    void macHandle(uint param_1);
    void macSampleEndNotify(int * param_1);
    uint macSetExternalKeyoff(int * param_1);
    void macSetPedalState(int * param_1, int param_2);
    void TimeQueueAdd(int param_1);
    void UnYieldMacro(int * param_1, int param_2);
    void macMakeActive(int * param_1);
    void macMakeInactive(int param_1, int param_2);
    int * macStart(uint param_1, byte param_2, byte param_3, short param_4, byte param_5, undefined param_6, undefined param_7, uint param_8, byte param_9, undefined param_10, ushort param_11, undefined param_12, byte param_13, undefined param_14, undefined param_15, int param_16);
    void macInit(void);
    int vidInit(void);
    undefined4 * get_vidlist(uint param_1);
    void vidRemoveVoiceReferences(int param_1);
    undefined4 vidMakeRoot(int param_1);
    int * vidMakeNew(int param_1, int param_2);
    undefined4 vidGetInternalId(uint param_1);
    void voiceRemovePriority(int param_1);
    void voiceSetPriority(int param_1, byte param_2);
    uint voiceAllocate(byte param_1, byte param_2, short param_3, char param_4);
    void voiceFree(int param_1);
    void synthInitAllocationAids(void);
    uint voiceBlock(byte param_1);
    void voiceUnblock(int param_1);
    void voiceKill(int param_1);
    undefined4 voiceKillSound(uint param_1);
    void synthKillVoicesByMacroReferences(ushort * param_1);
    undefined4 voiceIsLastStarted(int param_1);
    void empty_function(void);
    void voiceSetLastStarted(int param_1);
    void voiceResetLastStarted(int param_1);
    void voiceInitLastStarted(void);
    int sndPitchUpOne(uint param_1);
    void sndGetPitch(uint param_1, uint param_2);
    void adsrConvertTimeCents(void);
    undefined4 salChangeADSRState(char * param_1);
    void g_mcmdWaitMs_or_adsrSetup(char * param_1);
    undefined4 adsrStartRelease(char * param_1, uint param_2);
    undefined4 adsrRelease(byte * param_1);
    undefined4 adsrHandle(char * param_1, undefined2 * param_2, short * param_3);
    undefined4 adsrHandleLowPrecision(char * param_1, undefined2 * param_2, short * param_3);
    void vsInit(void);
    uint vsSampleStartNotify(byte param_1);
    void vsSampleEndNotify(uint param_1);
    void vsUpdateBuffer(int param_1, uint param_2);
    void vsSampleUpdates(void);
    void dataInitStack(void);
    void InsertData(uint param_1, int * param_2, byte param_3, int param_4);
    void ScanIDList(ushort * param_1, int * param_2, byte param_3, int param_4);
    void ScanIDListReverse(ushort * param_1, int * param_2, byte param_3, int param_4);
    undefined4 sndPushGroup(int * param_1, short param_2, undefined4 param_3, int * param_4, int * param_5);
    undefined4 sndPopGroup(void);
    uint seqPlaySong(short param_1, short param_2, int * param_3, uint * param_4, char param_5, undefined param_6);
    void sndSeqPlayEx(short param_1, short param_2, int * param_3, uint * param_4, undefined param_5);
    undefined4 salInitDspCtrl(byte param_1, byte param_2, int param_3);
    void salInitHRTFBuffer(void);
    void salActivateStudio(uint param_1, undefined param_2, undefined4 param_3);
    void salDeactivateStudio(uint param_1);
    undefined4 salCheckVolErrorAndResetDelta(undefined2 * param_1, undefined2 * param_2, short * param_3, short param_4, int param_5, ushort param_6);
    void HandleDepopVoice(int param_1, int * param_2);
    void SortVoices(int param_1, int param_2, int param_3);
    void salBuildCommandList(undefined4 param_1, uint param_2);
    undefined4 salSynthSendMessage(int param_1, undefined4 param_2);
    void salActivateVoice(int param_1, byte param_2);
    void salDeactivateVoice(int param_1);
    undefined4 salAddStudioInput(int param_1, byte * param_2);
    undefined4 salRemoveStudioInput(int param_1, int param_2);
    void salHandleAuxProcessing(void);
    void salCalcVolume(double param_1, double param_2, double param_3, char param_4, float * param_5, uint param_6, uint param_7, int param_8, int param_9);
    void UpdateRoomDistances(void);
    void CheckRoomStatus(void);
    void CheckDoorStatus(void);
    void CalcEmitter(int param_1, float * param_2, float * param_3, float * param_4, float * param_5, float * param_6);
    void SetFXParameters(double param_1, double param_2, undefined8 param_3, double param_4, double param_5, int param_6);
    void EmitterShutdown(int * param_1);
    void s3dKillEmitterByFXID(short * param_1, int param_2);
    void AddRunningEmitter(double param_1, int param_2);
    undefined4 AddStartingEmitter(double param_1, double param_2, double param_3, double param_4, double param_5, int param_6);
    void StartContinousEmitters(void);
    void s3dHandle(void);
    void s3dInit(uint param_1);
    int sndInit(byte param_1, undefined param_2, undefined param_3, byte param_4, uint param_5, int param_6);
    void salApplyMatrix(float * param_1, float * param_2, float * param_3);
    void salNormalizeVector(float * param_1);
    void inpSetGlobalMIDIDirtyFlag(uint param_1, uint param_2, uint param_3);
    void inpSetMidiCtrl(byte param_1, byte param_2, byte param_3, byte param_4);
    void inpSetMidiCtrl14(byte param_1, byte param_2, byte param_3, uint param_4);
    void inpResetMidiCtrl(uint param_1, uint param_2, int param_3);
    uint inpGetMidiCtrl(uint param_1, uint param_2, uint param_3);
    undefined4 return_0(void);
    undefined * inpGetChannelDefaults(uint param_1, uint param_2);
    void inpResetChannelDefaults(uint param_1, uint param_2);
    void inpAddCtrl(int param_1, uint param_2, undefined4 param_3, byte param_4, int param_5);
    void inpFXCopyCtrl(uint param_1, int param_2, int param_3);
    void inpSetMidiLastNote(uint param_1, uint param_2, undefined param_3);
    void inpGetMidiLastNote(uint param_1, uint param_2);
    uint _GetInputValue(int param_1, byte * param_2, uint param_3, uint param_4);
    void inpInit(int param_1);
    uint inpTranslateExCtrl(uint param_1);
    undefined4 return_0x82(void);
    undefined4 return_0xa0(void);
    undefined4 return_0xa1(void);
    undefined4 return_0x83(void);
    undefined4 return_0x84(void);
    undefined4 return_0xa2(void);
    undefined4 return_0xa3(void);
    undefined4 return_0xa4(void);
    uint inpGetExCtrl(int param_1, uint param_2);
    void inpSetExCtrl(int param_1, uint param_2, short param_3);
    uint sndRand(void);
    int sndSin(uint param_1);
    int sndBSearch(undefined4 param_1, int param_2, int param_3, int param_4, undefined * param_5);
    void sndConvertMs(int * param_1);
    void sndConvertTicks(uint * param_1, int param_2);
    uint sndConvert2Ms(uint param_1);
    void snd_handle_irq(void);
    undefined4 hwInit(undefined4 * param_1, byte param_2, byte param_3, uint param_4);
    void hwSetTimeOffset(undefined param_1);
    void WPADGetDpdSensitivity(void);
    bool hwIsActive(int param_1);
    void hwSetPriority(int param_1, undefined4 param_2);
    void hwInitSamplePlayback(int param_1, undefined2 param_2, undefined4 * param_3, int param_4, undefined4 param_5, undefined4 param_6, int param_7, char param_8);
    void hwBreak(int param_1);
    void hwSetADSR(int param_1, uint * param_2, byte param_3);
    void hwSetVirtualSampleLoopBuffer(int param_1, undefined4 param_2, undefined4 param_3);
    void hwGetVirtualSampleState(int param_1);
    void hwGetVirtualSampleState(int param_1);
    undefined2 hwGetSampleID(int param_1);
    void hwSetStreamLoopPS(int param_1, undefined param_2);
    void hwStart(int param_1, byte param_2);
    void hwKeyOff(int param_1);
    void hwSetPitch(int param_1, ushort param_2);
    void hwSetSRCType(int param_1, uint param_2);
    void hwSetPolyPhaseFilter(int param_1, uint param_2);
    void hwSetITDMode(int param_1, char param_2);
    void hwSetVolume(double param_1, double param_2, double param_3, int param_4, char param_5, uint param_6, uint param_7);
    void hwOff(int param_1);
    void hwSetAUXProcessingCallbacks(uint param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5);
    int hwGetPos(int param_1);
    void hwFlushStream(int param_1, uint param_2, int param_3, uint param_4, undefined4 param_5, undefined4 param_6);
    void hwInitStream(int param_1);
    void hwExitStream(uint param_1);
    void hwGetStreamPlayBuffer(uint param_1);
    void hwTransAddr(void);
    void hwFrq2Pitch(undefined4 param_1);
    void hwInitSampleMem(undefined4 param_1, int param_2);
    void hwSaveSample(int * param_1, void * * param_2);
    void hwRemoveSample(int param_1, undefined4 param_2);
    void hwSyncSampleMem(void);
    void empty_function(void);
    void sndSetHooks(undefined4 * param_1);
    undefined4 hwGetVirtualSampleID(int param_1);
    uint hwVoiceInStartup(int param_1);
    void aramUploadData(undefined4 param_1, undefined4 param_2, undefined4 param_3, int param_4, undefined4 param_5, undefined4 param_6);
    void aramSyncTransferQueue(void);
    void aramInit(int param_1);
    void aramGetZeroBuffer(void);
    int aramStoreData(void * param_1, int param_2);
    void aramRemoveData(undefined4 param_1, int param_2);
    void InitStreamBuffers(void);
    uint aramAllocateStreamBuffer(int param_1);
    undefined4 aramGetStreamBufferAddress(uint param_1, undefined4 * param_2);
    void aramFreeStreamBuffer(uint param_1);
    void salCallback(void);
    void Destroy(void);
    bool salInitAi(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    void salStartAi(void);
    int salAiGetDest(void);
    undefined4 salInitDsp(void);
    void salCtrlDsp(undefined4 param_1);
    uint salGetStartDelay(void);
    void hwInitIrq(void);
    void hwEnableIrq(void);
    void hwDisableIrq(void);
    void hwIRQEnterCritical(void);
    void hwIRQLeaveCritical(void);
    void salMalloc(void);
    void salFree(void);
    undefined4 sndAuxCallbackUpdateSettingsReverbHI(void * param_1);
    void sndAuxCallbackPrepareReverbHI(void * param_1);
    undefined4 g_something_with_sound8_wrapper(int param_1);
    undefined4 ReverbHICreate(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, void * param_7);
    undefined4 ReverbHIModify(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, void * param_7);
    void DoCrossTalk(undefined8 param_1, undefined8 param_2, uint * param_3, uint * param_4);
    void HandleReverb(uint * param_1, int param_2, int param_3);
    void ReverbHICallback(uint * param_1, uint * param_2, uint * param_3, int param_4);
    void ReverbHIFree(int param_1);
    void do_src1(int * * param_1);
    void do_src2(int * * param_1);
    undefined4 sndAuxCallbackUpdateSettingsChorus(int param_1);
    undefined4 sndAuxCallbackPrepareChorus(int * param_1);
    undefined4 sndAuxCallbackShutdownChorus(void);
    void * g_GXGetTexObjUserData_dup1_maybe_not(struct GXTexObj * obj);
    undefined4 SEQGetState(int param_1);
    u32 g_GXGetTexObjTlut_maybe_not(struct GXTexObj * obj);
    void * g_GXGetTexObjUserData_dup2_maybe_not(struct GXTexObj * obj);
    GXTexFmt g_GXGetTexObjFmt_dup2(struct GXTexObj * obj);
    void PPCMtwpar(void);
    void empty_function(void);
    void empty_function(void);
    void g_GXInitTexObjUserData_dup1(int param_1, undefined4 param_2);
    void g_GXInitTexObjUserData_dup2(int param_1, undefined4 param_2);
    void g_GXInitTexObjUserData_dup3(int param_1, undefined4 param_2);
    void g_GXInitTexObjUserData_dup4(int param_1, undefined4 param_2);
    GXTexFmt g_GXGetTexObjFmt_dup3(struct GXTexObj * obj);
    void g_adxt_set_out_pan(int param_1, int param_2, int param_3);
    void g_GXInitTexObjUserData_dup5(int param_1, undefined4 param_2);
    void empty_function(void);
    void IFFifoAlloc(int param_1);
    void g_create_some_threads(undefined4 * g_optional_initial_thread_priorities);
    void mwidle_thread_entrypoint(void * param);
    void vsync_thread_entrypoint(void * param);
    void safe_thread_entrypoint(void * param);
    void g_adxm_goto_mwidle_border(void);
    void g_something_that_suspends_safe_thread(void);
    void g_something_that_resumes_safe_thread(void);
    void empty_function(void);
    undefined4 return_0(void);
    void g_some_printf_function_0(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void empty_function(void);
    void g_cvFsAddDev(char * param_1, undefined * param_2);
    undefined4 math_init(void);
    float CHUNK__math_rsqrt(float param_1, float param_2);
    float math_sqrt(float param_1);
    float math_rsqrt(double param_1);
    float math_sqrt_rsqrt(double param_1, float * param_2);
    double g_math_unknown1(double param_1, double param_2);
    double g_math_unknown2(double param_1);
    float math_sin(s16 angle);
    void math_sin_cos_v(s16 param_1, float * param_2);
    void g_math_sin_cos_wrapper(s16 angle, float * out_sin, float * out_cos);
    void math_sin_cos(s16 angle);
    void CHUNK__math_sin_cos(s16 angle, float * out_sin, float * out_cos);
    float math_tan(s16 param_1);
    s16 math_atan2(double param_1, double param_2);
    int math_atan(double param_1);
    int CHUNK__math_atan(void);
    int g_math_unk3(double param_1);
    int g_math_unk4(double param_1);
    float vec_dot_normalized_safe(struct Vec * vec1, struct Vec * vec2);
    double CHUNK__vec_dot_normalized_safe(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6);
    void ray_scale(float scale, struct Vec * ray_start, struct Vec * ray_end, struct Vec * out_ray_end);
    void vec_set_len(float len, struct Vec * src, struct Vec * dest);
    float vec_normalize_len(struct Vec * vec);
    float vec_dot_normalized(struct Vec * vec1, struct Vec * vec2);
    undefined8 mtxa_from_identity(void);
    void mtx_from_identity(Mtx * mtx);
    undefined8 mtxa_sq_from_identity(void);
    void mtxa_from_translate(struct Vec * translate);
    void mtxa_from_translate_xyz(float x, float y, float z);
    void mtxa_from_rotate_x(short angle);
    void mtxa_from_rotate_y(short angle);
    void mtxa_from_rotate_z(short angle);
    void mtxa_from_mtxb_translate(struct Vec * vec);
    double mtxa_from_mtxb_translate_xyz(undefined8 param_1, undefined8 param_2, undefined8 param_3);
    void mtxa_normalize_basis(void);
    undefined8 mtxa_push(void);
    void mtxa_pop(void);
    void mtxa_to_mtx(Mtx * mtx);
    void mtxa_from_mtx(Mtx * mtx);
    undefined8 mtxa_peek(void);
    void mtxa_sq_to_mtx(Mtx * mtx);
    void mtxa_sq_from_mtx(Mtx * mtx);
    undefined8 mtxa_from_mtxb(void);
    undefined8 mtxa_to_mtxb(void);
    void mtx_copy(Mtx * src, Mtx * dest);
    void mtxa_invert(void);
    void mtxa_rigid_invert(void);
    void mtxa_mult_right(Mtx * mtx);
    void mtxa_mult_left(Mtx * mtx);
    void mtxa_from_mtxb_mult_mtx(Mtx * mtx);
    void mtx_mult(Mtx * mtx1, Mtx * mtx2, Mtx * dest);
    void mtxa_translate(struct Vec * vec);
    void mtxa_translate_xyz(float x, float y, float z);
    void mtxa_translate_neg(struct Vec * vec);
    void mtxa_translate_neg_xyz(float x, float y, float z);
    void mtxa_scale(struct Vec * vec);
    void mtxa_scale_s(float scale);
    void mtxa_scale_xyz(float x, float y, float z);
    void mtxa_tf_point(struct Vec * src, struct Vec * dest);
    void mtxa_tf_vec(struct Vec * src, struct Vec * dest);
    void mtxa_tf_point_xyz(float x, float y, float z, struct Vec * dest);
    void mtxa_tf_vec_xyz(float x, float y, float z, struct Vec * param_4);
    void mtxa_rigid_inv_tf_point(struct Vec * src, struct Vec * dst);
    void mtxa_rigid_inv_tf_point_xyz(float x, float y, float z, struct Vec * dst);
    void mtxa_rigid_inv_tf_tl(struct Vec * dst);
    void mtxa_rigid_inv_tf_vec(struct Vec * src, struct Vec * dst);
    void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, struct Vec * dst);
    void CHUNK__mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, undefined4 param_4, struct Vec * dst, void * mtxa);
    void mtxa_rotate_x(s16 angle);
    void mtxa_rotate_x_sin_cos(float sin_x_angle, float cos_x_angle);
    void mtxa_rotate_y(s16 angle);
    void mtxa_rotate_y_sin_cos(float sin_y_angle, float cos_y_angle);
    void mtxa_rotate_z(s16 angle);
    void mtxa_rotate_z_sin_cos(float sin_z_angle, float cos_z_angle);
    void mtxa_from_quat(struct Quat * quat);
    void quat_mult(struct Quat * dest, struct Quat * quat1, struct Quat * quat2);
    undefined8 g_math_smth1(int param_1);
    void g_math_unk6(float * param_1);
    void g_math_unk7(double param_1, struct Quat * param_2, float * param_3, float * param_4);
    void g_math_unk8(double param_1, struct Quat * param_2, float * param_3, float * param_4);
    void mtxa_to_quat(struct Quat * out_quat);
    void quat_from_axis_angle(struct Quat * quat, struct Vec * axis, int angle);
    void g_math_unk9_smth_w_quats(double param_1, struct Quat * param_2, struct Vec * param_3);
    void quat_to_axis_angle(struct Quat * quat, undefined4 out_axis);
    void quat_normalize(struct Quat * quat);
    void quat_from_dirs(struct Quat * out_quat, struct Vec * start, struct Vec * end);
    void quat_slerp(float t, struct Quat * dest, struct Quat * quat1, struct Quat * quat2);
    void g_math_quat_smth2(struct Quat * dst, struct Quat * quat1, struct Quat * quat2);
    void g_math_unk10(struct Vec * param_1, struct Vec * param_2);
    void g_math_unk11(struct Vec * param_1, struct Vec * param_2);
    void ray_to_euler(struct Vec * param_1, struct Vec * param_2, struct S16Vec * param_3);
    void ray_to_euler_xy(float * param_1, float * param_2, s16 * param_3, s16 * param_4);
    void vec_to_euler(struct Vec * vec, struct S16Vec * rot);
    void vec_to_euler_xy(struct Vec * vec, s16 * out_x_rot, s16 * out_y_rot);
    void g_math_unk12(struct Vec * param_1, float * param_2);
    void g_math_unk13(undefined4 param_1, undefined4 param_2, struct Vec * param_3);
    void mtxa_to_euler_yxz(s16 * rot_y, s16 * rot_x, s16 * rot_z);
    void mtxa_to_euler(struct S16Vec rot);
    undefined8 g_math_unk14(short * param_1, s16 * param_2, s16 * param_3);
    void g_math_unk15(double param_1, struct Vec * param_2, struct Vec * param_3);
    void g_math_unk16(float param_1, undefined4 param_2, undefined4 param_3);
    void g_math_unk17(double param_1, double param_2, struct Vec * param_3, struct Vec * param_4);
    void g_math_unk18(double param_1, double param_2, struct Vec * param_3, struct Vec * param_4);
    void g_math_unk19(double param_1, double param_2, struct Vec * param_3, struct Vec * param_4);
    void g_init_console_gx(void);
    void gx_start_new_frame(void);
    void gp_wait(void);
    void video_finish_frame(void);
    u32 get_gx_fifo_use_size2(void);
    void g_gx_reset_z_and_alpha_compare_modes(void);
    int swap_gx_cpu_fifo(void);
    void wait_for_gp_read_idle(void);
    void gx_draw_done_callback_func(void);
    void g_set_gx_vtx_formats(void);
    void init_loched_cache(BOOL32 use_locked_cache);
    void * g_something_with_locked_cache(uint param_1);
    void g_make_arena_simulate_24mb_console(void);
    void auto_set_render_mode(BOOL32 use_progressive_for_ntsc);
    void g_init_gx(BOOL32 make_second_fifo, u32 fifo_size, int g_something_with_alpha);
    void g_init_locked_cache_mtx_stack(int matrix_stack, int param_2);
    void * allocate_mem_from_arena(int size);
    void g_set_some_func_ptrs2(DVDFileInfo * (** param_1)(void));
    void g_set_some_dvd_func_ptrs(void);
    void * (* set_alloc_from_heap_func_ptr(void * (* new_func)(u32)))(u32);
    void (* set_free_to_heap_func_ptr(void (* new_func)(void *)))(void *);
    void reset_heap_alloc_func_ptrs(void);
    int * g_load_raw_gma(char * gma_file_name, struct TplBuffer * tpl);
    GmaBuffer * g_load_gma(char * file_path, struct TplBuffer * tpl);
    void g_something_freeing_heap(int * param_1);
    TplBuffer * g_load_tpl(char * tpl_file_path);
    void g_something_freeing_heap_2(void * param_1);
    void g_panic_in_avload(char * param_1, undefined4 param_2);
    void * alloc_from_set_heap(u32 size);
    void g_free_mem_to_heap_via_func_ptr(void * mem);
    void g_init_gma_texture(struct GmaTextureDescriptor * tex_desc, struct TplTextureHeader * tpl_texture, struct TplBuffer * tpl);
    void os_alloc_from_heap_wrapper(u32 size);
    void os_free_to_heap_wrapper(void * ptr);
    DVDFileInfo * g_get_some_dvd_file_info(void);
    BOOL32 dvd_open_wrapper(char * file_name, struct DVDFileInfo * file_info);
    s32 dvd_read_with_prio2(struct DVDFileInfo * fileInfo, void * addr, s32 length, s32 offset);
    BOOL32 dvd_close_wrapper(struct DVDFileInfo * file_info);
    void empty_function(void);
    void g_something_with_fog(double param_1, double param_2, int param_3);
    void g_something_with_raster_state(int param_1);
    void g_set_some_projection_matrix(double fov, double aspect_ratio, double near_clip, double far_clip);
    void gx_begin_display_list_wrapper(void * list, u32 size);
    u32 gx_end_display_list_wrapper(void);
    void g_zero_something(void);
    void g_something_with_GX_vtx_desc(uint param_1);
    void empty_function(void);
    void g_load_gpu_transform_mtx(Mtx * mtx, int param_2);
    undefined8 g_smth_with_gpu_and_locked_cache_mtx(void);
    void g_something_with_texture_scroll_3(undefined4 param_1, Mtx * param_2);
    void g_some_GmaSomeStruct_func7(struct GmaSomeStruct * gma_struct, GXTevStageID  * param_2);
    double g_some_GmaSomeStruct_func(struct GmaSomeStruct * param_1);
    double g_avdisp_smth_with_tex_materials(struct GmaModelHeader * gma_header, struct GmaSomeStruct * gma_struct);
    void g_some_GmaSomeStruct_func3(struct GmaSomeStruct * gma_struct, int g_some_gma_value);
    void g_something_with_GmaSomeStruct_GXBlendFactor(struct GmaSomeStruct * gma_struct);
    void g_some_GmaSomeStruct_func5_GXSetChanCtrl(struct GmaSomeStruct * gma_struct);
    void g_some_GmaSomeStruct_func6(struct GmaSomeStruct * gma_struct, int param_2);
    undefined8 g_some_GmaSomeStruct_func5(struct GmaSomeStruct * gma_struct);
    void g_some_GmaSomeStruct_func4(struct GmaSomeStruct * param_1);
    void g_free_some_memory(void);
    Mtx * draw_poly(int param_1, Mtx * * param_2);
    uint pointer_range_advance(byte * g_frame_pointer, int * toset);
    void g_avdisp_root_func1(struct GmaModelHeader * gma_header);
    void g_avdisp_root_func2(struct GmaModelHeader * gma_model);
    void g_avdisp_root_func3(struct GmaModelHeader * param_1);
    void g_write_to_gfx_fifos(struct GmaSomeStruct * param_1, undefined4 param_2);
    int * g_some_GmaSomeStruct_func8(uint * param_1, int param_2, struct GmaSomeStruct * gma_struct, int param_4, struct GmaModelHeader * gma_header);
    undefined8 g_avdisp_smth_with_transforms(struct GmaSomeStruct * vert_control);
    void g_avdisp_smth_with_skin_effective_model(struct GmaModelHeader * gma_header, struct GmaSomeStruct * gma_struct, int param_3);
    void g_avdisp_func9(struct GmaModelHeader * param_1, struct GmaSomeStruct * param_2, int param_3, int param_4);
    uint * g_avdisp_static_models(struct GmaModelHeader * gma_model, struct GmaSomeStruct * gma_struct, int g_some_gma_field_value);
    void g_some_GmaSomeStruct_func2(struct GmaSomeStruct * param_1);
    void g_smth_with_gma_model_matrices(struct GmaModelHeader * gma_model);
    void g_avdisp_func8(int param_1);
    void g_maybe_something_with_normals(int param_1);
    void g_init_gma(struct GmaBuffer * gma_buffer, struct GmaHeader * gma_header, struct TplBuffer * tpl);
    int g_init_gma_model_materials(struct GmaModelHeader * model, struct TplBuffer * tpl, struct GXTexObj * texobj_array);
    void g_memcpy_using_locked_cache(void * dest, void * curr_src_1_1_1_1, size_t count);
    void g_something_with_locked_cache_2(void * param_1, uint param_2, uint param_3);
    void memcpy2(void * dest, void * src, size_t count);
    int * __va_arg(char * param_1, int param_2);
    void __destroy_global_chain(void);
    int __cvt_fp2unsigned(double param_1);
    void _savefpr_14(void);
    void _savefpr_15(void);
    void _savefpr_16(void);
    void _savefpr_17(void);
    void _savefpr_18(void);
    void _savefpr_19(void);
    void _savefpr_20(void);
    void _savefpr_21(void);
    void _savefpr_22(void);
    void _savefpr_23(void);
    void _savefpr_24(void);
    void _savefpr_25(void);
    void _savefpr_26(void);
    void _savefpr_27(void);
    void _savefpr_28(void);
    void _savefpr_29(void);
    void _savefpr_30(void);
    void _savefpr_31(void);
    void _restfpr_14(void);
    void _restfpr_15(void);
    void _restfpr_16(void);
    void _restfpr_17(void);
    void _restfpr_18(void);
    void _restfpr_19(void);
    void _restfpr_20(void);
    void _restfpr_21(void);
    void _restfpr_22(void);
    void _restfpr_23(void);
    void _restfpr_24(void);
    void _restfpr_25(void);
    void _restfpr_26(void);
    void _restfpr_27(void);
    void _restfpr_28(void);
    void _restfpr_29(void);
    void _restfpr_30(void);
    void _restfpr_31(void);
    void _savegpr_14(void);
    void _savegpr_15(void);
    void _savegpr_16(void);
    void _savegpr_17(void);
    void _savegpr_18(void);
    void _savegpr_19(void);
    void _savegpr_20(void);
    void _savegpr_21(void);
    void _savegpr_22(void);
    void _savegpr_23(void);
    void _savegpr_24(void);
    void _savegpr_25(void);
    void _savegpr_26(void);
    void _savegpr_27(void);
    void _savegpr_28(void);
    void _savegpr_29(void);
    void _savegpr_30(void);
    void _savegpr_31(void);
    void _restgpr_14(void);
    void _restgpr_15(void);
    void _restgpr_16(void);
    void _restgpr_17(void);
    void _restgpr_18(void);
    void _restgpr_19(void);
    void _restgpr_20(void);
    void _restgpr_21(void);
    void _restgpr_22(void);
    void _restgpr_23(void);
    void _restgpr_24(void);
    void _restgpr_25(void);
    void _restgpr_26(void);
    void _restgpr_27(void);
    void _restgpr_28(void);
    void _restgpr_29(void);
    void _restgpr_30(void);
    void _restgpr_31(void);
    undefined8 __div2u(uint param_1, uint param_2, int param_3, uint param_4);
    undefined8 __div2i(uint param_1, uint param_2, uint param_3, uint param_4);
    undefined8 __mod2u(uint param_1, uint param_2, int param_3, uint param_4);
    void empty_function(void);
    undefined8 __mod2i(uint param_1, uint param_2, int param_3, uint param_4);
    undefined8 __shl2i(int param_1, uint param_2, int param_3);
    undefined8 __shr2u(uint param_1, uint param_2, int param_3);
    undefined8 __shr2i(int param_1, uint param_2, uint param_3);
    double __cvt_sll_flt(uint param_1, uint param_2);
    void __cvt_dbl_usll(ulonglong param_1);
    undefined4 OSGetStackPointer(void);
    void __fini_cpp_exceptions(void);
    void __init_cpp_exceptions(void);
    void __unregister_fragment(int param_1);
    undefined4 __register_fragment(undefined4 param_1, undefined4 param_2);
    void free(int * * __ptr);
    void deallocate_from_fixed_pools(int * * param_1, int * * param_2, uint param_3);
    uint * soft_allocate_from_var_pools(int * param_1, int param_2, uint * param_3);
    void SubBlock_merge_next(uint * param_1, uint * * param_2);
    void Block_link(int param_1, uint * param_2);
    undefined4 __flush_all(void);
    void __close_all(void);
    uint * __find_unopened_file(void);
    void __num2dec(double param_1, int param_2, char * param_3);
    void __num2dec_internal(double param_1, char * param_2);
    uint __equals_dec(int param_1, int param_2);
    void __two_exp(undefined4 * param_1, ushort param_2);
    void __timesdec(undefined * param_1, int param_2, int param_3);
    void __ull2dec(undefined * param_1, undefined4 param_2, uint param_3, uint param_4);
    int __count_trailing_zerol(uint param_1);
    int __flush_buffer(undefined4 * param_1, undefined4 * param_2);
    void __prep_buffer(int param_1);
    int g_very_similar_to_puts(char * str);
    uint __put_char(uint param_1, FILE * param_2);
    int to_lower(int c);
    size_t __fwrite(void * buffer, size_t size, size_t count, FILE * stream);
    size_t __fwrite2(void * buffer, size_t size, size_t count, FILE * stream);
    int fflush(FILE * __stream);
    int fclose(FILE * __stream);
    int _fseek(FILE * __stream, long __off, int __whence);
    int ftell(int __stream);
    uint wcstombs(int __s, ushort * __pwcs, uint __n);
    uint unicode_to_UTF8(int param_1, uint param_2);
    int memcmp(void * __s1, void * __s2, size_t __n);
    char * __memrchr(int param_1, char param_2, int param_3);
    void * memchr(void * __s, int __c, size_t __n);
    void * memmove(void * __dest, void * __src, size_t __n);
    void __copy_longs_rev_unaligned(int param_1, int param_2, uint param_3);
    void __copy_longs_unaligned(int param_1, int param_2, uint param_3);
    void __copy_longs_rev_aligned(int param_1, int param_2, uint param_3);
    void __copy_longs_aligned(int param_1, int param_2, uint param_3);
    void __sinit_AILoader_cpp(void);
    int sprintf(char * buffer, char * format, ...);
    s32 vsprintf(char * out_str, char * format_str, va_list args);
    int vprintf(char * format, char * param_2);
    int printf(char * format, ...);
    void * __StringWrite(struct __OutStrCtrl * osc, char * Buffer, size_t NumChars);
    FILE * __FileWrite(FILE * File, char * Buffer, size_t NumChars);
    int __pformatter(void (* WriteProc)(void *, char *, size_t), void * WriteProcArg, char * format_str, va_list arg, int is_secure);
    byte * float2str(double param_1, int param_2, int param_3);
    void round_decimal(int param_1, int param_2);
    char * double2hex(double param_1, int param_2, int param_3);
    char * longlong2str(uint param_1, uint param_2, int param_3, char * param_4);
    char * long2str(uint param_1, int param_2, char * param_3);
    char * parse_format(int param_1, char * param_2, uint * param_3);
    void srand(u32 seed);
    int rand(void);
    byte * __StringRead(byte * * param_1, byte * param_2, int param_3);
    char * strstr(char * __haystack, char * __needle);
    char * strchr(char * __s, int __c);
    int strncmp(char * __s1, char * __s2, size_t __n);
    int strcmp(char * __s1, char * __s2);
    char * strncat(char * __dest, char * __src, size_t __n);
    void strcat(char * dest, char * src);
    char * strncpy(char * __dest, char * __src, size_t __n);
    char * strcpy(char * __dest, char * __src);
    size_t strlen(char * __s);
    double atof(char * __nptr);
    int atoi(char * __nptr);
    uint __strtoul(uint param_1, uint param_2, undefined * param_3, undefined4 param_4, int * param_5, undefined4 * param_6, undefined4 * param_7);
    undefined4 return_0(void);
    undefined4 __write_console(undefined4 param_1, byte * param_2, uint * param_3);
    int fwide(FILE * stream, int mode);
    double __ieee754_acos(double __x);
    double pow_internal(double x, double y);
    uint __ieee754_rem_pio2(double param_1, double * param_2);
    double __kernel_cos(double param_1, double param_2);
    uint __kernel_rem_pio2(double * param_1, double * param_2, int param_3, int param_4, int param_5, int param_6);
    double __kernel_sin(double param_1, double param_2, int param_3);
    double __kernel_tan(double param_1, double param_2, uint param_3);
    double atan(double __x);
    double copysign(double __x, double __y);
    double cos(double __x);
    double floor(double __x);
    double frexp(double x, int * exponent);
    double ldexp(double __x, int __exponent);
    double modf(double __x, double * __iptr);
    double sin(double __x);
    double tan(double __x);
    float acosf(float __x);
    double pow(double x, double y);
    float floorf(float __x);
    float powf(float __x);
    float tanf(float __x);
    float sinf(float __x);
    float cosf(float __x);
    void TRKNubMainLoop(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void TRKDestructEvent(int param_1);
    void TRKConstructEvent(undefined * param_1, undefined param_2);
    undefined4 TRKPostEvent(int param_1);
    undefined4 TRKGetNextEvent(int param_1);
    undefined4 TRKInitializeEventQueue(void);
    void TRKNubWelcome(void);
    undefined4 TRKTerminateNub(void);
    int TRKInitializeNub(void);
    void TRKMessageSend(int param_1);
    void TRKReadBuffer_ui32(int param_1, undefined * param_2, int param_3);
    void TRKReadBuffer_ui8(int param_1, int param_2, int param_3);
    int TRKReadBuffer1_ui64(int param_1, undefined * param_2);
    int TRKReadBuffer1_ui32(int param_1, undefined * param_2);
    int TRKReadBuffer1_ui16(int param_1, undefined * param_2);
    undefined4 TRKReadBuffer1_ui8(int param_1, int param_2);
    void TRKAppendBuffer_ui32(int param_1, undefined4 * param_2, int param_3);
    int TRKAppendBuffer_ui8(int param_1, undefined * param_2, int param_3);
    void TRKAppendBuffer1_ui64(int param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
    void TRKAppendBuffer1_ui32(int param_1, undefined4 param_2);
    void TRKAppendBuffer1_ui16(int param_1, undefined2 param_2);
    undefined4 TRKReadBuffer(int param_1, int param_2, uint param_3);
    undefined4 TRKAppendBuffer(int param_1, undefined * param_2, uint param_3);
    undefined4 TRKSetBufferPosition(int param_1, uint param_2);
    void TRKResetBuffer(int param_1, char param_2);
    void TRKReleaseBuffer(int param_1);
    undefined * TRKGetBuffer(int param_1);
    int TRKGetFreeBuffer(int * param_1, undefined4 * param_2);
    undefined4 TRKInitializeMessageBuffers(void);
    undefined4 TRKTerminateSerialHandler(void);
    undefined4 TRKInitializeSerialHandler(void);
    void TRKProcessInput(undefined4 param_1);
    void TRKGetInput(void);
    int TRKTestForPacket(void);
    void usr_put_initialize(void);
    void usr_puts_serial(char * param_1);
    undefined4 TRKDispatchMessage(int param_1);
    undefined4 TRKTargetStop(void);
    void TRKDoSetOption(int param_1);
    void TRKDoStop(int param_1);
    void TRKDoStep(int param_1);
    void TRKDoContinue(int param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void TRKDoFlushCache(int param_1);
    void TRKDoWriteRegisters(int param_1);
    void TRKDoReadRegisters(int param_1);
    void TRKDoWriteMemory(int param_1);
    void TRKDoReadMemory(int param_1);
    void TRKDoCPUType(int param_1);
    void TRKDoSupportMask(int param_1);
    void TRKDoVersions(int param_1);
    undefined4 TRKDoReset(int param_1);
    int TRKDoDisconnect(int param_1);
    void TRKDoConnect(int param_1);
    void TRKDoUnsupported(int param_1);
    void TRKStandardACK(int param_1, undefined param_2, undefined param_3);
    void SetTRKConnected(undefined4 param_1);
    int HandlePositionFileSupportRequest(undefined4 param_1, undefined4 * param_2, undefined param_3, undefined * param_4);
    int HandleCloseFileSupportRequest(undefined4 param_1, undefined * param_2);
    int HandleOpenFileSupportRequest(char * param_1, undefined param_2, undefined4 * param_3, undefined * param_4);
    int TRKRequestSend(int param_1, int * param_2, uint param_3, int param_4, int param_5);
    int TRKSuppAccessFile(int param_1, int param_2, uint * param_3, char * param_4, int param_5, int param_6);
    undefined4 return_0(void);
    undefined4 return_0(void);
    undefined4 return_0(void);
    int TRKDoNotifyStopped(char param_1);
    void TRK_flush_cache(uint param_1, int param_2);
    void TRK_fill_mem(int param_1, uint param_2, uint param_3);
    void __TRK_get_MSR(void);
    void __TRK_set_MSR(void);
    void TRK_ppc_memcpy(int param_1, int param_2, int param_3);
    undefined4 TRKPPCAccessFPRegister(undefined4 * param_1, uint param_2, int param_3);
    undefined4 TRKTargetStop(void);
    undefined4 TRKTargetStopped(void);
    undefined4 TRKTargetFlushCache(undefined4 param_1, uint param_2, uint param_3);
    int TRKTargetSupportRequest(void);
    undefined4 TRKTargetGetPC(void);
    undefined4 TRKTargetStepOutOfRange(undefined4 param_1, undefined4 param_2, int param_3);
    undefined4 TRKTargetSingleStep(int param_1, int param_2);
    void TRKTargetAddExceptionInfo(int param_1);
    void TRKTargetAddStopInfo(int param_1);
    int TRKTargetInterrupt(byte * param_1);
    undefined4 TRKTargetCPUType(undefined * param_1);
    undefined4 TRKTargetSupportMask(undefined * param_1);
    undefined4 TRKTargetVersions(undefined * param_1);
    int TRKTargetAccessExtended2(uint param_1, uint param_2, int param_3, int * param_4, int param_5);
    uint TRKTargetAccessExtended1(uint param_1, uint param_2, int param_3, int * param_4, int param_5);
    int TRKTargetAccessFP(uint param_1, uint param_2, int param_3, int * param_4, int param_5);
    undefined4 TRKTargetAccessDefault(int param_1, uint param_2, int param_3, int * param_4, int param_5);
    int TRKTargetAccessMemory(int param_1, uint param_2, int * param_3, undefined4 param_4, int param_5);
    int TRKValidMemory32(uint param_1, int param_2, uint param_3);
    void g_twi_0(void);
    void g_twi_0(void);
    void g_twi_0(void);
    void g_twi_0(void);
    undefined8 InitMetroTRK(undefined4 param_1, undefined4 param_2, int param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    undefined4 TRKInitializeTarget(void);
    uint TRKTargetTranslate(uint param_1);
    void EnableMetroTRKInterrupts(void);
    void TRKSaveExtended1Block(void);
    void TRKRestoreExtended1Block(void);
    undefined4 TRKTargetCPUMinorType(void);
    void TRK_main(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    undefined4 * TRKLoadContext(undefined4 * param_1, int param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void empty_function(void);
    void TRK_board_display(char * param_1);
    void UnreserveEXI2Port(void);
    void UnreserveEXI2Port(void);
    int TRKReadUARTPoll(undefined * param_1);
    undefined4 WriteUART1(undefined param_1);
    int WriteUARTFlush(void);
    void UnreserveEXI2Port(void);
    undefined4 TRKInitializeIntDrivenUART(void);
    void InitMetroTRKCommTable(int param_1);
    void TRKEXICallBack(undefined4 param_1, undefined4 * param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    undefined4 TRKTargetContinue(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void SetUseSerialIO(undefined param_1);
    undefined4 __position_file(void);
    undefined4 __close_file(void);
    undefined4 __write_file(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    undefined4 __write_file(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    undefined4 __TRK_write_console(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    undefined4 __TRK_write_console(undefined4 param_1, undefined4 param_2, undefined4 * param_3);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    undefined4 DBWrite(undefined4 * param_1, uint param_2);
    undefined4 DBRead(undefined4 * param_1, int param_2);
    uint DBQueryData(void);
    void DBInitInterrupts(void);
    void DBInitComm(undefined4 * param_1, undefined4 param_2);
    void DBGHandler(short param_1);
    void MWCallback(void);
    uint DBGReadStatus(byte * param_1);
    uint DBGWrite(uint param_1, undefined4 * param_2, int param_3);
    uint DBGRead(uint param_1, undefined4 * param_2, int param_3);
    uint DBGReadStatus(byte * param_1);
    undefined4 DBGEXIImm(byte * param_1, int param_2, int param_3);
    undefined4 return_0(void);
    undefined4 return_0(void);
    void load_additional_rel(char * rel_filepath, struct RelBufferInfo * rel_buffer_ptrs);
    void g_purge_rel(struct RelBufferInfo * rel_buffer_info);
    void g_purge_rel2(void);
    void main_loop_prolog(void);
    void main_loop_epilog(void);
    void main_loop_unlinked_func(void);
    void g_something_with_sound6_and_stop_pad_motors_callback(void);
    void g_something_with_card_callback(void);
    void g_something_with_sound11_callback(void);
    void revert_console_reset_callback(void);
    uint console_reset_callback_impl(void);
    void g_init_some_stuff(void);
    void cd_to_dvd_root2(void);
    void cd_to_dvd_root(void);
    void g_create_initial_main_heap(void);
    void g_remake_initial_main_heap_somehow(void);
    void g_setup_and_create_game_heaps(void);
    void g_something_with_sound7_and_game_heaps(int param_1);
    void create_game_heaps(int heap_config_idx);
    void destroy_game_heaps(void);
    void * alloc_from_heap_or_panic(OSHeapHandle heap, u32 size, char * file, int line);
    void g_set_some_initial_state(void);
    void mode_tick(void);
    undefined4 g_something_with_title_debug_menu(void);
    void handle_start_button_with_debug_mode(void);
    void handle_start_button_no_debug_mode(void);
    void smd_null(void);
    void g_set_current_sub_mode_dest(undefined4 param_1);
    void g_maybe_call_some_func_ptr_related_to_sub_mode(void);
    uint get_next_player_idx(void);
    int g_get_next_stage_id(void);
    void g_something_with_pausemenu(int param_1);
    void g_check_input_in_pausemenu(int param_1);
    void handle_pausemenu_selection(int param_1);
    void g_related_to_pausemenu(void);
    void init_events(void);
    void tick_events(void);
    void event_init(EventID  event_id);
    void event_dest(EventID  event_id);
    void event_freeze(EventID  event_id);
    void event_restart(EventID  event_id);
    void dest_all_events(void);
    void g_draw_3d(undefined8 param_1, undefined8 param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, undefined4 param_9, undefined4 param_10, undefined4 * param_11, int param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void draw_mode(undefined8 param_1, undefined8 param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, undefined4 param_9, undefined4 param_10, undefined4 * param_11, int param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void draw_mode_adv(void);
    void g_draw_world(void);
    void g_set_clear_color(void);
    void g_smth_with_bg_color_drawing(struct GXColor * param_1);
    void g_something_with_view_stage(void);
    void take_pausemenu_screenshot(void * out_image_buffer, undefined4 src_left_px, undefined4 src_top_px, short width_px, short height_px, GXTexFmt  fmt);
    void init_pausemenu_screenshot_texobj(struct GXTexObj * param_1);
    void g_draw_pausemenu_screenshot(struct GXTexObj * tex);
    void md_adv_func(void);
    void empty_function(void);
    void empty_function(void);
    void smd_adv_logo_init(void);
    void smd_adv_logo_tick(void);
    void smd_adv_demo_init(void);
    void smd_adv_demo_tick(void);
    void smd_adv_movie_init(void);
    void smd_adv_movie_return(void);
    void g_smth_with_adv_sprites_and_stage_loading(s32 param_1);
    void smd_adv_title_init(void);
    void smd_adv_title_reinit(void);
    void smd_adv_title_tick(void);
    void smd_adv_info_init(void);
    void smd_adv_info_tick(void);
    void smd_adv_game_ready_init(void);
    void smd_adv_game_ready_tick(void);
    void smd_adv_game_play_init(void);
    void smd_adv_game_play_tick(void);
    void smd_adv_ranking_init(void);
    void smd_adv_ranking_tick(void);
    undefined4 return_0x2c(void);
    void smd_adv_start_init(void);
    void smd_adv_start_tick(void);
    void g_init_some_globals_for_adv(void);
    void smd_adv_prog_init(void);
    void smd_adv_prog_tick(void);
    void smd_adv_first_logo_init(void);
    void smd_adv_first_logo_tick(void);
    void g_maybe_draw_sega_splash(void);
    void g_something_with_loading_stages_on_gamestart(void);
    void g_something_w_adv_demo_init(void);
    undefined4 adv_demo_tick_function(void);
    void g_camera_func30_child(struct Camera * camera);
    void create_demo_mes_sprite(void);
    void sprite_demo_mes_tick(undefined4 param_1, int param_2);
    void sprite_demo_mes_disp(struct Sprite * param_1);
    void create_demo_mask_sprites(void);
    void sprite_demo_mask_banana_tick(u8 * status, struct Sprite * sprite);
    void g_set_lots_of_initial_state(void);
    void md_sel_func(void);
    void smd_sel_ngc_dest(void);
    void g_return_to_sel_mode(undefined4 param_1);
    void g_load_stage_for_menu_bg(char param_1, int param_2);
    void smd_sel_stage_init_and_ngc_init_and_ngc_reinit(void);
    void smd_sel_stage_and_ngc_tick(void);
    void init_cameras(void);
    void event_camera_init(void);
    void event_camera_tick(void);
    void g_some_camera_parent_func(undefined param_1, undefined param_2, undefined param_3, undefined param_4, undefined param_5, undefined param_6, undefined param_7, undefined param_8, undefined4 param_9, undefined4 param_10, undefined4 param_11, undefined8 param_12, undefined8 param_13, undefined8 param_14, undefined8 param_15, undefined4 param_16, undefined4 param_17);
    void event_camera_dest(void);
    void enable_camera(int camera_idx);
    void disable_all_cameras(void);
    void g_something_setting_camera_status(void);
    void set_camera_view_settings(float viewport_pos_x, float viewport_pos_y, float viewport_size_x, float viewport_size_y, undefined4 camera_idx);
    void g_something_with_camera2(int player_number);
    void g_some_camera_setup_function(void);
    void g_smth_with_camera_and_reflective_objects(int param_1);
    void g_set_mode_of_all_cameras_optionally(u8 param_1);
    void g_set_camera_mode(uint param_1, byte mode);
    void g_camera_func27(struct Camera * camera, struct Ball * ball);
    void g_camera_func28(struct Camera * camera, struct Ball * ball);
    void g_camera_func29(struct Camera * camera, struct Ball * ball);
    void g_camera_func30(struct Camera * camera, struct Ball * ball);
    void g_camera_func54(struct Camera * camera, struct Ball * ball);
    void g_camera_func55(struct Camera * camera, struct Ball * ball);
    void g_camera_func43(struct Camera * camera, struct Ball * ball);
    void g_camera_func12(struct Camera * camera, struct Ball * ball);
    void g_camera_func13(struct Camera * camera, struct Ball * ball);
    void g_camera_func52(struct Camera * camera, struct Ball * ball);
    void g_camera_func53(struct Camera * camera, struct Ball * ball);
    void g_camera_func80(struct Camera * camera, struct Ball * ball);
    void g_camera_func81_mainmenu(struct Camera * camera, struct Ball * ball);
    void g_camera_func87(struct Camera * camera, struct Ball * ball);
    void g_camera_func88(struct Camera * camera, struct Ball * ball);
    void g_camera_func10(struct Camera * camera, struct Ball * ball);
    void g_camera_func11_spinin(struct Camera * camera, struct Ball * ball);
    void g_camera_func38(struct Camera * camera, struct Ball * ball);
    void g_camera_func0(struct Camera * camera, struct Ball * ball);
    void g_camera_func1(struct Camera * camera, struct Ball * ball);
    void g_camera_func2(struct Camera * camera, struct Ball * ball);
    void g_camera_func3_setup(struct Camera * camera, struct Ball * ball);
    void g_camera_func62(struct Camera * camera, struct Ball * ball);
    void g_camera_func4_fallout(struct Camera * camera, struct Ball * ball);
    void g_camera_func5(struct Camera * camera, struct Ball * ball);
    void g_camera_func6(struct Camera * camera, struct Ball * ball);
    void g_camera_func7(struct Camera * camera, struct Ball * ball);
    void g_camera_func8(struct Camera * camera, struct Ball * ball);
    void g_camera_func9(struct Camera * camera, struct Ball * ball);
    void g_camera_func14(struct Camera * camera, struct Ball * ball);
    void g_camera_func15_goal(struct Camera * camera, struct Ball * ball);
    void g_camera_func16(struct Camera * camera, struct Ball * ball);
    void g_camera_func17(struct Camera * camera, struct Ball * ball);
    void g_camera_func18(struct Camera * camera, struct Ball * ball);
    void g_camera_func19(struct Camera * camera, struct Ball * ball);
    void g_camera_func20(struct Camera * camera, struct Ball * ball);
    void g_camera_func21(struct Camera * camera, struct Ball * ball);
    void g_camera_func22_23_24_timeover(struct Camera * camera, struct Ball * ball);
    void g_camera_func25(struct Camera * camera, struct Ball * ball);
    void g_camera_func26(struct Camera * camera, struct Ball * ball);
    void g_camera_func31(struct Camera * camera, struct Ball * ball);
    void g_camera_func32(struct Camera * camera, struct Ball * ball);
    void g_camera_func33(struct Camera * camera, struct Ball * ball);
    void g_camera_func34(struct Camera * camera, struct Ball * ball);
    void g_camera_func35(struct Camera * camera, struct Ball * ball);
    void g_camera_func36(struct Camera * camera, struct Ball * ball);
    void g_camera_func37(struct Camera * camera, struct Ball * ball);
    void g_camera_func39(struct Camera * camera, struct Ball * ball);
    void g_camera_func40(struct Camera * camera, struct Ball * ball);
    void g_camera_func41(struct Camera * camera, struct Ball * ball);
    void g_camera_func42(struct Camera * camera, struct Ball * ball);
    void g_camera_func44(struct Camera * camera, struct Ball * ball);
    void g_camera_func45(struct Camera * camera, struct Ball * ball);
    void g_camera_func46(struct Camera * camera, struct Ball * ball);
    void g_camera_func47(struct Camera * camera, struct Ball * ball);
    void g_camera_func48(struct Camera * camera, struct Ball * ball);
    void g_camera_func49(struct Camera * camera, struct Ball * ball);
    void g_camera_func50(struct Camera * camera, struct Ball * ball);
    void g_camera_func51(struct Camera * camera, struct Ball * ball);
    void g_camera_func56(struct Camera * camera, struct Ball * ball);
    void g_camera_func57(struct Camera * camera, struct Ball * ball);
    void g_camera_func63(struct Camera * camera, struct Ball * ball);
    void g_camera_func66(struct Camera * camera, struct Ball * ball);
    void g_camera_func68(struct Camera * camera, struct Ball * ball);
    void g_camera_func69(struct Camera * camera, struct Ball * ball);
    void g_camera_func70(struct Camera * camera, struct Ball * ball);
    void g_camera_func71(struct Camera * camera, struct Ball * ball);
    void g_camera_func72(struct Camera * camera, struct Ball * ball);
    void g_camera_func73(struct Camera * camera, struct Ball * ball);
    void g_camera_default_func(struct Camera * camera, struct Ball * ball);
    void g_main_camera_function_func75(struct Camera * camera, struct Ball * ball);
    void g_camera_func76_gameplay(struct Camera * camera, struct Ball * ball);
    void g_camera_func82(struct Camera * camera, struct Ball * ball);
    void g_camera_func83(struct Camera * camera, struct Ball * ball);
    void g_camera_func84(struct Camera * camera, struct Ball * ball);
    void g_camera_func85(struct Camera * camera, struct Ball * ball);
    void g_camera_func91(struct Camera * camera, struct Ball * ball);
    void g_camera_func92(struct Camera * camera, struct Ball * ball);
    bool g_is_sphere_visible(undefined8 radius, struct Vec * center);
    bool g_is_sphere_visible_scaled(double radius, double g_scale, struct Vec * center);
    void g_init_lights(void);
    void g_something_to_do_with_lights(void);
    undefined4 g_some_stage_init_func(void * param_1);
    void g_something_with_lights(int param_1);
    undefined4 g_get_maybe_something_with_lights(void);
    void g_push_light_group(void);
    void g_pop_light_group(void);
    void g_something_with_gx_modes_and_compare(void);
    void gx_finish_frame_and_reset_z_and_alpha_compare_modes(void);
    void g_something_with_graphics_processor2(void);
    void g_something_with_video_this_also_finishes_a_frame(void);
    void gp_wait_wrapper(void);
    u32 get_gx_fifo_use_size(void);
    void g_reset_cm_course(void);
    void event_info_init(void);
    void event_info_tick(void);
    void event_info_dest(void);
    void g_reset_mode_info_stuff(void);
    bool did_ball_enter_goal(struct Ball * ball, int * out_stage_goal_idx, int * out_itemgroup_id, byte * out_goal_flags);
    BOOL32 did_ball_enter_wormhole(struct Ball * ball, int * out_wormhole_idx);
    void g_set_goaled(void);
    void sprite_bonus_plus000_tick(u8 * status, struct Sprite * sprite);
    void sprite_bonus_banana_tick(u8 * status, struct Sprite * sprite);
    void sprite_ranking_tick(u8 * status, struct Sprite * sprite);
    void sprite_ranking_disp(void);
    void create_ranking_sprite(struct Ball * ball);
    void g_something_with_goals_and_ball(struct Ball * ball, int goal_idx, int ig_idx, undefined param_4);
    BOOL32 did_ball_fallout(struct Ball * ball);
    void g_something_playing_soundeffects(struct Ball * ball);
    void g_something_with_init_common(void);
    void g_load_minigame_gma_tpl(int param_1);
    void g_something_with_loading_wl_tpls(int world_number);
    void g_load_efcmdl_gmatpl(void);
    void load_efcmdl_files_from_disc(void);
    void g_something_freeing_heap_parent(void);
    void process_inputs(void);
    void threshold_analog_inputs(void);
    void g_calc_frames_since_last_input_change(void);
    void merge_inputs(void);
    TplBuffer * load_bmp(char * filepath);
    void g_something_with_bmp_bmp_com(int g_idx);
    void g_something_with_freeing_memory(int param_1);
    void g_zero_some_sprite_related_state(void);
    void draw_ui(void);
    void g_draw_sprite_requests_from_buffer(void);
    void g_init_some_sprite_related_obj(void);
    void perf_init_timer(int timer_id);
    uint perf_stop_timer(int timer_id);
    void g_init_perf_profiling(void);
    void g_perf_finish_frame(void);
    void g_perf_start_frame(void);
    void g_reset_draw_perf_counters(void);
    void g_printf_sound_error(bool is_warning, char * origin, undefined4 g_error_format, ...);
    void * try_alloc_memory_for_sound(u32 size);
    void g_OSFree_wrapper(void * param_1);
    void init_sound(void);
    void g_something_with_music_fading(void);
    void hwExit_wrapper(void);
    void ReadMusyXData(undefined param_1);
    void g_something_calls_sndPopGroup(void);
    void g_something_calls_sndPopGroup_2(void);
    void event_sound_init(void);
    void event_sound_tick(void);
    void event_sound_dest(void);
    double g_smth_called_by_event_tick_sound_1(char param_1, short param_2);
    void g_smth_called_by_event_tick_sound_2(void);
    void g_set_smth_with_sound(undefined param_1);
    void g_smth_calls_sndFXStartParaInfo(struct GSomeSoundStruct * param_1);
    int g_something_with_volume(uint * param_1, int param_2, int param_3);
    undefined4 g_something_calls_sndFXKeyOff(uint param_1, int param_2, int param_3);
    int SoundReq(uint g_some_id);
    int SoundReqDirect(uint sfx_id);
    int SoundReqID(uint g_sfx_id, int param_2);
    void call_SoundReqID_arg_0(u32 g_sfx_id);
    void call_SoundReqID_arg_1(uint g_sfx_id);
    void call_SoundReqID_arg_2(uint g_sfx_id);
    void call_SoundReqID_arg_1_copy(uint param_1);
    void SoundOff(uint param_1);
    void SoundVol(uint param_1, byte param_2);
    void SoundPan(uint param_1, char param_2, char param_3);
    void SoundPitch(uint param_1, uint param_2);
    void SoundDop(uint param_1, uint param_2);
    void SoundMod(uint param_1, uint param_2);
    void SoundRev(uint param_1, byte param_2);
    void SoundCho(uint param_1, byte param_2);
    bool SoundOffID(int param_1);
    void SoundVolID(int param_1, byte param_2);
    void SoundPanID(int param_1, char param_2, char param_3);
    void SoundRevID(int param_1, byte param_2);
    void SoundChoID(int param_1, byte param_2);
    uint SoundCheckStateID(int param_1);
    void SoundIcsReq(uint param_1, byte param_2, char param_3);
    void g_fade_track_volume(u32 volume, u8 param_2);
    int get_smgr_port(char param_1, int param_2, char param_3, short * param_4);
    void g_something_with_bgm(void);
    void g_crossfade_music(void);
    void SoftStreamSEReq(char param_1, int sfx_id, u32 sfx_volume, uint sfx_pan_L, uint sfx_pan_R);
    void empty_function(void);
    void call_SoftStreamSEReq_arg_0(int sfx_id, uint sfx_volume, uint sfx_pan_L, uint sfx_pan_R);
    void g_stop_music_sound_debug(int param_1);
    void g_some_dvd_read_async_sound_callback2(undefined4 param_1, struct DVDCommandBlock * param_2);
    s32 SoftStreamStart(u32 g_looping_state, BgmTrack  g_bgm_id, u32 param_3);
    void empty_function(void);
    void g_SoftStreamStart_with_some_defaults(BgmTrack  param_1);
    void g_SoftStreamStart_with_some_defaults_2(BgmTrack  param_1);
    void g_another_SoftStreamStart_wrapper_of_some_sort(BgmTrack  param_1, u32 param_2);
    undefined4 play_track_and_fade_out_other_tracks(undefined4 param_1, undefined4 param_2, byte volume);
    s32 g_smth_related_to_music(s32 param_1_00, s32 param_2_00, s32 param_3_00, u8 param_4, u8 param_5);
    void g_handle_world_bgm(u32 g_volume);
    void g_something_with_stopping_music_or_sfx(BgmTrack  param_1);
    int g_maybe_related_to_music_crossfading(int param_1);
    undefined4 g_check_current_track(BgmTrack  track_id);
    void g_maybe_smth_with_music(s32 param_1, u8 param_2);
    void g_change_music_volume(s32 param_1, s32 param_2, u8 volume);
    void SoundEffectInit(void);
    void g_something_with_sound9(void);
    void g_smth_calls_sndAuxCallbackUpdateSettingsReverbHI(undefined4 * param_1);
    void g_set_some_sound_data(void);
    void g_something_with_sound6(void);
    void g_something_with_sound11(void);
    void empty_function(void);
    uint g_something_with_sound4(undefined * param_1, undefined4 param_2, undefined param_3, undefined param_4, undefined param_5, undefined param_6);
    int g_smgr_allocate_stereo(undefined4 param_1, undefined4 param_2, undefined param_3, undefined param_4, char * param_5, char * param_6, char param_7);
    void g_some_dvd_read_async_sound_callback(s32 result, struct DVDFileInfo * file_info);
    void event_adx_init(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void event_adx_tick(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void event_adx_dest(void);
    void g_adx_error_call_back(undefined4 param_1, undefined4 param_2);
    void g_draw_ingame_debug_menu(uint * param_1, int param_2);
    void g_draw_debug_window_boundaries(uint * param_1);
    void draw_debugtext(void);
    void window_init(void);
    void g_something_with_replays3(void);
    void g_maybe_align_text_to_be_printed(uint g_x_pos, uint g_y_pos);
    void debug_window_printf(undefined param_1, char * format, ...);
    void g_smth_with_fifo_color(uint8_t color);
    void g_debug_text_put1(int param_1, char * param_2);
    void g_set_up_debugtext_buffer(void);
    undefined4 g_some_printf_function_1(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void g_print_formatted_string_on_screen(char * format, ...);
    void g_debug_text_put2(char * text);
    undefined4 g_some_printf_function_2(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, undefined4 param_9, char * param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void draw_debugtext_char_en(u32 x, u32 y, char ch, u8 color_rrggbbaa);
    void draw_debugtext_char_jp(u32 x, u32 y, byte * ch, uint color_unknown_format);
    void draw_debugtext_window_bg(double x1, double y1, double x2, double y2);
    bool g_is_sjis_and_more_than_24_mib(void);
    void g_debug_print_ape(int param_1, undefined4 param_2, int param_3);
    void debug_overlay_draw_fifo(void);
    void unset_all_dip_switches(void);
    void g_check_debug_mode_input(void);
    void pool_init(void);
    void pool_tick(void);
    void pool_update_idxs_of_all_pools(void);
    void pool_update_idxs(struct PoolInfo * tlm, EventID  event_filter);
    void g_do_nothing(void);
    int pool_alloc(struct PoolInfo * tlm, u8 status);
    void pool_clear(struct PoolInfo * info);
    void g_set_some_draw_values(float param_1, float param_2, float param_3);
    void g_something_with_some_sort_of_lzs(undefined4 param_1, undefined4 param_2, char * p_lz_path, char * lz_path);
    void empty_function(void);
    void g_ape_upright(struct Ape * ball);
    void g_move_ape_into_ball(struct Ape * ape);
    void g_default_ape_draw(struct Ape * ape, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void empty_function(void);
    void event_ball_init(void);
    void g_call_maybe_sets_number_of_starting_monkeys(struct Ball * ball);
    void create_stage_player(byte player_index, BallMode  physicsmode, byte ape_id, byte color_idx, uint LOD, void * draw_func, int g_variant);
    void g_create_stage_player_wrapper(byte player_index, BallMode  physicsMode, byte ape_id, byte param_4, ApeLOD  LOD, void * draw_func);
    void challenge_mode_init(void);
    void event_ball_tick(void);
    void event_ball_dest(void);
    void add_bananas(int bananas_to_add);
    void g_something_with_translating_items(double param_1, int param_2, struct Vec * param_position);
    void g_maybe_sets_number_of_starting_monkeys(struct Ball * ball);
    void g_reset_ball(struct Ball * in_ball);
    void ball_physics_g_something_w_postgoal_slowdown(struct Ball * param_1);
    void ball_physics_g_something_w_postgoal_blast_up(struct Ball * param_1);
    void g_ball_mode_play_replay(struct Ball * ball);
    void ball_physics_g_something_w_poastgoal_slowdown_blast_up(struct Ball * param_1);
    void ball_physics_g_something_w_postgoal_blast_up2(struct Ball * ball);
    void g_move_and_collide(struct Ball * ball, struct PhysicsBall * physicsBall);
    void g_apply_ball_velocity(struct Ball * ball);
    void collide_with_stage(struct Ball * ball, struct PhysicsBall * physicsball);
    void position_ball(struct Ball * ball, struct PhysicsBall * phys_ball);
    void set_ball_properties(struct Ball * ball, int constants_idx);
    void ball_collision_stars(struct Ball * ball);
    void init_physicsball_from_ball(struct Ball * ball, struct PhysicsBall * physicsball);
    void g_copy_physicsball_to_ball(struct Ball * ball, struct PhysicsBall * physicsball);
    void g_ball_ape_rotation(struct Ball * ball);
    void spawn_postgoal_ball_sparkle(void);
    void g_some_ballfunc(struct Ball * param_1);
    void ball_sounds_gameplay(struct Ball * ball);
    G_BallMode * ball_movement_sparks(struct Ball * ball);
    void set_visual_scale(struct Ball * ball);
    void g_draw_ball_and_ape(void);
    void g_something_with_view_stage_and_ball(void);
    undefined4 * g_some_ball_stage_coli_func(struct PhysicsBall * physicsball, struct StagedefFileHeader * stagedef);
    undefined4 meshcoli_grid_lookup(float x, float z, struct StagedefColiHeader * coli_header);
    void stcoli_sub03(struct PhysicsBall * physicsball, struct StagedefColiTri * tri);
    void stcoli_sub04(struct PhysicsBall * physball, struct StagedefColiTri * tri);
    void stcoli_sub05(struct PhysicsBall * param_1, struct Vec * param_2, struct Vec * param_3, float * param_4);
    void stcoli_sub06(struct PhysicsBall * physball, struct StagedefColiTri * tri);
    undefined8 stcoli_sub07(struct PhysicsBall * param_1, float * param_2, float * param_3);
    void g_some_jamabar_coli_func(struct PhysicsBall * physicsball, float * param_2);
    void g_cylinder_coli_something(struct PhysicsBall * physball, struct StagedefColiCylinder * cylinder);
    void stcoli_sub10(struct PhysicsBall * param_1, struct Vec * param_2);
    void g_sphere_coli_something(struct PhysicsBall * param_1, struct StagedefColiSphere * param_2);
    void g_cone_coli_something(struct PhysicsBall * param_1, struct StagedefColiCone * param_2);
    void g_something_with_physicsball_restitution(struct PhysicsBall * physicsball, struct Vec * param_2);
    BOOL32 line_intersects_rect(struct Vec * lineStart, struct Vec * lineEnd, struct Rect * rect);
    void stobj_jamabar_child_coli(struct PhysicsBall * physicsball, struct Stobj * stobj);
    void raycast_stage_down(struct Vec * origin, struct RaycastHit * out_hit, struct Vec * out_vel_at_point);
    BOOL32 raycast_tri(struct Vec * line_origin, struct Vec * line_dir, struct StagedefColiTri * tri);
    BOOL32 raycast_cone(struct Vec * line_origin, undefined4 line_dir, struct StagedefColiCone * cone, struct Vec * out_hit_pos, struct Vec * out_hit_normal);
    BOOL32 raycast_sphere(struct Vec * line_origin, struct Vec * line_dir, struct StagedefColiSphere * sphere, struct Vec * out_hit_pos, struct Vec * out_hit_normal);
    BOOL32 raycast_cylinder(undefined4 line_origin, undefined4 line_dir, struct StagedefColiCylinder * cylinder, undefined4 out_hit_pos, undefined4 out_hit_normal);
    uint g_goal_coli_something(struct PhysicsBall * param_1, struct StagedefGoal * param_2);
    void stcoli_sub22(struct PhysicsBall * param_1, struct Vec * param_2);
    void stcoli_sub24(struct PhysicsBall * param_1, struct Vec * param_2);
    void stcoli_sub25(struct PhysicsBall * param_1, int param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void g_draw_stage_collision(void);
    void stcoli_sub27(int param_1);
    void stcoli_sub28(struct Vec * param_1);
    void stcoli_sub29(float * param_1, float * param_2, float * param_3, float * param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void tf_physicsball_by_mtxa(struct PhysicsBall * physicsball1, struct PhysicsBall * physicsball2);
    void inv_tf_physicsball_by_mtxa(struct PhysicsBall * src_physicsball, struct PhysicsBall * dest_physicsball);
    void tf_physball_to_itemgroup_space(struct PhysicsBall * physicsball, int itemgroup_idx);
    uint g_something_w_ig_and_coli_headers(struct Itemgroup * ig_list, struct StagedefColiHeader * coli_header_list, undefined4 param_3, struct Vec * physicsball_x);
    undefined4 g_something_w_ig_and_coli_headers_2(struct Itemgroup * ig_list, struct StagedefColiHeader * coli_header_list, struct Vec * physicsball_pos);
    void event_world_init(void);
    double event_world_tick(double param_1);
    void event_world_dest(void);
    void event_stage_init(void);
    void event_stage_tick(void);
    void event_stage_dest(void);
    double g_advance_itemgroup_anim_frame(struct Itemgroup * itemgroup, struct StagedefColiHeader * colis_header);
    void g_advance_stage_animation(void);
    void g_transform_some_itemgroup_vec(void);
    GmaModelHeader * get_GmaBuffer_entry(struct GmaBuffer * buffer, char * name);
    void g_something_with_stgname3(void);
    void init_itemgroups(void);
    void empty_function(void);
    void g_load_stage_2(u32 stage_id);
    void queue_stage_load(uint stage_number);
    void g_load_stage_gma_tpl_lz(u32 stage_id);
    WorldTheme get_stage_world_theme(int stage_id);
    WorldTheme get_stage_world_theme(int stage_id);
    void g_smth_with_stage_anim_groups(int anim_group_id, uint param_2);
    void g_init_smth_with_seesaws(void);
    undefined4 get_seesaw_replay_state_size(struct SeesawInfo * seesaw_info);
    void g_smth_with_stage_fog(double param_1);
    bool is_stage_id_not_for_party_game(int stage_id);
    void seesaw_init(struct SeesawInfo * seesaw_info);
    void seesaw_dest(void);
    void seesaw_tick(struct SeesawInfo * seesaw_info);
    void seesaw_coli(struct SeesawInfo * seesaw_info, int param_2);
    void g_seesaw_func5(int param_1, undefined4 * param_2);
    void g_seesaw_replay_smth(double param_1, short * param_2, int param_3);
    void g_init_smth_with_stage_models(void);
    void g_draw_stage_models(void);
    void g_draw_goalpost_models(void);
    void g_draw_wormhole_models(void);
    void g_draw_generator_models(void);
    void g_draw_start_position_marker(void);
    void g_draw_stage(void);
    void g_draw_collision_triangles(void);
    bool is_stage_id_348_revolution(void);
    void g_special_handler_for_st348_revolution(void);
    void g_handle_hardcoded_special_case_stages(void);
    void g_special_load_for_st348(void);
    void g_smth_hardcoded_for_meltingpot(void);
    void g_smth_hardcoded_for_totalitarianism_and_meltingpot(void);
    void g_smth_hardcoded_for_st340(void);
    void g_smth_hardcoded_for_trainworm_and_nintendo(void);
    void g_init_shadows(void);
    void g_smth_with_viewstage_and_whs(void);
    int g_something_with_shadow_cast(void);
    void g_something_with_stgname(int locale_index);
    undefined4 g_something_with_stgname2(s32 stgname_dvd_entrynum);
    void g_queue_some_load(void);
    char * g_something_with_strings_and_dvd_file_reading(int param_1, char * param_2, int param_3);
    void load_stage_models(void);
    void init_seesaws(void);
    void load_stagedef(u32 stage_id);
    void g_free_stage(void);
    void g_something_with_replays5(void);
    void event_recplay_init(void);
    void event_recplay_tick(void);
    void event_recplay_dest(void);
    void g_something_with_recplay2(void);
    void g_something_with_recplay3(int param_1);
    undefined4 g_smth_with_replay_playback(int param_1);
    bool is_itemgroup_playable(u32 itemgroup_idx);
    u32 get_playable_itemgroup_count(void);
    u32 compute_playable_itemgroup_count(void);
    void g_smth_with_wormhole_replays(int ball_idx);
    void g_get_replay_frame_data(double replay_frames_remaining, int param_2, struct Vec * position_out);
    void g_smth_with_seesaws_and_replays(int param_1, uint param_2, uint param_3, void * param_4);
    void g_something_with_stage_world_themes(int param_1);
    void g_something_with_replays4(void);
    double g_something_with_replays6(int param_1);
    void g_something_with_replays(int param_1, void * param_2);
    void g_something_with_bundled_replays(void);
    undefined4 g_LoadRecFile(int param_1, int param_2);
    undefined4 g_something_with_replays2(int param_1);
    void empty_function(void);
    void empty_function(void);
    void g_something_with_score(void);
    void g_advance_replay2(double replay_frames_remaining, float * param_2);
    uint rle_encode(void * input, void * output, uint inputSize);
    uint get_compressed_replay_size(void);
    uint compress_replay(void * outCompressedReplay);
    int g_smth_with_cmp_recplay(void * param_1);
    Replay * init_replay(struct Replay * replay, int stage_time_limit);
    undefined4 init_replay_playable_ig_and_seesaw_state(struct Replay * replay, short * param_2, int param_3, u32 playableIgAndSeesawSize);
    Replay * create_replay(int stage_time_limit);
    void convert_replay_ptrs_to_offsets(struct Replay * replay, struct Replay * outReplay);
    void convert_replay_offsets_to_ptrs(struct Replay * replay);
    void increment_score(int g_some_flag, int score);
    int get_goal_score(uint * param_1, int * param_2);
    void event_effect_init(void);
    void event_effect_tick(void);
    void event_effect_dest(void);
    void g_draw_effects(void);
    int spawn_effect(struct Effect * effect);
    void overwrite_effect_funcs(EffectType  effect_type, void (** new_effect_funcs)(struct Effect *));
    void effect_default_init(void);
    void effect_default_tick(void);
    void effect_default_disp(void);
    void effect_default_dest(struct Effect * effect);
    void effect_paperfrag_init(struct Effect * effect);
    void effect_paperfrag_tick(short * param_1);
    void effect_paperfrag_disp(int param_1);
    void effect_paperfrag_dest(struct Effect * effect);
    void effect_get_banana_init(struct Effect * effect);
    void effect_get_banana_tick(int param_1);
    void effect_get_banana_disp(int param_1);
    void effect_get_banana_dest(struct Effect * effect);
    void effect_particle_init_coli(int param_1);
    void effect_particle_tick_coli(int param_1);
    void effect_particle_disp_coli(int param_1);
    void effect_particle_dest_coli(struct Effect * effect);
    void effect_holding_banana_init(struct Effect * effect);
    void effect_holding_banana_tick(struct Effect * effect);
    void effect_holding_banana_disp(int param_1);
    void effect_holding_banana_dest(struct Effect * effect);
    void effect_levitate_init(struct Effect * effect);
    void effect_levitate_tick(short * param_1);
    void effect_levitate_disp(int param_1);
    void effect_levitate_dest(struct Effect * effect);
    void effect_colistar_particle_init(struct Effect * effect);
    void effect_colistar_particle_tick(int param_1);
    void effect_colistar_particle_disp(int param_1);
    void effect_colistar_particle_dest(struct Effect * effect);
    void effect_bgwat_bubble_base_init(struct Effect * effect);
    void effect_bgwat_bubble_base_tick(int param_1);
    void effect_bgwat_bubble_base_disp(void);
    void effect_bgwat_bubble_base_dest(struct Effect * effect);
    void effect_bgwat_bubble_init(struct Effect * effect);
    void effect_bgwat_bubble_tick(int param_1);
    void effect_bgwat_bubble_disp(int param_1);
    void effect_bgwat_bubble_dest(struct Effect * effect);
    void effect_meteo_init(struct Effect * effect);
    void effect_meteo_tick(short * param_1);
    void effect_meteo_disp(int param_1);
    void effect_meteo_dest(struct Effect * effect);
    void effect_meteo_fix_init(struct Effect * effect);
    void effect_meteo_fix_tick(int param_1);
    void effect_meteo_fix_disp(int param_1);
    void effect_meteo_fix_dest(struct Effect * effect);
    void effect_coliflash_init(struct Effect * effect);
    void effect_coliflash_tick(int param_1);
    void effect_coliflash_disp(int param_1);
    void effect_coliflash_dest(struct Effect * effect);
    void effect_bns_stg_star_init(struct Effect * effect);
    void effect_bns_stg_star_tick(int param_1);
    void effect_bns_stg_star_disp(int param_1);
    void effect_bns_stg_star_dest(struct Effect * effect);
    void effect_bns_stg_star_tail_init(int param_1);
    void effect_bns_stg_star_tail_tick(struct Effect * effect);
    void effect_bns_stg_star_tail_disp(int param_1);
    void effect_bns_stg_star_tail_dest(struct Effect * effect);
    void effect_bgmst_gen_cloud_init(int param_1);
    void effect_bgmst_gen_cloud_tick(ushort * param_1);
    void effect_bgmst_gen_cloud_disp(int param_1);
    void effect_bgmst_gen_cloud_dest(struct Effect * effect);
    uint effect_bgstm_rainripple_init(int param_1);
    void effect_bgstm_rainripple_tick(int param_1);
    void effect_bgstm_rainripple_disp(struct Effect * effect);
    void effect_bgstm_rainripple_dest(struct Effect * effect);
    void effect_bgmst_water_init(void);
    void effect_bgmst_water_tick(void);
    void effect_bgmst_water_disp(int param_1);
    void effect_bgmst_water_dest(struct Effect * effect);
    void effect_commendfrag_init(int param_1);
    void effect_commendfrag_tick(short * param_1);
    void effect_commendfrag_disp(int param_1);
    void effect_commendfrag_dest(struct Effect * effect);
    void effect_banana_drop_init(int param_1);
    void effect_banana_drop_tick(int param_1);
    void effect_banana_drop_disp(int param_1);
    void effect_banana_drop_dest(struct Effect * effect);
    void effect_bgend_water_init(void);
    void effect_bgend_water_tick(void);
    void effect_bgend_water_disp(int param_1);
    void effect_bgend_water_dest(struct Effect * effect);
    void effect_bgbtmfire_ptcl_init(int param_1);
    void effect_bgbtmfire_ptcl_tick(int param_1);
    void effect_bgbtmfire_ptcl_disp(int param_1);
    void effect_bgbtmfire_ptcl_dest(struct Effect * effect);
    void effect_piyo_init(int param_1);
    void effect_piyo_tick(short * param_1, int param_2);
    void effect_piyo_disp(int param_1, int param_2);
    void effect_piyo_dest(struct Effect * effect);
    void event_background_init(void);
    void event_background_tick(void);
    void event_background_dest(void);
    void g_draw_bg(void);
    void g_something_with_world_themes(void);
    void g_something_with_init_or_maybe_leftover_from_smb1(int param_1);
    void g_something_with_stage_world_themes2(int theme_id);
    void g_something_with_view_stage_and_bg(void);
    void call_item_coin_coli_func_for_cur_world_theme(void);
    void bg_e3_and_bow2_and_gol2_init(void);
    void bg_e3_and_bow2_and_gol2_tick(void);
    void bg_e3_and_bow2_and_gol2_dest(void);
    void bg_e3_and_bow2_and_gol2_disp(void);
    void bg_e3_and_bow2_and_gol2_item_coin_coli(void);
    void g_render_foreground_objects(Mtx * g_stage_tilt_mtx, struct StagedefForegroundModel * fg_model, int foreground_model_count);
    void g_something_with_texture_scroll(float * param_1);
    void bg_nig_init(void);
    void bg_nig_tick(void);
    void bg_nig_dest(void);
    void bg_nig_disp(void);
    void bg_nig_item_coin_coli(void);
    void bg_ice2_init(void);
    void bg_ice2_tick(void);
    void bg_ice2_dest(void);
    void bg_ice2_disp(void);
    void bg_ice2_item_coin_coli(void);
    void bg_bil_init(void);
    void bg_bil_tick(void);
    void bg_bil_dest(void);
    void bg_bil_disp(void);
    void bg_bil_item_coin_coli(void);
    void bg_gol_init(void);
    void bg_gol_tick(void);
    void bg_gol_dest(void);
    void bg_gol_disp(void);
    void bg_gol_item_coin_coli(void);
    void bg_bow_init(void);
    void bg_bow_tick(void);
    void bg_bow_dest(void);
    void bg_bow_disp(void);
    void bg_bow_item_coin_coli(void);
    void bg_futsal_init(void);
    void bg_futsal_tick(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void bg_futsal_dest(void);
    void bg_futsal_disp(void);
    void bg_futsal_item_coin_coli(void);
    void bg_electric_init(void);
    void bg_electric_tick(void);
    void bg_electric_dest(void);
    void bg_electric_disp(void);
    void bg_electric_item_coin_coli(void);
    void bg_boat_init(void);
    void bg_boat_tick(void);
    void bg_boat_dest(void);
    void bg_boat_disp(void);
    void bg_boat_item_coin_coli(void);
    void bg_author_and_park_author_init(void);
    void bg_author_and_park_author_tick(void);
    void bg_author_and_park_author_dest(void);
    void bg_author_and_park_author_disp(void);
    void bg_author_and_park_author_item_coin_coli(void);
    void g_init_stagedef_bg_fg_models(struct StagedefBackgroundModel * model, int count, struct GSomeBgStruct * some_bg_struct, undefined * param_4);
    void bg_jun_init(void);
    void bg_jun_tick(void);
    void bg_jun_dest(void);
    void bg_jun_disp(void);
    void bg_jun_item_coin_coli(void);
    void bg_snd_init(void);
    void bg_snd_tick(void);
    void bg_snd_dest(void);
    void bg_snd_disp(void);
    void bg_snd_item_coin_coli(void);
    void bg_wat_init(void);
    void bg_wat_tick(void);
    void bg_wat_dest(void);
    void bg_wat_disp(void);
    void bg_wat_item_coin_coli(void);
    void bg_spa_init(void);
    void bg_spa_tick(void);
    void bg_spa_dest(void);
    void bg_spa_disp(void);
    void bg_spa_item_coin_coli(void);
    void bg_sun_init(void);
    void bg_sun_tick(void);
    void bg_sun_dest(void);
    void bg_sun_disp(void);
    void bg_sun_item_coin_coli(void);
    void bg_bns_init(void);
    void bg_bns_tick(void);
    void bg_bns_dest(void);
    void bg_bns_disp(void);
    void bg_bns_item_coin_coli(int param_1);
    void bg_stm_init(void);
    void bg_stm_tick(void);
    void bg_stm_dest(void);
    void bg_stm_disp(void);
    void bg_stm_item_coin_coli(void);
    void bg_mst_init(void);
    void bg_mst_tick(void);
    void bg_mst_dest(void);
    void bg_mst_disp(void);
    void bg_mst_item_coin_coli(void);
    void bg_pil_init(void);
    void bg_pil_tick(void);
    void bg_pil_dest(void);
    void bg_pil_disp(void);
    void bg_pil_item_coin_coli(void);
    void bg_end_init(void);
    void bg_end_tick(void);
    void bg_end_dest(void);
    void bg_end_disp(void);
    void bg_end_item_coin_coli(void);
    void bg_lava_init(void);
    void bg_lava_tick(void);
    void bg_lava_dest(void);
    void bg_lava_disp(void);
    void bg_lava_item_coin_coli(void);
    void g_something_with_stage_heap_and_lava_theme(int * param_1);
    void bg_wat2_and_wat2_author_init(void);
    void bg_bg_init_wat2_and_wat2_author_tick(void);
    void bg_wat2_and_wat2_author_dest(void);
    void bg_wat2_and_wat2_author_disp(void);
    void bg_wat2_and_wat2_author_item_coin_coli(void);
    void empty_function(void);
    void empty_function(void);
    void bg_pil2_and_pil2_author_init(void);
    void bg_pil2_and_pil2_author_tick(void);
    void bg_pil2_and_pil2_author_dest(void);
    void bg_pil2_and_pil2_author_disp(void);
    void bg_pil2_and_pil2_author_item_coin_coli(void);
    void bg_spa2_init(void);
    void bg_spa2_tick(void);
    void bg_spa2_dest(void);
    void bg_spa2_disp(void);
    void bg_spa2_item_coin_coli(void);
    void empty_function(void);
    void empty_function(void);
    void bg_bil2_init(void);
    void bg_bil2_tick(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void bg_bil2_dest(void);
    void bg_bil2_disp(void);
    void bg_bil2_item_coin_coli(void);
    void bg_pot_init(void);
    void bg_pot_tick(void);
    void bg_pot_dest(void);
    void bg_pot_disp(void);
    void bg_pot_item_coin_coli(void);
    void bg_pot_fgt_init(void);
    void bg_pot_fgt_tick(void);
    void bg_pot_fgt_dest(void);
    void bg_pot_fgt_disp(void);
    void bg_pot_fgt_item_coin_coli(void);
    void empty_function(void);
    void g_something_with_stage_heap_and_pot_theme(int param_1);
    void bg_park_init(void);
    void bg_park_tick(void);
    void bg_park_dest(void);
    void bg_park_disp(void);
    void bg_park_item_coin_coli(void);
    void bg_whale_init(void);
    void bg_whale_tick(void);
    void bg_whale_dest(void);
    void bg_whale_disp(void);
    void bg_whale_item_coin_coli(void);
    void g_something_with_stage_heap_and_whale_theme(int param_1);
    short * g_something_with_stage_heap_and_whale_theme2(int param_1);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void bg_bubble_and_bubble_author_init(void);
    void bg_bubble_and_bubble_author_tick(void);
    void bg_bubble_and_bubble_author_dest(void);
    void bg_bubble_and_bubble_author_disp(void);
    void bg_bubble_and_bubble_author_item_coin_coli(void);
    void empty_function(void);
    void empty_function(void);
    void bg_gear_and_gear_author_init(void);
    void bg_gear_and_gear_author_tick(void);
    void bg_gear_and_gear_author_dest(void);
    void bg_gear_and_gear_author_disp(void);
    void bg_gear_and_gear_author_item_coin_coli(void);
    void g_something_with_stage_heap_and_gear_theme(int param_1);
    void bg_jun2_init(void);
    void bg_jun2_tick(void);
    void bg_jun2_dest(void);
    void bg_jun2_disp(void);
    void bg_jun2_item_coin_coli(void);
    void g_load_ape_bg(void);
    void bg_bns2_init(void);
    void bg_bns2_tick(void);
    void bg_bns2_dest(void);
    void bg_bns2_disp(void);
    void bg_bns2_item_coin_coli(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void bg_bow2_author_init(void);
    void bg_bow2_author_tick(void);
    void bg_bow2_author_dest(void);
    void bg_bow2_author_disp(void);
    void bg_bow2_author_item_coin_coli(void);
    void clear_unlocked_cm_stages(void);
    void init_course(void);
    void event_course_init(void);
    void event_course_tick(void);
    void event_course_dest(void);
    bool is_stage_complete(struct CmEntry * entry);
    bool entered_goal_has_type(struct CmEntry * entry);
    bool did_beat_stage_faster_than(struct CmEntry * entry);
    void calc_stage_jump_distance(struct CmEntry * entry);
    void clear_next_cm_stage_id2(struct CmEntry * entry);
    void clear_next_cm_stage_id(struct CmEntry * entry);
    s32 g_get_current_cm_stage_time_limit(void);
    u32 g_update_cm_course(Difficulty  difficulty, s32 course_stage_num, ModeFlag  mode_flags);
    int calc_course_idx(Difficulty  difficulty, ModeFlag  mode_flags);
    bool g_are_on_final_course_level(int difficulty_id, int course_stage, uint difficulty_flags);
    undefined4 is_bonus_stage_being_played(int param_1);
    void g_something_with_cm_entries_practice_mode(void);
    undefined4 g_smth_with_cm_entries_in_main_menu(int param_1, int param_2, uint param_3);
    void empty_function(void);
    void update_cm_unlocked_levels(Difficulty  difficulty, int param_2, ModeFlag  mode_flags);
    void clear_cm_player_progress(void);
    void update_course_progress(void);
    void g_something_with_cm_player_progress(void);
    void sprite_debug_course_display_disp(undefined8 param_1, undefined8 param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, int param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void g_save_cm_unlock_entries(void);
    void g_load_cm_unlock_entries(void);
    int get_world_beaten_stage_count(int world);
    int g_lookup_storymode_stage_id(int world_idx, int world_stage_idx);
    int g_get_storymode_stage_id(int world, int stage);
    void clear_unlocked_storymode_stages(void);
    void g_save_storymode_unlock_entries(void);
    void g_load_storymode_unlock_entries(void);
    int is_timer_30s_or_60s(int param_1, int param_2);
    void event_item_init(void);
    void event_item_tick(void);
    void event_item_dest(void);
    void draw_items(void);
    int spawn_item(struct Item * item);
    void g_something_with_view_stage_and_item(void);
    void init_coli_header_items(struct StagedefColiHeader * coli_header, int coli_header_count);
    void item_default_init(void);
    void item_default_tick(void);
    void item_default_disp(void);
    void item_default_coli(void);
    void item_default_dest(void);
    void item_default_replay_init(void);
    void item_debug_default(void);
    void item_coin_init(struct Item * item);
    void item_coin_tick(struct Item * item);
    void item_coin_disp(struct Item * item);
    void item_coin_coli(struct Item * item, struct PhysicsBall * phys_ball);
    void item_coin_dest(void);
    void item_coin_replay_init(struct Item * item);
    void item_debug_coin(struct Item * item);
    GmaModelHeader * g_something_with_coins(int * * param_1);
    void event_stobj_collision_init(void);
    void event_stobj_collision_tick(void);
    void event_stobj_collision_dest(void);
    void event_stobj_collision_child_tick(void);
    uint g_some_item_collision_check(double ball_scale, double item_scale, struct Vec * ball_prev_pos, struct Vec * ball_pos, struct Vec * item_pos_copy, struct Vec * item_pos);
    void g_something_with_item_coli(double param_1, double param_2, struct Vec * param_3, struct Vec * param_4, struct Vec * param_5);
    void event_stobj_init(void);
    void event_stobj_tick(void);
    void event_stobj_dest(void);
    void g_draw_stobjs(void);
    int spawn_stobj(struct Stobj * stobj);
    void g_something_with_bumper_LODs(void);
    void load_bumper_stobjs(struct StagedefColiHeader * coli_header_list, int coli_header_count);
    void load_jamabar_stobjs(struct StagedefColiHeader * coli_header_list, int coli_header_count);
    void load_button_stobjs(struct StagedefColiHeader * coli_header_list, int coli_header_count);
    void stobj_bumper_init(struct Stobj * stobj);
    void stobj_bumper_tick(struct Stobj * stobj);
    void stobj_bumper_disp(struct Stobj * stobj);
    void stobj_bumper_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_bumper_dest(struct Stobj * stobj);
    void stobj_bumper_cb_f(struct Stobj * stobj);
    void stobj_bumper_bgspecial_init(struct Stobj * stobj);
    void stobj_bumper_bgspecial_tick(struct Stobj * stobj);
    void stobj_bumper_bgspecial_disp(struct Stobj * stobj);
    void stobj_bumper_bgspecial_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_bumper_bgspecial_dest(struct Stobj * stobj);
    void stobj_bumper_bgspecial_cb_f(struct Stobj * stobj);
    void stobj_jamabar_init(struct Stobj * stobj);
    void stobj_jamabar_tick(struct Stobj * stobj);
    void stobj_jamabar_disp(struct Stobj * stobj);
    void stobj_jamabar_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_jamabar_dest(struct Stobj * stobj);
    void stobj_jamabar_cb_f(struct Stobj * stobj);
    void stobj_default_init(void);
    void stobj_default_tick(void);
    void stobj_default_disp(void);
    void stobj_default_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_default_dest(void);
    void stobj_default_cb_f(void);
    void stobj_button_init(struct Stobj * stobj);
    void stobj_button_tick(struct Stobj * stobj);
    void stobj_button_disp(struct Stobj * stobj);
    void stobj_button_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_button_dest(struct Stobj * stobj);
    void stobj_button_cb_f(struct Stobj * stobj);
    void load_goaltape_and_goalbag_stobjs(struct StagedefColiHeader * coli_header_list, int coli_header_count);
    void stobj_goaltape_init(struct Stobj * stobj);
    double stobj_goaltape_tick(struct Stobj * stobj);
    void stobj_goaltape_disp(struct Stobj * stobj);
    void stobj_goaltape_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_goaltape_dest(struct Stobj * stobj);
    void stobj_goaltape_cb_f(void);
    double stobj_goalbag_init(struct Stobj * stobj);
    void stobj_goalbag_tick(struct Stobj * stobj);
    void stobj_goalbag_disp(struct Stobj * stobj);
    void stobj_goalbag_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_goalbag_dest(struct Stobj * stobj);
    void stobj_goalbag_cb_f(struct Stobj * stobj);
    double stobj_goalbag_exmaster_init(struct Stobj * stobj);
    void stobj_goalbag_exmaster_tick(struct Stobj * stobj);
    void stobj_goalbag_exmaster_disp(struct Stobj * stobj);
    void stobj_goalbag_exmaster_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_goalbag_exmaster_dest(struct Stobj * stobj);
    void stobj_goalbag_exmaster_cb_f(struct Stobj * stobj);
    void g_something_with_goals_and_physicsball(int goal_idx, struct PhysicsBall * physicsball);
    float g_get_sphere_camera_zdist_clamped(float radius, struct Vec * origin);
    void load_returngate_stobjs(struct StagedefColiHeader * coli_header_list, int coli_header_count);
    void stobj_returngate_init(struct Stobj * stobj);
    void stobj_returngate_tick(struct Stobj * stobj);
    void stobj_returngate_disp(struct Stobj * stobj);
    void stobj_returngate_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_returngate_dest(struct Stobj * stobj);
    void stobj_returngate_cb_f(struct Stobj * stobj);
    void event_sprite_init(void);
    void event_sprite_tick(void);
    void event_sprite_dest(void);
    void draw_sprite(struct Sprite * sprite);
    void call_something_with_bmp_bmp_com(int param_1);
    void g_something_with_iteratively_freeing_memory(void);
    Sprite * create_sprite(void);
    Sprite * create_linked_sprite(struct Sprite * sprite);
    void g_dest_sprite_with_font(int font);
    void dest_all_sprites(void);
    Sprite * g_find_sprite_with_probably_not_font(int probably_not_font);
    void g_reset_font_drawing_settings(void);
    void g_set_font_type(Font32  param_1);
    void g_mask_smth_with_font_drawing(uint param_1);
    void g_set_smth_with_font_drawing6(undefined4 param_1);
    void g_set_smth_with_font_drawing_depth(float param_1);
    void g_set_smth_with_font_drawing4(float param_1, float param_2);
    void g_set_smth_with_font_drawing9(double param_1);
    void g_set_smth_with_font_drawing8(uint param_1);
    void g_set_smth_with_font_drawing7(undefined param_1);
    void g_set_something_else_with_font_drawing(double param_1, double param_2);
    void g_set_smth_with_font_drawing1(float param_1, float param_2);
    void g_draw_str_with_font(byte * param_1);
    void g_printf_draw_with_font(double param_1, double param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, char * g_format, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void draw_text_sprite(struct Sprite * sprite);
    void draw_bmp_sprite(struct Sprite * sprite);
    void draw_texture_sprite(struct Sprite * sprite);
    void g_some_printf_function_3(double param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void g_get_string_sprite_width(int param_1);
    void fade_screen_to_color(uint flags, u32 color, uint frames);
    undefined4 draw_sprite_draw_request(struct SpriteDrawRequest * request);
    void g_smth_calls_GXLoadPosMtxImm(uint param_1);
    void g_smth_calls_GXLoadPosMtxImm2(void);
    void g_something_with_fonts2(void);
    void g_something_with_load_fonts(int param_1);
    void g_smth_with_sprite_draw_request(struct SpriteDrawRequest * request);
    int g_get_tex_id(undefined4 param_1, ushort param_2, ushort * param_3, int param_4);
    void g_smth_with_font_drawing(byte * param_1);
    double g_something_with_fonts4(int param_1, int param_2, char param_3);
    void g_some_printf_function_4(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, int param_9, char * param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void create_pausemenu_sprite(void);
    void g_smth_creates_gameover_sprite(undefined4 param_1, struct Sprite * sprite);
    void sprite_pausemenu_disp(struct Sprite * sprite);
    void create_titlescreen_press_start_sprites(int param_1);
    void sprite_titlescreen_press_start_tick(u8 * status, struct Sprite * sprite);
    void g_create_smd_adv_logo_sprites(void);
    void sprite_logo_tick(u8 * status, struct Sprite * sprite);
    void create_copyright_text_sprites(int param_1);
    void sprite_copyright_text_tick(u8 * status, struct Sprite * sprite);
    void create_title_sprite(s32 param_1);
    void sprite_title_tick(u8 * status, struct Sprite * sprite);
    void sprite_title_disp(struct Sprite * sprite);
    void g_create_some_controls_description_sprite(void);
    void create_stage_loadin_text_sprites(void);
    void sprite_loadin_stage_name_tick(u8 * status, struct Sprite * sprite);
    void sprite_loadin_stage_name_disp(int param_1);
    void sprite_player_num_tick(u8 * status, struct Sprite * sprite);
    void create_player_num_and_ready_sprites(s32 param_1);
    void sprite_ready_tick(u8 * status, struct Sprite * sprite);
    void sprite_bonus_stage_tick(u8 * status, struct Sprite * sprite);
    void sprite_final_stage_tick(u8 * status, struct Sprite * sprite);
    void sprite_final_stage_disp(struct Sprite * sprite);
    void create_go_sprite(s32 param_1);
    void sprite_go_tick(u8 * status, struct Sprite * sprite);
    void sprite_go_disp(struct Sprite * sprite);
    void create_goal_and_jump_to_stage_sprites(s32 g_num_frames);
    void sprite_goal_disp(struct Sprite * sprite);
    void sprite_jump_to_stage_tick(u8 * status, struct Sprite * sprite);
    void sprite_jump_to_stage_duplicate_tick(u8 * status, struct Sprite * sprite);
    void create_fallout_or_bonus_finish_sprite(s32 param_1);
    void sprite_fallout_tick(u8 * status, struct Sprite * sprite);
    void create_time_over_sprite(s32 param_1);
    void sprite_time_over_tick(u8 * status, struct Sprite * sprite);
    void create_bonus_finish_or_perfect_sprite(void);
    void create_bonus_finish_sprite(void);
    void sprite_bonus_finish_or_perfect_tick(u8 * status, struct Sprite * sprite);
    void create_continue_sprites(void);
    void sprite_continue_tick(u8 * status, struct Sprite * sprite);
    void sprite_continues_remaining_tick(u8 * status, struct Sprite * sprite);
    void sprite_continue_yes_or_no_tick(u8 * status, struct Sprite * sprite);
    void g_create_some_game_over_and_player_num_sprites(s32 param_1);
    void create_game_over_letter_sprite(s32 param_1, uint x, uint y, s32 param_4, char * letter);
    void sprite_game_over_tick(u8 * status, struct Sprite * sprite);
    void create_1up_sprite(s32 param_1);
    void sprite_1up_tick(u8 * status, struct Sprite * sprite);
    void empty_function(void);
    void create_replay_sprite(short param_1);
    void sprite_replay_tick(u8 * status, struct Sprite * sprite);
    void create_go_to_extra_or_master_stages_sprites(void);
    void g_sprite_extra_all_mask_tick(u8 * status, struct Sprite * sprite);
    void g_sprite_go_to_the_tick(u8 * status, struct Sprite * sprite);
    void g_sprite_go_to_the_disp(struct Sprite * sprite);
    void create_postgoal_score_sprites(undefined4 param_1, undefined4 param_2, uint param_3);
    void sprite_clear_score_disp(struct Sprite * sprite);
    void sprite_warp_bonus_disp(struct Sprite * sprite);
    void sprite_time_bonus_disp(struct Sprite * sprite);
    void sprite_stage_score_disp(struct Sprite * sprite);
    void sprite_goal_tick(u8 * status, struct Sprite * sprite);
    void g_make_some_continue_screen_sprites(void);
    void create_save_sprite(void);
    void sprite_save_tick(u8 * status, struct Sprite * sprite);
    void sprite_save_disp(struct Sprite * sprite);
    void sprite_monkey_counter_icon_disp(struct Sprite * sprite);
    void g_create_how_to_sprite(void);
    void g_sprite_how_to_tick(u8 * status, struct Sprite * sprite);
    void g_sprite_how_to_disp(struct Sprite * sprite);
    void create_hud_sprites(void);
    void create_score_sprites(float pos_x, float pos_y);
    void create_timer_sprites(float param_1, float param_2);
    void create_speed_sprites(float param_1, float param_2);
    void create_hud_stage_name_sprites(float param_1, float param_2);
    void create_banana_counter_sprites(double param_1, double param_2);
    void create_monkey_counter_sprites(double x, double y);
    void create_final_stage_sprite(void);
    void create_debug_course_display_sprites(void);
    void sprite_score_tick(u8 * status, struct Sprite * sprite);
    void sprite_score_disp(struct Sprite * sprite);
    void sprite_timer_ball_tick(u8 * status, struct Sprite * sprite);
    void sprite_current_stage_display_tick(u8 * status, struct Sprite * sprite);
    void sprite_current_stage_display_disp(struct Sprite * sprite);
    void sprite_hud_stage_name_tick(u8 * status, struct Sprite * sprite);
    void sprite_banana_icon_tick(u8 * param_1, struct Sprite * param_2);
    void sprite_banana_icon_shadow_tick(u8 * status, struct Sprite * sprite);
    void sprite_banana_count_tick(u8 * status, struct Sprite * sprite);
    void g_banana_disp_efc_req(char bananas_collected);
    void sprite_monkey_counter_tick(u8 * status, struct Sprite * sprite);
    void sprite_hud_player_num_disp(struct Sprite * sprite);
    void event_minimap_init(void);
    void event_minimap_tick(void);
    void event_minimap_dest(void);
    void set_minimap_mode(MinimapMode  minimap_mode);
    void toggle_minimap_zoom(void);
    void g_draw_minimap(void);
    void g_smth_with_rendefc_reflective_height(double height);
    void g_some_perf_timer_function(void);
    int g_maybe_req_model_to_be_drawn(uint param_1);
    void g_replay_screen_stage_name(void);
    void empty_function(void);
    void set_global_ape_LOD_1(void);
    void empty_function(void);
    undefined4 return_0(void);
    Ape * load_ape_gameplay_id_only(int monkey_id);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void set_global_LOD(int lod);
    void empty_function(void);
    void empty_function(void);
    void g_ape_anim_head(struct Ape * ape, struct Vec * ape_facedir_point, int flags);
    void empty_function(void);
    uint decompress_lz(byte * in_compressed, byte * out_decompressed);
    void g_some_arq_callback(u32 pointerToARQRequest);
    void load_disc_queue(void);
    BOOL32 g_something_with_dvd(s32 entry_num, undefined4 * param_2);
    BOOL32 g_open_file(char * file_path, struct GSomeFileStruct * fileStruct);
    BOOL32 dvd_close(struct GSomeFileStruct * file);
    void g_some_ARQPostRequest_callback(void);
    u32 g_something_with_reading_dvd_file(int * dvdEntrynum, void * buffer, u32 length, int offset);
    int fileSize(struct GSomeFileStruct * fileStruct);
    void g_some_dvd_callback(s32 result, struct DVDFileInfo * fileInfo);
    void g_something_with_dvd2(uint param_1, int param_2);
    int add_one_wrap_if_over127(int num);
    bool disc_queue_load(int entrynum);
    BOOL32 queue_disc_read(char * file_name);
    void get_load_group_status(void);
    void disc_queue_status(int identifier);
    int disc_queue_distance(int identifier);
    int g_get_debug_level_select_loading_left_asterisks(void);
    undefined4 g_swapDiscQueueGroup(undefined4 newValue);
    void g_fill_some_memory_with_0x0_and_0xff(void * ptr);
    undefined4 g_check_some_condition(ushort param_1);
    void debug_draw_shadow_textures(void);
    void md_mini_func(void);
    void g_set_minigame_specific_funcs(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
    void g_smd_mini_generic_init(void);
    void g_smd_mini_generic_tick(void);
    void smd_mini_select_init(void);
    void smd_mini_select_tick(void);
    void smd_mini_ending_init(void);
    void smd_mini_ending_tick(void);
    void smd_mini_s_roll_init(void);
    void smd_mini_s_roll_tick(void);
    void event_mouse_init(void);
    void event_mouse_tick(void);
    void event_mouse_dest(void);
    void event_rend_efc_init(void);
    void event_rend_efc_tick(void);
    void event_rend_efc_dest(void);
    void g_something_with_rend_efc(uint param_1);
    void g_init_rendefc_for_stage(void);
    void g_smth_with_loading_reflective_stgobjs(undefined * param_1);
    void g_smth_calling_reflective_obj_draw_hdlr(int g_some_flag, int * param_2);
    void g_reflective_object_draw_handler(int g_some_flag, int * g_some_ptr);
    void g_reflective_object_draw_handler_2(undefined4 param_1, int param_2);
    void g_some_rendefc_func_1(undefined * param_1);
    void g_something_with_stage_heap_and_target_theme(int param_1);
    void g_some_rendefc_func_2(int param_1, int param_2);
    void g_smth_with_reflective_models(undefined4 param_1, int param_2);
    void g_smth_with_wormhole_surfaces(undefined * param_1);
    void g_something_to_do_with_fog(void);
    void g_set_something3(char param_1);
    void smd_mini_commend_init(void);
    void smd_mini_commend_tick(void);
    void g_something_with_rotation_not_aiai(struct Ape * ape);
    void g_something_with_drawing_apes(void);
    void event_commend_init(void);
    void event_commend_tick(double param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8, undefined4 param_9);
    void event_commend_dest(void);
    void load_commend_stage(void);
    undefined4 return_1(void);
    void empty_function(void);
    void g_something_with_card13(void);
    void g_something_with_card3(void);
    void print_card_submode_error(byte * param_1);
    void empty_function(void);
    void empty_function(void);
    void mount_memory_card(byte * param_1);
    void g_something_free_card_blocks(struct MemCardFile * param_1);
    void g_open_card_file(struct MemCardFile * file);
    void something_that_calls_CARDCreateAsync(byte * param_1);
    void g_calls_CARDWriteAsync(byte * param_1);
    void g_related_to_memcard_rw(byte * param_1);
    void g_something_with_card12(byte * param_1);
    void g_something_with_card(void);
    void g_something_with_card10(byte * param_1);
    void g_some_printf_function_5(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void g_something_with_card11(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void g_something_with_card9(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void g_something_with_card5(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void g_something_with_card4(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void g_something_with_card8(void);
    void g_something_with_card6(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void g_something_with_card2(int card_chan, int param_2);
    uint g_something_with_fonts(void);
    void g_some_printf_function_6(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, int param_9, int param_10, int param_11, char * param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void smd_mini_ranking_init(void);
    void smd_mini_ranking_tick(void);
    void g_load_preview_texture(struct SpriteTex * sprite_tex, char * file_path, undefined4 param_3, u16 width, u16 height, GXTexFmt  format);
    void g_some_dvd_callback2(s32 result, struct DVDFileInfo * file_info);
    void g_load_sprite_tex_from_dvd(struct SpriteTex * sprite_tex);
    void g_maybe_save_previews(void);
    void g_something_with_smb1_ranking_main_game_defaults_wrapper(void);
    void event_name_entry_init(void);
    void event_name_entry_tick(void);
    void event_name_entry_dest(void);
    void g_something_with_name_entry(undefined8 param_1, undefined8 param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8);
    void g_something_with_smb1_ranking_main_game_defaults(void);
    void empty_function(void);
    void effect_nameent_code_init(int param_1);
    void effect_nameent_code_tick(short * param_1);
    void effect_nameent_code_disp(int param_1);
    void effect_nameent_code_dest(struct Effect * effect);
    void effect_get_nameent_code_init(int param_1);
    void effect_get_nameent_code_tick(short * param_1);
    void effect_get_nameent_code_disp(int param_1);
    void effect_get_nameent_code_dest(struct Effect * effect);
    void stobj_nameent_btn_init(struct Stobj * stobj);
    void stobj_nameent_btn_tick(struct Stobj * stobj);
    void stobj_nameent_btn_disp(struct Stobj * stobj);
    void stobj_nameent_btn_coli(struct Stobj * stobj, struct PhysicsBall * physicsball);
    void stobj_nameent_btn_dest(struct Stobj * stobj);
    void stobj_nameent_btn_cb_f(struct Stobj * stobj);
    void event_vibration_init(void);
    void event_vibration_tick(void);
    void event_vibration_dest(void);
    void g_rumble_controller(int controller_idx, int param_2, int param_3);
    void g_alloc_memory_for_ending(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void g_set_effect_ending_ballfrag(void);
    void g_clear_effect_ending_ballfrag(void);
    void effect_ending_ballfrag_init(int param_1);
    void effect_ending_ballfrag_tick(int param_1);
    void effect_ending_ballfrag_disp(int param_1);
    void effect_ending_ballfrag_dest(struct Effect * effect);
    void g_init_something_with_heaps(void);
    void g_free_some_memory2(void);
    void g_something_w_drawing_bgs_and_stobjs(struct GmaModelHeader * gma_model);
    void g_another_draw_func(struct GmaModelHeader * gma_model);
    void g_draw_non_translucid_gma_model(struct GmaModelHeader * gma_model);
    void g_unk_draw_func3(struct GmaModelHeader * gma_model);
    void g_smth_important_related_to_drawing(struct GmaModelHeader * gma_model);
    void g_smth_with_effect_drawing(struct GmaModelHeader * gma_model);
    void g_related_to_drawing_transparent_things(int param_1);
    undefined4 g_something_with_texture_scroll_2(int param_1);
    void g_stores_doubles(double param_1, double param_2, double param_3, double param_4);
    void g_stores_doubles2(double param_1, double param_2, double param_3, double param_4);
    void g_yet_another_unk_draw_func(undefined4 param_1);
    DVDFileInfo * g_get_some_dvd_file_info2(void);
    void empty_function(void);
    void * alloc_from_current_heap(u32 size);
    void free_from_current_heap(void * ptr);
    undefined8 teleport_through_wormhole(int ball_idx, int wormhole_idx);
    f32 * g_compute_wormhole_tf(int wormhole_idx, f32 * param_2);
    void apply_wormhole_tf_to_mtx(Mtx * src_tf, Mtx * wormhole_tf);
    void apply_wormhole_tf_to_vec(struct Vec * vec, Mtx * wormhole_tf);
    void apply_wormhole_tf_to_quat(struct Quat * quat, Mtx * wormhole_tf);
    void empty_function(void);
    void g_load_skl(struct SKLRoot * sklRoot, struct SKLInfo * boneData, struct SKLFile * sklFile);
    void g_handL_SKL_func(void * param_1, int param_2);
    void gan_setanim_wrapper(void * param_1, char * param_2);
    void gan_setAnim(void * mal, undefined * param_2, short animation_number);
    void gan_setanim_edance(undefined4 param_1, undefined param_2, uint param_3, uint param_4, uint param_5);
    void g_init_floatthing(struct gFloats * param_1);
    void gan_incframe(double g_speed, struct Ape * ape);
    void g_smth_with_quat_slerp(ushort * param_1);
    void g_something_arc(struct ArcFileInfo * fileInfo);
    uint g_table_index(struct SKLRoot * param1, char * str);
    void load_ape_body(int ape_index, int game_index);
    void gan_setanim_estagebegin(struct Ape * ape, int chara_index, int scene_index);
    void g_something_freeing_chara_heap_3(void * * param_1);
    void g_something_with_new_ape(struct Ape * ape, int chara_index, int scene_index);
    ulonglong event_ape_init(void);
    void event_ape_tick(void);
    void event_ape_dest(void);
    Ape * load_ape(int chara_index, ApeLOD  LOD, int game_index, int s_mal);
    Ape * load_ape_s(int chara_id, int LOD, int game_index);
    Ape * load_ape_gameplay(int monkey_id, undefined4 LOD, int g_variant);
    Ape * load_ape_gameplay_LOD(int monkey_id, ApeLOD  LOD);
    void draw_ape_subroutine(int param_1);
    void g_draw_ape1(double param_1, struct Ape * ape2);
    void draw_ape(struct Ape * ape);
    void g_something_with_freeing_chara_heap_ape(struct Ape * ape);
    void ape_default_anim(struct Ape * ape);
    void g_ape_anim_body(struct Ape * ape, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void g_load_mal_files_from_disc(int chara_index, int LOD, int s_mal, int game_index);
    void g_something_freeing_chara_heap_4(int param_1, int param_2, int param_3);
    void g_something_freeing_chara_heap_2(int param_1, int param_2);
    void assign_model_pointers(struct Ape * ape, ApeLOD  lod);
    int gan_getsomeframe(struct Ape * ape, int index);
    double body_frame_add72(struct Ape * ape);
    undefined * get_ape_game_string(struct Ape * ape);
    int get_ape_anim_flags(struct Ape * ape, undefined4 param_2, int param_3);
    void preload_ape(int chara_index, int LOD, int s_mal, int game_index);
    void g_load_ape_mta_mal_files(int game_index, int chara_index);
    uint g_something_with_comparing_strings(undefined4 param_1, char * param_2);
    void register_ape(struct Ape * ape);
    void unregister_ape(struct Ape * ape);
    Ape get_ape_index(struct Ape * __return_storage_ptr__, int index);
    void dip_apenum_draw(struct Ape * ape);
    char * get_ape_name_string(struct Ape * ape);
    char * get_ape_lod_string(struct Ape * ape);
    char * get_ape_face_string(struct Ape * ape);
    int seek(char * param_1);
    void g_load_models(struct Ape * ape);
    void g_something_freeing_heap_3(void * param_1);
    int g_get_ape_flag(struct Ape * ape, undefined4 g_something_with_game, int param_3);
    void gan_setanim_e4(struct Ape * ape);
    void gan_setAnim2(struct Ape * ape, int param_2, short * param_3);
    int g_some_ape_anim_func2(int param_1, int param_2, undefined4 param_3, undefined4 param_4, int param_5);
    void g_something_with_GXPeekZ(void);
    void empty_function(void);
    void g_init_rankings_to_defaults_wrapper(void);
    void g_init_rankings_to_defaults(void);
    void g_NameEntry2_InitFirst(void);
    void g_nameentry2_init(void);
    void ape_assignment(void);
    void sprite_rank_tick(void);
    void sprite_rank_disp(undefined8 param_1, undefined8 param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8);
    void g_something_with_name_entry_get_course(int param_1);
    void g_some_ape_anim_func1(struct Ape * ape);
    void set_ape_anim(struct Ape * ape, undefined animationType);
    void g_some_set_ape_anim(struct Ape * ape, undefined chara_anim_type, undefined2 param_3);
    void run_anim_funcs(struct Ape * ape);
    void g_something_with_loading_cutscenes(int param_1_00);
    uint g_something_to_do_with_cutscenes(int g_cutscene_id);
    void empty_function(void);
    void empty_function(void);
    void g_something_freeing_heap_4(void);
    void g_something_with_cutscenes_func2(void);
    undefined4 g_something_with_cutscenes_func(u16 g_something_cutscene);
    void g_dump_scene_info(void);
    void g_something_to_do_with_soft_fx(undefined4 param_1, undefined4 param_2, int param_3);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void g_SceneSoundLoad(char param_1_00);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void smd_author_select_init(void);
    void smd_author_select_tick(void);
    void g_print_author_debug_data(void);
    void smd_author_play_init(void);
    void smd_author_play_tick(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void smd_author_play_story_init(void);
    void smd_author_play_ending_init(void);
    void smd_author_play_from_sel(void);
    void smd_author_play_ret_sel(void);
    void md_author_func(void);
    void g_assign_new_main_and_sub_mode_for_play_tick(MainMode  param_1, SubMode  param_2);
    void g_set_some_author_tick_func(undefined4 param_1);
    undefined4 g_get_some_author_related_data(void);
    undefined4 g_get_author_scene(void);
    int g_get_author_scene_max(void);
    undefined2 g_get_author_frame(void);
    undefined2 g_get_author_frame_max(void);
    void g_some_author_cutscene_related_func(void);
    void g_load_stageselect_after_cutscene(void);
    void g_preload_next_stage_files(int param_1, int param_2, int param_3);
    void clear_unlock_info(void);
    byte g_is_master_unlocked(void);
    void empty_function(void);
    void g_set_unlockables_status(void);
    void empty_function(void);
    void compare_play_points_with_99999_after_exit_game(void);
    dword get_play_point_count(void);
    void g_display_playpoint_or_gift_message(double param_1, double param_2, undefined param_3);
    void unlock_party_game(int party_game);
    void g_something_with_assinging_unlockables(void);
    void unlock_movie(int movie);
    void unlock_staff_credits_game(void);
    uint g_check_if_partygame_unlocked(int param_1);
    int get_num_of_unlocked_party_games(void);
    bool are_all_party_games_unlocked(void);
    void set_unlocked_monkeys_to_three(void);
    byte get_num_unlocked_monkeys(void);
    void g_set_movie_as_unlocked(int param_1);
    uint g_is_movie_unlocked(int param_1);
    undefined4 return_5(void);
    undefined4 return_0(void);
    void main_game_rel_prolog(void);
    void main_game_rel_epilog(void);
    void main_game_rel_unlinked(void);
    void md_game_func(void);
    void smd_game_first_init(void);
    void smd_game_restart_init(void);
    void smd_game_ready_init(void);
    void smd_game_ready_tick(void);
    void smd_game_play_init(void);
    void smd_game_play_tick(void);
    void smd_game_goal_init(void);
    void smd_game_goal_tick(void);
    void smd_game_goal_replay_init(void);
    void smd_game_goal_replay_tick(void);
    void smd_game_continue_init(void);
    void smd_game_continue_tick(void);
    void smd_game_timeover_init(void);
    void smd_game_timeover_tick(void);
    void smd_game_ringout_init(void);
    void smd_game_ringout_tick(void);
    void smd_game_bonus_clear_init(void);
    void smd_game_bonus_clear_tick(void);
    void smd_game_first_tick(void);
    void smd_game_over_init(void);
    void smd_game_over_point_init(void);
    void smd_game_over_point_tick(void);
    void smd_game_scenario_init(void);
    void smd_game_scenario_tick(void);
    void g_some_dest_function_handler(void);
    void smd_game_scenario_return(void);
    void smd_game_retry_init(void);
    void smd_game_retry_tick(void);
    void smd_game_over_save(void);
    void smd_game_over_dest(void);
    void smd_game_nameentry_ready_init(void);
    void smd_game_nameentry_ready_tick(void);
    void smd_game_nameentry_init(void);
    void smd_game_nameentry_tick(void);
    void smd_game_roll_init(void);
    void smd_game_roll_tick(undefined4 param_1, undefined4 param_2_00, short param_3);
    void smd_game_extra_init(void);
    void smd_game_extra_tick(void);
    void smd_game_result_init(void);
    void smd_game_result_tick(void);
    void smd_game_result_menu(void);
    void smd_game_intr_sel_init(void);
    void smd_game_intr_sel_tick(void);
    void smd_game_sugg_save_init(void);
    void smd_game_sugg_save_tick(void);
    int get_next_stage_id(void);
    uint g_decrement_active_ball_monkey_count(void);
    void smd_game_force_exit_init(void);
    void smd_game_force_exit_tick(void);
    void smd_game_scenscnplay_return(void);
    void smd_game_force_over_init(void);
    void smd_game_force_over_tick(void);
    void g_preload_stage_or_results(void);
    void g_challenge_mode_start(struct Ball * ball);
    void challenge_mode_physics(struct Ball * ball);
    void apply_velocity(struct Ball * ball);
    void g_smth_with_mystery_3(struct Ball * ball);
    void g_camera_func89(struct Camera * camera, struct Ball * ball);
    void g_camera_func90(struct Camera * camera, struct Ball * ball);
    void smd_game_scenario_init_child(void);
    void smd_game_scenario_tick_child(void);
    void g_some_storymode_dest_function(void);
    undefined4 get_storymode_score(void);
    void set_storymode_score(int value);
    undefined4 get_storymode_banana_count(void);
    void set_storymode_bananas(int banana_count);
    void g_preload_ape_model_for_stageselect(void);
    void g_save_storymode_progress(void * param_1);
    int g_is_timer_30s_or_60s_current_stage(void);
    int is_timer_30s_or_60s_wrapper(int param_1, int param_2);
    void g_some_scenario_init_func_1(void);
    void g_some_storymode_mode_handler(void);
    void g_get_storymode_playtime_frames(void);
    void dmd_scen_select_init(void);
    void dmd_scen_select_main(void);
    void dmd_scen_1st_init(void);
    void dmd_scen_return_init(void);
    void dmd_scen_newgame_init(void);
    void dmd_scen_newgame_main(void);
    void dmd_scen_loadgame_init(void);
    void dmd_scen_loadgame_main(void);
    void dmd_scen_loadgame_restore(void);
    void dmd_scen_sceneplay_init(void);
    void dmd_scen_sceneplay_main(void);
    void dmd_scen_scnplay_return(void);
    void dmd_scen_scnplay_return_loading(void);
    void dmd_scen_sel_world_init(void);
    void dmd_scen_sel_world_next(void);
    void dmd_scen_sel_floor_init(void);
    void dmd_scen_sel_floor_main(void);
    void dmd_scen_game_init(void);
    void dmd_scen_game_main(void);
    void dmd_scen_game_clear_init(void);
    void dmd_scen_game_clear_main(void);
    void dmd_scen_nameentry_init(void);
    void dmd_scen_nameentry_main(void);
    void dmd_scen_game_over_init(void);
    void dmd_scen_game_over_main(void);
    void dmd_scen_entry_init(void);
    void dmd_scen_entry_main(void);
    void g_some_storymode_mode_func(void);
    StoryModeSaveFile * get_current_storymode_save_file(void);
    void g_some_scenario_init_func_4(void);
    void g_something_freeing_something_from_main_heap(void);
    void empty_function(void);
    void empty_function(void);
    void g_create_storymode_select_sprites(uint param_1);
    void g_related_to_loading_story_stageselect(uint param_1);
    void g_draw_ape_storymode_select_screen(int param_1);
    void empty_function(void);
    void g_preload_all_story_preview_images(void);
    void g_draw_now_loading_text(void);
    uint g_get_storymode_next_world(void);
    void * g_check_if_game_over_or_after_credits_sequence(int param_1, void * param_2);
    void empty_function(void);
    void g_some_scenario_init_func_3(void);
    void g_handle_storymode_stageselect_state(void);
    void dmd_scen_sel_floor_init_child(void);
    void g_set_some_storymode_stageids(void);
    void g_load_new_storymode_stage(u32 param_1);
    void g_some_scenario_init_func_2(void);
    void empty_function(void);
    void g_smth_with_storymode(void);
    void sprite_story_file_nameentry_disp(void);
    void create_story_file_nameentry_sprite(void);
    void dmd_scen_entry_main_child(void);
    void g_get_story_file_name(char * param_1);
    void draw_storymode_save_file(struct StoryModeSaveFile * saveFile, int index);
    void g_related_to_loading_dataselect_menu(void);
    void dmd_scen_loadgame_main_child(void);
    int get_storymode_menu_state(void);
    void staff_roll_init(void);
    void event_view_init(void);
    void event_view_tick(void);
    void event_view_dest(void);
    void g_something_with_view_stage2(void);
    void draw_reflective_objects_in_viewstage(void);
    void g_smth_with_rendering_models_for_reflective_surfaces(void);
    void g_draw_stobjs_in_viewstage(void);
    void g_something_with_view_stage_camera(struct Camera * camera);
    void sel_ngc_prolog(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void sel_ngc_epilog(void);
    void sel_ngc_unlinked_func(void);
    bool did_any_pad_press_input(PadInputID  input_id);
    void g_create_main_menu(void);
    void smd_sel_ngc_init(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void smd_sel_ngc_reinit(void);
    undefined4 g_called_when_fading_to_storymode(void);
    void g_sel_ngc_item_selected(int play_menu_selction_sound);
    void g_b_button_pressed_on_menu(int param_1_00);
    BOOL32 g_is_screen_in_screen_stack(byte g_screen_id);
    void g_some_menu_handling_func(void);
    void g_something_with_menus(int param_1);
    void g_menu_main_handler(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void g_menu_party_mode_handler(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8);
    void switch_mode_to_smd_adv_title_reinit(void);
    void g_something_with_menus2(undefined8 param_1, undefined8 param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, undefined4 param_9, undefined4 param_10, int * param_11, MenuScreenID  * param_12, u8 * param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void g_something_with_menus3(void);
    void g_on_main_menu_pressed(void);
    void g_something_with_menus4(void);
    void g_something_with_practice_stage_select_screen(void);
    void enter_challenge_mode(void);
    void enter_practice_mode(void);
    byte * g_handle_starting_monkeys_count(int param_1, int param_2);
    void create_main_menu_sprites(void);
    void g_display_game_settings_sprite(void);
    void sprite_title_str_tick(u8 * status, struct Sprite * sprite);
    void sprite_title_str_disp(struct Sprite * sprite);
    void sprite_info_str_tick(u8 * status, struct Sprite * sprite);
    void sprite_info_str_disp(struct Sprite * sprite);
    void sprite_start_mask_tick(u8 * status, struct Sprite * sprite);
    void sprite_menu_basic_tick(u8 * param_1, struct Sprite * param_2);
    void sprite_menu_basic_disp(struct Sprite * sprite);
    void sprite_menu_basic_stack_tick(u8 * status, struct Sprite * sprite);
    void sprite_menu_basic_title_tick(u8 * status, struct Sprite * param_2);
    void sprite_menu_basic_title_disp(struct Sprite * sprite);
    void sprite_game_settings_tick(undefined * param_1, int param_2);
    void sprite_game_settings_disp(int param_1);
    void sprite_practice_stage_select_tick(undefined * param_1, int param_2);
    void sprite_practice_stage_select_disp(undefined8 param_1, undefined8 param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, int param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void sprite_fight_stage_select_tick(undefined * param_1, int param_2);
    void sprite_fight_stage_select_disp(int param_1);
    void sprite_button_tick(undefined4 param_1, int param_2);
    void sprite_button_disp(struct Sprite * param_1);
    void g_blink_selected_menu_item(void);
    void g_something_with_menus5(void);
    void g_load_select_apes(void);
    void g_load_some_apes(void);
    void handle_main_menu_bg(void);
    void g_draw_menu_bg_overlay(struct GXColor * param_1, GXTexFmt  param_2);
    void free_some_apes(void);
    void g_draw_stage_0x9f_for_menu_bg(void);
    void g_draw_stage_0x8a_for_menu_bg(void);
    void g_draw_stage_for_menu_bg_2(void);
    void sprite_practice_stage_preview_tick(u8 * status, struct Sprite * sprite);
    void sprite_practice_stage_preview_disp(struct Sprite * sprite);
    void sprite_practice_stage_preview_mask_disp(u8 * status, struct Sprite * sprite);
    void g_create_some_practice_mode_preview_sprites(void);
    void g_draw_controller_sprites(void);
    void empty_function(void);
    void sel_stage_unlinked_func(void);
    void g_references_420_03(void);
    void bowling_rel_prolog(void);
    void bowling_rel_epilog(void);
    void bowling_rel_unlinked(void);
    void g_init_bowling(void);
    void g_bowl_drawFunc(struct Ape * param_1, int param_2, undefined4 param_3, undefined4 param_4, undefined4 param_5, undefined4 param_6, undefined4 param_7, undefined4 param_8);
    void g_load_missing_apes(void);
    void g_load_commend_apes(void);
    void g_load_selthing(void);
    BOOL32 ballid_matches_chara(u32 chara_index);
    void mini_billiards_unlinked_func(void);
    void g_references_420_04(void);
    void sample_unlinked_func(void);
    void test_mode_prolog(void);
    void test_mode_unlinked_func(void);
    void g_references_420_05_maybe_wraparound_for_debug(void);
    void g_init_smd_test_sound_main(void);
    void g_something_with_debug_mode_sound(void);
    void g_draw_debug_mode_sound_screen(undefined8 param_1, undefined8 param_2, double param_3, double param_4, double param_5, double param_6, double param_7, double param_8, undefined4 param_9, undefined4 param_10, int param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void g_something_that_loads_common_p_dot_lz(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void g_some_printf_function_7(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, char * param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void g_something_freeing_something_from_main_heap_2(void);
    void gan_setanim_e5(struct ArcFileInfo * * param_1, char * param_2, void * * param_3);
    void gan_set_anim_e6(int param_1);
    void gan_setanim_e7(int param_1, undefined4 param_2, int param_3);
    void gan_setanim_e8(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4, int param_5);
    void option_unlinked_func(void);
    void g_references_420_06(int param_1, short param_2);
    void race2_rel_prolog(void);
    void race2_rel_epilog(void);
    void race2_rel_unresolved(void);
    void race2_tick(void);
    void g_load_race(void);
    void g_draw_race_timer_ui(void);
    void g_print_race_time_remaining(undefined4 param_1, int param_2);
    void golf2_unlinked_func(void);
    void g_golf_init(void);
    void empty_function(void);
    void mini_fight2_unlinked_func(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void g_references_420_07(double param_1, double param_2, int param_3);
    void empty_function(void);
    void empty_function(void);
    void pilot2_unlinked_func(void);
    void g_load_pilot(void);
    void empty_function(void);
    void g_some_printf_function_8(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void g_some_printf_function_9(double param_1, double param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, uint param_9, undefined4 param_10, char * param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void item_pilot_init(struct Item * item);
    void item_pilot_tick(struct Item * item);
    void item_pilot_disp(struct Item * item);
    void item_pilot_coli(int param_1, int param_2);
    void item_pilot_dest(void);
    void item_pilot_replay_init(void);
    void item_debug_pilot(void);
    void boat_unlinked_func(void);
    void g_load_boat(void);
    void shooting_unlinked_func(void);
    void g_read_something_for_shooting_from_dvd(char * param_1, struct ArcFileInfo * * param_2, int * param_3);
    void g_read_something_for_shooting_2(struct SKLFile * * param_1, char * param_2, struct SKLRoot * * param_3);
    void mini_futsal_unlinked_func(void);
    void empty_function(void);
    void empty_function(void);
    void load_bg_ape(void);
    void dogfight_unlinked_func(void);
    void g_load_dog(void);
    void g_some_printf_function_10(undefined8 param_1, undefined8 param_2, undefined8 param_3, undefined8 param_4, undefined8 param_5, undefined8 param_6, undefined8 param_7, undefined8 param_8, char * param_9, undefined4 param_10, undefined4 param_11, undefined4 param_12, undefined4 param_13, undefined4 param_14, undefined4 param_15, undefined4 param_16);
    void item_dogfight_init(struct Item * item);
    void item_dogfight_tick(struct Item * item);
    void item_dogfight_disp(struct Item * item);
    void item_dogfight_coli(int param_1, int param_2);
    void item_dogfight_dest(void);
    void item_dogfight_replay_init(void);
    void item_debug_dogfight(void);
    void g_baseball_prolog(void);
    void baseball_unlinked_func(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    undefined4 g_baseball_game_loop(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void g_load_baseball(byte player_id, BallMode  param_2, undefined param_3, byte param_4, byte param_5, uint param_6, void * param_7, int param_8);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void empty_function(void);
    void g_baseball_init_seesaws(void);
    void empty_function(void);
    undefined4 g_get_sprite_work(int * param_1, int param_2);
    void empty_function(void);
    void g_tennis_prolog(void);
    void tennis_unlinked_func(void);
    double something_with_distance_sq(float * param_1, float * param_2);
    void GXResetOverflowCount(int param_1, undefined4 param_2);
    void ttyClearProperty(int param_1, undefined4 param_2);
    void exoption_unlinked_func(void);
#ifdef __cplusplus
} // extern "C"
#endif
