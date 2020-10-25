#include "iw.h"
#include "pad.h"
#include "assembly.h"

#include <mkb/mkb.h>
#include <cstring>
#include <cstdio>
#include <draw.h>

// TODO: track best times per world
// I tried this before but it seems like there might be a spurious frame where it thinks the IW is completed
// when beating the second-to-last level, so the fastest time isn't saving correctly.

namespace iw
{

static u32 s_anim_counter;
static const char *s_anim_strs[4] = {"/", "-", "\\", " |"};

// IW timer stuff
static u32 s_iw_time;
static u32 s_prev_retrace_count;

void init() {}

static void handle_iw_selection()
{
    if (mkb::data_select_menu_state != mkb::DSMS_DEFAULT) return;
    if (mkb::story_file_select_state == 1) return;
    if (pad::analog_down(pad::AR_LSTICK_LEFT) || pad::analog_down(pad::AR_LSTICK_RIGHT)) return;
    if (pad::button_down(pad::BUTTON_DPAD_LEFT) || pad::button_down(pad::BUTTON_DPAD_RIGHT)) return;

    bool lstick_up = pad::analog_pressed(pad::AR_LSTICK_UP);
    bool lstick_down = pad::analog_pressed(pad::AR_LSTICK_DOWN);
    bool dpad_up = pad::button_pressed(pad::BUTTON_DPAD_UP);
    bool dpad_down = pad::button_pressed(pad::BUTTON_DPAD_DOWN);

    s32 dir = lstick_up || dpad_up ? +1 : (lstick_down || dpad_down ? -1 : 0);
    auto &story_save = mkb::storymode_save_files[mkb::selected_story_file_idx];
    if (story_save.status_flag)
    {
        s32 world = story_save.current_world + dir;
        if (world < 0 || world > 9) story_save.status_flag = 0;
        else story_save.current_world = world;
    }
    else
    {
        if (dir != 0)
        {
            story_save.status_flag = 1;
            story_save.current_world = dir == +1 ? 0 : 9;
        }
    }

    main::currently_playing_iw = story_save.status_flag;
}

static void set_save_file_info()
{
    s_anim_counter += 1;

    for (s32 i = 0; i < 3; i++)
    {
        auto &story_save = mkb::storymode_save_files[i];
        if (story_save.status_flag)
        {
            sprintf(story_save.fileName, "W%02d IW %s",
                    story_save.current_world + 1,
                    s_anim_strs[s_anim_counter / 2 % 4]);
            story_save.num_beaten_stages_in_world = 0;
            story_save.score = 0;
            story_save.playtime_in_frames = 0;
        }
    }
}

static void handle_iw_timer()
{
    u32 retrace_count = gc::VIGetRetraceCount();

    if (mkb::main_mode != mkb::MD_GAME
        || mkb::main_game_mode != mkb::MGM_STORY
        || mkb::data_select_menu_state != mkb::DSMS_OPEN_DATA)
    {
        // We're not actually in the IW, zero the timer
        s_iw_time = 0;
    }
    else if (main::currently_playing_iw && !main::is_iw_complete())
    {
        // We're in story mode playing an IW and it isn't finished, so increment the IW timer
        s_iw_time += retrace_count - s_prev_retrace_count;
    }
    // Else we're in story mode playing an IW, but we finished it, so don't change the time

    s_prev_retrace_count = retrace_count;
}

void tick()
{
    if (mkb::main_mode == mkb::MD_GAME && mkb::main_game_mode == mkb::MGM_STORY)
    {
        if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_INIT)
        {
            const char *msg = "Up/Down to Change World.";
            strcpy(mkb::continue_saved_game_text, msg);
            strcpy(mkb::start_game_from_beginning_text, msg);
        }

        handle_iw_selection();
        set_save_file_info();

        // Maybe not the best way to detect if we're playing an IW but it works
        if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_MAIN)
        {
            mkb::StoryModeSaveFile &file = mkb::storymode_save_files[mkb::selected_story_file_idx];
            main::currently_playing_iw =
                file.status_flag
                && file.fileName[0] == 'W'
                && file.fileName[4] == 'I'
                && file.fileName[5] == 'W';
        }
    }

    handle_iw_timer();
}

void disp()
{
    if (mkb::main_mode != mkb::MD_GAME || mkb::main_game_mode != mkb::MGM_STORY || !main::currently_playing_iw) return;

    constexpr u32 SECOND = 60;
    constexpr u32 MINUTE = SECOND * 60;
    constexpr u32 HOUR = MINUTE * 60;

    constexpr s32 X = 380;
    constexpr s32 Y = 18;

    u32 hours = s_iw_time / HOUR;
    u32 minutes = s_iw_time % HOUR / MINUTE;
    u32 seconds = s_iw_time % MINUTE / SECOND;
    u32 centiseconds = (s_iw_time % SECOND) * 100 / 60;

    if (hours > 0)
    {
        draw::debug_text(X, Y, draw::Color::WHITE, "IW:  %d:%02d:%02d.%02d", hours, minutes, seconds, centiseconds);
    }
    else if (minutes > 0)
    {
        draw::debug_text(X, Y, draw::Color::WHITE, "IW:  %02d:%02d.%02d", minutes, seconds, centiseconds);
    }
    else
    {
        draw::debug_text(X, Y, draw::Color::WHITE, "IW:  %02d.%02d", seconds, centiseconds);
    }
}

}