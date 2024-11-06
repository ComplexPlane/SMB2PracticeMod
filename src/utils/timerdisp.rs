use crate::{cstr, fmt, mkb, systems::draw};

const SECOND_FRAMES: u32 = 60;
const MINUTE_FRAMES: u32 = SECOND_FRAMES * 60;
const HOUR_FRAMES: u32 = MINUTE_FRAMES * 60;

const X: u32 = 378;
const Y: u32 = 24;

pub fn draw_timer(frames: i32, prefix: &str, row: u32, color: mkb::GXColor, show_seconds: bool) {
    let positive = frames >= 0;
    let frames = frames.unsigned_abs();
    let sign = if positive { "" } else { "-" };

    let hours = frames / HOUR_FRAMES;
    let minutes = frames % HOUR_FRAMES / MINUTE_FRAMES;
    let seconds = frames % MINUTE_FRAMES / SECOND_FRAMES;
    let centiseconds = (frames % SECOND_FRAMES) * 100 / 60;

    let y = Y + row * 16;

    if hours > 0 && !show_seconds {
        draw::debug_text(X, y, color, prefix);
        draw::debug_text(
            X + 48,
            y,
            color,
            &fmt!(
                32,
                c"%s%d:%02d:%02d.%02d",
                cstr!(8, sign),
                hours,
                minutes,
                seconds,
                centiseconds
            ),
        );
    } else if minutes > 0 && !show_seconds {
        draw::debug_text(X, y, color, prefix);
        draw::debug_text(
            X + 48,
            y,
            color,
            &fmt!(
                32,
                c"%s%02d:%02d.%02d",
                cstr!(8, sign),
                minutes,
                seconds,
                centiseconds
            ),
        );
    } else {
        let total_seconds =
            seconds + (minutes * MINUTE_FRAMES + hours * HOUR_FRAMES) / SECOND_FRAMES;
        draw::debug_text(X, y, color, prefix);
        draw::debug_text(
            X + 48,
            y,
            color,
            &fmt!(
                32,
                c"%s%02d.%02d",
                cstr!(8, sign),
                total_seconds,
                centiseconds
            ),
        );
    }
}

pub fn draw_subtick_timer(
    frames: i32,
    prefix: &str,
    row: u32,
    color: mkb::GXColor,
    framesave: u32,
    extra_precision: bool,
) {
    let positive = frames >= 0;
    let frames = frames.unsigned_abs();
    let sign = if positive { "" } else { "-" };

    let hours = frames / HOUR_FRAMES;
    let minutes = frames % HOUR_FRAMES / MINUTE_FRAMES;
    let seconds = frames % MINUTE_FRAMES / SECOND_FRAMES;
    let milliseconds = ((frames % SECOND_FRAMES) * 100 + framesave) / 6; // 3 digit
    let extra = (((frames % SECOND_FRAMES) * 100 + framesave) * 10) / 6; // 4 digit

    let y = Y + row * 16;

    let total_seconds = seconds + (minutes * MINUTE_FRAMES + hours * HOUR_FRAMES) / SECOND_FRAMES;
    draw::debug_text(X, y, color, prefix);
    if extra_precision {
        draw::debug_text(
            X + 48,
            y,
            color,
            &fmt!(32, c"%s%02d.%04d", cstr!(8, sign), total_seconds, extra),
        );
    } else {
        draw::debug_text(
            X + 48,
            y,
            color,
            &fmt!(
                32,
                c"%s%02d.%03d",
                cstr!(8, sign),
                total_seconds,
                milliseconds
            ),
        );
    }
}

pub fn draw_percentage(fsave: i32, prefix: &str, row: u32, color: mkb::GXColor) {
    let y = Y + row * 16;
    draw::debug_text(X, y, color, prefix);
    draw::debug_text(X + 48, y, color, &fmt!(32, c"%2d%", fsave));
}
