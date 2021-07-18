#include "cmseg.h"

#include <mkb.h>
#include <log.h>
#include <patch.h>

#define ARRAY_LEN(x) (sizeof((x)) / sizeof((x)[0]))

namespace cmseg
{

enum class State
{
    Default,
    LoadMenuReq,
    LoadCmReq,
};

static State s_state = State::Default;
static Seg s_seg_request;
static void (*s_g_reset_cm_course_tramp)();

static bool s_cm_seg_active;
static s16 s_seg_course_stage_num;

//static void debug_print_course(mkb::CmEntry *course, u32 entry_count)
//{
//    static const char *type_strs[] = {"CMET_IF", "CMET_THEN", "CMET_INFO", "CMET_END"};
//
//    mkb::OSReport("Course entry count: %d\n", entry_count);
//    for (u32 i = 0; i < entry_count; i++)
//    {
//        mkb::CmEntry &entry = course[i];
//        mkb::OSReport("%s: n = %d, v = %d\n", type_strs[entry.type], entry.arg, entry.value);
//    }
//    mkb::OSReport("\n");
//}

/**
 * Create a new course on top of an existing one, by inserting a CMET_END entry and returning the first entry of the
 * segment.
 */
static void gen_course(mkb::CmEntry *course, u32 start_course_stage_num, u32 stage_count)
{
    s32 start_entry_idx = -1;
    s32 end_entry_idx = -1;

    u32 curr_stage_count = 0;
    for (s32 i = 0;; i++)
    {
        if (course[i].type == mkb::CMET_INFO && course[i].arg == 0)
        {
            curr_stage_count++;
            if (curr_stage_count == start_course_stage_num)
            {
                start_entry_idx = i;
            }
            else if (curr_stage_count == start_course_stage_num + stage_count)
            {
                end_entry_idx = i;
                break;
            }
        }
        else if (course[i].type == mkb::CMET_END)
        {
            if (curr_stage_count == start_course_stage_num + stage_count - 1)
            {
                end_entry_idx = i; // This CmEntry is one past the end - we tack on a CMET_END entry ourselves
            }
            break;
        }
    }

    // Check if we found stage indices
    MOD_ASSERT(start_entry_idx > -1);
    MOD_ASSERT(end_entry_idx > -1);

    // Write new course end marker
    // TODO backup and restore
    course[end_entry_idx].type = mkb::CMET_END;

    s16 first_stage_id = static_cast<s16>(course[start_entry_idx].value);
//    mkb::mode_info.next_cm_stage_id = first_stage_id; // Record first stage in course
    mkb::mode_info.cm_course_stage_num = start_course_stage_num;
    mkb::current_cm_entry = &course[start_entry_idx + 1];
    mkb::g_some_cm_stage_id2 = first_stage_id;

    // Make up "previous" stage for "current" stage
    mkb::CmStage &curr_stage = mkb::cm_player_progress[0].curr_stage;
    curr_stage.stage_course_num = start_course_stage_num - 1;
    curr_stage.stage_id = first_stage_id - 1;

    // Next stage for player is the first one we want to start on
    mkb::CmStage &next_stage = mkb::cm_player_progress[0].next_stages[0];
    next_stage.stage_course_num = start_course_stage_num;
    next_stage.stage_id = first_stage_id;

    s_cm_seg_active = true;
    s_seg_course_stage_num = start_course_stage_num;
}

void init()
{
    s_g_reset_cm_course_tramp = patch::hook_function(
        mkb::g_reset_cm_course, []()
        {
            s_g_reset_cm_course_tramp();
            if (s_cm_seg_active) mkb::mode_info.cm_course_stage_num = s_seg_course_stage_num;
        }
    );
}

void tick()
{
    if (s_state == State::LoadMenuReq)
    {
        mkb::g_some_other_flags &= ~mkb::OF_GAME_PAUSED; // Unpause the game to avoid weird darkening issues
        mkb::main_mode_request = mkb::MD_SEL;
        // Using REINIT instead of INIT seems to prevent weird game state issues, like
        // the Final Stage sprite being shown when loading a stage in story mode
        mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;


        s_state = State::LoadCmReq;
    }
    else if (s_state == State::LoadCmReq)
    {
        // TODO set difficulty, flags etc. based on requested course
        // TODO enforce 1-player game
        // TODO character, lives
        mkb::selected_cm_difficulty = 0;

        mkb::enter_challenge_mode();
        gen_course(mkb::beginner_noex_cm_entries, 2, 2);

        // TODO restore main menu state to look like we entered Challenge Mode
        // TODO do this before loading REINIT to avoid mode.cnt = 0 error (and in Go To Story Mode too)

        s_state = State::Default;
    }
}

void request_cm_seg(Seg seg)
{
    s_seg_request = seg;
    if (mkb::main_mode == mkb::MD_SEL) s_state = State::LoadCmReq; // Load challenge mode directly
    else s_state = State::LoadMenuReq; // Load main menu first
}

}