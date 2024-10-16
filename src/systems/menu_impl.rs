use core::ffi::c_char;

use arrayvec::{ArrayString, ArrayVec};

use crate::systems::draw;
use crate::utils::tinymap::TinyMapBuilder;
use crate::{mkb, utils::tinymap::TinyMap};
use crate::{notify, sprintf};

use super::binds::{self, Binds};
use super::draw::Draw;
use super::menu_defn::{self, AfterPush, MenuContext, Widget, ROOT_MENU};
use super::pad::{Dir, Pad, Prio};
use super::pref::{Pref, U8Pref};

#[derive(Clone, Copy, PartialEq, Eq, Default)]
enum BindingState {
    #[default]
    Inactive, // not currently binding
    Requested, // will bind as soon as all buttons are released
    Active,    // currently binding
}

const SCREEN_WIDTH: u32 = 640;
const SCREEN_HEIGHT: u32 = 480;
const MARGIN: u32 = 20;
const PAD: u32 = 8;
const LINE_HEIGHT: u32 = 20;
const LABEL_OFFSET: u32 = 2;
const PREF_OFFSET: u32 = 25;

const L_R_BIND: u8 = 64; // bind id for an L+R bind

static FOCUSED_COLOR: mkb::GXColor = draw::LIGHT_GREEN;
static UNFOCUSED_COLOR: mkb::GXColor = draw::LIGHT_PURPLE;

// Contents of a Widget::Menu
#[derive(Clone, Copy)]
struct Menu {
    label: &'static str,
    widgets: &'static [Widget],
    ptr: usize, // Integer instead of pointer so that we can implement Sync
}

fn insert_menu_widgets<const N: usize>(
    widgets: &'static [Widget],
    builder: &mut TinyMapBuilder<usize, u32, N>,
) {
    for widget in widgets {
        if let Widget::Menu {
            widgets: sub_widgets,
            ..
        } = widget
        {
            builder.insert(widget as *const _ as usize, 0);
            insert_menu_widgets(sub_widgets, builder);
        }
    }
}

fn get_menu_widget_sel_map<const N: usize>() -> TinyMap<usize, u32, N> {
    let mut builder: TinyMapBuilder<usize, u32, N> = TinyMapBuilder::new();
    builder.insert(&ROOT_MENU as *const _ as usize, 0);
    if let Widget::Menu { widgets, .. } = menu_defn::ROOT_MENU {
        insert_menu_widgets(widgets, &mut builder);
    }
    builder.build()
}

pub struct MenuImpl {
    visible: bool,
    cursor_frame: u32,
    menu_stack: ArrayVec<Menu, 5>,
    intedit_tick: i32,
    edit_tick: i32,
    binding: BindingState,
    menu_pos_map: TinyMap<usize, u32, 64>,
    binds: Binds,
}

impl MenuImpl {
    pub fn new() -> Self {
        let mut menu_stack = ArrayVec::<Menu, 5>::new();
        match ROOT_MENU {
            Widget::Menu { label, widgets } => {
                menu_stack.push(Menu {
                    label,
                    widgets,
                    ptr: &ROOT_MENU as *const _ as usize,
                });
            }
            _ => {}
        }
        Self {
            visible: false,
            cursor_frame: 0,
            menu_stack,
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
                        ptr: selected_widget as *const _ as usize,
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

    pub fn tick(&mut self, pad: &mut Pad, pref: &mut Pref, draw: &mut Draw) {
        let cx = &mut MenuContext { pad, pref, draw };

        if self.binding == BindingState::Active {
            self.handle_widget_bind(cx);
            return;
        }

        // TODO save settings on close
        // TODO save menu position as settings
        // TODO load bind from settings
        // let toggle = self
        //     .binds
        //     .bind_pressed(cx.pref.get_u8(U8Pref::MenuBind), true);
        let toggle = cx
            .pad
            .button_pressed(mkb::PAD_BUTTON_X as mkb::PadDigitalInput, Prio::High);
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
                    buf.as_ptr() as *mut c_char
                );
            }
            return;
        }

        let menu = self.menu_stack.last_mut().unwrap();

        // Update selected menu item
        let down_repeat = cx.pad.dir_repeat(Dir::Down, Prio::High);
        let up_repeat = cx.pad.dir_repeat(Dir::Up, Prio::High);
        let dir_delta: i32 = if down_repeat { 1 } else { 0 } + if up_repeat { -1 } else { 0 };

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

    fn draw_widget(
        &self,
        widget: &Widget,
        selected_idx: u32,
        selectable_idx: &mut u32,
        y: &mut u32,
        lerped_color: mkb::GXColor,
        cx: &mut MenuContext,
    ) {
        match widget {
            Widget::HideableGroup { widgets, show_if } => {
                if show_if(cx) {
                    for w in *widgets {
                        self.draw_widget(w, selected_idx, selectable_idx, y, lerped_color, cx);
                    }
                }
            }
            Widget::Header { label } => {
                draw::debug_text(MARGIN + PAD, *y, draw::ORANGE, label);
                *y += LINE_HEIGHT;
            }
            Widget::Text { label } => {
                draw::debug_text(MARGIN + PAD, *y, draw::WHITE, label);
                *y += LINE_HEIGHT;
            }
            Widget::ColoredText { label, color } => {
                draw::debug_text(MARGIN + PAD, *y, *color, label);
                *y += LINE_HEIGHT;
            }
            Widget::Checkbox { label, pref } => {
                if selected_idx == *selectable_idx {
                    draw_selectable_highlight(*y as f32);
                }
                let color = if selected_idx == *selectable_idx {
                    lerped_color
                } else {
                    UNFOCUSED_COLOR
                };
                draw::debug_text(MARGIN + PAD + 2, *y, color, label);
                draw::debug_text(
                    MARGIN + PAD + PREF_OFFSET,
                    *y,
                    color,
                    if cx.pref.get_bool(*pref) { "On" } else { "Off" },
                );
                *y += LINE_HEIGHT;
                *selectable_idx += 1;
            }
            Widget::GetSetCheckbox { label, get, .. } => {
                if selected_idx == *selectable_idx {
                    draw_selectable_highlight(*y as f32);
                }
                let color = if selected_idx == *selectable_idx {
                    lerped_color
                } else {
                    UNFOCUSED_COLOR
                };
                draw::debug_text(MARGIN + PAD + LABEL_OFFSET, *y, color, label);
                draw::debug_text(
                    MARGIN + PAD + PREF_OFFSET,
                    *y,
                    color,
                    if get(cx) { "On" } else { "Off" },
                );
                *y += LINE_HEIGHT;
                *selectable_idx += 1;
            }
            Widget::Separator {} => {
                *y += LINE_HEIGHT / 2;
            }
            Widget::Menu { label, .. } => {
                if selected_idx == *selectable_idx {
                    draw_selectable_highlight(*y as f32);
                }
                let color = if selected_idx == *selectable_idx {
                    lerped_color
                } else {
                    UNFOCUSED_COLOR
                };
                draw::debug_text(MARGIN + PAD + LABEL_OFFSET, *y, color, label);

                for i in 0..3 {
                    draw::debug_text(
                        MARGIN + PAD + 25 * draw::DEBUG_CHAR_WIDTH + i * 6,
                        *y,
                        color,
                        ".",
                    );
                }

                *selectable_idx += 1;
                *y += LINE_HEIGHT;
            }
            Widget::FloatView { label, get } => {
                draw::debug_text(MARGIN + PAD, *y, draw::WHITE, label);
                let mut float_str = ArrayString::<16>::new();
                sprintf!(float_str, c"%.3Ef", get() as f64);
                draw::debug_text(MARGIN + PAD + PREF_OFFSET, *y, draw::GREEN, &float_str);
                *y += LINE_HEIGHT;
            }
            Widget::Choose {
                label,
                choices,
                pref,
            } => {
                if selected_idx == *selectable_idx {
                    draw_selectable_highlight(*y as f32);
                }
                let color = if selected_idx == *selectable_idx {
                    lerped_color
                } else {
                    UNFOCUSED_COLOR
                };
                draw::debug_text(MARGIN + PAD + LABEL_OFFSET, *y, color, label);
                let current_choice = cx.pref.get_u8(*pref) as usize;

                let mut choice_text = ArrayString::<16>::from(choices[current_choice]).unwrap();
                choice_text.push('\0');

                let mut buf = ArrayString::<16>::new();
                sprintf!(
                    buf,
                    c"(%d/%d) %s",
                    current_choice + 1,
                    choices.len(),
                    &choice_text
                );
                draw::debug_text(MARGIN + PAD + PREF_OFFSET, *y, color, &buf);
                *y += LINE_HEIGHT;
                *selectable_idx += 1;
            }
            Widget::Button { label, .. } => {
                if selected_idx == *selectable_idx {
                    draw_selectable_highlight(*y as f32);
                }
                let color = if selected_idx == *selectable_idx {
                    lerped_color
                } else {
                    UNFOCUSED_COLOR
                };
                draw::debug_text(MARGIN + PAD + LABEL_OFFSET, *y, color, label);
                *y += LINE_HEIGHT;
                *selectable_idx += 1;
            }
            Widget::IntEdit { label, pref, .. } => {
                if selected_idx == *selectable_idx {
                    draw_selectable_highlight(*y as f32);
                }
                let color = if selected_idx == *selectable_idx {
                    lerped_color
                } else {
                    UNFOCUSED_COLOR
                };
                draw::debug_text(MARGIN + PAD + LABEL_OFFSET, *y, color, label);
                let mut buf = ArrayString::<8>::new();
                sprintf!(buf, c"%d", cx.pref.get_u8(*pref) as u32);
                draw::debug_text(MARGIN + PAD + PREF_OFFSET, *y, color, &buf);
                *y += LINE_HEIGHT;
                *selectable_idx += 1;
            }
            Widget::FloatEdit {
                label,
                pref,
                precision,
                floor,
                decimals,
                ..
            } => {
                if selected_idx == *selectable_idx {
                    draw_selectable_highlight(*y as f32);
                }
                let color = if selected_idx == *selectable_idx {
                    lerped_color
                } else {
                    UNFOCUSED_COLOR
                };
                draw::debug_text(MARGIN + PAD + LABEL_OFFSET, *y, color, label);

                let display = (cx.pref.get_u8(*pref) as f32 + *floor as f32) / *precision as f32;

                let mut buf = ArrayString::<16>::new();
                match decimals {
                    2 => sprintf!(buf, c"%0.2f", display as f64),
                    _ => sprintf!(buf, c"%0.3f", display as f64),
                }
                draw::debug_text(MARGIN + PAD + PREF_OFFSET, *y, color, &buf);

                *y += LINE_HEIGHT;
                *selectable_idx += 1;
            }
            Widget::InputSelect { label, pref, .. } => {
                if selected_idx == *selectable_idx {
                    draw_selectable_highlight(*y as f32);
                }
                let color =
                    if selected_idx == *selectable_idx && self.binding == BindingState::Active {
                        FOCUSED_COLOR
                    } else if selected_idx == *selectable_idx {
                        lerped_color
                    } else {
                        UNFOCUSED_COLOR
                    };
                draw::debug_text(MARGIN + PAD + LABEL_OFFSET, *y, color, label);

                let bind_color =
                    if selected_idx == *selectable_idx && self.binding == BindingState::Active {
                        draw::GOLD
                    } else if selected_idx == *selectable_idx {
                        lerped_color
                    } else {
                        UNFOCUSED_COLOR
                    };

                let input = cx.pref.get_u8(*pref);
                let mut buf = ArrayString::<32>::new();
                self.binds.get_bind_str(input, &mut buf);
                draw::debug_text(MARGIN + PAD + PREF_OFFSET, *y, bind_color, &buf);

                *y += LINE_HEIGHT;
                *selectable_idx += 1;
            }
            Widget::Custom { draw: custom_draw } => {
                custom_draw(cx);
            }
        }
    }

    fn draw_menu_widgets(&self, menu: &Menu, cx: &mut MenuContext) {
        let mut y = MARGIN + PAD + 2 * LINE_HEIGHT;
        let mut selectable_idx = 0;

        let lerped_color = lerp_colors(FOCUSED_COLOR, UNFOCUSED_COLOR, sin_lerp(40));

        for widget in menu.widgets {
            self.draw_widget(
                widget,
                *self.menu_pos_map.get(menu.ptr),
                &mut selectable_idx,
                &mut y,
                lerped_color,
                cx,
            );
        }
    }

    fn draw_breadcrumbs(&self) {
        const ARROW_STR: &str = " \u{001c} ";

        let mut x = MARGIN + PAD;
        for (i, menu) in self.menu_stack.iter().enumerate() {
            let grey = mkb::GXColor {
                r: 0xE0,
                g: 0xE0,
                b: 0xE0,
                a: 0xFF,
            };
            draw::debug_text(
                x,
                MARGIN + PAD,
                if i == self.menu_stack.len() - 1 {
                    draw::PURPLE
                } else {
                    grey
                },
                menu.label,
            );
            x += menu.label.len() as u32 * draw::DEBUG_CHAR_WIDTH;
            if i != self.menu_stack.len() - 1 {
                draw::debug_text(x, MARGIN + PAD, draw::BLUE, ARROW_STR);
                x += ARROW_STR.len() as u32 * draw::DEBUG_CHAR_WIDTH;
            }
        }

        // Draw line under breadcrumbs. You can draw lines directly with GX but I couldn't get it
        // working
        draw::rect(
            MARGIN as f32,
            (MARGIN + 30) as f32,
            (SCREEN_WIDTH - MARGIN) as f32,
            (MARGIN + 34) as f32,
            draw::GRAY,
        );
    }

    pub fn disp(&self, pad: &mut Pad, pref: &mut Pref, draw: &mut Draw) {
        let cx = &mut MenuContext { pad, pref, draw };

        if !self.visible {
            return;
        }
        let menu = self.menu_stack.last().unwrap();
        draw::rect(
            MARGIN as f32,
            MARGIN as f32,
            (SCREEN_WIDTH - MARGIN) as f32,
            (SCREEN_HEIGHT - MARGIN) as f32,
            mkb::GXColor {
                r: 0x00,
                g: 0x00,
                b: 0x00,
                a: 0xe0,
            },
        );
        self.draw_breadcrumbs();
        self.draw_menu_widgets(menu, cx);
        draw_help_layout();
        let selected = get_selected_widget(menu.widgets, *self.menu_pos_map.get(menu.ptr), cx);
        if let Some(selected) = selected {
            draw_help(selected);
        }
    }

    fn is_visible(&self) -> bool {
        self.visible
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

fn sin_lerp(period_frames: u32) -> f32 {
    unsafe {
        let angle = ((mkb::frame_counter % period_frames) - (period_frames / 2)) as f32
            * 0x8000 as f32
            / (period_frames as f32 / 2.0);
        let lerp = (mkb::math_sin(angle as i16) as f32 + 1.0) / 2.0;
        lerp
    }
}

fn draw_selectable_highlight(y: f32) {
    // let new_y = y * 1.072 - 3.0; // Do NOT ask why we need this
    // draw::rect(MARGIN as f32, new_y, SCREEN_WIDTH as f32 - MARGIN as f32, (new_y + LINE_HEIGHT as f32), mkb::GXColor { r: 0, g: 0, b: 0, a: 0xFF });

    // Draw selection arrow
    draw::debug_text(MARGIN + PAD + 2, y as u32, FOCUSED_COLOR, "\x1c");
}

const BLOCK_WIDTH: u32 = 150;
const START: u32 = MARGIN + 35;
const BUTTON_OFFSET: u32 = 83;
const Y_HEIGHT: u32 = SCREEN_HEIGHT - MARGIN - 52;
const HALF_SPACE: u32 = 12;

fn draw_help_layout() {
    // draw seperator
    draw::rect(
        MARGIN as f32,
        (SCREEN_HEIGHT - MARGIN - 34) as f32,
        (SCREEN_WIDTH - MARGIN) as f32,
        (SCREEN_HEIGHT - MARGIN - 30) as f32,
        draw::GRAY,
    );
    // draw b: back
    draw::debug_text(
        (START + 3 * BLOCK_WIDTH) as u32,
        Y_HEIGHT,
        draw::LIGHT_RED,
        "B",
    );
    draw::debug_text(4 * BLOCK_WIDTH - BUTTON_OFFSET, Y_HEIGHT, draw::WHITE, ":");
    draw::debug_text(
        4 * BLOCK_WIDTH + HALF_SPACE - BUTTON_OFFSET,
        Y_HEIGHT,
        draw::WHITE,
        "Back",
    );
}

fn draw_help(widget: &Widget) {
    // draw relevant controls for current widget
    match widget {
        Widget::Checkbox { .. } | Widget::GetSetCheckbox { .. } => {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_OFFSET + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 1 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Toggle",
            );
        }
        Widget::Menu { .. } => {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_OFFSET + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 1 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Open",
            );
        }
        Widget::Button { .. } => {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_OFFSET + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 1 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Activate",
            );
        }
        Widget::Choose { .. } => {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_OFFSET + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 1 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Next",
            );
            draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
            draw::debug_text(BUTTON_OFFSET + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 2 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Previous",
            );
            draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
            draw::debug_text(BUTTON_OFFSET + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 3 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Reset",
            );
        }
        Widget::IntEdit { .. } | Widget::FloatEdit { .. } => {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_OFFSET + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 1 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Increase",
            );
            draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
            draw::debug_text(BUTTON_OFFSET + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 2 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Decrease",
            );
            draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
            draw::debug_text(BUTTON_OFFSET + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 3 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Reset",
            );
        }
        Widget::InputSelect { can_unbind, .. } => {
            draw::debug_text(START, Y_HEIGHT, draw::LIGHT_GREEN, "A");
            draw::debug_text(BUTTON_OFFSET + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 1 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Bind",
            );
            if *can_unbind {
                draw::debug_text(START + 1 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "Y");
                draw::debug_text(BUTTON_OFFSET + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
                draw::debug_text(
                    BUTTON_OFFSET + 2 * BLOCK_WIDTH + HALF_SPACE,
                    Y_HEIGHT,
                    draw::WHITE,
                    "Unbind",
                );
            }
            draw::debug_text(START + 2 * BLOCK_WIDTH, Y_HEIGHT, draw::GRAY, "X");
            draw::debug_text(BUTTON_OFFSET + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::WHITE, ":");
            draw::debug_text(
                BUTTON_OFFSET + 3 * BLOCK_WIDTH + HALF_SPACE,
                Y_HEIGHT,
                draw::WHITE,
                "Reset",
            );
        }
        _ => {}
    }
}
