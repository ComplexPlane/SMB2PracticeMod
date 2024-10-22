extern crate alloc;

use core::ffi::c_char;
use core::ffi::c_long;
use core::ffi::c_void;
use core::ptr::null_mut;

use crate::mkb;
use crate::mkb_suppl::to_card_result;
use crate::mkb_suppl::CARDResult;
use crate::mkb_suppl::CARD_READ_SIZE;
use crate::mkb_suppl::CARD_WORKAREA_SIZE;
use crate::new_cstr;
use crate::utils::math;
use crate::utils::math::round_up_pow2;
use crate::utils::modlink::ModLink;
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
    card_work_area: &'static mut [u8],
    card_file_info: mkb::CARDFileInfo,
    write_request: Option<WriteRequest>,
}

impl CardIo {
    pub fn new(modlink: &ModLink) -> Self {
        let card_work_area = unsafe {
            match modlink.card_work_area {
                Some(work_area) => {
                    core::slice::from_raw_parts_mut(work_area as *mut u8, CARD_WORKAREA_SIZE)
                }
                None => vec![0; CARD_WORKAREA_SIZE].leak(),
            }
        };
        Self {
            card_work_area,
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
                new_cstr!(file_name, 16),
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
    // or not writing at all
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

    fn finish_write(req: &mut WriteRequest, res: CARDResult) {
        unsafe {
            mkb::CARDUnmount(0); // I'm assuming that trying to unmount when mounting failed is OK
            req.card_result = res;
            req.state = WriteState::Done;
        }
    }

    fn tick_state_machine(&mut self) {
        let req = self.write_request.as_mut().unwrap();

        unsafe {
            match req.state {
                WriteState::Probe => {
                    let mut sector_size: c_long = 0;
                    let res = to_card_result(mkb::CARDProbeEx(
                        0,
                        core::ptr::null_mut(),
                        &mut sector_size,
                    ));
                    if res == CARDResult::Busy {
                        return;
                    }
                    if res != CARDResult::Ready {
                        Self::finish_write(req, res);
                        return;
                    }

                    req.write_size =
                        (req.buf.len() + sector_size as usize - 1) & !(sector_size as usize - 1);
                    mkb::CARDMountAsync(
                        0,
                        self.card_work_area.as_mut_ptr() as *mut c_void,
                        core::ptr::null_mut(),
                        core::ptr::null_mut(),
                    );
                    req.state = WriteState::Mount;
                }

                WriteState::Mount => {
                    let res = to_card_result(mkb::CARDGetResultCode(0));
                    if res == CARDResult::Busy {
                        return;
                    }
                    if res != CARDResult::Ready {
                        Self::finish_write(req, res);
                        return;
                    }

                    // Try to open the file
                    let res = to_card_result(mkb::CARDOpen(
                        0,
                        new_cstr!(req.file_name.as_str(), 16),
                        &mut self.card_file_info,
                    ));
                    if res == CARDResult::Ready {
                        // Check if file is too small
                        let mut stat = mkb::CARDStat::default();
                        let res = to_card_result(mkb::CARDGetStatus(
                            0,
                            self.card_file_info.fileNo,
                            &mut stat,
                        ));
                        if res != CARDResult::Ready {
                            Self::finish_write(req, res);
                        } else if (stat.length as usize) < req.write_size {
                            // Recreate file
                            mkb::CARDFastDeleteAsync(
                                0,
                                self.card_file_info.fileNo,
                                core::ptr::null_mut(),
                            );
                            req.state = WriteState::Delete;
                        } else {
                            // Card opened successfully, proceed directly to writing
                            mkb::CARDWriteAsync(
                                &mut self.card_file_info,
                                req.buf.as_ptr() as *mut c_void,
                                req.write_size as c_long,
                                0,
                                core::ptr::null_mut(),
                            );
                            req.state = WriteState::Write;
                        }
                    } else if res == CARDResult::NoFile {
                        // Create new file
                        mkb::CARDCreateAsync(
                            0,
                            new_cstr!(req.file_name.as_str(), 16),
                            req.write_size as u32,
                            &mut self.card_file_info,
                            core::ptr::null_mut(),
                        );
                        req.state = WriteState::Create;
                    } else {
                        // Some other error, fail entire write operation
                        Self::finish_write(req, res);
                    }
                }

                WriteState::Create => {
                    let res = to_card_result(mkb::CARDGetResultCode(0));
                    if res == CARDResult::Busy {
                        return;
                    }
                    if res != CARDResult::Ready {
                        Self::finish_write(req, res);
                        return;
                    }

                    mkb::CARDWriteAsync(
                        &mut self.card_file_info,
                        req.buf.as_ptr() as *mut c_void,
                        req.write_size as c_long,
                        0,
                        core::ptr::null_mut(),
                    );
                    req.state = WriteState::Write;
                }

                WriteState::Delete => {
                    let res = to_card_result(mkb::CARDGetResultCode(0));
                    if res == CARDResult::Busy {
                        return;
                    }
                    if res != CARDResult::Ready {
                        Self::finish_write(req, res);
                        return;
                    }
                    mkb::CARDCreateAsync(
                        0,
                        new_cstr!(req.file_name.as_str(), 16),
                        req.write_size as u32,
                        &mut self.card_file_info,
                        core::ptr::null_mut(),
                    );
                    req.state = WriteState::Create;
                }

                WriteState::Write => {
                    let res = to_card_result(mkb::CARDGetResultCode(0));
                    if res != CARDResult::Busy {
                        // Either succeeded or failed, either way we're done
                        Self::finish_write(req, res);
                    }
                }

                _ => {}
            }
        }
    }
}
