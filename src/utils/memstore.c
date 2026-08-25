#include "utils/memstore.h"
#include "utils/log.h"

void Store_DoPass(Store *store, void *ptr, u32 size) {
    switch (store->state) {
        case Store_State_CalcSize: {
            store->size += size;
            break;
        }
        case Store_State_Save: {
            ASSERT((store->pos + size) <= store->size);
            mkb_memcpy((u8 *)store->buf + store->pos, ptr, size);
            store->pos += size;
            break;
        }
        case Store_State_Load: {
            ASSERT((store->pos + size) <= store->size);
            mkb_memcpy(ptr, (u8 *)store->buf + store->pos, size);
            store->pos += size;
            break;
        }
    }
}
