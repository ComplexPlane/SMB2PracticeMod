#pragma once
#include "utils/base.h"
constexpr u32 inputdisp_NUM_COLORS = 8;
void inputdisp_init(void); void inputdisp_on_PADRead(mkb_PADStatus *statuses); void inputdisp_tick(void); void inputdisp_disp(void);
