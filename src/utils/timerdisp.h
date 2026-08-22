#pragma once

#include "mkb/mkb.h"
#include "utils/draw.h"

namespace timerdisp {

// Where numbers on the right side get aligned (not their prefixes!)
constexpr s32 RIGHT_SIDE_TIMER_ALIGN_X = 378 + 4 * draw::DEBUG_CHAR_WIDTH;

// TimeComp = "Time Components"
struct TimeComp {
    u32 hours;
    u32 minutes;
    u32 seconds;
    u32 centiseconds;
};

enum class TimeFormat {
    SecondsOnly,               // ss.cc
    HoursAlways,               // h:mm:ss.cc or 0:mm:ss.cc
    MinutesAlwaysLeadingZero,  // 0m:ss.cc or mm:ss.cc if <1 hour, h:mm:ss.cc otherwise
    MinimalLeading,            // ss.cc if < 1 min, m:ss.cc if < 10 min, mm:ss.cc if > 10 min, < 1h,
                               // h:mm:ss.cc otherwise
    AlwaysLeadNonHours,        // mm:ss.cc if < 1 hour, ss.cc if < 1 min
    Unformatted
};

void format_time(char *buffer, u32 frames, TimeFormat format);
void format_signed_time(char *buffer, s32 frames, TimeFormat format);
void format_subtick_time(char *buffer, s32 frames, u32 framesave, bool extra_precision);

s32 row_number_to_vertical_pos(u32 row_num);
void draw_timer(s32 pos_x,
                u32 row,
                u32 text_offset,
                const char *prefix,
                s32 frames,
                bool show_seconds,
                GXColor color);
void draw_timer_right_side(s32 frames,
                           const char *prefix,
                           u32 row,
                           GXColor color,
                           bool show_seconds);
void draw_subtick_timer(s32 frames,
                        const char *prefix,
                        u32 row,
                        GXColor color,
                        bool show_minutes,
                        u32 framesave,
                        bool extra_precision);
void draw_percentage(s32 fsave, const char *prefix, u32 row, GXColor color);

}  // namespace timerdisp
