use mkb::mkb;

use arrayvec::ArrayString;
use num_enum::TryFromPrimitive;

use crate::mods::cmseg::{CmSeg, Seg};
use crate::mods::gotostory::GoToStory;
use crate::mods::stage_edits::StageEdits;
use crate::mods::unlock::Unlock;
use crate::mods::validate::Validate;
use crate::mods::{ballcolor, freecam};
use crate::systems::pref::{BoolPref, I16Pref};
use crate::utils::version;
use crate::{cstr_buf, fmt_buf};

use super::binds::Binds;
use super::draw::{self, Draw};
use super::pad::Pad;
use super::pref::{FromPref, Pref};

pub struct MenuContext<'a> {
    pub pad: &'a mut Pad,
    pub pref: &'a mut Pref,
    pub draw: &'a mut Draw,
    pub binds: &'a mut Binds,
    pub cm_seg: &'a mut CmSeg,
    pub go_to_story: &'a mut GoToStory,
    pub stage_edits: &'a mut StageEdits,
    pub unlock: &'a mut Unlock,
}

pub enum AfterPush {
    CloseMenu,
    GoBack,
}

pub enum TextLine {
    NewLine,
    Overlap,
}

#[derive(TryFromPrimitive)]
#[repr(i16)]
enum RgbFormat {
    Decimal,
    Hex,
}

fn int_edit_normal_formatter(value: i16, out_buf: &mut ArrayString<32>, _cx: &mut MenuContext) {
    fmt_buf!(out_buf, c"%d", value as i32);
}

fn int_edit_percent_formatter(value: i16, out_buf: &mut ArrayString<32>, _cx: &mut MenuContext) {
    fmt_buf!(out_buf, c"%d%%", value as i32);
}

fn int_edit_rgb_formatter(value: i16, out_buf: &mut ArrayString<32>, cx: &mut MenuContext) {
    match RgbFormat::from_pref(I16Pref::RgbFormat, cx.pref) {
        RgbFormat::Decimal => {
            fmt_buf!(out_buf, c"%d", value as i32);
        }
        RgbFormat::Hex => {
            fmt_buf!(out_buf, c"0x%02X", value as i32);
        }
    }
}

pub enum Widget {
    Text {
        label: &'static str,
    },
    TextFunc {
        label_func: fn(buf: &mut ArrayString<32>, cx: &mut MenuContext),
    },
    ColoredText {
        label: &'static str,
        color_left: mkb::GXColor,
        color_right: mkb::GXColor,
        offset_x: u8,
        line: TextLine,
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
    GetSetCheckbox {
        label: &'static str,
        get: fn(&mut MenuContext) -> bool,
        set: fn(bool, &mut MenuContext),
    },
    Choose {
        label: &'static str,
        choices: &'static [&'static str],
        pref: I16Pref,
    },
    Button {
        label: &'static str,
        push: fn(&mut MenuContext),
        after: AfterPush,
    },
    IntEdit {
        label: &'static str,
        pref: I16Pref,
        min: i16,
        max: i16,
        formatter: fn(value: i16, out_buf: &mut ArrayString<32>, cx: &mut MenuContext),
    },
    InputSelect {
        label: &'static str,
        pref: I16Pref,
        required_chord: bool,
        can_unbind: bool,
    },
    HideableGroup {
        widgets: &'static [Widget],
        show_if: fn(&mut MenuContext) -> bool,
    },
    Custom {
        draw: fn(&mut MenuContext),
    },
    RgbPreview {
        r_pref: I16Pref,
        g_pref: I16Pref,
        b_pref: I16Pref,
    },
    Separator {},
}

static CAMERA_OPTIONS: &[&str] = &["Default", "Force SMB2", "Force SMB1"];

static INPUT_PRESET: &[Widget] = &[Widget::Choose {
    label: "Preset Color",
    choices: &[
        "Purple", "Red", "Orange", "Yellow", "Green", "Blue", "Pink", "Black",
    ],
    pref: I16Pref::InputDispColor,
}];

static INPUT_RGB_SOLID: &[Widget] = &[
    Widget::RgbPreview {
        r_pref: I16Pref::InputDispRed,
        g_pref: I16Pref::InputDispGreen,
        b_pref: I16Pref::InputDispBlue,
    },
    Widget::IntEdit {
        label: "Color Red",
        pref: I16Pref::InputDispRed,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Color Green",
        pref: I16Pref::InputDispGreen,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Color Blue",
        pref: I16Pref::InputDispBlue,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
];

static INPUT_RGB_GRADIENT: &[Widget] = &[
    Widget::RgbPreview {
        r_pref: I16Pref::InputDispRed,
        g_pref: I16Pref::InputDispGreen,
        b_pref: I16Pref::InputDispBlue,
    },
    Widget::IntEdit {
        label: "Gradient Color 1 Red",
        pref: I16Pref::InputDispRed,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Gradient Color 1 Green",
        pref: I16Pref::InputDispGreen,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Gradient Color 1 Blue",
        pref: I16Pref::InputDispBlue,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::RgbPreview {
        r_pref: I16Pref::InputDispGradientColor2Red,
        g_pref: I16Pref::InputDispGradientColor2Green,
        b_pref: I16Pref::InputDispGradientColor2Blue,
    },
    Widget::IntEdit {
        label: "Gradient Color 2 Red",
        pref: I16Pref::InputDispGradientColor2Red,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Gradient Color 2 Green",
        pref: I16Pref::InputDispGradientColor2Green,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Gradient Color 2 Blue",
        pref: I16Pref::InputDispGradientColor2Blue,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Gradient Rotation",
        pref: I16Pref::InputDispGradientRotation,
        min: 0,
        max: 100,
        formatter: int_edit_percent_formatter,
    },
    Widget::IntEdit {
        label: "Gradient Start",
        pref: I16Pref::InputDispGradientStart,
        min: -200,
        max: 200,
        formatter: int_edit_percent_formatter,
    },
    Widget::IntEdit {
        label: "Gradient End",
        pref: I16Pref::InputDispGradientEnd,
        min: -100,
        max: 300,
        formatter: int_edit_percent_formatter,
    },
];

static INPUTDISP_SUBWIDGETS: &[Widget] = &[
    Widget::Choose {
        label: "Location",
        choices: &["Right", "Center"],
        pref: I16Pref::InputDispLocation,
    },
    Widget::Checkbox {
        label: "Notch Indicators",
        pref: BoolPref::InputDispNotchIndicators,
    },
    Widget::Checkbox {
        label: "Raw Stick Inputs",
        pref: BoolPref::InputDispRawStickInputs,
    },
    Widget::Separator {},
    Widget::Header { label: "Color" },
    Widget::Choose {
        label: "Color Type",
        choices: &[
            "Preset",
            "RGB Solid",
            "RGB Gradient",
            "Rainbow",
            "Match Ball",
        ],
        pref: I16Pref::InputDispColorType,
    },
    Widget::HideableGroup {
        widgets: INPUT_PRESET,
        show_if: |cx| cx.pref.get(I16Pref::InputDispColorType) == 0,
    },
    Widget::HideableGroup {
        widgets: INPUT_RGB_SOLID,
        show_if: |cx| cx.pref.get(I16Pref::InputDispColorType) == 1,
    },
    Widget::HideableGroup {
        widgets: INPUT_RGB_GRADIENT,
        show_if: |cx| cx.pref.get(I16Pref::InputDispColorType) == 2,
    },
];

static INPUTDISP_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Input Display",
    },
    Widget::Checkbox {
        label: "Show Input Display",
        pref: BoolPref::InputDisp,
    },
    Widget::HideableGroup {
        widgets: INPUTDISP_SUBWIDGETS,
        show_if: |cx| cx.pref.get(BoolPref::InputDisp),
    },
];

static BALL_COLORS: &[&str] = &[
    "Default", "Red", "Blue", "Yellow", "Green", "Teal", "Pink", "Black", "White",
];

static PRESET_WIDGETS: &[Widget] = &[Widget::Choose {
    label: "Preset Color",
    choices: BALL_COLORS,
    pref: I16Pref::BallColor,
}];

static PRESET_APE_WIDGETS: &[Widget] = &[Widget::Choose {
    label: "Preset Color",
    choices: BALL_COLORS,
    pref: I16Pref::ApeColor,
}];

static HEX_WIDGETS: &[Widget] = &[
    Widget::RgbPreview {
        r_pref: I16Pref::BallRed,
        g_pref: I16Pref::BallGreen,
        b_pref: I16Pref::BallBlue,
    },
    Widget::IntEdit {
        label: "Color Red",
        pref: I16Pref::BallRed,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Color Green",
        pref: I16Pref::BallGreen,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
    Widget::IntEdit {
        label: "Color Blue",
        pref: I16Pref::BallBlue,
        min: ballcolor::COLOR_MIN,
        max: ballcolor::COLOR_MAX,
        formatter: int_edit_rgb_formatter,
    },
];

static MONKEY_TYPES: &[&str] = &["Default", "Aiai", "Meemee", "Baby", "Gongon", "Random"];

static BALL_COLOR_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Ball Color",
    },
    Widget::Choose {
        label: "Ball Color Type",
        choices: &["Preset", "RGB", "Rainbow", "Random"],
        pref: I16Pref::BallColorType,
    },
    Widget::HideableGroup {
        widgets: PRESET_WIDGETS,
        show_if: |cx| cx.pref.get(I16Pref::BallColorType) == 0,
    },
    Widget::HideableGroup {
        widgets: HEX_WIDGETS,
        show_if: |cx| cx.pref.get(I16Pref::BallColorType) == 1,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Clothing Color",
    },
    Widget::Choose {
        label: "Clothing Color Type",
        choices: &["Preset", "Random"],
        pref: I16Pref::ApeColorType,
    },
    Widget::HideableGroup {
        widgets: PRESET_APE_WIDGETS,
        show_if: |cx| cx.pref.get(I16Pref::ApeColorType) == 0,
    },
    Widget::Separator {},
    Widget::Header { label: "Monkey" },
    Widget::Choose {
        label: "Monkey Type",
        choices: MONKEY_TYPES,
        pref: I16Pref::MonkeyType,
    },
];

static IL_BATTLE_SCORE_WIDGETS: &[Widget] = &[Widget::Choose {
    label: "Score Breakdown",
    choices: &["Off", "Minimal", "Full"],
    pref: I16Pref::IlBattleBreakdown,
}];

static IL_BATTLE_SUBWIDGETS: &[Widget] = &[
    Widget::Choose {
        label: "Battle Length",
        choices: &["5 min", "7 min", "10 min", "Endless"],
        pref: I16Pref::IlBattleLength,
    },
    Widget::InputSelect {
        label: "Ready Bind",
        pref: I16Pref::IlBattleReadyBind,
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
        show_if: |cx| cx.pref.get(BoolPref::IlBattleShowScore),
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
        show_if: |cx| cx.pref.get(BoolPref::IlBattleDisplay),
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
        set: |enable, _| rumble_set(0, enable),
    },
    Widget::GetSetCheckbox {
        label: "Controller 2 Rumble",
        get: |_| rumble_get(1),
        set: |enable, _| rumble_set(1, enable),
    },
    Widget::GetSetCheckbox {
        label: "Controller 3 Rumble",
        get: |_| rumble_get(2),
        set: |enable, _| rumble_set(2, enable),
    },
    Widget::GetSetCheckbox {
        label: "Controller 4 Rumble",
        get: |_| rumble_get(3),
        set: |enable, _| rumble_set(3, enable),
    },
];

static GIT_HASH_STR: &str = concat!("  Git commit:      ", env!("GIT_HASH"));

static ABOUT_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "SMB2 Practice Mod",
    },
    Widget::Text {
        label: "  Made with   by",
    },
    Widget::Custom {
        draw: |_| draw::heart(),
    },
    Widget::ColoredText {
        label: "ComplexPlane",
        color_left: mkb::GXColor {
            r: 0x00,
            g: 0xde,
            b: 0x38,
            a: 0xff,
        },
        color_right: mkb::GXColor {
            r: 0x0a,
            g: 0xf2,
            b: 0xef,
            a: 0xff,
        },
        offset_x: 17,
        line: TextLine::Overlap,
    },
    Widget::Separator {},
    Widget::Text {
        label: "  With contributions from        ,      ,",
    },
    Widget::ColoredText {
        label: "rehtrop",
        color_left: mkb::GXColor {
            r: 0x69,
            g: 0xe3,
            b: 0xff,
            a: 0xff,
        },
        color_right: mkb::GXColor {
            r: 0xdd,
            g: 0x9b,
            b: 0xdc,
            a: 0xff,
        },
        offset_x: 26,
        line: TextLine::Overlap,
    },
    Widget::ColoredText {
        label: "Nambo",
        color_left: mkb::GXColor {
            r: 0xd0,
            g: 0x67,
            b: 0xff,
            a: 0xff,
        },
        color_right: mkb::GXColor {
            r: 0xf3,
            g: 0xe3,
            b: 0xff,
            a: 0xff,
        },
        offset_x: 35,
        line: TextLine::Overlap,
    },
    Widget::Text { label: "  and" },
    Widget::ColoredText {
        label: "eddy",
        color_left: mkb::GXColor {
            r: 0x00,
            g: 0xd0,
            b: 0xff,
            a: 0xff,
        },
        color_right: mkb::GXColor {
            r: 0x00,
            g: 0xa4,
            b: 0xe5,
            a: 0xff,
        },
        offset_x: 6,
        line: TextLine::Overlap,
    },
    Widget::Separator {},
    Widget::ColoredText {
        label: "Written in Rust",
        color_left: mkb::GXColor {
            r: 0xf9,
            g: 0x6f,
            b: 0x58,
            a: 0xff,
        },
        color_right: mkb::GXColor {
            r: 0xef,
            g: 0xa7,
            b: 0x2c,
            a: 0xff,
        },
        offset_x: 2,
        line: TextLine::NewLine,
    },
    Widget::Custom {
        draw: |_| draw::tm(),
    },
    Widget::Separator {},
    Widget::Header { label: "Updates" },
    Widget::TextFunc {
        label_func: |buf, _cx| {
            let mut version_str = ArrayString::<32>::new();
            version::get_version_str(&mut version_str);
            fmt_buf!(buf, c"  Current version: v%s", cstr_buf!(version_str));
        },
    },
    Widget::Text {
        label: GIT_HASH_STR,
    },
    Widget::Separator {},
    Widget::Text {
        label: "  For the latest version of SMB2 Practice Mod:",
    },
    Widget::ColoredText {
        label: " github.com/ComplexPlane/SMB2PracticeMod/releases",
        color_left: draw::BLUE,
        color_right: draw::BLUE,
        offset_x: 0,
        line: TextLine::NewLine,
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
        color_left: draw::BLUE,
        color_right: draw::BLUE,
        offset_x: 0,
        line: TextLine::NewLine,
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
        label: "  Note that some visual-only mods may also be",
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
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Beginner1),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Beginner Extra 1-10",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::BeginnerExtra),
        after: AfterPush::CloseMenu,
    },
];

static CM_ADV_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Segment Selection",
    },
    Widget::Button {
        label: "Advanced 1-10",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Advanced1),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Advanced 11-20",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Advanced11),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Advanced 21-30",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Advanced21),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Advanced Extra 1-10",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::AdvancedExtra),
        after: AfterPush::CloseMenu,
    },
];

static CM_EXP_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Segment Selection",
    },
    Widget::Button {
        label: "Expert 1-10",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Expert1),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert 11-20",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Expert11),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert 21-30",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Expert21),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert 31-40",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Expert31),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert 41-50",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Expert41),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Expert Extra 1-10",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::ExpertExtra),
        after: AfterPush::CloseMenu,
    },
];

static CM_MAS_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Segment Selection",
    },
    Widget::Button {
        label: "Master 1-10",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::Master1),
        after: AfterPush::CloseMenu,
    },
    Widget::Button {
        label: "Master Extra 1-10",
        push: |cx| cx.cm_seg.request_cm_seg(Seg::MasterExtra),
        after: AfterPush::CloseMenu,
    },
];

static CM_SEG_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Segment Selection",
    },
    Widget::Menu {
        label: "Beginner",
        widgets: CM_BEG_WIDGETS,
    },
    Widget::Menu {
        label: "Advanced",
        widgets: CM_ADV_WIDGETS,
    },
    Widget::Menu {
        label: "Expert",
        widgets: CM_EXP_WIDGETS,
    },
    Widget::Menu {
        label: "Master",
        widgets: CM_MAS_WIDGETS,
    },
    Widget::Menu {
        label: "Story Mode",
        widgets: IW_HELP_WIDGETS,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Segment Options",
    },
    Widget::Choose {
        label: "Character",
        choices: CHARA_CHOICES,
        pref: I16Pref::CmChara,
    },
    Widget::Separator {},
    Widget::Text {
        label: "  Segments may crash in some romhacks",
    },
];

static TIMERS_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Realtime Timers",
    },
    Widget::Checkbox {
        label: "Realtime (RTA)",
        pref: BoolPref::TimerShowRTA,
    },
    Widget::Checkbox {
        label: "Pausetime (PAU)",
        pref: BoolPref::TimerShowPause,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Subtick Timers",
    },
    Widget::Checkbox {
        label: "Framesave (FSV)",
        pref: BoolPref::TimerShowFramesave,
    },
    Widget::Checkbox {
        label: "Subtick (SUB)",
        pref: BoolPref::TimerShowSubtick,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Segment Timers",
    },
    Widget::Checkbox {
        label: "Story Mode IWs (IW)",
        pref: BoolPref::IwTimer,
    },
    Widget::Checkbox {
        label: "CM Segments (SEG)",
        pref: BoolPref::CmTimer,
    },
];

static SOUND_WIDGETS: &[Widget] = &[
    Widget::Checkbox {
        label: "Mute Background Music",
        pref: BoolPref::MuteBgm,
    },
    Widget::Text {
        label: "  To apply background music setting:",
    },
    Widget::Text {
        label: "  Wait 2s then reset console",
    },
    Widget::Separator {},
    Widget::Checkbox {
        label: "Mute Timer Ding",
        pref: BoolPref::MuteTimerDing,
    },
];

static UNLOCK_CONFIRM_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  This will unlock all levels, lives, etc.",
    },
    Widget::Text {
        label: "  Save your game to make this persistent.",
    },
    Widget::Button {
        label: "Cancel",
        push: |_| {},
        after: AfterPush::GoBack,
    },
    Widget::Button {
        label: "Confirm",
        push: |cx| cx.unlock.unlock_everything(),
        after: AfterPush::GoBack,
    },
];

static UNLOCK_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Unlock Progress For This Session",
    },
    Widget::Menu {
        label: "Unlock Now",
        widgets: UNLOCK_CONFIRM_WIDGETS,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Always Unlock Progress",
    },
    Widget::Checkbox {
        label: "For Vanilla SMB2",
        pref: BoolPref::UnlockVanilla,
    },
    Widget::Checkbox {
        label: "For Romhacks",
        pref: BoolPref::UnlockRomhacks,
    },
    Widget::Text {
        label: "  Applied on game startup.",
    },
];

static FREECAM_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Freecam Toggle",
    },
    Widget::Checkbox {
        label: "Freecam",
        pref: BoolPref::Freecam,
    },
    Widget::InputSelect {
        label: "Toggle Bind",
        pref: I16Pref::FreecamToggleBind,
        required_chord: false,
        can_unbind: true,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Configuration",
    },
    Widget::Checkbox {
        label: "Invert Yaw",
        pref: BoolPref::FreecamInvertYaw,
    },
    Widget::Checkbox {
        label: "Invert Pitch",
        pref: BoolPref::FreecamInvertPitch,
    },
    Widget::IntEdit {
        label: "Turbo Speed Factor",
        pref: I16Pref::FreecamSpeedMult,
        min: freecam::TURBO_SPEED_MIN,
        max: freecam::TURBO_SPEED_MAX,
        formatter: int_edit_normal_formatter,
    },
    Widget::Checkbox {
        label: "Freeze Timer",
        pref: BoolPref::FreecamFreezeTimer,
    },
    Widget::Checkbox {
        label: "Hide HUD",
        pref: BoolPref::FreecamHideHud,
    },
    Widget::Separator {},
    Widget::Header { label: "Help" },
    Widget::Menu {
        label: "Controller Binds",
        widgets: FREECAM_HELP_WIDGETS,
    },
];

static HIDE_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Hide Elements",
    },
    Widget::Checkbox {
        label: "HUD",
        pref: BoolPref::HideHud,
    },
    Widget::Checkbox {
        label: "Ball",
        pref: BoolPref::HideBall,
    },
    Widget::Checkbox {
        label: "Stage Models",
        pref: BoolPref::HideStage,
    },
    Widget::Checkbox {
        label: "Stage Items",
        pref: BoolPref::HideItems,
    },
    Widget::Checkbox {
        label: "Stage Objects",
        pref: BoolPref::HideStobjs,
    },
    Widget::Checkbox {
        label: "Effects",
        pref: BoolPref::HideEffects,
    },
    Widget::Checkbox {
        label: "Background",
        pref: BoolPref::HideBg,
    },
];

static SAVESTATE_SUBWIDGETS: &[Widget] = &[
    Widget::Choose {
        label: "Save To",
        choices: &["Selected Slot", "Next Empty Slot", "Empty, Then Oldest"],
        pref: I16Pref::SavestateSaveTo,
    },
    Widget::InputSelect {
        label: "Clear Savestate Bind",
        pref: I16Pref::SavestateClearBind,
        required_chord: false,
        can_unbind: true,
    },
    Widget::InputSelect {
        label: "Clear All Bind",
        pref: I16Pref::SavestateClearAllBind,
        required_chord: false,
        can_unbind: true,
    },
];

static SAVESTATE_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Savestates",
    },
    Widget::Checkbox {
        label: "Enable Savestates",
        pref: BoolPref::Savestates,
    },
    Widget::HideableGroup {
        widgets: SAVESTATE_SUBWIDGETS,
        show_if: |cx| cx.pref.get(BoolPref::Savestates),
    },
    Widget::Separator {},
    Widget::Header { label: "Help" },
    Widget::Menu {
        label: "Controller Binds",
        widgets: SAVESTATES_HELP_WIDGETS,
    },
];

static TOOLS_WIDGETS: &[Widget] = &[
    Widget::Button {
        label: "Go To Story Mode",
        push: |cx| cx.go_to_story.load_storymode(),
        after: AfterPush::CloseMenu,
    },
    Widget::Menu {
        label: "Speedrun Segments",
        widgets: CM_SEG_WIDGETS,
    },
    Widget::Menu {
        label: "Savestates",
        widgets: SAVESTATE_WIDGETS,
    },
    Widget::Menu {
        label: "Freecam",
        widgets: FREECAM_WIDGETS,
    },
    Widget::Menu {
        label: "Hide Elements",
        widgets: HIDE_WIDGETS,
    },
    Widget::Menu {
        label: "Rumble",
        widgets: RUMBLE_WIDGETS,
    },
    Widget::Menu {
        label: "Audio",
        widgets: SOUND_WIDGETS,
    },
    Widget::Menu {
        label: "Progress Unlock",
        widgets: UNLOCK_WIDGETS,
    },
    Widget::Checkbox {
        label: "Debug Mode",
        pref: BoolPref::DebugMode,
    },
];

static RESET_ILMARK_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  Reset IL invalidating preferences to defaults?",
    },
    Widget::Button {
        label: "Cancel",
        push: |_| {},
        after: AfterPush::GoBack,
    },
    Widget::Button {
        label: "Confirm",
        push: |cx| Validate::disable_invalidating_settings(cx.pref),
        after: AfterPush::GoBack,
    },
];

static IL_MARK_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Disable IL Invalidating Settings",
    },
    Widget::Menu {
        label: "Disable Now",
        widgets: RESET_ILMARK_WIDGETS,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Show IL Validation Mark in...",
    },
    Widget::Checkbox {
        label: "Practice Mode",
        pref: BoolPref::IlMarkPractice,
    },
    Widget::Checkbox {
        label: "Story Mode",
        pref: BoolPref::IlMarkStory,
    },
    Widget::Checkbox {
        label: "Challenge Mode",
        pref: BoolPref::IlMarkChallenge,
    },
    Widget::Checkbox {
        label: "Romhacks",
        pref: BoolPref::IlMarkRomhacks,
    },
    Widget::Separator {},
    Widget::Header { label: "Help" },
    Widget::Menu {
        label: "About",
        widgets: IW_MARK_HELP_WIDGETS,
    },
];

static DISPLAYS_WIDGETS: &[Widget] = &[
    Widget::Menu {
        label: "Input Display",
        widgets: INPUTDISP_WIDGETS,
    },
    Widget::Menu {
        label: "Timers",
        widgets: TIMERS_WIDGETS,
    },
    Widget::Menu {
        label: "IL Battle",
        widgets: IL_BATTLE_WIDGETS,
    },
    Widget::Menu {
        label: "IL Validation Mark",
        widgets: IL_MARK_WIDGETS,
    },
    Widget::Checkbox {
        label: "9999 Banana Counter",
        pref: BoolPref::BananaCounter9999,
    },
];

static PHYSICS_PRESETS: &[&str] = &[
    "Default",
    "Light Ball",
    "No Friction",
    "Heavy Ball",
    "Bouncy Ball",
    "Sticky Ball",
    "High Friction",
    "Jump-Mod Physics",
];

static JUMP_PHYSICS: &[Widget] = &[
    Widget::Text {
        label: "  Friction: 0.010 -> 0.015",
    },
    Widget::Text {
        label: "  Restitution: 0.50 -> 0.25",
    },
];

static PHYSICS_WIDGETS: &[Widget] = &[
    Widget::Choose {
        label: "Physics Presets",
        choices: PHYSICS_PRESETS,
        pref: I16Pref::PhysicsPreset,
    },
    Widget::HideableGroup {
        widgets: &[Widget::Text {
            label: "  Weight: 1.00 -> 0.95",
        }],
        show_if: |cx| cx.pref.get(I16Pref::PhysicsPreset) == 1,
    },
    Widget::HideableGroup {
        widgets: &[Widget::Text {
            label: "  Friction: 0.010 -> 0.000",
        }],
        show_if: |cx| cx.pref.get(I16Pref::PhysicsPreset) == 2,
    },
    Widget::HideableGroup {
        widgets: &[Widget::Text {
            label: "  Weight: 1.00 -> 1.05",
        }],
        show_if: |cx| cx.pref.get(I16Pref::PhysicsPreset) == 3,
    },
    Widget::HideableGroup {
        widgets: &[Widget::Text {
            label: "  Restitution: 0.50 -> 1.20",
        }],
        show_if: |cx| cx.pref.get(I16Pref::PhysicsPreset) == 4,
    },
    Widget::HideableGroup {
        widgets: &[Widget::Text {
            label: "  Restitution: 0.50 -> 0.01",
        }],
        show_if: |cx| cx.pref.get(I16Pref::PhysicsPreset) == 5,
    },
    Widget::HideableGroup {
        widgets: &[Widget::Text {
            label: "  Friction: 0.010 -> 0.020",
        }],
        show_if: |cx| cx.pref.get(I16Pref::PhysicsPreset) == 6,
    },
    Widget::HideableGroup {
        widgets: JUMP_PHYSICS,
        show_if: |cx| cx.pref.get(I16Pref::PhysicsPreset) == 7,
    },
    Widget::Checkbox {
        label: "Display Physics Text",
        pref: BoolPref::CustomPhysicsDisp,
    },
];

static STAGE_EDIT_VARIANTS: &[&str] = &["None", "Golden Banana", "Dark Banana", "Reverse Mode"];

static REVERSE_GOAL_WIDGETS: &[Widget] = &[Widget::Button {
    label: "Select New Goal",
    push: |cx| cx.stage_edits.select_new_goal(),
    after: AfterPush::CloseMenu,
}];

static STAGE_EDIT_WIDGETS: &[Widget] = &[
    Widget::Choose {
        label: "Stage Edit Mode",
        choices: STAGE_EDIT_VARIANTS,
        pref: I16Pref::StageEditVariant,
    },
    Widget::HideableGroup {
        widgets: REVERSE_GOAL_WIDGETS,
        show_if: |cx| cx.pref.get(I16Pref::StageEditVariant) == 3,
    },
    Widget::Text {
        label: "  Stage Edits are activated on retry",
    },
];

static JUMP_COUNTS: &[&str] = &["One", "Two", "Infinite"];

static JUMP_CLASSIC_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  Classic Jump-Mod from its original release",
    },
    Widget::Separator {},
    Widget::Header {
        label: "Configuration",
    },
    Widget::Checkbox {
        label: "Use Changed Physics",
        pref: BoolPref::JumpChangePhysics,
    },
];

static JUMP_STANDARD_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  Standard Jump-Mod",
    },
    Widget::Separator {},
    Widget::Header {
        label: "Configuration",
    },
    Widget::Checkbox {
        label: "Use Changed Physics",
        pref: BoolPref::JumpChangePhysics,
    },
    Widget::Checkbox {
        label: "Allow Walljumps",
        pref: BoolPref::JumpAllowWalljumps,
    },
    Widget::Choose {
        label: "Jump Count",
        choices: JUMP_COUNTS,
        pref: I16Pref::JumpCount,
    },
];

static JUMP_PROFILES: &[&str] = &["Standard", "Classic"];

static JUMP_PROFILES_WIDGETS: &[Widget] = &[
    Widget::Choose {
        label: "Jump Profile",
        choices: JUMP_PROFILES,
        pref: I16Pref::JumpProfile,
    },
    Widget::HideableGroup {
        widgets: JUMP_STANDARD_WIDGETS,
        show_if: |cx| cx.pref.get(I16Pref::JumpProfile) == 0,
    },
    Widget::HideableGroup {
        widgets: JUMP_CLASSIC_WIDGETS,
        show_if: |cx| cx.pref.get(I16Pref::JumpProfile) == 1,
    },
];

static JUMP_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Jump Settings",
    },
    Widget::Checkbox {
        label: "Jump-Mod",
        pref: BoolPref::JumpMod,
    },
    Widget::HideableGroup {
        widgets: JUMP_PROFILES_WIDGETS,
        show_if: |cx| cx.pref.get(BoolPref::JumpMod),
    },
    Widget::Header { label: "Help" },
    Widget::Menu {
        label: "Controller Binds",
        widgets: JUMP_HELP_WIDGETS,
    },
];

static TIMER_TYPES: &[&str] = &["Default", "Frozen", "Freeze at 0", "Count up from 0"];
static FALLOUT_PLANE_TYPE: &[&str] = &["Normal", "Disabled", "Bouncy"];

static VARIANT_WIDGETS: &[Widget] = &[
    Widget::Header {
        label: "Modifications",
    },
    Widget::Choose {
        label: "Camera Type",
        choices: CAMERA_OPTIONS,
        pref: I16Pref::Camera,
    },
    Widget::Checkbox {
        label: "D-Pad Controls",
        pref: BoolPref::DpadControls,
    },
    Widget::Menu {
        label: "Physics",
        widgets: PHYSICS_WIDGETS,
    },
    Widget::Separator {},
    Widget::Header { label: "Assist" },
    Widget::Choose {
        label: "Timer Type",
        choices: TIMER_TYPES,
        pref: I16Pref::TimerType,
    },
    Widget::Choose {
        label: "Fallout Plane Type",
        choices: FALLOUT_PLANE_TYPE,
        pref: I16Pref::FalloutPlaneType,
    },
    Widget::Checkbox {
        label: "Disable Fallout Volume",
        pref: BoolPref::DisableFalloutVolumes,
    },
    Widget::Separator {},
    Widget::Header {
        label: "Alternative Modes",
    },
    Widget::Menu {
        label: "Jump-Mod",
        widgets: JUMP_WIDGETS,
    },
    Widget::Menu {
        label: "Stage Edits",
        widgets: STAGE_EDIT_WIDGETS,
    },
    Widget::Checkbox {
        label: "Marathon Mode",
        pref: BoolPref::Marathon,
    },
];

static RESET_PREFS_WIDGETS: &[Widget] = &[
    Widget::Text {
        label: "  Reset all preferences to defaults?",
    },
    Widget::Button {
        label: "Cancel",
        push: |_| {},
        after: AfterPush::GoBack,
    },
    Widget::Button {
        label: "Confirm",
        push: |cx| {
            cx.pref.reset_all_defaults();
            cx.pref.save();
        },
        after: AfterPush::GoBack,
    },
];

static PRACMOD_SETTINGS_WIDGETS: &[Widget] = &[
    Widget::InputSelect {
        label: "Menu Bind",
        pref: I16Pref::MenuBind,
        required_chord: true,
        can_unbind: false,
    },
    Widget::Text {
        label: "  Menu Bind Requires 2 Buttons",
    },
    Widget::Separator {},
    Widget::Choose {
        label: "RGB Format",
        choices: &["Decimal", "Hex"],
        pref: I16Pref::RgbFormat,
    },
    Widget::Separator {},
    Widget::Menu {
        label: "Restore Defaults",
        widgets: RESET_PREFS_WIDGETS,
    },
];

static ROOT_WIDGETS: &[Widget] = &[
    Widget::Menu {
        label: "Tools",
        widgets: TOOLS_WIDGETS,
    },
    Widget::Menu {
        label: "Displays",
        widgets: DISPLAYS_WIDGETS,
    },
    Widget::Menu {
        label: "Customization",
        widgets: BALL_COLOR_WIDGETS,
    },
    Widget::Menu {
        label: "Variants",
        widgets: VARIANT_WIDGETS,
    },
    Widget::Menu {
        label: "Settings",
        widgets: PRACMOD_SETTINGS_WIDGETS,
    },
    Widget::Menu {
        label: "About",
        widgets: ABOUT_WIDGETS,
    },
];

pub static ROOT_MENU: Widget = Widget::Menu {
    label: "Main Menu",
    widgets: ROOT_WIDGETS,
};
