#pragma once

#include <mkb.h>

namespace cardio {

enum class Result {
    Ok,
    ErrInsufficientSpace,
    ErrOther,
};

// Caller gets a heap-allocated buffer containing file, which they own
Result read_file(const char* file_name, void** out_buf);

Result write_file(const char* file_name, const void* buf, u32 buf_size);

}  // namespace cardio