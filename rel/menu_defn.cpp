#include "menu_defn.h"

#include <mkb.h>
#include <assembly.h>
#include <inputdisp.h>
#include <cmseg.h>

#include "jump.h"
#include "timer.h"
#include "savestate.h"
#include "gotostory.h"

#define ARRAY_LEN(a) (sizeof((a)) / sizeof((a)[0]))

namespace mkb
{

extern "C"
{
extern u8 rumble_enabled_bitflag;
}

}

namespace menu
{

static const char *inputdisp_colors[] = {
    "Purple",
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Blue",
    "Pink",
    "Black",
};
static_assert(ARRAY_LEN(inputdisp_colors) == inputdisp::NUM_COLORS);

static Widget inputdisp_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Show Input Display", inputdisp::is_visible, inputdisp::set_visible},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Use Center Location", inputdisp::is_in_center_loc, inputdisp::set_in_center_loc},
    },
    {
        .type = WidgetType::Choose,
        .choose = {
            .label = "Color",
            .choices = inputdisp_colors,
            .num_choices = ARRAY_LEN(inputdisp_colors),
            .get = []() { return static_cast<u32>(inputdisp::get_color()); },
            .set = [](u32 color)
            {
                inputdisp::set_color(static_cast<inputdisp::Color>(color));
            },
        },
    },
};

static Widget rumble_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Controller 1 Rumble",
            .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 0)); },
            .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 0); },
        }
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Controller 2 Rumble",
            .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 1)); },
            .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 1); },
        }
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Controller 3 Rumble",
            .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 2)); },
            .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 2); },
        }
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Controller 4 Rumble",
            .get = []() { return static_cast<bool>(mkb::rumble_enabled_bitflag & (1 << 3)); },
            .set = [](bool enable) { mkb::rumble_enabled_bitflag ^= (1 << 3); },
        }
    }
};

static Widget dev_tools_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Debug Mode",
            .get = []() { return main::debug_mode_enabled; },
            .set = [](bool enable) { main::debug_mode_enabled = enable; },
        }
    },
    {.type = WidgetType::Separator},

    {
        .type = WidgetType::FloatView,
        .float_view = {
            .label = "Ball Pos X",
            .get = []() { return mkb::balls[0].pos.x; },
        },
    },
    {
        .type = WidgetType::FloatView,
        .float_view = {
            .label = "Ball Pos Y",
            .get = []() { return mkb::balls[0].pos.y; },
        },
    },
    {
        .type = WidgetType::FloatView,
        .float_view = {
            .label = "Ball Pos Z",
            .get = []() { return mkb::balls[0].pos.z; },
        },
    },
    {.type = WidgetType::Separator},

    {
        .type = WidgetType::FloatView,
        .float_view = {
            .label = "Ball Vel X",
            .get = []() { return mkb::balls[0].vel.x; },
        },
    },
    {
        .type = WidgetType::FloatView,
        .float_view = {
            .label = "Ball Vel Y",
            .get = []() { return mkb::balls[0].vel.y; },
        },
    },
    {
        .type = WidgetType::FloatView,
        .float_view = {
            .label = "Ball Vel Z",
            .get = []() { return mkb::balls[0].vel.z; },
        },
    },
};

static Widget help_widgets[] = {
    {.type = WidgetType::Header, .header = {"Practice Tools Bindings"}},
    {.type = WidgetType::Text, .text = {"  L+R        \x1c Toggle this menu"}},
    {.type = WidgetType::Text, .text = {"  X          \x1c Create savestate"}},
    {.type = WidgetType::Text, .text = {"  Y          \x1c Load savestate"}},
    {.type = WidgetType::Text, .text = {"  C-Stick    \x1c Change savestate slot"}},
    {.type = WidgetType::Text, .text = {"  L+X or R+X \x1c Frame advance"}},
    {.type = WidgetType::Text, .text = {"  L+C or R+C \x1c Browse savestates"}},
    {.type = WidgetType::Separator},

    {.type = WidgetType::Header, .header = {"Jump Mod Bindings"}},
    {.type = WidgetType::Text, .text = {"  A          \x1c Jump"}},
    {.type = WidgetType::Text, .text = {"  B          \x1c Resize minimap"}},
    {.type = WidgetType::Separator},

    {.type = WidgetType::Header, .header = {"Updates"}},
    {.type = WidgetType::Text, .text = {"  Current version: v0.3.0"}},
    {.type = WidgetType::Text, .text = {"  For the latest version of this mod:"}},
    {.type = WidgetType::Text, .text = {"  github.com/ComplexPlane/ApeSphere/releases"}},
};

static const char *chara_choices[] = {
    "AiAi",
    "MeeMee",
    "Baby",
    "GonGon",
    "Dr. Bad-Boon",
    "Whale",
    "Random (main 4)",
    "Random (all)"
};
static cmseg::Chara s_chara_choice;

static Widget cm_seg_settings_widgets[] = {
    {
        .type = WidgetType::Choose,
        .choose = {
            .label = "Character",
            .choices = chara_choices,
            .num_choices = ARRAY_LEN(chara_choices),
            .get = [] { return static_cast<u32>(s_chara_choice); },
            .set = [](u32 choice) { s_chara_choice = static_cast<cmseg::Chara>(choice); },
        }
    },
    // TODO use integer selection widget
    {
        .type = WidgetType::Text,
        .text = {"Monkeys"},
    }
};

static Widget cm_seg_widgets[] = {
    // Settings
    {
        .type = WidgetType::Menu,
        .menu = {"Settings", cm_seg_settings_widgets, ARRAY_LEN(cm_seg_settings_widgets)},
    },
    {.type = WidgetType::Separator},

    // Beginner
    {
        .type = WidgetType::Button,
        .button = {.label = "Beginner 1-10", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Beginner1, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Beginner Extra 1-10", .push = [] { cmseg::request_cm_seg(cmseg::Seg::BeginnerExtra, s_chara_choice); }}
    },
    {.type = WidgetType::Separator},

    // Advanced
    {
        .type = WidgetType::Button,
        .button = {.label = "Advanced 1-10", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced1, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Advanced 11-20", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced11, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Advanced 21-30", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Advanced21, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Advanced Extra 1-10", .push = [] { cmseg::request_cm_seg(cmseg::Seg::AdvancedExtra, s_chara_choice); }}
    },
    {
        .type = WidgetType::Separator
    },

    // Expert
    {
        .type = WidgetType::Button,
        .button = {.label = "Expert 1-10", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert1, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Expert 11-20", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert11, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Expert 21-30", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert21, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Expert 31-40", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert31, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Expert 41-50", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Expert41, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Expert Extra 1-10", .push = [] { cmseg::request_cm_seg(cmseg::Seg::ExpertExtra, s_chara_choice); }}
    },
    {.type = WidgetType::Separator},

    // Master
    {
        .type = WidgetType::Button,
        .button = {.label = "Master 1-10", .push = [] { cmseg::request_cm_seg(cmseg::Seg::Master1, s_chara_choice); }}
    },
    {
        .type = WidgetType::Button,
        .button = {.label = "Master Extra 1-10", .push = [] { cmseg::request_cm_seg(cmseg::Seg::MasterExtra, s_chara_choice); }}
    },
};

static Widget root_widgets[] = {
    {
        .type = WidgetType::Button,
        .button = {"Go To Story Mode", gotostory::load_storymode},
    },
    {
        .type = WidgetType::Menu,
        .menu = {"Challenge Mode Seg", cm_seg_widgets, ARRAY_LEN(cm_seg_widgets)},
    },
    {
        .type = WidgetType::Menu, .menu = {"Input Display", inputdisp_widgets, ARRAY_LEN(inputdisp_widgets)},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Jump Mod",
            .get = jump::is_enabled,
            .set = [](bool enable) { if (enable) jump::init(); else jump::dest(); },
        },
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"RTA Timer", timer::is_visible, timer::set_visible},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Savestates", savestate::is_visible, savestate::set_visible},
    },
    {.type = WidgetType::Menu, .menu = {"Rumble", rumble_widgets, ARRAY_LEN(rumble_widgets)}},
    {.type = WidgetType::Menu, .menu = {"Help", help_widgets, ARRAY_LEN(help_widgets)}},
    {.type = WidgetType::Menu, .menu = {"Developer Tools", dev_tools_widgets, ARRAY_LEN(dev_tools_widgets)}},
};

MenuWidget root_menu = {
    .label = "ApeSphere Menu",
    .widgets = root_widgets,
    .num_widgets = ARRAY_LEN(root_widgets),
};

}