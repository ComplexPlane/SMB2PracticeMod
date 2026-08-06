#pragma once

#include "utils/base.h"

void cardio_init();
void cardio_tick();

// Caller gets a heap-allocated buffer containing file, which they own
// Synchronous at the moment. Also, do not call while write_file() is running!
mkb_CARDResult cardio_read_file(const char *file_name, void **out_buf);

// Writes asynchronously
void cardio_write_file(const char *file_name,
                       const void *buf,
                       u32 buf_size,
                       void (*callback)(mkb_CARDResult));
