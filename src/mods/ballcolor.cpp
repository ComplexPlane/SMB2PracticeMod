#include "ballcolor.h"

#include "mkb/mkb.h"

#include "systems/heap.h"
#include "utils/memstore.h"
#include "systems/pad.h"
#include "systems/pref.h"

#include "utils/patch.h"
#include "utils/draw.h"

namespace ballcolor {

static u8 convert_to_ball_color_id(u8 color_choice) {
    if (color_choice == 0) {
        return 3;
    }
    return color_choice - 1;
}

static u8 convert_to_ape_color_id(u8 color_choice) {
    if (color_choice == 0) {
        return 0;
    }
    return color_choice - 1;
}

void tick() {
    // Set ball & ape color to color ID
    if (mkb::balls[mkb::curr_player_idx].ape != nullptr) {  // Check for nullpointers first
        mkb::balls[mkb::curr_player_idx].ape->color_index =
            convert_to_ape_color_id(pref::get(pref::U8Pref::ApeColor));
        mkb::balls[mkb::curr_player_idx].g_ball_color_index =
            convert_to_ball_color_id(pref::get(pref::U8Pref::BallColor));
    }
}

}  // namespace ballcolor
