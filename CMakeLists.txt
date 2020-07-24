cmake_minimum_required(VERSION 3.15)
project(ApeSphere)

set(CMAKE_CXX_STANDARD 17)

# Specify 32-bit processor for better linting
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_FLAGS -m32)

include_directories(rel/include)
include_directories(rel/include/gc)
include_directories(rel/include/mkb)

add_definitions(-DMKB2_US)

add_executable(ApeSphere
        rel/assembly/MainLoopAssembly.s
        rel/runtime/restfpr_x.s
        rel/runtime/restgpr_x.s
        rel/cxx.cpp
        rel/heap.cpp
        rel/main.cpp
        rel/pad.cpp
        rel/patch.cpp
        rel/rel.cpp
        rel/tetris.cpp
        rel/memstore.cpp
        rel/savestate.cpp
        rel/draw.cpp
        rel/scratch.cpp rel/assembly.cpp rel/timer.cpp rel/include/timer.h rel/include/gc/vi.h rel/include/mkb/story.h rel/include/iw.h rel/iw.cpp rel/include/mkb/unlock.h rel/include/menu.h rel/menu.cpp)
