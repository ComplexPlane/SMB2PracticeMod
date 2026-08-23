#pragma once

#include "utils/base.h"

typedef struct {
    void *buf;
    u32 pos;
    u32 size;
} Store;

typedef void (*StoreFunc)(Store *store, void *ptr, u32 size);

void Store_ComputeSize(Store *store, void *ptr, u32 size);
void Store_Save(Store *store, void *ptr, u32 size);
void Store_Load(Store *store, void *ptr, u32 size);
