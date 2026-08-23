#include "utils/memstore.h"

void store_compute_size(Store *store, void *ptr, u32 size) {
    store->size += size;
}

void store_save(Store *store, void *ptr, u32 size) {
    mkb_memcpy((u8 *)store->buf + store->pos, ptr, size);
    store->pos += size;
}

void store_load(Store *store, void *ptr, u32 size) {
    mkb_memcpy(ptr, (u8 *)store->buf + store->pos, size);
    store->pos += size;
}
