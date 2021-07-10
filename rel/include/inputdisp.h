#pragma once

#include <mkb.h>

namespace inputdisp
{

enum class Color
{
   Purple,
   Red,
   Orange,
   Yellow,
   Green,
   Blue,
   Pink,
   Black,
};

static constexpr u32 NUM_COLORS = 8;

void init();
void tick();
void disp();
void set_visible(bool visible);
bool is_visible();

void set_in_center_loc(bool alternate_loc);
bool is_in_center_loc();

Color get_color();
void set_color(Color color);

}
