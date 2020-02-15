#include "global.h"

// Idk how to best organize and structure data (let alone whether a global module like this is a
// good idea) like this but let's just mess around like this for now

namespace mod::global {

void (*drawDebugText_trampoline)() = nullptr;

Tetris tetris;

}