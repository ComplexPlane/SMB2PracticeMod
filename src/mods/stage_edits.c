#include "mods/stage_edits.h"

#include "systems/pref.h"
#include "utils/patch.h"

static ActiveMode s_current_mode = StageEdits_Mode_None;
static u32 s_rev_goal_idx = 0;
static bool s_new_goal = false;

void StageEdits_SelectNewGoal() {
    s_new_goal = true;
}

static void undo_mode(ActiveMode mode) {
    switch (mode) {
        case StageEdits_Mode_None: {
            break;
        }
        case StageEdits_Mode_Golden: {
            // disable goals somehow
            for (u32 i = 0; i < mkb_stagedef->goal_count; i++) {
                mkb_stagedef->goal_list[i].position.y += 10000;
            }
            break;
        }
        case StageEdits_Mode_Dark: {
            break;
        }
        case StageEdits_Mode_Reverse: {
            if (mkb_stagedef->goal_count < 1) return;
            float x = mkb_stagedef->start->position.x;
            float y = mkb_stagedef->start->position.y;
            float z = mkb_stagedef->start->position.z;
            s16 sx = mkb_stagedef->start->rotation.x;
            s16 sy = mkb_stagedef->start->rotation.y;
            s16 sz = mkb_stagedef->start->rotation.z;

            mkb_stagedef->start->position.x = mkb_stagedef->goal_list[s_rev_goal_idx].position.x;
            mkb_stagedef->start->position.y =
                mkb_stagedef->goal_list[s_rev_goal_idx].position.y + 0.5;
            mkb_stagedef->start->position.z = mkb_stagedef->goal_list[s_rev_goal_idx].position.z;
            mkb_stagedef->start->rotation.x = mkb_stagedef->goal_list[s_rev_goal_idx].rotation.x;
            mkb_stagedef->start->rotation.y =
                mkb_stagedef->goal_list[s_rev_goal_idx].rotation.y + 32766;
            mkb_stagedef->start->rotation.z = mkb_stagedef->goal_list[s_rev_goal_idx].rotation.z;

            mkb_stagedef->goal_list[s_rev_goal_idx].position.x = x;
            mkb_stagedef->goal_list[s_rev_goal_idx].position.y = y - 0.5;
            mkb_stagedef->goal_list[s_rev_goal_idx].position.z = z;
            mkb_stagedef->goal_list[s_rev_goal_idx].rotation.x = sx;
            mkb_stagedef->goal_list[s_rev_goal_idx].rotation.y = sy - 32766;
            mkb_stagedef->goal_list[s_rev_goal_idx].rotation.z = sz;
            break;
        }
    }
}

static void set_mode(ActiveMode mode) {
    switch (mode) {
        case StageEdits_Mode_None: {
            break;
        }
        case StageEdits_Mode_Golden: {
            // disable goals somehow
            for (u32 i = 0; i < mkb_stagedef->goal_count; i++) {
                mkb_stagedef->goal_list[i].position.y -= 10000;
            }
            break;
        }
        case StageEdits_Mode_Dark: {
            break;
        }
        case StageEdits_Mode_Reverse: {
            if (mkb_stagedef->goal_count < 1) return;
            s_rev_goal_idx %= mkb_stagedef->goal_count;
            // switch goal and start
            float x = mkb_stagedef->start->position.x;
            float y = mkb_stagedef->start->position.y;
            float z = mkb_stagedef->start->position.z;
            s16 sx = mkb_stagedef->start->rotation.x;
            s16 sy = mkb_stagedef->start->rotation.y;
            s16 sz = mkb_stagedef->start->rotation.z;

            mkb_stagedef->start->position.x = mkb_stagedef->goal_list[s_rev_goal_idx].position.x;
            mkb_stagedef->start->position.y =
                mkb_stagedef->goal_list[s_rev_goal_idx].position.y + 0.5;
            mkb_stagedef->start->position.z = mkb_stagedef->goal_list[s_rev_goal_idx].position.z;
            mkb_stagedef->start->rotation.x = mkb_stagedef->goal_list[s_rev_goal_idx].rotation.x;
            mkb_stagedef->start->rotation.y =
                mkb_stagedef->goal_list[s_rev_goal_idx].rotation.y + 32766;
            mkb_stagedef->start->rotation.z = mkb_stagedef->goal_list[s_rev_goal_idx].rotation.z;

            mkb_stagedef->goal_list[s_rev_goal_idx].position.x = x;
            mkb_stagedef->goal_list[s_rev_goal_idx].position.y = y - 0.5;
            mkb_stagedef->goal_list[s_rev_goal_idx].position.z = z;
            mkb_stagedef->goal_list[s_rev_goal_idx].rotation.x = sx;
            mkb_stagedef->goal_list[s_rev_goal_idx].rotation.y = sy - 32766;
            mkb_stagedef->goal_list[s_rev_goal_idx].rotation.z = sz;
            break;
        }
    }
}

void StageEdits_SmdGameReadyInit() {
    ActiveMode next_mode = (ActiveMode)Pref_Get(Pref_StageEditVariant);
    if (s_current_mode != next_mode) {
        undo_mode(s_current_mode);
        s_current_mode = (ActiveMode)Pref_Get(Pref_StageEditVariant);
        set_mode(s_current_mode);
    } else if (s_current_mode == StageEdits_Mode_Reverse && s_new_goal) {
        undo_mode(StageEdits_Mode_Reverse);
        s_rev_goal_idx++;
        set_mode(StageEdits_Mode_Reverse);
    }
    s_new_goal = false;
}

TRAMP(s_load_stagedef_tramp, mkb_load_stagedef);

static void load_stagedef_hook(u32 stage_id) {
    s_load_stagedef_tramp.chain(stage_id);
    s_current_mode = (ActiveMode)Pref_Get(Pref_StageEditVariant);
    set_mode(s_current_mode);
}

void StageEdits_Init() {
    HOOK_TRAMP(s_load_stagedef_tramp, load_stagedef_hook);
}

void StageEdits_Tick() {
    switch (s_current_mode) {
        case StageEdits_Mode_None: {
            break;
        }
        case StageEdits_Mode_Golden: {
            if (mkb_mode_info.bananas_remaining == 0) {
                mkb_mode_info.ball_mode |= 0x228;
            }
            break;
        }
        case StageEdits_Mode_Dark: {
            if (mkb_stagedef != nullptr &&
                mkb_mode_info.bananas_remaining != mkb_stagedef->banana_count) {
                mkb_mode_info.ball_mode |= mkb_BALLMODE_FALLEN_OUT;
            }
            break;
        }
        case StageEdits_Mode_Reverse: {
            break;
        }
    }
}
