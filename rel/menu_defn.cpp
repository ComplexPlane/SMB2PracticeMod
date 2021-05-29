#include "menu_defn.h"

#include <mkb/mkb.h>
#include <assembly.h>
#include <inputdisp.h>

#include "jump.h"
#include "timer.h"
#include "savestate.h"
#include "iw.h"

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

static bool get_practice_tools_enabled()
{
    return timer::is_enabled() && savestate::is_enabled() && iw::is_enabled();
}

static void set_practice_tools_enabled(bool enable)
{
    if (enable)
    {
        timer::init();
        savestate::init();
        iw::init();
    }
    else
    {
        timer::dest();
        savestate::dest();
        iw::dest();
    }
}

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
    {.type = WidgetType::Text, .text = {"  Current version: v0.2.2"}},
    {.type = WidgetType::Text, .text = {"  For the latest version of this mod:"}},
    {.type = WidgetType::Text, .text = {"  github.com/ComplexPlane/ApeSphere/releases"}},
};

static Widget root_widgets[] = {
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Practice Tools", get_practice_tools_enabled, set_practice_tools_enabled},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Input Display",
            .get = inputdisp::is_enabled,
            .set = [](bool enable) { if (enable) inputdisp::init(); else inputdisp::dest(); },
        }
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {
            .label = "Jump Mod",
            .get = jump::is_enabled,
            .set = [](bool enable) { if (enable) jump::init(); else jump::dest(); },
        },
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