extern crate alloc;

use crate::pref::{BoolPref, U8Pref};

struct Color {
    r: u8,
    g: u8,
    b: u8,
}

enum AfterPush {
    None,
    CloseMenu,
    GoBack,
}

enum Widget {
    Text {
        label: &'static str,
    },
    ColoredText {
        label: &'static str,
        color: Color,
    },
    Header {
        label: &'static str,
    },
    Menu {
        label: &'static str,
        widgets: &'static [Widget],
    },
    Checkbox {
        label: &'static str,
        pref: BoolPref,
    },
    FloatView {
        label: &'static str,
        // TODO take state we can derive float from
        get: fn() -> f32,
    },
    Choose {
        label: &'static str,
        choices: &'static [&'static str],
        pref: U8Pref,
    },
    Button {
        label: &'static str,
        push: fn(),
        after: AfterPush,
    },
    IntEdit {
        label: &'static str,
        // TODO pref ID
        min: u8,
        max: u8,
    },
    FloatEdit {
        label: &'static str,
        pref: U8Pref,
        precision: u32, // denominator, 100
        min: u8,
        max: u8,
        floor: i32,
        decimals: u8,
    },
    InputSelect {
        label: &'static str,
        pref: U8Pref,
        required_chord: bool,
        can_unbind: bool,
    },
    HideableGroup {
        widgets: &'static [Widget],
        show_if: fn() -> bool,
    },
}
