#include "timerdisp.h"

#include "../utils/draw.h"

namespace timerdisp {

static constexpr u32 SECOND_FRAMES = 60;
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;

static constexpr s32 X = 378;
static constexpr s32 Y = 24;

s32 row_number_to_vertical_pos(u32 row_num) {
    return Y + 16 * row_num;
}

void draw_timer(u32 pos_x,
                u32 row,
                u32 text_offset,
                const char *prefix,
                s32 frames,
                bool show_seconds,
                GXColor color) {
    mkb::set_ui_widescreen_scale_mtx(320);
    bool positive = frames >= 0;
    if (!positive) frames = -frames;
    const char *sign = positive ? "" : "-";

    u32 hours = frames / HOUR_FRAMES;
    u32 minutes = frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 seconds = frames % MINUTE_FRAMES / SECOND_FRAMES;
    u32 centiseconds = (frames % SECOND_FRAMES) * 100 / 60;

    s32 A = pos_x;
    s32 a = A + text_offset;
    s32 b = row_number_to_vertical_pos(row);

    if (hours > 0 && !show_seconds) {
        draw::debug_text(A, b, color, prefix);
        draw::debug_text(a, b, color, "%s%d:%02d:%02d.%02d", sign, hours, minutes, seconds,
                         centiseconds);
    } else if (minutes > 0 && !show_seconds) {
        draw::debug_text(A, b, color, prefix);
        draw::debug_text(a, b, color, "%s%02d:%02d.%02d", sign, minutes, seconds, centiseconds);
    } else {
        u32 total_seconds =
            seconds + (minutes * MINUTE_FRAMES + hours * HOUR_FRAMES) / SECOND_FRAMES;
        draw::debug_text(A, b, color, prefix);
        draw::debug_text(a, b, color, "%s%02d.%02d", sign, total_seconds, centiseconds);
    }
    mkb::reset_ui_widescreen_scale_mtx();
}

// Special case of the above function with a hardcoded x-pos and text offset that covers a lot of
// use cases, used for the RTA/pause timer, challenge mode segment timer, and IW timer
void draw_timer_right_side(s32 frames,
                           const char *prefix,
                           u32 row,
                           mkb::GXColor color,
                           bool show_seconds) {
    draw_timer(X, row, 4 * draw::DEBUG_CHAR_WIDTH, prefix, frames, show_seconds, color);
}

void draw_subtick_timer(s32 frames,
                        const char *prefix,
                        u32 row,
                        GXColor color,
                        bool show_minutes,
                        u32 framesave,
                        bool extra_precision) {
    mkb::set_ui_widescreen_scale_mtx(320);

    bool positive = frames >= 0;
    if (!positive) frames = -frames;
    const char *sign = positive ? "" : "-";

    u32 hours = frames / HOUR_FRAMES;
    u32 minutes = frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 seconds = frames % MINUTE_FRAMES / SECOND_FRAMES;
    u32 milliseconds = ((frames % SECOND_FRAMES) * 100 + framesave) / 6;  // 3 digit
    u32 extra = (((frames % SECOND_FRAMES) * 100 + framesave) * 10) / 6;  // 4 digit

    s32 y = Y + row * 16;

    u32 total_seconds = seconds + (minutes * MINUTE_FRAMES + hours * HOUR_FRAMES) / SECOND_FRAMES;
    draw::debug_text(X, y, color, prefix);
    if (extra_precision) {
        draw::debug_text(X + 48, y, color, "%s%02d.%04d", sign, total_seconds, extra);
    } else {
        draw::debug_text(X + 48, y, color, "%s%02d.%03d", sign, total_seconds, milliseconds);
    }

    mkb::reset_ui_widescreen_scale_mtx();
}

void draw_percentage(s32 fsave, const char *prefix, u32 row, GXColor color) {
    mkb::set_ui_widescreen_scale_mtx(320);

    s32 y = Y + row * 16;
    draw::debug_text(X, y, color, prefix);
    draw::debug_text(X + 48, y, color, "%2d%", fsave);

    mkb::set_ui_widescreen_scale_mtx(0);
}

TimeComp format_time(u32 time) {  // input time in frames
    u32 time_hours = time / HOUR_FRAMES;
    u32 time_minutes = time % HOUR_FRAMES / MINUTE_FRAMES;
    u32 time_seconds = time % MINUTE_FRAMES / SECOND_FRAMES;
    u32 time_centiseconds = (time % SECOND_FRAMES) * 100 / 60;
    return {time_hours, time_minutes, time_seconds, time_centiseconds};
}

void format_time_to_buffer(char *buffer,
                           u32 time,
                           TimeFormatType format_type) {  // time is in frames
    TimeComp formatted_time = format_time(time);

    switch (format_type) {
        case TimeFormatType::SECONDS_ONLY: {
            u32 seconds = time / SECOND_FRAMES;
            u32 centiseconds = (time % SECOND_FRAMES) * 100 / 60;
            mkb::sprintf(buffer, "%d.%02d", seconds, centiseconds);
            break;
        }
        case TimeFormatType::HOURS_ALWAYS: {
            mkb::sprintf(buffer, "%d:%02d:%02d.%02d", formatted_time.hours, formatted_time.minutes,
                         formatted_time.seconds, formatted_time.centiseconds);
            break;
        }
        case TimeFormatType::MINUTES_ALWAYS_LEADING_ZERO: {
            if (formatted_time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", formatted_time.hours,
                             formatted_time.minutes, formatted_time.seconds,
                             formatted_time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%02d:%02d.%02d", formatted_time.minutes,
                             formatted_time.seconds, formatted_time.centiseconds);
            }
            break;
        }
        case TimeFormatType::MINIMAL_LEADING: {
            if (formatted_time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", formatted_time.hours,
                             formatted_time.minutes, formatted_time.seconds,
                             formatted_time.centiseconds);
            } else if (formatted_time.minutes > 0) {
                mkb::sprintf(buffer, "%d:%02d.%02d", formatted_time.minutes, formatted_time.seconds,
                             formatted_time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%d.%02d", formatted_time.seconds,
                             formatted_time.centiseconds);
            }
            break;
        }
        case TimeFormatType::ALWAYS_LEAD_NON_HOURS: {
            if (formatted_time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", formatted_time.hours,
                             formatted_time.minutes, formatted_time.seconds,
                             formatted_time.centiseconds);
            } else if (formatted_time.minutes > 0) {
                mkb::sprintf(buffer, "%02d:%02d.%02d", formatted_time.minutes,
                             formatted_time.seconds, formatted_time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%02d.%02d", formatted_time.seconds,
                             formatted_time.centiseconds);
            }
            break;
        }
    }
}

TimeComp format_time(u32 time) {  // input time in frames
    u32 time_hours = time / HOUR_FRAMES;
    u32 time_minutes = time % HOUR_FRAMES / MINUTE_FRAMES;
    u32 time_seconds = time % MINUTE_FRAMES / SECOND_FRAMES;
    u32 time_centiseconds = (time % SECOND_FRAMES) * 100 / 60;
    return {time_hours, time_minutes, time_seconds, time_centiseconds};
}

void format_time_to_buffer(char *buffer,
                           u32 time,
                           TimeFormatType format_type) {  // time is in frames
    TimeComp formatted_time = format_time(time);

    switch (format_type) {
        case TimeFormatType::SECONDS_ONLY: {
            u32 seconds = time / SECOND_FRAMES;
            u32 centiseconds = (time % SECOND_FRAMES) * 100 / 60;
            mkb::sprintf(buffer, "%d.%02d", seconds, centiseconds);
            break;
        }
        case TimeFormatType::HOURS_ALWAYS: {
            mkb::sprintf(buffer, "%d:%02d:%02d.%02d", formatted_time.hours, formatted_time.minutes,
                         formatted_time.seconds, formatted_time.centiseconds);
            break;
        }
        case TimeFormatType::MINUTES_ALWAYS_LEADING_ZERO: {
            if (formatted_time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", formatted_time.hours,
                             formatted_time.minutes, formatted_time.seconds,
                             formatted_time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%02d:%02d.%02d", formatted_time.minutes,
                             formatted_time.seconds, formatted_time.centiseconds);
            }
            break;
        }
        case TimeFormatType::MINIMAL_LEADING: {
            if (formatted_time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", formatted_time.hours,
                             formatted_time.minutes, formatted_time.seconds,
                             formatted_time.centiseconds);
            } else if (formatted_time.minutes > 0) {
                mkb::sprintf(buffer, "%d:%02d.%02d", formatted_time.minutes, formatted_time.seconds,
                             formatted_time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%d.%02d", formatted_time.seconds,
                             formatted_time.centiseconds);
            }
            break;
        }
        case TimeFormatType::ALWAYS_LEAD_NON_HOURS: {
            if (formatted_time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", formatted_time.hours,
                             formatted_time.minutes, formatted_time.seconds,
                             formatted_time.centiseconds);
            } else if (formatted_time.minutes > 0) {
                mkb::sprintf(buffer, "%02d:%02d.%02d", formatted_time.minutes,
                             formatted_time.seconds, formatted_time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%02d.%02d", formatted_time.seconds,
                             formatted_time.centiseconds);
            }
            break;
        }
    }
}

}  // namespace timerdisp
