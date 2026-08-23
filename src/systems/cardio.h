#pragma once

#include "utils/base.h"

void Card_Init();
void Card_Tick();

// Caller gets a heap-allocated buffer containing file, which they own
// Synchronous at the moment. Also, do not call while write_file() is running!
mkb_CARDResult Card_ReadFile(const char *file_name, void **out_buf);

// Writes asynchronously
void Card_WriteFile(const char *file_name,
                    const void *buf,
                    u32 buf_size,
                    void (*callback)(mkb_CARDResult));
