#pragma once

namespace mkb
{

constexpr s32 MAX_STAGE_OBJECTS = 144;
constexpr s32 MAX_GOALS = 16;

enum StobjType
{
    STOBJ_BUMPER = 0,
    STOBJ_JAMABAR = 1,
    STOBJ_GOALTAPE = 2,
    STOBJ_GOALBAG = 3,
    STOBJ_GOALBAG_EXMASTER = 4,
    STOBJ_MF_PNL_BUMPER = 5,
    STOBJ_MF_PNL_ELECTRAP = 6,
    STOBJ_MF_BULLET_TEST = 7,
    STOBJ_MF_BOX = 8,
    STOBJ_BUMPER_BGSPECIAL = 9,
    STOBJ_NAMEENT_BTN = 10,
    STOBJ_BUTTON = 11,
    STOBJ_RETURNGATE = 12
};

struct Stobj
{
    u16 idx;
    u16 id;
    u16 type;
    u8 unk_0x6[198];
};

static_assert(sizeof(Stobj) == 204);

// Extra data associated with goaltape stage objects
struct GoalTape
{
    u8 unk_0x0[408];
};

static_assert(sizeof(GoalTape) == 408);

// Extra data associated with party ball stage objects
struct GoalBag
{
    u8 unk_0x0[40];
};

static_assert(sizeof(GoalBag) == 40);

extern "C"
{
extern Stobj stobjs[MAX_STAGE_OBJECTS];
extern PoolInfo stobj_pool_info;
extern GoalTape goaltapes[MAX_GOALS];
extern GoalBag goalbags[MAX_GOALS];
}

}