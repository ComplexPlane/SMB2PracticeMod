#include "cmseg.h"

#include "mkb/mkb.h"

#include "mods/freecam.h"
#include "systems/log.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

namespace cmseg {

enum class State {
    Default,
    LoadMenu,
    EnterCm,
    SegActive,
    SegComplete,
};

static State s_state = State::Default;
static Seg s_seg_request;
static u32 s_start_time;
static u32 s_seg_time;

static patch::Tramp<decltype(&mkb::g_reset_cm_course)> s_reset_cm_course_tramp;

static mkb::CourseCommand* s_overwritten_entry;
static mkb::CourseCommandOpcode s_overwritten_opcode;
static s8 s_overwritten_starting_monkeys;

static u32 s_pbs[13];

// static void debug_print_course(mkb::CourseCommand *course, u32 entry_count)
//{
//     static const char *type_strs[] = {"COURSE_CMD_IF", "COURSE_CMD_THEN", "COURSE_CMD_INFO",
//     "COURSE_CMD_END"};
//
//     mkb::OSReport("Course entry count: %d\n", entry_count);
//     for (u32 i = 0; i < entry_count; i++)
//     {
//         mkb::CourseCommand &entry = course[i];
//         mkb::OSReport("%s: n = %d, v = %d\n", type_strs[entry.opcode], entry.arg, entry.value);
//     }
//     mkb::OSReport("\n");
// }

/**
 * Create a new course in an existing one by inserting a COURSE_CMD_END entry
 */
static void gen_course(mkb::CourseCommand* course, u32 start_course_stage_num, u32 stage_count) {
    s32 start_entry_idx = -1;
    s32 end_entry_idx = -1;

    u32 curr_stage_count = 0;
    for (s32 i = 0;; i++) {
        if (course[i].opcode == mkb::COURSE_CMD_INFO && course[i].type == 0) {
            curr_stage_count++;
            if (curr_stage_count == start_course_stage_num) {
                start_entry_idx = i;
            } else if (curr_stage_count == start_course_stage_num + stage_count) {
                end_entry_idx = i;
                break;
            }
        } else if (course[i].opcode == mkb::COURSE_CMD_END) {
            if (curr_stage_count == start_course_stage_num + stage_count - 1) {
                end_entry_idx = i;  // This CourseCommand is one past the end - we tack on a
                                    // COURSE_CMD_END entry ourselves
            }
            break;
        }
    }

    // Check if we found stage indices
    MOD_ASSERT(start_entry_idx > -1);
    MOD_ASSERT(end_entry_idx > -1);

    // Write new course end marker
    s_overwritten_entry = &course[end_entry_idx];
    s_overwritten_opcode = course[end_entry_idx].opcode;
    course[end_entry_idx].opcode = mkb::COURSE_CMD_END;

    s16 first_stage_id = static_cast<s16>(course[start_entry_idx].value);
    mkb::mode_info.cm_course_stage_num = start_course_stage_num;
    mkb::mode_info.cm_stage_id = first_stage_id;  // Record first stage in course
    mkb::current_cm_entry = &course[start_entry_idx + 1];
    mkb::g_another_stage_id = first_stage_id;

    // Make up "previous" stage for "current" stage
    mkb::CmStage& curr_stage = mkb::cm_player_progress[0].curr_stage;
    curr_stage.stage_course_num = start_course_stage_num - 1;
    curr_stage.stage_id = first_stage_id - 1;

    // Next stage for player is the first one we want to start on
    mkb::CmStage& next_stage = mkb::cm_player_progress[0].next_stages[0];
    next_stage.stage_course_num = start_course_stage_num;
    next_stage.stage_id = first_stage_id;
}

static void state_load_menu() {
    mkb::g_some_other_flags &=
        ~mkb::OF_GAME_PAUSED;  // Unpause the game to avoid weird darkening issues
    mkb::main_mode_request = mkb::MD_SEL;
    // Using REINIT instead of INIT seems to prevent weird game state issues, like
    // the Final Stage sprite being shown when loading a stage in story mode
    mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;

    // Set menu state to have chosen Main Game -> Challenge Mode
    mkb::sel_menu_info.menu_stack_ptr = 1;
    mkb::sel_menu_info.menu_stack[0] = 0;  // ??
    mkb::sel_menu_info.menu_stack[1] = 7;  // Main game
    mkb::g_focused_root_menu = 0;
    mkb::g_focused_maingame_menu = 1;

    mkb::fade_screen_to_color(
        0x100, 0, 0x1e);  // Same as what's used to fade in stages normally in challenge mode

    s_state = State::EnterCm;
}

static const mkb::ApeCharacter s_ape_charas[] = {mkb::APE_AIAI, mkb::APE_MEEMEE, mkb::APE_BABY,
                                                 mkb::APE_GONGON};

static void state_enter_cm() {
    mkb::num_players = 1;
    s_overwritten_starting_monkeys = mkb::number_of_starting_monkeys;
    mkb::number_of_starting_monkeys = 100;
    mkb::menu_start_challenge_mode();

    // Reset this timer to prevent dark pause menu when leaving an Exit Game menu. The game draws
    // the pause menu sprite at a different depth depending on this global, which can incorrectly
    // place it behind the pause menu dim quad.
    mkb::g_playpoint_msg_counter = 0;

    s_start_time = mkb::VIGetRetraceCount();
    s_state = State::SegActive;
}

static void restore_overwritten_state() {
    s_overwritten_entry->opcode = s_overwritten_opcode;  // Restore original challenge mode course
    mkb::number_of_starting_monkeys = s_overwritten_starting_monkeys;
}

static void check_exit_seg() {
    // Restore overwritten state if we exit main mode and thus thw IW
    if (mkb::main_mode != mkb::MD_GAME) {
        restore_overwritten_state();
        s_state = State::Default;
    }
}

static void state_seg_active() {
    // Set character
    if (mkb::sub_mode_request == mkb::SMD_GAME_READY_INIT) {
        Chara ch = static_cast<Chara>(pref::get(pref::U8Pref::CmChara));
        mkb::ApeCharacter real_chara;
        if (ch == Chara::Random) {
            real_chara = s_ape_charas[mkb::rand() % 4];
        } else {
            real_chara = s_ape_charas[pref::get(pref::U8Pref::CmChara)];
        }
        mkb::active_monkey_id[0] = real_chara;
    }

    // Nuke "Final Stage" sprite
    if (s_overwritten_opcode != mkb::COURSE_CMD_END) {
        for (u32 i = 0; i < mkb::sprite_pool_info.upper_bound; i++) {
            if (mkb::sprite_pool_info.status_list[i] == 0) continue;
            mkb::Sprite& sprite = mkb::sprites[i];
            if (sprite.tick_func == mkb::sprite_final_stage_tick) {
                mkb::sprite_pool_info.status_list[i] = 0;
                break;
            }
        }
    }

    s_seg_time = mkb::VIGetRetraceCount() - s_start_time;
    if (mkb::mode_info.cm_stage_id == -1 && mkb::is_stage_complete(nullptr)) {
        u32 seg = static_cast<u32>(s_seg_request);
        if (s_seg_time < s_pbs[seg]) {
            s_pbs[seg] = s_seg_time;
        }
        s_state = State::SegComplete;
    }

    check_exit_seg();
}

void state_seg_complete() {
    // If the final stage of the segment is a bonus stage, do a custom transition back to main menu
    if (mkb::mode_info.cm_stage_id == -1 &&
        mkb::mode_info.ball_mode & mkb::BALLMODE_ON_BONUS_STAGE) {
        if (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
            mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT) {
            mkb::sub_mode_frame_counter += 120;
        } else if ((mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN ||
                    mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) &&
                   mkb::sub_mode_frame_counter == 0x3c) {
            // Same as post-goal fadeout when completing difficulty
            mkb::fade_screen_to_color(0x101, 0, 0x3d);
            mkb::g_fade_track_volume(0x3c, 2);
        } else if (mkb::sub_mode_request == mkb::SMD_GAME_READY_INIT) {
            mkb::main_mode_request = mkb::MD_SEL;
            mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;
        }
    }

    // If we've reached end-of-difficulty, go back to main menu
    if (mkb::sub_mode_request == mkb::SMD_GAME_EXTRA_INIT ||
        mkb::sub_mode_request == mkb::SMD_GAME_RESULT_INIT ||
        mkb::main_mode_request == mkb::MD_AUTHOR) {
        mkb::main_mode_request = mkb::MD_SEL;
        mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;
    }

    check_exit_seg();
}

void init_seg() {
    mkb::CourseCommand* course = nullptr;
    u32 start_course_stage_num = 0;
    mkb::mode_flags &= ~(mkb::MF_G_PLAYING_MASTER_COURSE | mkb::MF_PLAYING_EXTRA_COURSE |
                         mkb::MF_PLAYING_MASTER_NOEX_COURSE | mkb::MF_PLAYING_MASTER_EX_COURSE);
    switch (s_seg_request) {
        case Seg::Beginner1: {
            mkb::curr_difficulty = mkb::DIFF_BEGINNER;
            course = mkb::cm_courses[0];
            start_course_stage_num = 1;
            break;
        }
        case Seg::BeginnerExtra: {
            mkb::curr_difficulty = mkb::DIFF_BEGINNER;
            mkb::mode_flags |= mkb::MF_PLAYING_EXTRA_COURSE;
            course = mkb::cm_courses[3];
            start_course_stage_num = 1;
            break;
        }
        case Seg::Advanced1: {
            mkb::curr_difficulty = mkb::DIFF_ADVANCED;
            course = mkb::cm_courses[1];
            start_course_stage_num = 1;
            break;
        }
        case Seg::Advanced11: {
            mkb::curr_difficulty = mkb::DIFF_ADVANCED;
            course = mkb::cm_courses[1];
            start_course_stage_num = 11;
            break;
        }
        case Seg::Advanced21: {
            mkb::curr_difficulty = mkb::DIFF_ADVANCED;
            course = mkb::cm_courses[1];
            start_course_stage_num = 21;
            break;
        }
        case Seg::AdvancedExtra: {
            mkb::curr_difficulty = mkb::DIFF_ADVANCED;
            mkb::mode_flags |= mkb::MF_PLAYING_EXTRA_COURSE;
            course = mkb::cm_courses[4];
            start_course_stage_num = 1;
            break;
        }
        case Seg::Expert1: {
            mkb::curr_difficulty = mkb::DIFF_EXPERT;
            course = mkb::cm_courses[2];
            start_course_stage_num = 1;
            break;
        }
        case Seg::Expert11: {
            mkb::curr_difficulty = mkb::DIFF_EXPERT;
            course = mkb::cm_courses[2];
            start_course_stage_num = 11;
            break;
        }
        case Seg::Expert21: {
            mkb::curr_difficulty = mkb::DIFF_EXPERT;
            course = mkb::cm_courses[2];
            start_course_stage_num = 21;
            break;
        }
        case Seg::Expert31: {
            mkb::curr_difficulty = mkb::DIFF_EXPERT;
            course = mkb::cm_courses[2];
            start_course_stage_num = 31;
            break;
        }
        case Seg::Expert41: {
            mkb::curr_difficulty = mkb::DIFF_EXPERT;
            course = mkb::cm_courses[2];
            start_course_stage_num = 41;
            break;
        }
        case Seg::ExpertExtra: {
            mkb::curr_difficulty = mkb::DIFF_EXPERT;
            mkb::mode_flags |= mkb::MF_PLAYING_EXTRA_COURSE;
            course = mkb::cm_courses[5];
            start_course_stage_num = 1;
            break;
        }
        case Seg::Master1: {
            mkb::curr_difficulty = mkb::DIFF_EXPERT;
            mkb::mode_flags |= mkb::MF_PLAYING_EXTRA_COURSE | mkb::MF_G_PLAYING_MASTER_COURSE |
                               mkb::MF_PLAYING_MASTER_NOEX_COURSE;
            course = mkb::cm_courses[6];
            start_course_stage_num = 1;
            break;
        }
        case Seg::MasterExtra: {
            mkb::curr_difficulty = mkb::DIFF_EXPERT;
            // Magic set of flags used in Master Extra,
            // can't be bothered to reverse all of them
            mkb::mode_flags = 0x0280071D;
            course = mkb::cm_courses[7];
            start_course_stage_num = 1;
            break;
        }
    }
    gen_course(course, start_course_stage_num, 10);
}

void request_cm_seg(Seg seg) {
    s_seg_request = seg;
    if (s_state == State::SegActive || s_state == State::SegComplete) {
        restore_overwritten_state();
    }
    if (mkb::main_mode == mkb::MD_SEL) {
        s_state = State::EnterCm;  // Load challenge mode directly
    } else {
        s_state = State::LoadMenu;  // Load main menu first
    }
}

void init() {
    patch::hook_function(s_reset_cm_course_tramp, mkb::g_reset_cm_course, []() {
        s_reset_cm_course_tramp.dest();
        if (s_state == State::SegActive) init_seg();
    });

    // Set PBs to maximum time
    for (u32& s_pb : s_pbs) {
        s_pb = 0xFFFFFFFF;
    }
}

void tick() {
    if (s_state == State::LoadMenu) {
        state_load_menu();
    } else if (s_state == State::EnterCm) {
        state_enter_cm();
    } else if (s_state == State::SegActive) {
        state_seg_active();
    } else if (s_state == State::SegComplete) {
        state_seg_complete();
    }
}

void disp() {
    if (!pref::get(pref::BoolPref::CmTimer) || freecam::should_hide_hud()) return;

    if (s_state == State::SegActive || s_state == State::SegComplete) {
        u32 seg = static_cast<u32>(s_seg_request);
        mkb::GXColor color;
        if (s_state == State::SegComplete && s_pbs[seg] == s_seg_time)
            color = draw::GOLD;
        else
            color = draw::WHITE;
        timerdisp::draw_timer(static_cast<s32>(s_seg_time), "SEG:", 0, color, false);
    }
}

}  // namespace cmseg
