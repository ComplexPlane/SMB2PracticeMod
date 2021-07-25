#pragma once

#include <mkb.h>

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
    Button,
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
    // It's too convenient to store currently selected menu entry in the widget itself,
    // even if it violates the otherwise immutable nature of the menu definition
    u32 selected_idx;
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

struct ButtonWidget
{
    const char *label;
    void (*push)();
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
        ButtonWidget button;
    };
};

extern MenuWidget root_menu;

}