#include "storybreakdown.h"

#include "deathcounter.h"
#include "storytimer.h"
#include "systems/goal.h"
#include "systems/pref.h"
#include "systems/textinfo.h"
#include "utils/draw.h"
#include "utils/mode.h"

namespace storybreakdown {

constexpr u16 WORLD_COUNT = mode::WORLD_COUNT;
constexpr u16 STAGES_PER_WORLD = mode::STAGES_PER_WORLD;

using Slot = textinfo::Slot;
using Format = timerdisp::TimeFormat;

// We only use this function for 0 <= row <= 10
s32 get_breakdown_row_x_pos(u16 row) {
    s32 num_x_pos = textinfo::get_slot_x_alignment(Slot::Left);
    if (row < WORLD_COUNT - 1) {  // "Wk:" is 3 characters long if 1 <= k <= 9
        return num_x_pos - 3 * draw::DEBUG_CHAR_WIDTH;
    } else if (row == WORLD_COUNT - 1) {  // "W10:" is 4 characters long
        return num_x_pos - 4 * draw::DEBUG_CHAR_WIDTH;
    } else {  // "Totals:" is 7 characters long
        return num_x_pos - 7 * draw::DEBUG_CHAR_WIDTH;
    }
}

void draw_breakdown_screen() {
    // Format: "Wk: world k split time (world k segment time) (world k deaths)"
    char split_buf[16] = {};
    char seg_buf[16] = {};

    for (u16 idx = 0; idx < WORLD_COUNT; idx++) {
        s32 pos_x = get_breakdown_row_x_pos(idx);
        u32 world_split = storytimer::get_split_timer_for_world(idx);
        u32 world_segment = storytimer::get_world_timer_info(idx).segment;
        u32 world_deaths = deathcounter::get_world_death_count(idx);

        timerdisp::format_time(split_buf, world_split, Format::MinutesAlwaysLeadingZero);
        timerdisp::format_time(seg_buf, world_segment, Format::MinutesAlwaysLeadingZero);

        textinfo::draw(Slot::Left, pos_x, draw::WHITE, true, "W%d:%s (%s) (%d)", idx + 1, split_buf,
                       seg_buf, world_deaths);
    }

    // For the totals row
    s32 totals_x_pos = get_breakdown_row_x_pos(WORLD_COUNT);
    u32 loadless_time = storytimer::get_loadless_time();
    u32 total_deaths = deathcounter::get_total_death_count();

    char total_time_buf[16] = {};
    timerdisp::format_time(total_time_buf, loadless_time, Format::MinutesAlwaysLeadingZero);

    textinfo::draw(Slot::Left, totals_x_pos, draw::WHITE, true, "Totals:%s (%d)", total_time_buf,
                   total_deaths);
}

void disp() {
    if (storytimer::should_not_display_timer_at_all()) {
        return;
    }

    if (pref::get(pref::Pref::ShowRunBreakdown) && goal::is_run_complete()) {
        draw_breakdown_screen();
    }
}

}  // namespace storybreakdown