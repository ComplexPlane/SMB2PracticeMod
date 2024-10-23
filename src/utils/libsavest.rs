use core::ffi::c_void;

use super::memstore::{self, MemStore};
use crate::mkb;

pub enum SaveError {
    MainMode,
    PostFallout,
    PostGoal,
    DuringRetry,
    PostTimeout,
    SubMode,
    ViewStage,
    InsufficientMemory,
}

pub enum LoadError {
    MainMode,
    SubMode,
    TimeOver,
    Empty,
    WrongStage,
    WrongMonkey,
    ViewStage,
    PausedAndNonGameplaySubmode,
}

#[derive(Default)]
struct SaveState {
    // None means empty savestate
    memstore: Option<memstore::Load>,
    reload_state: bool,
    state_loaded_this_frame: bool,

    // TODO store in MemStore?
    stage_id: u16,
    character: u32,
    pause_menu_sprite_status: u8,
    pause_menu_sprite: mkb::Sprite,
}

impl SaveState {
    pub fn new() -> Self {
        SaveState::default()
    }

    pub fn tick(&mut self) {
        self.state_loaded_this_frame = false;
        if self.reload_state {
            let _ = self.load(); // Ignore result, spooky!
        }
    }

    pub fn save(&mut self) -> Result<(), SaveError> {
        unsafe {
            // Must be in main game
            if mkb::main_mode != mkb::MD_GAME {
                return Err(SaveError::MainMode);
            }

            if mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode_request != mkb::SMD_INVALID
            {
                match mkb::sub_mode {
                    mkb::SMD_GAME_RINGOUT_INIT | mkb::SMD_GAME_RINGOUT_MAIN => {
                        return Err(SaveError::PostFallout);
                    }
                    mkb::SMD_GAME_GOAL_INIT | mkb::SMD_GAME_GOAL_MAIN => {
                        return Err(SaveError::PostGoal);
                    }
                    mkb::SMD_GAME_READY_INIT | mkb::SMD_GAME_READY_MAIN => {
                        return Err(SaveError::DuringRetry);
                    }
                    mkb::SMD_GAME_TIMEOVER_INIT | mkb::SMD_GAME_TIMEOVER_MAIN => {
                        return Err(SaveError::PostTimeout);
                    }
                    _ => return Err(SaveError::SubMode),
                }
            }

            if mkb::events[mkb::EVENT_VIEW as usize].status as u32 != mkb::STAT_NULL {
                return Err(SaveError::ViewStage);
            }

            let mut prealloc = memstore::Prealloc::new();
            Self::pass_over_regions(&mut MemStore::Prealloc(&mut prealloc));
            let mut save =
                memstore::Save::try_from(prealloc).map_err(|_| SaveError::InsufficientMemory)?;

            Self::pass_over_regions(&mut MemStore::Save(&mut save));
            self.stage_id = mkb::current_stage_id;
            self.character = mkb::active_monkey_id[mkb::curr_player_idx as usize];
            // handle_pause_menu_save(); // TODO

            self.memstore = Some(memstore::Load::from(save));
        }

        Ok(())
    }

    pub fn load(&self) -> Result<(), LoadError> {
        todo!()
    }

    pub fn is_empty(&self) -> bool {
        matches!(self.memstore, None)
    }

    pub fn loaded_this_frame(&self) -> bool {
        self.state_loaded_this_frame
    }

    pub fn clear(&mut self) {
        self.memstore = None
    }

    unsafe fn pass_over_regions(memstore: &mut MemStore) {
        memstore.scan_obj(&raw mut mkb::balls[0]);
        memstore.scan_obj(&raw mut mkb::sub_mode);
        memstore.scan_obj(&raw mut mkb::mode_info.stage_time_frames_remaining);
        memstore.scan_region(0x8054E03C as *mut c_void, 0xe0); // Camera region
        memstore.scan_region(0x805BD830 as *mut c_void, 0x1c); // Some physics region
        memstore.scan_obj(&raw mut mkb::mode_info.ball_mode);
        memstore.scan_obj(&raw mut mkb::g_camera_standstill_counters);

        // Ape state (goal is to only save stuff that affects physics)
        let ape = mkb::balls[0].ape;
        memstore.scan_obj(ape);
        // The full size of this buffer is ~10kb, but hopefully this is all we need
        memstore.scan_region((*(*ape).g_some_ape_state).g_buf5 as *mut _, 0x100);

        // Itemgroups
        memstore.scan_region(
            mkb::itemgroups as *mut _,
            size_of::<mkb::Itemgroup>() * (*mkb::stagedef).coli_header_count as usize,
        );

        // Bananas
        memstore.scan_region(
            &raw mut mkb::items as *mut _,
            size_of::<mkb::Item>() * (*mkb::stagedef).banana_count as usize,
        );

        // Goal tape, party ball, and button stage objects
        for i in 0..(mkb::stobj_pool_info.upper_bound as usize) {
            if *mkb::stobj_pool_info.status_list.offset(i as isize) == 0 {
                continue;
            }

            match mkb::stobjs[i].type_ as u32 {
                mkb::STOBJ_GOALTAPE
                | mkb::STOBJ_GOALBAG
                | mkb::STOBJ_GOALBAG_EXMASTER
                | mkb::STOBJ_BUTTON
                | mkb::STOBJ_JAMABAR => {
                    memstore.scan_obj(&raw mut mkb::stobjs[i]);
                }
                _ => {}
            }
        }
    }
}
