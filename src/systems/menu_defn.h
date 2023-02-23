#pragma once

#include "mkb/mkb.h"

namespace menu_defn {

enum class WidgetType {
    Text,
    ColoredText,
    Header,
    Checkbox,
    Separator,
    Menu,
    FloatView,
    Choose,
    Button,
    Custom,
};

struct TextWidget {
    const char* label;            // For static text
    const char* (*label_func)();  // For dynamic text
};

struct ColoredTextWidget {
    const char* label;
    mkb::GXColor color;
};

// Just a different color TextWidget
struct HeaderWidget {
    const char* label;
};

struct CheckboxWidget {
    const char* label;
    // We can't use std::function due to destructors in unions stuff
    bool (*get)();
    void (*set)(bool);
};

struct MenuWidget {
    const char* label;
    struct Widget* widgets;
    u32 num_widgets;
    // It's too convenient to store currently selected menu entry in the widget itself,
    // even if it violates the otherwise immutable nature of the menu definition
    u32 selected_idx;
};

struct FloatViewWidget {
    const char* label;
    f32 (*get)();
};

struct ChooseWidget {
    const char* label;
    const char** choices;
    u32 num_choices;
    u32 (*get)();
    void (*set)(u32);
};

namespace ButtonFlags {
enum {
    CloseMenu = 1 << 0,  // Close menu after pushed
    GoBack = 1 << 1,     // Go back to parent menu after pushed
};
}

struct ButtonWidget {
    const char* label;
    void (*push)();  // Runs when pushed. Can be null
    u32 flags;
};

struct CustomWidget {
    void (*draw)();
};

struct Widget {
    WidgetType type;
    union {
        TextWidget text;
        ColoredTextWidget colored_text;
        HeaderWidget header;
        CheckboxWidget checkbox;
        MenuWidget menu;
        FloatViewWidget float_view;
        ChooseWidget choose;
        ButtonWidget button;
        CustomWidget custom;
    };
};

extern MenuWidget root_menu;

void init();

}  // namespace menu_defn
