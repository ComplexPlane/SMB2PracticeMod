#include "menu_defn.h"

#include "mkb/mkb.h"
#include "mods/ballcolor.h"
#include "mods/cmseg.h"
#include "mods/freecam.h"
#include "mods/gotostory.h"
#include "mods/ilbattle.h"
#include "mods/ilmark.h"
#include "mods/inputdisp.h"
#include "mods/stage_edits.h"
#include "mods/unlock.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/gamecode.h"
#include "utils/macro_utils.h"

// TODO update buttons with close menu flag
// TODO let buttons have null push()

namespace menu_defn {

static char s_version_str[36];

#ifndef GIT_HASH
#define GIT_HASH "<unknown>"
#endif
static const char *GIT_HASH_STR = "  Git commit:      " GIT_HASH;

static const char *INPUTDISP_COLORS[] = {
    "Purple", "Red", "Orange", "Yellow", "Green", "Blue", "Pink", "Black",
};
static_assert(LEN(INPUTDISP_COLORS) == inputdisp::NUM_COLORS);

static const char *INPUTDISP_LOCATION_OPTIONS[] = {
    "Right",
    "Center",
};

static const char *CAMERA_OPTIONS[] = {"Default", "Force SMB2", "Force SMB1"};

static const char *SAVESTATE_SAVE_TO_OPTIONS[] = {
    "Selected Slot",
    "Next Empty Slot",
    "Empty, Then Oldest",
};

static const char *INPUTDISP_COLOR_TYPE_OPTIONS[] = {
    "Preset", "RGB Solid", "RGB Gradient", "Rainbow", "Match Ball",
};

static Widget s_input_preset[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Preset Color",
                .choices = INPUTDISP_COLORS,
                .num_choices = LEN(INPUTDISP_COLORS),
                .pref = pref::Pref::InputDispColor,
            },
    },
};

static Widget s_input_hex[] = {
    {
        .type = WidgetType::RgbPreview,
        .rgb_preview =
            {
                .r_pref = pref::Pref::InputDispRed,
                .g_pref = pref::Pref::InputDispGreen,
                .b_pref = pref::Pref::InputDispBlue,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit =
            {
                .label = "Color Red",
                .pref = pref::Pref::InputDispRed,
                .min = ballcolor::COLOR_MIN,
                .max = ballcolor::COLOR_MAX,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit =
            {
                .label = "Color Green",
                .pref = pref::Pref::InputDispGreen,
                .min = ballcolor::COLOR_MIN,
                .max = ballcolor::COLOR_MAX,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit =
            {
                .label = "Color Blue",
                .pref = pref::Pref::InputDispBlue,
                .min = ballcolor::COLOR_MIN,
                .max = ballcolor::COLOR_MAX,
            },
    },
};

static Widget s_input_gradient[] = {
    {
        .type = WidgetType::RgbPreview,
        .rgb_preview =
            {
                .r_pref = pref::Pref::InputDispRed,
                .g_pref = pref::Pref::InputDispGreen,
                .b_pref = pref::Pref::InputDispBlue,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient Color 1 Red", pref::Pref::InputDispRed, 0, 255},
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient Color 1 Green", pref::Pref::InputDispGreen, 0, 255},
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient Color 1 Blue", pref::Pref::InputDispBlue, 0, 255},
    },
    {
        .type = WidgetType::RgbPreview,
        .rgb_preview =
            {
                .r_pref = pref::Pref::InputDispGradientColor2Red,
                .g_pref = pref::Pref::InputDispGradientColor2Green,
                .b_pref = pref::Pref::InputDispGradientColor2Blue,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient Color 2 Red", pref::Pref::InputDispGradientColor2Red, 0, 255},
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient Color 2 Green", pref::Pref::InputDispGradientColor2Green, 0, 255},
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient Color 2 Blue", pref::Pref::InputDispGradientColor2Blue, 0, 255},
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient Rotation", pref::Pref::InputDispGradientRotation, 0, 100},
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient Start", pref::Pref::InputDispGradientStart, 0, 100},
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit = {"Gradient End", pref::Pref::InputDispGradientEnd, 0, 100},
    },
};

static Widget s_inputdisp_subwidgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Location",
                .choices = INPUTDISP_LOCATION_OPTIONS,
                .num_choices = LEN(INPUTDISP_LOCATION_OPTIONS),
                .pref = pref::Pref::InputDispLocation,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Notch Indicators",
                .pref = pref::Pref::InputDispNotchIndicators,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Raw Stick Inputs",
                .pref = pref::Pref::InputDispRawStickInputs,
            },
    },
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::Header,
        .header = {"Color"},
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Color Type",
                .choices = INPUTDISP_COLOR_TYPE_OPTIONS,
                .num_choices = LEN(INPUTDISP_COLOR_TYPE_OPTIONS),
                .pref = pref::Pref::InputDispColorType,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_input_preset,
                .num_widgets = LEN(s_input_preset),
                .show_if =
                    []() {
                        return pref::get(pref::Pref::InputDispColorType) ==
                               (u8)inputdisp::InputDispColorType::Preset;
                    },
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_input_hex,
                .num_widgets = LEN(s_input_hex),
                .show_if =
                    []() {
                        return pref::get(pref::Pref::InputDispColorType) ==
                               (u8)inputdisp::InputDispColorType::RGBSolid;
                    },
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_input_gradient,
                .num_widgets = LEN(s_input_gradient),
                .show_if =
                    []() {
                        return pref::get(pref::Pref::InputDispColorType) ==
                               (u8)inputdisp::InputDispColorType::RGBGradient;
                    },
            },
    },
};

static Widget s_inputdisp_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Input Display"},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show Input Display",
                .pref = pref::Pref::InputDisp,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_inputdisp_subwidgets,
                .num_widgets = LEN(s_inputdisp_subwidgets),
                .show_if = []() { return pref::get(pref::Pref::InputDisp) != 0; },
            },
    },
};

static const char *BALL_COLORS[] = {
    "Default", "Red", "Blue", "Yellow", "Green", "Teal", "Pink", "Black", "White",
};
static_assert(LEN(BALL_COLORS) == ballcolor::NUM_COLORS);

static const char *MONKEY_TYPES[] = {"Default", "Aiai", "Meemee", "Baby", "Gongon", "Random"};

static const char *BALL_COLOR_TYPES[] = {
    "Preset",
    "RGB",
    "Rainbow",
    "Random",
};

static const char *CLOTHING_COLOR_TYPES[] = {
    "Preset",
    "Random",
};

static Widget s_preset_widgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Preset Color",
                .choices = BALL_COLORS,
                .num_choices = LEN(BALL_COLORS),
                .pref = pref::Pref::BallColor,
            },
    },
};

static Widget s_preset_ape_widgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Preset Color",
                .choices = BALL_COLORS,
                .num_choices = LEN(BALL_COLORS),
                .pref = pref::Pref::ApeColor,
            },
    },
};

static Widget s_hex_widgets[] = {
    {
        .type = WidgetType::RgbPreview,
        .rgb_preview =
            {
                .r_pref = pref::Pref::BallRed,
                .g_pref = pref::Pref::BallGreen,
                .b_pref = pref::Pref::BallBlue,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit =
            {
                .label = "Color Red",
                .pref = pref::Pref::BallRed,
                .min = ballcolor::COLOR_MIN,
                .max = ballcolor::COLOR_MAX,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit =
            {
                .label = "Color Green",
                .pref = pref::Pref::BallGreen,
                .min = ballcolor::COLOR_MIN,
                .max = ballcolor::COLOR_MAX,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit =
            {
                .label = "Color Blue",
                .pref = pref::Pref::BallBlue,
                .min = ballcolor::COLOR_MIN,
                .max = ballcolor::COLOR_MAX,
            },
    },
};

static Widget s_ball_color_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Ball Color"},
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Ball Color Type",
                .choices = BALL_COLOR_TYPES,
                .num_choices = LEN(BALL_COLOR_TYPES),
                .pref = pref::Pref::BallColorType,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_preset_widgets,
                .num_widgets = LEN(s_preset_widgets),
                .show_if = []() { return pref::get(pref::Pref::BallColorType) == 0; },
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_hex_widgets,
                .num_widgets = LEN(s_hex_widgets),
                .show_if = []() { return pref::get(pref::Pref::BallColorType) == 1; },
            },
    },
    {WidgetType::Separator},
    {
        .type = WidgetType::Header,
        .header = {"Clothing Color"},
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Clothing Color Type",
                .choices = CLOTHING_COLOR_TYPES,
                .num_choices = LEN(CLOTHING_COLOR_TYPES),
                .pref = pref::Pref::ApeColorType,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_preset_ape_widgets,
                .num_widgets = LEN(s_preset_ape_widgets),
                .show_if = []() { return pref::get(pref::Pref::ApeColorType) == 0; },
            },
    },
    {WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Monkey"}},
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Monkey Type",
                .choices = MONKEY_TYPES,
                .num_choices = LEN(MONKEY_TYPES),
                .pref = pref::Pref::MonkeyType,
            },
    },
};

static const char *IL_BATTLE_LENGTHS[] = {
    "5 min",
    "7 min",
    "10 min",
    "Endless",
};
static const char *SCORE_BREAKDOWN_OPTIONS[] = {
    "Off",
    "Minimal",
    "Full",
};
static_assert(LEN(IL_BATTLE_LENGTHS) == ilbattle::NUM_LENGTHS);

static Widget s_il_battle_score_widgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Score Breakdown",
                .choices = SCORE_BREAKDOWN_OPTIONS,
                .num_choices = LEN(SCORE_BREAKDOWN_OPTIONS),
                .pref = pref::Pref::IlBattleBreakdown,
            },
    },
};

static Widget s_il_battle_subwidgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Battle Length",
                .choices = IL_BATTLE_LENGTHS,
                .num_choices = LEN(IL_BATTLE_LENGTHS),
                .pref = pref::Pref::IlBattleLength,
            },
    },
    {
        .type = WidgetType::InputSelect,
        .input_select =
            {
                .label = "Ready Bind",
                .pref = pref::Pref::IlBattleReadyBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
    {.type = WidgetType::Text, .text = {"  Press Ready Bind then Retry to start a battle"}},
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Main Displays"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show Time",
                .pref = pref::Pref::IlBattleShowTime,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show Score",
                .pref = pref::Pref::IlBattleShowScore,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_il_battle_score_widgets,
                .num_widgets = LEN(s_il_battle_score_widgets),
                .show_if = []() { return pref::get(pref::Pref::IlBattleShowScore) != 0; },
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Extra Displays"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show Tie Count",
                .pref = pref::Pref::IlBattleTieCount,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show Attempts",
                .pref = pref::Pref::IlBattleAttemptCount,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Buzzer Message",
                .pref = pref::Pref::IlBattleBuzzerOld,
            },
    },
};

static Widget s_il_battle_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Battle Tracker"},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "IL Battle Display",
                .pref = pref::Pref::IlBattleDisplay,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_il_battle_subwidgets,
                .num_widgets = LEN(s_il_battle_subwidgets),
                .show_if = []() { return pref::get(pref::Pref::IlBattleDisplay) != 0; },
            },
    },
};

// Forgive me for putting code in the menu definition
static bool rumble_get(int controller_idx) {
    return mkb::rumble_enabled_bitflag & (1 << controller_idx);
}

static void rumble_set(int controller_idx, bool value) {
    if (value) {
        mkb::rumble_enabled_bitflag |= 1 << controller_idx;
    } else {
        mkb::rumble_enabled_bitflag &= ~(1 << controller_idx);
    }
}

static Widget s_rumble_widgets[] = {
    {
        .type = WidgetType::GetSetCheckbox,
        .get_set_checkbox = {
            .label = "Controller 1 Rumble",
            .get = []() { return rumble_get(0); },
            .set = [](bool enable) { rumble_set(0, enable); },
        },
    },
    {
        .type = WidgetType::GetSetCheckbox,
        .get_set_checkbox = {
            .label = "Controller 2 Rumble",
            .get = []() { return rumble_get(1); },
            .set = [](bool enable) { rumble_set(1, enable); },
        },
    },
    {
        .type = WidgetType::GetSetCheckbox,
        .get_set_checkbox = {
            .label = "Controller 3 Rumble",
            .get = []() { return rumble_get(2); },
            .set = [](bool enable) { rumble_set(2, enable); },
        },
    },
    {
        .type = WidgetType::GetSetCheckbox,
        .get_set_checkbox = {
            .label = "Controller 4 Rumble",
            .get = []() { return rumble_get(3); },
            .set = [](bool enable) { rumble_set(3, enable); },
        },
    },
};

static Widget s_about_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"SMB2 Practice Mod"},
    },
    {
        .type = WidgetType::Text,
        .text = {"  Made with   by"},
    },
    {
        .type = WidgetType::Custom,
        .custom = {draw::heart},
    },
    {
        .type = WidgetType::ColoredText,
        .colored_text =
            {
                .label = "ComplexPlane",
                .color_left = {0x00, 0xde, 0x38, 0xff},
                .color_right = {0x0a, 0xf2, 0xef, 0xff},
                .offset_x = 17,
                .line = TextLine::Overlap,
            },
    },
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::Text,
        .text = {"  With contributions from:        ,      "},
    },
    {
        .type = WidgetType::ColoredText,
        .colored_text =
            {
                .label = "rehtrop",
                .color_left = {0x69, 0xe3, 0xff, 0xff},
                .color_right = {0xdd, 0x9b, 0xdc, 0xff},
                .offset_x = 27,
                .line = TextLine::Overlap,
            },
    },
    {
        .type = WidgetType::ColoredText,
        .colored_text =
            {
                .label = "Nambo",
                .color_left = {0xd0, 0x67, 0xff, 0xff},
                .color_right = {0xf3, 0xe3, 0xff, 0xff},
                .offset_x = 36,
                .line = TextLine::Overlap,
            },
    },
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::ColoredText,
        .colored_text =
            {
                .label = "Written in C++",
                .color_left = {0xf9, 0x6f, 0x58, 0xff},
                .color_right = {0xef, 0xa7, 0x2c, 0xff},
                .offset_x = 2,
                .line = TextLine::NewLine,
            },
    },
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::Header,
        .header = {"Updates"},
    },
    {
        .type = WidgetType::Text,
        .text = {s_version_str},
    },
    {
        .type = WidgetType::Text,
        .text = {GIT_HASH_STR},
    },
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::Text,
        .text = {"  Download the latest version at:"},
    },
    {
        .type = WidgetType::ColoredText,
        .colored_text =
            {
                .label = "  github.com/ComplexPlane/SMB2PracticeMod",
                .color_left = draw::BLUE,
                .color_right = draw::BLUE,
                .offset_x = 0,
                .line = TextLine::NewLine,
            },
    },
};

static const char *CHARA_CHOICES[] = {"AiAi", "MeeMee", "Baby", "GonGon", "Random"};

static Widget s_cm_beg_widgets[] = {
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Beginner 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Beginner1); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Beginner Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::BeginnerExtra); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
};

static Widget s_cm_adv_widgets[] = {
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Advanced 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced1); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Advanced 11-20",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced11); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Advanced 21-30",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced21); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Advanced Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::AdvancedExtra); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
};

static Widget s_cm_exp_widgets[] = {
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert1); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 11-20",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert11); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 21-30",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert21); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 31-40",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert31); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 41-50",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert41); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::ExpertExtra); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
};

static Widget s_cm_mas_widgets[] = {
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Master 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Master1); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Master Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::MasterExtra); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
};

static Widget s_iw_help_widgets[] = {
    {.type = WidgetType::Text, .text = {"  Story Mode individual world segments can"}},
    {.type = WidgetType::Text, .text = {"  be done by going to file select in Story"}},
    {.type = WidgetType::Text, .text = {"  Mode, pressing up/down on an empty file"}},
    {.type = WidgetType::Text, .text = {"  to select a world, and selecting the file."}},
};

static Widget s_cm_seg_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Segment Selection"},
    },
    // Beginner
    {
        .type = WidgetType::Menu,
        .menu = {"Beginner", s_cm_beg_widgets, LEN(s_cm_beg_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Advanced", s_cm_adv_widgets, LEN(s_cm_adv_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Expert", s_cm_exp_widgets, LEN(s_cm_exp_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Master", s_cm_mas_widgets, LEN(s_cm_mas_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Story Mode", s_iw_help_widgets, LEN(s_iw_help_widgets)},
    },
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::Header,
        .header = {"Segment Options"},
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Character",
                .choices = CHARA_CHOICES,
                .num_choices = LEN(CHARA_CHOICES),
                .pref = pref::Pref::CmChara,
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Text, .text = {"  Segments may crash in some romhacks"}},
};

static Widget s_cm_seg_menu_widgets[] = {
    {
        .type = WidgetType::Menu,
        .menu = {"Challenge Mode Seg", s_cm_seg_widgets, LEN(s_cm_seg_widgets)},
    },
};


static Widget s_timers_widgets[] = {
    {.type = WidgetType::Header, .header = {"Realtime Timers"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Realtime (RTA)",
                .pref = pref::Pref::TimerShowRTA,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Pausetime (PAU)",
                .pref = pref::Pref::TimerShowPause,
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Subtick Timers"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Framesave (FSV)",
                .pref = pref::Pref::TimerShowFramesave,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Subtick (SUB)",
                .pref = pref::Pref::TimerShowSubtick,
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Segment Timers"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Story Mode IWs (IW)",
                .pref = pref::Pref::IwTimer,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "CM Segments (SEG)",
                .pref = pref::Pref::CmTimer,
            },
    },
};

static Widget s_savestates_help_widgets[] = {
    {.type = WidgetType::Text, .text = {"  X          \x1c Create savestate"}},
    {.type = WidgetType::Text, .text = {"  Y          \x1c Load savestate"}},
    {.type = WidgetType::Text, .text = {"  C-Stick    \x1c Change savestate slot"}},
    // TODO replace this feature with a better one that works in-menu
    {.type = WidgetType::Text, .text = {"  L+X or R+X \x1c Frame advance"}},
    {.type = WidgetType::Text, .text = {"  L+C or R+C \x1c Browse savestates"}},
};

static Widget s_jump_help_widgets[] = {
    {.type = WidgetType::Text, .text = {"  A          \x1c Jump"}},
    {.type = WidgetType::Text, .text = {"  B          \x1c Resize minimap"}},
};

static Widget s_freecam_help_widgets[] = {
    {.type = WidgetType::Text, .text = {"  Left Stick \x1c Move"}},
    {.type = WidgetType::Text, .text = {"  C-Stick    \x1c Rotate"}},
    {.type = WidgetType::Text, .text = {"  L          \x1c Down"}},
    {.type = WidgetType::Text, .text = {"  R          \x1c Up"}},
    {.type = WidgetType::Text, .text = {"  Y          \x1c Turbo Speed"}},
    {.type = WidgetType::Text, .text = {"  X          \x1c Slow Speed"}},
    {.type = WidgetType::Text, .text = {"  D-pad Up   \x1c Increase Turbo Speed"}},
    {.type = WidgetType::Text, .text = {"  D-pad Down \x1c Decrease Turbo Speed"}},
};

static Widget s_iw_mark_help_widgets[] = {
    {.type = WidgetType::Text, .text = {"  The IL validation mark is shown after"}},
    {.type = WidgetType::Text, .text = {"  completing a level. It is green only if no "}},
    {.type = WidgetType::Text, .text = {"  gameplay-altering mods were used during the "}},
    {.type = WidgetType::Text, .text = {"  level completion, and red otherwise."}},
    {.type = WidgetType::Separator},

    {.type = WidgetType::Text, .text = {"  Showing this watermark is currently required"}},
    {.type = WidgetType::Text, .text = {"  to submit IL speedruns of vanilla SMB2 to the"}},
    {.type = WidgetType::Text, .text = {"  official leaderboards if you are using the"}},
    {.type = WidgetType::Text, .text = {"  SMB2 Practice Mod."}},
    {.type = WidgetType::Separator},

    {.type = WidgetType::Text, .text = {"  Note that some visual-only mods, such as raw"}},
    {.type = WidgetType::Text, .text = {"  input display, may also be disallowed for"}},
    {.type = WidgetType::Text, .text = {"  IL leaderboard submissions. The IL validation"}},
    {.type = WidgetType::Text, .text = {"  mark does not account for visual-only mods."}},
    {.type = WidgetType::Separator},

    {.type = WidgetType::Text, .text = {"  If you complete a stage without the watermark"}},
    {.type = WidgetType::Text, .text = {"  enabled, you can still enable it post-goal to"}},
    {.type = WidgetType::Text, .text = {"  validate your run."}},
    {.type = WidgetType::Separator},
};

static Widget s_sound_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Mute Background Music",
                .pref = pref::Pref::MuteBgm,
            },
    },
    {.type = WidgetType::Text, .text = {"  To apply background music setting:"}},
    {.type = WidgetType::Text, .text = {"  Wait 2s then reset console"}},
    {.type = WidgetType::Separator},

    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Mute Timer Ding",
                .pref = pref::Pref::MuteTimerDing,
            },
    },
};

static Widget s_unlock_confirm_widgets[] = {
    {
        .type = WidgetType::Text,
        .text = {"  This will unlock all levels, lives, etc."},
    },
    {
        .type = WidgetType::Text,
        .text = {"  Save your game to make this persistent."},
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Cancel",
                .push = nullptr,
                .flags = ButtonFlags::GoBack,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Confirm",
                .push = unlock::unlock_everything,
                .flags = ButtonFlags::GoBack,
            },
    },
};

static Widget s_unlock_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Unlock Progress For This Session"},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Unlock Now", s_unlock_confirm_widgets, LEN(s_unlock_confirm_widgets)},
    },
    {.type = WidgetType::Separator},

    {
        .type = WidgetType::Header,
        .header = {"Always Unlock Progress"},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "For Vanilla SMB2",
                .pref = pref::Pref::UnlockVanilla,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "For Romhacks",
                .pref = pref::Pref::UnlockRomhacks,
            },
    },
    {
        .type = WidgetType::Text,
        .text = {"  Applied on game startup."},
    },
};


static Widget s_freecam_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Freecam Toggle"},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Freecam",
                .pref = pref::Pref::Freecam,
            },
    },
    {
        .type = WidgetType::InputSelect,
        .input_select =
            {
                .label = "Toggle Bind",
                .pref = pref::Pref::FreecamToggleBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Configuration"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Invert Yaw",
                .pref = pref::Pref::FreecamInvertYaw,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Invert Pitch",
                .pref = pref::Pref::FreecamInvertPitch,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit =
            {
                .label = "Turbo Speed Factor",
                .pref = pref::Pref::FreecamSpeedMult,
                .min = freecam::TURBO_SPEED_MIN,
                .max = freecam::TURBO_SPEED_MAX,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Freeze Timer",
                .pref = pref::Pref::FreecamFreezeTimer,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide HUD",
                .pref = pref::Pref::FreecamHideHud,
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Help"}},
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Controller Binds",
                .widgets = s_freecam_help_widgets,
                .num_widgets = LEN(s_freecam_help_widgets),
            },
    },
};

static Widget s_hide_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Hide Elements"},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "HUD",
                .pref = pref::Pref::HideHud,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Ball",
                .pref = pref::Pref::HideBall,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Stage Models",
                .pref = pref::Pref::HideStage,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Stage Items",
                .pref = pref::Pref::HideItems,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Stage Objects",
                .pref = pref::Pref::HideStobjs,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Effects",
                .pref = pref::Pref::HideEffects,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Background",
                .pref = pref::Pref::HideBg,
            },
    },
};


static Widget s_savestate_subwidgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Save To",
                .choices = SAVESTATE_SAVE_TO_OPTIONS,
                .num_choices = LEN(SAVESTATE_SAVE_TO_OPTIONS),
                .pref = pref::Pref::SavestateSaveTo,
            },
    },
    {
        .type = WidgetType::InputSelect,
        .input_select =
            {
                .label = "Clear Savestate Bind",
                .pref = pref::Pref::SavestateClearBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
    {
        .type = WidgetType::InputSelect,
        .input_select =
            {
                .label = "Clear All Bind",
                .pref = pref::Pref::SavestateClearAllBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
};

static Widget s_savestate_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Savestates"},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Enable Savestates",
                .pref = pref::Pref::Savestates,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_savestate_subwidgets,
                .num_widgets = LEN(s_savestate_subwidgets),
                .show_if = []() { return pref::get(pref::Pref::Savestates) != 0; },
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Help"}},
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Controller Binds",
                .widgets = s_savestates_help_widgets,
                .num_widgets = LEN(s_savestates_help_widgets),
            },
    },
};

static Widget s_tools_widgets[] = {
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Go To Story Mode",
                .push = gotostory::load_storymode,
                .flags = ButtonFlags::CloseMenu,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_cm_seg_menu_widgets,
                .num_widgets = LEN(s_cm_seg_menu_widgets),
                .show_if = gamecode::is_vanilla,
            },
    },
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Savestates",
                .widgets = s_savestate_widgets,
                .num_widgets = LEN(s_savestate_widgets),
            },
    },
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Freecam",
                .widgets = s_freecam_widgets,
                .num_widgets = LEN(s_freecam_widgets),
            },
    },
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Hide Elements",
                .widgets = s_hide_widgets,
                .num_widgets = LEN(s_hide_widgets),
            },
    },
    {.type = WidgetType::Menu, .menu = {"Rumble", s_rumble_widgets, LEN(s_rumble_widgets)}},
    {
        .type = WidgetType::Menu,
        .menu = {"Audio", s_sound_widgets, LEN(s_sound_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Progress Unlock", s_unlock_widgets, LEN(s_unlock_widgets)},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Debug Mode",
                .pref = pref::Pref::DebugMode,
            },
    },
};

static Widget s_reset_ilmark_widgets[] = {
    {
        .type = WidgetType::Text,
        .text = {"  Reset IL invalidating preferences to defaults?"},
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Cancel",
                .push = nullptr,
                .flags = ButtonFlags::GoBack,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Confirm",
                .push = [] { ilmark::disable_invalidating_settings(); },
                .flags = ButtonFlags::GoBack,
            },
    },
};

static Widget s_il_mark_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Disable IL Invalidating Settings"},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Disable Now", s_reset_ilmark_widgets, LEN(s_reset_ilmark_widgets)},
    },
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::Header,
        .header = {"Show IL Validation Mark in..."},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Practice Mode",
                .pref = pref::Pref::IlMarkPractice,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Story Mode",
                .pref = pref::Pref::IlMarkStory,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Challenge Mode",
                .pref = pref::Pref::IlMarkChallenge,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Romhacks",
                .pref = pref::Pref::IlMarkRomhacks,
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Help"}},
    {
        .type = WidgetType::Menu,
        .menu = {"About", s_iw_mark_help_widgets, LEN(s_iw_mark_help_widgets)},
    },
};

static Widget s_displays_widgets[] = {
    {
        .type = WidgetType::Menu,
        .menu = {"Input Display", s_inputdisp_widgets, LEN(s_inputdisp_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Timers", s_timers_widgets, LEN(s_timers_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"IL Battle", s_il_battle_widgets, LEN(s_il_battle_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"IL Validation Mark", s_il_mark_widgets, LEN(s_il_mark_widgets)},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "9999 Banana Counter",
                .pref = pref::Pref::BananaCounter9999,
            },
    },
};

static Widget s_enabled_physics_widgets[] = {
    {
        .type = WidgetType::FloatEdit,
        .float_edit =
            {
                .label = "Friction",
                .pref = pref::Pref::Friction,
                .precision = 1000,
                .min = 0,
                .max = 255,
                .floor = -100,
                .decimals = 3,
            },
    },
    {
        .type = WidgetType::FloatEdit,
        .float_edit =
            {
                .label = "Restitution",
                .pref = pref::Pref::Restitution,
                .precision = 100,
                .min = 0,
                .max = 255,
                .floor = -100,
                .decimals = 2,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Moon Gravity",
                .pref = pref::Pref::Moon,
            },
    },
};

static Widget s_physics_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Use Custom Physics",
                .pref = pref::Pref::UseCustomPhysics,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_enabled_physics_widgets,
                .num_widgets = LEN(s_enabled_physics_widgets),
                .show_if = []() { return pref::get(pref::Pref::UseCustomPhysics) != 0; },
            },
    },
};

static const char *STAGE_EDIT_VARIANTS[] = {"None", "Golden Banana", "Dark Banana", "Reverse Mode"};

static Widget s_reverse_goal_widgets[] = {
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Select New Goal",
                .push = [] { stage_edits::select_new_goal(); },
                .flags = ButtonFlags::CloseMenu,
            },
    },
};

static Widget s_stage_edit_widgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Stage Edit Mode",
                .choices = STAGE_EDIT_VARIANTS,
                .num_choices = LEN(STAGE_EDIT_VARIANTS),
                .pref = pref::Pref::StageEditVariant,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_reverse_goal_widgets,
                .num_widgets = LEN(s_reverse_goal_widgets),
                .show_if = []() { return pref::get(pref::Pref::StageEditVariant) == 3; },
            },
    },
    {.type = WidgetType::Text, .text = {"  Stage Edits are activated on retry"}},
};

static const char *JUMP_COUNTS[] = {"One", "Two", "Infinite"};
static const char *JUMP_PROFILES[] = {"Standard", "Classic"};

static Widget s_jump_classic_widgets[] = {
    {.type = WidgetType::Text, .text = {"  Classic Jump-Mod from its original release"}},
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Configuration"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Use Changed Physics",
                .pref = pref::Pref::JumpChangePhysics,
            },
    },
};

static Widget s_jump_standard_widgets[] = {
    {.type = WidgetType::Text, .text = {"  Standard Jump-Mod"}},
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Configuration"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Use Changed Physics",
                .pref = pref::Pref::JumpChangePhysics,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Allow Walljumps",
                .pref = pref::Pref::JumpAllowWalljumps,
            },
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Jump Count",
                .choices = JUMP_COUNTS,
                .num_choices = LEN(JUMP_COUNTS),
                .pref = pref::Pref::JumpCount,
            },
    },
};

static Widget s_jump_profiles_widgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Jump Profile",
                .choices = JUMP_PROFILES,
                .num_choices = LEN(JUMP_PROFILES),
                .pref = pref::Pref::JumpProfile,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_jump_standard_widgets,
                .num_widgets = LEN(s_jump_standard_widgets),
                .show_if = []() { return pref::get(pref::Pref::JumpProfile) == 0; },
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_jump_classic_widgets,
                .num_widgets = LEN(s_jump_classic_widgets),
                .show_if = []() { return pref::get(pref::Pref::JumpProfile) == 1; },
            },
    },
};

static Widget s_jump_widgets[] = {
    {.type = WidgetType::Header, .header = {"Jump Settings"}},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Jump-Mod",
                .pref = pref::Pref::JumpMod,
            },
    },
    {
        .type = WidgetType::HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_jump_profiles_widgets,
                .num_widgets = LEN(s_jump_profiles_widgets),
                .show_if = []() { return pref::get(pref::Pref::JumpMod) != 0; },
            },
    },
    {.type = WidgetType::Header, .header = {"Help"}},
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Controller Binds",
                .widgets = s_jump_help_widgets,
                .num_widgets = LEN(s_jump_help_widgets),
            },
    },
};

static const char *TIMER_TYPES[] = {"Default", "Freeze at max", "Freeze at 0", "Count up from 0"};
static const char *FALLOUT_PLANE_TYPE[] = {"Normal", "Disabled", "Bouncy"};

static Widget s_gameplay_mods_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Modifications"},
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Camera Type",
                .choices = CAMERA_OPTIONS,
                .num_choices = LEN(CAMERA_OPTIONS),
                .pref = pref::Pref::Camera,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "D-Pad Controls",
                .pref = pref::Pref::DpadControls,
            },
    },
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Physics",
                .widgets = s_physics_widgets,
                .num_widgets = LEN(s_physics_widgets),
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Assist"}},
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Timer Type",
                .choices = TIMER_TYPES,
                .num_choices = LEN(TIMER_TYPES),
                .pref = pref::Pref::TimerType,
            },
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Fallout Plane Type",
                .choices = FALLOUT_PLANE_TYPE,
                .num_choices = LEN(FALLOUT_PLANE_TYPE),
                .pref = pref::Pref::FalloutPlaneType,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Disable Fallout Volume",
                .pref = pref::Pref::DisableFalloutVolumes,
            },
    },
    {.type = WidgetType::Separator},
    {.type = WidgetType::Header, .header = {"Alternative Modes"}},
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Jump-Mod",
                .widgets = s_jump_widgets,
                .num_widgets = LEN(s_jump_widgets),
            },
    },
    {
        .type = WidgetType::Menu,
        .menu =
            {
                .label = "Stage Edits",
                .widgets = s_stage_edit_widgets,
                .num_widgets = LEN(s_stage_edit_widgets),
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Marathon Mode",
                .pref = pref::Pref::Marathon,
            },
    },
};

static Widget s_reset_prefs_widgets[] = {
    {
        .type = WidgetType::Text,
        .text = {"  Reset all preferences to defaults?"},
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Cancel",
                .push = nullptr,
                .flags = ButtonFlags::GoBack,
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Confirm",
                .push =
                    [] {
                        pref::reset_all_defaults();
                        pref::save();
                    },
                .flags = ButtonFlags::GoBack,
            },
    },
};

static const char *RGB_FORMAT_OPTIONS[] = {
    "Decimal",
    "Hex",
};

static Widget s_pracmod_settings_widgets[] = {
    {
        .type = WidgetType::InputSelect,
        .input_select =
            {
                .label = "Menu Bind",
                .pref = pref::Pref::MenuBind,
                .required_chord = true,
                .can_unbind = false,
            },
    },
    {.type = WidgetType::Text, .text = {"  Menu Bind Requires 2 Buttons"}},
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "RGB Format",
                .choices = RGB_FORMAT_OPTIONS,
                .num_choices = LEN(RGB_FORMAT_OPTIONS),
                .pref = pref::Pref::RgbFormat,
            },
    },
    {.type = WidgetType::Separator},
    {
        .type = WidgetType::Menu,
        .menu = {"Restore Defaults", s_reset_prefs_widgets, LEN(s_reset_prefs_widgets)},
    },
};

static Widget s_root_widgets[] = {
    {
        .type = WidgetType::Menu,
        .menu = {"Tools", s_tools_widgets, LEN(s_tools_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Displays", s_displays_widgets, LEN(s_displays_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Customization", s_ball_color_widgets, LEN(s_ball_color_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Variants", s_gameplay_mods_widgets, LEN(s_gameplay_mods_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Settings", s_pracmod_settings_widgets, LEN(s_pracmod_settings_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"About", s_about_widgets, LEN(s_about_widgets)},
    },
};

MenuWidget root_menu = {
    .label = "Main Menu",
    .widgets = s_root_widgets,
    .num_widgets = LEN(s_root_widgets),
};

void init() {
    mkb::sprintf(s_version_str, "  Current version: v%s", version::get_version_str());
}

}  // namespace menu_defn
