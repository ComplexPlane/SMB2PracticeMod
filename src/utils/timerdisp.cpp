#include "timerdisp.h"

namespace timerdisp {

static constexpr u32 SECOND_FRAMES = 60;
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;

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

}  // namespace timerdisp
