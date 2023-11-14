#include "timer.h"

#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

namespace timer {

static u32 s_retrace_count;
static u32 s_prev_retrace_count;
static s32 s_rta_timer;
static s32 s_pause_timer;

static u32 s_framesave;
static patch::Tramp<decltype(&mkb::did_ball_enter_goal)> s_goal_tramp;

static bool line_intersects(const Vec& lineStart, const Vec& lineEnd, const mkb::Rect& rect) {
    Vec end;
    Vec start;
    float half_height;
    float half_width;

    start.x = lineStart.x;
    start.y = lineStart.y;
    start.z = lineStart.z;
    end.x = lineEnd.x;
    end.y = lineEnd.y;
    end.z = lineEnd.z;
    mkb::mtxa_from_translate(&const_cast<Vec&>(rect.pos));
    mkb::mtxa_rotate_z((rect.rot).z);
    mkb::mtxa_rotate_y((rect.rot).y);
    mkb::mtxa_rotate_x((rect.rot).x);
    mkb::mtxa_rigid_inv_tf_point(&start, &start);
    mkb::mtxa_rigid_inv_tf_point(&end, &end);
    if (((end.z < 0.0) && (start.z < 0.0)) || ((0.0 < end.z && (0.0 < start.z)))) {
        return false;
    } else {
        half_width = start.z - end.z;
        if (1.192093e-07 < half_width) {
            end.x = end.x - (start.x - end.x) * (end.z / half_width);
            end.y = end.y - (start.y - end.y) * (end.z / half_width);
        }
        half_width = rect.width * 0.5;
        half_height = rect.height * 0.5;
        if ((end.x < -half_width) || (half_width < end.x)) {
            return false;
        } else if ((end.y < -half_height) || (half_height < end.y)) {
            return false;
        } else {
            // update framesave if first goal entered
            if (mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT &&
                mkb::sub_mode != mkb::SMD_GAME_GOAL_MAIN &&
                mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_INIT &&
                mkb::sub_mode != mkb::SMD_GAME_GOAL_REPLAY_MAIN) {
                s_framesave = static_cast<u32>((start.z / (start.z - end.z)) * 100);
            }
            return true;
        }
    }
}

void find_framesave(mkb::Ball* ball, int* out_stage_goal_idx, int* out_itemgroup_id,
                    mkb::byte* out_goal_flags) {
    // mostly a ghidra copy-paste
    int itemgroup_goal_idx;
    mkb::StagedefGoal* goal;
    mkb::dword itemgroup_idx;
    int stage_goal_idx;
    mkb::StagedefColiHeader* itemgroup;
    mkb::Rect goal_trigger;
    mkb::PhysicsBall physicsball;

    mkb::init_physicsball_from_ball(ball, &physicsball);
    stage_goal_idx = 0;
    itemgroup = mkb::stagedef->coli_header_list;
    itemgroup_idx = 0;
    do {
        if ((int)mkb::stagedef->coli_header_count <= (int)itemgroup_idx) {
            break;
        }
        if (0 < (int)itemgroup->goal_count) {
            if (itemgroup_idx != physicsball.itemgroup_idx) {
                mkb::tf_physball_to_itemgroup_space(&physicsball, itemgroup_idx);
            }
            goal = itemgroup->goal_list;
            for (itemgroup_goal_idx = 0; itemgroup_goal_idx < (int)itemgroup->goal_count;
                 itemgroup_goal_idx = itemgroup_goal_idx + 1) {
                mkb::mtxa_from_translate(&goal->position);
                mkb::mtxa_rotate_z((goal->rotation).z);
                mkb::mtxa_rotate_y((goal->rotation).y);
                mkb::mtxa_rotate_x((goal->rotation).x);
                goal_trigger.pos.x = 0.0;
                goal_trigger.pos.y = 1.5;
                goal_trigger.pos.z = 0.0;
                mkb::mtxa_tf_point(&goal_trigger.pos, &goal_trigger.pos);
                goal_trigger.rot.x = (goal->rotation).x;
                goal_trigger.rot.y = (goal->rotation).y;
                goal_trigger.rot.z = (goal->rotation).z;
                goal_trigger.width = 3.0;
                goal_trigger.height = 3.0;
                if (line_intersects(physicsball.pos, physicsball.prev_pos, goal_trigger)) {
                    return;  // found a goal that ball travelled thru
                }
                stage_goal_idx = stage_goal_idx + 1;
                goal = goal + 1;
            }
        }
        itemgroup_idx = itemgroup_idx + 1;
        itemgroup = itemgroup + 1;
    } while (true);
}

void init() {
    s_retrace_count = mkb::VIGetRetraceCount();

    patch::hook_function(
        s_goal_tramp, &mkb::did_ball_enter_goal,
        [](mkb::Ball* ball, int* out_stage_goal_idx, int* out_itemgroup_id,
           mkb::byte* out_goal_flags) {
            bool result =
                s_goal_tramp.dest(ball, out_stage_goal_idx, out_itemgroup_id, out_goal_flags);
            if (result) {
                // determine framesave percentage
                find_framesave(ball, out_stage_goal_idx, out_itemgroup_id, out_goal_flags);
            }
            return result;
        });
}

// Need to do logic in disp() so that we can know the game state _after_ the frame has processed
void disp() {
    if (mkb::main_mode != mkb::MD_GAME) return;

    switch (mkb::sub_mode) {
        case mkb::SMD_GAME_READY_INIT:
        case mkb::SMD_GAME_READY_MAIN:
        case mkb::SMD_GAME_PLAY_INIT:
        case mkb::SMD_GAME_PLAY_MAIN:
        case mkb::SMD_GAME_GOAL_INIT:
        case mkb::SMD_GAME_GOAL_MAIN:
        case mkb::SMD_GAME_RINGOUT_INIT:
        case mkb::SMD_GAME_RINGOUT_MAIN:
        case mkb::SMD_GAME_TIMEOVER_INIT:
        case mkb::SMD_GAME_TIMEOVER_MAIN:
        case mkb::SMD_GAME_GOAL_REPLAY_INIT:
        case mkb::SMD_GAME_GOAL_REPLAY_MAIN:
            break;
        default:
            s_pause_timer = 0;
            return;
    }

    s_prev_retrace_count = s_retrace_count;
    s_retrace_count = mkb::VIGetRetraceCount();

    if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
        s_rta_timer = mkb::mode_info.stage_time_limit;
        s_pause_timer = 0;
    } else if ((mkb::mode_info.g_ball_mode & mkb::BALLMODE_FREEZE_TIMER) == 0) {
        s_rta_timer -= s_retrace_count - s_prev_retrace_count;
        //        if (s_rtaTimer < 0) s_rtaTimer = 0;
        if (mkb::g_some_other_flags & mkb::OF_GAME_PAUSED) {
            s_pause_timer++;
        }
    }
    
    u32 row = 1;
    
    if (pref::get(pref::BoolPref::TimerShowRTA) && !freecam::should_hide_hud()) {
        timerdisp::draw_timer(380, row++, 44, "RTA:", s_rta_timer, 0, false, true, draw::WHITE);
    }

    if (pref::get(pref::BoolPref::TimerShowPause) && !freecam::should_hide_hud()) {
        timerdisp::draw_timer(380, row++, 44, "PAU:", s_pause_timer, 0, false, true, draw::WHITE);
    }
    

    switch (mkb::sub_mode) {
        case mkb::SMD_GAME_GOAL_INIT:
        case mkb::SMD_GAME_GOAL_MAIN:
        case mkb::SMD_GAME_GOAL_REPLAY_INIT:
        case mkb::SMD_GAME_GOAL_REPLAY_MAIN:
            break;
        default:
            return;
    }

    if (pref::get(pref::BoolPref::TimerShowSubtick) && !freecam::should_hide_hud()) {
        timerdisp::draw_subtick_timer(mkb::mode_info.stage_time_frames_remaining, "SUB:", row++,
                                      draw::WHITE, true, s_framesave, false);
    }

    if (pref::get(pref::BoolPref::TimerShowUnrounded) && !freecam::should_hide_hud()) {
        timerdisp::draw_subtick_timer(mkb::mode_info.stage_time_frames_remaining, "CUR:", row++,
                                      draw::WHITE, true, 0, false);
        timerdisp::draw_subtick_timer(mkb::mode_info.stage_time_frames_remaining + 1, "NXT:", row++,
                                      draw::WHITE, true, 0, false);
    }

    if (pref::get(pref::BoolPref::TimerShowFramesave) && !freecam::should_hide_hud()) {
        timerdisp::draw_percentage(s_framesave, "FSV:", row++, draw::WHITE);
    } 
}

void save_state(memstore::MemStore* store) {
    store->do_region(&s_retrace_count, sizeof(s_retrace_count));
    store->do_region(&s_prev_retrace_count, sizeof(s_prev_retrace_count));
    store->do_region(&s_rta_timer, sizeof(s_rta_timer));
    store->do_region(&s_pause_timer, sizeof(s_pause_timer));
    if (store->get_mode() == memstore::Mode::LOAD) {
        // This might not be completely consistently correct
        u32 count = mkb::VIGetRetraceCount();
        s_prev_retrace_count = count - 1;
        s_retrace_count = count - 1;
    }
}

}  // namespace timer
