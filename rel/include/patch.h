#pragma once

#include <cstdint>

namespace patch
{

void clear_DC_IC_Cache(void *ptr, uint32_t size);
void writeBranch(void *ptr, void *destination);
void writeBranchBL(void *ptr, void *destination);
void writeBLR(void *ptr);
void writeBranchMain(void *ptr, void *destination, uint32_t branch);
void writeWord(void *ptr, uint32_t data);
void writeNop(void *ptr);
uint32_t *newTrampoline();

template<typename Func, typename Dest>
Func hookFunction(Func function, Dest destination)
{
    uint32_t *instructions = reinterpret_cast<uint32_t *>(function);

    uint32_t *trampoline = newTrampoline();

    // Original instruction
    trampoline[0] = instructions[0];
    clear_DC_IC_Cache(&trampoline[0], sizeof(uint32_t));

    // Branch to original function past hook
    writeBranch(&trampoline[1], &instructions[1]);

    // Write actual hook
    writeBranch(&instructions[0], reinterpret_cast<void *>(static_cast<Func>(destination)));

    return reinterpret_cast<Func>(trampoline);
}

}