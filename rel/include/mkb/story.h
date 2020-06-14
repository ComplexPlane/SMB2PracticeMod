#pragma once

#include <cstdint>

namespace mkb
{

// Yes this is the same as "story mode file select" the naming in ghidra atm is just inconsistent ok
enum DataSelectMenuState
{
    DSMS_DEFAULT = 0,
    DSMS_CHOOSE_DATA_TO_COPY = 1,
    DSMS_CHOOSE_WHERE_TO_COPY_TO = 2,
    DSMS_CONFIRM_COPY = 3,
    DSMS_CANCELLED_COPY = 4,
    DSMS_COPY_DATA = 5,
    DSMS_CHOOSE_DATA_TO_DELETE = 6,
    DSMS_CONFIRM_DELETE = 7,
    DSMS_CANCELLED_DELETE = 8,
    DSMS_DELETE_DATA = 9,
    DSMS_OPEN_DATA = 10,
    DSMS_RETURN_SELECTED = 11,
    DSMS_RETURN_TO_MAIN_GAME_SELECT = 12
};

struct StoryModeSaveFile
{
    uint8_t unk_0x0[4];
    uint32_t statusFlag;
    char fileName[13];
    uint8_t currentWorld;
    uint8_t unk_0x16[2];
    uint32_t playtimeInFrames;
    uint32_t score;
    uint8_t unk_0x20[4];
    uint8_t numBeatenStagesInWorld;
    uint8_t beatenStageIndices[10];
    uint8_t unk_0x2f[85];
} __attribute__((__packed__));

static_assert(sizeof(StoryModeSaveFile) == 132);

extern "C"
{
extern StoryModeSaveFile storyModeSaveFiles[3];
extern uint8_t dataSelectMenuState;
extern uint8_t storyFileSelectState;
extern uint8_t selectedStoryFileIdx;
}

}