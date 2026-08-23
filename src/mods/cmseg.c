#include "mods/cmseg.h"

#include "mods/freecam.h"
#include "mods/ilmark.h"
#include "systems/log.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

typedef enum State {
    State_Default,
    State_LoadMenu,
    State_EnterCm,
    State_SegActive,
    State_SegComplete,
} State;

static State s_state = State_Default;
static CM_Seg s_seg_request;
static u32 s_start_time;
static u32 s_seg_time;

static mkb_CourseCommand *s_overwritten_entry;
static mkb_CourseCommandOpcode s_overwritten_opcode;
static s8 s_overwritten_starting_monkeys;

static u32 s_pbs[14];

// static void debug_print_course(mkb_CourseCommand *course, u32 entry_count)
//{
//     static const char *type_strs[] = {"COURSE_CMD_IF", "COURSE_CMD_THEN", "COURSE_CMD_INFO",
//     "COURSE_CMD_END"};
//
//     mkb_OSReport("Course entry count: %d\n", entry_count);
//     for (u32 i = 0; i < entry_count; i++)
//     {
//         mkb_CourseCommand &entry = course[i];
//         mkb_OSReport("%s: n = %d, v = %d\n", type_strs[entry.opcode], entry.arg, entry.value);
//     }
//     mkb_OSReport("\n");
// }

/**
 * Create a new course in an existing one by inserting a COURSE_CMD_END entry
 */
static void gen_course(mkb_CourseCommand *course, u32 start_course_stage_num, u32 stage_count) {
    s32 start_entry_idx = -1;
    s32 end_entry_idx = -1;

    u32 curr_stage_count = 0;
    for (s32 i = 0;; i++) {
        if (course[i].opcode == mkb_COURSE_CMD_INFO && course[i].type == 0) {
            curr_stage_count++;
            if (curr_stage_count == start_course_stage_num) {
                start_entry_idx = i;
            } else if (curr_stage_count == start_course_stage_num + stage_count) {
                end_entry_idx = i;
                break;
            }
        } else if (course[i].opcode == mkb_COURSE_CMD_END) {
            if (curr_stage_count == start_course_stage_num + stage_count - 1) {
                end_entry_idx = i;  // This CourseCommand is one past the end - we tack on a
                                    // COURSE_CMD_END entry ourselves
            }
            break;
        }
    }

    // Check if we found stage indices
    ASSERT(start_entry_idx > -1);
    ASSERT(end_entry_idx > -1);

    // Write new course end marker
    s_overwritten_entry = &course[end_entry_idx];
    s_overwritten_opcode = course[end_entry_idx].opcode;
    course[end_entry_idx].opcode = mkb_COURSE_CMD_END;

    s16 first_stage_id = (s16)course[start_entry_idx].value;
    mkb_mode_info.cm_course_stage_num = start_course_stage_num;
    mkb_mode_info.cm_stage_id = first_stage_id;  // Record first stage in course
    mkb_current_cm_entry = &course[start_entry_idx + 1];
    mkb_g_another_stage_id = first_stage_id;

    // Make up "previous" stage for "current" stage
    mkb_CmStage *curr_stage = &mkb_cm_player_progress[0].curr_stage;
    curr_stage->stage_course_num = start_course_stage_num - 1;
    curr_stage->stage_id = first_stage_id - 1;

    // Next stage for player is the first one we want to start on
    mkb_CmStage *next_stage = &mkb_cm_player_progress[0].next_stages[0];
    next_stage->stage_course_num = start_course_stage_num;
    next_stage->stage_id = first_stage_id;
}

static void state_load_menu() {
    mkb_g_some_other_flags &=
        ~mkb_OF_GAME_PAUSED;  // Unpause the game to avoid weird darkening issues
    mkb_main_mode_request = mkb_MD_SEL;
    // Using REINIT instead of INIT seems to prevent weird game state issues, like
    // the Final Stage sprite being shown when loading a stage in story mode
    mkb_sub_mode_request = mkb_SMD_SEL_NGC_REINIT;

    // Set menu state to have chosen Main Game -> Challenge Mode
    mkb_sel_menu_info.menu_stack_ptr = 1;
    mkb_sel_menu_info.menu_stack[0] = 0;  // ??
    mkb_sel_menu_info.menu_stack[1] = 7;  // Main game
    mkb_g_focused_root_menu = 0;
    mkb_g_focused_maingame_menu = 1;

    mkb_fade_screen_to_color(
        0x100, 0, 0x1e);  // Same as what's used to fade in stages normally in challenge mode

    s_state = State_EnterCm;
}

static const mkb_ApeCharacter s_ape_charas[] = {mkb_CHARA_AIAI, mkb_CHARA_MEEMEE,
                                                mkb_CHARA_BABY, mkb_CHARA_GONGON};

static void state_enter_cm() {
    mkb_num_players = 1;
    s_overwritten_starting_monkeys = mkb_number_of_starting_monkeys;
    mkb_number_of_starting_monkeys = 100;
    mkb_menu_start_challenge_mode();

    // Reset this timer to prevent dark pause menu when leaving an Exit Game menu. The game draws
    // the pause menu sprite at a different depth depending on this global, which can incorrectly
    // place it behind the pause menu dim quad.
    mkb_g_playpoint_msg_counter = 0;

    s_start_time = mkb_VIGetRetraceCount();
    s_state = State_SegActive;
}

static void restore_overwritten_state() {
    s_overwritten_entry->opcode = s_overwritten_opcode;  // Restore original challenge mode course
    mkb_number_of_starting_monkeys = s_overwritten_starting_monkeys;
}

static void check_exit_seg() {
    // Restore overwritten state if we exit main mode and thus thw IW
    if (mkb_main_mode != mkb_MD_GAME) {
        restore_overwritten_state();
        s_state = State_Default;
    }
}

static void state_seg_active() {
    // Set character
    if (mkb_sub_mode_request == mkb_SMD_GAME_READY_INIT) {
        CM_Chara ch = (CM_Chara)Pref_Get(Pref_CmChara);
        mkb_ApeCharacter real_chara;
        if (ch == CM_Chara_Random) {
            real_chara = s_ape_charas[mkb_rand() % 4];
        } else {
            real_chara = s_ape_charas[Pref_Get(Pref_CmChara)];
        }
        mkb_active_monkey_id[0] = real_chara;
    }

    // Nuke "Final Stage" sprite
    if (s_overwritten_opcode != mkb_COURSE_CMD_END) {
        for (u32 i = 0; i < mkb_sprite_pool_info.upper_bound; i++) {
            if (mkb_sprite_pool_info.status_list[i] == 0) continue;
            mkb_Sprite *sprite = &mkb_sprites[i];
            if (sprite->tick_func == mkb_sprite_final_stage_tick) {
                mkb_sprite_pool_info.status_list[i] = 0;
                break;
            }
        }
    }

    s_seg_time = mkb_VIGetRetraceCount() - s_start_time;
    if (mkb_mode_info.cm_stage_id == -1 && mkb_is_stage_complete(nullptr)) {
        u32 seg = (u32)s_seg_request;
        if (s_seg_time < s_pbs[seg]) {
            s_pbs[seg] = s_seg_time;
        }
        s_state = State_SegComplete;
    }

    check_exit_seg();
}

static void state_seg_complete() {
    // If the final stage of the segment is a bonus stage, do a custom transition back to main menu
    if (mkb_mode_info.cm_stage_id == -1 &&
        mkb_mode_info.ball_mode & mkb_BALLMODE_ON_BONUS_STAGE) {
        if (mkb_sub_mode == mkb_SMD_GAME_RINGOUT_INIT ||
            mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_INIT) {
            mkb_sub_mode_frame_counter += 120;
        } else if ((mkb_sub_mode == mkb_SMD_GAME_RINGOUT_MAIN ||
                    mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_MAIN) &&
                   mkb_sub_mode_frame_counter == 0x3c) {
            // Same as post-goal fadeout when completing difficulty
            mkb_fade_screen_to_color(0x101, 0, 0x3d);
            mkb_g_fade_track_volume(0x3c, 2);
        } else if (mkb_sub_mode_request == mkb_SMD_GAME_READY_INIT) {
            mkb_main_mode_request = mkb_MD_SEL;
            mkb_sub_mode_request = mkb_SMD_SEL_NGC_REINIT;
        }
    }

    // If we've reached end-of-difficulty, go back to main menu
    if (mkb_sub_mode_request == mkb_SMD_GAME_EXTRA_INIT ||
        mkb_sub_mode_request == mkb_SMD_GAME_RESULT_INIT ||
        mkb_main_mode_request == mkb_MD_AUTHOR) {
        mkb_main_mode_request = mkb_MD_SEL;
        mkb_sub_mode_request = mkb_SMD_SEL_NGC_REINIT;
    }

    check_exit_seg();
}

static void init_seg() {
    mkb_CourseCommand *course = nullptr;
    u32 start_course_stage_num = 0;
    mkb_mode_flags &= ~(mkb_MF_G_PLAYING_MASTER_COURSE | mkb_MF_PLAYING_EXTRA_COURSE |
                        mkb_MF_PLAYING_MASTER_NOEX_COURSE | mkb_MF_PLAYING_MASTER_EX_COURSE);
    switch (s_seg_request) {
        case CM_Seg_Beginner1: {
            mkb_curr_difficulty = mkb_DIFF_BEGINNER;
            course = mkb_cm_courses[0];
            start_course_stage_num = 1;
            break;
        }
        case CM_Seg_BeginnerExtra: {
            mkb_curr_difficulty = mkb_DIFF_BEGINNER;
            mkb_mode_flags |= mkb_MF_PLAYING_EXTRA_COURSE;
            course = mkb_cm_courses[3];
            start_course_stage_num = 1;
            break;
        }
        case CM_Seg_Advanced1: {
            mkb_curr_difficulty = mkb_DIFF_ADVANCED;
            course = mkb_cm_courses[1];
            start_course_stage_num = 1;
            break;
        }
        case CM_Seg_Advanced11: {
            mkb_curr_difficulty = mkb_DIFF_ADVANCED;
            course = mkb_cm_courses[1];
            start_course_stage_num = 11;
            break;
        }
        case CM_Seg_Advanced21: {
            mkb_curr_difficulty = mkb_DIFF_ADVANCED;
            course = mkb_cm_courses[1];
            start_course_stage_num = 21;
            break;
        }
        case CM_Seg_AdvancedExtra: {
            mkb_curr_difficulty = mkb_DIFF_ADVANCED;
            mkb_mode_flags |= mkb_MF_PLAYING_EXTRA_COURSE;
            course = mkb_cm_courses[4];
            start_course_stage_num = 1;
            break;
        }
        case CM_Seg_Expert1: {
            mkb_curr_difficulty = mkb_DIFF_EXPERT;
            course = mkb_cm_courses[2];
            start_course_stage_num = 1;
            break;
        }
        case CM_Seg_Expert11: {
            mkb_curr_difficulty = mkb_DIFF_EXPERT;
            course = mkb_cm_courses[2];
            start_course_stage_num = 11;
            break;
        }
        case CM_Seg_Expert21: {
            mkb_curr_difficulty = mkb_DIFF_EXPERT;
            course = mkb_cm_courses[2];
            start_course_stage_num = 21;
            break;
        }
        case CM_Seg_Expert31: {
            mkb_curr_difficulty = mkb_DIFF_EXPERT;
            course = mkb_cm_courses[2];
            start_course_stage_num = 31;
            break;
        }
        case CM_Seg_Expert41: {
            mkb_curr_difficulty = mkb_DIFF_EXPERT;
            course = mkb_cm_courses[2];
            start_course_stage_num = 41;
            break;
        }
        case CM_Seg_ExpertExtra: {
            mkb_curr_difficulty = mkb_DIFF_EXPERT;
            mkb_mode_flags |= mkb_MF_PLAYING_EXTRA_COURSE;
            course = mkb_cm_courses[5];
            start_course_stage_num = 1;
            break;
        }
        case CM_Seg_Master1: {
            mkb_curr_difficulty = mkb_DIFF_EXPERT;
            mkb_mode_flags |= mkb_MF_PLAYING_EXTRA_COURSE | mkb_MF_G_PLAYING_MASTER_COURSE |
                              mkb_MF_PLAYING_MASTER_NOEX_COURSE;
            course = mkb_cm_courses[6];
            start_course_stage_num = 1;
            break;
        }
        case CM_Seg_MasterExtra: {
            mkb_curr_difficulty = mkb_DIFF_EXPERT;
            // Magic set of flags used in Master Extra,
            // can't be bothered to reverse all of them
            mkb_mode_flags = 0x0280071D;
            course = mkb_cm_courses[7];
            start_course_stage_num = 1;
            break;
        }
    }
    gen_course(course, start_course_stage_num, 10);
}

void CM_RequestSeg(CM_Seg seg) {
    s_seg_request = seg;
    if (s_state == State_SegActive || s_state == State_SegComplete) {
        restore_overwritten_state();
    }
    if (mkb_main_mode == mkb_MD_SEL) {
        s_state = State_EnterCm;  // Load challenge mode directly
    } else {
        s_state = State_LoadMenu;  // Load main menu first
    }
}

static void reset_cm_course_hook();

TRAMP(s_reset_cm_course_tramp, mkb_g_reset_cm_course, reset_cm_course_hook);

static void reset_cm_course_hook() {
    s_reset_cm_course_tramp.chain();
    if (s_state == State_SegActive) init_seg();
}

static void sprite_go_disp_hook(mkb_Sprite *sprite);

TRAMP(s_sprite_go_disp_tramp, mkb_sprite_go_disp, sprite_go_disp_hook);

static void sprite_go_disp_hook(mkb_Sprite *sprite) {
    int i;
    int t;
    int x_offset;
    int y_offset;
    float x_add;
    float y_add;
    float phi_f30_2;

    t = sprite->para1 - sprite->g_counter;
    mkb_textdraw_reset();
    mkb_textdraw_set_font(sprite->font);
    mkb_textdraw_set_depth(sprite->depth);
    mkb_textdraw_set_flags(sprite->g_flags1);
    mkb_textdraw_set_alpha(sprite->alpha);
    mkb_textdraw_set_mul_color(
        RGBA(sprite->mult_color.red, sprite->mult_color.green, sprite->mult_color.blue, 0));
    mkb_textdraw_set_add_color(
        RGBA(sprite->add_color.red, sprite->add_color.green, sprite->add_color.blue, 0));
    mkb_textdraw_set_scale(1.5f * sprite->width, 1.5f * sprite->height);
    x_offset = 1.5f * (36.0f * sprite->width);
    y_offset = 1.5f * (32.0f * sprite->height);

    for (i = 0; i < 2; i++) {
        if (t < 15) {
            phi_f30_2 = (i == 0) ? -320.0f : 320.0f;
            x_add = phi_f30_2 * mkb_math_sin((0xF - t) * 0x444);
            y_add = 0.0f;
        } else if (t < 30) {
            x_add = 0.0f;
            y_add = 0.0f;
        } else if (t < 45) {
            x_add = 0.0f;
            y_add = 0.0f;
        } else {
            float x_fudge = 0;
            if (ILMark_IsEnabled()) {
                x_fudge = (t - 45) / 15.f * 5;
            }
            x_add = i == 0 ? -x_fudge : x_fudge;
            phi_f30_2 = (i == 0) ? -240.0f : 240.0f;
            y_add = phi_f30_2 * mkb_math_sin((0xF - sprite->g_counter) * 0x444);
        }
        mkb_textdraw_set_pos(
            (sprite->pos.x + x_add) - x_offset + ((i == 0) ? -x_offset : x_offset),
            (sprite->pos.y + y_add) - y_offset);
        mkb_textdraw_put_char((i == 0) ? 0x47 : 0x4F);
    }
}

void CM_Init() {
    HOOK_TRAMP(s_reset_cm_course_tramp);

    // Set PBs to maximum time
    for (u32 i = 0; i < LEN(s_pbs); i++) {
        s_pbs[i] = 0xFFFFFFFF;
    }

    HOOK_TRAMP(s_sprite_go_disp_tramp);
}

void CM_Tick() {
    if (s_state == State_LoadMenu) {
        state_load_menu();
    } else if (s_state == State_EnterCm) {
        state_enter_cm();
    } else if (s_state == State_SegActive) {
        state_seg_active();
    } else if (s_state == State_SegComplete) {
        state_seg_complete();
    }
}

void CM_Disp() {
    if (!Pref_Get(Pref_CmTimer) || freecam_should_hide_hud()) return;

    if (s_state == State_SegActive || s_state == State_SegComplete) {
        u32 seg = (u32)s_seg_request;
        GXColor color;
        if (s_state == State_SegComplete && s_pbs[seg] == s_seg_time)
            color = COLOR_GOLD;
        else
            color = COLOR_WHITE;
        timerdisp_draw_timer((s32)s_seg_time, "SEG:", 0, color, false);
    }
}
