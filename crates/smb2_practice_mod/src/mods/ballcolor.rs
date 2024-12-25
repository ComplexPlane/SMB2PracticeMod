use critical_section::Mutex;
use num_enum::TryFromPrimitive;

use mkb::mkb;

use crate::{
    app::with_app,
    hook,
    systems::{
        draw,
        pref::{FromPref, I16Pref, Pref},
    },
    utils::misc::with_mutex,
};

struct Globals {
    load_stagedef_hook: LoadStagedefHook,
}

static GLOBALS: Mutex<Globals> = Mutex::new(Globals {
    load_stagedef_hook: LoadStagedefHook::new(),
});

pub const COLOR_MIN: i16 = 0;
pub const COLOR_MAX: i16 = 0xff;

hook!(LoadStagedefHook, stage_id: u32 => (), mkb::load_stagedef, |stage_id| {
    with_mutex(&GLOBALS, |cx| {
        cx.load_stagedef_hook.call(stage_id);
    });
    with_app(|cx| {
        cx.ball_color.switch_monkey(&cx.pref);
    });
});

#[derive(PartialEq, Eq, TryFromPrimitive)]
#[repr(i16)]
pub enum BallColorType {
    Preset = 0,
    Rgb = 1,
    Rainbow = 2,
    Random = 3,
}

#[derive(PartialEq, Eq, TryFromPrimitive)]
#[repr(i16)]
pub enum ClothingType {
    Preset = 0,
    Random = 1,
}

#[derive(PartialEq, Eq, TryFromPrimitive)]
#[repr(i16)]
pub enum MonkeyType {
    Default = 0,
    Aiai = 1,
    Meemee = 2,
    Baby = 3,
    Gongon = 4,
    Random = 5,
}

// These appear to be different in some romhacks, so just hardcode them to vanilla colors
static PRESET_COLORS: &[mkb::GXColor] = &[
    mkb::GXColor {
        r: 0xFF,
        g: 0x00,
        b: 0x00,
        a: 0xFF,
    },
    mkb::GXColor {
        r: 0x00,
        g: 0x4C,
        b: 0xFF,
        a: 0xFF,
    },
    mkb::GXColor {
        r: 0xFF,
        g: 0xE5,
        b: 0x00,
        a: 0xFF,
    },
    mkb::GXColor {
        r: 0x00,
        g: 0xCC,
        b: 0x00,
        a: 0xFF,
    },
    mkb::GXColor {
        r: 0x00,
        g: 0xCC,
        b: 0xCC,
        a: 0xFF,
    },
    mkb::GXColor {
        r: 0xFF,
        g: 0x00,
        b: 0xCC,
        a: 0xFF,
    },
    mkb::GXColor {
        r: 0x19,
        g: 0x33,
        b: 0x33,
        a: 0xFF,
    },
    mkb::GXColor {
        r: 0xFF,
        g: 0xFF,
        b: 0xE5,
        a: 0xFF,
    },
];

pub struct BallColor {
    rainbow: u32,
    default_color: mkb::GXColor,
    current_color: mkb::GXColor,
}

impl Default for BallColor {
    fn default() -> Self {
        with_mutex(&GLOBALS, |cx| {
            cx.load_stagedef_hook.hook();
        });
        Self {
            rainbow: 0,
            default_color: unsafe { *(0x80472a34 as *const mkb::GXColor) },
            current_color: Default::default(),
        }
    }
}

impl BallColor {
    pub fn get_current_color(&self) -> mkb::GXColor {
        self.current_color
    }

    fn convert_to_ball_color_id(color_choice: i16) -> i16 {
        if color_choice == 0 {
            3
        } else {
            color_choice - 1
        }
    }

    fn convert_to_ape_color_id(color_choice: i16) -> i16 {
        if color_choice == 0 {
            0
        } else {
            color_choice - 1
        }
    }

    pub fn switch_monkey(&self, pref: &Pref) {
        unsafe {
            match MonkeyType::from_pref(I16Pref::MonkeyType, pref) {
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

    pub fn tick(&mut self, pref: &Pref) {
        unsafe {
            let ball_type = BallColorType::from_pref(I16Pref::BallColorType, pref);

            let valid_mode = mkb::main_mode == mkb::MD_GAME
                && (mkb::sub_mode != mkb::SMD_GAME_SCENARIO_INIT
                    && mkb::sub_mode != mkb::SMD_GAME_SCENARIO_MAIN
                    && mkb::sub_mode != mkb::SMD_GAME_SCENARIO_RETURN);

            match ball_type {
                BallColorType::Preset => {
                    if valid_mode {
                        mkb::balls[mkb::curr_player_idx as usize].g_ball_color_index =
                            Self::convert_to_ball_color_id(0) as u8;
                    }
                    self.current_color = PRESET_COLORS
                        [Self::convert_to_ball_color_id(pref.get(I16Pref::BallColor)) as usize];
                    if valid_mode {
                        *(0x80472a34 as *mut mkb::GXColor) = self.current_color;
                    }
                }
                BallColorType::Rgb => {
                    if valid_mode {
                        mkb::balls[mkb::curr_player_idx as usize].g_ball_color_index =
                            Self::convert_to_ball_color_id(0) as u8;
                    }
                    let red = pref.get(I16Pref::BallRed);
                    let green = pref.get(I16Pref::BallGreen);
                    let blue = pref.get(I16Pref::BallBlue);
                    self.current_color = mkb::GXColor {
                        r: red as u8,
                        g: green as u8,
                        b: blue as u8,
                        a: 0,
                    };
                    if valid_mode {
                        *(0x80472a34 as *mut mkb::GXColor) = self.current_color;
                    }
                }
                BallColorType::Rainbow => {
                    if valid_mode {
                        mkb::balls[mkb::curr_player_idx as usize].g_ball_color_index =
                            Self::convert_to_ball_color_id(0) as u8;
                    }

                    let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
                    if !paused_now {
                        self.rainbow = (self.rainbow + 3) % 1080;
                    }
                    self.current_color = draw::num_to_rainbow(self.rainbow);
                    if valid_mode {
                        *(0x80472a34 as *mut mkb::GXColor) = self.current_color;
                    }
                }
                BallColorType::Random => {
                    if valid_mode {
                        mkb::balls[mkb::curr_player_idx as usize].g_ball_color_index =
                            Self::convert_to_ball_color_id(0) as u8;
                    }

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
                        if valid_mode {
                            *(0x80472a34 as *mut mkb::GXColor) = self.current_color;
                        }
                    }
                }
            }

            if !valid_mode {
                *(0x80472a34 as *mut mkb::GXColor) = self.default_color;
            }

            let ape = mkb::balls[mkb::curr_player_idx as usize].ape;
            if !ape.is_null() {
                let clothing_type = ClothingType::from_pref(I16Pref::ApeColorType, pref);

                match clothing_type {
                    ClothingType::Preset => {
                        (*ape).color_index =
                            Self::convert_to_ape_color_id(pref.get(I16Pref::ApeColor)) as i32;
                    }
                    ClothingType::Random => {
                        if mkb::sub_mode == mkb::SMD_GAME_READY_INIT {
                            (*ape).color_index =
                                Self::convert_to_ape_color_id((mkb::rand() % 9) as i16) as i32;
                        }
                    }
                }
            }
        }
    }
}
