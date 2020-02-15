#pragma once

#include "patch.h"
#include "tetris.h"
#include "pad.h"

namespace mod::global {

extern void (*drawDebugText_trampoline)();

extern Tetris tetris;

}