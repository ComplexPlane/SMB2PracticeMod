#pragma once

#include <gc/gc.h>
#include <mkb/dip.h>
#include <mkb/mathtypes.h>
#include <mkb/stagedef.h>
#include <mkb/stage.h>
#include <mkb/ball.h>
#include <mkb/tickable.h>
#include <mkb/item.h>
#include <mkb/effect.h>
#include <mkb/stobj.h>
#include <mkb/mode.h>
#include <mkb/sprite.h>
#include <mkb/minimap.h>
#include <mkb/story.h>

#include <cstdint>

namespace mkb
{

enum Status
{
    STAT_NULL = 0,
    STAT_INIT = 1,
    STAT_NORMAL = 2,
    STAT_DEST = 3,
    STAT_FREEZE = 4,
    STAT_INIT_FIRST = 5,
    STAT_GOAL_INIT = 6,
    STAT_GOAL = 7,
    STAT_RINGOUT_INIT = 8,
    STAT_RINGOUT = 9,
    STAT_MINI_MODE_0_INIT = 10,
    STAT_MINI_MODE_0 = 11,
};

struct GraphicsInfo
{
    void *activeFramebuffer; /* A pointer to either framebuffer1 or framebuffer2 */
    void *framebuffer1;
    void *framebuffer2;
    uint32_t frameCount;
    uint32_t activeFramebufferIndex; /* Either 0 or 1, representing framebuffer1 or framebuffer2 */
    gc::GXFifoObj *fifos[2];
};

extern "C" {

extern uint32_t padButtonBitfield;
extern uint32_t padAnalogBitfield;

extern gc::GXColor debugTextBgColor;
extern GraphicsInfo *graphicsInfo;

extern gc::OSHeapHandle mainHeap, stageHeap, bgHeap, charHeap, replayHeap;
extern uint32_t mainHeapSize, stageHeapSize, bgHeapSize, charHeapSize, replayHeapSize;
extern uint32_t gSomethingWithGameHeaps;

extern uint16_t stageTimer;

void drawDebugText();
void drawDebugTextBg(double x1, double y1, double x2, double y2);
void drawDebugTextCharEn(uint32_t x, uint32_t y, char ch, uint32_t color);

void GXSetZModeIfDifferent(gc::GXBool compare_enable, gc::GXCompare func, gc::GXBool update_enable);
void GXLoadTexObjIfDifferent(gc::GXTexObj *obj, gc::GXTexMapID id);

void eventActivateSound();
void eventTickSound();
void eventDeactivateSound();

void createGameHeaps(int param1);

}

}