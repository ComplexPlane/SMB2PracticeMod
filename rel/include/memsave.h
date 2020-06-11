#pragma once

#include <cstdint>

namespace memsave
{

class MemSave
{
public:
    MemSave();
    ~MemSave();

    // Calls to `saveRegion()` are just used to precompute buffer sizes to allocate on the heap
    void enterPreallocMode();

    // Allocate save buffers on the heap, and subsequent calls to `saveRegion()` actually save a memory region.
    // Returns whether the heap allocation was successful
    bool enterSaveStateMode();

    void saveRegion(void *ptr, uint32_t size);
    void loadState();

private:
    struct RegionInfo
    {
        void *ptr;
        uint32_t size;
    };

    bool m_preallocMode;

    uint8_t *m_saveBuf;
    RegionInfo *m_regions; // List of region metadata

    uint32_t m_saveBufIdx;
    uint32_t m_regionsIdx;
    uint32_t m_saveBufLen;
    uint32_t m_regionsLen;

    void clear();

};

}