#pragma once

#include "mkb/mkb.h"

namespace store {

struct Store {
    void* buf;
    u32 pos;
    u32 size;
};

typedef void (*StoreFunc)(Store* store, void* ptr, u32 size);

void compute_size(Store* store, void* ptr, u32 size);
void save(Store* store, void* ptr, u32 size);
void load(Store* store, void* ptr, u32 size);

}  // namespace store
