#include "menu_impl.h"

#include "mkb/mkb.h"
#include "systems/log.h"
#include "systems/menu_defn.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

using namespace menu_defn;

namespace menu_impl {

constexpr s32 SCREEN_WIDTH = 640;
constexpr s32 SCREEN_HEIGHT = 480;
constexpr s32 MARGIN = 20;
constexpr s32 PAD = 8;
constexpr s32 LINE_HEIGHT = 20;

static bool s_visible;
static u32 s_cursor_frame = 0;

constexpr u32 MENU_STACK_SIZE = 5;
static MenuWidget* s_menu_stack[MENU_STACK_SIZE] = {&root_menu};
static u32 s_menu_stack_ptr = 0;

static void push_menu(MenuWidget* menu) {
    MOD_ASSERT(s_menu_stack_ptr < MENU_STACK_SIZE - 1);  // Menu stack overflow
    s_menu_stack_ptr++;
    s_menu_stack[s_menu_stack_ptr] = menu;
}

static void pop_menu() {
    if (s_menu_stack_ptr == 0) {
        s_visible = false;
    } else {
        s_menu_stack_ptr--;
    }
}

static Widget* get_selected_widget() {
    MenuWidget* menu = s_menu_stack[s_menu_stack_ptr];
    s32 sel = menu->selected_idx;

    s32 selectable = -1;
    for (u32 i = 0; i < menu->num_widgets; i++) {
        Widget* child = &menu->widgets[i];
        if (child->type == WidgetType::Checkbox || child->type == WidgetType::Menu ||
            child->type == WidgetType::Choose || child->type == WidgetType::Button) {
            selectable++;
            if (selectable == sel) return child;
        }
    }

    return nullptr;
}

static u32 get_menu_selectable_widget_count(MenuWidget* menu) {
    u32 selectable = 0;
    for (u32 i = 0; i < menu->num_widgets; i++) {
        Widget* child = &menu->widgets[i];
        if (child->type == WidgetType::Checkbox || child->type == WidgetType::Menu ||
            child->type == WidgetType::Choose || child->type == WidgetType::Button) {
            selectable++;
        }
    }
    return selectable;
}

static void handle_widget_bind() {
    bool a_pressed = pad::button_pressed(mkb::PAD_BUTTON_A, true);
    bool y_pressed = pad::button_pressed(mkb::PAD_BUTTON_Y, true);

    Widget* selected = get_selected_widget();
    if (selected == nullptr) return;

    if (selected->type == WidgetType::Checkbox && a_pressed) {
        selected->checkbox.set(!selected->checkbox.get());
        pref::save();
    } else if (selected->type == WidgetType::Menu && a_pressed) {
        push_menu(&selected->menu);
        s_cursor_frame = 0;
    } else if (selected->type == WidgetType::Choose) {
        auto& choose = selected->choose;
        if (a_pressed) {
            choose.set((choose.get() + 1) % choose.num_choices);
            pref::save();
        }
        if (y_pressed) {
            choose.set((choose.get() + choose.num_choices - 1) % choose.num_choices);
            pref::save();
        }
        // TODO handle setting default value
    } else if (selected->type == WidgetType::Button && a_pressed) {
        selected->button.push();
        s_visible = false;
    }
}

void tick() {
    // TODO save settings on close
    // TODO save menu position as settings
    bool toggle = false;
    if (pad::button_pressed(mkb::PAD_BUTTON_B, true)) {
        pop_menu();
        s_cursor_frame = 0;
    } else {
        toggle = pad::button_chord_pressed(mkb::PAD_TRIGGER_L, mkb::PAD_TRIGGER_R, true);
        s_visible ^= toggle;
    }
    bool just_opened = s_visible && toggle;

    pad::set_exclusive_mode(s_visible);

    if (!s_visible) return;

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

void draw_menu_widget(MenuWidget* menu) {
    u32 y = MARGIN + PAD + 2.f * LINE_HEIGHT;
    u32 selectable_idx = 0;
    s32 cursor_y = -1;

    mkb::GXColor focused = draw::LIGHT_GREEN;
    mkb::GXColor unfocused = draw::LIGHT_PURPLE;
    mkb::GXColor lerped_color = lerp_colors(focused, unfocused, sin_lerp(40));

    for (u32 i = 0; i < menu->num_widgets; i++) {
        Widget& widget = menu->widgets[i];

        switch (widget.type) {
            case WidgetType::Header: {
                draw::debug_text(MARGIN + PAD, y, draw::ORANGE, widget.header.label);
                y += LINE_HEIGHT;
                break;
            }
            case WidgetType::Text: {
                draw::debug_text(MARGIN + PAD, y, draw::WHITE, widget.text.label);
                y += LINE_HEIGHT;
                break;
            }
            case WidgetType::ColoredText: {
                draw::debug_text(MARGIN + PAD, y, widget.colored_text.color,
                                 widget.colored_text.label);
                y += LINE_HEIGHT;
                break;
            }
            case WidgetType::Checkbox: {
                draw::debug_text(MARGIN + PAD, y,
                                 menu->selected_idx == selectable_idx ? lerped_color : unfocused,
                                 "  %s", widget.checkbox.label);
                draw::debug_text(MARGIN + PAD, y,
                                 menu->selected_idx == selectable_idx ? lerped_color : unfocused,
                                 "                        %s",
                                 widget.checkbox.get() ? "On" : "Off");

                if (menu->selected_idx == selectable_idx) cursor_y = y;
                y += LINE_HEIGHT;
                selectable_idx++;
                break;
            }
            case WidgetType::Separator: {
                y += LINE_HEIGHT / 2;
                break;
            }
            case WidgetType::Menu: {
                draw::debug_text(MARGIN + PAD, y,
                                 menu->selected_idx == selectable_idx ? lerped_color : unfocused,
                                 "  %s", widget.menu.label);

                // Draw "..." with dots closer together
                for (s32 i = 0; i < 3; i++) {
                    draw::debug_text(
                        MARGIN + PAD + 24 * draw::DEBUG_CHAR_WIDTH + i * 6, y,
                        menu->selected_idx == selectable_idx ? lerped_color : unfocused, ".");
                }

                if (menu->selected_idx == selectable_idx) cursor_y = y;
                selectable_idx++;
                y += LINE_HEIGHT;
                break;
            }
            case WidgetType::FloatView: {
                draw::debug_text(MARGIN + PAD, y, draw::WHITE, "%s", widget.checkbox.label);
                draw::debug_text(MARGIN + PAD, y, draw::GREEN, "                        %.3Ef",
                                 widget.float_view.get());
                y += LINE_HEIGHT;
                break;
            }
            case WidgetType::Choose: {
                draw::debug_text(MARGIN + PAD, y,
                                 menu->selected_idx == selectable_idx ? lerped_color : unfocused,
                                 "  %s", widget.choose.label);
                draw::debug_text(MARGIN + PAD, y,
                                 menu->selected_idx == selectable_idx ? lerped_color : unfocused,
                                 "                        (%d/%d) %s", widget.choose.get() + 1,
                                 widget.choose.num_choices,
                                 widget.choose.choices[widget.choose.get()]);

                if (menu->selected_idx == selectable_idx) cursor_y = y;
                y += LINE_HEIGHT;
                selectable_idx++;
                break;
            }
            case WidgetType::Button: {
                draw::debug_text(MARGIN + PAD, y,
                                 menu->selected_idx == selectable_idx ? lerped_color : unfocused,
                                 "  %s", widget.button.label);

                if (menu->selected_idx == selectable_idx) cursor_y = y;
                y += LINE_HEIGHT;
                selectable_idx++;
                break;
            }
            case WidgetType::Custom: {
                widget.custom.draw();
                break;
            }
        }
    }

    // Draw selection arrow
    if (cursor_y != -1) draw::debug_text(MARGIN + PAD + 2, cursor_y, focused, "\x1c");
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
    if (!s_visible) return;

    draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN,
               {0x00, 0x00, 0x00, 0xe0});
    draw_breadcrumbs();
    draw_menu_widget(s_menu_stack[s_menu_stack_ptr]);
}

}  // namespace menu_impl
