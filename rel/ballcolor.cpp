#include "ballcolor.h"

#include <mkb.h>

#include <cstring>
#include "draw.h"
#include "heap.h"
#include "memstore.h"
#include "mkb2_ghidra.h"
#include "pad.h"
#include "patch.h"
#include "pref.h"
#include "timer.h"

namespace ballcolor {

static s32 ballColorID = 0;
static s32 apeColorID = 0;

static void convert_to_ballColorID(u8 color_choice){
    switch(color_choice){
        case 0: // Default
            ballColorID = 3;
            break;
        case 1: // Red
        case 2: // Blue
        case 3: // Yellow
        case 4: // Green
        case 5: // Teal
        case 6: // Pink
        case 7: // Black
        case 8: // White
            ballColorID = color_choice - 1;
            break;
    }
}

static void convert_to_apeColorID(u8 color_choice){
    switch(color_choice){
        case 0: // Default
            apeColorID = 0;
            break;
        case 1: // Red
        case 2: // Blue
        case 3: // Yellow
        case 4: // Green
        case 5: // Teal
        case 6: // Pink
        case 7: // Black
        case 8: // White
            apeColorID = color_choice - 1;
            break;
    }
}

static void set_color() { // Sets the ball color to colorID
    if(mkb::balls[mkb::curr_player_idx].ape != nullptr){ // Check for nullpointers first
        convert_to_ballColorID(pref::get_ball_color());
        convert_to_apeColorID(pref::get_ape_color());
        mkb::balls[mkb::curr_player_idx].ape->color_index = apeColorID;
        mkb::balls[mkb::curr_player_idx].g_ball_color_index = ballColorID;
    }
}

void tick() {
    set_color();
}

} // namespace ballcolor
