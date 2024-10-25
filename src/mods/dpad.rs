use crate::{
    mkb,
    systems::pref::{BoolPref, Pref},
};

pub fn on_pad_read(statuses: *mut mkb::PADStatus, pref: &mut Pref) {
    unsafe {
        if !pref.get_bool(BoolPref::DpadControls) {
            return;
        }

        for i in 0..4 {
            let status = statuses.add(i);
            if (*status).err != mkb::PAD_ERR_NONE as mkb::PadError {
                continue;
            }

            let up = (*status).button & mkb::PAD_BUTTON_UP as mkb::PadDigitalInput != 0;
            let down = (*status).button & mkb::PAD_BUTTON_DOWN as mkb::PadDigitalInput != 0;
            let left = (*status).button & mkb::PAD_BUTTON_LEFT as mkb::PadDigitalInput != 0;
            let right = (*status).button & mkb::PAD_BUTTON_RIGHT as mkb::PadDigitalInput != 0;

            let mut new_x = (*status).stickX as i32;
            let mut new_y = (*status).stickY as i32;
            const CARDINAL: i32 = 107;
            const DIAG: i32 = 78;

            // Modify raw stick input so that input display still looks reasonable when using dpad
            if up && left {
                new_x -= DIAG;
                new_y += DIAG;
            } else if up && right {
                new_x += DIAG;
                new_y += DIAG;
            } else if down && left {
                new_x -= DIAG;
                new_y -= DIAG;
            } else if down && right {
                new_x += DIAG;
                new_y -= DIAG;
            } else if up {
                new_y += CARDINAL;
            } else if down {
                new_y -= CARDINAL;
            } else if left {
                new_x -= CARDINAL;
            } else if right {
                new_x += CARDINAL;
            }

            if up || down || left || right {
                // This is enough to prevent overflow, but diagonals can still exceed their normal range
                // when using stick+dpad
                new_x = new_x.clamp(-CARDINAL, CARDINAL);
                new_y = new_y.clamp(-CARDINAL, CARDINAL);
            }

            (*status).stickX = new_x as i8;
            (*status).stickY = new_y as i8;
        }
    }
}
