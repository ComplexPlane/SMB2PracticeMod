#include "memstore.h"

namespace store {

void compute_size(Store *store, void *ptr, u32 size) {
    store->size += size;
}

void save(Store *store, void *ptr, u32 size) {
    mkb::memcpy((u8 *)store->buf + store->pos, ptr, size);
    store->pos += size;
}

void load(Store *store, void *ptr, u32 size) {
    mkb::memcpy(ptr, (u8 *)store->buf + store->pos, size);
    store->pos += size;
}

}  // namespace store
