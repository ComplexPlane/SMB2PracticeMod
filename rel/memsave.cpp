#include "memsave.h"

namespace memsave
{

MemSave::MemSave() :
    m_saveBuf{nullptr},
    m_regionCount{0},
    m_schema{nullptr},
    m_preallocMode{true} {}

MemSave::~MemSave()
{
    clear();
}

void MemSave::enterPreallocMode()
{
    clear();
    m_preallocMode = true;
}

bool MemSave::allocSaveBuf()
{
    return false;
}

void MemSave::enterSaveStateMode()
{
    m_preallocMode = false;
}

void MemSave::saveRegion(void *ptr, uint32_t size)
{
    if (m_preallocMode)
    {
        m_saveBufPos += size;
        m_regionCount++;
    }
}

void MemSave::loadState()
{

}

void MemSave::clear()
{
    if (m_saveBuf)
    {
        delete[] m_saveBuf;
        m_saveBuf = 0;
    }
    if (m_schema)
    {
        delete[] m_schema;
        m_schema = 0;
    }
    m_regionCount = 0;
}

}