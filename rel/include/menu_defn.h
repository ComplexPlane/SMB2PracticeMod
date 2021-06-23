#pragma once

#include <gc/gc.h>

namespace menu
{

enum class WidgetType
{
    Text,
    Header,
    Checkbox,
    Separator,
    Menu,
    FloatView,
    Choose,
};

struct TextWidget
{
    const char *label;
};

// Just a different color TextWidget
struct HeaderWidget
{
    const char *label;
};

struct CheckboxWidget
{
    const char *label;
    // We can't use std::function due to destructors in unions stuff
    bool (*get)();
    void (*set)(bool);
};

struct MenuWidget
{
    const char *label;
    struct Widget *widgets;
    u32 num_widgets;
};

struct FloatViewWidget
{
    const char *label;
    f32 (*get)();
};

struct ChooseWidget
{
    const char *label;
    const char **choices;
    u32 num_choices;
    u32 (*get)();
    void (*set)(u32);
};

struct Widget
{
    WidgetType type;
    union
    {
        TextWidget text;
        HeaderWidget header;
        CheckboxWidget checkbox;
        MenuWidget menu;
        FloatViewWidget float_view;
        ChooseWidget choose;
    };
};

extern MenuWidget root_menu;

}