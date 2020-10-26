#pragma once

#include <gc/mathtypes.h>

namespace patch
{

void clear_dc_ic_cache(void *ptr, u32 size);
void write_branch(void *ptr, void *destination);
void write_branch_bl(void *ptr, void *destination);
void write_blr(void *ptr);
void write_branch_main(void *ptr, void *destination, u32 branch);
void write_word(void *ptr, u32 data);
void write_nop(void *ptr);
u32 *new_trampoline();

template<typename Func, typename Dest>
Func hook_function(Func function, Dest destination)
{
    u32 *instructions = reinterpret_cast<u32 *>(function);

    u32 *trampoline = new_trampoline();

    // Original instruction
    trampoline[0] = instructions[0];
    clear_dc_ic_cache(&trampoline[0], sizeof(u32));

    // Branch to original function past hook
    write_branch(&trampoline[1], &instructions[1]);

    // Write actual hook
    write_branch(&instructions[0], reinterpret_cast<void *>(static_cast<Func>(destination)));

    return reinterpret_cast<Func>(trampoline);
}

}