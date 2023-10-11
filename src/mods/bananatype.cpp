#include "bananatype.h"
#include "systems/pref.h"
#include "utils/patch.h"

namespace bananatype {

enum class BananaType {
    Normal = 0,
    Golden = 1,  // collect all bananas for a perfect
    Dark = 2,    // colect a banana and die!
};

static patch::Tramp<decltype(&mkb::load_stagedef)> load_stagedef_tramp;

void init() {
    patch::hook_function(load_stagedef_tramp, mkb::load_stagedef, [](u32 stage_id) {
        load_stagedef_tramp.dest(stage_id);
        if (pref::get(pref::BoolPref::ReverseMode) && mkb::stagedef->goal_count > 0) {
            // switch goal and start
            float x = mkb::stagedef->start->position.x;
            float y = mkb::stagedef->start->position.y;
            float z = mkb::stagedef->start->position.z;
            s16 sx = mkb::stagedef->start->rotation.x;
            s16 sy = mkb::stagedef->start->rotation.y;
            s16 sz = mkb::stagedef->start->rotation.z;

            mkb::stagedef->start->position.x = mkb::stagedef->goal_list[0].position.x;
            mkb::stagedef->start->position.y = mkb::stagedef->goal_list[0].position.y + 0.5;
            mkb::stagedef->start->position.z = mkb::stagedef->goal_list[0].position.z;
            mkb::stagedef->start->rotation.x = mkb::stagedef->goal_list[0].rotation.x;
            mkb::stagedef->start->rotation.y = mkb::stagedef->goal_list[0].rotation.y - 32766;
            mkb::stagedef->start->rotation.z = mkb::stagedef->goal_list[0].rotation.z;

            mkb::stagedef->goal_list[0].position.x = x;
            mkb::stagedef->goal_list[0].position.y = y - 0.5;
            mkb::stagedef->goal_list[0].position.z = z;
            mkb::stagedef->goal_list[0].rotation.x = sx;
            mkb::stagedef->goal_list[0].rotation.y = sy - 32766;
            mkb::stagedef->goal_list[0].rotation.z = sz;
        } else if (BananaType(pref::get(pref::U8Pref::BananaType)) == BananaType::Golden) {
            // disable goal somehow
            for (u32 i = 0; i < mkb::stagedef->goal_count; i++) {
                mkb::stagedef->goal_list[i].position.y = -1000000;
            }
        }
    });
}

void tick() {
    BananaType type = BananaType(pref::get(pref::U8Pref::BananaType));
    switch (type) {
        case BananaType::Golden: {
            if (mkb::stagedef != nullptr) {
            }
            if (mkb::mode_info.bananas_remaining == 0) {
                mkb::mode_info.g_ball_mode |= 0x228;
            }
            break;
        }
        case BananaType::Dark: {
            if (mkb::stagedef != nullptr &&
                mkb::mode_info.bananas_remaining != mkb::stagedef->banana_count) {
                mkb::mode_info.g_ball_mode |= mkb::BALLMODE_FALLEN_OUT;
            }
            break;
        }
        case BananaType::Normal: {
            break;
        }
    }
}

}  // namespace bananatype