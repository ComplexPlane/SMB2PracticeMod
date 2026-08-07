#pragma once

#include "mkb/mkb.h"

#define TRAMP(name, s, d)      \
    struct {                   \
        typeof(&(s)) src;      \
        typeof(&(s)) override; \
        typeof(&(s)) chain;    \
        u32 instrs[2];         \
    } name = {.src = (&s), .override = (&d)}

#define HOOK_TRAMP(tramp)                                                                          \
    ({                                                                                             \
        typeof(&tramp) _tramp_ = (&tramp);                                                         \
        patch::hook_function_tramp((void*)_tramp_->src, (void*)_tramp_->override, _tramp_->instrs, \
                                   (void**)&_tramp_->chain);                                       \
    })

namespace patch {

void clear_dc_ic_cache(void* ptr, u32 size);

// These return the overwritten word
u32 write_branch(void* ptr, void* destination);
u32 write_branch_bl(void* ptr, void* destination);
u32 write_blr(void* ptr);
u32 write_branch_main(void* ptr, void* destination, u32 branch);
u32 write_word(void* ptr, u32 data);
u32 write_nop(void* ptr);

void mook_function_replace(void* function, void* dest);
void hook_function_tramp(void* func, void* dest, u32* tramp_instrs, void** tramp_dest);

}  // namespace patch
