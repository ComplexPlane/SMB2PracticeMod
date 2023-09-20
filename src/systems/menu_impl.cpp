#include "menu_impl.h"

#include "../mkb/mkb.h"
#include "../systems/binds.h"
#include "../systems/log.h"
#include "../systems/menu_defn.h"
#include "../systems/pad.h"
#include "../systems/pref.h"
#include "../utils/draw.h"
#include "../utils/macro_utils.h"

using namespace menu_defn;

namespace menu_impl {

static constexpr s32 SCREEN_WIDTH = 640;
static constexpr s32 SCREEN_HEIGHT = 480;
static constexpr s32 MARGIN = 20;
static constexpr s32 PAD = 8;
static constexpr s32 LINE_HEIGHT = 20;

static const mkb::GXColor FOCUSED_COLOR = draw::LIGHT_GREEN;
static const mkb::GXColor UNFOCUSED_COLOR = draw::LIGHT_PURPLE;

static bool s_visible;
static u32 s_cursor_frame = 0;

constexpr u32 MENU_STACK_SIZE = 5;
static MenuWidget* s_menu_stack[MENU_STACK_SIZE] = {&root_menu};
static u32 s_menu_stack_ptr = 0;

static s32 s_intedit_tick = 0;

static bool s_binding = false;
static u32 s_delay_frames = 0;
static const char* INPUT_STRINGS[] = {
    "A", "B", "X", "Y", "L", "R", "Z", "Dpad-Up", "Dpad-Down", "Dpad-Left", "Dpad-Right", "Start"};

static void push_menu(MenuWidget* menu) {
    MOD_ASSERT(s_menu_stack_ptr < MENU_STACK_SIZE - 1);  // Menu stack overflow
    s_menu_stack_ptr++;
    s_menu_stack[s_menu_stack_ptr] = menu;
    s_cursor_frame = 0;
    pad::reset_dir_repeat();
}

static void pop_menu() {
    if (s_menu_stack_ptr == 0) {
        s_visible = false;
    } else {
        s_menu_stack_ptr--;
    }
    s_cursor_frame = 0;
    pad::reset_dir_repeat();
}

static bool show_hideable_widget(Widget* widget) {
    HideableWidget hideable = widget->hideable_widget;
    switch (hideable.hideable_type) {
        case HideableType::BoolHideable: {
            return pref::get(hideable.bool_pref);
        }
        case HideableType::U8Hideable: {
            return pref::get(hideable.u8_pref) == hideable.show_if;
        }
        default: {
            return false;
        }
    }
}

static bool is_widget_selectable(WidgetType type) {
    return type == WidgetType::Checkbox || type == WidgetType::GetSetCheckbox ||
           type == WidgetType::Menu || type == WidgetType::Choose || type == WidgetType::Button ||
           type == WidgetType::IntEdit || type == WidgetType::FloatEdit ||
           type == WidgetType::InputSelect;
}

static Widget* get_sub_selected_widget(HideableWidget* widget, s32* selectable, s32* sel) {
    for (u32 i = 0; i < widget->num_widgets; i++) {
        Widget* child = &widget->widget[i];
        if (is_widget_selectable(child->type)) {
            *selectable += 1;
            if (*selectable == *sel) return child;
        } else if (child->type == WidgetType::HideableWidget && show_hideable_widget(child)) {
            Widget* possible_selection =
                get_sub_selected_widget(&child->hideable_widget, selectable, sel);
            if (possible_selection != nullptr) {
                return possible_selection;
            }
        }
    }

    return nullptr;
}

static Widget* get_selected_widget() {
    MenuWidget* menu = s_menu_stack[s_menu_stack_ptr];
    s32 sel = menu->selected_idx;

    s32 selectable = -1;
    for (u32 i = 0; i < menu->num_widgets; i++) {
        Widget* child = &menu->widgets[i];
        if (is_widget_selectable(child->type)) {
            selectable++;
            if (selectable == sel) return child;
        } else if (child->type == WidgetType::HideableWidget && show_hideable_widget(child)) {
            Widget* possible_selection =
                get_sub_selected_widget(&child->hideable_widget, &selectable, &sel);
            if (possible_selection != nullptr) {
                return possible_selection;
            }
        }
    }

    return nullptr;
}

static u32 get_submenu_selectable_widget_count(HideableWidget widget) {
    u32 selectable = 0;
    for (u32 i = 0; i < widget.num_widgets; i++) {
        Widget child = widget.widget[i];
        if (is_widget_selectable(child.type)) {
            selectable++;
        } else if (child.type == WidgetType::HideableWidget && show_hideable_widget(&child)) {
            selectable += get_submenu_selectable_widget_count(child.hideable_widget);
        }
    }
    return selectable;
}

static u32 get_menu_selectable_widget_count(MenuWidget* menu) {
    u32 selectable = 0;

    for (u32 i = 0; i < menu->num_widgets; i++) {
        Widget* child = &menu->widgets[i];
        if (is_widget_selectable(child->type)) {
            selectable++;
        } else if (child->type == WidgetType::HideableWidget && show_hideable_widget(child)) {
            selectable += get_submenu_selectable_widget_count(child->hideable_widget);
        }
    }
    return selectable;
}

static void handle_widget_bind() {
    Widget* selected = get_selected_widget();
    if (selected == nullptr) return;

    bool a_pressed = pad::button_pressed(mkb::PAD_BUTTON_A, true);
    bool x_pressed = pad::button_pressed(mkb::PAD_BUTTON_X, true);
    bool y_pressed = pad::button_pressed(mkb::PAD_BUTTON_Y, true);
    bool a_repeat = pad::button_repeat(mkb::PAD_BUTTON_A, true);
    bool y_repeat = pad::button_repeat(mkb::PAD_BUTTON_Y, true);

    // slow down scroll
    if (s_intedit_tick > 0) {
        s_intedit_tick--;
    }

    if (selected->type == WidgetType::Checkbox) {
        auto& checkbox = selected->checkbox;
        if (a_pressed || y_pressed) {
            pref::set(checkbox.pref, !pref::get(checkbox.pref));
            pref::save();
        }
        if (x_pressed) {
            pref::set(checkbox.pref, pref::get_default(checkbox.pref));
            pref::save();
        }

    } else if (selected->type == WidgetType::GetSetCheckbox) {
        auto& get_set_checkbox = selected->get_set_checkbox;
        if (a_pressed || y_pressed) {
            get_set_checkbox.set(!get_set_checkbox.get());
        }

    } else if (selected->type == WidgetType::Menu && a_pressed) {
        push_menu(&selected->menu);

    } else if (selected->type == WidgetType::Choose) {
        auto& choose = selected->choose;
        if (a_pressed) {
            u8 new_value = (static_cast<u32>(pref::get(choose.pref)) + 1) % choose.num_choices;
            pref::set(choose.pref, new_value);
            pref::save();
        }
        if (y_pressed) {
            u8 new_value = (static_cast<u32>(pref::get(choose.pref)) + choose.num_choices - 1) %
                           choose.num_choices;
            pref::set(choose.pref, new_value);
            pref::save();
        }
        if (x_pressed) {
            pref::set(choose.pref, pref::get_default(choose.pref));
            pref::save();
        }

    } else if (selected->type == WidgetType::Button && a_pressed) {
        auto& button = selected->button;
        if (button.push != nullptr) {
            selected->button.push();
        }
        if (button.flags & ButtonFlags::CloseMenu) {
            s_visible = false;
        }
        if (button.flags & ButtonFlags::GoBack) {
            pop_menu();
        }

    } else if (selected->type == WidgetType::IntEdit) {
        auto& int_edit = selected->int_edit;
        int next = pref::get(int_edit.pref);
        if (a_repeat || y_repeat) {
            s_intedit_tick += 5;
        }
        u8 edit_speed = 1;
        if (s_intedit_tick < 15) {
            edit_speed = 1;
        } else if (s_intedit_tick < 25) {
            edit_speed = 2;
        } else {
            edit_speed = 5;
        }

        if (a_repeat) {
            next += edit_speed;
        }
        if (y_repeat) {
            next -= edit_speed;
        }
        if (x_pressed) {
            next = pref::get_default(int_edit.pref);
        }
        next = CLAMP(next, int_edit.min, int_edit.max);
        if (next != pref::get(int_edit.pref)) {
            pref::set(int_edit.pref, next);
            pref::save();
        }
    } else if (selected->type == WidgetType::FloatEdit) {
        auto& float_edit = selected->float_edit;
        int next = pref::get(float_edit.pref);
        if (a_repeat || y_repeat) {
            s_intedit_tick += 5;
        }
        u8 edit_speed = 1;
        if (s_intedit_tick < 15) {
            edit_speed = 1;
        } else if (s_intedit_tick < 25) {
            edit_speed = 2;
        } else {
            edit_speed = 5;
        }

        if (a_repeat) {
            next += edit_speed;
        }
        if (y_repeat) {
            next -= edit_speed;
        }
        if (x_pressed) {
            next = pref::get_default(float_edit.pref);
        }
        next = CLAMP(next, float_edit.min, float_edit.max);
        if (next != pref::get(float_edit.pref)) {
            pref::set(float_edit.pref, next);
            pref::save();
        }
    } else if (selected->type == WidgetType::InputSelect) {
        auto& input_select = selected->input_select;
        if (s_binding) {
            // set new bind
            u8 new_value = binds::get_current_encoding();
            if (new_value != 0) {
                pref::set(input_select.pref, new_value);
                pref::save();
                s_binding = false;
            }
        } else if (a_pressed) {
            // enter rebind mode
            s_binding = true;
        } else if (x_pressed) {
            pref::set(input_select.pref, pref::get_default(input_select.pref));
            pref::save();
        }
    }
}

void tick() {
    if (s_binding) {
        handle_widget_bind();
        return;
    }
    // TODO save settings on close
    // TODO save menu position as settings
    bool toggle = binds::button_chord_pressed(pref::get(pref::U8Pref::MenuBind), true);
    if (toggle) {
        s_visible ^= toggle;
    } else if (pad::button_pressed(mkb::PAD_BUTTON_B, true)) {
        pop_menu();
    }
    bool just_opened = s_visible && toggle;
    if (just_opened) {
        pad::reset_dir_repeat();
        s_cursor_frame = 0;
        s_delay_frames = 15;
    }
    if (s_delay_frames > 0) {
        s_delay_frames--;
    }

    pad::set_exclusive_mode(s_visible);

    if (!s_visible || s_delay_frames > 0) return;

    MenuWidget* menu = s_menu_stack[s_menu_stack_ptr];

    // Update selected menu item
    s32 dir_delta = pad::dir_repeat(pad::DIR_DOWN, true) - pad::dir_repeat(pad::DIR_UP, true);
    u32 selectable = get_menu_selectable_widget_count(menu);
    menu->selected_idx = (menu->selected_idx + dir_delta + selectable) % selectable;

    // Make selected menu item green if selection changed or menu opened
    if (dir_delta != 0 || just_opened)
        s_cursor_frame = 0;
    else
        s_cursor_frame++;

    handle_widget_bind();
}

static mkb::GXColor lerp_colors(mkb::GXColor color1, mkb::GXColor color2, f32 t) {
    f32 r = (1.f - t) * color1.r + t * color2.r;
    f32 g = (1.f - t) * color1.g + t * color2.g;
    f32 b = (1.f - t) * color1.b + t * color2.b;
    f32 a = (1.f - t) * color1.a + t * color2.a;

    mkb::GXColor ret;
    ret.r = CLAMP(r, 0, 255);
    ret.g = CLAMP(g, 0, 255);
    ret.b = CLAMP(b, 0, 255);
    ret.a = CLAMP(a, 0, 255);

    return ret;
}

static f32 sin_lerp(s32 period_frames) {
    f32 angle = (static_cast<s32>(s_cursor_frame % period_frames) - (period_frames / 2.f)) *
                0x8000 / (period_frames / 2.f);
    f32 lerp = (mkb::math_sin(angle) + 1.f) / 2.f;
    return lerp;
}

static void draw_selectable_highlight(float y) {
    // float new_y = y * 1.072 - 3; // Do NOT ask why we need this
    // draw::rect(MARGIN, new_y, SCREEN_WIDTH - MARGIN, (new_y + LINE_HEIGHT), {0, 0, 0, 0xFF});

    // Draw selection arrow
    draw::debug_text(MARGIN + PAD + 2, y, FOCUSED_COLOR, "\x1c");
}

void draw_sub_widget(Widget& widget, u32 selected_idx, u32* selectable_idx, u32* y,
                     mkb::GXColor lerped_color) {
    switch (widget.type) {
        case WidgetType::HideableWidget: {
            if (show_hideable_widget(&widget)) {
                for (u32 i = 0; i < widget.hideable_widget.num_widgets; i++) {
                    Widget& w = widget.hideable_widget.widget[i];
                    draw_sub_widget(w, selected_idx, selectable_idx, y, lerped_color);
                }
            }
            break;
        }
        case WidgetType::Header: {
            draw::debug_text(MARGIN + PAD, *y, draw::ORANGE, widget.header.label);
            *y += LINE_HEIGHT;
            break;
        }
        case WidgetType::Text: {
            draw::debug_text(MARGIN + PAD, *y, draw::WHITE, widget.text.label);
            *y += LINE_HEIGHT;
            break;
        }
        case WidgetType::ColoredText: {
            draw::debug_text(MARGIN + PAD, *y, widget.colored_text.color,
                             widget.colored_text.label);
            *y += LINE_HEIGHT;
            break;
        }
        case WidgetType::Checkbox:
        case WidgetType::GetSetCheckbox: {
            const char* label = nullptr;
            bool value = false;
            if (widget.type == WidgetType::Checkbox) {
                label = widget.checkbox.label;
                value = pref::get(widget.checkbox.pref);
            } else {
                label = widget.get_set_checkbox.label;
                value = widget.get_set_checkbox.get();
            }

            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "  %s", label);
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "                         %s", value ? "On" : "Off");

            *y += LINE_HEIGHT;
            *selectable_idx += 1;
            break;
        }
        case WidgetType::Separator: {
            *y += LINE_HEIGHT / 2;
            break;
        }
        case WidgetType::Menu: {
            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "  %s", widget.menu.label);

            // Draw "..." with dots closer together
            for (s32 i = 0; i < 3; i++) {
                draw::debug_text(MARGIN + PAD + 25 * draw::DEBUG_CHAR_WIDTH + i * 6, *y,
                                 selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                                 ".");
            }

            *selectable_idx += 1;
            *y += LINE_HEIGHT;
            break;
        }
        case WidgetType::FloatView: {
            draw::debug_text(MARGIN + PAD, *y, draw::WHITE, "%s", widget.float_view.label);
            draw::debug_text(MARGIN + PAD, *y, draw::GREEN, "                         %.3Ef",
                             widget.float_view.get());
            y += LINE_HEIGHT;
            break;
        }
        case WidgetType::Choose: {
            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "  %s", widget.choose.label);
            draw::debug_text(
                MARGIN + PAD, *y, selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                "                         (%d/%d) %s", pref::get(widget.choose.pref) + 1,
                widget.choose.num_choices, widget.choose.choices[pref::get(widget.choose.pref)]);

            *y += LINE_HEIGHT;
            *selectable_idx += 1;
            break;
        }
        case WidgetType::Button: {
            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "  %s", widget.button.label);

            *y += LINE_HEIGHT;
            *selectable_idx += 1;
            break;
        }
        case WidgetType::IntEdit: {
            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "  %s", widget.int_edit.label);
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "                         %d", pref::get(widget.int_edit.pref));

            *y += LINE_HEIGHT;
            *selectable_idx += 1;
            break;
        }
        case WidgetType::FloatEdit: {
            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }

            float display =
                (float)pref::get(widget.float_edit.pref) / (float)widget.float_edit.precision;

            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "  %s", widget.float_edit.label);
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "                         %0.3f", display);

            *y += LINE_HEIGHT;
            *selectable_idx += 1;
            break;
        }
        case WidgetType::InputSelect: {
            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }
            u8 input = pref::get(widget.input_select.pref);
            if (s_binding) {
                draw::debug_text(MARGIN + PAD, *y, draw::GOLD, "  %s", widget.input_select.label);
            } else {
                draw::debug_text(MARGIN + PAD, *y,
                                 selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                                 "  %s", widget.input_select.label);
            }
            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "                         (%s+%s)", INPUT_STRINGS[input % 12],
                             INPUT_STRINGS[(input - (input % 12)) / 12]);

            *y += LINE_HEIGHT;
            *selectable_idx += 1;
            break;
        }
        case WidgetType::Custom: {
            widget.custom.draw();
            break;
        }
    }
}

void draw_menu_widget(MenuWidget* menu) {
    u32 y = MARGIN + PAD + 2.f * LINE_HEIGHT;
    u32 selectable_idx = 0;

    mkb::GXColor lerped_color = lerp_colors(FOCUSED_COLOR, UNFOCUSED_COLOR, sin_lerp(40));

    for (u32 i = 0; i < menu->num_widgets; i++) {
        Widget& widget = menu->widgets[i];
        draw_sub_widget(widget, menu->selected_idx, &selectable_idx, &y, lerped_color);
    }
}

static void draw_breadcrumbs() {
    const char* ARROW_STR = " \x1c ";

    u32 x = MARGIN + PAD;
    for (u32 i = 0; i <= s_menu_stack_ptr; i++) {
        MenuWidget* menu = s_menu_stack[i];
        mkb::GXColor grey = {0xE0, 0xE0, 0xE0, 0xFF};
        draw::debug_text(x, MARGIN + PAD, i == s_menu_stack_ptr ? draw::PURPLE : grey, menu->label);
        x += mkb::strlen(const_cast<char*>(menu->label)) * draw::DEBUG_CHAR_WIDTH;
        if (i != s_menu_stack_ptr) {
            draw::debug_text(x, MARGIN + PAD, draw::BLUE, ARROW_STR);
            x += mkb::strlen(const_cast<char*>(ARROW_STR)) * draw::DEBUG_CHAR_WIDTH;
        }
    }

    // Draw line under breadcrumbs. You can draw lines directly with GX but I couldn't get it
    // working
    draw::rect(MARGIN, MARGIN + 30, SCREEN_WIDTH - MARGIN, MARGIN + 34, {0x70, 0x70, 0x70, 0xFF});
}

void disp() {
    if (!s_visible) {
        // Default binding is L+R, but this lets you know the current binding in case you forget
        // what you changed it to
        u8 input = pref::get(pref::U8Pref::MenuBind);
        if (pad::button_chord_pressed(mkb::PAD_TRIGGER_L, mkb::PAD_TRIGGER_R, true) &&
            input != 64) {
            draw::notify(draw::RED, "Use %s+%s to toggle menu", INPUT_STRINGS[input % 12],
                         INPUT_STRINGS[(input - (input % 12)) / 12]);
        }
        return;
    }

    draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN,
               {0x00, 0x00, 0x00, 0xe0});
    draw_breadcrumbs();
    draw_menu_widget(s_menu_stack[s_menu_stack_ptr]);
}

bool is_visible() { return s_visible; }

}  // namespace menu_impl
