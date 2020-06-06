#pragma once

namespace mkb
{

constexpr int MAX_STAGE_OBJECTS = 256;

enum StageObjectType
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

struct StageObject
{
    uint16_t idx;
    uint16_t id;
    uint16_t type;
    uint8_t unk_0x6[198];
};

static_assert(sizeof(StageObject) == 204);

extern "C"
{
extern StageObject stageObjects[MAX_STAGE_OBJECTS];
extern TickableListMeta stobjListMeta;
}

}