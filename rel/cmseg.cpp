#include "cmseg.h"

#include <mkb.h>
#include <log.h>

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
static mkb::CmEntry *gen_course(mkb::CmEntry *course, u32 stage_start_idx, u32 stage_count)
{
    s32 start_entry_idx = -1;
    s32 end_entry_idx = -1;

    u32 curr_stage_count = 0;
    for (s32 i = 0;; i++)
    {
        if (course[i].type == mkb::CMET_INFO && course[i].arg == 0)
        {
            curr_stage_count++;
            if (curr_stage_count == stage_start_idx + 1)
            {
                start_entry_idx = i;
            }
            else if (curr_stage_count == stage_start_idx + stage_count + 1)
            {
                end_entry_idx = i;
                break;
            }
        }
        else if (course[i].type == mkb::CMET_END)
        {
            if (curr_stage_count == stage_start_idx + stage_count)
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

    return &course[start_entry_idx];
}

void init()
{
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
        mkb::selected_cm_difficulty = 0;

        mkb::CmEntry *first_entry = gen_course(mkb::beginner_noex_cm_entries, 3, 2);
        mkb::g_enter_challenge_mode();

        mkb::mode_info.g_some_cm_stage_id = first_entry->value; // Record first stage in course
        mkb::current_cm_entry = first_entry;
        mkb::g_some_cm_stage_id2 = mkb::mode_info.g_some_cm_stage_id;

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