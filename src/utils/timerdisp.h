#pragma once

#include "../mkb/mkb.h"

namespace timerdisp {

// TimeComp = "Time Components"
struct TimeComp {
    u32 hours;
    u32 minutes;
    u32 seconds;
    u32 centiseconds;
};

enum class TimeFormatType {
    SECONDS_ONLY,                 // ss.cc
    HOURS_ALWAYS,                 // h:mm:ss.cc or 0:mm:ss.cc
    MINUTES_ALWAYS_LEADING_ZERO,  // 0m:ss.cc or mm:ss.cc if <1 hour, h:mm:ss.cc otherwise
    MINIMAL_LEADING,       // ss.cc if < 1 min, m:ss.cc if < 10 min, mm:ss.cc if > 10 min, < 1h,
                           // h:mm:ss.cc otherwise
    ALWAYS_LEAD_NON_HOURS  // mm:ss.cc if < 1 hour, ss.cc if < 1 min
};

s32 row_number_to_vertical_pos(u32 row_num);
void draw_timer(u32 pos_x,
                u32 row,
                u32 text_offset,
                const char *prefix,
                s32 frames,
                bool show_seconds,
                mkb::GXColor color);
void draw_timer_right_side(s32 frames,
                           const char *prefix,
                           u32 row,
                           mkb::GXColor color,
                           bool show_seconds);
void draw_subtick_timer(s32 frames,
                        const char *prefix,
                        u32 row,
                        mkb::GXColor color,
                        bool show_minutes,
                        u32 framesave,
                        bool extra_precision);
void draw_percentage(s32 fsave, const char *prefix, u32 row, mkb::GXColor color);

TimeComp format_time(u32 time);
void format_time_to_buffer(char *buffer, u32 time, TimeFormatType format_type);

}  // namespace timerdisp
