#pragma once

#include <mkb.h>

namespace main {

extern "C" {

// Assembly overwrite functions
void custom_titlescreen_text_color();
void full_debug_text_color();

// main.cpp
void start_main_loop_assembly();

// Functions accessed by assembly overwrites
// main.cpp
void tick();

// Full color to draw debug text in, if not zero
// Normally, the debug text drawing function uses a single byte for the color in the form RRGGBBAA
extern mkb::GXColor debug_text_color;

extern bool debug_mode_enabled;

// IwMenu
extern u32 currently_playing_iw;
void stage_select_menu_hook();
void pause_menu_text_hook();
bool is_iw_complete();
}

}  // namespace main