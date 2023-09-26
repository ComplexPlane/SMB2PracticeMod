#include "randomizer.h"
#include "mkb/mkb2_ghidra.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"

namespace randomizer {

// static bool s_selected_mode;
// static RandomizerMode s_mode;
// static u8 s_deaths;

// static u8 s_attempts;  // change me with a pref probably

// static patch::Tramp<decltype(&mkb::stobj_bumper_coli)> s_stobj_bumper_coli;
// static patch::Tramp<decltype(&mkb::stobj_button_tick)> stobj_button_tick;

void init() {
    // patch::hook_function(s_stobj_bumper_coli, mkb::stobj_bumper_coli,
    //                      [](mkb::Stobj* stobj, mkb::PhysicsBall* physball) {});
    // patch::hook_function(stobj_button_tick, mkb::stobj_button_tick, [](mkb::Stobj* stdobj) {});
}

char* mode_to_text(RandomizerMode mode) {
    switch (mode) {
        case RandomizerMode::AnimationsPause: {
            return "Frozen Animation";
        }
        case RandomizerMode::AnimationsFastForward: {
            return "Fast-Forward Animations";
        }
        case RandomizerMode::AnimationsFastReverse: {
            return "Fast-Reversed Animations";
        }
        case RandomizerMode::JumpMod: {
            return "Jump Mod";
        }
        case RandomizerMode::MoonGravity: {
            return "Moon Gravity";
        }
        case RandomizerMode::NoFriction: {
            return "No Friction";
        }
        default: {
            return "";
        }
    }
}

void enable_mode(RandomizerMode mode) {
    switch (mode) {
        case RandomizerMode::JumpMod: {
            pref::set(pref::BoolPref::JumpMod, true);
            break;
        }
        case RandomizerMode::MoonGravity: {
            pref::set(pref::BoolPref::Moon, true);
            break;
        }
        case RandomizerMode::NoFriction: {
            pref::set(pref::BoolPref::UseCustomPhysics, true);
            pref::set(pref::U8Pref::Friction, 100);
            break;
        }
        default: {
            break;
        }
    }
}

void disable_mode(RandomizerMode mode) {
    switch (mode) {
        case RandomizerMode::JumpMod: {
            pref::set(pref::BoolPref::JumpMod, pref::get_default(pref::BoolPref::JumpMod));
            break;
        }
        case RandomizerMode::MoonGravity: {
            pref::set(pref::BoolPref::Moon, pref::get_default(pref::BoolPref::Moon));
            break;
        }
        case RandomizerMode::NoFriction: {
            pref::set(pref::BoolPref::UseCustomPhysics,
                      pref::get_default(pref::BoolPref::UseCustomPhysics));
            pref::set(pref::U8Pref::Friction, pref::get_default(pref::U8Pref::Friction));
            break;
        }
        default: {
            break;
        }
    }
}

void tick_mode(RandomizerMode mode) {
    switch (mode) {
        case RandomizerMode::AnimationsPause: {
            if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT ||
                mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
                for (u32 i = 0; i < mkb::stagedef->coli_header_count; i++) {
                    mkb::itemgroups[i].anim_frame = 0;
                }
            }
            break;
        }
        case RandomizerMode::AnimationsFastForward: {
            if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT ||
                mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
                for (u32 i = 0; i < mkb::stagedef->coli_header_count; i++) {
                    mkb::itemgroups[i].playback_state = 3;
                }
            }
            break;
        }
        case RandomizerMode::AnimationsFastReverse: {
            if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT ||
                mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
                for (u32 i = 0; i < mkb::stagedef->coli_header_count; i++) {
                    mkb::itemgroups[i].playback_state = 4;
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void tick() {
    // if (!pref::get(pref::BoolPref::Randomizer) || mkb::main_mode != mkb::MD_GAME) {
    //     disable_mode(s_mode);
    //     return;
    // }
    // bool dpad_r = pad::button_pressed(mkb::PAD_BUTTON_RIGHT);

    // if (dpad_r) {
    //     s_selected_mode = true;
    //     s_deaths = 0;
    //     disable_mode(s_mode);
    //     s_mode = RandomizerMode(((int)s_mode + 1) % MODE_COUNT);
    //     enable_mode(s_mode);
    // } else if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT) {
    //     s_deaths++;
    // } else if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT && !s_selected_mode) {
    //     s_selected_mode = true;
    //     s_deaths = 0;
    //     disable_mode(s_mode);
    //     s_mode = RandomizerMode(mkb::rand() % MODE_COUNT);
    //     enable_mode(s_mode);
    // } else if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || s_deaths > s_attempts - 1) {
    //     s_selected_mode = false;
    //     s_deaths = 0;
    // }

    // tick_mode(s_mode);
}

void disp() {
    // if (!pref::get(pref::BoolPref::Randomizer) || mkb::main_mode != mkb::MD_GAME) {
    //     return;
    // }
    // draw::debug_text(160 - 12 * 12, 48, draw::WHITE, "%s", mode_to_text(s_mode));
}

}  // namespace randomizer