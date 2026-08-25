#pragma once

#include "utils/base.h"

typedef enum {
    Store_State_CalcSize,
    Store_State_Save,
    Store_State_Load,
} Store_State;

typedef struct {
    Store_State state;
    void *buf;
    u32 pos;
    u32 size;
} Store;

void Store_DoPass(Store *store, void *ptr, u32 size);
