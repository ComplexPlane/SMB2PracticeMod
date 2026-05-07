#include "mkb/mkb.h"
#include "systems/heap.h"

struct Store {
    void* buf;
    u32 pos;
    u32 size;
};

#define MAX_SLOTS 8

static Store s_slots[MAX_SLOTS];

void StoreComputeSize(Store *store, void *ptr, u32 size) {
    store->size += size;
}

void StoreSave(Store *store, void *ptr, u32 size) {
    mkb::memcpy((u8 *)store->buf + store->pos, ptr, size);
    store->pos += size;
}

void StoreLoad(Store *store, void *ptr, u32 size) {
    mkb::memcpy(ptr, (u8 *)store->buf + store->pos, size);
    store->pos += size;
}

typedef void (*StoreFunc)(Store *store, void *ptr, u32 size);

void PassOverRegions(StoreFunc f, Store *store) {
    f(store, &mkb::balls[0], sizeof(mkb::balls[0]));
    f(store, &mkb::sub_mode, sizeof(mkb::sub_mode));
    f(store, &mkb::mode_info.stage_time_frames_remaining, 
            sizeof(mkb::mode_info.stage_time_frames_remaining));
    f(store, reinterpret_cast<void*>(0x8054E03C), 0xe0);  // Camera region
    f(store, reinterpret_cast<void*>(0x805BD830), 0x1c);  // Some physics region
    f(store, &mkb::mode_info.g_ball_mode, sizeof(mkb::mode_info.g_ball_mode));
    f(store, mkb::g_camera_standstill_counters, sizeof(mkb::g_camera_standstill_counters));
}

enum SavestSlot {
    SS_Slot0,
    SS_Slot1,
    SS_Slot2,
    SS_Slot3,
    SS_Slot4,
    SS_Slot5,
    SS_Slot6,
    SS_Slot7,
};

LSS_SaveError libsavest_save(LSS_Slot slot) {
    Store store;
    PassOverRegions(StoreComputeSize, &store);
    store.buf = heap::alloc(state->store.size);
    if (store.buf == nullptr) {
        return;
    }

    PassOverRegions(StoreSave, &store);
    store.pos = 0; // Ready to load
    if (state->store.buf != nullptr) {
        heap::free(state->store.buf);
    }
    state->store = store;
}
