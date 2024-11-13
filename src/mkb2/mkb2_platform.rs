use crate::platform::Platform;

pub struct Mkb2Platform {}

impl Platform for Mkb2Platform {
    fn os_report(s: *const u8) {
        todo!()
    }
}
