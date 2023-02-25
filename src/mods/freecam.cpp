#include "freecam.h"

#include "mkb/mkb.h"

#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/patch.h"
#include "utils/macro_utils.h"

namespace freecam {

enum FreecamFlags {
    FREECAM_FLAG_ENABLED_PREV_FRAME = 1 << 0,
};

static bool s_enabledPrevTick = false;
static Vec s_fcEye = {};
static S16Vec s_fcRot = {};

static patch::Tramp<decltype(&mkb::event_camera_tick)> s_event_camera_tick_tramp;

static void update_cam(mkb::Camera* camera, mkb::Ball* ball) {
    bool enabledNow = pref::get(pref::BoolPref::Freecam);
    if (enabledNow != s_enabledPrevTick) {
        s_enabledPrevTick = enabledNow;
        if (enabledNow) {
            s_fcEye = mkb::cameras[0].pos;
            s_fcRot = mkb::cameras[0].rot;
        }
    }

    if (!enabledNow) return;

    float stickX = mkb::pad_status_groups[0].raw.stickX / 60.f;
    float stickY = mkb::pad_status_groups[0].raw.stickY / 60.f;
    float substickX = mkb::pad_status_groups[0].raw.substickX / 60.f;
    float substickY = mkb::pad_status_groups[0].raw.substickY / 60.f;
    float triggerLeft = mkb::pad_status_groups[0].raw.triggerLeft / 128.f;
    float triggerRight = mkb::pad_status_groups[0].raw.triggerRight / 128.f;
    bool fast = pad::button_down(mkb::PAD_BUTTON_Y);

    float speedMult = fast ? 3.0f : 1.0f;

    // New rotation
    s_fcRot.x -= substickY * 300;
    s_fcRot.y += substickX * 500;
    s_fcRot.z = 0;

    // New position
    Vec deltaPos = {stickX * speedMult, 0, -stickY * speedMult};
    mkb::mtxa_push();
    mkb::mtxa_from_rotate_y(s_fcRot.y);
    mkb::mtxa_rotate_x(s_fcRot.x);
    mkb::mtxa_rotate_z(s_fcRot.z);
    mkb::mtxa_tf_vec(&deltaPos, &deltaPos);
    mkb::mtxa_pop();
    s_fcEye.x += deltaPos.x;
    s_fcEye.y += deltaPos.y - triggerLeft + triggerRight;
    s_fcEye.z += deltaPos.z;

    camera->pos = s_fcEye;
    camera->rot = s_fcRot;

    // Lock ball in place
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::main_mode == mkb::MD_MINI) {
        ball->pos = mkb::stagedef->start->position;
        ball->vel.x = 0;
        ball->vel.y = 0;
        ball->vel.z = 0;
    }
}

void init() {
    patch::hook_function(s_event_camera_tick_tramp, mkb::event_camera_tick, []() {
        if (pref::get(pref::BoolPref::Freecam)) {
            for (u32 i = 0; i < LEN(mkb::world_infos); i++) {
                mkb::world_infos[i].stage_tilt_x = 0;
                mkb::world_infos[i].stage_tilt_z = 0;
            }
        }
        s_event_camera_tick_tramp.dest();
    });
}

void tick() {
    bool enabledNow = pref::get(pref::BoolPref::Freecam);
    if (enabledNow != s_enabledPrevTick) {
        s_enabledPrevTick = enabledNow;
        if (enabledNow) {
            s_fcEye = mkb::cameras[0].pos;
            s_fcRot = mkb::cameras[0].rot;

            // Patch camera update function with our own
            patch::write_branch_bl(reinterpret_cast<void*>(0x8028353c),
                                   reinterpret_cast<void*>(update_cam));
        } else {
            // Original camera update blctrl
            patch::write_word(reinterpret_cast<void*>(0x8028353c), 0x4e800421);
        }
    }
}

}  // namespace freecam
