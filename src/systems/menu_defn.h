#pragma once

#include "mkb/mkb.h"

#include "pref.h"

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

// Something we can get/set/get default
// I'm probably going to regret this...
template <typename ValueType, typename PrefType>
class PrefLike {
 public:
    PrefLike(ValueType (*get)(), void (*set)(ValueType), ValueType (*get_default)()) {}
    PrefLike(PrefType pref);

 private:
    bool is_pref;
    union {
        PrefType pref;
    }
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

namespace CheckboxFlags {
enum {
    Pref = 1 << 0,          // Use bool preference ID
    GetterSetter = 1 << 1,  // Use manual getters/setters
};
}

struct CheckboxWidget {
    const char* label;
    u32 flags;
    union {
        pref::BoolPref pref;
        struct {
            // We can't use std::function due to destructors in unions stuff
            bool (*get)();
            void (*set)(bool);
            bool (*get_default)();
        };
    };
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
    u16 num_choices;
    pref::U8Pref pref;
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
