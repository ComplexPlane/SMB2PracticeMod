#pragma once

#include <cstdint>

namespace memstore
{

class MemStore
{
public:
    MemStore() noexcept;
    ~MemStore();

    MemStore(const MemStore& memStore) = delete;
    MemStore(MemStore&& memStore) = delete;
    MemStore& operator=(const MemStore& memStore) = delete;
    MemStore& operator=(MemStore&& memStore) = delete;

    void enterPreallocMode();

    // Allocate save buffers on the heap, and subsequent calls to `doRegion()` actually save a memory region.
    // Returns whether the heap allocation was successful
    [[nodiscard]] bool enterSaveMode();

    // Subsequent calls to `doRegion()` will cause the region to be restored, rather than saved or prealloc'd
    void enterLoadMode();

    // In PREALLOC mode, this is used to size the buffer
    // In SAVE mode, this copies the region into the buffer
    // In LOAD mode, this copies the region from the buffer to the ptr
    void doRegion(void *ptr, uint32_t size);

    void printStats() const;

private:
    enum class Mode
    {
        PREALLOC,
        SAVE,
        LOAD,
    };

    Mode m_mode;

    uint8_t *m_saveBuf;
    uint32_t m_saveBufIdx;
    uint32_t m_saveBufLen;
};

}