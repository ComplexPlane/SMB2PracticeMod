#include "iw.h"

#include "mkb/mkb.h"

#include "mods/freecam.h"
#include "systems/assembly.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/relutil.h"
#include "utils/timerdisp.h"

// TODO: track best times per world
// I tried this before but it seems like there might be a spurious frame where it thinks the IW is
// completed when beating the second-to-last level, so the fastest time isn't saving correctly.

namespace iw {

static u32 s_anim_counter;
static const char* s_anim_strs[4] = {"/", "-", "\\", " |"};

static u32 s_iw_files;  // Bitflag for which save files are IW save files

// IW timer stuff
static u32 s_iw_time;
static u32 s_prev_retrace_count;

static void handle_iw_selection() {
    if (mkb::scen_info.mode != 5) return;

    if (pad::analog_down(mkb::PAI_LSTICK_LEFT) || pad::analog_down(mkb::PAI_LSTICK_RIGHT)) return;
    if (pad::button_down(mkb::PAD_BUTTON_LEFT) || pad::button_down(mkb::PAD_BUTTON_RIGHT)) return;

    bool lstick_up = pad::analog_pressed(mkb::PAI_LSTICK_UP);
    bool lstick_down = pad::analog_pressed(mkb::PAI_LSTICK_DOWN);
    bool dpad_up = pad::button_pressed(mkb::PAD_BUTTON_UP);
    bool dpad_down = pad::button_pressed(mkb::PAD_BUTTON_DOWN);

    s32 dir = lstick_up || dpad_up ? +1 : (lstick_down || dpad_down ? -1 : 0);
    auto& story_save = mkb::storymode_save_files[mkb::selected_story_file_idx];
    if (s_iw_files & (1 << mkb::selected_story_file_idx)) {
        s32 world = story_save.current_world + dir;
        if (world < 0 || world > 9) {
            story_save.is_valid = 0;
            s_iw_files &= ~(1 << mkb::selected_story_file_idx);
        } else {
            story_save.current_world = world;
        }
    } else {
        if (dir != 0) {
            story_save.is_valid = 1;
            story_save.current_world = dir == +1 ? 0 : 9;
            s_iw_files |= (1 << mkb::selected_story_file_idx);
        }
    }
}

static void set_save_file_info() {
    if (mkb::scen_info.mode != 5) return;

    s_anim_counter += 1;

    for (s32 i = 0; i < 3; i++) {
        auto& story_save = mkb::storymode_save_files[i];
        if (s_iw_files & (1 << i)) {
            mkb::sprintf(story_save.file_name, "W%02d IW %s", story_save.current_world + 1,
                         s_anim_strs[s_anim_counter / 2 % 4]);
            story_save.num_beaten_stages_in_current_world = 0;
            story_save.score = 0;
            story_save.playtime_in_frames = 0;
        }
    }
}

static void handle_iw_timer() {
    u32 retrace_count = mkb::VIGetRetraceCount();

    // Halt the timer if we're selecting the story mode file
    // If we're still on the file selection screen and the IW file has been opened though,
    // start the timer during the open animation (to be more consistent with prior versions
    // of the IW code)
    if (mkb::scen_info.mode == 5 && mkb::data_select_menu_state != mkb::DSMS_OPEN_DATA) {
        // We're not actually in the IW, zero the timer
        s_iw_time = 0;
    } else if (main::currently_playing_iw && !main::is_iw_complete()) {
        // We're in story mode playing an IW and it isn't finished, so increment the IW timer
        s_iw_time += retrace_count - s_prev_retrace_count;
    }
    // Else we're in story mode playing an IW, but we finished it, so don't change the time

    s_prev_retrace_count = retrace_count;
}

void init() {
    patch::write_branch(relutil::relocate_addr(0x80274804),
                        reinterpret_cast<void*>(main::stage_select_menu_hook));
    patch::write_branch(relutil::relocate_addr(0x8032a86c),
                        reinterpret_cast<void*>(main::pause_menu_text_hook));
}

void tick() {
    main::currently_playing_iw = false;
    if (mkb::main_mode != mkb::MD_GAME || mkb::main_game_mode != mkb::STORY_MODE) return;

    const char* msg = "Up/Down to Change World.";
    mkb::strcpy(mkb::continue_saved_game_text, const_cast<char*>(msg));
    mkb::strcpy(mkb::start_game_from_beginning_text, const_cast<char*>(msg));

    handle_iw_selection();
    set_save_file_info();

    u8 file_idx =
        mkb::scen_info.mode == 5 ? mkb::selected_story_file_idx : mkb::scen_info.save_file_idx;

    // Maybe not the best way to detect if we're playing an IW but it works
    main::currently_playing_iw = s_iw_files & (1 << file_idx);

    handle_iw_timer();
}

void disp() {
    if (!pref::get(pref::BoolPref::IwTimer) || mkb::main_mode != mkb::MD_GAME ||
        mkb::main_game_mode != mkb::STORY_MODE || !main::currently_playing_iw ||
        freecam::should_hide_hud())
        return;
    timerdisp::draw_timer(static_cast<s32>(s_iw_time), "IW:", 0, draw::WHITE, false);
}

}  // namespace iw
