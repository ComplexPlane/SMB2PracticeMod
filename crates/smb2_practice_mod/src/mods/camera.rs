use mkb::mkb;
use num_enum::TryFromPrimitive;

use crate::patch;
use crate::systems::pref::{self, I16Pref};
use crate::systems::pref::{FromPref, Pref};
use crate::utils::ppc;

#[derive(TryFromPrimitive)]
#[repr(i16)]
enum CameraType {
    Default = 0,
    ForceSMB2 = 1,
    ForceSMB1 = 2,
}

#[derive(Default)]
pub struct Camera {}

impl Camera {
    pub fn tick(&mut self, pref: &Pref) {
        unsafe {
            let value = CameraType::from_pref(I16Pref::Camera, pref);

            match value {
                CameraType::Default => {
                    if pref.did_change(pref::I16Pref::Camera) {
                        // restore cam to smb2 once (so toggle still works)
                        if mkb::cameras[0].mode == 0x1 {
                            mkb::cameras[0].mode = 0x4c;
                        }

                        patch::write_word(0x802886c8 as *mut _, ppc::instr_li(ppc::Reg::R0, 0x200));
                        mkb::g_camera_turn_rate_scale = 0.75;
                        mkb::camera_pivot_height = 0.18;
                        mkb::camera_height = 0.8;
                    }
                }
                CameraType::ForceSMB2 => {
                    // write every frame to force the values
                    if mkb::cameras[0].mode == 0x1 {
                        mkb::cameras[0].mode = 0x4c;
                    }

                    patch::write_word(0x802886c8 as *mut _, ppc::instr_li(ppc::Reg::R0, 0x200));
                    mkb::g_camera_turn_rate_scale = 0.75;
                    mkb::camera_pivot_height = 0.18;
                    mkb::camera_height = 0.8;
                }
                CameraType::ForceSMB1 => {
                    // write every frame to force the values
                    if mkb::cameras[0].mode == 0x4c {
                        mkb::cameras[0].mode = 0x1;
                    }
                    // Explanation from bomb in WS
                    // The write_word statement moves the camera's angle down by 2.8 degrees to match SMB1's
                    // angle. Everything else brings the camera position/pivot values in-line with SMB1's
                    // values Camera mode 0x1 enables SMB1-like vertical camera tracking, camera mode 0x4c
                    // is SMB2's default
                    patch::write_word(0x802886c8 as *mut _, ppc::instr_li(ppc::Reg::R0, 0x400));
                    mkb::g_camera_turn_rate_scale = 0.6875;
                    mkb::camera_pivot_height = -0.5;
                    mkb::camera_height = 1.0;
                }
            }
        }
    }
}
