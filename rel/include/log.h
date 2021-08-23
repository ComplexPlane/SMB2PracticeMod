#pragma once

#include <mkb.h>

namespace log {
void mod_assert(const char* file, u32 line, bool exp);
}

#define MOD_ASSERT(exp) (log::mod_assert(__FILE__, __LINE__, (exp)))