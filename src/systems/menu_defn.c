#include "systems/menu_defn.h"

#include "mods/ballcolor.h"
#include "mods/cmseg.h"
#include "mods/freecam.h"
#include "mods/gotostory.h"
#include "mods/ilbattle.h"
#include "mods/ilmark.h"
#include "mods/inputdisp.h"
#include "mods/physics.h"
#include "mods/stage_edits.h"
#include "mods/unlock.h"
#include "mods/validate.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

// TODO: update buttons with close menu flag
// TODO: let buttons have null push()

static char s_version_str[36];

static void int_edit_rgb_format(s16 value, char *buf) {
    if (Pref_Get(Pref_RgbFormat) == 1) {
        mkb_sprintf(buf, "0x%02X", (int)value);
    } else {
        mkb_sprintf(buf, "%d", (int)value);
    }
}

static void int_edit_percent_format(s16 value, char *buf) {
    mkb_sprintf(buf, "%d%%", (int)value);
}

static const char *INPUTDISP_COLORS[] = {
    "Purple", "Red", "Orange", "Yellow", "Green", "Blue", "Pink", "Black",
};
static_assert(LEN(INPUTDISP_COLORS) == InputDisp_NUM_COLORS);

static const char *CAMERA_OPTIONS[] = {"Default", "Force SMB2", "Force SMB1"};

static const char *INPUTDISP_COLOR_TYPE_OPTIONS[] = {
    "Preset", "RGB Solid", "RGB Gradient", "Rainbow", "Match Ball",
};

static Widget s_input_preset[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Preset Color",
                .choices = INPUTDISP_COLORS,
                .num_choices = LEN(INPUTDISP_COLORS),
                .pref = Pref_InputDispColor,
            },
    },
};

static Widget s_input_hex[] = {
    {
        .type = WidgetType_RgbPreview,
        .rgb_preview =
            {
                .r_pref = Pref_InputDispRed,
                .g_pref = Pref_InputDispGreen,
                .b_pref = Pref_InputDispBlue,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Red Value",
                .pref = Pref_InputDispRed,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Green Value",
                .pref = Pref_InputDispGreen,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Blue Value",
                .pref = Pref_InputDispBlue,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
};

static Widget s_input_gradient[] = {
    {
        .type = WidgetType_RgbPreview,
        .rgb_preview =
            {
                .r_pref = Pref_InputDispRed,
                .g_pref = Pref_InputDispGreen,
                .b_pref = Pref_InputDispBlue,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient Color 1 Red",
                .pref = Pref_InputDispRed,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient Color 1 Green",
                .pref = Pref_InputDispGreen,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient Color 1 Blue",
                .pref = Pref_InputDispBlue,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_RgbPreview,
        .rgb_preview =
            {
                .r_pref = Pref_InputDispGradientColor2Red,
                .g_pref = Pref_InputDispGradientColor2Green,
                .b_pref = Pref_InputDispGradientColor2Blue,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient Color 2 Red",
                .pref = Pref_InputDispGradientColor2Red,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient Color 2 Green",
                .pref = Pref_InputDispGradientColor2Green,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient Color 2 Blue",
                .pref = Pref_InputDispGradientColor2Blue,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient Rotation",
                .pref = Pref_InputDispGradientRotation,
                .min = 0,
                .max = 100,
                .format = int_edit_percent_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient Start",
                .pref = Pref_InputDispGradientStart,
                .min = -200,
                .max = 200,
                .format = int_edit_percent_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Gradient End",
                .pref = Pref_InputDispGradientEnd,
                .min = -100,
                .max = 300,
                .format = int_edit_percent_format,
            },
    },
};

static const char *INPUTDISP_LOCATION_OPTIONS[] = {"Right", "Center"};

static bool show_if_inputdisp_color_preset() {
    return Pref_Get(Pref_InputDispColorType) == 0;
}

static bool show_if_inputdisp_color_rgb() {
    return Pref_Get(Pref_InputDispColorType) == 1;
}

static bool show_if_inputdisp_color_gradient() {
    return Pref_Get(Pref_InputDispColorType) == 2;
}

static Widget s_inputdisp_subwidgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Location",
                .choices = INPUTDISP_LOCATION_OPTIONS,
                .num_choices = LEN(INPUTDISP_LOCATION_OPTIONS),
                .pref = Pref_InputDispLocation,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Notch Indicators",
                .pref = Pref_InputDispNotchIndicators,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Raw Stick Inputs",
                .pref = Pref_InputDispRawStickInputs,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Color Type",
                .choices = INPUTDISP_COLOR_TYPE_OPTIONS,
                .num_choices = LEN(INPUTDISP_COLOR_TYPE_OPTIONS),
                .pref = Pref_InputDispColorType,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_input_preset,
                .num_widgets = LEN(s_input_preset),
                .show_if = show_if_inputdisp_color_preset,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_input_hex,
                .num_widgets = LEN(s_input_hex),
                .show_if = show_if_inputdisp_color_rgb,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_input_gradient,
                .num_widgets = LEN(s_input_gradient),
                .show_if = show_if_inputdisp_color_gradient,
            },
    },
};

static bool show_if_inputdisp_enabled() {
    return Pref_Get(Pref_InputDisp) != 0;
}

static Widget s_inputdisp_widgets[] = {
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Show Input Display",
                .pref = Pref_InputDisp,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_inputdisp_subwidgets,
                .num_widgets = LEN(s_inputdisp_subwidgets),
                .show_if = show_if_inputdisp_enabled,
            },
    },
};

static const char *BALL_COLORS[] = {
    "Default", "Red", "Blue", "Yellow", "Green", "Teal", "Pink", "Black", "White",
};
static_assert(LEN(BALL_COLORS) == BallColor_NUM_COLORS);

static const char *BALL_COLOR_TYPES[] = {
    "Preset",
    "RGB Selector",
    "Rainbow",
    "Random",
};

static const char *CLOTHING_COLOR_TYPES[] = {
    "Preset",
    "Random",
};

static Widget s_preset_widgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Preset Color",
                .choices = BALL_COLORS,
                .num_choices = LEN(BALL_COLORS),
                .pref = Pref_BallColor,
            },
    },
};

static Widget s_preset_ape_widgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Preset Color",
                .choices = BALL_COLORS,
                .num_choices = LEN(BALL_COLORS),
                .pref = Pref_ApeColor,
            },
    },
};

static Widget s_hex_widgets[] = {
    {
        .type = WidgetType_RgbPreview,
        .rgb_preview =
            {
                .r_pref = Pref_BallRed,
                .g_pref = Pref_BallGreen,
                .b_pref = Pref_BallBlue,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Red Value",
                .pref = Pref_BallRed,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Green Value",
                .pref = Pref_BallGreen,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Blue Value",
                .pref = Pref_BallBlue,
                .min = BallColor_COLOR_MIN,
                .max = BallColor_COLOR_MAX,
                .format = int_edit_rgb_format,
            },
    },
};

static const char *MONKEY_TYPES[] = {
    "Default", "Aiai", "Meemee", "Baby", "Gongon", "Random",
};

static bool show_if_ball_color_preset() {
    return Pref_Get(Pref_BallColorType) == 0;
}

static bool show_if_ball_color_rgb() {
    return Pref_Get(Pref_BallColorType) == 1;
}

static bool show_if_ape_color_preset() {
    return Pref_Get(Pref_ApeColorType) == 0;
}

static Widget s_ball_color_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Ball Color"},
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Ball Color Type",
                .choices = BALL_COLOR_TYPES,
                .num_choices = LEN(BALL_COLOR_TYPES),
                .pref = Pref_BallColorType,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_preset_widgets,
                .num_widgets = LEN(s_preset_widgets),
                .show_if = show_if_ball_color_preset,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_hex_widgets,
                .num_widgets = LEN(s_hex_widgets),
                .show_if = show_if_ball_color_rgb,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Clothing Color"},
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Clothing Color Type",
                .choices = CLOTHING_COLOR_TYPES,
                .num_choices = LEN(CLOTHING_COLOR_TYPES),
                .pref = Pref_ApeColorType,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_preset_ape_widgets,
                .num_widgets = LEN(s_preset_ape_widgets),
                .show_if = show_if_ape_color_preset,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Monkey"},
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Monkey Type",
                .choices = MONKEY_TYPES,
                .num_choices = LEN(MONKEY_TYPES),
                .pref = Pref_MonkeyType,
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
static_assert(LEN(IL_BATTLE_LENGTHS) == ILBattle_NUM_LENGTHS);

static Widget s_il_battle_score_widgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Score Breakdown",
                .choices = SCORE_BREAKDOWN_OPTIONS,
                .num_choices = LEN(SCORE_BREAKDOWN_OPTIONS),
                .pref = Pref_IlBattleBreakdown,
            },
    },
};

static bool show_if_il_battle_show_score() {
    return Pref_Get(Pref_IlBattleShowScore) != 0;
}

static Widget s_il_battle_subwidgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Battle Length",
                .choices = IL_BATTLE_LENGTHS,
                .num_choices = LEN(IL_BATTLE_LENGTHS),
                .pref = Pref_IlBattleLength,
            },
    },
    {
        .type = WidgetType_InputSelect,
        .input_select =
            {
                .label = "Ready Bind",
                .pref = Pref_IlBattleReadyBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
    {.type = WidgetType_Text, .text = {"  Press Ready Bind then Retry to start a battle"}},
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Main Displays"},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Show Time",
                .pref = Pref_IlBattleShowTime,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Show Score",
                .pref = Pref_IlBattleShowScore,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_il_battle_score_widgets,
                .num_widgets = LEN(s_il_battle_score_widgets),
                .show_if = show_if_il_battle_show_score,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Extra Displays"},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Show Tie Count",
                .pref = Pref_IlBattleTieCount,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Show Attempts",
                .pref = Pref_IlBattleAttemptCount,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Buzzer Message",
                .pref = Pref_IlBattleBuzzerOld,
            },
    },
};

static bool show_if_il_battle_enabled() {
    return Pref_Get(Pref_IlBattleDisplay) != 0;
}

static Widget s_il_battle_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Battle Tracker"},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "IL Battle Display",
                .pref = Pref_IlBattleDisplay,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_il_battle_subwidgets,
                .num_widgets = LEN(s_il_battle_subwidgets),
                .show_if = show_if_il_battle_enabled,
            },
    },
};

// Forgive me for putting code in the menu definition
static bool rumble_get(int controller_idx) {
    return mkb_rumble_enabled_bitflag & (1 << controller_idx);
}

static void rumble_set(int controller_idx, bool value) {
    if (value) {
        mkb_rumble_enabled_bitflag |= 1 << controller_idx;
    } else {
        mkb_rumble_enabled_bitflag &= ~(1 << controller_idx);
    }
}

static bool rumble_get_0() {
    return rumble_get(0);
}
static void rumble_set_0(bool enable) {
    rumble_set(0, enable);
}
static bool rumble_get_1() {
    return rumble_get(1);
}
static void rumble_set_1(bool enable) {
    rumble_set(1, enable);
}
static bool rumble_get_2() {
    return rumble_get(2);
}
static void rumble_set_2(bool enable) {
    rumble_set(2, enable);
}
static bool rumble_get_3() {
    return rumble_get(3);
}
static void rumble_set_3(bool enable) {
    rumble_set(3, enable);
}

static Widget s_rumble_widgets[] = {
    {
        .type = WidgetType_GetSetCheckbox,
        .get_set_checkbox =
            {
                .label = "Controller 1 Rumble",
                .get = rumble_get_0,
                .set = rumble_set_0,
            },
    },
    {
        .type = WidgetType_GetSetCheckbox,
        .get_set_checkbox =
            {
                .label = "Controller 2 Rumble",
                .get = rumble_get_1,
                .set = rumble_set_1,
            },
    },
    {
        .type = WidgetType_GetSetCheckbox,
        .get_set_checkbox =
            {
                .label = "Controller 3 Rumble",
                .get = rumble_get_2,
                .set = rumble_set_2,
            },
    },
    {
        .type = WidgetType_GetSetCheckbox,
        .get_set_checkbox =
            {
                .label = "Controller 4 Rumble",
                .get = rumble_get_3,
                .set = rumble_set_3,
            },
    },
};

static Widget s_about_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"SMB2 Practice Mod"},
    },
    {
        .type = WidgetType_Text,
        .text = {"  Made with   by"},
    },
    {
        .type = WidgetType_Custom,
        .custom = {Draw_Heart},
    },
    {
        .type = WidgetType_ColoredText,
        .colored_text =
            {
                .label = "ComplexPlane",
                .color_left = {0x00, 0xde, 0x38, 0xff},
                .color_right = {0x0a, 0xf2, 0xef, 0xff},
                .offset_x = 17,
                .overlap = true,
            },
    },
    {
        .type = WidgetType_Text,
        .text = {"  With contributions from:        ,      "},
    },
    {
        .type = WidgetType_ColoredText,
        .colored_text =
            {
                .label = "rehtrop",
                .color_left = {0x69, 0xe3, 0xff, 0xff},
                .color_right = {0xdd, 0x9b, 0xdc, 0xff},
                .offset_x = 27,
                .overlap = true,
            },
    },
    {
        .type = WidgetType_ColoredText,
        .colored_text =
            {
                .label = "Nambo",
                .color_left = {0xd0, 0x67, 0xff, 0xff},
                .color_right = {0xf3, 0xe3, 0xff, 0xff},
                .offset_x = 36,
                .overlap = true,
            },
    },
    {WidgetType_Separator},

    {
        .type = WidgetType_Header,
        .header = {"Updates"},
    },
    {
        .type = WidgetType_Text,
        .text = {"  Version:        "},
    },
    {
        .type = WidgetType_ColoredText,
        .colored_text =
            {
                .label = s_version_str,
                .color_left = COLOR_LIGHT_GRAY,
                .color_right = COLOR_LIGHT_GRAY,
                .offset_x = 19,
                .overlap = true,
            },
    },
    {
        .type = WidgetType_Text,
        .text = {"  Git commit:      "},
    },
    {
        .type = WidgetType_ColoredText,
        .colored_text =
            {
                .label = GIT_HASH,
                .color_left = COLOR_LIGHT_GRAY,
                .color_right = COLOR_LIGHT_GRAY,
                .offset_x = 19,
                .overlap = true,
            },
    },
    {
        .type = WidgetType_Separator,
    },
    {
        .type = WidgetType_Text,
        .text = {"  For the latest version of SMB2 Practice Mod:"},
    },
    {
        .type = WidgetType_ColoredText,
        .colored_text =
            {
                .label = "  https://github.com/ComplexPlane/SMB2PracticeMod",
                .color_left = COLOR_BLUE,
                .color_right = COLOR_BLUE,
            },
    },
};

static Widget s_savestates_help_widgets[] = {
    {.type = WidgetType_Text, .text = {"  X          \x1c Create savestate"}},
    {.type = WidgetType_Text, .text = {"  Y          \x1c Load savestate"}},
    {.type = WidgetType_Text, .text = {"  C-Stick    \x1c Change savestate slot"}},
    // TODO: replace this feature with a better one that works in-menu
    {.type = WidgetType_Text, .text = {"  L+X or R+X \x1c Frame advance"}},
    {.type = WidgetType_Text, .text = {"  L+C or R+C \x1c Browse savestates"}},
};

static Widget s_iw_help_widgets[] = {
    {.type = WidgetType_Text, .text = {"  Story Mode individual world segments can"}},
    {.type = WidgetType_Text, .text = {"  be done by going to file select in Story"}},
    {.type = WidgetType_Text, .text = {"  Mode, pressing up/down on an empty file"}},
    {.type = WidgetType_Text, .text = {"  to select a world, and selecting the file."}},
};

static Widget s_jump_help_widgets[] = {
    {.type = WidgetType_Text, .text = {"  A          \x1c Jump"}},
    {.type = WidgetType_Text, .text = {"  B          \x1c Resize minimap"}},
};

static Widget s_freecam_help_widgets[] = {
    {.type = WidgetType_Text, .text = {"  Left Stick \x1c Move"}},
    {.type = WidgetType_Text, .text = {"  C-Stick    \x1c Rotate"}},
    {.type = WidgetType_Text, .text = {"  L          \x1c Down"}},
    {.type = WidgetType_Text, .text = {"  R          \x1c Up"}},
    {.type = WidgetType_Text, .text = {"  Y          \x1c Turbo Speed"}},
    {.type = WidgetType_Text, .text = {"  X          \x1c Slow Speed"}},
    {.type = WidgetType_Text, .text = {"  D-pad Up   \x1c Increase Turbo Speed"}},
    {.type = WidgetType_Text, .text = {"  D-pad Down \x1c Decrease Turbo Speed"}},
};

static Widget s_iw_mark_help_widgets[] = {
    {.type = WidgetType_Text, .text = {"  The IL validation mark is shown after"}},
    {.type = WidgetType_Text, .text = {"  completing a level. It is green only if no"}},
    {.type = WidgetType_Text, .text = {"  gameplay-altering mods were used during the"}},
    {.type = WidgetType_Text, .text = {"  level completion, and red otherwise."}},
    {WidgetType_Separator},

    {.type = WidgetType_Text, .text = {"  Showing this watermark is currently required"}},
    {.type = WidgetType_Text, .text = {"  to submit IL speedruns of vanilla SMB2 to the"}},
    {.type = WidgetType_Text, .text = {"  official leaderboards if you are using the"}},
    {.type = WidgetType_Text, .text = {"  SMB2 Practice Mod."}},
    {WidgetType_Separator},

    {.type = WidgetType_Text, .text = {"  Note that some visual-only mods may also be"}},
    {.type = WidgetType_Text, .text = {"  disallowed for IL leaderboard submissions."}},
    {.type = WidgetType_Text, .text = {"  The IL validation mark does not account for"}},
    {.type = WidgetType_Text, .text = {"  these visual-only mods."}},
    {WidgetType_Separator},

    {.type = WidgetType_Text, .text = {"  If you complete a stage without the watermark"}},
    {.type = WidgetType_Text, .text = {"  enabled, you can still enable it post-goal to"}},
    {.type = WidgetType_Text, .text = {"  validate your run."}},
};

static const char *CHARA_CHOICES[] = {"AiAi", "MeeMee", "Baby", "GonGon", "Random"};

static void push_cm_beg_1_10() {
    CM_RequestSeg(CM_Seg_Beginner1);
}
static void push_cm_beg_ex_1_10() {
    CM_RequestSeg(CM_Seg_BeginnerExtra);
}

static Widget s_cm_beg_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Segment Selection"},
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Beginner 1-10",
                .push = push_cm_beg_1_10,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Beginner Extra 1-10",
                .push = push_cm_beg_ex_1_10,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static void push_cm_adv_1_10() {
    CM_RequestSeg(CM_Seg_Advanced1);
}
static void push_cm_adv_11_20() {
    CM_RequestSeg(CM_Seg_Advanced11);
}
static void push_cm_adv_21_30() {
    CM_RequestSeg(CM_Seg_Advanced21);
}
static void push_cm_adv_ex_1_10() {
    CM_RequestSeg(CM_Seg_AdvancedExtra);
}

static Widget s_cm_adv_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Segment Selection"},
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Advanced 1-10",
                .push = push_cm_adv_1_10,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Advanced 11-20",
                .push = push_cm_adv_11_20,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Advanced 21-30",
                .push = push_cm_adv_21_30,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Advanced Extra 1-10",
                .push = push_cm_adv_ex_1_10,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static void push_cm_exp_1_10() {
    CM_RequestSeg(CM_Seg_Expert1);
}
static void push_cm_exp_11_20() {
    CM_RequestSeg(CM_Seg_Expert11);
}
static void push_cm_exp_21_30() {
    CM_RequestSeg(CM_Seg_Expert21);
}
static void push_cm_exp_31_40() {
    CM_RequestSeg(CM_Seg_Expert31);
}
static void push_cm_exp_41_50() {
    CM_RequestSeg(CM_Seg_Expert41);
}
static void push_cm_exp_ex_1_10() {
    CM_RequestSeg(CM_Seg_ExpertExtra);
}

static Widget s_cm_exp_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Segment Selection"},
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 1-10",
                .push = push_cm_exp_1_10,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 11-20",
                .push = push_cm_exp_11_20,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 21-30",
                .push = push_cm_exp_21_30,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 31-40",
                .push = push_cm_exp_31_40,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 41-50",
                .push = push_cm_exp_41_50,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert Extra 1-10",
                .push = push_cm_exp_ex_1_10,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static void push_cm_mas_1_10() {
    CM_RequestSeg(CM_Seg_Master1);
}
static void push_cm_mas_ex_1_10() {
    CM_RequestSeg(CM_Seg_MasterExtra);
}

static Widget s_cm_mas_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Segment Selection"},
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Master 1-10",
                .push = push_cm_mas_1_10,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Master Extra 1-10",
                .push = push_cm_mas_ex_1_10,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static Widget s_cm_seg_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Segment Selection"},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Beginner", s_cm_beg_widgets, LEN(s_cm_beg_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Advanced", s_cm_adv_widgets, LEN(s_cm_adv_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Expert", s_cm_exp_widgets, LEN(s_cm_exp_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Master", s_cm_mas_widgets, LEN(s_cm_mas_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Story Mode", s_iw_help_widgets, LEN(s_iw_help_widgets)},
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Segment Options"},
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Character",
                .choices = CHARA_CHOICES,
                .num_choices = LEN(CHARA_CHOICES),
                .pref = Pref_CmChara,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Text,
        .text = {"  Segments may crash in some romhacks"},
    },
};

static Widget s_timers_widgets[] = {
    {.type = WidgetType_Header, .header = {"Realtime Timers"}},
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Realtime (RTA)",
                .pref = Pref_TimerShowRTA,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Pausetime (PAU)",
                .pref = Pref_TimerShowPause,
            },
    },
    {WidgetType_Separator},
    {.type = WidgetType_Header, .header = {"Subtick Timers"}},
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Framesave (FSV)",
                .pref = Pref_TimerShowFramesave,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Subtick (SUB)",
                .pref = Pref_TimerShowSubtick,
            },
    },
    {WidgetType_Separator},
    {.type = WidgetType_Header, .header = {"Segment Timers"}},
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Story Mode IWs (IW)",
                .pref = Pref_IwTimer,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "CM Segments (SEG)",
                .pref = Pref_CmTimer,
            },
    },
};

static Widget s_sound_widgets[] = {
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Mute Background Music",
                .pref = Pref_MuteBgm,
            },
    },
    {.type = WidgetType_Text, .text = {"  To apply background music setting:"}},
    {.type = WidgetType_Text, .text = {"  Wait 2s then reset console"}},
    {WidgetType_Separator},

    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Mute Timer Ding",
                .pref = Pref_MuteTimerDing,
            },
    },
};

static Widget s_unlock_confirm_widgets[] = {
    {
        .type = WidgetType_Text,
        .text = {"  This will unlock all levels, lives, etc."},
    },
    {
        .type = WidgetType_Text,
        .text = {"  Save your game to make this persistent."},
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Cancel",
                .push = nullptr,
                .flags = ButtonFlag_GoBack,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Confirm",
                .push = Unlock_Everything,
                .flags = ButtonFlag_GoBack,
            },
    },
};

static Widget s_unlock_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Unlock Progress For This Session"},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Unlock Now", s_unlock_confirm_widgets, LEN(s_unlock_confirm_widgets)},
    },
    {WidgetType_Separator},

    {
        .type = WidgetType_Header,
        .header = {"Always Unlock Progress"},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "For Vanilla SMB2",
                .pref = Pref_UnlockVanilla,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "For Romhacks",
                .pref = Pref_UnlockRomhacks,
            },
    },
    {
        .type = WidgetType_Text,
        .text = {"  Applied on game startup."},
    },
};

static Widget s_freecam_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Freecam Toggle"},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Freecam",
                .pref = Pref_Freecam,
            },
    },
    {
        .type = WidgetType_InputSelect,
        .input_select =
            {
                .label = "Toggle Bind",
                .pref = Pref_FreecamToggleBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Configuration"},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Invert Yaw",
                .pref = Pref_FreecamInvertYaw,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Invert Pitch",
                .pref = Pref_FreecamInvertPitch,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Turbo Speed Factor",
                .pref = Pref_FreecamSpeedMult,
                .min = Freecam_TURBO_SPEED_MIN,
                .max = Freecam_TURBO_SPEED_MAX,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Freeze Timer",
                .pref = Pref_FreecamFreezeTimer,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Hide HUD",
                .pref = Pref_FreecamHideHud,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Help"},
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Controller Binds",
                .widgets = s_freecam_help_widgets,
                .num_widgets = LEN(s_freecam_help_widgets),
            },
    },
};

static Widget s_hide_widgets[] = {
    {.type = WidgetType_Header, .header = {"Hide Elements"}},
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "HUD",
                .pref = Pref_HideHud,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Ball",
                .pref = Pref_HideBall,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Stage Models",
                .pref = Pref_HideStage,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Stage Items",
                .pref = Pref_HideItems,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Stage Objects",
                .pref = Pref_HideStobjs,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Effects",
                .pref = Pref_HideEffects,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Background",
                .pref = Pref_HideBg,
            },
    },
};

static const char *SAVESTATE_SAVE_TO_OPTIONS[] = {"Selected Slot", "Next Empty Slot",
                                                  "Empty, Then Oldest"};

static Widget s_savestate_subwidgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Save To",
                .choices = SAVESTATE_SAVE_TO_OPTIONS,
                .num_choices = LEN(SAVESTATE_SAVE_TO_OPTIONS),
                .pref = Pref_SavestateSaveTo,
            },
    },
    {
        .type = WidgetType_InputSelect,
        .input_select =
            {
                .label = "Clear Savestate Bind",
                .pref = Pref_SavestateClearBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
    {
        .type = WidgetType_InputSelect,
        .input_select =
            {
                .label = "Clear All Bind",
                .pref = Pref_SavestateClearAllBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
};

static bool show_if_savestates_enabled() {
    return Pref_Get(Pref_Savestates) != 0;
}

static Widget s_savestate_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Savestates"},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Enable Savestates",
                .pref = Pref_Savestates,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_savestate_subwidgets,
                .num_widgets = LEN(s_savestate_subwidgets),
                .show_if = show_if_savestates_enabled,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Help"},
    },
    {
        .type = WidgetType_Menu,
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
        .type = WidgetType_Button,
        .button =
            {
                .label = "Go To Story Mode",
                .push = GoToStory_LoadStoryMode,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Speedrun Segments", s_cm_seg_widgets, LEN(s_cm_seg_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Savestates",
                .widgets = s_savestate_widgets,
                .num_widgets = LEN(s_savestate_widgets),
            },
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Freecam",
                .widgets = s_freecam_widgets,
                .num_widgets = LEN(s_freecam_widgets),
            },
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Hide Elements",
                .widgets = s_hide_widgets,
                .num_widgets = LEN(s_hide_widgets),
            },
    },
    {.type = WidgetType_Menu, .menu = {"Rumble", s_rumble_widgets, LEN(s_rumble_widgets)}},
    {
        .type = WidgetType_Menu,
        .menu = {"Audio", s_sound_widgets, LEN(s_sound_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Progress Unlock", s_unlock_widgets, LEN(s_unlock_widgets)},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Debug Mode",
                .pref = Pref_DebugMode,
            },
    },
};

static Widget s_reset_ilmark_widgets[] = {
    {
        .type = WidgetType_Text,
        .text = {"  Reset IL invalidating preferences to defaults?"},
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Cancel",
                .push = nullptr,
                .flags = ButtonFlag_GoBack,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Confirm",
                .push = Validate_DisableInvalidatingSettings,
                .flags = ButtonFlag_GoBack,
            },
    },
};

static Widget s_il_mark_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Disable IL Invalidating Settings"},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Disable Now", s_reset_ilmark_widgets, LEN(s_reset_ilmark_widgets)},
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Show IL Validation Mark in..."},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Practice Mode",
                .pref = Pref_IlMarkPractice,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Story Mode",
                .pref = Pref_IlMarkStory,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Challenge Mode",
                .pref = Pref_IlMarkChallenge,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Romhacks",
                .pref = Pref_IlMarkRomhacks,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Help"},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"About", s_iw_mark_help_widgets, LEN(s_iw_mark_help_widgets)},
    },
};

static Widget s_displays_widgets[] = {
    {
        .type = WidgetType_Menu,
        .menu = {"Input Display", s_inputdisp_widgets, LEN(s_inputdisp_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Timers", s_timers_widgets, LEN(s_timers_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"IL Battle", s_il_battle_widgets, LEN(s_il_battle_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"IL Validation Mark", s_il_mark_widgets, LEN(s_il_mark_widgets)},
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "9999 Banana Counter",
                .pref = Pref_BananaCounter9999,
            },
    },
};

static const char *PHYSICS_PRESETS[] = {"Default",       "Light Ball",      "No Friction",
                                        "Heavy Ball",    "Bouncy Ball",     "Sticky Ball",
                                        "High Friction", "Jump-Mod Physics"};

static Widget s_lightball[] = {{.type = WidgetType_Text, .text = {"  Weight: 1.00 -> 0.95"}}};
static Widget s_nofriction[] = {{.type = WidgetType_Text, .text = {"  Friction: 0.010 -> 0.000"}}};
static Widget s_heavyball[] = {{.type = WidgetType_Text, .text = {"  Weight: 1.00 -> 1.05"}}};
static Widget s_bouncyball[] = {{.type = WidgetType_Text, .text = {"  Restitution: 0.50 -> 1.20"}}};
static Widget s_stickyball[] = {{.type = WidgetType_Text, .text = {"  Restitution: 0.50 -> 0.01"}}};
static Widget s_high_friction[] = {
    {.type = WidgetType_Text, .text = {"  Friction: 0.010 -> 0.020"}},
};

static void push_reset_prefs() {
    Pref_ResetAllDefaults();
    Pref_Save();
}

static Widget s_jump_physics[] = {
    {.type = WidgetType_Text, .text = {"  Friction: 0.010 -> 0.015"}},
    {.type = WidgetType_Text, .text = {"  Restitution: 0.50 -> 0.25"}},
};

static bool show_if_physics_light_ball() {
    return (PhysicsPreset)Pref_Get(Pref_PhysicsPreset) == PhysicsPreset_LightBall;
}

static bool show_if_physics_no_friction() {
    return (PhysicsPreset)Pref_Get(Pref_PhysicsPreset) == PhysicsPreset_NoFriction;
}

static bool show_if_physics_heavy_ball() {
    return (PhysicsPreset)Pref_Get(Pref_PhysicsPreset) == PhysicsPreset_HeavyBall;
}

static bool show_if_physics_bouncy_ball() {
    return (PhysicsPreset)Pref_Get(Pref_PhysicsPreset) == PhysicsPreset_BouncyBall;
}

static bool show_if_physics_sticky_ball() {
    return (PhysicsPreset)Pref_Get(Pref_PhysicsPreset) == PhysicsPreset_StickyBall;
}

static bool show_if_physics_high_friction() {
    return (PhysicsPreset)Pref_Get(Pref_PhysicsPreset) == PhysicsPreset_HighFriction;
}

static bool show_if_physics_jump() {
    return (PhysicsPreset)Pref_Get(Pref_PhysicsPreset) == PhysicsPreset_JumpPhysics;
}

static Widget s_physics_widgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Physics Presets",
                .choices = PHYSICS_PRESETS,
                .num_choices = LEN(PHYSICS_PRESETS),
                .pref = Pref_PhysicsPreset,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_lightball,
                .num_widgets = LEN(s_lightball),
                .show_if = show_if_physics_light_ball,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_nofriction,
                .num_widgets = LEN(s_nofriction),
                .show_if = show_if_physics_no_friction,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_heavyball,
                .num_widgets = LEN(s_heavyball),
                .show_if = show_if_physics_heavy_ball,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_bouncyball,
                .num_widgets = LEN(s_bouncyball),
                .show_if = show_if_physics_bouncy_ball,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_stickyball,
                .num_widgets = LEN(s_stickyball),
                .show_if = show_if_physics_sticky_ball,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_high_friction,
                .num_widgets = LEN(s_high_friction),
                .show_if = show_if_physics_high_friction,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_jump_physics,
                .num_widgets = LEN(s_jump_physics),
                .show_if = show_if_physics_jump,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Display Physics Text",
                .pref = Pref_CustomPhysicsDisp,
            },
    },
};

static const char *STAGE_EDIT_VARIANTS[] = {"None", "Golden Banana", "Dark Banana", "Reverse Mode"};

static void push_select_new_goal() {
    StageEdits_SelectNewGoal();
}

static bool show_if_stage_edit_reverse() {
    return Pref_Get(Pref_StageEditVariant) == 3;
}

static Widget s_reverse_goal_widgets[] = {
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Select New Goal",
                .push = push_select_new_goal,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static Widget s_stage_edit_widgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Stage Edit Mode",
                .choices = STAGE_EDIT_VARIANTS,
                .num_choices = LEN(STAGE_EDIT_VARIANTS),
                .pref = Pref_StageEditVariant,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_reverse_goal_widgets,
                .num_widgets = LEN(s_reverse_goal_widgets),
                .show_if = show_if_stage_edit_reverse,
            },
    },
    {
        .type = WidgetType_Text,
        .text = {"  Stage Edits are activated on retry"},
    },
};

static const char *JUMP_COUNTS[] = {"One", "Two", "Infinite"};

static Widget s_jump_classic_widgets[] = {
    {
        .type = WidgetType_Text,
        .text = {"  Classic Jump-Mod from its original release"},
    },
    {WidgetType_Separator},
    {.type = WidgetType_Header, .header = {"Configuration"}},
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Use Changed Physics",
                .pref = Pref_JumpChangePhysics,
            },
    },
};

static Widget s_jump_standard_widgets[] = {
    {
        .type = WidgetType_Text,
        .text = {"  Standard Jump-Mod"},
    },
    {WidgetType_Separator},
    {.type = WidgetType_Header, .header = {"Configuration"}},
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Use Changed Physics",
                .pref = Pref_JumpChangePhysics,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Allow Walljumps",
                .pref = Pref_JumpAllowWalljumps,
            },
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Jump Count",
                .choices = JUMP_COUNTS,
                .num_choices = LEN(JUMP_COUNTS),
                .pref = Pref_JumpCount,
            },
    },
};

static const char *JUMP_PROFILES[] = {"Standard", "Classic"};

static bool show_if_jump_profile_standard() {
    return Pref_Get(Pref_JumpProfile) == 0;
}

static bool show_if_jump_profile_classic() {
    return Pref_Get(Pref_JumpProfile) == 1;
}

static Widget s_jump_profiles[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Jump Profile",
                .choices = JUMP_PROFILES,
                .num_choices = LEN(JUMP_PROFILES),
                .pref = Pref_JumpProfile,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_jump_standard_widgets,
                .num_widgets = LEN(s_jump_standard_widgets),
                .show_if = show_if_jump_profile_standard,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_jump_classic_widgets,
                .num_widgets = LEN(s_jump_classic_widgets),
                .show_if = show_if_jump_profile_classic,
            },
    },
};

static bool show_if_jump_enabled() {
    return Pref_Get(Pref_JumpMod) != 0;
}

static Widget s_jump_widgets[] = {
    {.type = WidgetType_Header, .header = {"Jump Settings"}},
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Jump-Mod",
                .pref = Pref_JumpMod,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_jump_profiles,
                .num_widgets = LEN(s_jump_profiles),
                .show_if = show_if_jump_enabled,
            },
    },
    {
        .type = WidgetType_Header,
        .header = {"Help"},
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Controller Binds",
                .widgets = s_jump_help_widgets,
                .num_widgets = LEN(s_jump_help_widgets),
            },
    },
};

static const char *TIMER_TYPES[] = {"Default", "Frozen", "Freeze at 0", "Count up from 0"};
static const char *FALLOUT_PLANE_TYPE[] = {"Normal", "Disabled", "Bouncy"};

static Widget s_variant_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Modifications"},
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Camera Type",
                .choices = CAMERA_OPTIONS,
                .num_choices = LEN(CAMERA_OPTIONS),
                .pref = Pref_Camera,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "D-Pad Controls",
                .pref = Pref_DpadControls,
            },
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Physics",
                .widgets = s_physics_widgets,
                .num_widgets = LEN(s_physics_widgets),
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Assist"},
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Timer Type",
                .choices = TIMER_TYPES,
                .num_choices = LEN(TIMER_TYPES),
                .pref = Pref_TimerType,
            },
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Fallout Plane Type",
                .choices = FALLOUT_PLANE_TYPE,
                .num_choices = LEN(FALLOUT_PLANE_TYPE),
                .pref = Pref_FalloutPlaneType,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Disable Fallout Volume",
                .pref = Pref_DisableFalloutVolumes,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Header,
        .header = {"Alternate Modes"},
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Jump-Mod",
                .widgets = s_jump_widgets,
                .num_widgets = LEN(s_jump_widgets),
            },
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Stage Edits",
                .widgets = s_stage_edit_widgets,
                .num_widgets = LEN(s_stage_edit_widgets),
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Marathon Mode",
                .pref = Pref_Marathon,
            },
    },
};

static Widget s_reset_prefs_widgets[] = {
    {
        .type = WidgetType_Text,
        .text = {"  Reset all preferences to defaults?"},
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Cancel",
                .push = nullptr,
                .flags = ButtonFlag_GoBack,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Confirm",
                .push = push_reset_prefs,
                .flags = ButtonFlag_GoBack,
            },
    },
};

static const char *RGB_FORMAT_OPTIONS[] = {"Decimal", "Hex"};

static Widget s_pracmod_settings_widgets[] = {
    {
        .type = WidgetType_InputSelect,
        .input_select =
            {
                .label = "Menu Bind",
                .pref = Pref_MenuBind,
                .required_chord = true,
                .can_unbind = false,
            },
    },
    {.text = {"  Menu Bind Requires 2 Buttons"}},
    {WidgetType_Separator},
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "RGB Format",
                .choices = RGB_FORMAT_OPTIONS,
                .num_choices = LEN(RGB_FORMAT_OPTIONS),
                .pref = Pref_RgbFormat,
            },
    },
    {WidgetType_Separator},
    {
        .type = WidgetType_Menu,
        .menu = {"Restore Defaults", s_reset_prefs_widgets, LEN(s_reset_prefs_widgets)},
    },
    {WidgetType_Separator},
};

static Widget s_root_widgets[] = {
    {
        .type = WidgetType_Menu,
        .menu = {"Tools", s_tools_widgets, LEN(s_tools_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Displays", s_displays_widgets, LEN(s_displays_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Customization", s_ball_color_widgets, LEN(s_ball_color_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Variants", s_variant_widgets, LEN(s_variant_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Settings", s_pracmod_settings_widgets, LEN(s_pracmod_settings_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"About", s_about_widgets, LEN(s_about_widgets)},
    },
};

MenuWidget menu_root = {
    .label = "Main Menu",
    .widgets = s_root_widgets,
    .num_widgets = LEN(s_root_widgets),
};

void MenuDefn_Init() {
    mkb_sprintf(s_version_str, "v%s", Version_GetStr());
}
