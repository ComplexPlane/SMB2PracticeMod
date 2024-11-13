#![cfg(feature = "mkb2")]

use crate::{
    app::AppContext,
    hook,
    mkb2::mkb2,
    systems::pref::{BoolPref, Pref},
};

hook!(PadReadHook, statuses: *mut mkb2::PADStatus => u32, mkb2::PADRead, |statuses, cx| {
    let dpad = &mut cx.dpad.borrow_mut();
    let pref = &mut cx.pref.borrow_mut();

    let ret = dpad.pad_read_hook.call(statuses);
    dpad.on_pad_read(statuses, pref);
    ret
});

hook!(CreateSpeedSpritesHook, x: f32, y: f32 => (), mkb2::create_speed_sprites, |x, y, cx| {
    cx.dpad.borrow().create_speed_sprites_hook.call(x + 5.0, y);
});

#[derive(Default)]
pub struct Dpad {
    pad_read_hook: PadReadHook,
    create_speed_sprites_hook: CreateSpeedSpritesHook,
}

impl Dpad {
    pub fn on_main_loop_load(&mut self, _cx: &AppContext) {
        self.pad_read_hook.hook();
        self.create_speed_sprites_hook.hook();
    }

    pub fn on_pad_read(&self, statuses: *mut mkb2::PADStatus, pref: &mut Pref) {
        unsafe {
            if !pref.get_bool(BoolPref::DpadControls) {
                return;
            }

            for i in 0..4 {
                let status = statuses.add(i);
                if (*status).err != mkb2::PAD_ERR_NONE as mkb2::PadError {
                    continue;
                }

                let up = (*status).button & mkb2::PAD_BUTTON_UP as mkb2::PadDigitalInput != 0;
                let down = (*status).button & mkb2::PAD_BUTTON_DOWN as mkb2::PadDigitalInput != 0;
                let left = (*status).button & mkb2::PAD_BUTTON_LEFT as mkb2::PadDigitalInput != 0;
                let right = (*status).button & mkb2::PAD_BUTTON_RIGHT as mkb2::PadDigitalInput != 0;

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
}
