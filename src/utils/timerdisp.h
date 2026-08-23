#pragma once

#include "mkb/mkb.h"

namespace timerdisp {

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

s32 row_number_to_vertical_pos(s32 row_num);

}  // namespace timerdisp
