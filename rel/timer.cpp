#include "timer.h"

#include "draw.h"

#include <mkb/mkb.h>

namespace timer
{

static uint32_t s_retrace_count;
static uint32_t s_prev_retrace_count;
static int s_rta_timer;

void init()
{
    s_retrace_count = gc::VIGetRetraceCount();
}

void tick() {}

static void convert_frame_time(int frames, int *sec, int *centisec)
{
    if (sec) *sec = frames / 60;
    if (centisec)
    {
        *centisec = (frames % 60) * 100 / 60;
        if (*centisec < 0) *centisec = -*centisec;
    }
}

// Need to do logic in disp() so that we can know the game state _after_ the frame has been process
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

    int sec = 0, centisec = 0;
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

void save_state(memstore::MemStore *store)
{
    store->do_region(&s_retrace_count, sizeof(s_retrace_count));
    store->do_region(&s_prev_retrace_count, sizeof(s_prev_retrace_count));
    store->do_region(&s_rta_timer, sizeof(s_rta_timer));
    if (store->get_mode() == memstore::Mode::LOAD)
    {
        // This might not be completely consistently correct
        uint32_t count = gc::VIGetRetraceCount();
        s_prev_retrace_count = count - 1;
        s_retrace_count = count - 1;
    }
}

}