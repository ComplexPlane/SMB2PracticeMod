#include "memsave.h"
#include "log.h"

#include <cstring>

namespace memsave
{

MemSave::MemSave() :
    m_preallocMode{true},
    m_saveBuf{nullptr},
    m_regions{nullptr},
    m_saveBufIdx{0},
    m_regionsIdx{0},
    m_saveBufLen{0},
    m_regionsLen{0} {}

MemSave::~MemSave()
{
    clear();
}

bool MemSave::enterSaveStateMode()
{
    m_saveBuf = new uint8_t[m_saveBufIdx];
    m_regions = new RegionInfo[m_regionsIdx];
    if (!m_saveBuf || !m_regions) return false;

    m_saveBufLen = m_saveBufIdx;
    m_regionsLen = m_regionsIdx;
    m_saveBufIdx = 0;
    m_regionsIdx = 0;

    return true;
}

void MemSave::saveRegion(void *ptr, uint32_t size)
{
    if (m_preallocMode)
    {
        m_saveBufIdx += size;
        m_regionsIdx++;
    }
    else
    {
        memcpy(&m_saveBuf[m_saveBufIdx], ptr, size);
        m_saveBufIdx += size;

        m_regions[m_regionsIdx].ptr = ptr;
        m_regions[m_regionsIdx].size = size;
        m_regionsIdx++;
    }
}

void MemSave::loadState()
{
    MOD_ASSERT(!m_preallocMode);

    uint32_t bufPos = 0;
    for (uint32_t i = 0; i < m_regionsLen; i++)
    {
        memcpy(m_regions[i].ptr, &m_saveBuf[bufPos], m_regions[i].size);
        bufPos += m_regions[i].size;
    }
}

void MemSave::printStats()
{
    gc::OSReport("[mod] Memsave total size: %d bytes\n", m_saveBufLen + m_regionsLen * sizeof(RegionInfo));
    gc::OSReport("[mod] Memsave save buffer size: %d bytes\n", m_saveBufLen);
    gc::OSReport("[mod] Memsave region metadata size: %d bytes\n", m_regionsLen * sizeof(RegionInfo));
    gc::OSReport("[mod] Memsave region count: %d\n", m_regionsLen);
}

void MemSave::clear()
{
    if (m_saveBuf)
    {
        delete[] m_saveBuf;
        m_saveBuf = nullptr;
    }
    if (m_regions)
    {
        delete[] m_regions;
        m_regions = nullptr;
    }

    m_preallocMode = true;
    m_saveBufIdx = 0;
    m_regionsIdx = 0;
}

}