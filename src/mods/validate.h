#pragma once

#include "utils/base.h"

void Validate_Run();
bool Validate_WasRunValid(bool mods_allowed);
void Validate_DisableInvalidatingSettings();
u32 Validate_GetFramesave();

void Validate_Init();
void Validate_Tick();
