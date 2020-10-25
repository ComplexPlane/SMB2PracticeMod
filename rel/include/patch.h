#pragma once

#include <cstdint>

namespace patch
{

void clear_dc_ic_cache(void *ptr, uint32_t size);
void write_branch(void *ptr, void *destination);
void write_branch_bl(void *ptr, void *destination);
void write_blr(void *ptr);
void write_branch_main(void *ptr, void *destination, uint32_t branch);
void write_word(void *ptr, uint32_t data);
void write_nop(void *ptr);
uint32_t *new_trampoline();

template<typename Func, typename Dest>
Func hook_function(Func function, Dest destination)
{
    uint32_t *instructions = reinterpret_cast<uint32_t *>(function);

    uint32_t *trampoline = new_trampoline();

    // Original instruction
    trampoline[0] = instructions[0];
    clear_dc_ic_cache(&trampoline[0], sizeof(uint32_t));

    // Branch to original function past hook
    write_branch(&trampoline[1], &instructions[1]);

    // Write actual hook
    write_branch(&instructions[0], reinterpret_cast<void *>(static_cast<Func>(destination)));

    return reinterpret_cast<Func>(trampoline);
}

}