#pragma once

#include "mkb/mkb.h"

namespace patch {

void clear_dc_ic_cache(void *ptr, u32 size);

// These return the overwritten word
u32 write_branch(void *ptr, void *destination);
u32 write_branch_bl(void *ptr, void *destination);
u32 write_blr(void *ptr);
u32 write_branch_main(void *ptr, void *destination, u32 branch);
u32 write_word(void *ptr, u32 data);
u32 write_nop(void *ptr);

void hook_function_replace(void *function, void *dest);
void hook_function_tramp(void *func, void *dest, u32 *tramp_instrs, void **tramp_dest);

template <auto F>
class Tramp;

template <typename Ret, typename... Args, Ret (*F)(Args...)>
class Tramp<F> {
   public:
    using Func = Ret (*)(Args...);

    template <typename Dest>
    constexpr Tramp(Dest dest) : m_dest(static_cast<Func>(dest)) {
    }

    // Perform the hook. Afterwards, chain() calls the original hooked function
    void hook() {
        hook_function_tramp(reinterpret_cast<void *>(F), reinterpret_cast<void *>(m_dest), m_instrs,
                            reinterpret_cast<void **>(&m_chain));
    }

    // Call the original hooked function
    Ret chain(Args... args) const {
        return m_chain(args...);
    }

   private:
    Func m_dest;
    Func m_chain = nullptr;
    u32 m_instrs[2];  // Overwritten instruction and branch to original hooked function
};

}  // namespace patch
