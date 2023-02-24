#pragma once

#include "mkb/mkb.h"

namespace log {

// To save space and because it just makes more sense, we differentiate
// user-facing error messages from assertion failures meant for developers.
//
// abort() is for user-caused errors, with clear error messages to help resolve
// the problem.
//
// MOD_ASSERT() includes line/col number of assertion failure but not a message,
// which is more appropriate for a developer.

void mod_assert(const char *file, s32 line, bool exp);

[[noreturn]] void abort();
[[noreturn]] void abort(const char *format, ...);

} // namespace log

// Factor as much out of the macro as possible to save space
#define MOD_ASSERT(exp) (log::mod_assert(__FILE__, __LINE__, (exp)))
