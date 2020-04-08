cmake_minimum_required(VERSION 3.15)
project(ApeSphere)

set(CMAKE_CXX_STANDARD 14)

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
        rel/tetris.cpp rel/include/gc/gc.h rel/include/gc/osheap.h rel/include/gc/gxstruct.h rel/include/gc/gxenum.h rel/include/mkb/dip.h rel/bigmem.cpp rel/bigmem.h rel/include/gc/osassert.h)
