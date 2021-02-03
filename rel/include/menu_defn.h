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
    void (*set)(bool value);
};

struct MenuWidget
{
    const char *label;
    struct Widget *widgets;
    u32 num_widgets;
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
    };
};

extern MenuWidget root_menu;

}