use crate::platform::Platform;

pub struct Mkb1Platform {}

impl Platform for Mkb1Platform {
    fn os_report(s: *const u8) {
        todo!()
    }
}
