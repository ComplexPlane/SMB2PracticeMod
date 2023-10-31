#include "timerdisp.h"

#include "utils/draw.h"

namespace timerdisp {

static constexpr u32 SECOND_FRAMES = 60;
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;

static constexpr s32 X = 380;
static constexpr s32 X_2 = 18;
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
        draw::debug_text(X + 54, y, color, "%s%d:%02d:%02d.%02d", sign, hours, minutes, seconds,
                         centiseconds);
    } else if (minutes > 0 && !show_seconds) {
        draw::debug_text(X, y, color, prefix);
        draw::debug_text(X + 54, y, color, "%s%02d:%02d.%02d", sign, minutes, seconds,
                         centiseconds);
    } else {
        u32 total_seconds =
            seconds + (minutes * MINUTE_FRAMES + hours * HOUR_FRAMES) / SECOND_FRAMES;
        draw::debug_text(X, y, color, prefix);
        draw::debug_text(X + 54, y, color, "%s%02d.%02d", sign, total_seconds, centiseconds);
    }
}

/*
void draw_storytimer(s32 frames, const char* prefix, u32 row, mkb::GXColor color, bool show_seconds) {
    bool positive = frames >= 0;
    if (!positive) frames = -frames;
    const char* sign = positive ? "" : "-";

    u32 hours = frames / HOUR_FRAMES;
    u32 minutes = frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 seconds = frames % MINUTE_FRAMES / SECOND_FRAMES;
    u32 centiseconds = (frames % SECOND_FRAMES) * 100 / 60;

    s32 y = Y + (row+2) * 16;

    if (hours > 0 && !show_seconds) {
        draw::debug_text(X_2, y, color, prefix);
        draw::debug_text(X_2 + 54, y, color, "%s%d:%02d:%02d.%02d", sign, hours, minutes, seconds,
                         centiseconds);
    } else if (minutes > 0 && !show_seconds) {
        draw::debug_text(X_2, y, color, prefix);
        draw::debug_text(X_2 + 54, y, color, "%s%02d:%02d.%02d", sign, minutes, seconds,
                         centiseconds);
    } else {
        u32 total_seconds =
            seconds + (minutes * MINUTE_FRAMES + hours * HOUR_FRAMES) / SECOND_FRAMES;
        draw::debug_text(X_2, y, color, prefix);
        draw::debug_text(X_2 + 54, y, color, "%s%02d.%02d", sign, total_seconds, centiseconds);
    }
} */

void draw_storytimer(s32 frames_1, const char* prefix, u32 row, mkb::GXColor color, bool show_seconds, bool second_argument, s32 frames_2) {
    bool positive = frames_1 >= 0;
    if (!positive) frames_1 = -frames_1;
    const char* sign = positive ? "" : "-";

    u32 hours_1 = frames_1 / HOUR_FRAMES;
    u32 minutes_1 = frames_1 % HOUR_FRAMES / MINUTE_FRAMES;
    u32 seconds_1 = frames_1 % MINUTE_FRAMES / SECOND_FRAMES;
    u32 centiseconds_1 = (frames_1 % SECOND_FRAMES) * 100 / 60;

    u32 hours_2 = frames_2 / HOUR_FRAMES;
    u32 minutes_2 = frames_2 % HOUR_FRAMES / MINUTE_FRAMES;
    u32 seconds_2 = frames_2 % MINUTE_FRAMES / SECOND_FRAMES;
    u32 centiseconds_2 = (frames_2 % SECOND_FRAMES) * 100 / 60;

    s32 y = Y + (row+2) * 16;
/*
    if (second_argument == false) {
        if (hours_1 > 0 && !show_seconds) {
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%d:%02d:%02d.%02d", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1);
        } else if (minutes_1 > 0 && !show_seconds) {
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%02d:%02d.%02d", sign, minutes_1, seconds_1,
                            centiseconds_1);
        } else {
            u32 total_seconds_1 =
                seconds_1 + (minutes_1 * MINUTE_FRAMES + hours_1 * HOUR_FRAMES) / SECOND_FRAMES;
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%02d.%02d", sign, total_seconds_1, centiseconds_1);
        }
    } else {
         if (hours_1 > 0 && hours_2 > 0 && !show_seconds) {
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%d:%02d:%02d.%02d (%s%d:%02d:%02d.%02d)", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1, sign, hours_2, minutes_2, seconds_2, centiseconds_2);
        } else if (minutes_1 > 0 && minutes_2 > 0 && !show_seconds) {
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%d:%02d:%02d.%02d (%s%d:%02d:%02d.%02d)", sign, minutes_1, seconds_1,
                            centiseconds_1, sign, minutes_2, seconds_2, centiseconds_2);
        } else {
            u32 total_seconds_2 =
                seconds_2 + (minutes_2 * MINUTE_FRAMES + hours_2 * HOUR_FRAMES) / SECOND_FRAMES;
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%02d.%02d", sign, total_seconds_2, centiseconds_2);
        }
    }
*/
    if (second_argument == true){ 
        if (hours_1 > 0) {
            if (hours_2 > 0 ) {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%d:%02d:%02d.%02d (%s%d:%02d:%02d.%02d)", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1, sign, hours_2, minutes_2, seconds_2, centiseconds_2);
            } else if (minutes_2 > 0) {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%d:%02d:%02d.%02d (%s%02d:%02d.%02d)", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1, sign, minutes_2, seconds_2, centiseconds_2);
            } else {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%d:%02d:%02d.%02d (%s%02d.%02d)", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1, sign, seconds_2, centiseconds_2);
            }
        } else if (minutes_1 > 0) {
            if (hours_2 > 0) {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%02d:%02d.%02d (%s%d:%02d:%02d.%02d)", sign, minutes_1, seconds_1,
                            centiseconds_1, sign, hours_2, minutes_2, seconds_2, centiseconds_2);
            } else if (minutes_2 > 0) {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%02d:%02d.%02d (%s%02d:%02d.%02d)", sign, minutes_1, seconds_1,
                            centiseconds_1, sign, minutes_2, seconds_2, centiseconds_2);
            } else {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%02d:%02d.%02d (%s%02d.%02d)", sign, minutes_1, seconds_1,
                            centiseconds_1, sign, seconds_2, centiseconds_2);
            }

        } else {
            if (hours_2 > 0) {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%d.%02d (%s%d:%02d:%02d.%02d)", sign, seconds_1,
                            centiseconds_1, sign, hours_2, minutes_2, seconds_2, centiseconds_2);
            } else if (minutes_2 > 0) {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%d.%02d (%s%d:%02d.%02d)", sign, seconds_1,
                            centiseconds_1, sign, minutes_2, seconds_2, centiseconds_2);
            } else {
                draw::debug_text(X_2, y, color, prefix);
                draw::debug_text(X_2 + 54, y, color, "%s%d.%02d (%s%d.%02d)", sign, seconds_1,
                            centiseconds_1, sign, seconds_2, centiseconds_2);
            }
        }
    } else {
        if (hours_1 > 0 && !show_seconds) {
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%d:%02d:%02d.%02d", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1);
        } else if (minutes_1 > 0 && !show_seconds) {
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%02d:%02d.%02d", sign, minutes_1, seconds_1,
                            centiseconds_1);
        } else {
            u32 total_seconds_1 =
                seconds_1 + (minutes_1 * MINUTE_FRAMES + hours_1 * HOUR_FRAMES) / SECOND_FRAMES;
            draw::debug_text(X_2, y, color, prefix);
            draw::debug_text(X_2 + 54, y, color, "%s%02d.%02d", sign, total_seconds_1, centiseconds_1);
        }
    }
}

void draw_timer_general(u32 pos_x, u32 pos_y, u32 text_offset, const char* prefix, s32 frames_1, s32 frames_2, bool show_second_argument, bool show_seconds_only, mkb::GXColor color){
    bool positive = frames_1 >= 0;
    if (!positive) frames_1 = -frames_1;
    const char* sign = positive ? "" : "-";

    u32 hours_1 = frames_1 / HOUR_FRAMES;
    u32 minutes_1 = frames_1 % HOUR_FRAMES / MINUTE_FRAMES;
    u32 seconds_1 = frames_1 % MINUTE_FRAMES / SECOND_FRAMES;
    u32 centiseconds_1 = (frames_1 % SECOND_FRAMES) * 100 / 60;

    u32 hours_2 = frames_2 / HOUR_FRAMES;
    u32 minutes_2 = frames_2 % HOUR_FRAMES / MINUTE_FRAMES;
    u32 seconds_2 = frames_2 % MINUTE_FRAMES / SECOND_FRAMES;
    u32 centiseconds_2 = (frames_2 % SECOND_FRAMES) * 100 / 60;

    s32 A = pos_x;
    s32 a = A + text_offset; 
    s32 b = Y + (pos_y+2)*16;

    if (show_second_argument == true){ 
        if (hours_1 > 0) {
            if (hours_2 > 0 ) {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%d:%02d:%02d.%02d (%s%d:%02d:%02d.%02d)", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1, sign, hours_2, minutes_2, seconds_2, centiseconds_2);
            } else if (minutes_2 > 0) {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%d:%02d:%02d.%02d (%s%02d:%02d.%02d)", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1, sign, minutes_2, seconds_2, centiseconds_2);
            } else {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%d:%02d:%02d.%02d (%s%02d.%02d)", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1, sign, seconds_2, centiseconds_2);
            }
        } else if (minutes_1 > 0) {
            if (hours_2 > 0) {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%02d:%02d.%02d (%s%d:%02d:%02d.%02d)", sign, minutes_1, seconds_1,
                            centiseconds_1, sign, hours_2, minutes_2, seconds_2, centiseconds_2);
            } else if (minutes_2 > 0) {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%02d:%02d.%02d (%s%02d:%02d.%02d)", sign, minutes_1, seconds_1,
                            centiseconds_1, sign, minutes_2, seconds_2, centiseconds_2);
            } else {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%02d:%02d.%02d (%s%02d.%02d)", sign, minutes_1, seconds_1,
                            centiseconds_1, sign, seconds_2, centiseconds_2);
            }

        } else {
            if (hours_2 > 0) {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%d.%02d (%s%d:%02d:%02d.%02d)", sign, seconds_1,
                            centiseconds_1, sign, hours_2, minutes_2, seconds_2, centiseconds_2);
            } else if (minutes_2 > 0) {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%d.%02d (%s%d:%02d.%02d)", sign, seconds_1,
                            centiseconds_1, sign, minutes_2, seconds_2, centiseconds_2);
            } else {
                draw::debug_text(A, b, color, prefix);
                draw::debug_text(a, b, color, "%s%d.%02d (%s%d.%02d)", sign, seconds_1,
                            centiseconds_1, sign, seconds_2, centiseconds_2);
            }
        }
    } else {
        if (hours_1 > 0 && !show_seconds_only) {
            draw::debug_text(A, b, color, prefix);
            draw::debug_text(a, b, color, "%s%d:%02d:%02d.%02d", sign, hours_1, minutes_1, seconds_1,
                            centiseconds_1);
        } else if (minutes_1 > 0 && !show_seconds_only) {
            draw::debug_text(A, b, color, prefix);
            draw::debug_text(A, b, color, "%s%02d:%02d.%02d", sign, minutes_1, seconds_1,
                            centiseconds_1);
        } else {
            u32 total_seconds_1 =
                seconds_1 + (minutes_1 * MINUTE_FRAMES + hours_1 * HOUR_FRAMES) / SECOND_FRAMES;
            draw::debug_text(A, b, color, prefix);
            draw::debug_text(a, b, color, "%s%02d.%02d", sign, total_seconds_1, centiseconds_1);
        }
    }
}

}  // namespace timerdisp
