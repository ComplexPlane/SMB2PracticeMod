#include "memstore.h"
#include "log.h"

#include <heap.h>
#include <cstring>

namespace memstore {

MemStore::MemStore()
    : m_mode{Mode::PREALLOC}, m_save_buf{nullptr}, m_save_buf_idx{0}, m_save_buf_len{0} {}

MemStore::~MemStore() { enter_prealloc_mode(); }

void MemStore::enter_prealloc_mode() {
    if (m_save_buf) {
        heap::free(m_save_buf);
        m_save_buf = nullptr;
    }

    m_mode = Mode::PREALLOC;
    m_save_buf_idx = 0;
}

bool MemStore::enter_save_mode() {
    // Allocate directly so a failed allocation (nullptr) can be detected
    // C++ new operator never returns nullptr
    m_save_buf = static_cast<u8*>(heap::alloc(m_save_buf_idx));
    if (!m_save_buf) return false;

    m_mode = Mode::SAVE;
    m_save_buf_len = m_save_buf_idx;
    m_save_buf_idx = 0;

    return true;
}

void MemStore::enter_load_mode() {
    MOD_ASSERT(m_mode != Mode::PREALLOC);
    MOD_ASSERT(m_save_buf_idx == m_save_buf_len);

    m_mode = Mode::LOAD;
    m_save_buf_idx = 0;
}

void MemStore::do_region(void* ptr, u32 size) {
    switch (m_mode) {
        case Mode::PREALLOC: {
            m_save_buf_idx += size;
            break;
        }
        case Mode::SAVE: {
            memcpy(&m_save_buf[m_save_buf_idx], ptr, size);
            m_save_buf_idx += size;
            break;
        }
        case Mode::LOAD: {
            memcpy(ptr, &m_save_buf[m_save_buf_idx], size);
            m_save_buf_idx += size;
            break;
        }
    }
}

void MemStore::print_stats() const {
    mkb::OSReport("[pracmod] MemStore total size: %d bytes\n", m_save_buf_len);
}

Mode MemStore::get_mode() const { return m_mode; }

}  // namespace memstore