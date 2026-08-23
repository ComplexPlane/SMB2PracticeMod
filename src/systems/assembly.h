#pragma once

#include "utils/base.h"

// Assembly overwrite functions
void asm_custom_titlescreen_text_color();

// main.cpp
void asm_start_main_loop_assembly();

// Functions accessed by assembly overwrites
// main.cpp
void main_tick();

// Full color to draw debug text in, if not zero
// Normally, the debug text drawing function uses a single byte for the color in the form RRGGBBAA
extern GXColor asm_debug_text_color;

// IwMenu
extern u32 asm_currently_playing_iw;
void asm_stage_select_menu_hook();
void asm_pause_menu_text_hook();
bool asm_is_iw_complete();
