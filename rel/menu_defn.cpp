#include "menu_defn.h"

#include <assembly.h>
#include <banans.h>
#include <cmseg.h>
#include <dpad.h>
#include <draw.h>
#include <inputdisp.h>
#include <iw.h>
#include <mkb.h>

#include "gotostory.h"
#include "jump.h"
#include "macro_utils.h"
#include "pref.h"
#include "savestate.h"
#include "sfx.h"
#include "timer.h"

namespace menu {

static const char* inputdisp_colors[] = {
    "Purple", "Red", "Orange", "Yellow", "Green", "Blue", "Pink", "Black",
};
static_assert(LEN(inputdisp_colors) == inputdisp::NUM_COLORS);

typedef Widget pWidget[4];
pWidget inputdisp_widgets = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Show Input Display", pref::get_input_disp, pref::set_input_disp},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Use Center Location", pref::get_input_disp_center_location,
                     pref::set_input_disp_center_location},
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
        .checkbox = {"Notch Indicators", pref::get_input_disp_notch_indicators,
                     pref::set_input_disp_notch_indicators},
    },
};

static Widget rumble_widgets[] = {
    {.type = WidgetType::Checkbox,
     .checkbox = {
         .label = "Controller 1 Rumble",
         .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 0)); },
         .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 0); },
     }},
    {.type = WidgetType::Checkbox,
     .checkbox = {
         .label = "Controller 2 Rumble",
         .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 1)); },
         .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 1); },
     }},
    {.type = WidgetType::Checkbox,
     .checkbox = {
         .label = "Controller 3 Rumble",
         .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 2)); },
         .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 2); },
     }},
    {.type = WidgetType::Checkbox,
     .checkbox = {
         .label = "Controller 4 Rumble",
         .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 3)); },
         .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 3); },
     }}};

static Widget about_widgets[] = {
    {.type = WidgetType::Header, .header = {"SMB2 Practice Mod"}},
    {.type = WidgetType::Text, .text = {"  Made with   by ComplexPlane"}},
    {.type = WidgetType::Custom, .custom = {draw::heart}},
    {.type = WidgetType::Separator},

    {.type = WidgetType::Header, .header = {"Updates"}},
    {.type = WidgetType::Text, .text = {"  Current version: v0.5.1"}},
    {.type = WidgetType::Text, .text = {"  For the latest version of this mod:"}},
    {.type = WidgetType::ColoredText,
     .colored_text = {"  github.com/ComplexPlane/ApeSphere/releases", draw::BLUE}},
};

static const char* chara_choices[] = {"AiAi", "MeeMee", "Baby", "GonGon", "Random"};

static Widget cm_seg_widgets[] = {
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
    {.type = WidgetType::Button,
     .button = {.label = "Beginner 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Beginner1); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Beginner Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::BeginnerExtra); }}},
    {.type = WidgetType::Separator},

    // Advanced
    {.type = WidgetType::Button,
     .button = {.label = "Advanced 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced1); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Advanced 11-20",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced11); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Advanced 21-30",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced21); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Advanced Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::AdvancedExtra); }}},
    {.type = WidgetType::Separator},

    // Expert
    {.type = WidgetType::Button,
     .button = {.label = "Expert 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert1); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Expert 11-20",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert11); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Expert 21-30",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert21); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Expert 31-40",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert31); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Expert 41-50",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert41); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Expert Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::ExpertExtra); }}},
    {.type = WidgetType::Separator},

    // Master
    {.type = WidgetType::Button,
     .button = {.label = "Master 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::Master1); }}},
    {.type = WidgetType::Button,
     .button = {.label = "Master Extra 1-10",
                .push = [] { cmseg::request_cm_seg(cmseg::Seg::MasterExtra); }}},
};

static Widget timers_widgets[] = {
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

static Widget help_widgets[] = {
    {.type = WidgetType::Header, .header = {"Savestates Bindings"}},
    {.type = WidgetType::Text, .text = {"  X          \x1c Create savestate"}},
    {.type = WidgetType::Text, .text = {"  Y          \x1c Load savestate"}},
    {.type = WidgetType::Text, .text = {"  C-Stick    \x1c Change savestate slot"}},
    // TODO replace this feature with a better one that works in-menu
    {.type = WidgetType::Text, .text = {"  L+X or R+X \x1c Frame advance"}},
    {.type = WidgetType::Text, .text = {"  L+C or R+C \x1c Browse savestates"}},
    {.type = WidgetType::Separator},

    {.type = WidgetType::Header, .header = {"Jump Mod Bindings"}},
    {.type = WidgetType::Text, .text = {"  A          \x1c Jump"}},
    {.type = WidgetType::Text, .text = {"  B          \x1c Resize minimap"}},
};

static Widget sound_widgets[] = {
    {.type = WidgetType::Checkbox,
     .checkbox =
         {
             .label = "Mute Background Music",
             .get = pref::get_mute_bgm,
             .set = pref::set_mute_bgm,
         }},
    {.type = WidgetType::Text, .text = {"  To apply background music setting:"}},
    {.type = WidgetType::Text, .text = {"  Wait 2s then reset console"}},

    {.type = WidgetType::Separator},
    {.type = WidgetType::Checkbox,
     .checkbox =
         {
             .label = "Mute Timer Ding",
             .get = pref::get_mute_timer_ding,
             .set = pref::set_mute_timer_ding,
         }},
};

static Widget mods_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Savestates", pref::get_savestates, pref::set_savestates},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Input Display", inputdisp_widgets, LEN(inputdisp_widgets)},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Timers", timers_widgets, LEN(timers_widgets)},
    },
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
        .type = WidgetType::Menu,
        .menu = {"Audio", sound_widgets, LEN(sound_widgets)},
    },
    {.type = WidgetType::Checkbox,
     .checkbox =
         {
             .label = "Freeze Timer",
             .get = pref::get_freeze_timer,
             .set = pref::set_freeze_timer,
         }},
    {
        .type = WidgetType::Checkbox,
        .checkbox =
            {
                .label = "9999 Banana Counter",
                .get = pref::get_9999_banana_counter,
                .set = pref::set_9999_banana_counter,
            },
    },
    {.type = WidgetType::Checkbox,
     .checkbox =
         {
             .label = "D-pad Controls",
             .get = pref::get_dpad_controls,
             .set = pref::set_dpad_controls,
         }},
    {.type = WidgetType::Checkbox,
     .checkbox =
         {
             .label = "Debug Mode",
             .get = pref::get_debug_mode,
             .set = pref::set_debug_mode,
         }},
};

static Widget root_widgets[] = {
    {
        .type = WidgetType::Button,
        .button = {"Go To Story Mode", gotostory::load_storymode},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Challenge Mode Seg", cm_seg_widgets, LEN(cm_seg_widgets)},
    },
    {.type = WidgetType::Menu, .menu = {"Mods", mods_widgets, LEN(mods_widgets)}},
    {.type = WidgetType::Menu, .menu = {"Rumble Settings", rumble_widgets, LEN(rumble_widgets)}},
    {.type = WidgetType::Menu, .menu = {"Help", help_widgets, LEN(help_widgets)}},
    {.type = WidgetType::Menu, .menu = {"About", about_widgets, LEN(about_widgets)}},
};

MenuWidget root_menu = {
    .label = "Main Menu",
    .widgets = root_widgets,
    .num_widgets = LEN(root_widgets),
};

}  // namespace menu