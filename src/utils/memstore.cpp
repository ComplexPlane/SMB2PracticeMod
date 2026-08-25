#include "memstore.h"

#include "systems/log.h"

namespace store {

void do_pass(Store *store, void *ptr, u32 size) {
    switch (store->state) {
        case State::CalcSize: {
            store->size += size;
            break;
        }
        case State::Save: {
            ASSERT((store->pos + size) <= store->size);
            mkb::memcpy((u8 *)store->buf + store->pos, ptr, size);
            store->pos += size;
            break;
        }
        case State::Load: {
            ASSERT((store->pos + size) <= store->size);
            mkb::memcpy(ptr, (u8 *)store->buf + store->pos, size);
            store->pos += size;
            break;
        }
    }
}

}  // namespace store
