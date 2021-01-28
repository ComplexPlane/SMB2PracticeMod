#pragma once

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
    u8 unk_0x0[4];
    u32 status_flag;
    char fileName[13];
    u8 current_world;
    u8 unk_0x16[2];
    u32 playtime_in_frames;
    u32 score;
    u8 unk_0x20[4];
    u8 num_beaten_stages_in_world;
    u8 beatenStageIndices[10];
    u8 unk_0x2f[85];
} __attribute__((__packed__));

static_assert(sizeof(StoryModeSaveFile) == 132);

extern "C"
{
extern StoryModeSaveFile storymode_save_files[3];
extern u16 g_storymode_mode;
extern u8 curr_storymode_save_file_idx;

// These get wiped after loading a savefile (main_game REL gets reloaded when loading a new file??)
extern u8 data_select_menu_state;
extern u8 story_file_select_state;
extern u8 selected_story_file_idx;

extern char continue_saved_game_text[40];
extern char start_game_from_beginning_text[36];
extern u16 curr_world;
}

}