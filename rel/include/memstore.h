#pragma once

#include <mkb.h>

namespace memstore
{

enum class Mode
{
    PREALLOC,
    SAVE,
    LOAD,
};

class MemStore
{
public:

    MemStore() noexcept;
    ~MemStore();

    MemStore(const MemStore& store) = delete;
    MemStore(MemStore&& store) = delete;
    MemStore& operator=(const MemStore& store) = delete;
    MemStore& operator=(MemStore&& store) = delete;

    void enter_prealloc_mode();

    // Allocate save buffers on the heap, and subsequent calls to `do_region()` actually save a memory region.
    // Returns whether the heap allocation was successful
    [[nodiscard]] bool enter_save_mode();

    // Subsequent calls to `do_region()` will cause the region to be restored, rather than saved or prealloc'd
    void enter_load_mode();

    // In PREALLOC mode, this is used to size the buffer
    // In SAVE mode, this copies the region into the buffer
    // In LOAD mode, this copies the region from the buffer to the ptr
    void do_region(void *ptr, u32 size);

    void print_stats() const;
    Mode get_mode() const;

private:
    Mode m_mode;

    u8 *m_save_buf;
    u32 m_save_buf_idx;
    u32 m_save_buf_len;
};

}