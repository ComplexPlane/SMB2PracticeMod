#include "utils/memstore.h"

void Store_DoPass(Store *store, void *ptr, u32 size) {
    switch (store->state) {
        case Store_State_CalcSize: {
            store->n += size;
            break;
        }
        case Store_State_Save: {
            mkb_memcpy((u8 *)store->buf + store->n, ptr, size);
            store->n += size;
            break;
        }
        case Store_State_Load: {
            mkb_memcpy(ptr, (u8 *)store->buf + store->n, size);
            store->n += size;
            break;
        }
    }
}
