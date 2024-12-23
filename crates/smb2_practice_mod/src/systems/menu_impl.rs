use mkb::mkb;

use arrayvec::{ArrayString, ArrayVec};

use crate::systems::draw::{self, NotifyDuration};
use crate::utils::math::cbrt_approx;
use crate::utils::tinymap::TinyMap;
use crate::utils::tinymap::TinyMapBuilder;
use crate::{cstr, cstr_buf, fmt};

use super::binds::{self};
use super::draw::DEBUG_CHAR_WIDTH;
use super::menu_defn::{self, AfterPush, MenuContext, TextLine, Widget, ROOT_MENU};
use super::pad::{Button, Dir, Prio};
use super::pref::U8Pref;

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
const LABEL_OFFSET: u32 = 2 * draw::DEBUG_CHAR_WIDTH;
const PREF_OFFSET: u32 = 25 * draw::DEBUG_CHAR_WIDTH;

const L_R_BIND: u8 = 64; // bind id for an L+R bind

static FOCUSED_COLOR: mkb::GXColor = draw::LIGHT_GREEN;
static UNFOCUSED_COLOR: mkb::GXColor = draw::LIGHT_PURPLE;

// Do not ask why we need this...
const fn real_y(y: f32) -> f32 {
    y * 1.072 - 3.0
}

#[derive(Clone, Copy)]
struct Oklab {
    l: f32,
    a: f32,
    b: f32,
}

fn rgb_to_oklab(color: mkb::GXColor) -> Oklab {
    let r = color.r as f32 / 255.0;
    let g = color.g as f32 / 255.0;
    let b = color.b as f32 / 255.0;

    let l = 0.0514459929f32 * b + 0.4122214708f32 * r + 0.5363325363 * g;
    let m = 0.1073969566f32 * b + 0.2119034982f32 * r + 0.6806995451 * g;
    let s = 0.6299787005f32 * b + 0.0883024619f32 * r + 0.2817188376 * g;

    let l_ = cbrt_approx(l);
    let m_ = cbrt_approx(m);
    let s_ = cbrt_approx(s);

    Oklab {
        l: -0.0040720468f32 * s_ + 0.2104542553f32 * l_ + 0.7936177850 * m_,
        a: 0.4505937099f32 * s_ + 1.9779984951f32 * l_ - 2.4285922050 * m_,
        b: -0.8086757660f32 * s_ + 0.0259040371f32 * l_ + 0.7827717662 * m_,
    }
}

fn oklab_to_rgb(color: Oklab) -> mkb::GXColor {
    let l_ = 0.2158037573f32 * color.b + 0.3963377774f32 * color.a + color.l;
    let m_ = -0.0638541728f32 * color.b - 0.1055613458f32 * color.a + color.l;
    let s_ = -1.2914855480f32 * color.b - 0.0894841775f32 * color.a + color.l;

    let l = l_ * l_ * l_;
    let m = m_ * m_ * m_;
    let s = s_ * s_ * s_;

    let r = 0.2309699292f32 * s + 4.0767416621f32 * l - 3.3077115913 * m;
    let g = -0.3413193965f32 * s - 1.2684380046f32 * l + 2.6097574011 * m;
    let b = 1.7076147010f32 * s - 0.0041960863f32 * l - 0.7034186147 * m;

    mkb::GXColor {
        r: (r.clamp(0.0, 1.0) * 255.0) as u8,
        g: (g.clamp(0.0, 1.0) * 255.0) as u8,
        b: (b.clamp(0.0, 1.0) * 255.0) as u8,
        a: 255,
    }
}

fn lerp_oklab(t: f32, c1: mkb::GXColor, c2: mkb::GXColor) -> mkb::GXColor {
    let c1_lab = rgb_to_oklab(c1);
    let c2_lab = rgb_to_oklab(c2);

    let result_lab = Oklab {
        l: (1.0 - t) * c1_lab.l + t * c2_lab.l,
        a: (1.0 - t) * c1_lab.a + t * c2_lab.a,
        b: (1.0 - t) * c1_lab.b + t * c2_lab.b,
    };

    oklab_to_rgb(result_lab)
}

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

#[derive(Clone, Copy, PartialEq, Eq)]
enum EditDir {
    Increase,
    Decrease,
    Neutral,
}

pub struct MenuImpl {
    visible: bool,
    cursor_frame: u32,
    menu_stack: ArrayVec<Menu, 5>,
    edit_inc: f32,
    last_intedit_dir: EditDir,
    binding: BindingState,
    menu_pos_map: TinyMap<usize, u32, 64>,
}

impl Default for MenuImpl {
    fn default() -> Self {
        let mut menu_stack = ArrayVec::<Menu, 5>::new();
        if let Widget::Menu { label, widgets } = ROOT_MENU {
            menu_stack.push(Menu {
                label,
                widgets,
                ptr: &ROOT_MENU as *const _ as usize,
            });
        }
        Self {
            visible: false,
            cursor_frame: 0,
            menu_stack,
            edit_inc: 1.0,
            last_intedit_dir: EditDir::Neutral,
            binding: BindingState::default(),
            menu_pos_map: get_menu_widget_sel_map(),
        }
    }
}

impl MenuImpl {
    fn push_menu(&mut self, menu: Menu, cx: &mut MenuContext) {
        self.menu_stack.push(menu);
        self.cursor_frame = 0;
        cx.pad.reset_dir_repeat();
    }

    fn pop_menu(&mut self, cx: &mut MenuContext) {
        if self.menu_stack.len() == 1 {
            self.visible = false;
        } else {
            self.menu_stack.pop();
        }
        self.cursor_frame = 0;
        self.edit_inc = 1.0;
        self.last_intedit_dir = EditDir::Neutral;

        cx.pad.reset_dir_repeat();
    }

    fn handle_widget_bind(&mut self, cx: &mut MenuContext) {
        let menu = self.menu_stack.last().unwrap();
        let selected_idx = *self.menu_pos_map.get(menu.ptr);
        let selected_widget = get_selected_widget(menu.widgets, selected_idx, cx);
        let selected_widget = match selected_widget {
            Some(widget) => widget,
            None => return,
        };

        let a_down = cx.pad.button_down(Button::A, Prio::High);
        let y_down = cx.pad.button_down(Button::Y, Prio::High);
        let a_pressed = cx.pad.button_pressed(Button::A, Prio::High);
        let x_pressed = cx.pad.button_pressed(Button::X, Prio::High);
        let y_pressed = cx.pad.button_pressed(Button::Y, Prio::High);
        let a_repeat = cx.pad.button_repeat(Button::A, Prio::High);
        let y_repeat = cx.pad.button_repeat(Button::Y, Prio::High);

        match selected_widget {
            Widget::Checkbox { pref, .. } => {
                if a_pressed || y_pressed {
                    cx.pref.set(*pref, !cx.pref.get(*pref));
                    cx.pref.save();
                }
                if x_pressed {
                    cx.pref.set(*pref, cx.pref.get_default(*pref));
                    cx.pref.save();
                }
            }
            Widget::GetSetCheckbox { get, set, .. } => {
                if a_pressed || y_pressed {
                    let curr = get(cx);
                    set(!curr, cx);
                }
            }
            Widget::Menu { label, widgets } => {
                if a_pressed {
                    self.push_menu(
                        Menu {
                            label,
                            widgets,
                            ptr: selected_widget as *const _ as usize,
                        },
                        cx,
                    );
                }
            }
            Widget::Choose { pref, choices, .. } => {
                let curr_value = cx.pref.get(*pref);
                let curr_value = match choices.get(curr_value as usize) {
                    Some(_) => curr_value,
                    None => cx.pref.get_default(*pref),
                };
                if a_pressed {
                    let new_value = (curr_value as usize + 1) % choices.len();
                    cx.pref.set(*pref, new_value as u8);
                    cx.pref.save();
                }
                if y_pressed {
                    let new_value = (curr_value as usize + choices.len() - 1) % choices.len();
                    cx.pref.set(*pref, new_value as u8);
                    cx.pref.save();
                }
                if x_pressed {
                    cx.pref.set(*pref, cx.pref.get_default(*pref));
                    cx.pref.save();
                }
            }
            Widget::Button { push, after, .. } => {
                if a_pressed {
                    push(cx);
                    match after {
                        AfterPush::CloseMenu => self.visible = false,
                        AfterPush::GoBack => self.pop_menu(cx),
                    }
                }
            }
            Widget::IntEdit { pref, min, max, .. } => {
                let mut next = cx.pref.get(*pref) as i32;

                let edit_dir = if a_down && y_down || !a_down && !y_down {
                    EditDir::Neutral
                } else if a_down {
                    EditDir::Increase
                } else {
                    EditDir::Decrease
                };
                if edit_dir != self.last_intedit_dir {
                    self.edit_inc = 1.0;
                    self.last_intedit_dir = edit_dir;
                    cx.pad.reset_dir_repeat();
                }

                if x_pressed {
                    next = cx.pref.get_default(*pref) as i32;
                } else if edit_dir == EditDir::Increase && a_repeat {
                    self.edit_inc += 0.18;
                    next += self.edit_inc as i32;
                } else if edit_dir == EditDir::Decrease && y_repeat {
                    self.edit_inc += 0.18;
                    next -= self.edit_inc as i32;
                }
                next = next.clamp(*min as i32, *max as i32);
                if next != cx.pref.get(*pref) as i32 {
                    cx.pref.set(*pref, next as u8);
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
                    && cx.pad.button_released(Button::A, Prio::High)
                {
                    self.binding = BindingState::Active;
                } else if self.binding == BindingState::Active {
                    // set new bind
                    let encoding_type = cx.binds.get_encoding_type();
                    if encoding_type != binds::EncodingType::Invalid
                        && !(encoding_type == binds::EncodingType::SinglePress && *required_chord)
                    {
                        let value = cx.binds.get_current_encoding();
                        cx.pref.set(*pref, value);
                        cx.pref.save();
                        cx.pad.reset_dir_repeat();
                        self.binding = BindingState::Inactive;
                    }
                } else if a_pressed {
                    // enter rebind mode
                    self.binding = BindingState::Requested;
                } else if y_pressed {
                    // unbind
                    if *can_unbind {
                        cx.pref.set(*pref, 255);
                        cx.pref.save();
                    }
                } else if x_pressed {
                    // reset default bind
                    cx.pref.set(*pref, cx.pref.get_default(*pref));
                    cx.pref.save();
                }
            }
            _ => {}
        }
    }

    pub fn tick(&mut self, cx: &mut MenuContext) {
        if self.binding == BindingState::Active {
            self.handle_widget_bind(cx);
            return;
        }

        let toggle = cx
            .binds
            .bind_pressed(cx.pref.get(U8Pref::MenuBind), Prio::High, cx.pad);
        if toggle {
            self.visible ^= toggle;
        } else if cx.pad.button_pressed(Button::B, Prio::High) {
            self.pop_menu(cx);
        }
        let just_opened = self.visible && toggle;
        if just_opened {
            cx.pad.reset_dir_repeat();
            self.cursor_frame = 0;
        }

        cx.pad
            .set_priority(if self.visible { Prio::High } else { Prio::Low });

        if !self.visible {
            // Default binding is L+R, but this lets you know the current binding in case you forget
            // what you changed it to
            let input = cx.pref.get(U8Pref::MenuBind);
            if cx
                .pad
                .button_chord_pressed(Button::L, Button::R, Prio::High)
                && input != L_R_BIND
            {
                let mut buf = ArrayString::<32>::new();
                cx.binds.get_bind_str(input, &mut buf);
                cx.draw.notify(
                    draw::RED,
                    NotifyDuration::Long,
                    &fmt!(32, c"Use %s to toggle menu", cstr_buf!(buf)),
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
        if selectable > 0 {
            let selected_idx = self.menu_pos_map.get_mut(menu.ptr);
            *selected_idx =
                (*selected_idx as i32 + dir_delta + selectable as i32) as u32 % selectable;
        }

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
            Widget::TextFunc { label_func } => {
                let mut buf = ArrayString::<32>::new();
                label_func(&mut buf, cx);
                draw::debug_text(MARGIN + PAD, *y, draw::WHITE, &buf);
                *y += LINE_HEIGHT;
            }
            Widget::ColoredText {
                label,
                color_left,
                color_right,
                offset_x,
                line,
            } => {
                *y -= match line {
                    TextLine::Overlap => LINE_HEIGHT,
                    TextLine::NewLine => 0,
                };

                for i in 0..label.len() {
                    let t = if label.len() > 1 {
                        i as f32 / (label.len() - 1) as f32
                    } else {
                        0.0
                    };

                    let lerped_color = lerp_oklab(t, *color_left, *color_right);
                    let x = MARGIN + PAD + (*offset_x as u32 + i as u32) * DEBUG_CHAR_WIDTH;
                    let ch = &label[i..i + 1]; // Plz only use ASCII
                    draw::debug_text(x, *y, lerped_color, ch);
                }

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
                draw::debug_text(MARGIN + PAD + LABEL_OFFSET, *y, color, label);
                draw::debug_text(
                    MARGIN + PAD + PREF_OFFSET,
                    *y,
                    color,
                    if cx.pref.get(*pref) { "On" } else { "Off" },
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
                let current_idx = cx.pref.get(*pref) as usize;
                let default_idx = cx.pref.get_default(*pref) as usize;
                let (idx, choice) = match choices.get(current_idx) {
                    Some(c) => (current_idx, *c),
                    None => (default_idx, choices[default_idx]),
                };

                draw::debug_text(
                    MARGIN + PAD + PREF_OFFSET,
                    *y,
                    color,
                    &fmt!(32, c"(%d/%d) %s", idx + 1, choices.len(), cstr!(32, choice)),
                );
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
                draw::debug_text(
                    MARGIN + PAD + PREF_OFFSET,
                    *y,
                    color,
                    &fmt!(8, c"%d", cx.pref.get(*pref) as u32),
                );
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

                let input = cx.pref.get(*pref);
                let mut buf = ArrayString::<32>::new();
                cx.binds.get_bind_str(input, &mut buf);
                draw::debug_text(MARGIN + PAD + PREF_OFFSET, *y, bind_color, &buf);

                *y += LINE_HEIGHT;
                *selectable_idx += 1;
            }
            Widget::Custom { draw: custom_draw } => {
                custom_draw(cx);
            }
            Widget::RgbPreview {
                r_pref,
                g_pref,
                b_pref,
            } => {
                let color = mkb::GXColor {
                    r: cx.pref.get(*r_pref),
                    g: cx.pref.get(*g_pref),
                    b: cx.pref.get(*b_pref),
                    a: 0xff,
                };
                let x1 = 400.0;
                let y1 = real_y(*y as f32) + 4.0;
                let y2 = real_y(*y as f32) + 58.0;
                let x2 = (y2 - y1) + x1;
                draw::rect(x1, y1, x2, y2, color);
            }
        }
    }

    fn draw_menu_widgets(&self, menu: &Menu, cx: &mut MenuContext) {
        let mut y = MARGIN + PAD + 2 * LINE_HEIGHT;
        let mut selectable_idx = 0;

        let lerped_color = lerp_oklab(self.sin_lerp(40), FOCUSED_COLOR, UNFOCUSED_COLOR);

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

    pub fn draw(&self, cx: &mut MenuContext) {
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

    pub fn is_visible(&self) -> bool {
        self.visible
    }

    fn sin_lerp(&self, period_frames: u32) -> f32 {
        unsafe {
            let angle = ((self.cursor_frame % period_frames) as i32 - (period_frames / 2) as i32)
                as f32
                * 0x8000 as f32
                / (period_frames as f32 / 2.0);
            (mkb::math_sin(angle as i16) as f32 + 1.0) / 2.0
        }
    }
}

fn is_widget_selectable(widget: &Widget) -> bool {
    matches!(
        widget,
        Widget::Checkbox { .. }
            | Widget::GetSetCheckbox { .. }
            | Widget::Menu { .. }
            | Widget::Choose { .. }
            | Widget::Button { .. }
            | Widget::IntEdit { .. }
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
    draw::debug_text(START + 3 * BLOCK_WIDTH, Y_HEIGHT, draw::LIGHT_RED, "B");
    draw::debug_text(4 * BLOCK_WIDTH - BUTTON_OFFSET, Y_HEIGHT, draw::WHITE, ":");
    draw::debug_text(
        4 * BLOCK_WIDTH + HALF_SPACE - BUTTON_OFFSET,
        Y_HEIGHT,
        draw::WHITE,
        "Back",
    );
}

fn draw_bind_help(bind: &str, label: &str, index: u32, color: mkb::GXColor) {
    draw::debug_text(START + index * BLOCK_WIDTH, Y_HEIGHT, color, bind);
    draw::debug_text(
        (index + 1) * BLOCK_WIDTH - BUTTON_OFFSET,
        Y_HEIGHT,
        draw::WHITE,
        ":",
    );
    draw::debug_text(
        (index + 1) * BLOCK_WIDTH + HALF_SPACE - BUTTON_OFFSET,
        Y_HEIGHT,
        draw::WHITE,
        label,
    );
}

fn draw_help(widget: &Widget) {
    // draw relevant controls for current widget
    match widget {
        Widget::Checkbox { .. } | Widget::GetSetCheckbox { .. } => {
            draw_bind_help("A", "Toggle", 0, draw::LIGHT_GREEN);
            draw_bind_help("X", "Reset", 1, draw::GRAY);
        }
        Widget::Menu { .. } => {
            draw_bind_help("A", "Open", 0, draw::LIGHT_GREEN);
        }
        Widget::Button { .. } => {
            draw_bind_help("A", "Activate", 0, draw::LIGHT_GREEN);
        }
        Widget::Choose { .. } => {
            draw_bind_help("A", "Next", 0, draw::LIGHT_GREEN);
            draw_bind_help("Y", "Previous", 1, draw::GRAY);
            draw_bind_help("X", "Reset", 2, draw::GRAY);
        }
        Widget::IntEdit { .. } => {
            draw_bind_help("A", "Increase", 0, draw::LIGHT_GREEN);
            draw_bind_help("Y", "Decrease", 1, draw::GRAY);
            draw_bind_help("X", "Reset", 2, draw::GRAY);
        }
        Widget::InputSelect { can_unbind, .. } => {
            draw_bind_help("A", "Bind", 0, draw::LIGHT_GREEN);
            if *can_unbind {
                draw_bind_help("Y", "Unbind", 1, draw::GRAY);
                draw_bind_help("X", "Reset", 2, draw::GRAY);
            } else {
                draw_bind_help("X", "Reset", 1, draw::GRAY);
            }
        }
        _ => {}
    }
}
