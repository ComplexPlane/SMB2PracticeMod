#pragma once

#include "mkb/mkb.h"

namespace store {

enum class State {
    CalcSize,
    Save,
    Load,
};

struct Store {
    State state;
    void *buf;
    u32 pos;
    u32 size;
};

void do_pass(Store *store, void *ptr, u32 size);

}  // namespace store
