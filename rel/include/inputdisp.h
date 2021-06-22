#pragma once

namespace inputdisp
{
void init();
void tick();
void disp();
void set_visible(bool visible);
bool is_visible();

void set_in_center_loc(bool alternate_loc);
bool is_in_center_loc();
}
