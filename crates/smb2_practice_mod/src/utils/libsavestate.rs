use core::{cell::Cell, ffi::c_void};
use critical_section::Mutex;
use mkb::mkb;

use super::{
    memstore::{self, MemStore},
    misc::with_mutex,
};
use crate::{hook, mods::timer::Timer};

// Reentrant hook, cannot use app state
hook!(SoundReqIdHook, sfx_idx: u32 => (), mkb::call_SoundReqID_arg_0, |sfx_idx| {
    with_mutex(&GLOBALS, |cx| {
        if !cx.state_loaded_this_frame.get() {
            cx.sound_req_id_hook.call(sfx_idx);
        }
    });
});

struct Globals {
    sound_req_id_hook: SoundReqIdHook,
    state_loaded_this_frame: Cell<bool>,
}

static GLOBALS: Mutex<Globals> = Mutex::new(Globals {
    sound_req_id_hook: SoundReqIdHook::new(),
    state_loaded_this_frame: Cell::new(false),
});

pub struct LibSaveState {}

impl Default for LibSaveState {
    fn default() -> Self {
        with_mutex(&GLOBALS, |cx| {
            cx.sound_req_id_hook.hook();
        });
        Self {}
    }
}

impl LibSaveState {
    pub fn loaded_this_frame(&self) -> bool {
        with_mutex(&GLOBALS, |cx| cx.state_loaded_this_frame.get())
    }
}

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
    Empty,
    MainMode,
    SubMode,
    TimeOver,
    WrongStage,
    WrongMonkey,
    ViewStage,
    PausedAndNonGameplaySubmode,
}

#[derive(Default)]
pub struct SaveState {
    // None means empty savestate
    memstore: Option<memstore::Load>,
    reload_state: bool,

    // TODO store in MemStore?
    stage_id: u16,
    character: u32,
    pause_menu_sprite_status: u8,
    pause_menu_sprite: mkb::Sprite,
}

impl SaveState {
    pub fn tick(&mut self, timer: &mut Timer) {
        with_mutex(&GLOBALS, |cx| {
            cx.state_loaded_this_frame.set(false);
        });
        if self.reload_state {
            let _ = self.load(timer); // Ignore result, spooky!
        }
    }

    pub fn save(&mut self, timer: &mut Timer) -> Result<(), SaveError> {
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
            Self::pass_over_regions(&mut MemStore::Prealloc(&mut prealloc), timer);
            let mut save =
                memstore::Save::try_from(prealloc).map_err(|_| SaveError::InsufficientMemory)?;

            Self::pass_over_regions(&mut MemStore::Save(&mut save), timer);
            self.stage_id = mkb::current_stage_id;
            self.character = mkb::active_monkey_id[mkb::curr_player_idx as usize];
            self.handle_pause_menu_save();

            self.memstore = Some(memstore::Load::from(save));
        }

        Ok(())
    }

    unsafe fn handle_pause_menu_save(&mut self) {
        self.pause_menu_sprite_status = 0;

        // Look for an active sprite that has the same dest func pointer as the pause menu sprite
        for i in 0..(mkb::sprite_pool_info.upper_bound as usize) {
            if *mkb::sprite_pool_info.status_list.add(i) == 0 {
                continue;
            }

            let sprite = &raw mut mkb::sprites[i];
            let disp_func = (*sprite).disp_func;
            if disp_func == Some(mkb::sprite_pausemenu_disp) {
                self.pause_menu_sprite_status = *mkb::sprite_pool_info.status_list.add(i);
                self.pause_menu_sprite = *sprite;
                break;
            }
        }
    }

    pub fn load(&mut self, timer: &mut Timer) -> Result<(), LoadError> {
        unsafe {
            // Must be in main game
            if mkb::main_mode != mkb::MD_GAME {
                return Err(LoadError::MainMode);
            }

            self.reload_state = false;

            if mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT
                || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN
            {
                return Err(LoadError::TimeOver);
            }
            if mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT
                || mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_INIT
                || mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_MAIN
            {
                return Err(LoadError::SubMode);
            }

            if matches!(self.memstore, None) {
                return Err(LoadError::Empty);
            }

            if self.stage_id != mkb::current_stage_id {
                return Err(LoadError::WrongStage);
            }
            if self.character != mkb::active_monkey_id[mkb::curr_player_idx as usize] {
                return Err(LoadError::WrongMonkey);
            }
            if mkb::events[mkb::EVENT_VIEW as usize].status != mkb::STAT_NULL as u8 {
                return Err(LoadError::ViewStage);
            }
            self.handle_load_state_from_nonplay_submode()?;

            // Need to handle pausemenu-specific loading first so we can detect the game isn't currently
            // paused
            self.handle_pause_menu_load();

            let mut memstore = self.memstore.as_mut().unwrap();
            memstore.reset();
            Self::pass_over_regions(&mut MemStore::Load(&mut memstore), timer);

            Self::destruct_non_gameplay_sprites();
            Self::destruct_distracting_effects();

            // If a state is loaded on first spin-in, minimap may never be shown
            if mkb::g_minimap_mode == mkb::MINIMAP_HIDDEN {
                mkb::set_minimap_mode(mkb::MINIMAP_EXPAND);
            }

            with_mutex(&GLOBALS, |cx| {
                cx.state_loaded_this_frame.set(true);
            });
        }

        Ok(())
    }

    unsafe fn handle_load_state_from_nonplay_submode(&mut self) -> Result<(), LoadError> {
        if !(mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT
            || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN
            || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_INIT
            || mkb::sub_mode == mkb::SMD_GAME_GOAL_REPLAY_MAIN
            || mkb::sub_mode == mkb::SMD_GAME_READY_INIT
            || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)
        {
            return Ok(());
        }

        // Loading a state while paused in a non-gameplay mode causes issues for some reason
        let paused_now = unsafe { *(0x805BC474 as *const u32) & 8 != 0 }; // TODO actually give this a name
        if paused_now {
            return Err(LoadError::PausedAndNonGameplaySubmode);
        }

        mkb::event_init(mkb::EVENT_VIBRATION); // Post-goal replay can disable rumble
        mkb::smd_game_play_init();
        mkb::sub_mode_request = mkb::SMD_GAME_PLAY_MAIN;

        // Loading a state for one frame after being in a replay fails to load the state properly, but
        // also loading after a frame has elapsed seems to fix it. There's probably some extra data I
        // need to save, but for now this works.
        self.reload_state = true;

        Ok(())
    }

    unsafe fn handle_pause_menu_load(&mut self) {
        let paused_now = mkb::g_some_other_flags & mkb::OF_GAME_PAUSED != 0;
        let paused_in_state = self.pause_menu_sprite_status != 0;

        if paused_now && !paused_in_state {
            // Destroy the pause menu sprite that currently exists
            for i in 0..(mkb::sprite_pool_info.upper_bound as usize) {
                if *mkb::sprite_pool_info.status_list.add(i) == 0 {
                    continue;
                }

                if let Some(f) = mkb::sprites[i as usize].disp_func {
                    if f as usize == 0x8032a4bc {
                        *mkb::sprite_pool_info.status_list.add(i) = 0;
                        break;
                    }
                }
            }
        } else if !paused_now && paused_in_state {
            // Allocate a new pause menu sprite
            let i = mkb::pool_alloc(
                &raw mut mkb::sprite_pool_info,
                self.pause_menu_sprite_status,
            ) as usize;
            mkb::sprites[i] = self.pause_menu_sprite;
        }
    }

    unsafe fn destruct_non_gameplay_sprites() {
        for i in 0..(mkb::sprite_pool_info.upper_bound as usize) {
            if *mkb::sprite_pool_info.status_list.add(i) == 0 {
                continue;
            }

            let sprite = &mkb::sprites[i];
            let post_goal_sprite_tick = if let Some(tick_func) = sprite.tick_func {
                tick_func == mkb::sprite_fallout_tick
                    || tick_func == mkb::sprite_bonus_finish_or_perfect_tick
                    || tick_func == mkb::sprite_ready_tick
                    || tick_func == mkb::sprite_go_tick
                    || tick_func == mkb::sprite_player_num_tick
                    || tick_func == mkb::sprite_replay_tick
                    || tick_func == mkb::sprite_loadin_stage_name_tick
                    || tick_func == mkb::sprite_bonus_stage_tick
                    || tick_func == mkb::sprite_final_stage_tick
            } else {
                false
            };

            let post_goal_sprite_disp = if let Some(disp_func) = sprite.disp_func {
                disp_func == mkb::sprite_goal_disp
                    || disp_func == mkb::sprite_clear_score_disp
                    || disp_func == mkb::sprite_warp_bonus_disp
                    || disp_func == mkb::sprite_time_bonus_disp
                    || disp_func == mkb::sprite_stage_score_disp
            } else {
                false
            };

            let post_goal_sprite = post_goal_sprite_disp || post_goal_sprite_tick;
            if post_goal_sprite {
                *mkb::sprite_pool_info.status_list.add(i) = 0;
            }
        }
    }

    unsafe fn destruct_distracting_effects() {
        // Destruct current spark effects so we don't see big sparks
        // generated when changing position by a large amount.
        // Also destruct banana grabbing effects
        for i in 0..(mkb::effect_pool_info.upper_bound as usize) {
            if *mkb::effect_pool_info.status_list.add(i) == 0 {
                continue;
            }

            if matches!(
                mkb::effects[i].type_ as u32,
                mkb::EFFECT_COLI_PARTICLE | mkb::EFFECT_HOLDING_BANANA | mkb::EFFECT_GET_BANANA
            ) {
                *mkb::effect_pool_info.status_list.add(i) = 0;
            }
        }
    }

    pub fn is_empty(&self) -> bool {
        matches!(self.memstore, None)
    }

    pub fn clear(&mut self) {
        self.memstore = None
    }

    unsafe fn pass_over_regions(memstore: &mut MemStore, timer: &mut Timer) {
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
            if *mkb::stobj_pool_info.status_list.add(i) == 0 {
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

        // Seesaws
        for i in 0..((*mkb::stagedef).coli_header_count as usize) {
            if (*(*mkb::stagedef).coli_header_list.add(i)).anim_loop_type_and_seesaw
                == mkb::ANIM_SEESAW as mkb::StagedefAnimType
            {
                memstore.scan_region((*(*mkb::itemgroups.add(i)).seesaw_info).state as *mut _, 12);
            }
        }

        // Goal tape and party ball-specific extra data
        memstore.scan_region(
            &raw mut mkb::goaltapes as *mut _,
            size_of::<mkb::GoalTape>() * (*mkb::stagedef).goal_count as usize,
        );
        memstore.scan_region(
            &raw mut mkb::goalbags as *mut _,
            size_of::<mkb::GoalBag>() * (*mkb::stagedef).goal_count as usize,
        );

        // Pause menu
        memstore.scan_region(0x8054DCA8 as *mut _, 56); // Pause menu state
        memstore.scan_region(0x805BC474 as *mut _, 4); // Pause menu bitfield

        for i in 0..(mkb::sprite_pool_info.upper_bound as usize) {
            if *mkb::sprite_pool_info.status_list.add(i) == 0 {
                continue;
            }
            let sprite = &raw mut mkb::sprites[i];

            let tick_func = (*sprite).tick_func;
            if tick_func == Some(mkb::sprite_timer_ball_tick) {
                // Timer ball sprite (it'll probably always be in the same place in the sprite array)
                memstore.scan_obj(sprite);
            } else if tick_func == Some(mkb::sprite_score_tick) {
                // Score sprite's lerped score value
                memstore.scan_obj(&raw mut (*sprite).fpara1);
            }
        }

        // RTA timer
        timer.save_state(memstore);
    }
}

// TODO: move Sprite struct into savestate so we don't get complaints about pointers
// preventing send/sync
unsafe impl Send for SaveState {}
unsafe impl Sync for SaveState {}
