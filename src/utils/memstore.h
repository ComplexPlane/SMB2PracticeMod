#pragma once

#include "utils/base.h"

typedef struct Store Store;
struct Store {
    void *buf;
    u32 pos;
    u32 size;
};

typedef void (*StoreFunc)(Store *store, void *ptr, u32 size);

void store_compute_size(Store *store, void *ptr, u32 size);
void store_save(Store *store, void *ptr, u32 size);
void store_load(Store *store, void *ptr, u32 size);
