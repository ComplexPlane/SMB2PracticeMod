#include "timerdisp.h"

#include "utils/draw.h"

namespace timerdisp {

static constexpr u32 SECOND_FRAMES = 60;
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;

// static constexpr s32 X = 378;
static constexpr s32 Y = 24;
static constexpr s32 ROW_HEIGHT = 16;

s32 row_number_to_vertical_pos(s32 row_num) {
    return Y + ROW_HEIGHT * row_num;
}

TimeComp get_time_components(u32 frames) {
    u32 time_hours = frames / HOUR_FRAMES;
    u32 time_minutes = frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 time_seconds = frames % MINUTE_FRAMES / SECOND_FRAMES;
    u32 time_centiseconds = (frames % SECOND_FRAMES) * 100 / 60;
    return {time_hours, time_minutes, time_seconds, time_centiseconds};
}

void format_time(char *buffer, u32 frames, TimeFormat format) {
    TimeComp time = get_time_components(frames);

    switch (format) {
        case TimeFormat::SecondsOnly: {
            u32 total_seconds = frames / SECOND_FRAMES;
            mkb::sprintf(buffer, "%d.%02d", total_seconds, time.centiseconds);
            break;
        }
        case TimeFormat::HoursAlways: {
            mkb::sprintf(buffer, "%d:%02d:%02d.%02d", time.hours, time.minutes, time.seconds,
                         time.centiseconds);
            break;
        }
        case TimeFormat::MinutesAlwaysLeadingZero: {
            if (time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", time.hours, time.minutes, time.seconds,
                             time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%02d:%02d.%02d", time.minutes, time.seconds,
                             time.centiseconds);
            }
            break;
        }
        case TimeFormat::MinimalLeading: {
            if (time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", time.hours, time.minutes, time.seconds,
                             time.centiseconds);
            } else if (time.minutes > 0) {
                mkb::sprintf(buffer, "%d:%02d.%02d", time.minutes, time.seconds, time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%d.%02d", time.seconds, time.centiseconds);
            }
            break;
        }
        case TimeFormat::AlwaysLeadNonHours: {
            if (time.hours > 0) {
                mkb::sprintf(buffer, "%d:%02d:%02d.%02d", time.hours, time.minutes, time.seconds,
                             time.centiseconds);
            } else if (time.minutes > 0) {
                mkb::sprintf(buffer, "%02d:%02d.%02d", time.minutes, time.seconds,
                             time.centiseconds);
            } else {
                mkb::sprintf(buffer, "%02d.%02d", time.seconds, time.centiseconds);
            }
            break;
        }
        case TimeFormat::Unformatted: {
            mkb::sprintf(buffer, "%d", frames);
        }
    }
}

void format_signed_time(char *buffer, s32 frames, TimeFormat format) {
    bool positive = frames >= 0;
    if (!positive) frames = -frames;
    const char *sign = positive ? "" : "-";

    char time_buf[16] = {};
    format_time(time_buf, frames, format);

    mkb::sprintf(buffer, "%s%s", sign, time_buf);
}

/* void draw_timer(s32 pos_x,
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

    s32 pos_y = row_number_to_vertical_pos(row);

    char buf[16] = {};
    if (!show_seconds) {
        format_time(buf, frames, TimeFormat::AlwaysLeadNonHours);
    } else {
        format_time(buf, frames, TimeFormat::SecondsOnly);
    }

    draw::debug_text(pos_x, pos_y, color, prefix);
    draw::debug_text(pos_x + text_offset, pos_y, color, "%s%s", sign, buf);
    mkb::reset_ui_widescreen_scale_mtx();
} */

// Special case of the above function with a hardcoded x-pos and text offset that covers a lot of
// use cases, used for the RTA/pause timer, challenge mode segment timer, and IW timer
/* void draw_timer_right_side(s32 frames,
                           const char *prefix,
                           u32 row,
                           mkb::GXColor color,
                           bool show_seconds) {
    draw_timer(X, row, 4 * draw::DEBUG_CHAR_WIDTH, prefix, frames, show_seconds, color);
} */

void format_subtick_time(char *buffer, s32 frames, u32 framesave, bool extra_precision) {
    bool positive = frames >= 0;
    if (!positive) frames = -frames;
    const char *sign = positive ? "" : "-";

    TimeComp time = get_time_components(frames);
    u32 total_seconds = frames / SECOND_FRAMES;
    u32 milliseconds = time.centiseconds * 10 + framesave / 6;  // 3 digit
    u32 extra = time.centiseconds * 100 + framesave * 10 / 6;   // 4 digit

    if (extra_precision) {
        mkb::sprintf(buffer, "%s%02d.%04d", sign, total_seconds, extra);
    } else {
        mkb::sprintf(buffer, "%s%02d.%03d", sign, total_seconds, milliseconds);
    }
}

/* void draw_subtick_timer(s32 frames,
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
    // (frames % SECOND_FRAMES) * 1000 / 60 + framesave / 6;
    // centiseconds * 10 + framesave / 6;
    u32 extra = (((frames % SECOND_FRAMES) * 100 + framesave) * 10) / 6;  // 4 digit
    // (frames % SECOND_FRAMES) * 10000 / 60 + framesave * 10 / 6;
    // centiseconds * 100 + framesave * 10 / 6;

    s32 y = row_number_to_vertical_pos(row);

    u32 total_seconds = seconds + (minutes * MINUTE_FRAMES + hours * HOUR_FRAMES) / SECOND_FRAMES;
    draw::debug_text(X, y, color, prefix);
    if (extra_precision) {
        draw::debug_text(X + 4 * draw::DEBUG_CHAR_WIDTH, y, color, "%s%02d.%04d", sign,
                         total_seconds, extra);
    } else {
        draw::debug_text(X + 4 * draw::DEBUG_CHAR_WIDTH, y, color, "%s%02d.%03d", sign,
                         total_seconds, milliseconds);
    }

    mkb::reset_ui_widescreen_scale_mtx();
} */

/* void draw_percentage(s32 fsave, const char *prefix, u32 row, GXColor color) {
    mkb::set_ui_widescreen_scale_mtx(320);

    s32 y = row_number_to_vertical_pos(row);
    draw::debug_text(X, y, color, prefix);
    draw::debug_text(X + 48, y, color, "%2d%", fsave);

    mkb::set_ui_widescreen_scale_mtx(0);
} */

}  // namespace timerdisp
