#include "stage_edits.h"
#include "systems/pref.h"
#include "utils/patch.h"

namespace stage_edits {

enum class ActiveMode {
    None = 0,
    Golden = 1,
    Dark = 2,
    Reverse = 3,
};

static ActiveMode s_current_mode = ActiveMode::None;
static u32 s_rev_goal_idx = 0;
static bool s_new_goal = false;

static patch::Tramp<decltype(&mkb::smd_game_ready_init)> s_ready_init_tramp;
static patch::Tramp<decltype(&mkb::load_stagedef)> s_load_stagedef_tramp;

void select_new_goal() { s_new_goal = true; }

static void undo_mode(ActiveMode mode) {
    switch (mode) {
        case ActiveMode::None: {
            break;
        }
        case ActiveMode::Golden: {
            // disable goals somehow
            for (u32 i = 0; i < mkb::stagedef->goal_count; i++) {
                mkb::stagedef->goal_list[i].position.y += 10000;
            }
            break;
        }
        case ActiveMode::Dark: {
            break;
        }
        case ActiveMode::Reverse: {
            if (mkb::stagedef->goal_count < 1) return;
            float x = mkb::stagedef->start->position.x;
            float y = mkb::stagedef->start->position.y;
            float z = mkb::stagedef->start->position.z;
            s16 sx = mkb::stagedef->start->rotation.x;
            s16 sy = mkb::stagedef->start->rotation.y;
            s16 sz = mkb::stagedef->start->rotation.z;

            mkb::stagedef->start->position.x = mkb::stagedef->goal_list[s_rev_goal_idx].position.x;
            mkb::stagedef->start->position.y =
                mkb::stagedef->goal_list[s_rev_goal_idx].position.y + 0.5;
            mkb::stagedef->start->position.z = mkb::stagedef->goal_list[s_rev_goal_idx].position.z;
            mkb::stagedef->start->rotation.x = mkb::stagedef->goal_list[s_rev_goal_idx].rotation.x;
            mkb::stagedef->start->rotation.y =
                mkb::stagedef->goal_list[s_rev_goal_idx].rotation.y + 32766;
            mkb::stagedef->start->rotation.z = mkb::stagedef->goal_list[s_rev_goal_idx].rotation.z;

            mkb::stagedef->goal_list[s_rev_goal_idx].position.x = x;
            mkb::stagedef->goal_list[s_rev_goal_idx].position.y = y - 0.5;
            mkb::stagedef->goal_list[s_rev_goal_idx].position.z = z;
            mkb::stagedef->goal_list[s_rev_goal_idx].rotation.x = sx;
            mkb::stagedef->goal_list[s_rev_goal_idx].rotation.y = sy - 32766;
            mkb::stagedef->goal_list[s_rev_goal_idx].rotation.z = sz;
            break;
        }
    }
}

static void set_mode(ActiveMode mode) {
    switch (mode) {
        case ActiveMode::None: {
            break;
        }
        case ActiveMode::Golden: {
            // disable goals somehow
            for (u32 i = 0; i < mkb::stagedef->goal_count; i++) {
                mkb::stagedef->goal_list[i].position.y -= 10000;
            }
            break;
        }
        case ActiveMode::Dark: {
            break;
        }
        case ActiveMode::Reverse: {
            if (mkb::stagedef->goal_count < 1) return;
            s_rev_goal_idx %= mkb::stagedef->goal_count;
            // switch goal and start
            float x = mkb::stagedef->start->position.x;
            float y = mkb::stagedef->start->position.y;
            float z = mkb::stagedef->start->position.z;
            s16 sx = mkb::stagedef->start->rotation.x;
            s16 sy = mkb::stagedef->start->rotation.y;
            s16 sz = mkb::stagedef->start->rotation.z;

            mkb::stagedef->start->position.x = mkb::stagedef->goal_list[s_rev_goal_idx].position.x;
            mkb::stagedef->start->position.y =
                mkb::stagedef->goal_list[s_rev_goal_idx].position.y + 0.5;
            mkb::stagedef->start->position.z = mkb::stagedef->goal_list[s_rev_goal_idx].position.z;
            mkb::stagedef->start->rotation.x = mkb::stagedef->goal_list[s_rev_goal_idx].rotation.x;
            mkb::stagedef->start->rotation.y =
                mkb::stagedef->goal_list[s_rev_goal_idx].rotation.y + 32766;
            mkb::stagedef->start->rotation.z = mkb::stagedef->goal_list[s_rev_goal_idx].rotation.z;

            mkb::stagedef->goal_list[s_rev_goal_idx].position.x = x;
            mkb::stagedef->goal_list[s_rev_goal_idx].position.y = y - 0.5;
            mkb::stagedef->goal_list[s_rev_goal_idx].position.z = z;
            mkb::stagedef->goal_list[s_rev_goal_idx].rotation.x = sx;
            mkb::stagedef->goal_list[s_rev_goal_idx].rotation.y = sy - 32766;
            mkb::stagedef->goal_list[s_rev_goal_idx].rotation.z = sz;
            break;
        }
    }
}

void main_game_init() {
    patch::hook_function(s_ready_init_tramp, mkb::smd_game_ready_init, []() {
        ActiveMode next_mode = ActiveMode(pref::get(pref::U8Pref::StageEditVariant));
        if (s_current_mode != next_mode) {
            undo_mode(s_current_mode);
            s_current_mode = ActiveMode(pref::get(pref::U8Pref::StageEditVariant));
            set_mode(s_current_mode);
        } else if (s_current_mode == ActiveMode::Reverse && s_new_goal) {
            undo_mode(ActiveMode::Reverse);
            s_rev_goal_idx++;
            set_mode(ActiveMode::Reverse);
        }
        s_new_goal = false;
        s_ready_init_tramp.dest();
    });
}

void init() {
    patch::hook_function(s_load_stagedef_tramp, mkb::load_stagedef, [](u32 stage_id) {
        s_load_stagedef_tramp.dest(stage_id);
        s_current_mode = ActiveMode(pref::get(pref::U8Pref::StageEditVariant));
        set_mode(s_current_mode);
    });
}

void tick() {
    switch (s_current_mode) {
        case ActiveMode::None: {
            break;
        }
        case ActiveMode::Golden: {
            if (mkb::mode_info.bananas_remaining == 0) {
                mkb::mode_info.g_ball_mode |= 0x228;
            }
            break;
        }
        case ActiveMode::Dark: {
            if (mkb::stagedef != nullptr &&
                mkb::mode_info.bananas_remaining != mkb::stagedef->banana_count) {
                mkb::mode_info.g_ball_mode |= mkb::BALLMODE_FALLEN_OUT;
            }
            break;
        }
        case ActiveMode::Reverse: {
            break;
        }
    }
}

}  // namespace stage_edits
