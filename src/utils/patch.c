#include "utils/patch.h"

void Patch_ClearDCICCache(void *ptr, u32 size) {
    mkb_DCFlushRange(ptr, size);
    mkb_ICInvalidateRange(ptr, size);
}

u32 Patch_WriteBranch(void *ptr, void *destination) {
    u32 branch = 0x48000000;  // b
    return Patch_WriteBranchMain(ptr, destination, branch);
}

u32 Patch_WriteBranchBL(void *ptr, void *destination) {
    u32 branch = 0x48000001;  // bl
    return Patch_WriteBranchMain(ptr, destination, branch);
}

u32 Patch_WriteBLR(void *ptr) {
    return Patch_WriteWord(ptr, 0x4e800020);
}

u32 Patch_WriteBranchMain(void *ptr, void *destination, u32 branch) {
    u32 delta = (u32)destination - (u32)ptr;

    branch |= (delta & 0x03FFFFFC);

    u32 *p = (u32 *)ptr;
    u32 orig_word = *p;
    *p = branch;

    Patch_ClearDCICCache(ptr, sizeof(u32));

    return orig_word;
}

u32 Patch_WriteWord(void *ptr, u32 data) {
    u32 *p = (u32 *)ptr;
    u32 orig_word = *p;
    *p = data;
    Patch_ClearDCICCache(ptr, sizeof(u32));

    return orig_word;
}

u32 Patch_WriteNop(void *ptr) {
    return Patch_WriteWord(ptr, 0x60000000);
}

void Patch_HookFunctionReplace(void *func, void *dest) {
    // Branch directly to the destination function from the original function,
    // leaving no option to call the original function
    u32 *instructions = (u32 *)func;
    Patch_WriteBranch(&instructions[0], dest);
}

void Patch_HookFunctionTramp(void *func, void *dest, u32 *tramp_instrs, void **tramp_dest) {
    u32 *func_instrs = (u32 *)func;

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
        u32 old_dest = (u32)func + old_dest_offset;

        // Hook to our new func instead
        Patch_WriteBranch(&func_instrs[0], dest);

        // Use the old hooked func as the trampoline dest
        *tramp_dest = (void *)old_dest;

    } else {
        // Func has not been hooked yet

        // Original instruction
        tramp_instrs[0] = func_instrs[0];
        Patch_ClearDCICCache(tramp_instrs, sizeof(u32));

        // Branch to original func past hook
        Patch_WriteBranch(&tramp_instrs[1], &func_instrs[1]);

        // The function pointer to run as the original function is the addr of the trampoline
        // instructions array
        *tramp_dest = tramp_instrs;

        // Write actual hook
        Patch_WriteBranch(&func_instrs[0], dest);
    }
}
