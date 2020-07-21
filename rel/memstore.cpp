#include "memstore.h"
#include "log.h"

#include <cstring>

namespace memstore
{

MemStore::MemStore() :
    m_mode{Mode::PREALLOC},
    m_saveBuf{nullptr},
    m_saveBufIdx{0},
    m_saveBufLen{0} {}

MemStore::~MemStore()
{
    enterPreallocMode();
}

void MemStore::enterPreallocMode()
{
    if (m_saveBuf)
    {
        delete[] m_saveBuf;
        m_saveBuf = nullptr;
    }

    m_mode = Mode::PREALLOC;
    m_saveBufIdx = 0;
}


bool MemStore::enterSaveMode()
{
    m_saveBuf = new uint8_t[m_saveBufIdx];
    if (!m_saveBuf) return false;

    m_mode = Mode::SAVE;
    m_saveBufLen = m_saveBufIdx;
    m_saveBufIdx = 0;

    return true;
}

void MemStore::enterLoadMode()
{
    MOD_ASSERT(m_mode != Mode::PREALLOC);
    MOD_ASSERT(m_saveBufIdx == m_saveBufLen);

    m_mode = Mode::LOAD;
    m_saveBufIdx = 0;
}

void MemStore::doRegion(void *ptr, uint32_t size)
{
    switch (m_mode)
    {
        case Mode::PREALLOC:
        {
            m_saveBufIdx += size;
            break;
        }
        case Mode::SAVE:
        {
            memcpy(&m_saveBuf[m_saveBufIdx], ptr, size);
            m_saveBufIdx += size;
            break;
        }
        case Mode::LOAD:
        {
            memcpy(ptr, &m_saveBuf[m_saveBufIdx], size);
            m_saveBufIdx += size;
            break;
        }
    }
}

void MemStore::printStats() const
{
    gc::OSReport("[mod] MemStore total size: %d bytes\n", m_saveBufLen);
}

Mode MemStore::getMode() const
{
    return m_mode;
}

}