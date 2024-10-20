extern crate alloc;

use core::ffi::c_char;
use core::ffi::c_long;
use core::ffi::c_void;
use core::ptr::null_mut;

use crate::mkb;
use crate::mkb_suppl;
use crate::mkb_suppl::CARDResult;
use crate::mkb_suppl::CARD_READ_SIZE;
use crate::mkb_suppl::CARD_WORKAREA_SIZE;
use crate::utils::math;
use alloc::vec;
use alloc::vec::Vec;

struct FakeGamecode {
    orig_game_name: [c_char; 4],
    orig_game_company: [c_char; 2],
}

impl FakeGamecode {
    fn new() -> Self {
        unsafe {
            mkb::DVD_GAME_NAME = [b'G' as i8, b'M' as i8, b'2' as i8, b'E' as i8];
            mkb::DVD_COMPANY = [b'8' as i8, b'P' as i8];

            Self {
                orig_game_name: mkb::DVD_GAME_NAME,
                orig_game_company: mkb::DVD_COMPANY,
            }
        }
    }
}

impl Drop for FakeGamecode {
    fn drop(&mut self) {
        unsafe {
            mkb::DVD_GAME_NAME = self.orig_game_name;
            mkb::DVD_COMPANY = self.orig_game_company;
        }
    }
}

fn to_card_result(raw_result: c_long) -> CARDResult {
    (raw_result as i32).try_into().unwrap()
}

pub struct CardIo {
    card_work_area: Vec<u8>, // TODO point to wsmod's card work area if it exists
    card_file_info: mkb::CARDFileInfo,
}

impl CardIo {
    pub fn new() -> Self {
        Self {
            card_work_area: vec![0; CARD_WORKAREA_SIZE as usize],
            card_file_info: mkb::CARDFileInfo::default(),
        }
    }

    pub fn tick(&mut self) {}

    // Synchronous at the moment. Also, do not call while write_file() is running!
    pub fn read_file(&mut self, file_name: &str) -> Result<Vec<u8>, CARDResult> {
        unsafe {
            let _fake_gamecode = FakeGamecode::new();
            let mut res;

            // Probe card
            loop {
                res = to_card_result(mkb::CARDProbeEx(0, null_mut(), null_mut()));
                if res != CARDResult::Busy {
                    break;
                }
            }
            if res != CARDResult::Ready {
                return Err(res);
            }

            // Mount card
            mkb::CARDMountAsync(
                0,
                self.card_work_area.as_mut_ptr() as *mut c_void,
                null_mut(),
                null_mut(),
            );
            loop {
                res = to_card_result(mkb::CARDGetResultCode(0));
                if res != CARDResult::Busy {
                    break;
                }
            }
            if res != CARDResult::Ready {
                return Err(res);
            }
            // Open file
            res = to_card_result(mkb::CARDOpen(
                0,
                file_name.as_ptr() as *mut i8,
                &mut self.card_file_info,
            ));
            if res != CARDResult::Ready {
                mkb::CARDUnmount(0);
                return Err(res);
            }

            // Get file size
            let mut stat = mkb::CARDStat::default();
            res = to_card_result(mkb::CARDGetStatus(0, self.card_file_info.fileNo, &mut stat));
            if res != CARDResult::Ready {
                mkb::CARDUnmount(0);
                return Err(res);
            }

            let buf_size = math::round_up_pow2(stat.length as usize, CARD_READ_SIZE as usize);
            let mut buf = vec![0u8; buf_size];

            mkb::CARDReadAsync(
                &mut self.card_file_info as *mut _,
                buf.as_mut_ptr() as *mut _,
                buf_size as c_long,
                0,
                null_mut(),
            );
            loop {
                res = to_card_result(mkb::CARDGetResultCode(0));
                if res != CARDResult::Busy {
                    break;
                }
            }
            if res != CARDResult::Ready {
                mkb::CARDUnmount(0);
                return Err(res);
            }

            mkb::CARDUnmount(0);
            Ok(buf)
        }
    }

    // Writes asynchronously.
    // Takes ownership of the write buffer and gives it back when the write is complete
    pub fn begin_write_file(&mut self, file_name: &str, buf: Vec<u8>) {}

    // Returns the written buffer back and potential error when completed, or None if still writing
    pub fn get_write_result(&mut self) -> Option<(Vec<u8>, CARDResult)> {
        None
    }
}
