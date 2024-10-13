use num_enum::IntoPrimitive;

use crate::mkb;

#[derive(Clone, Copy, PartialEq, Eq)]
enum Dir {
    Up,
    UpRight,
    Right,
    DownRight,
    Down,
    DownLeft,
    Left,
    UpLeft,
    None = -1,
}

#[derive(Default)]
struct StickState {
    x: i32,
    y: i32,
}

#[derive(Default)]
struct TriggerState {
    l: i32,
    r: i32,
}

#[derive(Default)]
struct AnalogState {
    raw_stick_x: i32,
    raw_stick_y: i32,
    stick_x: i32,
    stick_y: i32,
    substick_x: i32,
    substick_y: i32,
    trigger_l: i32,
    trigger_r: i32,
}

const MAX_STICK: i32 = 60;
const MAX_TRIGGER: i32 = 128;
const DIR_REPEAT_PERIOD: u32 = 3;
const DIR_REPEAT_WAIT: u32 = 14;

// TODO use Default once we can non-const initialize AppContext

const ZERO_ANALOG_INPUT_GROUP: mkb::AnalogInputGroup = mkb::AnalogInputGroup {
    raw: 0,
    prev_tick: 0,
    pressed: 0,
    released: 0,
    repeated: 0,
};

const ZERO_DIGITAL_INPUT_GROUP: mkb::DigitalInputGroup = mkb::DigitalInputGroup {
    raw: 0,
    prev_tick: 0,
    pressed: 0,
    released: 0,
    repeated: 0,
};

const ZERO_PAD_STATUS: mkb::PADStatus = mkb::PADStatus {
    button: 0,
    stickX: 0,
    stickY: 0,
    substickX: 0,
    substickY: 0,
    triggerLeft: 0,
    triggerRight: 0,
    analogA: 0,
    analogB: 0,
    err: 0,
    field_0xb: [0],
};

const ZERO_PAD_STATUS_GROUP: mkb::PadStatusGroup = mkb::PadStatusGroup {
    raw: ZERO_PAD_STATUS,
    prev_tick: ZERO_PAD_STATUS,
    pressed: ZERO_PAD_STATUS,
    released: ZERO_PAD_STATUS,
    repeated: ZERO_PAD_STATUS,
};

struct Pad {
    analog_state: AnalogState,
    konami_progress: i32,
    konami_input_prev_tick: bool,
    exclusive_mode: bool,
    exclusive_mode_request: bool,

    merged_analog_inputs: mkb::AnalogInputGroup,
    merged_digital_inputs: mkb::DigitalInputGroup,
    pad_status_groups: [mkb::PadStatusGroup; 4],
    original_inputs: [mkb::PADStatus; 4],
    analog_inputs: [mkb::AnalogInputGroup; 4],

    dir_down_time: [u8; 8],
}

impl Pad {
    pub const fn new() -> Self {
        Self {
            konami_progress: 0,
            konami_input_prev_tick: false,
            exclusive_mode: false,
            exclusive_mode_request: false,
            merged_analog_inputs: ZERO_ANALOG_INPUT_GROUP,
            merged_digital_inputs: ZERO_DIGITAL_INPUT_GROUP,
            analog_inputs: [ZERO_ANALOG_INPUT_GROUP; 4],
            pad_status_groups: [ZERO_PAD_STATUS_GROUP; 4],
            original_inputs: [ZERO_PAD_STATUS; 4],
            analog_state: todo!(),

            dir_down_time: todo!(),
        };
    }

    fn get_merged_stick(&self) -> StickState {
        StickState {
            x: self.analog_state.stick_x,
            y: self.analog_state.stick_y,
        }
    }

    fn get_merged_raw_stick(&self) -> StickState {
        StickState {
            x: self.analog_state.raw_stick_x,
            y: self.analog_state.raw_stick_y,
        }
    }

    fn get_merged_substick(&self) -> StickState {
        StickState {
            x: self.analog_state.substick_x,
            y: self.analog_state.substick_y,
        }
    }

    fn get_merged_triggers(&self) -> TriggerState {
        TriggerState {
            l: self.analog_state.trigger_l,
            r: self.analog_state.trigger_r,
        }
    }

    fn on_PADRead(&mut self, statuses: &[mkb::PADStatus]) {
        self.original_inputs.copy_from_slice(statuses);
    }

    fn button_down(&self, digital_input: mkb::PadDigitalInput, priority: bool) -> bool {
        (!self.exclusive_mode || priority) && (self.merged_digital_inputs.raw & digital_input != 0)
    }

    fn button_pressed(&self, digital_input: mkb::PadDigitalInput, priority: bool) -> bool {
        (!self.exclusive_mode || priority)
            && self.merged_digital_inputs.pressed & digital_input != 0
    }

    fn button_released(&self, digital_input: mkb::PadDigitalInput, priority: bool) -> bool {
        (!self.exclusive_mode || priority)
            && self.merged_digital_inputs.released & digital_input != 0
    }

    fn button_repeat(&self, digital_input: mkb::PadDigitalInput, priority: bool) -> bool {
        (!self.exclusive_mode || priority)
            && self.merged_digital_inputs.repeated & digital_input != 0
    }

    fn analog_down(&self, analog_input: mkb::PadAnalogInput, priority: bool) -> bool {
        (!self.exclusive_mode || priority) && self.merged_analog_inputs.raw & analog_input != 0
    }

    fn analog_pressed(&self, analog_input: mkb::PadAnalogInput, priority: bool) -> bool {
        (!self.exclusive_mode || priority) && self.merged_analog_inputs.pressed & analog_input != 0
    }

    fn analog_released(&self, analog_input: mkb::PadAnalogInput, priority: bool) -> bool {
        (!self.exclusive_mode || priority) && self.merged_analog_inputs.released & analog_input != 0
    }

    fn any_input_down(&self) -> bool {
        (self.merged_analog_inputs.raw | self.merged_digital_inputs.raw) != 0
    }

    fn any_input_pressed(&self) -> bool {
        (self.merged_analog_inputs.pressed | self.merged_digital_inputs.pressed) != 0
    }

    fn update_konami(&mut self) {
        if self.konami_progress >= 11 {
            self.konami_progress = 0;
        }

        if self.konami_input_prev_tick && self.any_input_down() {
            return;
        }

        if !self.any_input_pressed() {
            self.konami_input_prev_tick = false;
            return;
        }

        self.konami_input_prev_tick = true;
        match self.konami_progress {
            0 | 1 => {
                if self.dir_pressed(Dir::Up, true) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            2 | 3 => {
                if self.dir_pressed(Dir::Down, true) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            4 | 6 => {
                if self.dir_pressed(Dir::Left, true) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            5 | 7 => {
                if self.dir_pressed(Dir::Right, true) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            8 => {
                if self.button_pressed(mkb::PAD_BUTTON_B as mkb::PadDigitalInput, true) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            9 => {
                if self.button_pressed(mkb::PAD_BUTTON_A as mkb::PadDigitalInput, true) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            10 => {
                if self.button_pressed(mkb::PAD_BUTTON_START as mkb::PadDigitalInput, true) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            _ => {
                self.konami_progress = 0;
            }
        }
    }

    fn button_chord_pressed(
        &self,
        btn1: mkb::PadDigitalInput,
        btn2: mkb::PadDigitalInput,
        priority: bool,
    ) -> bool {
        (self.button_down(btn1, priority) && self.button_pressed(btn2, priority))
            || (self.button_pressed(btn1, priority) && self.button_down(btn2, priority))
    }

    fn analog_chord_pressed(
        &self,
        analog1: mkb::PadAnalogInput,
        analog2: mkb::PadAnalogInput,
        priority: bool,
    ) -> bool {
        (self.analog_down(analog1, priority) && self.analog_pressed(analog2, priority))
            || (self.analog_pressed(analog1, priority) && self.analog_down(analog2, priority))
    }

    fn get_cstick_dir(&self, priority: bool) -> Dir {
        let left = self.analog_down(mkb::PAI_CSTICK_LEFT as mkb::PadAnalogInput, priority);
        let right = self.analog_down(mkb::PAI_CSTICK_RIGHT as mkb::PadAnalogInput, priority);
        let up = self.analog_down(mkb::PAI_CSTICK_UP as mkb::PadAnalogInput, priority);
        let down = self.analog_down(mkb::PAI_CSTICK_DOWN as mkb::PadAnalogInput, priority);

        if up && left {
            return Dir::UpLeft;
        }
        if up && right {
            return Dir::UpRight;
        }
        if down && left {
            return Dir::DownLeft;
        }
        if down && right {
            return Dir::DownRight;
        }
        if up {
            return Dir::Up;
        }
        if down {
            return Dir::Down;
        }
        if left {
            return Dir::Left;
        }
        if right {
            return Dir::Right;
        }
        Dir::None
    }

    fn dir_down(&self, dir: Dir, priority: bool) -> bool {
        match dir {
            Dir::Up => {
                self.button_down(mkb::PAD_BUTTON_UP as mkb::PadDigitalInput, priority)
                    || self.analog_down(mkb::PAI_LSTICK_UP as mkb::PadAnalogInput, priority)
            }
            Dir::Left => {
                self.button_down(mkb::PAD_BUTTON_LEFT as mkb::PadDigitalInput, priority)
                    || self.analog_down(mkb::PAI_LSTICK_LEFT as mkb::PadAnalogInput, priority)
            }
            Dir::Right => {
                self.button_down(mkb::PAD_BUTTON_RIGHT as mkb::PadDigitalInput, priority)
                    || self.analog_down(mkb::PAI_LSTICK_RIGHT as mkb::PadAnalogInput, priority)
            }
            Dir::Down => {
                self.button_down(mkb::PAD_BUTTON_DOWN as mkb::PadDigitalInput, priority)
                    || self.analog_down(mkb::PAI_LSTICK_DOWN as mkb::PadAnalogInput, priority)
            }
            _ => false,
        }
    }

    fn dir_pressed(&self, dir: Dir, priority: bool) -> bool {
        match dir {
            Dir::Up => {
                self.button_pressed(mkb::PAD_BUTTON_UP as mkb::PadDigitalInput, priority)
                    || self.analog_pressed(mkb::PAI_LSTICK_UP as mkb::PadAnalogInput, priority)
            }
            Dir::Left => {
                self.button_pressed(mkb::PAD_BUTTON_LEFT as mkb::PadDigitalInput, priority)
                    || self.analog_pressed(mkb::PAI_LSTICK_LEFT as mkb::PadAnalogInput, priority)
            }
            Dir::Right => {
                self.button_pressed(mkb::PAD_BUTTON_RIGHT as mkb::PadDigitalInput, priority)
                    || self.analog_pressed(mkb::PAI_LSTICK_RIGHT as mkb::PadAnalogInput, priority)
            }
            Dir::Down => {
                self.button_pressed(mkb::PAD_BUTTON_DOWN as mkb::PadDigitalInput, priority)
                    || self.analog_pressed(mkb::PAI_LSTICK_DOWN as mkb::PadAnalogInput, priority)
            }
            _ => false,
        }
    }

    fn dir_repeat(&self, dir: Dir, priority: bool) -> bool {
        if self.exclusive_mode && !priority {
            return false;
        }

        let t = self.dir_down_time[dir as usize];
        self.dir_pressed(dir, priority)
            || (t as u32 >= DIR_REPEAT_WAIT
                && ((t as u32 - DIR_REPEAT_WAIT) % DIR_REPEAT_PERIOD) == 0)
    }

    fn reset_dir_repeat(&mut self) {
        self.dir_down_time = [0; 8];
    }

    fn konami_pressed(&self) -> bool {
        self.konami_progress == 11
    }

    fn set_exclusive_mode(&mut self, enabled: bool) {
        self.exclusive_mode_request = enabled;
    }

    fn get_exclusive_mode(&self) -> bool {
        self.exclusive_mode
    }

    fn on_frame_start(&mut self) {
        unsafe {
            if self.exclusive_mode {
                // Restore previous controller inputs so new inputs can be computed correctly by the game
                mkb::merged_analog_inputs = self.merged_analog_inputs;
                mkb::merged_digital_inputs = self.merged_digital_inputs;
                mkb::pad_status_groups.copy_from_slice(&self.pad_status_groups);
                mkb::analog_inputs.copy_from_slice(&self.analog_inputs);
            }

            // Only now do we honor the request to change into/out of exclusive mode
            self.exclusive_mode = self.exclusive_mode_request;
        }
    }

    fn update_dir_times(&mut self) {
        for dir in [
            Dir::Up,
            Dir::UpRight,
            Dir::Right,
            Dir::DownRight,
            Dir::Down,
            Dir::DownLeft,
            Dir::Left,
            Dir::UpLeft,
        ] {
            if self.dir_down(dir, true) {
                let dir_time = &mut self.dir_down_time[dir as usize];
                *dir_time = (*dir_time).saturating_add(1);
                if *dir_time == 120 {
                    *dir_time = 120 - DIR_REPEAT_PERIOD as u8;
                }
            } else {
                let dir_time = &mut self.dir_down_time[dir as usize];
                *dir_time = 0;
            }
        }
    }

    fn tick(&mut self) {
        unsafe {
            self.merged_analog_inputs = mkb::merged_analog_inputs;
            self.merged_digital_inputs = mkb::merged_digital_inputs;
            self.pad_status_groups
                .copy_from_slice(&mkb::pad_status_groups);
            self.analog_inputs.copy_from_slice(&mkb::analog_inputs);

            self.analog_state = AnalogState::default();
            if self.exclusive_mode {
                // Zero controller inputs in the game
                mkb::merged_analog_inputs = mkb::AnalogInputGroup::default();
                mkb::merged_digital_inputs = mkb::DigitalInputGroup::default();
                mkb::pad_status_groups = [mkb::PadStatusGroup::default(); 4];
                mkb::analog_inputs = [mkb::AnalogInputGroup::default(); 4];
            } else {
                // update analog state
                for i in 0..mkb::pad_status_groups.len() {
                    let status = &mkb::pad_status_groups[i].raw;
                    if self.original_inputs[i].err == mkb::PAD_ERR_NONE as mkb::PadError {
                        self.analog_state.raw_stick_x += self.original_inputs[i].stickX as i32;
                        self.analog_state.raw_stick_y += self.original_inputs[i].stickY as i32;
                    }
                    if status.err == mkb::PAD_ERR_NONE as mkb::PadError {
                        self.analog_state.stick_x += status.stickX as i32;
                        self.analog_state.stick_y += status.stickY as i32;
                        self.analog_state.substick_x += status.substickX as i32;
                        self.analog_state.substick_y += status.substickY as i32;
                        self.analog_state.trigger_l += status.triggerLeft as i32;
                        self.analog_state.trigger_r += status.triggerRight as i32;
                    }
                }
                self.analog_state.raw_stick_x = self.analog_state.raw_stick_x.clamp(-128, 127);
                self.analog_state.raw_stick_y = self.analog_state.raw_stick_y.clamp(-128, 127);
                self.analog_state.stick_x = self.analog_state.stick_x.clamp(-MAX_STICK, MAX_STICK);
                self.analog_state.stick_y = self.analog_state.stick_y.clamp(-MAX_STICK, MAX_STICK);
                self.analog_state.substick_x =
                    self.analog_state.substick_x.clamp(-MAX_STICK, MAX_STICK);
                self.analog_state.substick_y =
                    self.analog_state.substick_y.clamp(-MAX_STICK, MAX_STICK);
                self.analog_state.trigger_l = self.analog_state.trigger_l.clamp(0, MAX_TRIGGER);
                self.analog_state.trigger_r = self.analog_state.trigger_r.clamp(0, MAX_TRIGGER);
            }

            self.update_konami();
            self.update_dir_times();
        }
    }
}
