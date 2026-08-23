#pragma once

#include "utils/base.h"

void Heap_Init();
void *Heap_Alloc(u32 size);
bool Heap_Free(void *ptr);
void Heap_CheckIntegrity();
u32 Heap_GetFreeSpace();
u32 Heap_GetTotalSpace();
