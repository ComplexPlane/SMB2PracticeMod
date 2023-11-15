#include "jump.h"

#include "mkb/mkb.h"
#include "mods/physics.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

namespace jump {

enum class MaxJumpCount {
    One = 0,
    Two = 1,
    Infinite = 2,
};

enum class JumpState {
    NotJumping = 0,
    GroundedJump = 1,
    AerialJump = 2,
};

static constexpr s32 JUMP_LENGTH = 15;
static constexpr s32 EARLY_BUFFER_LENGTH = 4;
static constexpr s32 LATE_BUFFER_LENGTH = 5;
static constexpr f32 WALLJUMP_NORMAL = -0.5;

static constexpr s32 CLASSIC_EARLY_BUFFER_LENGTH = 4;
static constexpr s32 CLASSIC_LATE_BUFFER_LENGTH = 8;

static u32 s_patch1;
static u32 s_patch2;

static JumpState s_jumping = JumpState::NotJumping;
static u32 s_jump_frames = 0;
static u32 s_ticks_since_jump_input = 0;
static u32 s_ticks_since_ground = 0;
static s32 s_aerial_jumps = 0;

static void reset() {
    s_ticks_since_jump_input = EARLY_BUFFER_LENGTH + 1;
    s_ticks_since_ground = LATE_BUFFER_LENGTH + 1;
    s_jumping = JumpState::NotJumping;
    s_jump_frames = 0;
    s_aerial_jumps = 0;
}

void patch_minimap() {
    // Patch out Minimap Toggle
    // Function is ran whenever minimap is enabled or whenever main_game.rel is loaded
    if (mkb::main_mode == mkb::MD_GAME && pref::get(pref::BoolPref::JumpMod)) {
        u32* patch1_loc = reinterpret_cast<u32*>(0x808f4d18);
        u32* patch2_loc = reinterpret_cast<u32*>(0x808f5168);

        // Patch instructions if they aren't nop
        if (*patch1_loc != 0x60000000) {
            s_patch1 = patch::write_nop(reinterpret_cast<void*>(0x808f4d18));
        }
        if (*patch2_loc != 0x60000000) {
            s_patch2 = patch::write_nop(reinterpret_cast<void*>(0x808f5168));
        }
    }
}

static void restore_minimap() {
    if (mkb::main_mode == mkb::MD_GAME) {
        // These overwrites exist in main_game.rel which isn't always loaded
        patch::write_word(reinterpret_cast<void*>(0x808f4d18), s_patch1);
        patch::write_word(reinterpret_cast<void*>(0x808f5168), s_patch2);
    }
}

static void enable() {
    patch_minimap();
    if (pref::get(pref::BoolPref::JumpChangePhysics)) {
        pref::set(pref::U8Pref::PhysicsPreset,
                  static_cast<u8>(physics::PhysicsPreset::JumpPhysics));
        pref::save();
    }
    reset();
}

static void disable() {
    restore_minimap();
    if (pref::get(pref::BoolPref::JumpChangePhysics)) {
        pref::set(pref::U8Pref::PhysicsPreset, static_cast<u8>(physics::PhysicsPreset::Default));
        pref::save();
    }
}

static void end_jump() {
    s_jumping = JumpState::NotJumping;
    s_jump_frames = 0;
}

static f32 jump_curve(s32 current, s32 max) {
    f32 lerp = static_cast<f32>(max - current) / max;
    return lerp * lerp * lerp;
}

static void toggle_minimap() {
    // Minimap Toggle with B
    if (mkb::sub_mode == mkb::SMD_GAME_READY_MAIN || mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN) {
        if (pad::button_pressed(mkb::PAD_BUTTON_B)) {
            mkb::toggle_minimap_zoom();
        }
    }
}

static void jumping() {
    // Reset state on READY_INIT
    if (mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN && mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT) {
        reset();
        return;
    }

    // Setup vars
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx];
    bool a_pressed = pad::button_pressed(mkb::PAD_BUTTON_A);
    bool a_down = pad::button_down(mkb::PAD_BUTTON_A);
    bool a_released = pad::button_released(mkb::PAD_BUTTON_A);
    bool ground_touched = (ball.phys_flags & mkb::PHYS_ON_GROUND);
    Vec normal_vec = mkb::balls[mkb::curr_player_idx].g_last_collision_normal;

    // Track Jump Presses
    if (a_pressed) {
        s_ticks_since_jump_input = 0;
    } else {
        s_ticks_since_jump_input++;
    }

    bool valid_location =
        normal_vec.y < WALLJUMP_NORMAL || pref::get(pref::BoolPref::JumpAllowWalljumps);
    // Track Ground Touched
    if (ground_touched && valid_location) {
        s_ticks_since_ground = 0;

        MaxJumpCount count = MaxJumpCount(pref::get(pref::U8Pref::JumpCount));
        if (count == MaxJumpCount::Two) {
            s_aerial_jumps = 1;
        } else {
            s_aerial_jumps = 0;
        }
    } else {
        s_ticks_since_ground++;
    }

    // check if jump was buffered before touching ground, or pressed during "coyote time" after
    // leaving ground
    bool buffered_early =
        ground_touched && s_ticks_since_jump_input < EARLY_BUFFER_LENGTH && a_down;
    bool coyote_late = s_ticks_since_ground < LATE_BUFFER_LENGTH && a_pressed;
    // check extra jump count
    bool aerial_jumped = (s_aerial_jumps > 0 || MaxJumpCount(pref::get(pref::U8Pref::JumpCount)) ==
                                                    MaxJumpCount::Infinite) &&
                         a_pressed;
    bool start_jump = mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT &&
                      s_ticks_since_jump_input < EARLY_BUFFER_LENGTH && a_down;

    if (start_jump) {
        s_jumping = JumpState::GroundedJump;
        s_ticks_since_ground += LATE_BUFFER_LENGTH;  // cannot coyote jump anymore
    } else if ((buffered_early || coyote_late) && valid_location) {
        s_jumping = JumpState::GroundedJump;
        s_ticks_since_ground += LATE_BUFFER_LENGTH;  // cannot coyote jump anymore
    } else if (aerial_jumped) {
        s_jumping = JumpState::AerialJump;
        s_aerial_jumps--;
    }

    // end jump
    if (a_released) {
        end_jump();
        return;
    }

    // jump!
    if (s_jumping == JumpState::GroundedJump || s_jumping == JumpState::AerialJump) {
        // first frame of jump
        if (s_jump_frames == 0) {
            mkb::call_SoundReqID_arg_0(268);
            if (ball.vel.y < 0) {
                ball.vel.y = 0;
            }
        }
        // tick jump frames
        if (s_jump_frames > JUMP_LENGTH) {
            end_jump();
            return;
        } else {
            s_jump_frames++;
        }

        if (s_jumping == JumpState::GroundedJump) {
            ball.vel.x += jump_curve(s_jump_frames, JUMP_LENGTH) * (0.05 * -normal_vec.x);
            ball.vel.z += jump_curve(s_jump_frames, JUMP_LENGTH) * (0.05 * -normal_vec.z);
            f32 expected_height = (0.1 * -normal_vec.y);
            f32 bonus_height = (1.0 - ABS(normal_vec.y)) * 0.08;
            f32 lerped_height =
                jump_curve(s_jump_frames, JUMP_LENGTH) * (expected_height + bonus_height);
            ball.vel.y += lerped_height;
        } else {
            ball.vel.y += jump_curve(s_jump_frames, JUMP_LENGTH) * 0.09;
        }
    }
}

static void classic_jumping() {
    if (mkb::sub_mode != mkb::SMD_GAME_READY_MAIN && mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT &&
        mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN) {
        s_ticks_since_jump_input = CLASSIC_EARLY_BUFFER_LENGTH + 1;
        s_ticks_since_ground = CLASSIC_LATE_BUFFER_LENGTH + 1;
        s_jumping = JumpState::NotJumping;
        s_jump_frames = 0;
        return;
    }

    // Setup vars
    mkb::Ball& ball = mkb::balls[mkb::curr_player_idx];
    bool a_pressed = pad::button_pressed(mkb::PAD_BUTTON_A);
    bool a_down = pad::button_down(mkb::PAD_BUTTON_A);
    bool a_released = pad::button_released(mkb::PAD_BUTTON_A);
    bool ground_touched = (ball.phys_flags & mkb::PHYS_ON_GROUND);

    if (a_pressed) {
        s_ticks_since_jump_input = 0;
    } else {
        s_ticks_since_jump_input++;
    }
    if (ground_touched) {
        s_ticks_since_ground = 0;
    } else {
        s_ticks_since_ground++;
    }

    bool before = ground_touched && s_ticks_since_jump_input < CLASSIC_EARLY_BUFFER_LENGTH;
    bool after = a_pressed && s_ticks_since_ground < CLASSIC_LATE_BUFFER_LENGTH;
    bool go_buffered_press = mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT && a_down;

    if (before || after || go_buffered_press) s_jumping = JumpState::GroundedJump;

    if (a_released) {
        s_jumping = JumpState::NotJumping;
        s_jump_frames = 0;
    }

    if (s_jumping == JumpState::GroundedJump) {
        if (s_jump_frames == 0) {
            mkb::call_SoundReqID_arg_0(268);
        }

        s_jump_frames++;
        if (s_jump_frames > JUMP_LENGTH) {
            s_jumping = JumpState::NotJumping;
            s_jump_frames = 0;
            return;
        }

        f32 lerp = static_cast<f32>(JUMP_LENGTH - s_jump_frames) / JUMP_LENGTH;
        lerp = lerp * lerp * lerp;
        ball.vel.y += lerp * 0.1;
    }
}

void tick() {
    bool enabled = pref::get(pref::BoolPref::JumpMod);
    if (pref::did_pref_change(pref::BoolPref::JumpMod)) {
        if (enabled) {
            enable();
        } else {
            disable();
        }
    }
    if (enabled) {
        if (pref::did_pref_change(pref::BoolPref::JumpChangePhysics)) {
            if (pref::get(pref::BoolPref::JumpChangePhysics)) {
                pref::set(pref::U8Pref::PhysicsPreset,
                          static_cast<u8>(physics::PhysicsPreset::JumpPhysics));
            } else {
                pref::set(pref::U8Pref::PhysicsPreset,
                          static_cast<u8>(physics::PhysicsPreset::Default));
            }
            pref::save();
        }
        // Don't run logic while paused
        bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
        if (paused_now) return;
        toggle_minimap();

        if (pref::get(pref::U8Pref::JumpProfile) == 0) {
            jumping();
        } else {
            classic_jumping();
        }
    }
}

}  // namespace jump
