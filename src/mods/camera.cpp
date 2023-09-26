#include "camera.h"

#include "mkb/mkb.h"
#include "mkb/mkb2_ghidra.h"
#include "systems/pref.h"
#include "utils/patch.h"
#include "utils/ppcutil.h"

namespace camera {

static bool s_prev_value;

void init() {}

void tick() {
    u8 value = pref::get(pref::U8Pref::Camera);

    switch (value) {
        case 0: {
            if (s_prev_value != value) {
                // restore cam to smb2 once
                if (mkb::cameras[0].mode == 0x1) {
                    mkb::cameras[0].mode = 0x4c;
                }

                patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x200));
                mkb::g_camera_turn_rate_scale = 0.75;
                mkb::camera_pivot_height = 0.18;
                mkb::camera_height = 0.8;
            }
            break;
        }
        case 1: {  // force smb2 cam
            // write every frame to force the values
            if (mkb::cameras[0].mode == 0x1) {
                mkb::cameras[0].mode = 0x4c;
            }

            patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x200));
            mkb::g_camera_turn_rate_scale = 0.75;
            mkb::camera_pivot_height = 0.18;
            mkb::camera_height = 0.8;
            break;
        }
        case 2: {  // force smb1 cam
            // write every frame to force the values
            if (mkb::cameras[0].mode == 0x4c) {
                mkb::cameras[0].mode = 0x1;
            }
            // The write_word statement moves the camera's angle down by 2.8 degrees to match SMB1's
            // angle. Everything else brings the camera position/pivot values in-line with SMB1's
            // values Camera mode 0x1 enables SMB1-like vertical camera tracking, camera mode 0x4c
            // is SMB2's default
            patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x400));
            mkb::g_camera_turn_rate_scale = 0.6875;
            mkb::camera_pivot_height = -0.5;
            mkb::camera_height = 1;
            break;
        }
        case 3: {  // custom values
            // if (mkb::cameras[0].mode != pref::get(pref::U8Pref::CameraMode)) {
            //     mkb::cameras[0].mode = pref::get(pref::U8Pref::CameraMode);
            // }
            // patch::write_word(reinterpret_cast<void*>(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x200));
            // mkb::g_camera_turn_rate_scale =
            //     (float)pref::get(pref::U8Pref::CameraTurnRateScale) / 100.0;
            // mkb::camera_pivot_height = (float)pref::get(pref::U8Pref::CameraPivotHeight) / 100.0;
            // mkb::camera_height = (float)pref::get(pref::U8Pref::CameraHeight) / 100.0;

            // more potential custom values?
            // mkb::g_camera_aspect_ratio ??
            // extern undefined4 g_something_with_camera;
            // extern double g_related_to_camera_turn_rate_and_stage_tilt;
            // extern float camera_near_clip_z;
            // extern float camera_far_clip_z;
            // extern undefined4 g_camera_fov;
            // extern double g_camera_turn_rate_scale;
            // extern double camera_height;
            // extern double camera_pivot_height;
            // extern float camera_distance;
            break;
        }
    }
    s_prev_value = value;
}

}  // namespace camera
