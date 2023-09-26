#pragma once

#include "mkb/mkb.h"

namespace patch {

void clear_dc_ic_cache(void* ptr, u32 size);

// These return the overwritten word
u32 write_branch(void* ptr, void* destination);
u32 write_branch_bl(void* ptr, void* destination);
u32 write_blr(void* ptr);
u32 write_branch_main(void* ptr, void* destination, u32 branch);
u32 write_word(void* ptr, u32 data);
u32 read_word(void* ptr);
u32 write_nop(void* ptr);

template <typename T>
struct Tramp {
    u32 instrs[2];  // Overwritten instruction and branch to original hooked function
    T dest;         // Call this function to call the original hooked function
};

void hook_function_internal(void* function, void* dest);
void hook_function_internal(void* func, void* dest, u32* tramp_instrs, void** tramp_dest);

/**
 * Run function `dest` in place of function `func`.
 *
 * Use this version if you don't wish to call the original hooked function again.
 */
template <typename Func, typename Dest>
void hook_function(Func func, Dest dest) {
    // static_cast is to cast captureless lambda to function pointer and check that function types
    // are compatible
    hook_function_internal(reinterpret_cast<void*>(func),
                           reinterpret_cast<void*>(static_cast<Func>(dest)));
}

/**
 * Run function `dest` in place of function `func`, and fill out the given trampoline
 * such that the original function `func` may still be called.
 */
template <typename TrampFunc, typename Func, typename Dest>
void hook_function(Tramp<TrampFunc>& tramp, Func func, Dest dest) {
    // static_cast is to cast captureless lambda to function pointer and check that function types
    // are compatible
    hook_function_internal(reinterpret_cast<void*>(static_cast<TrampFunc>(func)),
                           reinterpret_cast<void*>(static_cast<TrampFunc>(dest)), tramp.instrs,
                           reinterpret_cast<void**>(&tramp.dest));
}

}  // namespace patch
