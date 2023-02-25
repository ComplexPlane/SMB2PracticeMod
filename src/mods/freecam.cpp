#include "freecam.h"

#include "mkb/mkb.h"

#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"

namespace freecam {

namespace Flags {
enum {
    EnabledThisTick = 1 << 0,
    EnabledPrevTick = 1 << 1,
};
}

static u32 s_flags;
static Vec s_eye = {};
static S16Vec s_rot = {};

static patch::Tramp<decltype(&mkb::event_camera_tick)> s_event_camera_tick_tramp;

bool enabled() {
    return pref::get(pref::BoolPref::Freecam) && mkb::main_mode != mkb::MD_SEL &&
           mkb::main_mode_request != mkb::MD_SEL && mkb::main_mode_request;
}

static void update_cam(mkb::Camera* camera, mkb::Ball* ball) {
    if (!(s_flags & Flags::EnabledPrevTick)) {
        s_eye = mkb::cameras[0].pos;
        s_rot = mkb::cameras[0].rot;
    }

    float stick_x = mkb::pad_status_groups[0].raw.stickX / 60.f;
    float stick_y = mkb::pad_status_groups[0].raw.stickY / 60.f;
    float substick_x = mkb::pad_status_groups[0].raw.substickX / 60.f;
    float substick_y = mkb::pad_status_groups[0].raw.substickY / 60.f;
    float trigger_left = mkb::pad_status_groups[0].raw.triggerLeft / 128.f;
    float trigger_right = mkb::pad_status_groups[0].raw.triggerRight / 128.f;
    bool fast = pad::button_down(mkb::PAD_BUTTON_Y);

    float speed_mult = fast ? 3.0f : 1.0f;

    // New rotation
    bool invert_yaw = pref::get(pref::BoolPref::FreecamInvertYaw);
    bool invert_pitch = pref::get(pref::BoolPref::FreecamInvertPitch);
    s_rot.x -= substick_y * 300 * (invert_pitch ? -1 : 1);
    s_rot.y += substick_x * 470 * (invert_yaw ? -1 : 1);
    s_rot.z = 0;

    // New position
    Vec deltaPos = {stick_x * speed_mult, 0, -stick_y * speed_mult};
    mkb::mtxa_push();
    mkb::mtxa_from_rotate_y(s_rot.y);
    mkb::mtxa_rotate_x(s_rot.x);
    mkb::mtxa_rotate_z(s_rot.z);
    mkb::mtxa_tf_vec(&deltaPos, &deltaPos);
    mkb::mtxa_pop();
    s_eye.x += deltaPos.x;
    s_eye.y += deltaPos.y - trigger_left + trigger_right;
    s_eye.z += deltaPos.z;

    camera->pos = s_eye;
    camera->rot = s_rot;

    // Lock ball in place
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::main_mode == mkb::MD_MINI) {
        ball->pos = mkb::stagedef->start->position;
        ball->vel.x = 0;
        ball->vel.y = 0;
        ball->vel.z = 0;
    }
}

static void call_camera_func_hook(mkb::Camera* camera, mkb::Ball* ball) {
    if (s_flags & Flags::EnabledThisTick) {
        update_cam(camera, ball);
    } else {
        mkb::camera_funcs[camera->mode](camera, ball);
    }
}

void init() {
    patch::write_branch_bl(reinterpret_cast<void*>(0x8028353c),
                           reinterpret_cast<void*>(call_camera_func_hook));

    patch::hook_function(s_event_camera_tick_tramp, mkb::event_camera_tick, []() {
        if (enabled()) {
            for (u32 i = 0; i < LEN(mkb::world_infos); i++) {
                mkb::world_infos[i].stage_tilt_x = 0;
                mkb::world_infos[i].stage_tilt_z = 0;
            }
        }
        s_event_camera_tick_tramp.dest();
    });
}

void tick() {
    s_flags &= ~Flags::EnabledPrevTick;
    if (s_flags & Flags::EnabledThisTick) {
        s_flags |= Flags::EnabledPrevTick;
    }
    s_flags &= ~Flags::EnabledThisTick;
    if (enabled()) {
        s_flags |= Flags::EnabledThisTick;
    }
}

}  // namespace freecam
