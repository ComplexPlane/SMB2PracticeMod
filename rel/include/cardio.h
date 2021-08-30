#pragma once

#include <mkb.h>

namespace cardio {

void tick();

// Caller gets a heap-allocated buffer containing file, which they own
mkb::CARDResult read_file(const char* file_name, void** out_buf);

// Writes asynchronously
void write_file(const char* file_name, const void* buf, u32 buf_size,
                void (*callback)(mkb::CARDResult));

}  // namespace cardio