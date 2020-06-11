#pragma once

#include <cstdint>

namespace memsave
{

class MemSave
{
public:
    MemSave();
    ~MemSave();

    // Allocate save buffers on the heap, and subsequent calls to `saveRegion()` actually save a memory region.
    // Returns whether the heap allocation was successful
    bool enterSaveStateMode();

    // Initially, calls to `saveRegion()` are just used to precompute buffer sizes to allocate on the heap.
    // But after `enterSaveStateMode()`, calls will actually save the regions
    void saveRegion(void *ptr, uint32_t size);
    void loadState();

    void printStats();
    void clear();

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
};

}