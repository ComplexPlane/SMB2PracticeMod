cmake_minimum_required(VERSION 3.15)
project(ApeSphere)

set(CMAKE_CXX_STANDARD 17)

# Specify 32-bit processor for better linting
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_FLAGS -m32)

include_directories(rel/include)
include_directories(rel/include/gc)
include_directories(rel/include/mkb)

add_executable(ApeSphere
        rel/assembly/MainLoopAssembly.s
        rel/runtime/restfpr_x.s
        rel/runtime/restgpr_x.s
        rel/cxx.cpp
        rel/global.cpp
        rel/heap.cpp
        rel/main.cpp
        rel/mod.cpp
        rel/pad.cpp
        rel/patch.cpp
        rel/rel.cpp
        rel/savestate.cpp
        rel/tetris.cpp rel/include/mkb/stagedef.h rel/include/mkb/mathtypes.h rel/include/mkb/stage.h rel/include/mkb/ball.h rel/include/mkb/tickable.h rel/include/mkb/item.h rel/include/mkb/effect.h rel/include/mkb/stobj.h rel/include/mkb/mode.h rel/titlescreen.cpp rel/include/titlescreen.h rel/include/mkb/sprite.h)
