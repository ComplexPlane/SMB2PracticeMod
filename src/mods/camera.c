#include "mods/camera.h"

#include "systems/pref.h"
#include "utils/base.h"
#include "utils/patch.h"
#include "utils/ppcutil.h"

typedef enum CameraType CameraType;
enum CameraType {
    CameraType_Default,
    CameraType_ForceSMB2,
    CameraType_ForceSMB1,
};

static CameraType s_prev_value;

void camera_init() {}

void camera_tick() {
    CameraType value = (CameraType)pref_get(Pref_Camera);

    switch (value) {
    case CameraType_Default:
        if (s_prev_value == value) break;
        // Restore the SMB2 camera once so the game's toggle still works.
        if (mkb_cameras[0].mode == 0x1) mkb_cameras[0].mode = 0x4c;
        patch_write_word((void *)0x802886c8, PPC_INSTR_LI(PPC_R0, 0x200));
        mkb_g_camera_turn_rate_scale = 0.75;
        mkb_camera_pivot_height = 0.18;
        mkb_camera_height = 0.8;
        break;
    case CameraType_ForceSMB2:
        if (mkb_cameras[0].mode == 0x1) mkb_cameras[0].mode = 0x4c;
        patch_write_word((void *)0x802886c8, PPC_INSTR_LI(PPC_R0, 0x200));
        mkb_g_camera_turn_rate_scale = 0.75;
        mkb_camera_pivot_height = 0.18;
        mkb_camera_height = 0.8;
        break;
    case CameraType_ForceSMB1:
        if (mkb_cameras[0].mode == 0x4c) mkb_cameras[0].mode = 0x1;
        patch_write_word((void *)0x802886c8, PPC_INSTR_LI(PPC_R0, 0x400));
        mkb_g_camera_turn_rate_scale = 0.6875;
        mkb_camera_pivot_height = -0.5;
        mkb_camera_height = 1;
        break;
    }
    s_prev_value = value;
}
