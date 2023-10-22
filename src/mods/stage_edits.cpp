#include "stage_edits.h"
#include "systems/pref.h"
#include "utils/patch.h"

namespace stage_edits {

enum class BananaVariant {
    Normal = 0,
    Golden = 1,  // collect all bananas for a perfect
    Dark = 2,    // colect a banana and die!
};

enum class GoalType {
    Any = 0,
    Blue = 1,
    Green = 2,
    Red = 3,
};

static patch::Tramp<decltype(&mkb::load_stagedef)> load_stagedef_tramp;

static u32 count_goals(u32 type) {
    u32 count = 0;
    for (u32 i = 0; i < mkb::stagedef->goal_count; i++) {
        if (mkb::stagedef->goal_list[i].type == type) count++;
    }
    return count;
}

void init() {
    patch::hook_function(load_stagedef_tramp, mkb::load_stagedef, [](u32 stage_id) {
        load_stagedef_tramp.dest(stage_id);
        if (pref::get(pref::BoolPref::ReverseMode) && mkb::stagedef->goal_count > 0) {
            u32 goal_num = mkb::rand() % mkb::stagedef->goal_count;
            u8 type = pref::get(pref::U8Pref::ReverseGoalType);
            if (type != 0) {
                u32 count = count_goals(type - 1);
                if (count > 0) {
                    u32 target = mkb::rand() % count;
                    for (u32 i = 0; i < mkb::stagedef->goal_count; i++) {
                        if (mkb::stagedef->goal_list[i].type != type - 1) continue;
                        if (target == 0) {
                            goal_num = i;
                        } else {
                            target--;
                        }
                    }
                }
            }
            // switch goal and start
            float x = mkb::stagedef->start->position.x;
            float y = mkb::stagedef->start->position.y;
            float z = mkb::stagedef->start->position.z;
            s16 sx = mkb::stagedef->start->rotation.x;
            s16 sy = mkb::stagedef->start->rotation.y;
            s16 sz = mkb::stagedef->start->rotation.z;

            mkb::stagedef->start->position.x = mkb::stagedef->goal_list[goal_num].position.x;
            mkb::stagedef->start->position.y = mkb::stagedef->goal_list[goal_num].position.y + 0.5;
            mkb::stagedef->start->position.z = mkb::stagedef->goal_list[goal_num].position.z;
            mkb::stagedef->start->rotation.x = mkb::stagedef->goal_list[goal_num].rotation.x;
            mkb::stagedef->start->rotation.y =
                mkb::stagedef->goal_list[goal_num].rotation.y - 32766;
            mkb::stagedef->start->rotation.z = mkb::stagedef->goal_list[goal_num].rotation.z;

            mkb::stagedef->goal_list[goal_num].position.x = x;
            mkb::stagedef->goal_list[goal_num].position.y = y - 0.5;
            mkb::stagedef->goal_list[goal_num].position.z = z;
            mkb::stagedef->goal_list[goal_num].rotation.x = sx;
            mkb::stagedef->goal_list[goal_num].rotation.y = sy - 32766;
            mkb::stagedef->goal_list[goal_num].rotation.z = sz;
        } else if (BananaVariant(pref::get(pref::U8Pref::BananaVariant)) == BananaVariant::Golden) {
            // disable goals somehow
            for (u32 i = 0; i < mkb::stagedef->goal_count; i++) {
                mkb::stagedef->goal_list[i].position.y = -10000;
            }
        }
    });
}

void tick() {
    BananaVariant type = BananaVariant(pref::get(pref::U8Pref::BananaVariant));
    switch (type) {
        case BananaVariant::Golden: {
            if (mkb::mode_info.bananas_remaining == 0) {
                mkb::mode_info.g_ball_mode |= 0x228;
            }
            break;
        }
        case BananaVariant::Dark: {
            if (mkb::stagedef != nullptr &&
                mkb::mode_info.bananas_remaining != mkb::stagedef->banana_count) {
                mkb::mode_info.g_ball_mode |= mkb::BALLMODE_FALLEN_OUT;
            }
            break;
        }
        case BananaVariant::Normal: {
            break;
        }
    }
}

}  // namespace stage_edits