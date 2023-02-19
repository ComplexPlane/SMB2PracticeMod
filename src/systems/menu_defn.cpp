#include "menu_defn.h"

#include "mkb/mkb.h"
#include "mods/ballcolor.h"
#include "mods/cmseg.h"
#include "mods/gotostory.h"
#include "mods/ilbattle.h"
#include "mods/inputdisp.h"
#include "mods/jump.h"
#include "mods/unlock.h"
#include "systems/pref.h"
#include "systems/version.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

// TODO update buttons with close menu flag
// TODO let buttons have null push()

namespace menu_defn {

static char s_version_str[30];

static const char* inputdisp_colors[] = {
    "Purple", "Red", "Orange", "Yellow", "Green", "Blue", "Pink", "Black",
};
static_assert(LEN(inputdisp_colors) == inputdisp::NUM_COLORS);

static Widget s_inputdisp_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                "Show Input Display",
                pref::get_input_disp,
                pref::set_input_disp,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                "Use Center Location",
                pref::get_input_disp_center_location,
                pref::set_input_disp_center_location,
            },
    },
    {
        .type = WidgetType::Choose,
        .choose = {
            .label = "Color",
            .choices = inputdisp_colors,
            .num_choices = LEN(inputdisp_colors),
            .get = []() { return static_cast<u32>(pref::get_input_disp_color()); },
            .set = [](u32 color) { pref::set_input_disp_color(static_cast<u8>(color)); },
        },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                "Notch Indicators",
                pref::get_input_disp_notch_indicators,
                pref::set_input_disp_notch_indicators,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                "Raw Stick Inputs",
                pref::get_input_disp_raw_stick_inputs,
                pref::set_input_disp_raw_stick_inputs,
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
        .choose = {
            .label = "Ball Color",
            .choices = s_ball_colors,
            .num_choices = LEN(s_ball_colors),
            .get = []() { return static_cast<u32>(pref::get_ball_color()); },
            .set = [](u32 color) { pref::set_ball_color(static_cast<u8>(color)); },
        },
    },
    {
        .type = WidgetType::Choose,
        .choose = {
            .label = "Ape Color",
            .choices = s_ball_colors,
            .num_choices = LEN(s_ball_colors),
            .get = []() { return static_cast<u32>(pref::get_ape_color()); },
            .set = [](u32 color) { pref::set_ape_color(static_cast<u8>(color)); },
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
                "IL Battle Display",
                pref::get_il_battle_display,
                pref::set_il_battle_display,
            },
    },
    {
        .type = WidgetType::Choose,
        .choose = {
            .label = "Battle Length",
            .choices = s_il_battle_lengths,
            .num_choices = LEN(s_il_battle_lengths),
            .get = []() { return static_cast<u32>(pref::get_il_battle_length()); },
            .set = [](u32 color) { pref::set_il_battle_length(static_cast<u8>(color)); },
        },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                "Score Breakdown Info",
                pref::get_il_battle_breakdown,
                pref::set_il_battle_breakdown,
            },
    },
    {.type = WidgetType::Text, .text = {"  Dpad-Down then Retry to start a new battle"}},
};

static Widget s_rumble_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Controller 1 Rumble",
            .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 0)); },
            .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 0); },
        },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Controller 2 Rumble",
            .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 1)); },
            .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 1); },
        },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Controller 3 Rumble",
            .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 2)); },
            .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 2); },
        },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Controller 4 Rumble",
            .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 3)); },
            .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 3); },
        },
    }};

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

static const char* chara_choices[] = {"AiAi", "MeeMee", "Baby", "GonGon", "Random"};

static Widget s_cm_seg_widgets[] = {
    // Settings
    {.type = WidgetType::Choose,
     .choose = {
         .label = "Character",
         .choices = chara_choices,
         .num_choices = LEN(chara_choices),
         .get = [] { return static_cast<u32>(pref::get_cm_chara()); },
         .set = [](u32 choice) { pref::set_cm_chara(static_cast<u8>(choice)); },
     }},
    {.type = WidgetType::Separator},

    // Beginner
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Beginner 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Beginner1); },
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Beginner Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::BeginnerExtra); },
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
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Advanced 11-20",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced11); },
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Advanced 21-30",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced21); },
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Advanced Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::AdvancedExtra); },
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
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 11-20",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert11); },
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 21-30",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert21); },
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 31-40",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert31); },
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert 41-50",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert41); },
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Expert Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::ExpertExtra); },
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
            },
    },
    {
        .type = WidgetType::Button,
        .button =
            {
                .label = "Master Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::MasterExtra); },
            },
    },
};

static Widget s_timers_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"RTA+Pause Timer", pref::get_rta_pause_timer, pref::set_rta_pause_timer},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Story Mode IW Timer", pref::get_iw_timer, pref::set_iw_timer},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"CM Seg Timer", pref::get_cm_timer, pref::set_cm_timer},
    }};

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
    {.type = WidgetType::Text, .text = {"  Left Stick \x1c Pan"}},
    {.type = WidgetType::Text, .text = {"  C-Stick    \x1c Rotate"}},
    {.type = WidgetType::Text, .text = {"  L          \x1c Down"}},
    {.type = WidgetType::Text, .text = {"  R          \x1c Up"}},
    {.type = WidgetType::Text, .text = {"  Y          \x1c Fast"}},
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

static Widget s_help_widgets[] = {
    {
        .type = WidgetType::Menu,
        .menu = {"Savestates Bindings", s_savestates_help_widgets, LEN(s_savestates_help_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Jump Mod Bindings", s_jump_help_widgets, LEN(s_jump_help_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Freecam Bindings", s_freecam_help_widgets, LEN(s_freecam_help_widgets)},
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
                .get = pref::get_mute_bgm,
                .set = pref::set_mute_bgm,
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
                .get = pref::get_mute_timer_ding,
                .set = pref::set_mute_timer_ding,
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
        .button = {.label = "Cancel", .push = nullptr, .flags = ButtonFlags::GoBack},
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Confirm",
                   .push = unlock::unlock_everything,
                   .flags = ButtonFlags::GoBack},
    },
};

static Widget s_unlock_widgets[] = {
    {
        .type = WidgetType::Header,
        .header = {"Unlock Progress For This Session"},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Unlock Everything Now", s_unlock_confirm_widgets, LEN(s_unlock_confirm_widgets)},
    },
    {.type = WidgetType::Separator},

    {
        .type = WidgetType::Header,
        .header = {"Always Unlock Progress"},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"For Vanilla SMB2", pref::get_unlock_vanilla, pref::set_unlock_vanilla},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"For Romhacks", pref::get_unlock_romhacks, pref::set_unlock_romhacks},
    },
    {
        .type = WidgetType::Text,
        .text = {"  Applied on game startup."},
    },
};

static Widget s_tools_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Savestates", pref::get_savestates, pref::set_savestates},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Freeze Timer",
                .get = pref::get_freeze_timer,
                .set = pref::set_freeze_timer,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {.label = "Freecam", .get = pref::get_freecam, .set = pref::set_freecam},
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
                .get = pref::get_debug_mode,
                .set = pref::set_debug_mode,
            },
    },
};

static Widget s_il_mark_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show in Practice Mode",
                .get = pref::get_il_mark_practice,
                .set = pref::set_il_mark_practice,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show in Story Mode",
                .get = pref::get_il_mark_story,
                .set = pref::set_il_mark_story,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Show in Challenge Mode",
                .get = pref::get_il_mark_challenge,
                .set = pref::set_il_mark_challenge,
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
                .get = pref::get_9999_banana_counter,
                .set = pref::set_9999_banana_counter,
            },
    },
};

static Widget s_gameplay_mods_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Jump Mod",
                .get = pref::get_jump_mod,
                .set = pref::set_jump_mod,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Moon Gravity",
                .get = pref::get_moon,
                .set = pref::set_moon,
            },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "Marathon Mode",
                .get = pref::get_marathon,
                .set = pref::set_marathon,
            },
    },
    {.type = WidgetType::Checkbox,
     .checkbox =
         {
             .label = "D-pad Controls",
             .get = pref::get_dpad_controls,
             .set = pref::set_dpad_controls,
         }},
};

static Widget s_root_widgets[] = {
    {
        .type = WidgetType::Button,
        .button = {"Go To Story Mode", gotostory::load_storymode},
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
