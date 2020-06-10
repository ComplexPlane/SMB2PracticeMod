#pragma once

#include <cstdint>

namespace memsave
{

class MemSave
{
public:
    MemSave();
    ~MemSave();

    void enterPreallocMode();
    bool allocSaveBuf();
    void enterSaveStateMode();

    void saveRegion(void *ptr, uint32_t size);
    void loadState();

private:
    struct RegionInfo
    {
        void *ptr;
        uint32_t size;
    };

    uint8_t *m_saveBuf;
    RegionInfo *m_schema; // List of region metadata
    uint32_t m_regionCount;
    bool m_preallocMode;
    uint32_t m_saveBufPos;

    void clear();

};

}