use core::ffi::c_char;

use arrayvec::{ArrayString, ArrayVec};

use crate::notify;
use crate::systems::draw;
use crate::utils::tinymap::TinyMapBuilder;
use crate::{mkb, utils::tinymap::TinyMap};

use super::binds::{self, Binds};
use super::menu_defn::{self, AfterPush, MenuContext, Widget};
use super::pad::{self, Dir, Prio};
use super::pref::U8Pref;

#[derive(Clone, Copy, PartialEq, Eq, Default)]
enum BindingState {
    #[default]
    Inactive, // not currently binding
    Requested, // will bind as soon as all buttons are released
    Active,    // currently binding
}

const SCREEN_WIDTH: i32 = 640;
const SCREEN_HEIGHT: i32 = 480;
const MARGIN: i32 = 20;
const PAD: i32 = 8;
const LINE_HEIGHT: i32 = 20;

const L_R_BIND: u8 = 64; // bind id for an L+R bind

static FOCUSED_COLOR: mkb::GXColor = draw::LIGHT_GREEN;
static UNFOCUSED_COLOR: mkb::GXColor = draw::LIGHT_PURPLE;

// Contents of a Widget::Menu
#[derive(Clone, Copy)]
struct Menu {
    label: &'static str,
    widgets: &'static [Widget],
    ptr: *const Widget,
}

fn insert_menu_widgets<const N: usize>(
    widgets: &'static [Widget],
    builder: &mut TinyMapBuilder<*const Widget, u32, N>,
) {
    for widget in widgets {
        if let Widget::Menu {
            widgets: sub_widgets,
            ..
        } = widget
        {
            builder.insert(widget as *const _, 0);
            insert_menu_widgets(sub_widgets, builder);
        }
    }
}

fn get_menu_widget_sel_map<const N: usize>() -> TinyMap<*const Widget, u32, N> {
    let mut builder: TinyMapBuilder<*const Widget, u32, N> = TinyMapBuilder::new();
    if let Widget::Menu { widgets, .. } = menu_defn::ROOT_MENU {
        insert_menu_widgets(widgets, &mut builder);
    }
    builder.build()
}

struct MenuImpl {
    visible: bool,
    cursor_frame: u32,
    menu_stack: ArrayVec<Menu, 5>,
    intedit_tick: i32,
    edit_tick: i32,
    binding: BindingState,
    menu_pos_map: TinyMap<*const Widget, u32, 32>,
    binds: Binds,
}

impl MenuImpl {
    fn new() -> Self {
        Self {
            visible: false,
            cursor_frame: 0,
            menu_stack: ArrayVec::new(),
            intedit_tick: 0,
            edit_tick: 0,
            binding: BindingState::default(),
            menu_pos_map: get_menu_widget_sel_map(),
            binds: Binds::new(),
        }
    }

    fn push_menu(&mut self, menu: Menu) {
        self.menu_stack.push(menu);
        self.cursor_frame = 0;
        // TODO
        // pad::reset_dir_repeat();
    }

    fn pop_menu(&mut self) {
        if self.menu_stack.is_empty() {
            self.visible = false;
        } else {
            self.menu_stack.pop();
        }
        self.cursor_frame = 0;

        // TODO
        // pad::reset_dir_repeat();
    }

    fn handle_widget_bind(&mut self, cx: &mut MenuContext) {
        let menu = self.menu_stack.last().unwrap();
        let selected_idx = *self.menu_pos_map.get(menu.ptr);
        let selected_widget = get_selected_widget(menu.widgets, selected_idx, cx);
        let selected_widget = match selected_widget {
            Some(widget) => widget,
            None => return,
        };

        let a_pressed = cx
            .pad
            .button_pressed(mkb::PAD_BUTTON_A as mkb::PadDigitalInput, Prio::High);
        let x_pressed = cx
            .pad
            .button_pressed(mkb::PAD_BUTTON_X as mkb::PadDigitalInput, Prio::High);
        let y_pressed = cx
            .pad
            .button_pressed(mkb::PAD_BUTTON_Y as mkb::PadDigitalInput, Prio::High);
        let a_repeat = cx
            .pad
            .button_repeat(mkb::PAD_BUTTON_A as mkb::PadDigitalInput, Prio::High);
        let y_repeat = cx
            .pad
            .button_repeat(mkb::PAD_BUTTON_Y as mkb::PadDigitalInput, Prio::High);

        // slow down scroll
        if self.edit_tick > 0 {
            self.edit_tick -= 1;
        } else if self.edit_tick < 0 {
            self.edit_tick += 1;
        }
        if self.intedit_tick > 0 {
            self.intedit_tick -= 1;
        }

        match selected_widget {
            Widget::Checkbox { pref, .. } => {
                if a_pressed || y_pressed {
                    cx.pref.set_bool(*pref, !cx.pref.get_bool(*pref));
                    cx.pref.save();
                }
                if x_pressed {
                    cx.pref.set_bool(*pref, cx.pref.get_default_bool(*pref));
                    cx.pref.save();
                }
            }
            Widget::GetSetCheckbox { get, set, .. } => {
                if a_pressed || y_pressed {
                    let curr = get(cx);
                    set(cx, !curr);
                }
            }
            Widget::Menu { label, widgets } => {
                if a_pressed {
                    self.push_menu(Menu {
                        label,
                        widgets,
                        ptr: selected_widget as *const _,
                    });
                }
            }
            Widget::Choose { pref, choices, .. } => {
                if a_pressed {
                    let new_value = (cx.pref.get_u8(*pref) as u32 + 1) % choices.len() as u32;
                    cx.pref.set_u8(*pref, new_value as u8);
                    cx.pref.save();
                }
                if y_pressed {
                    let new_value = (cx.pref.get_u8(*pref) as u32 + choices.len() as u32 - 1)
                        % choices.len() as u32;
                    cx.pref.set_u8(*pref, new_value as u8);
                    cx.pref.save();
                }
                if x_pressed {
                    cx.pref.set_u8(*pref, cx.pref.get_default_u8(*pref));
                    cx.pref.save();
                }
            }
            Widget::Button { push, after, .. } => {
                if a_pressed {
                    push(cx);
                    match after {
                        AfterPush::CloseMenu => self.visible = false,
                        AfterPush::GoBack => self.pop_menu(),
                        AfterPush::None => {}
                    }
                }
            }
            Widget::IntEdit { pref, min, max, .. } | Widget::FloatEdit { pref, min, max, .. } => {
                let mut next = cx.pref.get_u8(*pref) as i32;

                if cx
                    .pad
                    .button_released(mkb::PAD_BUTTON_A as mkb::PadDigitalInput, Prio::High)
                    && self.edit_tick > 0
                {
                    self.edit_tick = 0;
                } else if cx
                    .pad
                    .button_released(mkb::PAD_BUTTON_Y as mkb::PadDigitalInput, Prio::High)
                    && self.edit_tick < 0
                {
                    self.edit_tick = 0;
                }

                if x_pressed {
                    next = cx.pref.get_default_u8(*pref) as i32;
                } else if a_repeat
                    && !cx
                        .pad
                        .button_down(mkb::PAD_BUTTON_Y as mkb::PadDigitalInput, Prio::High)
                {
                    self.edit_tick += 5;
                    next += self.edit_tick / 5;
                } else if y_repeat
                    && !cx
                        .pad
                        .button_down(mkb::PAD_BUTTON_A as mkb::PadDigitalInput, Prio::High)
                {
                    self.edit_tick -= 5;
                    next += self.edit_tick / 5;
                }
                next = next.clamp(*min as i32, *max as i32);
                if next != cx.pref.get_u8(*pref) as i32 {
                    cx.pref.set_u8(*pref, next as u8);
                    cx.pref.save();
                }
            }
            Widget::InputSelect {
                pref,
                required_chord,
                can_unbind,
                ..
            } => {
                if self.binding == BindingState::Requested
                    && cx
                        .pad
                        .button_released(mkb::PAD_BUTTON_A as mkb::PadDigitalInput, Prio::High)
                {
                    self.binding = BindingState::Active;
                } else if self.binding == BindingState::Active {
                    // set new bind
                    let encoding_type = self.binds.get_encoding_type();
                    if encoding_type != binds::EncodingType::Invalid
                        && !(encoding_type == binds::EncodingType::SinglePress && *required_chord)
                    {
                        let value = self.binds.get_current_encoding();
                        cx.pref.set_u8(*pref, value);
                        cx.pref.save();
                    }
                    self.binding = BindingState::Inactive;
                } else if a_pressed {
                    // enter rebind mode
                    self.binding = BindingState::Requested;
                } else if y_pressed {
                    // unbind
                    if *can_unbind {
                        cx.pref.set_u8(*pref, 255);
                        cx.pref.save();
                    }
                } else if x_pressed {
                    // reset default bind
                    cx.pref.set_u8(*pref, cx.pref.get_default_u8(*pref));
                    cx.pref.save();
                }
            }
            _ => {}
        }
    }

    fn tick(&mut self, cx: &mut MenuContext) {
        if self.binding == BindingState::Active {
            self.handle_widget_bind(cx);
            return;
        }

        // TODO save settings on close
        // TODO save menu position as settings
        let toggle = self
            .binds
            .bind_pressed(cx.pref.get_u8(U8Pref::MenuBind), true);
        if toggle {
            self.visible ^= toggle;
        } else if cx
            .pad
            .button_pressed(mkb::PAD_BUTTON_B as mkb::PadDigitalInput, Prio::High)
        {
            self.pop_menu();
        }
        let just_opened = self.visible && toggle;
        if just_opened {
            // TODO: pad::reset_dir_repeat();
            self.cursor_frame = 0;
        }

        cx.pad
            .set_priority(if self.visible { Prio::High } else { Prio::Low });

        if !self.visible {
            // Default binding is L+R, but this lets you know the current binding in case you forget
            // what you changed it to
            let input = cx.pref.get_u8(U8Pref::MenuBind);
            if cx.pad.button_chord_pressed(
                mkb::PAD_TRIGGER_L as mkb::PadDigitalInput,
                mkb::PAD_TRIGGER_R as mkb::PadDigitalInput,
                Prio::High,
            ) && input != L_R_BIND
            {
                let mut buf = ArrayString::<32>::new();
                self.binds.get_bind_str(input, &mut buf);
                buf.push('\0');
                notify!(
                    cx.draw,
                    draw::RED,
                    c"Use %s to toggle menu",
                    buf.as_ptr() as *mut c_char,
                );
            }
            return;
        }

        let menu = self.menu_stack.last_mut().unwrap();

        // Update selected menu item
        let down_repeat = cx.pad.dir_repeat(Dir::Down, Prio::High);
        let up_repeat = cx.pad.dir_repeat(Dir::Up, Prio::High);
        let dir_delta: i32 = if down_repeat { -1 } else { 0 } + if up_repeat { 1 } else { 0 };

        let selectable = get_selectable_widget_count(menu.widgets, cx);
        let selected_idx = self.menu_pos_map.get_mut(menu.ptr);
        *selected_idx = (*selected_idx as i32 + dir_delta + selectable as i32) as u32 % selectable;

        // Make selected menu item green if selection changed or menu opened
        if dir_delta != 0 || just_opened {
            self.cursor_frame = 0;
        } else {
            self.cursor_frame += 1;
        }

        self.handle_widget_bind(cx);
    }
}

fn is_widget_selectable(widget: &'static Widget) -> bool {
    matches!(
        widget,
        Widget::Checkbox { .. }
            | Widget::GetSetCheckbox { .. }
            | Widget::Menu { .. }
            | Widget::Choose { .. }
            | Widget::Button { .. }
            | Widget::IntEdit { .. }
            | Widget::FloatEdit { .. }
            | Widget::InputSelect { .. }
    )
}

fn get_selected_widget_internal(
    widgets: &'static [Widget],
    curr_idx: i32,
    target_idx: u32,
    cx: &mut MenuContext,
) -> Result<&'static Widget, i32> {
    let mut new_idx = curr_idx;

    for widget in widgets {
        if is_widget_selectable(widget) {
            new_idx += 1;
            if new_idx == target_idx as i32 {
                return Ok(widget);
            }
        } else if let Widget::HideableGroup {
            widgets: hideable_widgets,
            show_if,
        } = widget
        {
            if show_if(cx) {
                match get_selected_widget_internal(hideable_widgets, new_idx, target_idx, cx) {
                    Ok(widget) => return Ok(widget),
                    Err(idx) => new_idx = idx,
                }
            }
        }
    }
    Err(new_idx)
}

fn get_selected_widget(
    widgets: &'static [Widget],
    target_idx: u32,
    cx: &mut MenuContext,
) -> Option<&'static Widget> {
    get_selected_widget_internal(widgets, -1, target_idx, cx).ok()
}

fn get_selectable_widget_count(widgets: &'static [Widget], cx: &mut MenuContext) -> u32 {
    let mut selectable_count = 0;

    for widget in widgets {
        if is_widget_selectable(widget) {
            selectable_count += 1;
        } else if let Widget::HideableGroup {
            widgets: hideable_widgets,
            show_if,
        } = widget
        {
            if show_if(cx) {
                selectable_count += get_selectable_widget_count(hideable_widgets, cx);
            }
        }
    }
    selectable_count
}

fn lerp_colors(color1: mkb::GXColor, color2: mkb::GXColor, t: f32) -> mkb::GXColor {
    let r = (1.0 - t) * color1.r as f32 + t * color2.r as f32;
    let g = (1.0 - t) * color1.g as f32 + t * color2.g as f32;
    let b = (1.0 - t) * color1.b as f32 + t * color2.b as f32;
    let a = (1.0 - t) * color1.a as f32 + t * color2.a as f32;

    mkb::GXColor {
        r: r.clamp(0.0, 255.0) as u8,
        g: g.clamp(0.0, 255.0) as u8,
        b: b.clamp(0.0, 255.0) as u8,
        a: a.clamp(0.0, 255.0) as u8,
    }
}

// static f32 sin_lerp(s32 period_frames) {
//     f32 angle = (static_cast<s32>(s_cursor_frame % period_frames) - (period_frames / 2.f)) *
//                 0x8000 / (period_frames / 2.f);
//     f32 lerp = (mkb::math_sin(angle) + 1.f) / 2.f;
//     return lerp;
// }

// static void draw_selectable_highlight(float y) {
//     // float new_y = y * 1.072 - 3; // Do NOT ask why we need this
//     // draw::rect(MARGIN, new_y, SCREEN_WIDTH - MARGIN, (new_y + LINE_HEIGHT), {0, 0, 0, 0xFF});

//     // Draw selection arrow
//     draw::debug_text(MARGIN + PAD + 2, y, FOCUSED_COLOR, "\x1c");
// }

// static constexpr s32 BLOCK_WIDTH = 150;
// static constexpr s32 START = MARGIN + 35;
// static constexpr s32 BUTTON_START = -83;
// static constexpr s32 Y_HEIGHT = SCREEN_HEIGHT - MARGIN - 52;
// static constexpr s32 HALF_SPACE = 12;

// static void draw_help_layout() {
//     // draw seperator
//     draw::rect(MARGIN, SCREEN_HEIGHT - MARGIN - 34, SCREEN_WIDTH - MARGIN,
//                SCREEN_HEIGHT - MARGIN - 30, draw::GRAY);
//     // draw b: back
//     draw::debug_text(START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::LIGHT_RED, "B");
//     draw::debug_text(BUTTON_START + 4 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//     draw::debug_text(BUTTON_START + 4 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE, "Back");
// }

// static void draw_help(const Widget& widget) {
//     // draw relevant controls for current widget
//     switch (widget.type) {
//         case WidgetType::Checkbox:
//         case WidgetType::GetSetCheckbox: {
//             draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Toggle");
//             break;
//         }
//         case WidgetType::Menu: {
//             draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Open");
//             break;
//         }
//         case WidgetType::Button: {
//             draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Activate");
//             break;
//         }
//         case WidgetType::Choose: {
//             draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Next");
//             draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
//             draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Previous");
//             draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
//             draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Reset");
//             break;
//         }
//         case WidgetType::IntEdit:
//         case WidgetType::FloatEdit: {
//             draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Increase");
//             draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
//             draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Decrease");
//             draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
//             draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Reset");
//             break;
//         }
//         case WidgetType::InputSelect: {
//             draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 1 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Bind");
//             if (widget.input_select.can_unbind) {
//                 draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
//                 draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//                 draw::debug_text(BUTTON_START + 2 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                                  "Unbind");
//             }
//             draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
//             draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
//             draw::debug_text(BUTTON_START + 3 * BLOCK_WIDTH + HALF_SPACE, Y_HEIGHT, draw::WHITE,
//                              "Reset");
//             break;
//         }
//         default: {
//             break;
//         }
//     }
// }

// void draw_widget(Widget& widget, u32 selected_idx, u32* selectable_idx, u32* y,
//                  mkb::GXColor lerped_color) {
//     switch (widget.type) {
//         case WidgetType::HideableGroupWidget: {
//             if (widget.hideable_group.show_if()) {
//                 for (u32 i = 0; i < widget.hideable_group.num_widgets; i++) {
//                     Widget& w = widget.hideable_group.widgets[i];
//                     draw_widget(w, selected_idx, selectable_idx, y, lerped_color);
//                 }
//             }
//             break;
//         }
//         case WidgetType::Header: {
//             draw::debug_text(MARGIN + PAD, *y, draw::ORANGE, widget.header.label);
//             *y += LINE_HEIGHT;
//             break;
//         }
//         case WidgetType::Text: {
//             draw::debug_text(MARGIN + PAD, *y, draw::WHITE, widget.text.label);
//             *y += LINE_HEIGHT;
//             break;
//         }
//         case WidgetType::ColoredText: {
//             draw::debug_text(MARGIN + PAD, *y, widget.colored_text.color,
//                              widget.colored_text.label);
//             *y += LINE_HEIGHT;
//             break;
//         }
//         case WidgetType::Checkbox:
//         case WidgetType::GetSetCheckbox: {
//             const char* label = nullptr;
//             bool value = false;
//             if (widget.type == WidgetType::Checkbox) {
//                 label = widget.checkbox.label;
//                 value = pref::get(widget.checkbox.pref);
//             } else {
//                 label = widget.get_set_checkbox.label;
//                 value = widget.get_set_checkbox.get();
//             }

//             if (selected_idx == *selectable_idx) {
//                 draw_selectable_highlight(*y);
//             }
//             draw::debug_text(MARGIN + PAD, *y,
//                              selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                              "  %s", label);
//             draw::debug_text(MARGIN + PAD, *y,
//                              selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                              "                         %s", value ? "On" : "Off");

//             *y += LINE_HEIGHT;
//             (*selectable_idx)++;
//             break;
//         }
//         case WidgetType::Separator: {
//             *y += LINE_HEIGHT / 2;
//             break;
//         }
//         case WidgetType::Menu: {
//             if (selected_idx == *selectable_idx) {
//                 draw_selectable_highlight(*y);
//             }
//             draw::debug_text(MARGIN + PAD, *y,
//                              selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                              "  %s", widget.menu.label);

//             // Draw "..." with dots closer together
//             for (s32 i = 0; i < 3; i++) {
//                 draw::debug_text(MARGIN + PAD + 25 * draw::DEBUG_CHAR_WIDTH + i * 6, *y,
//                                  selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                                  ".");
//             }

//             (*selectable_idx)++;
//             *y += LINE_HEIGHT;
//             break;
//         }
//         case WidgetType::FloatView: {
//             draw::debug_text(MARGIN + PAD, *y, draw::WHITE, "%s", widget.float_view.label);
//             draw::debug_text(MARGIN + PAD, *y, draw::GREEN, "                         %.3Ef",
//                              widget.float_view.get());
//             y += LINE_HEIGHT;
//             break;
//         }
//         case WidgetType::Choose: {
//             if (selected_idx == *selectable_idx) {
//                 draw_selectable_highlight(*y);
//             }
//             draw::debug_text(MARGIN + PAD, *y,
//                              selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                              "  %s", widget.choose.label);
//             draw::debug_text(
//                 MARGIN + PAD, *y, selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                 "                         (%d/%d) %s", pref::get(widget.choose.pref) + 1,
//                 widget.choose.num_choices, widget.choose.choices[pref::get(widget.choose.pref)]);

//             *y += LINE_HEIGHT;
//             (*selectable_idx)++;
//             break;
//         }
//         case WidgetType::Button: {
//             if (selected_idx == *selectable_idx) {
//                 draw_selectable_highlight(*y);
//             }
//             draw::debug_text(MARGIN + PAD, *y,
//                              selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                              "  %s", widget.button.label);

//             *y += LINE_HEIGHT;
//             (*selectable_idx)++;
//             break;
//         }
//         case WidgetType::IntEdit: {
//             if (selected_idx == *selectable_idx) {
//                 draw_selectable_highlight(*y);
//             }
//             draw::debug_text(MARGIN + PAD, *y,
//                              selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                              "  %s", widget.int_edit.label);
//             draw::debug_text(MARGIN + PAD, *y,
//                              selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                              "                         %d", pref::get(widget.int_edit.pref));

//             *y += LINE_HEIGHT;
//             (*selectable_idx)++;
//             break;
//         }
//         case WidgetType::FloatEdit: {
//             if (selected_idx == *selectable_idx) {
//                 draw_selectable_highlight(*y);
//             }

//             float display = ((float)(pref::get(widget.float_edit.pref) + widget.float_edit.floor) /
//                              (float)widget.float_edit.precision);

//             draw::debug_text(MARGIN + PAD, *y,
//                              selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                              "  %s", widget.float_edit.label);
//             switch (widget.float_edit.decimals) {
//                 case 2: {
//                     draw ::debug_text(
//                         MARGIN + PAD, *y,
//                         selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                         "                         %0.2f", display);
//                     break;
//                 }
//                 default: {
//                     draw ::debug_text(
//                         MARGIN + PAD, *y,
//                         selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                         "                         %0.3f", display);
//                     break;
//                 }
//             }

//             *y += LINE_HEIGHT;
//             (*selectable_idx)++;
//             break;
//         }
//         case WidgetType::InputSelect: {
//             if (selected_idx == *selectable_idx) {
//                 draw_selectable_highlight(*y);
//             }
//             if (s_binding == BindingState::Active && selected_idx == *selectable_idx) {
//                 draw::debug_text(MARGIN + PAD, *y, FOCUSED_COLOR, "  %s",
//                                  widget.input_select.label);
//             } else {
//                 draw::debug_text(MARGIN + PAD, *y,
//                                  selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR,
//                                  "  %s", widget.input_select.label);
//             }
//             mkb::GXColor bind_color =
//                 selected_idx == *selectable_idx ? lerped_color : UNFOCUSED_COLOR;
//             if (s_binding == BindingState::Active && selected_idx == *selectable_idx) {
//                 bind_color = draw::GOLD;
//             }
//             u8 input = pref::get(widget.input_select.pref);
//             char buf[25];
//             binds::get_bind_str(input, buf);
//             draw::debug_text(MARGIN + PAD, *y, bind_color, "                         %s", buf);

//             *y += LINE_HEIGHT;
//             (*selectable_idx)++;
//             break;
//         }
//         case WidgetType::Custom: {
//             widget.custom.draw();
//             break;
//         }
//     }
// }

// void draw_menu_widgets(MenuWidget* menu) {
//     u32 y = MARGIN + PAD + 2.f * LINE_HEIGHT;
//     u32 selectable_idx = 0;

//     mkb::GXColor lerped_color = lerp_colors(FOCUSED_COLOR, UNFOCUSED_COLOR, sin_lerp(40));

//     for (u32 i = 0; i < menu->num_widgets; i++) {
//         Widget& widget = menu->widgets[i];
//         draw_widget(widget, menu->selected_idx, &selectable_idx, &y, lerped_color);
//     }
// }

// static void draw_breadcrumbs() {
//     const char* ARROW_STR = " \x1c ";

//     u32 x = MARGIN + PAD;
//     for (u32 i = 0; i <= s_menu_stack_ptr; i++) {
//         MenuWidget* menu = s_menu_stack[i];
//         mkb::GXColor grey = {0xE0, 0xE0, 0xE0, 0xFF};
//         draw::debug_text(x, MARGIN + PAD, i == s_menu_stack_ptr ? draw::PURPLE : grey, menu->label);
//         x += mkb::strlen(const_cast<char*>(menu->label)) * draw::DEBUG_CHAR_WIDTH;
//         if (i != s_menu_stack_ptr) {
//             draw::debug_text(x, MARGIN + PAD, draw::BLUE, ARROW_STR);
//             x += mkb::strlen(const_cast<char*>(ARROW_STR)) * draw::DEBUG_CHAR_WIDTH;
//         }
//     }

//     // Draw line under breadcrumbs. You can draw lines directly with GX but I couldn't get it
//     // working
//     draw::rect(MARGIN, MARGIN + 30, SCREEN_WIDTH - MARGIN, MARGIN + 34, draw::GRAY);
// }

// void disp() {
//     if (!s_visible) return;
//     MenuWidget* menu = s_menu_stack[s_menu_stack_ptr];
//     draw::rect(MARGIN, MARGIN, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT - MARGIN,
//                {0x00, 0x00, 0x00, 0xe0});
//     draw_breadcrumbs();
//     draw_menu_widgets(menu);
//     draw_help_layout();
//     s32 curr_idx = -1;
//     Widget* selected =
//         get_selected_widget(menu->widgets, menu->num_widgets, curr_idx, menu->selected_idx);
//     if (selected != nullptr) {
//         draw_help(*selected);
//     }
// }

// bool is_visible() { return s_visible; }
