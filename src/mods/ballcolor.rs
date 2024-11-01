use core::cell::RefCell;

use critical_section::Mutex;
use num_enum::TryFromPrimitive;
use once_cell::sync::Lazy;

use crate::{
    app_defn::AppContext,
    hook, log,
    mkb::{self},
    systems::{
        draw,
        pref::{Pref, U8Pref},
    },
};

pub const NUM_COLORS: u32 = 9;
pub const COLOR_MIN: u8 = 0;
pub const COLOR_MAX: u8 = 0xff;

// Unfortunately we must move the hook out of BallColor to avoid double borrows
static GAME_READY_INIT_HOOK: Lazy<Mutex<RefCell<GameReadyInitHook>>> =
    Lazy::new(|| Mutex::new(RefCell::new(GameReadyInitHook::new())));

hook!(GameReadyInitHook => (), mkb::smd_game_ready_init, |cx| {
    cx.ball_color.borrow_mut().switch_monkey(&mut cx.pref.borrow_mut());
    critical_section::with(|cs| {
        GAME_READY_INIT_HOOK.borrow(cs).borrow().call();
    })
});

hook!(LoadStagedefHook, stage_id: u32 => (), mkb::load_stagedef, |stage_id, cx| {
    cx.ball_color.borrow().load_stagedef_hook.call(stage_id);
    cx.ball_color.borrow_mut().switch_monkey(&mut cx.pref.borrow_mut());
});

#[derive(PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
pub enum BallColorType {
    Preset = 0,
    RGB = 1,
    Rainbow = 2,
    Random = 3,
}

#[derive(PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
pub enum ClothingType {
    Preset = 0,
    Random = 1,
}

#[derive(PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
pub enum MonkeyType {
    Default = 0,
    Aiai = 1,
    Meemee = 2,
    Baby = 3,
    Gongon = 4,
    Random = 5,
}

pub struct BallColor {
    rainbow: u32,
    default_color: mkb::GXColor,
    current_color: mkb::GXColor,
    load_stagedef_hook: LoadStagedefHook,
}

impl BallColor {
    pub fn new() -> Self {
        Self {
            rainbow: 0,
            default_color: unsafe { *(0x80472a34 as *const mkb::GXColor) },
            current_color: mkb::GXColor::default(),
            load_stagedef_hook: LoadStagedefHook::new(),
        }
    }

    pub fn on_main_loop_load(&mut self, _cx: &AppContext) {
        self.load_stagedef_hook.hook();
    }

    pub fn on_main_game_load(&mut self, _cx: &AppContext) {
        critical_section::with(|cs| {
            GAME_READY_INIT_HOOK.borrow(cs).borrow_mut().hook();
        });
    }

    pub fn get_current_color(&self) -> mkb::GXColor {
        self.current_color
    }

    fn convert_to_ball_color_id(color_choice: u8) -> u8 {
        if color_choice == 0 {
            3
        } else {
            color_choice - 1
        }
    }

    fn convert_to_ape_color_id(color_choice: u8) -> u8 {
        if color_choice == 0 {
            0
        } else {
            color_choice - 1
        }
    }

    pub fn switch_monkey(&self, pref: &mut Pref) {
        unsafe {
            match MonkeyType::try_from(pref.get_u8(U8Pref::MonkeyType)).unwrap() {
                MonkeyType::Default => (),
                MonkeyType::Aiai => {
                    mkb::active_monkey_id[mkb::curr_player_idx as usize] = 0;
                }
                MonkeyType::Meemee => {
                    mkb::active_monkey_id[mkb::curr_player_idx as usize] = 1;
                }
                MonkeyType::Baby => {
                    mkb::active_monkey_id[mkb::curr_player_idx as usize] = 2;
                }
                MonkeyType::Gongon => {
                    mkb::active_monkey_id[mkb::curr_player_idx as usize] = 3;
                }
                MonkeyType::Random => {
                    mkb::active_monkey_id[mkb::curr_player_idx as usize] = (mkb::rand() % 4) as u32;
                }
            }
        }
    }

    pub fn tick(&mut self, cx: &AppContext) {
        unsafe {
            let pref = &mut cx.pref.borrow_mut();
            let ball_type = BallColorType::try_from(pref.get_u8(U8Pref::BallColorType)).unwrap();

            if mkb::main_mode != mkb::MD_GAME
                || (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_INIT
                    || mkb::sub_mode == mkb::SMD_GAME_SCENARIO_MAIN
                    || mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN)
            {
                *(0x80472a34 as *mut mkb::GXColor) = self.default_color;
                return;
            }

            let ape = mkb::balls[mkb::curr_player_idx as usize].ape;
            if ape.is_null() {
                return;
            }

            match ball_type {
                BallColorType::Preset => {
                    *(0x80472a34 as *mut mkb::GXColor) = self.default_color;
                    let color_id = Self::convert_to_ball_color_id(pref.get_u8(U8Pref::BallColor));
                    mkb::balls[mkb::curr_player_idx as usize].g_ball_color_index = color_id;
                    self.current_color = (0x80472a28 as *const mkb::GXColor)
                        .add(color_id as usize)
                        .read();
                }
                BallColorType::RGB => {
                    mkb::balls[mkb::curr_player_idx as usize].g_ball_color_index =
                        Self::convert_to_ball_color_id(0);
                    let red = pref.get_u8(U8Pref::BallRed);
                    let green = pref.get_u8(U8Pref::BallGreen);
                    let blue = pref.get_u8(U8Pref::BallBlue);
                    self.current_color = mkb::GXColor {
                        r: red,
                        g: green,
                        b: blue,
                        a: 0,
                    };
                    *(0x80472a34 as *mut mkb::GXColor) = self.current_color;
                }
                BallColorType::Rainbow => {
                    mkb::balls[mkb::curr_player_idx as usize].g_ball_color_index =
                        Self::convert_to_ball_color_id(0);

                    let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
                    if !paused_now {
                        self.rainbow = (self.rainbow + 3) % 1080;
                    }
                    self.current_color = draw::num_to_rainbow(self.rainbow);
                    *(0x80472a34 as *mut mkb::GXColor) = self.current_color;
                }
                BallColorType::Random => {
                    mkb::balls[mkb::curr_player_idx as usize].g_ball_color_index =
                        Self::convert_to_ball_color_id(0);

                    if mkb::sub_mode == mkb::SMD_GAME_READY_INIT {
                        let bonus_brightness = mkb::rand() % 86;
                        let red = ((mkb::rand() % 256) + bonus_brightness).clamp(0, 0xff);
                        let green = ((mkb::rand() % 256) + bonus_brightness).clamp(0, 0xff);
                        let blue = ((mkb::rand() % 256) + bonus_brightness).clamp(0, 0xff);
                        self.current_color = mkb::GXColor {
                            r: red as u8,
                            g: green as u8,
                            b: blue as u8,
                            a: 0,
                        };
                        *(0x80472a34 as *mut mkb::GXColor) = self.current_color;
                    }
                }
            }

            let clothing_type = ClothingType::try_from(pref.get_u8(U8Pref::ApeColorType)).unwrap();

            match clothing_type {
                ClothingType::Preset => {
                    (*ape).color_index =
                        Self::convert_to_ape_color_id(pref.get_u8(U8Pref::ApeColor)) as i32;
                }
                ClothingType::Random => {
                    if mkb::sub_mode == mkb::SMD_GAME_READY_INIT {
                        (*ape).color_index =
                            Self::convert_to_ape_color_id((mkb::rand() % 9) as u8) as i32;
                    }
                }
            }
        }
    }
}
