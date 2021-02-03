#include "menu_defn.h"

#include <mkb/mkb.h>

#include "jump.h"
#include "timer.h"
#include "savestate.h"
#include "iw.h"

#define ARRAY_LEN(a) (sizeof((a)) / sizeof((a)[0]))

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

static void set_jump_mod_enabled(bool enable) { if (enable) jump::init(); else jump::dest(); }

static bool get_rumble_enabled()
{
    return true;
}

static void set_rumble_enabled(bool enable) {}

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
        .checkbox = {"Jump Mod", jump::is_enabled, set_jump_mod_enabled},
    },
    {
        .type = WidgetType::Checkbox,
        .checkbox = {"Rumble", get_rumble_enabled, set_rumble_enabled},
    },
    {.type = WidgetType::Menu, .menu = {"Help", help_widgets, ARRAY_LEN(help_widgets)}},
};

MenuWidget root_menu = {
    .label = "ApeSphere Menu",
    .widgets = root_widgets,
    .num_widgets = ARRAY_LEN(root_widgets),
};

}