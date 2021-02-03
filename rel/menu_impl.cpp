#include "menu_impl.h"

#include <mkb/mkb.h>
#include <cstring>

#include "menu_defn.h"
#include "pad.h"
#include "draw.h"
#include "log.h"

constexpr f32 clamp_256(f32 x) { return x < 0 ? 0 : (x > 255 ? 255 : x); }

namespace menu
{

constexpr s32 SCREEN_WIDTH = 640;
constexpr s32 SCREEN_HEIGHT = 480;
constexpr s32 MARGIN = 20;
constexpr s32 PAD = 8;
constexpr s32 LINE_HEIGHT = 20;

static bool s_visible;
static u32 s_cursor_frame = 0;

constexpr u32 MENU_STACK_SIZE = 5;
static MenuWidget *s_menu_stack[MENU_STACK_SIZE] = {&root_menu};
static u32 s_cursor_stack[MENU_STACK_SIZE] = {0};
static u32 s_menu_stack_ptr = 0;

static void push_menu(MenuWidget *menu, u32 cursor_pos)
{
    MOD_ASSERT_MSG(s_menu_stack_ptr < MENU_STACK_SIZE - 1, "Menu stack overflow");
    s_menu_stack_ptr++;
    s_menu_stack[s_menu_stack_ptr] = menu;
    s_cursor_stack[s_menu_stack_ptr] = cursor_pos;
}

static void pop_menu()
{
    if (s_menu_stack_ptr == 0) s_visible = false;
    else s_menu_stack_ptr--;
}

static Widget *get_selected_widget()
{
    MenuWidget *menu = s_menu_stack[s_menu_stack_ptr];
    s32 sel = s_cursor_stack[s_menu_stack_ptr];

    s32 selectable = -1;
    for (u32 i = 0; i < menu->num_widgets; i++)
    {
        Widget *child = &menu->widgets[i];
        if (child->type == WidgetType::Checkbox || child->type == WidgetType::Menu)
        {
            selectable++;
            if (selectable == sel) return child;
        }
    }

    return nullptr;
}

static void handle_select_button()
{
    if (!pad::button_pressed(gc::PAD_BUTTON_A, true)) return;

    Widget *selected = get_selected_widget();
    if (selected == nullptr) return;

    if (selected->type == WidgetType::Checkbox)
    {
        selected->checkbox.set(!selected->checkbox.get());
    }
    else if (selected->type == WidgetType::Menu)
    {
        push_menu(&selected->menu, 0);
    }
}

void tick()
{
    bool toggle = false;
    if (pad::button_pressed(gc::PAD_BUTTON_B, true))
    {
        pop_menu();
    }
    else
    {
        toggle = pad::button_chord_pressed(gc::PAD_TRIGGER_L, gc::PAD_TRIGGER_R, true);
        s_visible ^= toggle;
    }
    bool just_opened = s_visible && toggle;

    pad::set_exclusive_mode(s_visible);

    if (!s_visible) return;

    MenuWidget *menu = s_menu_stack[s_menu_stack_ptr];
    u32 &cursor = s_cursor_stack[s_menu_stack_ptr];

    // Update selected menu item
    s32 dir_delta = pad::dir_pressed(pad::DIR_DOWN, true) - pad::dir_pressed(pad::DIR_UP, true);
    cursor = (cursor + dir_delta + menu->num_widgets) % menu->num_widgets;

    // Make selected menu item green if selection changed or menu opened
    if (dir_delta != 0 || just_opened) s_cursor_frame = 0;
    else s_cursor_frame++;

    handle_select_button();
}

static gc::GXColor lerp_colors(gc::GXColor color1, gc::GXColor color2, f32 t)
{
    f32 r = (1.f - t) * color1.r + t * color2.r;
    f32 g = (1.f - t) * color1.g + t * color2.g;
    f32 b = (1.f - t) * color1.b + t * color2.b;
    f32 a = (1.f - t) * color1.a + t * color2.a;

    gc::GXColor ret;
    ret.r = clamp_256(r);
    ret.g = clamp_256(g);
    ret.b = clamp_256(b);
    ret.a = clamp_256(a);

    return ret;
}

static f32 sin_lerp(s32 period_frames)
{
    f32 angle =
        (static_cast<s32>(s_cursor_frame % period_frames) - (period_frames / 2.f)) * 0x8000 / (period_frames / 2.f);
    f32 lerp = (mkb::math_sin(angle) + 1.f) / 2.f;
    return lerp;
}

void draw_menu_widget(MenuWidget *menu, u32 cursor_pos)
{
    u32 y = MARGIN + PAD + 2.f * LINE_HEIGHT;
    u32 selectable_idx = 0;
    s32 cursor_y = -1;

    gc::GXColor light_green = {0xad, 0xff, 0xa6, 0xff};
    gc::GXColor unfocused = {0xa2, 0xad, 0xff, 0xff};
    gc::GXColor lerped_color = lerp_colors(light_green, unfocused, sin_lerp(40));

    for (u32 i = 0; i < menu->num_widgets; i++)
    {
        Widget &widget = menu->widgets[i];

        switch (widget.type)
        {
            case WidgetType::Header:
            {
                draw::debug_text(MARGIN + PAD, y, draw::Color::Orange, widget.header.label);
                y += LINE_HEIGHT;
                break;
            }
            case WidgetType::Text:
            {
                draw::debug_text(MARGIN + PAD, y, draw::Color::White, widget.header.label);
                y += LINE_HEIGHT;
                break;
            }
            case WidgetType::Checkbox:
            {
                draw::debug_text(
                    MARGIN + PAD,
                    y,
                    cursor_pos == selectable_idx ? lerped_color : unfocused,
                    "  %s",
                    widget.checkbox.label);
                draw::debug_text(
                    MARGIN + PAD,
                    y,
                    cursor_pos == selectable_idx ? lerped_color : unfocused,
                    "                      %s",
                    widget.checkbox.get() ? "Enabled" : "Disabled");

                if (cursor_pos == selectable_idx) cursor_y = y;
                y += LINE_HEIGHT;
                selectable_idx++;
                break;
            }
            case WidgetType::Separator:
            {
                y += LINE_HEIGHT / 2;
                break;
            }
            case WidgetType::Menu:
            {
                draw::debug_text(
                    MARGIN + PAD,
                    y,
                    cursor_pos == selectable_idx ? lerped_color : unfocused,
                    "  %s",
                    widget.menu.label);

                if (cursor_pos == selectable_idx) cursor_y = y;
                selectable_idx++;
                y += LINE_HEIGHT;
                break;
            }
        }
    }

    // Draw selection arrow
    if (cursor_y != -1) draw::debug_text(MARGIN + PAD + 2, cursor_y, light_green, "\x1c");
}

static void draw_breadcrumbs()
{
    const char *ARROW_STR = " \x1c ";

    u32 x = MARGIN + PAD;
    for (u32 i = 0; i <= s_menu_stack_ptr; i++)
    {
        MenuWidget *menu = s_menu_stack[i];
        draw::debug_text(x, MARGIN + PAD, i == 0 ? draw::Color::Purple : draw::Color::White, menu->label);
        x += strlen(menu->label) * draw::DEBUG_CHAR_WIDTH;
        if (i != s_menu_stack_ptr)
        {
            draw::debug_text(x, MARGIN + PAD, draw::Color::White, ARROW_STR);
            x += strlen(ARROW_STR) * draw::DEBUG_CHAR_WIDTH;
        }
    }
}

void disp()
{
    if (!s_visible) return;

    draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN, {0x00, 0x00, 0x00, 0xd0});
    draw_breadcrumbs();
    draw_menu_widget(s_menu_stack[s_menu_stack_ptr], s_cursor_stack[s_menu_stack_ptr]);
}

}