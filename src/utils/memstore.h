#pragma once

#include "utils/base.h"

typedef struct {
    void *buf;
    u32 pos;
    u32 size;
} Store;

typedef void (*StoreFunc)(Store *store, void *ptr, u32 size);

void store_compute_size(Store *store, void *ptr, u32 size);
void store_save(Store *store, void *ptr, u32 size);
void store_load(Store *store, void *ptr, u32 size);
