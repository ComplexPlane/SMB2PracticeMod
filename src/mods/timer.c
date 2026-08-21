#include "timer.h"

#include "mods/freecam.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

static u32 s_retrace_count;
static u32 s_prev_retrace_count;
static s32 s_rta_timer;
static s32 s_pause_timer;

static u32 s_framesave;

static bool did_ball_enter_goal(mkb_Ball *ball,
                                int *out_stage_goal_idx,
                                int *out_itemgroup_id,
                                byte *out_goal_flags);

TRAMP(s_goal_tramp, mkb_did_ball_enter_goal, did_ball_enter_goal);

static bool line_intersects(const Vec *line_start, const Vec *line_end, const mkb_Rect *rect) {
    Vec end;
    Vec start;
    float half_height;
    float half_width;

    start = *line_start;
    end = *line_end;
    mkb_mtxa_from_translate((Vec *)&rect->pos);
    mkb_mtxa_rotate_z(rect->rot.z);
    mkb_mtxa_rotate_y(rect->rot.y);
    mkb_mtxa_rotate_x(rect->rot.x);
    mkb_mtxa_rigid_inv_tf_point(&start, &start);
    mkb_mtxa_rigid_inv_tf_point(&end, &end);
    if (((end.z < 0.0) && (start.z < 0.0)) || ((0.0 < end.z && (0.0 < start.z)))) {
        return false;
    } else {
        half_width = start.z - end.z;
        if (1.192093e-07 < half_width) {
            end.x = end.x - (start.x - end.x) * (end.z / half_width);
            end.y = end.y - (start.y - end.y) * (end.z / half_width);
        }
        half_width = rect->width * 0.5;
        half_height = rect->height * 0.5;
        if ((end.x < -half_width) || (half_width < end.x)) {
            return false;
        } else if ((end.y < -half_height) || (half_height < end.y)) {
            return false;
        } else {
            // update framesave if first goal entered
            if (mkb_sub_mode != mkb_SMD_GAME_GOAL_INIT && mkb_sub_mode != mkb_SMD_GAME_GOAL_MAIN &&
                mkb_sub_mode != mkb_SMD_GAME_GOAL_REPLAY_INIT &&
                mkb_sub_mode != mkb_SMD_GAME_GOAL_REPLAY_MAIN) {
                s_framesave = (u32)((start.z / (start.z - end.z)) * 100);
            }
            return true;
        }
    }
}

static void find_framesave(mkb_Ball *ball,
                           int *out_stage_goal_idx,
                           int *out_itemgroup_id,
                           byte *out_goal_flags) {
    // mostly a ghidra copy-paste
    int itemgroup_goal_idx;
    mkb_StagedefGoal *goal;
    dword itemgroup_idx;
    int stage_goal_idx;
    mkb_StagedefColiHeader *itemgroup;
    mkb_Rect goal_trigger;
    mkb_PhysicsBall physicsball;

    mkb_init_physicsball_from_ball(ball, &physicsball);
    stage_goal_idx = 0;
    itemgroup = mkb_stagedef->coli_header_list;
    itemgroup_idx = 0;
    do {
        if ((int)mkb_stagedef->coli_header_count <= (int)itemgroup_idx) {
            break;
        }
        if (0 < (int)itemgroup->goal_count) {
            if (itemgroup_idx != physicsball.itemgroup_idx) {
                mkb_tf_physball_to_itemgroup_space(&physicsball, itemgroup_idx);
            }
            goal = itemgroup->goal_list;
            for (itemgroup_goal_idx = 0; itemgroup_goal_idx < (int)itemgroup->goal_count;
                 itemgroup_goal_idx = itemgroup_goal_idx + 1) {
                mkb_mtxa_from_translate(&goal->position);
                mkb_mtxa_rotate_z((goal->rotation).z);
                mkb_mtxa_rotate_y((goal->rotation).y);
                mkb_mtxa_rotate_x((goal->rotation).x);
                goal_trigger.pos.x = 0.0;
                goal_trigger.pos.y = 1.5;
                goal_trigger.pos.z = 0.0;
                mkb_mtxa_tf_point(&goal_trigger.pos, &goal_trigger.pos);
                goal_trigger.rot.x = (goal->rotation).x;
                goal_trigger.rot.y = (goal->rotation).y;
                goal_trigger.rot.z = (goal->rotation).z;
                goal_trigger.width = 3.0;
                goal_trigger.height = 3.0;
                if (line_intersects(&physicsball.pos, &physicsball.prev_pos, &goal_trigger)) {
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

static bool did_ball_enter_goal(mkb_Ball *ball,
                                int *out_stage_goal_idx,
                                int *out_itemgroup_id,
                                byte *out_goal_flags) {
    bool result = s_goal_tramp.chain(ball, out_stage_goal_idx, out_itemgroup_id, out_goal_flags);
    if (result) {
        // Determine framesave percentage.
        find_framesave(ball, out_stage_goal_idx, out_itemgroup_id, out_goal_flags);
    }
    return result;
}

void timer_init() {
    s_retrace_count = mkb_VIGetRetraceCount();
    HOOK_TRAMP(s_goal_tramp);
}

// Need to do logic in disp() so that we can know the game state _after_ the frame has processed
void timer_disp() {
    if (mkb_main_mode != mkb_MD_GAME) return;

    switch (mkb_sub_mode) {
    case mkb_SMD_GAME_READY_INIT:
    case mkb_SMD_GAME_READY_MAIN:
    case mkb_SMD_GAME_PLAY_INIT:
    case mkb_SMD_GAME_PLAY_MAIN:
    case mkb_SMD_GAME_GOAL_INIT:
    case mkb_SMD_GAME_GOAL_MAIN:
    case mkb_SMD_GAME_RINGOUT_INIT:
    case mkb_SMD_GAME_RINGOUT_MAIN:
    case mkb_SMD_GAME_TIMEOVER_INIT:
    case mkb_SMD_GAME_TIMEOVER_MAIN:
    case mkb_SMD_GAME_GOAL_REPLAY_INIT:
    case mkb_SMD_GAME_GOAL_REPLAY_MAIN:
        break;
    default:
        s_pause_timer = 0;
        return;
    }

    s_prev_retrace_count = s_retrace_count;
    s_retrace_count = mkb_VIGetRetraceCount();

    if (mkb_sub_mode == mkb_SMD_GAME_READY_INIT) {
        s_rta_timer = mkb_mode_info.stage_time_limit;
        s_pause_timer = 0;
    } else if ((mkb_mode_info.ball_mode & mkb_BALLMODE_FREEZE_TIMER) == 0) {
        s_rta_timer -= s_retrace_count - s_prev_retrace_count;
        //        if (s_rtaTimer < 0) s_rtaTimer = 0;
        if (mkb_g_some_other_flags & mkb_OF_GAME_PAUSED) {
            s_pause_timer++;
        }
    }

    u32 row = 1;

    if (Pref_Get(Pref_TimerShowRTA) && !freecam_should_hide_hud()) {
        timerdisp_draw_timer(380, row++, 44, "RTA:", s_rta_timer, 0, false, true, COLOR_WHITE);
    }

    if (Pref_Get(Pref_TimerShowPause) && !freecam_should_hide_hud()) {
        timerdisp_draw_timer(380, row++, 44, "PAU:", s_pause_timer, 0, false, true, COLOR_WHITE);
    }

    switch (mkb_sub_mode) {
    case mkb_SMD_GAME_GOAL_INIT:
    case mkb_SMD_GAME_GOAL_MAIN:
    case mkb_SMD_GAME_GOAL_REPLAY_INIT:
    case mkb_SMD_GAME_GOAL_REPLAY_MAIN:
        break;
    default:
        return;
    }

    if (Pref_Get(Pref_TimerShowSubtick) && !freecam_should_hide_hud()) {
        timerdisp_draw_subtick_timer(mkb_mode_info.stage_time_frames_remaining, "SUB:", row++,
                                     COLOR_WHITE, true, s_framesave, false);
    }

    if (Pref_Get(Pref_TimerShowUnrounded) && !freecam_should_hide_hud()) {
        timerdisp_draw_subtick_timer(mkb_mode_info.stage_time_frames_remaining, "CUR:", row++,
                                     COLOR_WHITE, true, 0, false);
        timerdisp_draw_subtick_timer(mkb_mode_info.stage_time_frames_remaining + 1, "NXT:", row++,
                                     COLOR_WHITE, true, 0, false);
    }

    if (Pref_Get(Pref_TimerShowFramesave) && !freecam_should_hide_hud()) {
        timerdisp_draw_percentage(s_framesave, "FSV:", row++, COLOR_WHITE);
    }
}

void timer_save_state(Store *store, StoreFunc func) {
    func(store, &s_retrace_count, sizeof(s_retrace_count));
    func(store, &s_prev_retrace_count, sizeof(s_prev_retrace_count));
    func(store, &s_rta_timer, sizeof(s_rta_timer));
    func(store, &s_pause_timer, sizeof(s_pause_timer));
    if (func == store_load) {
        // This might not be completely consistently correct
        u32 count = mkb_VIGetRetraceCount();
        s_prev_retrace_count = count - 1;
        s_retrace_count = count - 1;
    }
}
