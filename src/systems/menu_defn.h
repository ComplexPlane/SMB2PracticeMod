#pragma once

#include "pref.h"
#include "utils/base.h"

typedef enum WidgetType WidgetType;
enum WidgetType {
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
};

typedef struct TextWidget TextWidget;
struct TextWidget {
    const char *label;            // For static text
    const char *(*label_func)();  // For dynamic text
};

typedef struct ColoredTextWidget ColoredTextWidget;
struct ColoredTextWidget {
    const char *label;
    mkb::GXColor color;
};

// Just a different color TextWidget
typedef struct HeaderWidget HeaderWidget;
struct HeaderWidget {
    const char *label;
};

typedef struct CheckboxWidget CheckboxWidget;
struct CheckboxWidget {
    const char *label;
    Pref pref;
};

// For the rare cases a checkbox doesn't correspond to a preference
typedef struct GetSetCheckboxWidget GetSetCheckboxWidget;
struct GetSetCheckboxWidget {
    const char *label;
    bool (*get)();
    void (*set)(bool);
};

typedef struct MenuWidget MenuWidget;
struct MenuWidget {
    const char *label;
    struct Widget *widgets;
    u32 num_widgets;
    // It's too convenient to store currently selected menu entry in the widget itself,
    // even if it violates the otherwise immutable nature of the menu definition
    u32 selected_idx;
};

typedef struct FloatViewWidget FloatViewWidget;
struct FloatViewWidget {
    const char *label;
    f32 (*get)();
};

typedef struct ChooseWidget ChooseWidget;
struct ChooseWidget {
    const char *label;
    const char **choices;
    u16 num_choices;
    Pref pref;
};

typedef enum ButtonFlag ButtonFlag;
enum ButtonFlag {
    ButtonFlag_CloseMenu = 1 << 0,  // Close menu after pushed
    ButtonFlag_GoBack = 1 << 1,     // Go back to parent menu after pushed
};

typedef struct ButtonWidget ButtonWidget;
struct ButtonWidget {
    const char *label;
    void (*push)();  // Runs when pushed. Can be null
    ButtonFlag flags;
};

// Pretty limited for now
typedef struct IntEditWidget IntEditWidget;
struct IntEditWidget {
    const char *label;
    Pref pref;
    u8 min;
    u8 max;
};

// even more limited for now
typedef struct FloatEditWidget FloatEditWidget;
struct FloatEditWidget {
    const char *label;
    Pref pref;
    u32 precision;  // denominator, 100
    u8 min;
    u8 max;
    s32 floor;
    u8 decimals;
};

typedef struct InputSelectWidget InputSelectWidget;
struct InputSelectWidget {
    const char *label;
    Pref pref;
    bool required_chord;  // must be a 2 button bind if true
    bool can_unbind;
};

enum class HideableType : u8 { U8Hideable, BoolHideable };

typedef struct HideableGroupWidget HideableGroupWidget;
struct HideableGroupWidget {
    struct Widget *widgets;
    u32 num_widgets;
    bool (*show_if)();  // show if function returns true
};

typedef struct CustomWidget CustomWidget;
struct CustomWidget {
    void (*draw)();
};

typedef struct Widget Widget;
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
    };
};

extern MenuWidget menu_root;

void menu_init();
