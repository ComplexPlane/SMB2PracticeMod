#include "menu_defn.h"
#include <stddef.h>

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
#include "utils/base.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

// TODO update buttons with close menu flag
// TODO let buttons have null push()

static char s_version_str[36];

static const char *INPUTDISP_COLORS[] = {
    "Purple", "Red", "Orange", "Yellow", "Green", "Blue", "Pink", "Black",
};
static_assert(LEN(INPUTDISP_COLORS) == inputdisp_NUM_COLORS);

static const char *CAMERA_OPTIONS[] = {"Default", "Force SMB2", "Force SMB1"};

static const char *INPUTDISP_COLOR_TYPE_OPTIONS[] = {
    "Preset",
    "RGB Selector",
    "Rainbow",
    "Match Ball",
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
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Red Value",
                .pref = Pref_InputDispRed,
                .min = ballcolor_COLOR_MIN,
                .max = ballcolor_COLOR_MAX,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Green Value",
                .pref = Pref_InputDispGreen,
                .min = ballcolor_COLOR_MIN,
                .max = ballcolor_COLOR_MAX,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Blue Value",
                .pref = Pref_InputDispBlue,
                .min = ballcolor_COLOR_MIN,
                .max = ballcolor_COLOR_MAX,
            },
    },
};

static bool inputdisp_uses_preset_color() {
    return Pref_Get(Pref_InputDispColorType) == 0;
}
static bool inputdisp_uses_rgb_color() {
    return Pref_Get(Pref_InputDispColorType) == 1;
}

static Widget s_inputdisp_subwidgets[] = {
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Use Center Location",
                .pref = Pref_InputDispCenterLocation,
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
                .show_if = inputdisp_uses_preset_color,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_input_hex,
                .num_widgets = LEN(s_input_hex),
                .show_if = inputdisp_uses_rgb_color,
            },
    },
};

static bool inputdisp_is_enabled() {
    return Pref_Get(Pref_InputDisp);
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
                .show_if = inputdisp_is_enabled,
            },
    },
};

static const char *BALL_COLORS[] = {
    "Default", "Red", "Blue", "Yellow", "Green", "Teal", "Pink", "Black", "White",
};
static_assert(LEN(BALL_COLORS) == ballcolor_NUM_COLORS);

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
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Red Value",
                .pref = Pref_BallRed,
                .min = ballcolor_COLOR_MIN,
                .max = ballcolor_COLOR_MAX,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Green Value",
                .pref = Pref_BallGreen,
                .min = ballcolor_COLOR_MIN,
                .max = ballcolor_COLOR_MAX,
            },
    },
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Blue Value",
                .pref = Pref_BallBlue,
                .min = ballcolor_COLOR_MIN,
                .max = ballcolor_COLOR_MAX,
            },
    },
};

static bool ball_uses_preset_color() {
    return Pref_Get(Pref_BallColorType) == 0;
}
static bool ball_uses_rgb_color() {
    return Pref_Get(Pref_BallColorType) == 1;
}
static bool ape_uses_preset_color() {
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
                .show_if = ball_uses_preset_color,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_hex_widgets,
                .num_widgets = LEN(s_hex_widgets),
                .show_if = ball_uses_rgb_color,
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
                .show_if = ape_uses_preset_color,
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
static_assert(LEN(IL_BATTLE_LENGTHS) == ilbattle_NUM_LENGTHS);

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

static bool il_battle_score_is_shown() {
    return Pref_Get(Pref_IlBattleShowScore);
}

static Widget s_il_battle_subwidgets[] = {
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
                .show_if = il_battle_score_is_shown,
            },
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
                .label = "Old Buzzer Message",
                .pref = Pref_IlBattleBuzzerOld,
            },
    },
    {.type = WidgetType_Text, .text = {"  Press Ready Bind then Retry to start a battle"}},
};

static bool il_battle_display_is_enabled() {
    return Pref_Get(Pref_IlBattleDisplay);
}

static Widget s_il_battle_widgets[] = {
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
                .show_if = il_battle_display_is_enabled,
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

static bool controller_1_rumble_get() {
    return rumble_get(0);
}
static void controller_1_rumble_set(bool enable) {
    rumble_set(0, enable);
}
static bool controller_2_rumble_get() {
    return rumble_get(1);
}
static void controller_2_rumble_set(bool enable) {
    rumble_set(1, enable);
}
static bool controller_3_rumble_get() {
    return rumble_get(2);
}
static void controller_3_rumble_set(bool enable) {
    rumble_set(2, enable);
}
static bool controller_4_rumble_get() {
    return rumble_get(3);
}
static void controller_4_rumble_set(bool enable) {
    rumble_set(3, enable);
}

static Widget s_rumble_widgets[] = {
    {
        .type = WidgetType_GetSetCheckbox,
        .get_set_checkbox =
            {
                .label = "Controller 1 Rumble",
                .get = controller_1_rumble_get,
                .set = controller_1_rumble_set,
            },
    },
    {
        .type = WidgetType_GetSetCheckbox,
        .get_set_checkbox =
            {
                .label = "Controller 2 Rumble",
                .get = controller_2_rumble_get,
                .set = controller_2_rumble_set,
            },
    },
    {
        .type = WidgetType_GetSetCheckbox,
        .get_set_checkbox =
            {
                .label = "Controller 3 Rumble",
                .get = controller_3_rumble_get,
                .set = controller_3_rumble_set,
            },
    },
    {
        .type = WidgetType_GetSetCheckbox,
        .get_set_checkbox =
            {
                .label = "Controller 4 Rumble",
                .get = controller_4_rumble_get,
                .set = controller_4_rumble_set,
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
        .text = {"  Made with   by ComplexPlane"},
    },
    {
        .type = WidgetType_Custom,
        .custom = {Draw_Heart},
    },
    {
        .type = WidgetType_Text,
        .text = {"  With contributions from rehtrop & Nambo"},
    },
    {.type = WidgetType_Separator},

    {
        .type = WidgetType_Header,
        .header = {"Updates"},
    },
    {
        .type = WidgetType_Text,
        .text = {s_version_str},
    },
    {
        .type = WidgetType_Text,
        .text = {"  For the latest version of SMB2 Practice Mod:"},
    },
    {
        .type = WidgetType_ColoredText,
        .colored_text = {" github.com/ComplexPlane/SMB2PracticeMod/releases", COLOR_BLUE},
    },
};

static const char *CHARA_CHOICES[] = {"AiAi", "MeeMee", "Baby", "GonGon", "Random"};

static void start_beginner_1_10_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Beginner1);
}
static void start_beginner_extra_segment() {
    cmseg_request_cm_seg(cmseg_Seg_BeginnerExtra);
}

static Widget s_cm_beg_widgets[] = {
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Beginner 1-10",
                .push = start_beginner_1_10_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Beginner Extra 1-10",
                .push = start_beginner_extra_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static void start_advanced_1_10_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Advanced1);
}
static void start_advanced_11_20_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Advanced11);
}
static void start_advanced_21_30_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Advanced21);
}
static void start_advanced_extra_segment() {
    cmseg_request_cm_seg(cmseg_Seg_AdvancedExtra);
}

static Widget s_cm_adv_widgets[] = {
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Advanced 1-10",
                .push = start_advanced_1_10_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Advanced 11-20",
                .push = start_advanced_11_20_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Advanced 21-30",
                .push = start_advanced_21_30_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Advanced Extra 1-10",
                .push = start_advanced_extra_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static void start_expert_1_10_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Expert1);
}
static void start_expert_11_20_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Expert11);
}
static void start_expert_21_30_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Expert21);
}
static void start_expert_31_40_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Expert31);
}
static void start_expert_41_50_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Expert41);
}
static void start_expert_extra_segment() {
    cmseg_request_cm_seg(cmseg_Seg_ExpertExtra);
}

static Widget s_cm_exp_widgets[] = {
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 1-10",
                .push = start_expert_1_10_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 11-20",
                .push = start_expert_11_20_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 21-30",
                .push = start_expert_21_30_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 31-40",
                .push = start_expert_31_40_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert 41-50",
                .push = start_expert_41_50_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Expert Extra 1-10",
                .push = start_expert_extra_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static void start_master_1_10_segment() {
    cmseg_request_cm_seg(cmseg_Seg_Master1);
}
static void start_master_extra_segment() {
    cmseg_request_cm_seg(cmseg_Seg_MasterExtra);
}

static Widget s_cm_mas_widgets[] = {
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Master 1-10",
                .push = start_master_1_10_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Master Extra 1-10",
                .push = start_master_extra_segment,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static Widget s_cm_seg_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Segment Selection"},
    },
    // Beginner
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
    {.type = WidgetType_Separator},
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
    {.type = WidgetType_Separator},
    {
        .type = WidgetType_Text,
        .text = {"Segments may crash in some romhacks"},
    },
};

static const char *TIMER_OPTIONS[] = {
    "Don't show",
    "Always show",
    "Between worlds",
    "End of run",
};

static Widget s_loadless_timers_widgets[] = {
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Fullgame Timer",
                .choices = TIMER_OPTIONS,
                .num_choices = LEN(TIMER_OPTIONS),
                .pref = Pref_FullgameTimerOptions,
            },
    },
    {
        .type = WidgetType_Choose,
        .choose =
            {
                .label = "Segment Timer",
                .choices = TIMER_OPTIONS,
                .num_choices = LEN(TIMER_OPTIONS),
                .pref = Pref_SegmentTimerOptions,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Timer Not On Warning",
                .pref = Pref_StoryTimerWarning,
            },
    },
};

static Widget s_timers_widgets[] = {
    // I might want to reorganize this with the addition of a loadless timer
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
    {.type = WidgetType_Separator},
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
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Unrounded (CUR/NXT)",
                .pref = Pref_TimerShowUnrounded,
            },
    },
    {.type = WidgetType_Separator},
    {.type = WidgetType_Header, .header = {"Segment & Loadless Timers"}},
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
    {
        .type = WidgetType_Menu,
        .menu = {"Loadless Timers", s_loadless_timers_widgets, LEN(s_loadless_timers_widgets)},
    },
};

static Widget s_savestates_help_widgets[] = {
    {.type = WidgetType_Text, .text = {"  X          \x1c Create savestate"}},
    {.type = WidgetType_Text, .text = {"  Y          \x1c Load savestate"}},
    {.type = WidgetType_Text, .text = {"  C-Stick    \x1c Change savestate slot"}},
    // TODO replace this feature with a better one that works in-menu
    {.type = WidgetType_Text, .text = {"  L+X or R+X \x1c Frame advance"}},
    {.type = WidgetType_Text, .text = {"  L+C or R+C \x1c Browse savestates"}},
};

static Widget s_iw_help_widgets[] = {
    {.type = WidgetType_Text, .text = {"  Go to Story Mode, then press up/down on"}},
    {.type = WidgetType_Text, .text = {"  a file to choose a world."}},
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
    {.type = WidgetType_Text, .text = {"  completing a level. It is green only if no "}},
    {.type = WidgetType_Text, .text = {"  gameplay-altering mods were used during the "}},
    {.type = WidgetType_Text, .text = {"  level completion, and red otherwise."}},
    {.type = WidgetType_Separator},

    {.type = WidgetType_Text, .text = {"  Showing this watermark is currently required"}},
    {.type = WidgetType_Text, .text = {"  to submit IL speedruns of vanilla SMB2 to the"}},
    {.type = WidgetType_Text, .text = {"  official leaderboards if you are using the"}},
    {.type = WidgetType_Text, .text = {"  SMB2 Practice Mod."}},
    {.type = WidgetType_Separator},

    {.type = WidgetType_Text, .text = {"  Note that some visual-only mods, such as raw"}},
    {.type = WidgetType_Text, .text = {"  input display, may also be disallowed for"}},
    {.type = WidgetType_Text, .text = {"  IL leaderboard submissions. The IL validation"}},
    {.type = WidgetType_Text, .text = {"  mark does not account for visual-only mods."}},
    {.type = WidgetType_Separator},

    {.type = WidgetType_Text, .text = {"  If you complete a stage without the watermark"}},
    {.type = WidgetType_Text, .text = {"  enabled, you can still enable it post-goal to"}},
    {.type = WidgetType_Text, .text = {"  validate your run."}},
    {.type = WidgetType_Separator},
};

static Widget s_help_widgets[] = {
    {
        .type = WidgetType_Menu,
        .menu = {"Savestates", s_savestates_help_widgets, LEN(s_savestates_help_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Jump Mod", s_jump_help_widgets, LEN(s_jump_help_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Freecam", s_freecam_help_widgets, LEN(s_freecam_help_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Story Mode IWs", s_iw_help_widgets, LEN(s_iw_help_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"IL Validation Mark", s_iw_mark_help_widgets, LEN(s_iw_mark_help_widgets)},
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
    {.type = WidgetType_Separator},

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
                .push = NULL,
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
    {.type = WidgetType_Separator},

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

static Widget s_freecam_subwidgets[] = {
    {
        .type = WidgetType_IntEdit,
        .int_edit =
            {
                .label = "Turbo Speed Factor",
                .pref = Pref_FreecamSpeedMult,
                .min = freecam_TURBO_SPEED_MIN,
                .max = freecam_TURBO_SPEED_MAX,
            },
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
};

static bool freecam_is_enabled() {
    return Pref_Get(Pref_Freecam);
}

static Widget s_freecam_widgets[] = {
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
                .label = "Freecam Toggle Bind",
                .pref = Pref_FreecamToggleBind,
                .required_chord = false,
                .can_unbind = true,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_freecam_subwidgets,
                .num_widgets = LEN(s_freecam_subwidgets),
                .show_if = freecam_is_enabled,
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

static const char *TIMER_TYPES[] = {"Default", "Freeze at max", "Freeze at 0", "Count up from 0"};
static const char *FALLOUT_PLANE_TYPE[] = {"Normal", "Disabled", "Bouncy"};

static Widget s_assist_widgets[] = {
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
};

static Widget s_savestate_subwidgets[] = {
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
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Prevent Overriding",
                .pref = Pref_SavestateDisableOverwrite,
            },
    },
};

static bool savestates_are_enabled() {
    return Pref_Get(Pref_Savestates);
}

static Widget s_savestate_widgets[] = {
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
                .show_if = savestates_are_enabled,
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
        .menu = {"Challenge Mode Seg", s_cm_seg_widgets, LEN(s_cm_seg_widgets)},
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

static void disable_il_invalidating_settings() {
    ILMark_DisableInvalidatingSettings();
}

static Widget s_il_mark_widgets[] = {
    {
        .type = WidgetType_Header,
        .header = {"Disable IL Invalidating Settings"},
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Disable Now",
                .push = disable_il_invalidating_settings,
                .flags = ButtonFlag_GoBack,
            },
    },
    {.type = WidgetType_Separator},
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
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Show Death Counter",
                .pref = Pref_ShowDeathCounter,
            },
    },
};

static Widget s_enabled_physics_widgets[] = {
    {
        .type = WidgetType_FloatEdit,
        .float_edit =
            {
                .label = "Friction",
                .pref = Pref_Friction,
                .precision = 1000,
                .min = 0,
                .max = 255,
                .floor = -100,
                .decimals = 3,
            },
    },
    {
        .type = WidgetType_FloatEdit,
        .float_edit =
            {
                .label = "Restitution",
                .pref = Pref_Restitution,
                .precision = 100,
                .min = 0,
                .max = 255,
                .floor = -100,
                .decimals = 2,
            },
    },
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Moon Gravity",
                .pref = Pref_Moon,
            },
    },
};

static bool custom_physics_is_enabled() {
    return Pref_Get(Pref_UseCustomPhysics);
}

static Widget s_physics_widgets[] = {
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "Use Custom Physics",
                .pref = Pref_UseCustomPhysics,
            },
    },
    {
        .type = WidgetType_HideableGroupWidget,
        .hideable_group =
            {
                .widgets = s_enabled_physics_widgets,
                .num_widgets = LEN(s_enabled_physics_widgets),
                .show_if = custom_physics_is_enabled,
            },
    },
};

static const char *STAGE_EDIT_VARIANTS[] = {"None", "Golden Banana", "Dark Banana", "Reverse Mode"};

static void select_new_reverse_mode_goal() {
    stage_edits_select_new_goal();
}

static Widget s_reverse_goal_widgets[] = {
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Select New Goal",
                .push = select_new_reverse_mode_goal,
                .flags = ButtonFlag_CloseMenu,
            },
    },
};

static bool reverse_mode_is_selected() {
    return Pref_Get(Pref_StageEditVariant) == 3;
}

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
                .show_if = reverse_mode_is_selected,
            },
    },
    {
        .type = WidgetType_Text,
        .text = {"Stage Edits are activated on retry"},
    },
};

static Widget s_gameplay_mods_widgets[] = {
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
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Physics",
                .widgets = s_physics_widgets,
                .num_widgets = LEN(s_physics_widgets),
            },
    },
    {
        .type = WidgetType_Menu,
        .menu =
            {
                .label = "Assist",
                .widgets = s_assist_widgets,
                .num_widgets = LEN(s_assist_widgets),
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
                .label = "Jump Mod",
                .pref = Pref_JumpMod,
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
    {
        .type = WidgetType_Checkbox,
        .checkbox =
            {
                .label = "D-pad Controls",
                .pref = Pref_DpadControls,
            },
    },
};

static void restore_default_preferences() {
    Pref_SetDefaults();
    Prev_Save();
}

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
                .push = NULL,
                .flags = ButtonFlag_GoBack,
            },
    },
    {
        .type = WidgetType_Button,
        .button =
            {
                .label = "Confirm",
                .push = restore_default_preferences,
                .flags = ButtonFlag_GoBack,
            },
    },
};

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
    {
        .type = WidgetType_Menu,
        .menu = {"Restore Defaults", s_reset_prefs_widgets, LEN(s_reset_prefs_widgets)},
    },
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
        .menu = {"Variants", s_gameplay_mods_widgets, LEN(s_gameplay_mods_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Settings", s_pracmod_settings_widgets, LEN(s_pracmod_settings_widgets)},
    },
    {
        .type = WidgetType_Menu,
        .menu = {"Help", s_help_widgets, LEN(s_help_widgets)},
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

void menu_init() {
    mkb_sprintf(s_version_str, "  Current version: v%s", version_get_str());
}
