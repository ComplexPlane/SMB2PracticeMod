#pragma once

#include <gc/gc.h>
#include <mkb/dip.h>
#include <mkb/stagedef.h>
#include <mkb/stage.h>
#include <mkb/ball.h>
#include <mkb/pool.h>
#include <mkb/item.h>
#include <mkb/effect.h>
#include <mkb/stobj.h>
#include <mkb/mode.h>
#include <mkb/sprite.h>
#include <mkb/minimap.h>
#include <mkb/story.h>
#include <mkb/unlock.h>
#include <mkb/event.h>
#include <mkb/mathutil.h>

namespace mkb
{

struct GraphicsInfo
{
    void *active_framebuffer; /* A pointer to either framebuffer1 or framebuffer2 */
    void *framebuffer1;
    void *framebuffer2;
    u32 frame_count;
    u32 active_framebuffer_index; /* Either 0 or 1, representing framebuffer1 or framebuffer2 */
    gc::GXFifoObj *fifos[2];
};

extern "C" {

extern u32 filtered_button_inputs_bitfield;
extern u32 filtered_analog_inputs_bitfield;

extern gc::GXColor debug_text_bg_color;
extern GraphicsInfo *graphics_info;

extern gc::OSHeapHandle main_heap, stage_heap, bg_heap, char_heap, replay_heap;
extern u32 main_heap_size, stage_heap_size, bg_heap_size, char_heap_size, replay_heap_size;
extern u32 g_something_with_game_heaps;

extern u32 curr_player_idx;
extern u8 selected_characters[4];

void draw_debugtext();
void draw_debugtext_bg(double x1, double y1, double x2, double y2);
void draw_debugtext_char_en(u32 x, u32 y, char ch, u32 color);

void GXSetZMode_cached(gc::GXBool compare_enable, gc::GXCompare func, gc::GXBool update_enable);
void GXLoadTexObj_cached(gc::GXTexObj *obj, gc::GXTexMapID id);

}

}