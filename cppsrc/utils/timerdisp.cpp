#include "timerdisp.h"

#include "utils/draw.h"

namespace timerdisp {

static constexpr u32 SECOND_FRAMES = 60;
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;

static constexpr s32 X = 378;
static constexpr s32 Y = 24;

void draw_timer(s32 frames, const char* prefix, u32 row, mkb::GXColor color, bool show_seconds) {
    bool positive = frames >= 0;
    if (!positive) frames = -frames;
    const char* sign = positive ? "" : "-";

    u32 hours = frames / HOUR_FRAMES;
    u32 minutes = frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 seconds = frames % MINUTE_FRAMES / SECOND_FRAMES;
    u32 centiseconds = (frames % SECOND_FRAMES) * 100 / 60;

    s32 y = Y + row * 16;

    if (hours > 0 && !show_seconds) {
        draw::debug_text(X, y, color, prefix);
        draw::debug_text(X + 48, y, color, "%s%d:%02d:%02d.%02d", sign, hours, minutes, seconds,
                         centiseconds);
    } else if (minutes > 0 && !show_seconds) {
        draw::debug_text(X, y, color, prefix);
        draw::debug_text(X + 48, y, color, "%s%02d:%02d.%02d", sign, minutes, seconds,
                         centiseconds);
    } else {
        u32 total_seconds =
            seconds + (minutes * MINUTE_FRAMES + hours * HOUR_FRAMES) / SECOND_FRAMES;
        draw::debug_text(X, y, color, prefix);
        draw::debug_text(X + 48, y, color, "%s%02d.%02d", sign, total_seconds, centiseconds);
    }
}

void draw_subtick_timer(s32 frames, const char* prefix, u32 row, mkb::GXColor color,
                        bool show_minutes, u32 framesave, bool extra_precision) {
    bool positive = frames >= 0;
    if (!positive) frames = -frames;
    const char* sign = positive ? "" : "-";

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
}

void draw_percentage(s32 fsave, const char* prefix, u32 row, mkb::GXColor color) {
    s32 y = Y + row * 16;
    draw::debug_text(X, y, color, prefix);
    draw::debug_text(X + 48, y, color, "%2d%", fsave);
}

}  // namespace timerdisp
