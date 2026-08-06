#include "mods/stage_edits.h"
#include "systems/pref.h"
#include "utils/patch.h"

typedef enum ActiveMode ActiveMode;
enum ActiveMode {
    ActiveMode_None = 0,
    ActiveMode_Golden = 1,
    ActiveMode_Dark = 2,
    ActiveMode_Reverse = 3,
};

static ActiveMode s_current_mode = ActiveMode_None;
static u32 s_rev_goal_idx = 0;
static bool s_new_goal = false;

static void ready_init_hook();
static void load_stagedef_hook(u32 stage_id);
static TRAMP(s_ready_init_tramp, mkb_smd_game_ready_init, ready_init_hook);
static TRAMP(s_load_stagedef_tramp, mkb_load_stagedef, load_stagedef_hook);

void stage_edits_select_new_goal() {
    s_new_goal = true;
}

static void undo_mode(ActiveMode mode) {
    switch (mode) {
    case ActiveMode_None: {
        break;
    }
    case ActiveMode_Golden: {
        // disable goals somehow
        for (u32 i = 0; i < mkb_stagedef->goal_count; i++) {
            mkb_stagedef->goal_list[i].position.y += 10000;
        }
        break;
    }
    case ActiveMode_Dark: {
        break;
    }
    case ActiveMode_Reverse: {
        if (mkb_stagedef->goal_count < 1) return;
        float x = mkb_stagedef->start->position.x;
        float y = mkb_stagedef->start->position.y;
        float z = mkb_stagedef->start->position.z;
        s16 sx = mkb_stagedef->start->rotation.x;
        s16 sy = mkb_stagedef->start->rotation.y;
        s16 sz = mkb_stagedef->start->rotation.z;

        mkb_stagedef->start->position.x = mkb_stagedef->goal_list[s_rev_goal_idx].position.x;
        mkb_stagedef->start->position.y = mkb_stagedef->goal_list[s_rev_goal_idx].position.y + 0.5;
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
    case ActiveMode_None: {
        break;
    }
    case ActiveMode_Golden: {
        // disable goals somehow
        for (u32 i = 0; i < mkb_stagedef->goal_count; i++) {
            mkb_stagedef->goal_list[i].position.y -= 10000;
        }
        break;
    }
    case ActiveMode_Dark: {
        break;
    }
    case ActiveMode_Reverse: {
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
        mkb_stagedef->start->position.y = mkb_stagedef->goal_list[s_rev_goal_idx].position.y + 0.5;
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

static void ready_init_hook() {
    ActiveMode next_mode = (ActiveMode)(pref_get(Pref_StageEditVariant));
    if (s_current_mode != next_mode) {
        undo_mode(s_current_mode);
        s_current_mode = (ActiveMode)(pref_get(Pref_StageEditVariant));
        set_mode(s_current_mode);
    } else if (s_current_mode == ActiveMode_Reverse && s_new_goal) {
        undo_mode(ActiveMode_Reverse);
        s_rev_goal_idx++;
        set_mode(ActiveMode_Reverse);
    }
    s_new_goal = false;
    s_ready_init_tramp.chain();
}

void stage_edits_main_game_init() {
    HOOK_TRAMP(s_ready_init_tramp);
}

void stage_edits_init() {
    HOOK_TRAMP(s_load_stagedef_tramp);
}

static void load_stagedef_hook(u32 stage_id) {
    s_load_stagedef_tramp.chain(stage_id);
    s_current_mode = (ActiveMode)pref_get(Pref_StageEditVariant);
    set_mode(s_current_mode);
}

void stage_edits_tick() {
    switch (s_current_mode) {
    case ActiveMode_None: {
        break;
    }
    case ActiveMode_Golden: {
        if (mkb_mode_info.bananas_remaining == 0) {
            mkb_mode_info.ball_mode |= 0x228;
        }
        break;
    }
    case ActiveMode_Dark: {
        if (mkb_stagedef != 0 && mkb_mode_info.bananas_remaining != mkb_stagedef->banana_count) {
            mkb_mode_info.ball_mode |= mkb_BALLMODE_FALLEN_OUT;
        }
        break;
    }
    case ActiveMode_Reverse: {
        break;
    }
    }
}
