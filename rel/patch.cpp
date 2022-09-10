#include "patch.h"

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

void hook_function_internal(void* func, void* dest) {
    // Branch directly to the destination function from the original function,
    // leaving no option to call the original function
    u32* instructions = static_cast<u32*>(func);
    write_branch(&instructions[0], dest);
}

void hook_function_internal(void* func, void* dest, u32* tramp_instrs, void** tramp_dest) {
    u32* func_instrs = static_cast<u32*>(func);

    constexpr u32 B_OPCODE_MASK = 0xFC000000;
    constexpr u32 B_OPCODE = 0x48000000;
    constexpr u32 B_DEST_MASK = 0x03FFFFFC;

    if ((func_instrs[0] & B_OPCODE_MASK) == B_OPCODE) {
        // Func has been hooked already, chain the hooks

        // Compute dest currently branched to
        u32 old_dest_offset = func_instrs[0] & B_DEST_MASK;
        // Sign extend to make it actually a s32
        if (old_dest_offset & (0x02000000)) {
            old_dest_offset |= 0xFC000000;
        }
        u32 old_dest = reinterpret_cast<u32>(func) + old_dest_offset;

        // Hook to our new func instead
        write_branch(&func_instrs[0], dest);

        // Use the old hooked func as the trampoline dest
        *tramp_dest = reinterpret_cast<void*>(old_dest);

    } else {
        // Func has not been hooked yet

        // Original instruction
        tramp_instrs[0] = func_instrs[0];
        clear_dc_ic_cache(tramp_instrs, sizeof(u32));

        // Branch to original func past hook
        write_branch(&tramp_instrs[1], &func_instrs[1]);

        // The function pointer to run as the original function is the addr of the trampoline
        // instructions array
        *tramp_dest = tramp_instrs;

        // Write actual hook
        write_branch(&func_instrs[0], dest);
    }
}

}  // namespace patch
