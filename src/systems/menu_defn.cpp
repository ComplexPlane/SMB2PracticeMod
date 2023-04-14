#include "menu_defn.h"

#include "mkb/mkb.h"
#include "mods/ballcolor.h"
#include "mods/cmseg.h"
#include "mods/freecam.h"
#include "mods/gotostory.h"
#include "mods/ilbattle.h"
#include "mods/inputdisp.h"
#include "mods/jump.h"
#include "mods/unlock.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

// TODO update buttons with close menu flag
// TODO let buttons have null push()

namespace menu_defn {

static char s_version_str[30];

static const char* s_inputdisp_colors[] = {
    "Purple", "Red", "Orange", "Yellow", "Green", "Blue", "Pink", "Black",
};
static_assert(LEN(s_inputdisp_colors) == inputdisp::NUM_COLORS);

static Widget s_inputdisp_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show Input Display",
                .pref = pref::BoolPref::InputDisp,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Use Center Location",
                .pref = pref::BoolPref::InputDispCenterLocation,
            },
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Color",
                .choices = s_inputdisp_colors,
                .num_choices = LEN(s_inputdisp_colors),
                .pref = pref::U8Pref::InputDispColor,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Notch Indicators",
                .pref = pref::BoolPref::InputDispNotchIndicators,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Raw Stick Inputs",
                .pref = pref::BoolPref::InputDispRawStickInputs,
            },
    },
};

static const char* s_ball_colors[] = {
    "Default", "Red", "Blue", "Yellow", "Green", "Teal", "Pink", "Black", "White",
};
static_assert(LEN(s_ball_colors) == ballcolor::NUM_COLORS);

static Widget s_ball_color_widgets[] = {
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Ball Color",
                .choices = s_ball_colors,
                .num_choices = LEN(s_ball_colors),
                .pref = pref::U8Pref::BallColor,
            },
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Ape Color",
                .choices = s_ball_colors,
                .num_choices = LEN(s_ball_colors),
                .pref = pref::U8Pref::ApeColor,
            },
    },
};

static const char* s_il_battle_lengths[] = {
    "5 min",
    "7 min",
    "10 min",
    "Endless",
};
static_assert(LEN(s_il_battle_lengths) == ilbattle::NUM_LENGTHS);

static Widget s_il_battle_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "IL Battle Display",
                .pref = pref::BoolPref::IlBattleDisplay,
            },
    },
    {
        .type = WidgetType::Choose,
        .choose =
            {
                .label = "Battle Length",
                .choices = s_il_battle_lengths,
                .num_choices = LEN(s_il_battle_lengths),
                .pref = pref::U8Pref::IlBattleLength,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Score Breakdown Info",
                .pref = pref::BoolPref::IlBattleBreakdown,
            },
    },
    {.type = WidgetType::Text, .text = {"  Dpad-Down then Retry to start a new battle"}},
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
        .text = {"  Made with   by ComplexPlane"},
    },
    {
        .type = WidgetType::Custom,
        .custom = {draw::heart},
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
        .text = {"  For the latest version of SMB2 Practice Mod:"},
    },
    {
        .type = WidgetType::ColoredText,
        .colored_text = {" github.com/ComplexPlane/SMB2PracticeMod/releases", draw::BLUE},
    },
};

static const char* s_chara_choices[] = {"AiAi", "MeeMee", "Baby", "GonGon", "Random"};

static Widget s_cm_seg_widgets[] = {
    // Settings
    {.type = WidgetType::Choose,
     .choose =
         {
             .label = "Character",
             .choices = s_chara_choices,
             .num_choices = LEN(s_chara_choices),
             .pref = pref::U8Pref::CmChara,
         }},
    {.type = WidgetType::Separator},

    // Beginner
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
    {.type = WidgetType::Separator},

    // Advanced
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
    {.type = WidgetType::Separator},

    // Expert
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
    {.type = WidgetType::Separator},

    // Master
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

static Widget s_timers_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "RTA+Pause Timer",
                .pref = pref::BoolPref::RtaPauseTimer,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Story Mode IW Timer",
                .pref = pref::BoolPref::IwTimer,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "CM Seg Timer",
                .pref = pref::BoolPref::CmTimer,
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

static Widget s_iw_help_widgets[] = {
    {.type = WidgetType::Text, .text = {"  Go to Story Mode, then press up/down on a file"}},
    {.type = WidgetType::Text, .text = {"  to choose a world."}},
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
    {.type = WidgetType::Text, .text = {"  Z          \x1c Toggle (If Enabled)"}},
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

static Widget s_pracmod_menu_widgets[] = {
    {.type = WidgetType::Text, .text = {"  L+R        \x1c Toggle Menu"}},
    {.type = WidgetType::Text, .text = {"  A          \x1c Select/Next Value"}},
    {.type = WidgetType::Text, .text = {"  B          \x1c Back"}},
    {.type = WidgetType::Text, .text = {"  Y          \x1c Previous Value"}},
    {.type = WidgetType::Text, .text = {"  X          \x1c Reset to Default"}},
};

static Widget s_help_widgets[] = {
    {
        .type = WidgetType::Menu,
        .menu = {"Practice Mod Menu", s_pracmod_menu_widgets, LEN(s_pracmod_menu_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Savestates", s_savestates_help_widgets, LEN(s_savestates_help_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Jump Mod", s_jump_help_widgets, LEN(s_jump_help_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Freecam", s_freecam_help_widgets, LEN(s_freecam_help_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Story Mode IWs", s_iw_help_widgets, LEN(s_iw_help_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"IL Validation Mark", s_iw_mark_help_widgets, LEN(s_iw_mark_help_widgets)},
    },
};

static Widget s_sound_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Mute Background Music",
                .pref = pref::BoolPref::MuteBgm,
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
                .pref = pref::BoolPref::MuteTimerDing,
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
                .pref = pref::BoolPref::UnlockVanilla,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "For Romhacks",
                .pref = pref::BoolPref::UnlockRomhacks,
            },
    },
    {
        .type = WidgetType::Text,
        .text = {"  Applied on game startup."},
    },
};

static Widget s_freecam_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Freecam",
                .pref = pref::BoolPref::Freecam,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Toggle With Z",
                .pref = pref::BoolPref::FreecamToggleWithZ,
            },
    },
    {
        .type = WidgetType::IntEdit,
        .int_edit =
            {
                .label = "Turbo Speed Factor",
                .pref = pref::U8Pref::FreecamSpeedMult,
                .min = freecam::TURBO_SPEED_MIN,
                .max = freecam::TURBO_SPEED_MAX,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Invert Yaw",
                .pref = pref::BoolPref::FreecamInvertYaw,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Invert Pitch",
                .pref = pref::BoolPref::FreecamInvertPitch,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Freeze Timer",
                .pref = pref::BoolPref::FreecamFreezeTimer,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide HUD",
                .pref = pref::BoolPref::FreecamHideHud,
            },
    },
};

static Widget s_hide_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide HUD",
                .pref = pref::BoolPref::HideHud,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide Ball",
                .pref = pref::BoolPref::HideBall,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide Stage Models",
                .pref = pref::BoolPref::HideStage,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide Stage Items",
                .pref = pref::BoolPref::HideItems,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide Stage Objects",
                .pref = pref::BoolPref::HideStobjs,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide Effects",
                .pref = pref::BoolPref::HideEffects,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Hide Background",
                .pref = pref::BoolPref::HideBg,
            },
    },
};

static Widget s_tools_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Savestates",
                .pref = pref::BoolPref::Savestates,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Freeze Timer",
                .pref = pref::BoolPref::FreezeTimer,
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
                .pref = pref::BoolPref::DebugMode,
            },
    },
};

static Widget s_il_mark_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show in Practice Mode",
                .pref = pref::BoolPref::IlMarkPractice,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show in Story Mode",
                .pref = pref::BoolPref::IlMarkStory,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show in Challenge Mode",
                .pref = pref::BoolPref::IlMarkChallenge,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show in Romhacks",
                .pref = pref::BoolPref::IlMarkRomhacks,
            },
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
        .menu = {"Ball & Ape Color", s_ball_color_widgets, LEN(s_ball_color_widgets)},
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
                .pref = pref::BoolPref::BananaCounter9999,
            },
    },
};

static Widget s_gameplay_mods_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Jump Mod",
                .pref = pref::BoolPref::JumpMod,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Moon Gravity",
                .pref = pref::BoolPref::Moon,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Marathon Mode",
                .pref = pref::BoolPref::Marathon,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "D-pad Controls",
                .pref = pref::BoolPref::DpadControls,
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

static Widget s_pracmod_settings_widgets[] = {{
    .type = WidgetType::Menu,
    .menu = {"Restore Defaults", s_reset_prefs_widgets, LEN(s_reset_prefs_widgets)},
}};

static Widget s_root_widgets[] = {
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
        .type = WidgetType::Menu,
        .menu = {"Challenge Mode Seg", s_cm_seg_widgets, LEN(s_cm_seg_widgets)},
    },
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
        .menu = {"Gameplay Mods", s_gameplay_mods_widgets, LEN(s_gameplay_mods_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Practice Mod Settings", s_pracmod_settings_widgets,
                 LEN(s_pracmod_settings_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Help", s_help_widgets, LEN(s_help_widgets)},
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

void init() { mkb::sprintf(s_version_str, "  Current version: v%s", version::get_version_str()); }

}  // namespace menu_defn
