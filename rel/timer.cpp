#include "timer.h"

#include "draw.h"

#include <mkb/mkb.h>

namespace timer
{

// Enabled for the timer just means "is drawing on the screen"; it's always running
// regardless to prevent fudging IL RTA times
static bool s_enabled = false;
static bool s_ever_enabled = false;

static u32 s_retrace_count;
static u32 s_prev_retrace_count;
static s32 s_rta_timer;

void init()
{
    if (s_enabled) return;

    if (!s_ever_enabled) s_retrace_count = gc::VIGetRetraceCount();

    s_enabled = true;
    s_ever_enabled = true;
}

void tick() {}

static void convert_frame_time(s32 frames, s32 *sec, s32 *centisec)
{
    if (sec) *sec = frames / 60;
    if (centisec)
    {
        *centisec = (frames % 60) * 100 / 60;
        if (*centisec < 0) *centisec = -*centisec;
    }
}

// Need to do logic in disp() so that we can know the game state _after_ the frame has processed
void disp()
{
    if (mkb::main_mode != mkb::MD_GAME) return;

    switch (mkb::sub_mode)
    {
        case mkb::SMD_GAME_READY_INIT:
        case mkb::SMD_GAME_READY_MAIN:
        case mkb::SMD_GAME_PLAY_INIT:
        case mkb::SMD_GAME_PLAY_MAIN:
        case mkb::SMD_GAME_GOAL_INIT:
        case mkb::SMD_GAME_GOAL_MAIN:
        case mkb::SMD_GAME_RINGOUT_INIT:
        case mkb::SMD_GAME_RINGOUT_MAIN:
        case mkb::SMD_GAME_TIMEOVER_INIT:
        case mkb::SMD_GAME_TIMEOVER_MAIN:
            break;
        default:
            return;
    }

    s_prev_retrace_count = s_retrace_count;
    s_retrace_count = gc::VIGetRetraceCount();

    if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT)
    {
        s_rta_timer = mkb::stage_time_limit;
    }
    // This flag seems to correspond to when the timer should be running...
    // See 0x802974bc in memory
    else if ((mkb::ball_mode & 0x8u) == 0)
    {
        s_rta_timer -= s_retrace_count - s_prev_retrace_count;
//        if (s_rtaTimer < 0) s_rtaTimer = 0;
    }

    if (s_enabled)
    {
        s32 sec = 0, centisec = 0;
        convert_frame_time(s_rta_timer, &sec, &centisec);
        draw::debug_text(
            380, 34,
            draw::Color::WHITE,
            "RTA: %02d.%02d", sec, centisec);

        convert_frame_time(mkb::stage_time_frames_remaining - s_rta_timer, &sec, &centisec);
        draw::debug_text(
            380, 50,
            draw::Color::WHITE,
            "PAU: %02d.%02d", sec, centisec);
    }
}

void dest()
{
    s_enabled = false;
}

bool is_enabled()
{
    return s_enabled;
}

void save_state(memstore::MemStore *store)
{
    store->do_region(&s_retrace_count, sizeof(s_retrace_count));
    store->do_region(&s_prev_retrace_count, sizeof(s_prev_retrace_count));
    store->do_region(&s_rta_timer, sizeof(s_rta_timer));
    if (store->get_mode() == memstore::Mode::LOAD)
    {
        // This might not be completely consistently correct
        u32 count = gc::VIGetRetraceCount();
        s_prev_retrace_count = count - 1;
        s_retrace_count = count - 1;
    }
}

}