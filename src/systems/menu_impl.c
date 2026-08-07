#include "menu_impl.h"
#include <stddef.h>

#include "systems/binds.h"
#include "systems/log.h"
#include "systems/menu_defn.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/base.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

typedef enum BindingState {
    BindingState_Inactive,   // not currently binding
    BindingState_Requested,  // will bind as soon as all buttons are released
    BindingState_Active,     // currently binding
} BindingState;
static BindingState s_binding = BindingState_Inactive;

static const s32 SCREEN_WIDTH = 640;
static const s32 SCREEN_HEIGHT = 480;
static const s32 MARGIN = 20;
static const s32 PAD = 8;
static const s32 LINE_HEIGHT = 20;

static const s32 L_R_BIND = 64;  // bind id for an L+R bind

static const GXColor FOCUSED_COLOR = COLOR_LIGHT_GREEN;
static const GXColor UNFOCUSED_COLOR = COLOR_LIGHT_PURPLE;

static bool s_visible;
static u32 s_cursor_frame = 0;

enum { MENU_STACK_SIZE = 5 };
static MenuWidget *s_menu_stack[MENU_STACK_SIZE] = {&menu_root};
static u32 s_menu_stack_ptr = 0;

static s32 s_intedit_tick = 0;
static s32 s_edit_tick = 0;

static void push_menu(MenuWidget *menu) {
    MOD_ASSERT(s_menu_stack_ptr < MENU_STACK_SIZE - 1);  // Menu stack overflow
    s_menu_stack_ptr++;
    s_menu_stack[s_menu_stack_ptr] = menu;
    s_cursor_frame = 0;
    pad_reset_dir_repeat();
}

static void pop_menu() {
    if (s_menu_stack_ptr == 0) {
        s_visible = false;
    } else {
        s_menu_stack_ptr--;
    }
    s_cursor_frame = 0;
    pad_reset_dir_repeat();
}

static bool is_widget_selectable(WidgetType type) {
    return type == WidgetType_Checkbox || type == WidgetType_GetSetCheckbox ||
           type == WidgetType_Menu || type == WidgetType_Choose || type == WidgetType_Button ||
           type == WidgetType_IntEdit || type == WidgetType_FloatEdit ||
           type == WidgetType_InputSelect;
}

static Widget *get_selected_widget(Widget *widgets,
                                   u32 num_widgets,
                                   s32 *curr_idx,
                                   s32 target_idx) {
    for (u32 i = 0; i < num_widgets; i++) {
        if (is_widget_selectable(widgets[i].type)) {
            (*curr_idx)++;
            if (*curr_idx == target_idx) return &widgets[i];
        } else if (widgets[i].type == WidgetType_HideableGroupWidget &&
                   widgets[i].hideable_group.show_if()) {
            Widget *possible_selection =
                get_selected_widget(widgets[i].hideable_group.widgets,
                                    widgets[i].hideable_group.num_widgets, curr_idx, target_idx);
            if (possible_selection != NULL) {
                return possible_selection;
            }
        }
    }
    return NULL;
}

static u32 get_selectable_widget_count(Widget *widgets, u32 num_widgets) {
    u32 selectable = 0;

    for (u32 i = 0; i < num_widgets; i++) {
        Widget *child = &widgets[i];
        if (is_widget_selectable(child->type)) {
            selectable++;
        } else if (child->type == WidgetType_HideableGroupWidget &&
                   child->hideable_group.show_if()) {
            selectable += get_selectable_widget_count(child->hideable_group.widgets,
                                                      child->hideable_group.num_widgets);
        }
    }
    return selectable;
}

static void handle_widget_bind() {
    MenuWidget *menu = s_menu_stack[s_menu_stack_ptr];
    s32 target_idx = menu->selected_idx;
    s32 curr_idx = -1;
    Widget *selected = get_selected_widget(menu->widgets, menu->num_widgets, &curr_idx, target_idx);
    if (selected == NULL) return;

    bool a_pressed = pad_button_pressed(mkb_PAD_BUTTON_A, true);
    bool x_pressed = pad_button_pressed(mkb_PAD_BUTTON_X, true);
    bool y_pressed = pad_button_pressed(mkb_PAD_BUTTON_Y, true);
    bool a_repeat = pad_button_repeat(mkb_PAD_BUTTON_A, true);
    bool y_repeat = pad_button_repeat(mkb_PAD_BUTTON_Y, true);

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
    case WidgetType_Checkbox: {
        CheckboxWidget *checkbox = &selected->checkbox;
        if (a_pressed || y_pressed) {
            pref_set(checkbox->pref, !pref_get(checkbox->pref));
            pref_save();
        }
        if (x_pressed) {
            pref_set(checkbox->pref, pref_get_default(checkbox->pref));
            pref_save();
        }
        break;
    }
    case WidgetType_GetSetCheckbox: {
        GetSetCheckboxWidget *get_set_checkbox = &selected->get_set_checkbox;
        if (a_pressed || y_pressed) {
            get_set_checkbox->set(!get_set_checkbox->get());
        }
        break;
    }
    case WidgetType_Menu: {
        if (a_pressed) {
            push_menu(&selected->menu);
        }
        break;
    }
    case WidgetType_Choose: {
        ChooseWidget *choose = &selected->choose;
        if (a_pressed) {
            u8 new_value = ((u32)(pref_get(choose->pref)) + 1) % choose->num_choices;
            pref_set(choose->pref, new_value);
            pref_save();
        }
        if (y_pressed) {
            u8 new_value =
                ((u32)(pref_get(choose->pref)) + choose->num_choices - 1) % choose->num_choices;
            pref_set(choose->pref, new_value);
            pref_save();
        }
        if (x_pressed) {
            pref_set(choose->pref, pref_get_default(choose->pref));
            pref_save();
        }
        break;
    }
    case WidgetType_Button: {
        if (a_pressed) {
            ButtonWidget *button = &selected->button;
            if (button->push != NULL) {
                selected->button.push();
            }
            if (button->flags & ButtonFlag_CloseMenu) {
                s_visible = false;
            }
            if (button->flags & ButtonFlag_GoBack) {
                pop_menu();
            }
        }
        break;
    }
    case WidgetType_IntEdit:
    case WidgetType_FloatEdit: {
        int next;
        Pref edit_pref;
        u8 min, max;
        if (selected->type == WidgetType_IntEdit) {
            IntEditWidget *int_edit = &selected->int_edit;
            next = pref_get(int_edit->pref);
            edit_pref = int_edit->pref;
            min = int_edit->min;
            max = int_edit->max;
        } else {
            FloatEditWidget *float_edit = &selected->float_edit;
            next = pref_get(float_edit->pref);
            edit_pref = float_edit->pref;
            min = float_edit->min;
            max = float_edit->max;
        }

        if (pad_button_released(mkb_PAD_BUTTON_A, true) && s_edit_tick > 0) {
            s_edit_tick = 0;
        } else if (pad_button_released(mkb_PAD_BUTTON_Y, true) && s_edit_tick < 0) {
            s_edit_tick = 0;
        }

        if (x_pressed) {
            next = pref_get_default(edit_pref);
        } else if (a_repeat && !pad_button_down(mkb_PAD_BUTTON_Y, true)) {
            s_edit_tick += 5;
            next += (s_edit_tick / 5);
        } else if (y_repeat && !pad_button_down(mkb_PAD_BUTTON_A, true)) {
            s_edit_tick -= 5;
            next += (s_edit_tick / 5);
        }
        next = CLAMP(next, min, max);
        if (next != pref_get(edit_pref)) {
            pref_set(edit_pref, next);
            pref_save();
        }
        break;
    }
    case WidgetType_InputSelect: {
        InputSelectWidget *input_select = &selected->input_select;
        if (s_binding == BindingState_Requested && pad_button_released(mkb_PAD_BUTTON_A, true)) {
            s_binding = BindingState_Active;
        } else if (s_binding == BindingState_Active) {
            // set new bind
            binds_EncodingType type = binds_get_encoding_type();
            if (type == binds_EncodingType_Invalid ||
                (type == binds_EncodingType_SinglePress && input_select->required_chord))
                return;
            u8 value = binds_get_current_encoding();
            pref_set(input_select->pref, value);
            pref_save();
            s_binding = BindingState_Inactive;
        } else if (a_pressed) {
            // enter rebind mode
            s_binding = BindingState_Requested;
        } else if (y_pressed) {
            // unbind
            if (!input_select->can_unbind) return;
            pref_set(input_select->pref, 255);
            pref_save();
        } else if (x_pressed) {
            // reset default bind
            pref_set(input_select->pref, pref_get_default(input_select->pref));
            pref_save();
        }
        break;
    }
    default: {
        break;
    }
    }
}

void menu_impl_tick() {
    if (s_binding == BindingState_Active) {
        handle_widget_bind();
        return;
    }

    // TODO save settings on close
    // TODO save menu position as settings
    bool toggle = binds_bind_pressed(pref_get(Pref_MenuBind), true);
    if (toggle) {
        s_visible ^= toggle;
    } else if (pad_button_pressed(mkb_PAD_BUTTON_B, true)) {
        pop_menu();
    }
    bool just_opened = s_visible && toggle;
    if (just_opened) {
        pad_reset_dir_repeat();
        s_cursor_frame = 0;
    }

    pad_set_exclusive_mode(s_visible);

    if (!s_visible) {
        // Default binding is L+R, but this lets you know the current binding in case you forget
        // what you changed it to
        u8 input = pref_get(Pref_MenuBind);
        if (pad_button_chord_pressed(mkb_PAD_TRIGGER_L, mkb_PAD_TRIGGER_R, true) &&
            input != L_R_BIND) {
            char buf[25];
            binds_get_bind_str(input, buf);
            draw_notify(COLOR_RED, "Use %s to toggle menu", buf);
        }
        return;
    }

    MenuWidget *menu = s_menu_stack[s_menu_stack_ptr];

    // Update selected menu item
    s32 dir_delta = pad_dir_repeat(PadDir_Down, true) - pad_dir_repeat(PadDir_Up, true);
    u32 selectable = get_selectable_widget_count(menu->widgets, menu->num_widgets);
    menu->selected_idx = (menu->selected_idx + dir_delta + selectable) % selectable;

    // Make selected menu item green if selection changed or menu opened
    if (dir_delta != 0 || just_opened)
        s_cursor_frame = 0;
    else
        s_cursor_frame++;

    handle_widget_bind();
}

static GXColor lerp_colors(GXColor color1, GXColor color2, f32 t) {
    f32 r = (1.f - t) * color1.r + t * color2.r;
    f32 g = (1.f - t) * color1.g + t * color2.g;
    f32 b = (1.f - t) * color1.b + t * color2.b;
    f32 a = (1.f - t) * color1.a + t * color2.a;

    GXColor ret;
    ret.r = CLAMP(r, 0, 255);
    ret.g = CLAMP(g, 0, 255);
    ret.b = CLAMP(b, 0, 255);
    ret.a = CLAMP(a, 0, 255);

    return ret;
}

static f32 sin_lerp(s32 period_frames) {
    f32 angle = ((s32)(s_cursor_frame % period_frames) - (period_frames / 2.f)) * 0x8000 /
                (period_frames / 2.f);
    f32 lerp = (mkb_math_sin(angle) + 1.f) / 2.f;
    return lerp;
}

static void draw_selectable_highlight(float y) {
    // float new_y = y * 1.072 - 3; // Do NOT ask why we need this
    // draw_rect(MARGIN, new_y, SCREEN_WIDTH - MARGIN, (new_y + LINE_HEIGHT), {0, 0, 0, 0xFF});

    // Draw selection arrow
    draw_debug_text(MARGIN + PAD + 2, y, FOCUSED_COLOR, "\x1c");
}

static const s32 BLOCK_WIDTH = 150;
static const s32 START = MARGIN + 35;
static const s32 BUTTON_START = -83;
static const s32 Y_HEIGHT = SCREEN_HEIGHT - MARGIN - 52;
static const s32 HALF_SPACE = 12;

static void draw_help_layout() {
    // draw seperator
    draw_rect(MARGIN, SCREEN_HEIGHT - MARGIN - 34, SCREEN_WIDTH - MARGIN,
              SCREEN_HEIGHT - MARGIN - 30, COLOR_GRAY);
    // draw b: back
    draw_debug_text(START + 3 * BLOCK_WIDTH, Y_HEIGHT, COLOR_LIGHT_RED, "B");
    draw_debug_text(BUTTON_START + 4 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
    draw_debug_text(BUTTON_START + 4 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE, "Back");
}

static void draw_help(const Widget *widget) {
    // draw relevant controls for current widget
    switch (widget->type) {
    case WidgetType_Checkbox:
    case WidgetType_GetSetCheckbox: {
        draw_debug_text(START, Y_HEIGHT, COLOR_LIGHT_GREEN, "A");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                        "Toggle");
        break;
    }
    case WidgetType_Menu: {
        draw_debug_text(START, Y_HEIGHT, COLOR_LIGHT_GREEN, "A");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE, "Open");
        break;
    }
    case WidgetType_Button: {
        draw_debug_text(START, Y_HEIGHT, COLOR_LIGHT_GREEN, "A");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                        "Activate");
        break;
    }
    case WidgetType_Choose: {
        draw_debug_text(START, Y_HEIGHT, COLOR_LIGHT_GREEN, "A");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE, "Next");
        draw_debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_GRAY, "Y");
        draw_debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                        "Previous");
        draw_debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, COLOR_GRAY, "X");
        draw_debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                        "Reset");
        break;
    }
    case WidgetType_IntEdit:
    case WidgetType_FloatEdit: {
        draw_debug_text(START, Y_HEIGHT, COLOR_LIGHT_GREEN, "A");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                        "Increase");
        draw_debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_GRAY, "Y");
        draw_debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                        "Decrease");
        draw_debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, COLOR_GRAY, "X");
        draw_debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                        "Reset");
        break;
    }
    case WidgetType_InputSelect: {
        draw_debug_text(START, Y_HEIGHT, COLOR_LIGHT_GREEN, "A");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE, "Bind");
        if (widget->input_select.can_unbind) {
            draw_debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, COLOR_GRAY, "Y");
            draw_debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
            draw_debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                            "Unbind");
        }
        draw_debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, COLOR_GRAY, "X");
        draw_debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, COLOR_WHITE, ":");
        draw_debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, COLOR_WHITE,
                        "Reset");
        break;
    }
    default: {
        break;
    }
    }
}

static void draw_widget(Widget *widget,
                        u32 selected_idx,
                        u32 *selectable_idx,
                        u32 *y,
                        GXColor lerped_color) {
    switch (widget->type) {
    case WidgetType_HideableGroupWidget: {
        if (widget->hideable_group.show_if()) {
            for (u32 i = 0; i < widget->hideable_group.num_widgets; i++) {
                Widget *w = &widget->hideable_group.widgets[i];
                draw_widget(w, selected_idx, selectable_idx, y, lerped_color);
            }
        }
        break;
    }
    case WidgetType_Header: {
        draw_debug_text(MARGIN + PAD, *y, COLOR_ORANGE, widget->header.label);
        *y += LINE_HEIGHT;
        break;
    }
    case WidgetType_Text: {
        draw_debug_text(MARGIN + PAD, *y, COLOR_WHITE, widget->text.label);
        *y += LINE_HEIGHT;
        break;
    }
    case WidgetType_ColoredText: {
        draw_debug_text(MARGIN + PAD, *y, widget->colored_text.color, widget->colored_text.label);
        *y += LINE_HEIGHT;
        break;
    }
    case WidgetType_Checkbox:
    case WidgetType_GetSetCheckbox: {
        const char *label = NULL;
        bool value = false;
        if (widget->type == WidgetType_Checkbox) {
            label = widget->checkbox.label;
            value = pref_get(widget->checkbox.pref);
        } else {
            label = widget->get_set_checkbox.label;
            value = widget->get_set_checkbox.get();
        }

        if (selected_idx == *selectable_idx) {
            draw_selectable_highlight(*y);
        }
        draw_debug_text(MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR, "  %s",
                        label);
        draw_debug_text(MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                        "                         %s", value ? "On" : "Off");

        *y += LINE_HEIGHT;
        (*selectable_idx)++;
        break;
    }
    case WidgetType_Separator: {
        *y += LINE_HEIGHT / 2;
        break;
    }
    case WidgetType_Menu: {
        if (selected_idx == *selectable_idx) {
            draw_selectable_highlight(*y);
        }
        draw_debug_text(MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR, "  %s",
                        widget->menu.label);

        // Draw "..." with dots closer together
        for (s32 i = 0; i < 3; i++) {
            draw_debug_text(MARGIN + PAD + 25 * DRAW_DEBUG_CHAR_WIDTH + i * 6, *y,
                            selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR, ".");
        }

        (*selectable_idx)++;
        *y += LINE_HEIGHT;
        break;
    }
    case WidgetType_FloatView: {
        draw_debug_text(MARGIN + PAD, *y, COLOR_WHITE, "%s", widget->float_view.label);
        draw_debug_text(MARGIN + PAD, *y, COLOR_GREEN, "                         %.3Ef",
                        widget->float_view.get());
        *y += LINE_HEIGHT;
        break;
    }
    case WidgetType_Choose: {
        if (selected_idx == *selectable_idx) {
            draw_selectable_highlight(*y);
        }
        draw_debug_text(MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR, "  %s",
                        widget->choose.label);
        draw_debug_text(
            MARGIN + PAD, *y, selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
            "                         (%d/%d) %s", pref_get(widget->choose.pref) + 1,
            widget->choose.num_choices, widget->choose.choices[pref_get(widget->choose.pref)]);

        *y += LINE_HEIGHT;
        (*selectable_idx)++;
        break;
    }
    case WidgetType_Button: {
        if (selected_idx == *selectable_idx) {
            draw_selectable_highlight(*y);
        }
        draw_debug_text(MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR, "  %s",
                        widget->button.label);

        *y += LINE_HEIGHT;
        (*selectable_idx)++;
        break;
    }
    case WidgetType_IntEdit: {
        if (selected_idx == *selectable_idx) {
            draw_selectable_highlight(*y);
        }
        draw_debug_text(MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR, "  %s",
                        widget->int_edit.label);
        draw_debug_text(MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                        "                         %d", pref_get(widget->int_edit.pref));

        *y += LINE_HEIGHT;
        (*selectable_idx)++;
        break;
    }
    case WidgetType_FloatEdit: {
        if (selected_idx == *selectable_idx) {
            draw_selectable_highlight(*y);
        }

        float display = ((float)(pref_get(widget->float_edit.pref) + widget->float_edit.floor) /
                         (float)widget->float_edit.precision);

        draw_debug_text(MARGIN + PAD, *y,
                        selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR, "  %s",
                        widget->float_edit.label);
        switch (widget->float_edit.decimals) {
        case 2: {
            draw_debug_text(MARGIN + PAD, *y,
                            selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                            "                         %0.2f", display);
            break;
        }
        default: {
            draw_debug_text(MARGIN + PAD, *y,
                            selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                            "                         %0.3f", display);
            break;
        }
        }

        *y += LINE_HEIGHT;
        (*selectable_idx)++;
        break;
    }
    case WidgetType_InputSelect: {
        if (selected_idx == *selectable_idx) {
            draw_selectable_highlight(*y);
        }
        if (s_binding == BindingState_Active && selected_idx == *selectable_idx) {
            draw_debug_text(MARGIN + PAD, *y, FOCUSED_COLOR, "  %s", widget->input_select.label);
        } else {
            draw_debug_text(MARGIN + PAD, *y,
                            selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
                            "  %s", widget->input_select.label);
        }
        GXColor bind_color = selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR;
        if (s_binding == BindingState_Active && selected_idx == *selectable_idx) {
            bind_color = COLOR_GOLD;
        }
        u8 input = pref_get(widget->input_select.pref);
        char buf[25];
        binds_get_bind_str(input, buf);
        draw_debug_text(MARGIN + PAD, *y, bind_color, "                         %s", buf);

        *y += LINE_HEIGHT;
        (*selectable_idx)++;
        break;
    }
    case WidgetType_Custom: {
        widget->custom.draw();
        break;
    }
    }
}

static void draw_menu_widgets(MenuWidget *menu) {
    u32 y = MARGIN + PAD + 2.f * LINE_HEIGHT;
    u32 selectable_idx = 0;

    GXColor lerped_color = lerp_colors(FOCUSED_COLOR, UNFOCUSED_COLOR, sin_lerp(40));

    for (u32 i = 0; i < menu->num_widgets; i++) {
        Widget *widget = &menu->widgets[i];
        draw_widget(widget, menu->selected_idx, &selectable_idx, &y, lerped_color);
    }
}

static void draw_breadcrumbs() {
    const char *ARROW_STR = " \x1c ";

    u32 x = MARGIN + PAD;
    for (u32 i = 0; i <= s_menu_stack_ptr; i++) {
        MenuWidget *menu = s_menu_stack[i];
        GXColor grey = {0xE0, 0xE0, 0xE0, 0xFF};
        draw_debug_text(x, MARGIN + PAD, i == s_menu_stack_ptr ? COLOR_PURPLE : grey, menu->label);
        x += mkb_strlen((char *)menu->label) * DRAW_DEBUG_CHAR_WIDTH;
        if (i != s_menu_stack_ptr) {
            draw_debug_text(x, MARGIN + PAD, COLOR_BLUE, ARROW_STR);
            x += mkb_strlen((char *)ARROW_STR) * DRAW_DEBUG_CHAR_WIDTH;
        }
    }

    // Draw line under breadcrumbs. You can draw lines directly with GX but I couldn't get it
    // working
    draw_rect(MARGIN, MARGIN + 30, SCREEN_WIDTH - MARGIN, MARGIN + 34, COLOR_GRAY);
}

void menu_impl_disp() {
    if (!s_visible) return;
    MenuWidget *menu = s_menu_stack[s_menu_stack_ptr];
    draw_rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN,
              (GXColor){0x00, 0x00, 0x00, 0xe0});
    draw_breadcrumbs();
    draw_menu_widgets(menu);
    draw_help_layout();
    s32 curr_idx = -1;
    Widget *selected =
        get_selected_widget(menu->widgets, menu->num_widgets, &curr_idx, menu->selected_idx);
    if (selected != NULL) {
        draw_help(selected);
    }
}

bool menu_impl_is_visible() {
    return s_visible;
}
