#pragma once

#include <mkb.h>
#include "heap.h"
#include "log.h"

namespace patch {

void clear_dc_ic_cache(void* ptr, u32 size);

// These return the overwritten word
u32 write_branch(void* ptr, void* destination);
u32 write_branch_bl(void* ptr, void* destination);
u32 write_blr(void* ptr);
u32 write_branch_main(void* ptr, void* destination, u32 branch);
u32 write_word(void* ptr, u32 data);
u32 write_nop(void* ptr);

void* hook_function_internal(void* function, void* destination);

template <typename Func, typename Dest>
Func hook_function(Func function, Dest destination) {
    return reinterpret_cast<Func>(
        hook_function_internal(reinterpret_cast<void*>(function),
                               reinterpret_cast<void*>(static_cast<Func>(destination))));
}

}  // namespace patch