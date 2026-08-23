#include "mods/camera.h"

#include "systems/pref.h"
#include "utils/patch.h"
#include "utils/ppcutil.h"
#include "utils/relutil.h"

typedef enum {
    CameraType_Default = 0,
    CameraType_ForceSMB2 = 1,
    CameraType_ForceSMB1 = 2,
} CameraType;

void Camera_Init() {
}

void Camera_Tick() {
    CameraType value = (CameraType)Pref_Get(Pref_Camera);

    switch (value) {
        case CameraType_Default: {
            if (Pref_DidChange(Pref_Camera)) {
                // restore cam to smb2 once (so toggle still works)
                if (mkb_cameras[0].mode == 0x1) {
                    mkb_cameras[0].mode = 0x4c;
                }

                Patch_WriteWord(Rel_RelocateAddr(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x200));
                mkb_g_camera_turn_rate_scale = 0.75;
                mkb_camera_pivot_height = 0.18;
                mkb_camera_height = 0.8;
            }
            break;
        }
        case CameraType_ForceSMB2: {
            // write every frame to force the values
            if (mkb_cameras[0].mode == 0x1) {
                mkb_cameras[0].mode = 0x4c;
            }

            Patch_WriteWord(Rel_RelocateAddr(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x200));
            mkb_g_camera_turn_rate_scale = 0.75;
            mkb_camera_pivot_height = 0.18;
            mkb_camera_height = 0.8;
            break;
        }
        case CameraType_ForceSMB1: {
            // write every frame to force the values
            if (mkb_cameras[0].mode == 0x4c) {
                mkb_cameras[0].mode = 0x1;
            }
            // Explanation from bomb in WS
            // The write_word statement moves the camera's angle down by 2.8 degrees to match SMB1's
            // angle. Everything else brings the camera position/pivot values in-line with SMB1's
            // values Camera mode 0x1 enables SMB1-like vertical camera tracking, camera mode 0x4c
            // is SMB2's default
            Patch_WriteWord(Rel_RelocateAddr(0x802886c8), PPC_INSTR_LI(PPC_R0, 0x400));
            mkb_g_camera_turn_rate_scale = 0.6875;
            mkb_camera_pivot_height = -0.5;
            mkb_camera_height = 1;
            break;
        }
    }
}
