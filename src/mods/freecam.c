#include "mods/freecam.h"

#include "utils/base.h"

#include "systems/binds.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

enum {
    Flags_EnabledThisTick = 1 << 0,
    Flags_EnabledPrevTick = 1 << 1,
};

static u32 s_flags;
static Vec s_eye = {0};
static S16Vec s_rot = {0};

static void event_camera_tick_hook();
TRAMP(s_event_camera_tick_tramp, mkb_event_camera_tick, event_camera_tick_hook);

bool freecam_enabled() {
    bool correct_main_mode = mkb_main_mode == mkb_MD_GAME || mkb_main_mode == mkb_MD_ADV ||
                             mkb_main_mode == mkb_MD_MINI || mkb_main_mode == mkb_MD_AUTHOR;
    bool correct_sub_mode =
        mkb_sub_mode != mkb_SMD_GAME_SCENARIO_INIT && mkb_sub_mode != mkb_SMD_GAME_SCENARIO_MAIN &&
        mkb_sub_mode != mkb_SMD_GAME_SCENARIO_RETURN && mkb_sub_mode != mkb_SMD_ADV_TITLE_INIT &&
        mkb_sub_mode != mkb_SMD_ADV_TITLE_MAIN && mkb_sub_mode != mkb_SMD_ADV_TITLE_REINIT;
    return Pref_Get(Pref_Freecam) && correct_main_mode && correct_sub_mode;
}

bool freecam_should_freeze_timer() {
    return freecam_enabled() && Pref_Get(Pref_FreecamFreezeTimer);
}

bool freecam_should_hide_hud() {
    return freecam_enabled() && Pref_Get(Pref_FreecamHideHud);
}

static void update_cam(mkb_Camera *camera, mkb_Ball *ball) {
    if (!(s_flags & Flags_EnabledPrevTick)) {
        s_eye = mkb_cameras[0].pos;
        s_rot = mkb_cameras[0].rot;
    }

    float stick_x = mkb_pad_status_groups[0].raw.stickX / 60.f;
    float stick_y = mkb_pad_status_groups[0].raw.stickY / 60.f;
    float substick_x = mkb_pad_status_groups[0].raw.substickX / 60.f;
    float substick_y = mkb_pad_status_groups[0].raw.substickY / 60.f;
    float trigger_left = mkb_pad_status_groups[0].raw.triggerLeft / 128.f;
    float trigger_right = mkb_pad_status_groups[0].raw.triggerRight / 128.f;
    bool fast = Pad_ButtonDown(mkb_PAD_BUTTON_Y, false);
    bool slow = Pad_ButtonDown(mkb_PAD_BUTTON_X, false);

    float speed_mult = fast ? Pref_Get(Pref_FreecamSpeedMult) : 1;
    speed_mult = slow ? 0.15 : speed_mult;

    // New rotation
    bool invert_yaw = Pref_Get(Pref_FreecamInvertYaw);
    bool invert_pitch = Pref_Get(Pref_FreecamInvertPitch);
    s_rot.x -= substick_y * 300 * (invert_pitch ? -1 : 1);
    s_rot.y += substick_x * 490 * (invert_yaw ? -1 : 1);
    s_rot.z = 0;

    // New position
    Vec deltaPos = {stick_x * speed_mult, 0, -stick_y * speed_mult};
    mkb_mtxa_push();
    mkb_mtxa_from_rotate_y(s_rot.y);
    mkb_mtxa_rotate_x(s_rot.x);
    mkb_mtxa_rotate_z(s_rot.z);
    mkb_mtxa_tf_vec(&deltaPos, &deltaPos);
    mkb_mtxa_pop();

    // += leads to a crash on console! Compiler bug?
    s_eye.x = s_eye.x + deltaPos.x;
    s_eye.y = s_eye.y + deltaPos.y + (-trigger_left + trigger_right) * speed_mult;
    s_eye.z = s_eye.z + deltaPos.z;

    camera->pos = s_eye;
    camera->rot = s_rot;

    // Lock ball in place
    bool lock_ball = mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN || mkb_main_mode == mkb_MD_MINI;
    if (lock_ball) {
        if (mkb_stagedef != nullptr && mkb_stagedef->start != nullptr) {
            ball->pos = mkb_stagedef->start->position;
        }
        ball->vel.x = 0;
        ball->vel.y = 0;
        ball->vel.z = 0;
    }
}

static void call_camera_func_hook(mkb_Camera *camera, mkb_Ball *ball) {
    if (s_flags & Flags_EnabledThisTick) {
        update_cam(camera, ball);
    } else {
        mkb_camera_funcs[camera->mode](camera, ball);
    }
}

static void event_camera_tick_hook() {
    if (freecam_enabled()) {
        for (u32 i = 0; i < LEN(mkb_world_infos); i++) {
            mkb_world_infos[i].stage_tilt_x = 0;
            mkb_world_infos[i].stage_tilt_z = 0;
        }
    }
    s_event_camera_tick_tramp.chain();
}

void freecam_init() {
    patch_write_branch_bl((void *)(0x8028353c), (void *)(call_camera_func_hook));

    HOOK_TRAMP(s_event_camera_tick_tramp);
}

void freecam_tick() {
    // Compute enabled on previous tick
    s_flags &= ~Flags_EnabledPrevTick;
    if (s_flags & Flags_EnabledThisTick) {
        s_flags |= Flags_EnabledPrevTick;
    }

    // Optionally toggle freecam with Z
    if (Binds_Pressed(Pref_Get(Pref_FreecamToggleBind), false)) {
        Pref_Set(Pref_Freecam, !Pref_Get(Pref_Freecam));
        Prev_Save();
    }

    s_flags &= ~Flags_EnabledThisTick;
    if (freecam_enabled()) {
        s_flags |= Flags_EnabledThisTick;

        // Adjust turbo speed multiplier
        int speed_mult = Pref_Get(Pref_FreecamSpeedMult);
        bool input_made = false;
        if (Pad_ButtonRepeat(mkb_PAD_BUTTON_DOWN, false)) {
            speed_mult--;
            input_made = true;
        }
        if (Pad_ButtonRepeat(mkb_PAD_BUTTON_UP, false)) {
            speed_mult++;
            input_made = true;
        }
        speed_mult = CLAMP(speed_mult, freecam_TURBO_SPEED_MIN, freecam_TURBO_SPEED_MAX);
        if (input_made) {
            Draw_Notify(COLOR_WHITE, "Freecam Turbo Speed Factor: %dX", speed_mult);
            Pref_Set(Pref_FreecamSpeedMult, speed_mult);
            Prev_Save();
        }
    }
}
