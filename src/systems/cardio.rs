extern crate alloc;

use core::ffi::c_char;
use core::ffi::c_long;
use core::ffi::c_void;
use core::ptr::null_mut;

use crate::mkb;
use crate::mkb_suppl;
use crate::mkb_suppl::to_card_result;
use crate::mkb_suppl::CARDResult;
use crate::mkb_suppl::CARD_READ_SIZE;
use crate::mkb_suppl::CARD_WORKAREA_SIZE;
use crate::utils::math;
use crate::utils::math::round_up_pow2;
use alloc::vec;
use alloc::vec::Vec;
use arrayvec::ArrayString;

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

#[derive(PartialEq, Eq, Clone, Copy)]
enum WriteState {
    Probe,
    Mount,
    Create,
    Delete,
    Write,
    Done,
}

struct WriteRequest {
    file_name: ArrayString<16>,
    buf: Vec<u8>,
    write_size: usize,
    state: WriteState,
    // Sets fake gamecube when request started, unsets when completed
    fake_gamecode: FakeGamecode,
    card_result: CARDResult,
}

pub struct CardIo {
    card_work_area: Vec<u8>, // TODO point to wsmod's card work area if it exists
    card_file_info: mkb::CARDFileInfo,
    write_request: Option<WriteRequest>,
}

impl CardIo {
    pub fn new() -> Self {
        Self {
            card_work_area: vec![0; CARD_WORKAREA_SIZE as usize],
            card_file_info: mkb::CARDFileInfo::default(),
            write_request: None,
        }
    }

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
    pub fn begin_write_file(&mut self, file_name: &str, buf: Vec<u8>) {
        assert!(matches!(self.write_request, None));
        self.write_request = Some(WriteRequest {
            file_name: ArrayString::from(file_name).unwrap(),
            buf,
            write_size: 0,
            state: WriteState::Probe,
            fake_gamecode: FakeGamecode::new(),
            card_result: CARDResult::Ready,
        });
    }

    // Returns the written buffer back and potential error when completed, or None if still writing
    pub fn get_write_result(&mut self) -> Option<(Vec<u8>, CARDResult)> {
        // We could avoid matching twice by taking then matching, but that would require moving the
        // matched write request back into self.write_request if not done
        if let Some(req) = self.write_request.as_ref() {
            if req.state == WriteState::Done {
                if let Some(req) = self.write_request.take() {
                    return Some((req.buf, req.card_result));
                }
            }
        }
        None
    }

    pub fn tick(&mut self) {
        if let Some(req) = self.write_request.as_ref() {
            let mut prev_state = req.state;
            loop {
                self.tick_state_machine();
                let curr_state = self.write_request.as_ref().unwrap().state;
                if curr_state == prev_state {
                    break;
                }
                prev_state = curr_state;
            }
        }
    }

    fn tick_state_machine(&mut self) {
        let req = self.write_request.as_mut().unwrap();
        unsafe {
            // Try again next tick if busy, or finish for any result other than Ready
            req.card_result = to_card_result(mkb::CARDGetResultCode(0));
            if req.card_result == CARDResult::Busy {
                return;
            }
            if req.card_result != CARDResult::Ready {
                req.state = WriteState::Done;
                // I'm assuming that trying to unmount when mounting failed is OK
                mkb::CARDUnmount(0);
                return;
            }

            match req.state {
                WriteState::Probe => {
                    let mut sector_size: c_long = 0;
                    req.card_result =
                        to_card_result(mkb::CARDProbeEx(0, null_mut(), &raw mut sector_size));
                    req.write_size = round_up_pow2(req.buf.len(), sector_size as usize);
                    mkb::CARDMountAsync(
                        0,
                        self.card_work_area.as_mut_ptr() as *mut c_void,
                        null_mut(),
                        null_mut(),
                    );
                    req.state = WriteState::Mount;
                }
                WriteState::Mount => {
                    // Try to open the file
                    req.card_result = to_card_result(mkb::CARDOpen(
                        0,
                        req.file_name.as_ptr() as *mut i8,
                        &mut self.card_file_info,
                    ));
                    if req.card_result == CARDResult::Ready {
                        // Check if file is too small
                        let mut stat = mkb::CARDStat::default();
                        req.card_result = to_card_result(mkb::CARDGetStatus(
                            0,
                            self.card_file_info.fileNo,
                            &mut stat,
                        ));
                        if req.card_result != CARDResult::Ready {
                            req.state = WriteState::Done;
                        } else if (stat.length as usize) < req.write_size {
                            // Recreate file
                            mkb::CARDFastDeleteAsync(0, self.card_file_info.fileNo, null_mut());
                            req.state = WriteState::Delete;
                        } else {
                            // Card opened successfully, proceed directly to writing
                            mkb::CARDWriteAsync(
                                &mut self.card_file_info,
                                req.buf.as_ptr() as *mut c_void,
                                req.write_size as c_long,
                                0,
                                null_mut(),
                            );
                            req.state = WriteState::Write;
                        }
                    } else if req.card_result == CARDResult::NoFile {
                        // Create new file
                        mkb::CARDCreateAsync(
                            0,
                            req.file_name.as_ptr() as *mut i8,
                            req.write_size as u32,
                            &mut self.card_file_info,
                            null_mut(),
                        );
                        req.state = WriteState::Create;
                    } else {
                        // Some other error, fail entire write operation
                        req.state = WriteState::Done;
                    }
                }
                WriteState::Create => {
                    mkb::CARDWriteAsync(
                        &mut self.card_file_info,
                        req.buf.as_mut_ptr() as *mut c_void,
                        req.buf.len() as c_long,
                        0,
                        null_mut(),
                    );
                    req.state = WriteState::Write;
                }
                WriteState::Delete => {
                    mkb::CARDCreateAsync(
                        0,
                        req.file_name.as_ptr() as *mut i8,
                        req.write_size as u32,
                        &mut self.card_file_info,
                        null_mut(),
                    );
                    req.state = WriteState::Create;
                }
                WriteState::Write => {
                    // Wait until the CARDResult is not Busy, which is checked earlier
                    if req.card_result == CARDResult::Ready {
                        req.state = WriteState::Done;
                    }
                }
                WriteState::Done => {
                    // Do nothing until the result is read
                }
            }
        }
    }
}
