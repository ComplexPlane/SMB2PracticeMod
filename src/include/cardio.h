#pragma once

#include <mkb.h>

namespace cardio {

void init();
void tick();

// Caller gets a heap-allocated buffer containing file, which they own
// Synchronous at the moment. Also, do not call while write_file() is running!
mkb::CARDResult read_file(const char* file_name, void** out_buf);

// Writes asynchronously
void write_file(const char* file_name, const void* buf, u32 buf_size,
                void (*callback)(mkb::CARDResult));

}  // namespace cardio