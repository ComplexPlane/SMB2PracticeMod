use arrayvec::ArrayString;

use super::pad::{Button, Pad, Prio};

#[derive(Clone, Copy, PartialEq, Eq, Default)]
pub enum EncodingType {
    SinglePress,
    ChordPress,
    #[default]
    Invalid,
}

struct InputDefn {
    input: Button,
    display_string: &'static str,
}

const INPUT_DEFNS: &[InputDefn] = &[
    InputDefn {
        input: Button::A,
        display_string: "A",
    },
    InputDefn {
        input: Button::B,
        display_string: "B",
    },
    InputDefn {
        input: Button::X,
        display_string: "X",
    },
    InputDefn {
        input: Button::Y,
        display_string: "Y",
    },
    InputDefn {
        input: Button::L,
        display_string: "L",
    },
    InputDefn {
        input: Button::R,
        display_string: "R",
    },
    InputDefn {
        input: Button::Z,
        display_string: "Z",
    },
    InputDefn {
        input: Button::DpadUp,
        display_string: "Dpad-Up",
    },
    InputDefn {
        input: Button::DpadDown,
        display_string: "Dpad-Down",
    },
    InputDefn {
        input: Button::DpadLeft,
        display_string: "Dpad-Left",
    },
    InputDefn {
        input: Button::DpadRight,
        display_string: "Dpad-Right",
    },
    InputDefn {
        input: Button::Start,
        display_string: "Start",
    },
];

#[derive(Default)]
pub struct Binds {
    pressed: [bool; INPUT_DEFNS.len()],
    prev_pressed: [u8; 2],
    encoding: u8,
    encoding_type: EncodingType,
    num_prev_held: u8,
}

impl Binds {
    pub const INVALID: u8 = 255;

    fn get_button_values(&mut self, pad: &Pad) {
        for (i, input_defn) in INPUT_DEFNS.iter().enumerate() {
            self.pressed[i] = pad.button_down(input_defn.input, Prio::High);
        }
    }

    fn is_num_pressed(&self, num: usize) -> bool {
        num < self.pressed.len() && self.pressed[num]
    }

    fn encode_bind(&mut self, type_: EncodingType) {
        self.encoding_type = type_;
        match type_ {
            EncodingType::Invalid => {}
            EncodingType::SinglePress => {
                self.encoding =
                    self.prev_pressed[0] + self.prev_pressed[0] * INPUT_DEFNS.len() as u8;
            }
            EncodingType::ChordPress => {
                if self.prev_pressed[0] < self.prev_pressed[1] {
                    self.encoding =
                        self.prev_pressed[0] + self.prev_pressed[1] * INPUT_DEFNS.len() as u8;
                } else {
                    self.encoding =
                        self.prev_pressed[1] + self.prev_pressed[0] * INPUT_DEFNS.len() as u8;
                }
            }
        }
    }

    pub fn tick(&mut self, pad: &Pad) {
        self.get_button_values(pad);

        let mut pressed_count = 0;
        let mut current_pressed = [Self::INVALID; 2];
        for i in 0..self.pressed.len() {
            if self.is_num_pressed(i) {
                if pressed_count < 2 {
                    current_pressed[pressed_count] = i as u8;
                }
                pressed_count += 1;
            }
        }

        if pressed_count < 2 && self.num_prev_held == 2 {
            self.encode_bind(EncodingType::ChordPress);
        } else if pressed_count < 1 && self.num_prev_held == 1 {
            self.encode_bind(EncodingType::SinglePress);
        } else {
            self.encode_bind(EncodingType::Invalid);
        }

        self.num_prev_held = pressed_count as u8;
        self.prev_pressed = current_pressed;
    }

    pub fn get_current_encoding(&self) -> u8 {
        if self.encoding_type == EncodingType::Invalid {
            Self::INVALID
        } else {
            self.encoding
        }
    }

    pub fn get_encoding_type(&self) -> EncodingType {
        self.encoding_type
    }

    pub fn get_input1(&self, bind_id: u8) -> u8 {
        bind_id % INPUT_DEFNS.len() as u8
    }

    pub fn get_input2(&self, bind_id: u8) -> u8 {
        (bind_id - (bind_id % INPUT_DEFNS.len() as u8)) / INPUT_DEFNS.len() as u8
    }

    pub fn get_bind_str(&self, bind_id: u8, buf: &mut ArrayString<32>) {
        let i1 = self.get_input1(bind_id);
        let i2 = self.get_input2(bind_id);
        if bind_id == Self::INVALID {
            buf.push_str("(No Bind)");
        } else if i1 == i2 {
            buf.push('(');
            buf.push_str(INPUT_DEFNS[i1 as usize].display_string);
            buf.push(')');
        } else {
            buf.push('(');
            buf.push_str(INPUT_DEFNS[i1 as usize].display_string);
            buf.push('+');
            buf.push_str(INPUT_DEFNS[i2 as usize].display_string);
            buf.push(')');
        }
    }

    pub fn bind_pressed(&self, bind_id: u8, priority: Prio, pad: &Pad) -> bool {
        if bind_id == Self::INVALID {
            return false;
        }
        let input1 = INPUT_DEFNS[self.get_input1(bind_id) as usize].input;
        let input2 = INPUT_DEFNS[self.get_input2(bind_id) as usize].input;

        if input1 == input2 {
            pad.button_pressed(input1, priority)
        } else {
            pad.button_chord_pressed(input1, input2, priority)
        }
    }
}
