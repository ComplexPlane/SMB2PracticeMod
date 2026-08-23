#include "utils/memstore.h"

void Store_ComputeSize(Store *store, void *ptr, u32 size) {
    store->size += size;
}

void Store_Save(Store *store, void *ptr, u32 size) {
    mkb_memcpy((u8 *)store->buf + store->pos, ptr, size);
    store->pos += size;
}

void Store_Load(Store *store, void *ptr, u32 size) {
    mkb_memcpy(ptr, (u8 *)store->buf + store->pos, size);
    store->pos += size;
}
