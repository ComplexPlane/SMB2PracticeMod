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

#[derive(Default, Clone, Copy)]
pub struct StickState {
    pub x: i32,
    pub y: i32,
}

#[derive(Default, Clone, Copy)]
pub struct TriggerState {
    pub l: i32,
    pub r: i32,
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

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Default)]
pub enum Prio {
    #[default]
    Low,
    High,
}

pub const MAX_STICK: i32 = 60;
pub const MAX_TRIGGER: i32 = 128;

const DIR_REPEAT_PERIOD: u32 = 3;
const DIR_REPEAT_WAIT: u32 = 14;

#[derive(Default)]
pub struct Pad {
    analog_state: AnalogState,
    konami_progress: i32,
    konami_input_prev_tick: bool,
    curr_priority: Prio,
    priority_request: Prio,

    merged_analog_inputs: mkb::AnalogInputGroup,
    merged_digital_inputs: mkb::DigitalInputGroup,
    pad_status_groups: [mkb::PadStatusGroup; 4],
    original_inputs: [mkb::PADStatus; 4],
    analog_inputs: [mkb::AnalogInputGroup; 4],

    dir_down_time: [u8; 8],
}

impl Pad {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn get_merged_stick(&self) -> StickState {
        StickState {
            x: self.analog_state.stick_x,
            y: self.analog_state.stick_y,
        }
    }

    pub fn get_merged_raw_stick(&self) -> StickState {
        StickState {
            x: self.analog_state.raw_stick_x,
            y: self.analog_state.raw_stick_y,
        }
    }

    pub fn get_merged_substick(&self) -> StickState {
        StickState {
            x: self.analog_state.substick_x,
            y: self.analog_state.substick_y,
        }
    }

    pub fn get_merged_triggers(&self) -> TriggerState {
        TriggerState {
            l: self.analog_state.trigger_l,
            r: self.analog_state.trigger_r,
        }
    }

    pub fn on_padread(&mut self, statuses: &[mkb::PADStatus; 4]) {
        self.original_inputs = *statuses;
    }

    pub fn button_down(&self, digital_input: mkb::PadDigitalInput, priority: Prio) -> bool {
        (priority >= self.curr_priority) && (self.merged_digital_inputs.raw & digital_input != 0)
    }

    pub fn button_pressed(&self, digital_input: mkb::PadDigitalInput, priority: Prio) -> bool {
        (priority >= self.curr_priority) && self.merged_digital_inputs.pressed & digital_input != 0
    }

    pub fn button_released(&self, digital_input: mkb::PadDigitalInput, priority: Prio) -> bool {
        (priority >= self.curr_priority) && self.merged_digital_inputs.released & digital_input != 0
    }

    pub fn button_repeat(&self, digital_input: mkb::PadDigitalInput, priority: Prio) -> bool {
        (priority >= self.curr_priority) && self.merged_digital_inputs.repeated & digital_input != 0
    }

    pub fn analog_down(&self, analog_input: mkb::PadAnalogInput, priority: Prio) -> bool {
        (priority >= self.curr_priority) && self.merged_analog_inputs.raw & analog_input != 0
    }

    pub fn analog_pressed(&self, analog_input: mkb::PadAnalogInput, priority: Prio) -> bool {
        (priority >= self.curr_priority) && self.merged_analog_inputs.pressed & analog_input != 0
    }

    pub fn analog_released(&self, analog_input: mkb::PadAnalogInput, priority: Prio) -> bool {
        (priority >= self.curr_priority) && self.merged_analog_inputs.released & analog_input != 0
    }

    pub fn any_input_down(&self) -> bool {
        (self.merged_analog_inputs.raw | self.merged_digital_inputs.raw) != 0
    }

    pub fn any_input_pressed(&self) -> bool {
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
                if self.dir_pressed(Dir::Up, Prio::High) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            2 | 3 => {
                if self.dir_pressed(Dir::Down, Prio::High) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            4 | 6 => {
                if self.dir_pressed(Dir::Left, Prio::High) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            5 | 7 => {
                if self.dir_pressed(Dir::Right, Prio::High) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            8 => {
                if self.button_pressed(mkb::PAD_BUTTON_B as mkb::PadDigitalInput, Prio::High) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            9 => {
                if self.button_pressed(mkb::PAD_BUTTON_A as mkb::PadDigitalInput, Prio::High) {
                    self.konami_progress += 1;
                } else {
                    self.konami_progress = 0;
                }
            }
            10 => {
                if self.button_pressed(mkb::PAD_BUTTON_START as mkb::PadDigitalInput, Prio::High) {
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

    pub fn button_chord_pressed(
        &self,
        btn1: mkb::PadDigitalInput,
        btn2: mkb::PadDigitalInput,
        priority: Prio,
    ) -> bool {
        (self.button_down(btn1, priority) && self.button_pressed(btn2, priority))
            || (self.button_pressed(btn1, priority) && self.button_down(btn2, priority))
    }

    pub fn analog_chord_pressed(
        &self,
        analog1: mkb::PadAnalogInput,
        analog2: mkb::PadAnalogInput,
        priority: Prio,
    ) -> bool {
        (self.analog_down(analog1, priority) && self.analog_pressed(analog2, priority))
            || (self.analog_pressed(analog1, priority) && self.analog_down(analog2, priority))
    }

    pub fn get_cstick_dir(&self, priority: Prio) -> Dir {
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

    pub fn dir_down(&self, dir: Dir, priority: Prio) -> bool {
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

    pub fn dir_pressed(&self, dir: Dir, priority: Prio) -> bool {
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

    pub fn dir_repeat(&self, dir: Dir, priority: Prio) -> bool {
        if priority < self.curr_priority {
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

    pub fn konami_pressed(&self) -> bool {
        self.konami_progress == 11
    }

    pub fn set_exclusive_mode(&mut self, priority: Prio) {
        self.priority_request = priority;
    }

    pub fn get_priority(&self) -> Prio {
        self.curr_priority
    }

    pub fn on_frame_start(&mut self) {
        unsafe {
            if self.curr_priority == Prio::High {
                // Restore previous controller inputs so new inputs can be computed correctly by the game
                mkb::merged_analog_inputs = self.merged_analog_inputs;
                mkb::merged_digital_inputs = self.merged_digital_inputs;
                mkb::pad_status_groups = self.pad_status_groups;
                mkb::analog_inputs = self.analog_inputs;
            }

            // Only now do we honor the request to change into/out of exclusive mode
            self.curr_priority = self.priority_request;
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
            if self.dir_down(dir, Prio::High) {
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

    pub fn tick(&mut self) {
        unsafe {
            self.merged_analog_inputs = mkb::merged_analog_inputs;
            self.merged_digital_inputs = mkb::merged_digital_inputs;
            self.pad_status_groups = mkb::pad_status_groups;
            self.analog_inputs = mkb::analog_inputs;

            self.analog_state = AnalogState::default();
            if self.curr_priority == Prio::High {
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