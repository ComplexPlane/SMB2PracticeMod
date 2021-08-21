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

template <typename Func, typename Dest>
Func hook_function(Func function, Dest destination) {
    u32* instructions = reinterpret_cast<u32*>(function);

    u32* trampoline = static_cast<u32*>(heap::alloc_from_heap(8));
    MOD_ASSERT(trampoline != nullptr);

    // Original instruction
    trampoline[0] = instructions[0];
    clear_dc_ic_cache(&trampoline[0], sizeof(u32));

    // Branch to original function past hook
    write_branch(&trampoline[1], &instructions[1]);

    // Write actual hook
    write_branch(&instructions[0], reinterpret_cast<void*>(static_cast<Func>(destination)));

    return reinterpret_cast<Func>(trampoline);
}

}  // namespace patch