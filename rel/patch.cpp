#include "patch.h"

#include <mkb.h>

#include "log.h"
#include "pad.h"

namespace patch {

void clear_dc_ic_cache(void* ptr, u32 size) {
    mkb::DCFlushRange(ptr, size);
    mkb::ICInvalidateRange(ptr, size);
}

u32 write_branch(void* ptr, void* destination) {
    u32 branch = 0x48000000;  // b
    return write_branch_main(ptr, destination, branch);
}

u32 write_branch_bl(void* ptr, void* destination) {
    u32 branch = 0x48000001;  // bl
    return write_branch_main(ptr, destination, branch);
}

u32 write_blr(void* ptr) { return write_word(ptr, 0x4e800020); }

u32 write_branch_main(void* ptr, void* destination, u32 branch) {
    u32 delta = reinterpret_cast<u32>(destination) - reinterpret_cast<u32>(ptr);

    branch |= (delta & 0x03FFFFFC);

    u32* p = reinterpret_cast<u32*>(ptr);
    u32 orig_word = *p;
    *p = branch;

    clear_dc_ic_cache(ptr, sizeof(u32));

    return orig_word;
}

u32 write_word(void* ptr, u32 data) {
    u32* p = reinterpret_cast<u32*>(ptr);
    u32 orig_word = *p;
    *p = data;
    clear_dc_ic_cache(ptr, sizeof(u32));

    return orig_word;
}

u32 write_nop(void* ptr) { return write_word(ptr, 0x60000000); }

void* hook_function_internal(void* function, void* destination) {
    u32* instructions = static_cast<u32*>(function);

    u32* trampoline = static_cast<u32*>(heap::alloc_from_heap(8));
    MOD_ASSERT(trampoline != nullptr);

    // Original instruction
    trampoline[0] = instructions[0];
    clear_dc_ic_cache(&trampoline[0], sizeof(u32));

    // Branch to original function past hook
    write_branch(&trampoline[1], &instructions[1]);

    // Write actual hook
    write_branch(&instructions[0], destination);

    return trampoline;
}

}  // namespace patch