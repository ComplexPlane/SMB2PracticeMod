#pragma once

#include "patch.h"
#include "tetris.h"

namespace mod::global {

extern void (*drawDebugText_trampoline)();

extern Tetris tetris;

}