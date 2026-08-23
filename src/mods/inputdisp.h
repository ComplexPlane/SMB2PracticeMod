#pragma once

#include "utils/base.h"

constexpr u32 INPUTDISP_NUM_COLORS = 8;

void InputDisp_Init();
void InputDisp_OnPadRead(mkb_PADStatus *statuses);
void InputDisp_Tick();
void InputDisp_Disp();
