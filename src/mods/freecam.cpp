#include "freecam.h"

#include "mkb/mkb.h"

#include "mkb/mkb2_ghidra.h"
#include "systems/binds.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
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
    bool correct_main_mode = mkb::main_mode == mkb::MD_GAME || mkb::main_mode == mkb::MD_ADV ||
                             mkb::main_mode == mkb::MD_MINI || mkb::main_mode == mkb::MD_AUTHOR ||
                             mkb::main_mode == mkb::MD_EXOPT;
    bool correct_sub_mode = mkb::sub_mode != mkb::SMD_GAME_SCENARIO_INIT &&
                            mkb::sub_mode != mkb::SMD_GAME_SCENARIO_MAIN &&
                            mkb::sub_mode != mkb::SMD_GAME_SCENARIO_RETURN &&
                            mkb::sub_mode != mkb::SMD_ADV_TITLE_INIT &&
                            mkb::sub_mode != mkb::SMD_ADV_TITLE_MAIN &&
                            mkb::sub_mode != mkb::SMD_ADV_TITLE_REINIT &&
                            mkb::sub_mode != mkb::SMD_EXOPT_REPLAY_LOAD_INIT &&
                            mkb::sub_mode != mkb::SMD_EXOPT_REPLAY_LOAD_MAIN;
    return pref::get(pref::BoolPref::Freecam) && correct_main_mode && correct_sub_mode;
}

bool should_freeze_timer() { return enabled() && pref::get(pref::BoolPref::FreecamFreezeTimer); }

bool should_hide_hud() { return enabled() && pref::get(pref::BoolPref::FreecamHideHud); }

static void update_cam(mkb::Camera* camera, mkb::Ball* ball) {
    if (!(s_flags & Flags::EnabledPrevTick)) {
        s_eye = mkb::cameras[0].pos;
        s_rot = mkb::cameras[0].rot;
    }

    pad::StickInputs stick, substick;
    pad::TriggerInputs trigger;
    pad::get_merged_stick(stick);
    pad::get_merged_substick(substick);
    pad::get_merged_triggers(trigger);

    float stick_x = stick.x / 60.f;
    float stick_y = stick.y / 60.f;
    float substick_x = substick.x / 60.f;
    float substick_y = substick.y / 60.f;
    float trigger_left = trigger.l / 128.f;
    float trigger_right = trigger.r / 128.f;
    bool fast = pad::button_down(mkb::PAD_BUTTON_Y);
    bool slow = pad::button_down(mkb::PAD_BUTTON_X);

    float speed_mult = fast ? pref::get(pref::U8Pref::FreecamSpeedMult) : 1;
    speed_mult = slow ? 0.15 : speed_mult;

    // New rotation
    bool invert_yaw = pref::get(pref::BoolPref::FreecamInvertYaw);
    bool invert_pitch = pref::get(pref::BoolPref::FreecamInvertPitch);
    s_rot.x -= substick_y * 300 * (invert_pitch ? -1 : 1);
    s_rot.y += substick_x * 490 * (invert_yaw ? -1 : 1);
    s_rot.z = 0;

    // New position
    Vec deltaPos = {stick_x * speed_mult, 0, -stick_y * speed_mult};
    mkb::mtxa_push();
    mkb::mtxa_from_rotate_y(s_rot.y);
    mkb::mtxa_rotate_x(s_rot.x);
    mkb::mtxa_rotate_z(s_rot.z);
    mkb::mtxa_tf_vec(&deltaPos, &deltaPos);
    mkb::mtxa_pop();

    // += leads to a crash on console! Compiler bug?
    s_eye.x = s_eye.x + deltaPos.x;
    s_eye.y = s_eye.y + deltaPos.y + (-trigger_left + trigger_right) * speed_mult;
    s_eye.z = s_eye.z + deltaPos.z;

    camera->pos = s_eye;
    camera->rot = s_rot;

    // Lock ball in place
    bool lock_ball = mkb::sub_mode == mkb::SMD_GAME_PLAY_MAIN || mkb::main_mode == mkb::MD_MINI;
    if (lock_ball) {
        if (mkb::stagedef != nullptr && mkb::stagedef->start != nullptr) {
            ball->pos = mkb::stagedef->start->position;
        }
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
    // Compute enabled on previous tick
    s_flags &= ~Flags::EnabledPrevTick;
    if (s_flags & Flags::EnabledThisTick) {
        s_flags |= Flags::EnabledPrevTick;
    }

    // Optionally toggle freecam with Z
    if (binds::bind_pressed(pref::get(pref::U8Pref::FreecamToggleBind))) {
        pref::set(pref::BoolPref::Freecam, !pref::get(pref::BoolPref::Freecam));
        pref::save();
    }

    s_flags &= ~Flags::EnabledThisTick;
    if (enabled()) {
        s_flags |= Flags::EnabledThisTick;

        // Adjust turbo speed multiplier
        int speed_mult = pref::get(pref::U8Pref::FreecamSpeedMult);
        bool input_made = false;
        if (pad::button_repeat(mkb::PAD_BUTTON_DOWN)) {
            speed_mult--;
            input_made = true;
        }
        if (pad::button_repeat(mkb::PAD_BUTTON_UP)) {
            speed_mult++;
            input_made = true;
        }
        speed_mult = CLAMP(speed_mult, TURBO_SPEED_MIN, TURBO_SPEED_MAX);
        if (input_made) {
            draw::notify(draw::WHITE, "Freecam Turbo Speed Factor: %dX", speed_mult);
            pref::set(pref::U8Pref::FreecamSpeedMult, speed_mult);
            pref::save();
        }
    }
}

}  // namespace freecam
