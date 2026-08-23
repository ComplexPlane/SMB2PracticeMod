#pragma once

#include "utils/base.h"

#define TRAMP(name, s, d)      \
    static struct {            \
        typeof(&(s)) src;      \
        typeof(&(s)) override; \
        typeof(&(s)) chain;    \
        u32 instrs[2];         \
    } name = {.src = (&s), .override = (&d)}

#define HOOK_TRAMP(tramp)                                                                         \
    ({                                                                                            \
        typeof(&(tramp)) _tramp_ = (&(tramp));                                                    \
        Patch__HookFunctionTramp((void *)_tramp_->src, (void *)_tramp_->override, _tramp_->instrs, \
                                (void **)&_tramp_->chain);                                        \
    })

// These return the overwritten word
u32 Patch_WriteB(void *ptr, void *destination);
u32 Patch_WriteBL(void *ptr, void *destination);
u32 Patch_WriteBLR(void *ptr);
u32 Patch_WriteBranchMain(void *ptr, void *destination, u32 branch);
u32 Patch_WriteWord(void *ptr, u32 data);
u32 Patch_WriteNop(void *ptr);

void Patch__HookFunctionReplace(void *function, void *dest);
void Patch__HookFunctionTramp(void *func, void *dest, u32 *tramp_instrs, void **tramp_dest);
