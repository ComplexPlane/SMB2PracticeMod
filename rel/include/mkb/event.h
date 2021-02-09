#pragma once

namespace mkb
{

constexpr u32 NUM_EVENTS = 23;

enum Status
{ /* NULL, INIT, NORMAL, and DEST, and FREEZE seem to be the most common */
    STAT_DEST = 3,
    STAT_FREEZE = 4,
    STAT_GOAL = 7,
    STAT_GOAL_INIT = 6,
    STAT_INIT = 1,
    STAT_INIT_FIRST = 5,
    STAT_MINI_MODE_0 = 11,
    STAT_MINI_MODE_0_INIT = 10,
    STAT_NORMAL = 2,
    STAT_NULL = 0,
    STAT_RINGOUT = 9,
    STAT_RINGOUT_INIT = 8
};

enum EventID
{
    EVENT_ADX = 22,
    EVENT_APE = 3,
    EVENT_BACKGROUND = 20,
    EVENT_BALL = 2,
    EVENT_CAMERA = 16,
    EVENT_COMMEND = 12,
    EVENT_COURSE = 10,
    EVENT_EFFECT = 14,
    EVENT_INFO = 9,
    EVENT_ITEM = 5,
    EVENT_MINIMAP = 15,
    EVENT_MOUSE = 18,
    EVENT_NAME_ENTRY = 8,
    EVENT_NONE = -1,
    EVENT_OBJ_COLLISION = 7,
    EVENT_RECPLAY = 6,
    EVENT_REND_EFC = 21,
    EVENT_SOUND = 19,
    EVENT_SPRITE = 17,
    EVENT_STAGE = 0,
    EVENT_STOBJ = 4,
    EVENT_VIBRATION = 11,
    EVENT_VIEW = 13,
    EVENT_WORLD = 1
};

struct Event
{ /* A subset of continuously running game functionality that can be turned on or off independent of other Events */
    Status status;
    char *name; /* The name of the event */
    void (*init_func)(void); /* Normally, called once when status is STAT_INIT, then status is set to STAT_NORMAL */
    void (*tick_func)(void); /* Normally, called every frame if status is STAT_NORMAL */
    void (*dest_func)(void); /* "destruct:" or "destroy" function; normally, called once when status is STAT_DEST, then status is set to STAT_NULL */
    u32 tick_time; /* For performance metrics (presumably this is what's seen in the debug overlay performance > event menu) */
};

extern "C"
{
extern Event events[NUM_EVENTS];
void event_init(EventID event);
}

}