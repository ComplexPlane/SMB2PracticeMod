extern crate alloc;

use core::cell::Cell;

use critical_section::Mutex;

use crate::mkb;
use crate::mods::ballcolor;
use crate::systems::pref::{BoolPref, U8Pref};
use crate::utils::tinymap::TinyMapBuilder;

use super::draw::{self, Draw};
use super::pref::Pref;

struct MenuContext<'a> {
    pref: &'a mut Pref,
    // physics: &'a mut Physics,
    // cmseg
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
        color: mkb::GXColor,
    },
    Header {
        label: &'static str,
    },
    Menu {
        label: &'static str,
        widgets: &'static [Widget],
        // It's too convenient to store currently selected menu entry in the widget itself,
        // even if it violates the otherwise immutable nature of the menu definition
        selected_idx: &'static Mutex<Cell<u32>>,
    },
    Checkbox {
        label: &'static str,
        pref: BoolPref,
    },
    GetSetCheckbox {
        label: &'static str,
        get: fn(&MenuContext) -> bool,
        set: fn(&MenuContext, bool),
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
        push: fn(&MenuContext),
        after: AfterPush,
    },
    IntEdit {
        label: &'static str,
        pref: U8Pref,
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
        show_if: fn(&MenuContext) -> bool,
    },
    Custom {
        draw: fn(&MenuContext),
    },
    Separator {},
}

static CAMERA_OPTIONS: &[&str] = &["Default", "Force SMB2", "Force SMB1"];

static INPUT_PRESET: &[Widget] = &[Widget::Choose {
    label: "Preset Color",
    choices: &[
        "Purple", "Red", "Orange", "Yellow", "Green", "Blue", "Pink", "Black",
    ],
    pref: U8Pref::InputDispColor,
}];

static INPUT_HEX: &[Widget] = &[
    Widget::IntEdit {
        label: "Red Value",
        pref: U8Pref::InputDispRed,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
    },
    Widget::IntEdit {
        label: "Green Value",
        pref: U8Pref::InputDispGreen,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
    },
    Widget::IntEdit {
        label: "Blue Value",
        pref: U8Pref::InputDispBlue,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
    },
];

static INPUTDISP_SUBWIDGETS: &[Widget] = &[
    Widget::Checkbox {
        label: "Use Center Location",
        pref: BoolPref::InputDispCenterLocation,
    },
    Widget::Checkbox {
        label: "Notch Indicators",
        pref: BoolPref::InputDispNotchIndicators,
    },
    Widget::Checkbox {
        label: "Raw Stick Inputs",
        pref: BoolPref::InputDispRawStickInputs,
    },
    Widget::Choose {
        label: "Color Type",
        choices: &["Preset", "RGB Selector", "Rainbow", "Match Ball"],
        pref: U8Pref::InputDispColorType,
    },
    Widget::HideableGroup {
        widgets: INPUT_PRESET,
        show_if: |cx| cx.pref.get_u8(U8Pref::InputDispColorType) == 0,
    },
    Widget::HideableGroup {
        widgets: INPUT_HEX,
        show_if: |cx| cx.pref.get_u8(U8Pref::InputDispColorType) == 1,
    },
];

static INPUTDISP_WIDGETS: &[Widget] = &[
    Widget::Checkbox {
        label: "Show Input Display",
        pref: BoolPref::InputDisp,
    },
    Widget::HideableGroup {
        widgets: INPUTDISP_SUBWIDGETS,
        show_if: |cx| cx.pref.get_bool(BoolPref::InputDisp),
    },
];

static BALL_COLORS: &[&str] = &[
    "Default", "Red", "Blue", "Yellow", "Green", "Teal", "Pink", "Black", "White",
];

static PRESET_WIDGETS: &[Widget] = &[Widget::Choose {
    label: "Preset Color",
    choices: BALL_COLORS,
    pref: U8Pref::BallColor,
}];

static PRESET_APE_WIDGETS: &[Widget] = &[Widget::Choose {
    label: "Preset Color",
    choices: BALL_COLORS,
    pref: U8Pref::ApeColor,
}];

static HEX_WIDGETS: &[Widget] = &[
    Widget::IntEdit {
        label: "Red Value",
        pref: U8Pref::BallRed,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
    },
    Widget::IntEdit {
        label: "Green Value",
        pref: U8Pref::BallGreen,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
    },
    Widget::IntEdit {
        label: "Blue Value",
        pref: U8Pref::BallBlue,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
    },
];

static MONKEY_TYPES: &[&str] = &["Default", "Aiai", "Meemee", "Baby", "Gongon", "Random"];

static BALL_COLOR_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Ball Color",
    },
    Widget::Choose {
        label: "Ball Color Type",
        choices: &["Preset", "RGB Selector", "Rainbow", "Random"],
        pref: U8Pref::BallColorType,
    },
    Widget::HideableGroup {
        widgets: PRESET_WIDGETS,
        show_if: |cx| cx.pref.get_u8(U8Pref::BallColorType) == 0,
    },
    Widget::HideableGroup {
        widgets: HEX_WIDGETS,
        show_if: |cx| cx.pref.get_u8(U8Pref::BallColorType) == 1,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Clothing Color",
    },
    Widget::Choose {
        label: "Clothing Color Type",
        choices: &["Preset", "Random"],
        pref: U8Pref::ApeColorType,
    },
    Widget::HideableGroup {
        widgets: PRESET_APE_WIDGETS,
        show_if: |cx| cx.pref.get_u8(U8Pref::ApeColorType) == 0,
    },
    Widget::Separator {},
    Widget::Header { label: "Monkey" },
    Widget::Choose {
        label: "Monkey Type",
        choices: MONKEY_TYPES,
        pref: U8Pref::MonkeyType,
    },
];

static IL_BATTLE_SCORE_WIDGETS: &[Widget] = &[Widget::Choose {
    label: "Score Breakdown",
    choices: &["Off", "Minimal", "Full"],
    pref: U8Pref::IlBattleBreakdown,
}];

static IL_BATTLE_SUBWIDGETS: &[Widget] = &[
    Widget::Choose {
        label: "Battle Length",
        choices: &["5 min", "7 min", "10 min", "Endless"],
        pref: U8Pref::IlBattleLength,
    },
    Widget::InputSelect {
        label: "Ready Bind",
        pref: U8Pref::IlBattleReadyBind,
        required_chord: false,
        can_unbind: true,
    },
    Widget::Text {
        label: "  Press Ready Bind then Retry to start a battle",
    },
    Widget::Separator {},
    Widget::Header {
        label: "Main Displays",
    },
    Widget::Checkbox {
        label: "Show Time",
        pref: BoolPref::IlBattleShowTime,
    },
    Widget::Checkbox {
        label: "Show Score",
        pref: BoolPref::IlBattleShowScore,
    },
    Widget::HideableGroup {
        widgets: IL_BATTLE_SCORE_WIDGETS,
        show_if: |cx| cx.pref.get_bool(BoolPref::IlBattleShowScore),
    },
    Widget::Separator {},
    Widget::Header {
        label: "Extra Displays",
    },
    Widget::Checkbox {
        label: "Show Tie Count",
        pref: BoolPref::IlBattleTieCount,
    },
    Widget::Checkbox {
        label: "Show Attempts",
        pref: BoolPref::IlBattleAttemptCount,
    },
    Widget::Checkbox {
        label: "Buzzer Message",
        pref: BoolPref::IlBattleBuzzerOld,
    },
];

static IL_BATTLE_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Battle Tracker",
    },
    Widget::Checkbox {
        label: "IL Battle Display",
        pref: BoolPref::IlBattleDisplay,
    },
    Widget::HideableGroup {
        widgets: IL_BATTLE_SUBWIDGETS,
        show_if: |cx| cx.pref.get_bool(BoolPref::IlBattleDisplay),
    },
];

fn rumble_get(controller_idx: usize) -> bool {
    unsafe { mkb::rumble_enabled_bitflag & (1 << controller_idx) != 0 }
}

fn rumble_set(controller_idx: usize, value: bool) {
    unsafe {
        if value {
            mkb::rumble_enabled_bitflag |= 1 << controller_idx;
        } else {
            mkb::rumble_enabled_bitflag &= !(1 << controller_idx);
        }
    }
}

static RUMBLE_WIDGETS: &[Widget] = &[
    Widget::GetSetCheckbox {
        label: "Controller 1 Rumble",
        get: |_| rumble_get(0),
        set: |_, enable| rumble_set(0, enable),
    },
    Widget::GetSetCheckbox {
        label: "Controller 2 Rumble",
        get: |_| rumble_get(1),
        set: |_, enable| rumble_set(1, enable),
    },
    Widget::GetSetCheckbox {
        label: "Controller 3 Rumble",
        get: |_| rumble_get(2),
        set: |_, enable| rumble_set(2, enable),
    },
    Widget::GetSetCheckbox {
        label: "Controller 4 Rumble",
        get: |_| rumble_get(3),
        set: |_, enable| rumble_set(3, enable),
    },
];

static ABOUT_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "SMB2 Practice Mod",
    },
    Widget::Text {
        label: "  Made with   by ComplexPlane",
    },
    Widget::Custom {
        draw: |_| draw::heart(),
    },
    Widget::Text {
        label: "  With contributions from rehtrop & Nambo",
    },
    Widget::Separator {},
    Widget::Header { label: "Updates" },
    Widget::Text {
        // TODO
        label: "TODO: version info",
        // label: s_version_str,
    },
    Widget::Text {
        label: "  For the latest version of SMB2 Practice Mod:",
    },
    Widget::ColoredText {
        label: " github.com/ComplexPlane/SMB2PracticeMod/releases",
        color: draw::BLUE,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Documentation",
    },
    Widget::Text {
        label: "  For full documentation of SMB2 Practice Mod:",
    },
    Widget::ColoredText {
        label: "  github.com/ComplexPlane/SMB2PracticeMod/wiki",
        color: draw::BLUE,
    },
];

static SAVESTATES_HELP_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  X          \x1c Create savestate",
    },
    Widget::Text {
        label: "  Y          \x1c Load savestate",
    },
    Widget::Text {
        label: "  C-Stick    \x1c Change savestate slot",
    },
    // TODO: replace this feature with a better one that works in-menu
    Widget::Text {
        label: "  L+X or R+X \x1c Frame advance",
    },
    Widget::Text {
        label: "  L+C or R+C \x1c Browse savestates",
    },
];

static IW_HELP_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  Story Mode individual world segments can",
    },
    Widget::Text {
        label: "  be done by going to file select in Story",
    },
    Widget::Text {
        label: "  Mode, pressing up/down on an empty file",
    },
    Widget::Text {
        label: "  to select a world, and selecting the file.",
    },
];

static JUMP_HELP_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  A          \x1c Jump",
    },
    Widget::Text {
        label: "  B          \x1c Resize minimap",
    },
];

static FREECAM_HELP_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  Left Stick \x1c Move",
    },
    Widget::Text {
        label: "  C-Stick    \x1c Rotate",
    },
    Widget::Text {
        label: "  L          \x1c Down",
    },
    Widget::Text {
        label: "  R          \x1c Up",
    },
    Widget::Text {
        label: "  Y          \x1c Turbo Speed",
    },
    Widget::Text {
        label: "  X          \x1c Slow Speed",
    },
    Widget::Text {
        label: "  D-pad Up   \x1c Increase Turbo Speed",
    },
    Widget::Text {
        label: "  D-pad Down \x1c Decrease Turbo Speed",
    },
];

static IW_MARK_HELP_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  The IL validation mark is shown after",
    },
    Widget::Text {
        label: "  completing a level. It is green only if no",
    },
    Widget::Text {
        label: "  gameplay-altering mods were used during the",
    },
    Widget::Text {
        label: "  level completion, and red otherwise.",
    },
    Widget::Separator {},
    Widget::Text {
        label: "  Showing this watermark is currently required",
    },
    Widget::Text {
        label: "  to submit IL speedruns of vanilla SMB2 to the",
    },
    Widget::Text {
        label: "  official leaderboards if you are using the",
    },
    Widget::Text {
        label: "  SMB2 Practice Mod.",
    },
    Widget::Separator {},
    Widget::Text {
        label: "  Note that some visual-only mods, may also be",
    },
    Widget::Text {
        label: "  disallowed for for IL leaderboard submissions.",
    },
    Widget::Text {
        label: "  The IL validation does not account for these",
    },
    Widget::Text {
        label: "  visual-only mods.",
    },
    Widget::Separator {},
    Widget::Text {
        label: "  If you complete a stage without the watermark",
    },
    Widget::Text {
        label: "  enabled, you can still enable it post-goal to",
    },
    Widget::Text {
        label: "  validate your run.",
    },
];

static CHARA_CHOICES: &[&str] = &["AiAi", "MeeMee", "Baby", "GonGon", "Random"];

static CM_BEG_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Segment Selection",
    },
    Widget::Button {
        label: "Beginner 1-10",
        // TODO
        // push: || cmseg::request_cm_seg(cmseg::Seg::Beginner1),
        push: |_| {},
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Beginner Extra 1-10",
        // TODO
        // push: || cmseg::request_cm_seg(cmseg::Seg::BeginnerExtra),
        push: |_| {},
        after: AfterPush::CloseMenu,
    },
];

static CM_ADV_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Segment Selection",
    },
    Widget::Button {
        label: "Advanced 1-10",
        // TODO
        // push: || cmseg::request_cm_seg(cmseg::Seg::Advanced1),
        push: |_| {},
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Advanced 11-20",
        // TODO
        // push: || cmseg::request_cm_seg(cmseg::Seg::Advanced11),
        push: |_| {},
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Advanced 21-30",
        // TODO
        // push: || cmseg::request_cm_seg(cmseg::Seg::Advanced21),
        push: |_| {},
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Advanced Extra 1-10",
        // TODO
        // push: || cmseg::request_cm_seg(cmseg::Seg::AdvancedExtra),
        push: |_| {},
        after: AfterPush::CloseMenu,
    },
];

static CM_EXP_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Segment Selection",
    },
    Widget::Button {
        label: "Expert 1-10",
        push: |_| {}, // TODO: Implement cmseg::request_cm_seg(cmseg::Seg::Expert1)
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert 11-20",
        push: |_| {}, // TODO: Implement cmseg::request_cm_seg(cmseg::Seg::Expert11)
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert 21-30",
        push: |_| {}, // TODO: Implement cmseg::request_cm_seg(cmseg::Seg::Expert21)
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert 31-40",
        push: |_| {}, // TODO: Implement cmseg::request_cm_seg(cmseg::Seg::Expert31)
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert 41-50",
        push: |_| {}, // TODO: Implement cmseg::request_cm_seg(cmseg::Seg::Expert41)
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert Extra 1-10",
        push: |_| {}, // TODO: Implement cmseg::request_cm_seg(cmseg::Seg::ExpertExtra)
        after: AfterPush::CloseMenu,
    },
];
