#pragma once

#include "systems/pref.h"
#include "utils/base.h"

typedef enum WidgetType {
    WidgetType_Text,
    WidgetType_ColoredText,
    WidgetType_Header,
    WidgetType_Checkbox,
    WidgetType_GetSetCheckbox,
    WidgetType_Separator,
    WidgetType_Menu,
    WidgetType_FloatView,
    WidgetType_Choose,
    WidgetType_Button,
    WidgetType_IntEdit,
    WidgetType_FloatEdit,
    WidgetType_InputSelect,
    WidgetType_HideableGroupWidget,
    WidgetType_Custom,
    WidgetType_RgbPreview,
} WidgetType;

typedef struct TextWidget {
    const char *label;            // For static text
    const char *(*label_func)();  // For dynamic text
} TextWidget;

typedef struct ColoredTextWidget {
    const char *label;
    GXColor color_left;   // Gradient start color
    GXColor color_right;  // Gradient end color
    u16 offset_x;         // Character offset from line start
    bool overlap;         // Draw on top of the previous line instead of a new line
} ColoredTextWidget;

// Just a different color TextWidget
typedef struct HeaderWidget {
    const char *label;
} HeaderWidget;

typedef struct CheckboxWidget {
    const char *label;
    Pref pref;
} CheckboxWidget;

// For the rare cases a checkbox doesn't correspond to a preference
typedef struct GetSetCheckboxWidget {
    const char *label;
    bool (*get)();
    void (*set)(bool);
} GetSetCheckboxWidget;

typedef struct MenuWidget {
    const char *label;
    struct Widget *widgets;
    u32 num_widgets;
    // It's too convenient to store currently selected menu entry in the widget itself,
    // even if it violates the otherwise immutable nature of the menu definition
    u32 selected_idx;
} MenuWidget;

typedef struct FloatViewWidget {
    const char *label;
    f32 (*get)();
} FloatViewWidget;

typedef struct ChooseWidget {
    const char *label;
    const char **choices;
    u16 num_choices;
    Pref pref;
} ChooseWidget;

typedef enum ButtonFlag {
    ButtonFlag_CloseMenu = 1 << 0,  // Close menu after pushed
    ButtonFlag_GoBack = 1 << 1,     // Go back to parent menu after pushed
} ButtonFlag;

typedef struct ButtonWidget {
    const char *label;
    void (*push)();  // Runs when pushed. Can be null
    u32 flags;
} ButtonWidget;

// Pretty limited for now
typedef struct IntEditWidget {
    const char *label;
    Pref pref;
    s16 min;
    s16 max;
    // Formats the value into a buffer; if null, the value displays as decimal
    void (*format)(s16 value, char *buf);
} IntEditWidget;

// even more limited for now
typedef struct FloatEditWidget {
    const char *label;
    Pref pref;
    u32 precision;  // denominator, 100
    u8 min;
    u8 max;
    s32 floor;
    u8 decimals;
} FloatEditWidget;

typedef struct InputSelectWidget {
    const char *label;
    Pref pref;
    bool required_chord;  // must be a 2 button bind if true
    bool can_unbind;
} InputSelectWidget;

typedef enum HideableType : u8 {
    HideableType_U8Hideable,
    HideableType_BoolHideable,
} HideableType;

typedef struct HideableGroupWidget {
    struct Widget *widgets;
    u32 num_widgets;
    bool (*show_if)();  // show if function returns true
} HideableGroupWidget;

typedef struct CustomWidget {
    void (*draw)();
} CustomWidget;

// Shows a live color swatch next to RGB IntEdit widgets
typedef struct RgbPreviewWidget {
    Pref r_pref;
    Pref g_pref;
    Pref b_pref;
} RgbPreviewWidget;

typedef struct Widget {
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
} Widget;

extern MenuWidget menu_root;

void menu_init();
