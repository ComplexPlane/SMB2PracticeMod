#include "menu_impl.h"

#include "mkb/mkb.h"
#include "systems/binds.h"
#include "systems/log.h"
#include "systems/menu_defn.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

using namespace menu_defn;

namespace menu_impl {

enum class BindingState {
    Inactive,   // not currently binding
    Requested,  // will bind as soon as all buttons are released
    Active,     // currently binding
};
static BindingState s_binding = BindingState::Inactive;

static constexpr s32 SCREEN_WIDTH = 640;
static constexpr s32 SCREEN_HEIGHT = 480;
static constexpr s32 MARGIN = 20;
static constexpr s32 PAD = 8;
static constexpr s32 LINE_HEIGHT = 20;

static constexpr s32 L_R_BIND = 64;  // bind id for an L+R bind

static const mkb::GXColor FOCUSED_COLOR = draw::LIGHT_GREEN;
static const mkb::GXColor UNFOCUSED_COLOR = draw::LIGHT_PURPLE;

static bool s_visible;
static u32 s_cursor_frame = 0;

constexpr u32 MENU_STACK_SIZE = 5;
static MenuWidget* s_menu_stack[MENU_STACK_SIZE] = {&root_menu};
static u32 s_menu_stack_ptr = 0;

static s32 s_intedit_tick = 0;
static s32 s_edit_tick = 0;

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

static bool is_widget_selectable(WidgetType type) {
    return type == WidgetType::Checkbox || type == WidgetType::GetSetCheckbox ||
           type == WidgetType::Menu || type == WidgetType::Choose || type == WidgetType::Button ||
           type == WidgetType::IntEdit || type == WidgetType::FloatEdit ||
           type == WidgetType::InputSelect;
}

static Widget* get_selected_widget(Widget* widgets, u32 num_widgets, s32& curr_idx,
                                   s32 target_idx) {
    for (u32 i = 0; i < num_widgets; i++) {
        if (is_widget_selectable(widgets[i].type)) {
            curr_idx++;
            if (curr_idx == target_idx) return &widgets[i];
        } else if (widgets[i].type == WidgetType::HideableGroupWidget &&
                   widgets[i].hideable_group.show_if()) {
            Widget* possible_selection =
                get_selected_widget(widgets[i].hideable_group.widgets,
                                    widgets[i].hideable_group.num_widgets, curr_idx, target_idx);
            if (possible_selection != nullptr) {
                return possible_selection;
            }
        }
    }
    return nullptr;
}

static u32 get_selectable_widget_count(Widget* widgets, u32 num_widgets) {
    u32 selectable = 0;

    for (u32 i = 0; i < num_widgets; i++) {
        Widget child = widgets[i];
        if (is_widget_selectable(child.type)) {
            selectable++;
        } else if (child.type == WidgetType::HideableGroupWidget &&
                   child.hideable_group.show_if()) {
            selectable += get_selectable_widget_count(child.hideable_group.widgets,
                                                      child.hideable_group.num_widgets);
        }
    }
    return selectable;
}

static void handle_widget_bind() {
    MenuWidget* menu = s_menu_stack[s_menu_stack_ptr];
    s32 target_idx = menu->selected_idx;
    s32 curr_idx = -1;
    Widget* selected = get_selected_widget(menu->widgets, menu->num_widgets, curr_idx, target_idx);
    if (selected == nullptr) return;

    bool a_pressed = pad::button_pressed(mkb::PAD_BUTTON_A, true);
    bool x_pressed = pad::button_pressed(mkb::PAD_BUTTON_X, true);
    bool y_pressed = pad::button_pressed(mkb::PAD_BUTTON_Y, true);
    bool a_repeat = pad::button_repeat(mkb::PAD_BUTTON_A, true);
    bool y_repeat = pad::button_repeat(mkb::PAD_BUTTON_Y, true);

    // slow down scroll
    if (s_edit_tick > 0) {
        s_edit_tick--;
    } else if (s_edit_tick < 0) {
        s_edit_tick++;
    }
    if (s_intedit_tick > 0) {
        s_intedit_tick--;
    }

    switch (selected->type) {
        case WidgetType::Checkbox: {
            auto& checkbox = selected->checkbox;
            if (a_pressed || y_pressed) {
                pref::set(checkbox.pref, !pref::get(checkbox.pref));
                pref::save();
            }
            if (x_pressed) {
                pref::set(checkbox.pref, pref::get_default(checkbox.pref));
                pref::save();
            }
            break;
        }
        case WidgetType::GetSetCheckbox: {
            auto& get_set_checkbox = selected->get_set_checkbox;
            if (a_pressed || y_pressed) {
                get_set_checkbox.set(!get_set_checkbox.get());
            }
            break;
        }
        case WidgetType::Menu: {
            if (a_pressed) {
                push_menu(&selected->menu);
            }
            break;
        }
        case WidgetType::Choose: {
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
            break;
        }
        case WidgetType::Button: {
            if (a_pressed) {
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
            }
            break;
        }
        case WidgetType::IntEdit:
        case WidgetType::FloatEdit: {
            int next;
            pref::U8Pref edit_pref;
            u8 min, max;
            if (selected->type == WidgetType::IntEdit) {
                auto& int_edit = selected->int_edit;
                next = pref::get(int_edit.pref);
                edit_pref = int_edit.pref;
                min = int_edit.min;
                max = int_edit.max;
            } else {
                auto& float_edit = selected->float_edit;
                next = pref::get(float_edit.pref);
                edit_pref = float_edit.pref;
                min = float_edit.min;
                max = float_edit.max;
            }

            if (pad::button_released(mkb::PAD_BUTTON_A) && s_edit_tick > 0) {
                s_edit_tick = 0;
            } else if (pad::button_released(mkb::PAD_BUTTON_Y) && s_edit_tick < 0) {
                s_edit_tick = 0;
            }

            if (x_pressed) {
                next = pref::get_default(edit_pref);
            } else if (a_repeat && !pad::button_down(mkb::PAD_BUTTON_Y, true)) {
                s_edit_tick += 5;
                next += (s_edit_tick / 5);
            } else if (y_repeat && !pad::button_down(mkb::PAD_BUTTON_A, true)) {
                s_edit_tick -= 5;
                next += (s_edit_tick / 5);
            }
            next = CLAMP(next, min, max);
            if (next != pref::get(edit_pref)) {
                pref::set(edit_pref, next);
                pref::save();
            }
            break;
        }
        case WidgetType::InputSelect: {
            auto& input_select = selected->input_select;
            if (s_binding == BindingState::Requested &&
                pad::button_released(mkb::PAD_BUTTON_A, true)) {
                s_binding = BindingState::Active;
            } else if (s_binding == BindingState::Active) {
                // set new bind
                binds::EncodingType type = binds::get_encoding_type();
                if (type == binds::EncodingType::Invalid ||
                    (type == binds::EncodingType::SinglePress && input_select.required_chord))
                    return;
                u8 value = binds::get_current_encoding();
                pref::set(input_select.pref, value);
                pref::save();
                s_binding = BindingState::Inactive;
            } else if (a_pressed) {
                // enter rebind mode
                s_binding = BindingState::Requested;
            } else if (y_pressed) {
                // unbind
                if (!input_select.can_unbind) return;
                pref::set(input_select.pref, 255);
                pref::save();
            } else if (x_pressed) {
                // reset default bind
                pref::set(input_select.pref, pref::get_default(input_select.pref));
                pref::save();
            }
            break;
        }
        default: {
            break;
        }
    }
}

void tick() {
    if (s_binding == BindingState::Active) {
        handle_widget_bind();
        return;
    }

    // TODO save settings on close
    // TODO save menu position as settings
    bool toggle = binds::bind_pressed(pref::get(pref::U8Pref::MenuBind), true);
    if (toggle) {
        s_visible ^= toggle;
    } else if (pad::button_pressed(mkb::PAD_BUTTON_B, true)) {
        pop_menu();
    }
    bool just_opened = s_visible && toggle;
    if (just_opened) {
        pad::reset_dir_repeat();
        s_cursor_frame = 0;
    }

    pad::set_exclusive_mode(s_visible);

    if (!s_visible) {
        // Default binding is L+R, but this lets you know the current binding in case you forget
        // what you changed it to
        u8 input = pref::get(pref::U8Pref::MenuBind);
        if (pad::button_chord_pressed(mkb::PAD_TRIGGER_L, mkb::PAD_TRIGGER_R, true) &&
            input != L_R_BIND) {
            char buf[25];
            binds::get_bind_str(input, buf);
            draw::notify(draw::RED, "Use %s to toggle menu", buf);
        }
        return;
    }

    MenuWidget* menu = s_menu_stack[s_menu_stack_ptr];

    // Update selected menu item
    s32 dir_delta = pad::dir_repeat(pad::DIR_DOWN, true) - pad::dir_repeat(pad::DIR_UP, true);
    u32 selectable = get_selectable_widget_count(menu->widgets, menu->num_widgets);
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

static constexpr s32 BLOCK_WIDTH = 150;
static constexpr s32 START = MARGIN + 35;
static constexpr s32 BUTTON_START = -83;
static constexpr s32 Y_HEIGHT = SCREEN_HEIGHT - MARGIN - 52;
static constexpr s32 HALF_SPACE = 12;

static void draw_help_layout() {
    // draw seperator
    draw::rect(MARGIN, SCREEN_HEIGHT - MARGIN - 34, SCREEN_WIDTH - MARGIN,
               SCREEN_HEIGHT - MARGIN - 30, draw::GRAY);
    // draw b: back
    draw::debug_text(START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::LIGHT_RED, "B");
    draw::debug_text(BUTTON_START + 4 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
    draw::debug_text(BUTTON_START + 4 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE, "Back");
}

static void draw_help(const Widget& widget) {
    // draw relevant controls for current widget
    switch (widget.type) {
        case WidgetType::Checkbox:
        case WidgetType::GetSetCheckbox: {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Toggle");
            break;
        }
        case WidgetType::Menu: {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Open");
            break;
        }
        case WidgetType::Button: {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Activate");
            break;
        }
        case WidgetType::Choose: {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Next");
            draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
            draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Previous");
            draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
            draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Reset");
            break;
        }
        case WidgetType::IntEdit:
        case WidgetType::FloatEdit: {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Increase");
            draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
            draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Decrease");
            draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
            draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Reset");
            break;
        }
        case WidgetType::InputSelect: { 
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Bind");
            if (widget.input_select.can_unbind) {
                draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
                draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
                draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                                 "Unbind");
            }
            draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
            draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
                             "Reset");
            break;
        }
        default: {
            break;
        }
    }
}

void draw_widget(Widget& widget, u32 selected_idx, u32* selectable_idx, u32* y,
                 mkb::GXColor lerped_color) {
    switch (widget.type) {
        case WidgetType::HideableGroupWidget: {
            if (widget.hideable_group.show_if()) {
                for (u32 i = 0; i < widget.hideable_group.num_widgets; i++) {
                    Widget& w = widget.hideable_group.widgets[i];
                    draw_widget(w, selected_idx, selectable_idx, y, lerped_color);
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
            (*selectable_idx)++;
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

            (*selectable_idx)++;
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
            (*selectable_idx)++;
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
            (*selectable_idx)++;
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
            (*selectable_idx)++;
            break;
        }
        case WidgetType::FloatEdit: {
            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }

            float display = ((float)(pref::get(widget.float_edit.pref) + widget.float_edit.floor) /
                             (float)widget.float_edit.precision);

            draw::debug_text(MARGIN + PAD, *y,
                             selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                             "  %s", widget.float_edit.label);
            switch (widget.float_edit.decimals) {
                case 2: {
                    draw ::debug_text(
                        MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                        "                         %0.2f", display);
                    break;
                }
                default: {
                    draw ::debug_text(
                        MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                        "                         %0.3f", display);
                    break;
                }
            }

            *y += LINE_HEIGHT;
            (*selectable_idx)++;
            break;
        }
        case WidgetType::InputSelect: {
            if (selected_idx == *selectable_idx) {
                draw_selectable_highlight(*y);
            }
            if (s_binding == BindingState::Active && selected_idx == *selectable_idx) {
                draw::debug_text(MARGIN + PAD, *y, FOCUSED_COLOR, "  %s",
                                 widget.input_select.label);
            } else {
                draw::debug_text(MARGIN + PAD, *y,
                                 selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                                 "  %s", widget.input_select.label);
            }
            mkb::GXColor bind_color =
                selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR;
            if (s_binding == BindingState::Active && selected_idx == *selectable_idx) {
                bind_color = draw::GOLD;
            }
            u8 input = pref::get(widget.input_select.pref);
            char buf[25];
            binds::get_bind_str(input, buf);
            draw::debug_text(MARGIN + PAD, *y, bind_color, "                         %s", buf);

            *y += LINE_HEIGHT;
            (*selectable_idx)++;
            break;
        }
        case WidgetType::Custom: {
            widget.custom.draw();
            break;
        }
    }
}

void draw_menu_widgets(MenuWidget* menu) {
    u32 y = MARGIN + PAD + 2.f * LINE_HEIGHT;
    u32 selectable_idx = 0;

    mkb::GXColor lerped_color = lerp_colors(FOCUSED_COLOR, UNFOCUSED_COLOR, sin_lerp(40));

    for (u32 i = 0; i < menu->num_widgets; i++) {
        Widget& widget = menu->widgets[i];
        draw_widget(widget, menu->selected_idx, &selectable_idx, &y, lerped_color);
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
    draw::rect(MARGIN, MARGIN + 30, SCREEN_WIDTH - MARGIN, MARGIN + 34, draw::GRAY);
}

void disp() {
    if (!s_visible) return;
    MenuWidget* menu = s_menu_stack[s_menu_stack_ptr];
    draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN,
               {0x00, 0x00, 0x00, 0xe0});
    draw_breadcrumbs();
    draw_menu_widgets(menu);
    draw_help_layout();
    s32 curr_idx = -1;
    Widget* selected =
        get_selected_widget(menu->widgets, menu->num_widgets, curr_idx, menu->selected_idx);
    if (selected != nullptr) {
        draw_help(*selected);
    }
}

bool is_visible() { return s_visible; }

}  // namespace menu_impl
