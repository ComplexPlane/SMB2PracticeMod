#pragma once

#include "mkb/mkb.h"
#include "pref.h"

namespace menu_defn {

enum class WidgetType {
    Text,
    ColoredText,
    Header,
    Checkbox,
    GetSetCheckbox,
    Separator,
    Menu,
    FloatView,
    Choose,
    Button,
    IntEdit,
    FloatEdit,
    InputSelect,
    HideableGroupWidget,
    Custom,
    RgbPreview,
};

struct TextWidget {
    const char *label;            // For static text
    const char *(*label_func)();  // For dynamic text
};

struct ColoredTextWidget {
    const char *label;
    mkb::GXColor color_left;   // Gradient start color
    mkb::GXColor color_right;  // Gradient end color
    u16 offset_x;              // Character offset from line start
    bool overlap;              // Draw on top of the previous line instead of a new line
};

// Just a different color TextWidget
struct HeaderWidget {
    const char *label;
};

struct CheckboxWidget {
    const char *label;
    pref::Pref pref;
};

// For the rare cases a checkbox doesn't correspond to a preference
struct GetSetCheckboxWidget {
    const char *label;
    bool (*get)();
    void (*set)(bool);
};

struct MenuWidget {
    const char *label;
    struct Widget *widgets;
    u32 num_widgets;
    // It's too convenient to store currently selected menu entry in the widget itself,
    // even if it violates the otherwise immutable nature of the menu definition
    u32 selected_idx;
};

struct FloatViewWidget {
    const char *label;
    f32 (*get)();
};

struct ChooseWidget {
    const char *label;
    const char **choices;
    u16 num_choices;
    pref::Pref pref;
};

namespace ButtonFlags {
enum {
    CloseMenu = 1 << 0,  // Close menu after pushed
    GoBack = 1 << 1,     // Go back to parent menu after pushed
};
}

struct ButtonWidget {
    const char *label;
    void (*push)();  // Runs when pushed. Can be null
    u32 flags;
};

// Pretty limited for now
struct IntEditWidget {
    const char *label;
    pref::Pref pref;
    s16 min;
    s16 max;
    // Formats the value into a buffer; if null, the value displays as decimal
    void (*format)(s16 value, char *buf);
};

// even more limited for now
struct FloatEditWidget {
    const char *label;
    pref::Pref pref;
    u32 precision;  // denominator, 100
    u8 min;
    u8 max;
    s32 floor;
    u8 decimals;
};

struct InputSelectWidget {
    const char *label;
    pref::Pref pref;
    bool required_chord;  // must be a 2 button bind if true
    bool can_unbind;
};

enum class HideableType : u8 { U8Hideable, BoolHideable };

struct HideableGroupWidget {
    struct Widget *widgets;
    u32 num_widgets;
    bool (*show_if)();  // show if function returns true
};

struct CustomWidget {
    void (*draw)();
};

// Shows a live color swatch next to RGB IntEdit widgets
struct RgbPreviewWidget {
    pref::Pref r_pref;
    pref::Pref g_pref;
    pref::Pref b_pref;
};

struct Widget {
    WidgetType type;
    union {
        TextWidget text;
        ColoredTextWidget colored_text;
        HeaderWidget header;
        CheckboxWidget checkbox;
        GetSetCheckboxWidget get_set_checkbox;
        MenuWidget menu;
        FloatViewWidget float_view;
        ChooseWidget choose;
        ButtonWidget button;
        IntEditWidget int_edit;
        FloatEditWidget float_edit;
        InputSelectWidget input_select;
        HideableGroupWidget hideable_group;
        CustomWidget custom;
        RgbPreviewWidget rgb_preview;
    };
};

extern MenuWidget root_menu;

void init();

}  // namespace menu_defn
