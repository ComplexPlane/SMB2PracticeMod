#include "validate.h"

#include "mods/physics.h"
#include "systems/menu_impl.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/libsavest.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

namespace validate {

// calculate from did_ball_enter_goal for subtick timer
static u32 s_framesave;
// values for validation
static bool s_entered_goal;
static bool s_used_mods;
static bool s_has_paused;
static bool s_loaded_savestate;

static patch::Tramp<decltype(&mkb::did_ball_enter_goal)> s_goal_tramp;

static constexpr pref::BoolPref INVALID_BOOL_PREFS[] = {
    pref::BoolPref::DisableFalloutVolumes,
    pref::BoolPref::JumpMod,
    pref::BoolPref::Marathon,
    pref::BoolPref::DebugMode,
};

static constexpr pref::U8Pref INVALID_U8_PREFS[] = {
    pref::U8Pref::PhysicsPreset,
    pref::U8Pref::TimerType,
    pref::U8Pref::FalloutPlaneType,
    pref::U8Pref::StageEditVariant,
};

void disable_invalidating_settings() {
    // set all bool prefs to default
    for (u8 i = 0; i < LEN(INVALID_BOOL_PREFS); i++) {
        pref::set(INVALID_BOOL_PREFS[i], pref::get_default(INVALID_BOOL_PREFS[i]));
    }
    // set all u8 prefs to default
    for (u8 i = 0; i < LEN(INVALID_U8_PREFS); i++) {
        pref::set(INVALID_U8_PREFS[i], pref::get_default(INVALID_U8_PREFS[i]));
    }

    pref::save();
}

void validate_run() {
    // Track pauses
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    if (paused_now && !s_entered_goal) {
        s_has_paused = true;
    }

    // Track savestates
    if (libsavest::state_loaded_this_frame()) s_loaded_savestate = true;

    // Using dpad controls is disallowed
    bool dpad_down =
        pad::button_down(mkb::PAD_BUTTON_DOWN) || pad::button_down(mkb::PAD_BUTTON_LEFT) ||
        pad::button_down(mkb::PAD_BUTTON_RIGHT) || pad::button_down(mkb::PAD_BUTTON_UP);
    if (pref::get(pref::BoolPref::DpadControls) && dpad_down) s_used_mods = true;

    // Opening the mod menu is disallowed
    if (menu_impl::is_visible()) s_used_mods = true;

    // Physics must be default or custom (with all default values)
    if (physics::using_custom_physics()) s_used_mods = true;

    // Invalid bool prefs are enabled
    for (u8 i = 0; i < LEN(INVALID_BOOL_PREFS); i++) {
        if (pref::get(INVALID_BOOL_PREFS[i]) != pref::get_default(INVALID_BOOL_PREFS[i])) {
            s_used_mods = true;
        }
    }
    // Invalid u8 prefs are enabled
    for (u8 i = 0; i < LEN(INVALID_U8_PREFS); i++) {
        if (pref::get(INVALID_U8_PREFS[i]) != pref::get_default(INVALID_U8_PREFS[i])) {
            s_used_mods = true;
        }
    }
}

// Copy-paste of line_intersects from ghidra with slight changes to calculate s_framesave
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

// Copy paste from ghidra of did_ball_enter_goal with slight changes to calculate s_framesave
void find_framesave(mkb::Ball* ball, int* out_stage_goal_idx, int* out_itemgroup_id,
                    mkb::byte* out_goal_flags) {
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
    patch::hook_function(
        s_goal_tramp, &mkb::did_ball_enter_goal,
        [](mkb::Ball* ball, int* out_stage_goal_idx, int* out_itemgroup_id,
           mkb::byte* out_goal_flags) {
            bool result =
                s_goal_tramp.dest(ball, out_stage_goal_idx, out_itemgroup_id, out_goal_flags);
            if (result) {
                // determine framesave percentage
                find_framesave(ball, out_stage_goal_idx, out_itemgroup_id, out_goal_flags);
                s_entered_goal = result;
            }
            return result;
        });
}

void tick() {
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT) {
        s_entered_goal = false;
        s_used_mods = false;
        s_has_paused = false;
        s_loaded_savestate = false;
    }
}

bool was_run_valid(bool mods_allowed) {
    return (!s_used_mods || mods_allowed) && !s_has_paused && !s_loaded_savestate && s_entered_goal;
}

u32 get_framesave() { return s_framesave; }

}  // namespace validate