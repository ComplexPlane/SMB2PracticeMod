#include "mods/jump.h"

#include "mods/physics.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"
#include "utils/relutil.h"

typedef enum {
    MaxJumpCount_One = 0,
    MaxJumpCount_Two = 1,
    MaxJumpCount_Infinite = 2,
} MaxJumpCount;

typedef enum {
    JumpState_NotJumping = 0,
    JumpState_GroundedJump = 1,
    JumpState_AerialJump = 2,
} JumpState;

static constexpr s32 JUMP_LENGTH = 15;
static constexpr s32 EARLY_BUFFER_LENGTH = 4;
static constexpr s32 LATE_BUFFER_LENGTH = 5;
static constexpr f32 WALLJUMP_NORMAL = -0.5;

static constexpr s32 CLASSIC_EARLY_BUFFER_LENGTH = 4;
static constexpr s32 CLASSIC_LATE_BUFFER_LENGTH = 8;

static u32 s_patch1;
static u32 s_patch2;

static JumpState s_jumping = JumpState_NotJumping;
static u32 s_jump_frames = 0;
static u32 s_ticks_since_jump_input = 0;
static u32 s_ticks_since_ground = 0;
static s32 s_aerial_jumps = 0;

static void reset() {
    s_ticks_since_jump_input = EARLY_BUFFER_LENGTH + 1;
    s_ticks_since_ground = LATE_BUFFER_LENGTH + 1;
    s_jumping = JumpState_NotJumping;
    s_jump_frames = 0;
    s_aerial_jumps = 0;
}

void Jump_PatchMinimap() {
    // Patch out Minimap Toggle
    // Function is ran whenever minimap is enabled or whenever main_game.rel is loaded
    if (mkb_main_mode == mkb_MD_GAME && Pref_Get(Pref_JumpMod)) {
        u32 *patch1_loc = (u32 *)Rel_RelocateAddr(0x808f4d18);
        u32 *patch2_loc = (u32 *)Rel_RelocateAddr(0x808f5168);

        // Patch instructions if they aren't nop
        if (*patch1_loc != 0x60000000) {
            s_patch1 = Patch_WriteNop(Rel_RelocateAddr(0x808f4d18));
        }
        if (*patch2_loc != 0x60000000) {
            s_patch2 = Patch_WriteNop(Rel_RelocateAddr(0x808f5168));
        }
    }
}

static void restore_minimap() {
    if (mkb_main_mode == mkb_MD_GAME) {
        // These overwrites exist in main_game.rel which isn't always loaded
        Patch_WriteWord(Rel_RelocateAddr(0x808f4d18), s_patch1);
        Patch_WriteWord(Rel_RelocateAddr(0x808f5168), s_patch2);
    }
}

static void enable() {
    Jump_PatchMinimap();
    if (Pref_Get(Pref_JumpChangePhysics)) {
        Pref_Set(Pref_PhysicsPreset, (u8)PhysicsPreset_JumpPhysics);
        Pref_Save();
    }
    reset();
}

static void disable() {
    restore_minimap();
    if (Pref_Get(Pref_JumpChangePhysics)) {
        Pref_Set(Pref_PhysicsPreset, (u8)PhysicsPreset_Default);
        Pref_Save();
    }
}

static void end_jump() {
    s_jumping = JumpState_NotJumping;
    s_jump_frames = 0;
}

static f32 jump_curve(s32 current, s32 max) {
    f32 lerp = (f32)(max - current) / max;
    return lerp * lerp * lerp;
}

static void toggle_minimap() {
    // Minimap Toggle with B
    if (mkb_sub_mode == mkb_SMD_GAME_READY_MAIN || mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN) {
        if (Pad_ButtonPressed(mkb_PAD_BUTTON_B, false)) {
            mkb_toggle_minimap_zoom();
        }
    }
}

static void jumping() {
    // Reset state on READY_INIT
    if (mkb_sub_mode != mkb_SMD_GAME_PLAY_MAIN && mkb_sub_mode != mkb_SMD_GAME_PLAY_INIT) {
        reset();
        return;
    }

    // Setup vars
    mkb_Ball *ball = &mkb_balls[mkb_curr_player_idx];
    bool a_pressed = Pad_ButtonPressed(mkb_PAD_BUTTON_A, false);
    bool a_down = Pad_ButtonDown(mkb_PAD_BUTTON_A, false);
    bool a_released = Pad_ButtonReleased(mkb_PAD_BUTTON_A, false);
    bool ground_touched = (ball->phys_flags & mkb_PHYS_ON_GROUND);
    Vec normal_vec = mkb_balls[mkb_curr_player_idx].g_last_coli_normal;

    // Track Jump Presses
    if (a_pressed) {
        s_ticks_since_jump_input = 0;
    } else {
        s_ticks_since_jump_input++;
    }

    bool valid_location = normal_vec.y < WALLJUMP_NORMAL || Pref_Get(Pref_JumpAllowWalljumps);
    // Track Ground Touched
    if (ground_touched && valid_location) {
        s_ticks_since_ground = 0;

        MaxJumpCount count = (MaxJumpCount)Pref_Get(Pref_JumpCount);
        if (count == MaxJumpCount_Two) {
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
    bool aerial_jumped =
        (s_aerial_jumps > 0 || (MaxJumpCount)Pref_Get(Pref_JumpCount) == MaxJumpCount_Infinite) &&
        a_pressed;
    bool start_jump = mkb_sub_mode == mkb_SMD_GAME_PLAY_INIT &&
                      s_ticks_since_jump_input < EARLY_BUFFER_LENGTH && a_down;

    if (start_jump) {
        s_jumping = JumpState_GroundedJump;
        s_ticks_since_ground += LATE_BUFFER_LENGTH;  // cannot coyote jump anymore
    } else if ((buffered_early || coyote_late) && valid_location) {
        s_jumping = JumpState_GroundedJump;
        s_ticks_since_ground += LATE_BUFFER_LENGTH;  // cannot coyote jump anymore
    } else if (aerial_jumped) {
        s_jumping = JumpState_AerialJump;
        s_aerial_jumps--;
    }

    // end jump
    if (a_released) {
        end_jump();
        return;
    }

    // jump!
    if (s_jumping == JumpState_GroundedJump || s_jumping == JumpState_AerialJump) {
        // first frame of jump
        if (s_jump_frames == 0) {
            mkb_call_SoundReqID_arg_0(268);
            if (ball->vel.y < 0) {
                ball->vel.y = 0;
            }
        }
        // tick jump frames
        if (s_jump_frames > JUMP_LENGTH) {
            end_jump();
            return;
        } else {
            s_jump_frames++;
        }

        if (s_jumping == JumpState_GroundedJump) {
            ball->vel.x += jump_curve(s_jump_frames, JUMP_LENGTH) * (0.05 * -normal_vec.x);
            ball->vel.z += jump_curve(s_jump_frames, JUMP_LENGTH) * (0.05 * -normal_vec.z);
            f32 expected_height = (0.1 * -normal_vec.y);
            f32 bonus_height = (1.0 - ABS(normal_vec.y)) * 0.08;
            f32 lerped_height =
                jump_curve(s_jump_frames, JUMP_LENGTH) * (expected_height + bonus_height);
            ball->vel.y += lerped_height;
        } else {
            ball->vel.y += jump_curve(s_jump_frames, JUMP_LENGTH) * 0.09;
        }
    }
}

static void classic_jumping() {
    if (mkb_sub_mode != mkb_SMD_GAME_READY_MAIN && mkb_sub_mode != mkb_SMD_GAME_PLAY_INIT &&
        mkb_sub_mode != mkb_SMD_GAME_PLAY_MAIN) {
        s_ticks_since_jump_input = CLASSIC_EARLY_BUFFER_LENGTH + 1;
        s_ticks_since_ground = CLASSIC_LATE_BUFFER_LENGTH + 1;
        s_jumping = JumpState_NotJumping;
        s_jump_frames = 0;
        return;
    }

    // Setup vars
    mkb_Ball *ball = &mkb_balls[mkb_curr_player_idx];
    bool a_pressed = Pad_ButtonPressed(mkb_PAD_BUTTON_A, false);
    bool a_down = Pad_ButtonDown(mkb_PAD_BUTTON_A, false);
    bool a_released = Pad_ButtonReleased(mkb_PAD_BUTTON_A, false);
    bool ground_touched = (ball->phys_flags & mkb_PHYS_ON_GROUND);

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
    bool go_buffered_press = mkb_sub_mode == mkb_SMD_GAME_PLAY_INIT && a_down;

    if (before || after || go_buffered_press) s_jumping = JumpState_GroundedJump;

    if (a_released) {
        s_jumping = JumpState_NotJumping;
        s_jump_frames = 0;
    }

    if (s_jumping == JumpState_GroundedJump) {
        if (s_jump_frames == 0) {
            mkb_call_SoundReqID_arg_0(268);
        }

        s_jump_frames++;
        if (s_jump_frames > JUMP_LENGTH) {
            s_jumping = JumpState_NotJumping;
            s_jump_frames = 0;
            return;
        }

        f32 lerp = (f32)(JUMP_LENGTH - s_jump_frames) / JUMP_LENGTH;
        lerp = lerp * lerp * lerp;
        ball->vel.y += lerp * 0.1;
    }
}

void Jump_Tick() {
    bool enabled = Pref_Get(Pref_JumpMod);
    if (Pref_DidChange(Pref_JumpMod)) {
        if (enabled) {
            enable();
        } else {
            disable();
        }
    }
    if (enabled) {
        if (Pref_DidChange(Pref_JumpChangePhysics)) {
            if (Pref_Get(Pref_JumpChangePhysics)) {
                Pref_Set(Pref_PhysicsPreset, (u8)PhysicsPreset_JumpPhysics);
            } else {
                Pref_Set(Pref_PhysicsPreset, (u8)PhysicsPreset_Default);
            }
            Pref_Save();
        }
        // Don't run logic while paused
        bool paused_now = *(u32 *)Rel_RelocateAddr(0x805BC474) & 8;
        if (paused_now) return;
        toggle_minimap();

        if (Pref_Get(Pref_JumpProfile) == 0) {
            jumping();
        } else {
            classic_jumping();
        }
    }
}
