use arrayvec::ArrayString;

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum EncodingType {
    SinglePress,
    ChordPress,
    Invalid,
}

pub struct Binds {}

impl Binds {
    pub fn new() -> Self {
        Self {}
    }

    pub fn bind_pressed(&self, bind_id: u8, priority: bool) -> bool {
        false
    }

    pub fn bind_down(&self, bind_id: u8, priority: bool) -> bool {
        false
    }

    pub fn get_encoding_type(&self) -> EncodingType {
        EncodingType::Invalid
    }

    pub fn get_current_encoding(&self) -> u8 {
        0
    }

    pub fn get_bind_str<const N: usize>(&self, bind_id: u8, buf: &mut ArrayString<N>) {}

    pub fn init(&mut self) {}

    pub fn tick(&mut self) {}
}
