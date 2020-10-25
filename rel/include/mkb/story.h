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
    uint32_t status_flag;
    char fileName[13];
    uint8_t current_world;
    uint8_t unk_0x16[2];
    uint32_t playtime_in_frames;
    uint32_t score;
    uint8_t unk_0x20[4];
    uint8_t num_beaten_stages_in_world;
    uint8_t beatenStageIndices[10];
    uint8_t unk_0x2f[85];
} __attribute__((__packed__));

static_assert(sizeof(StoryModeSaveFile) == 132);

extern "C"
{
extern StoryModeSaveFile storymode_save_files[3];
extern uint8_t data_select_menu_state;
extern uint8_t story_file_select_state;
extern uint8_t selected_story_file_idx;
extern char continue_saved_game_text[40];
extern char start_game_from_beginning_text[36];
extern uint16_t curr_world;
}

}