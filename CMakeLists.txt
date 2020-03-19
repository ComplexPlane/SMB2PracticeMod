cmake_minimum_required(VERSION 3.15)
project(ApeSphere)

set(CMAKE_CXX_STANDARD 14)

include_directories(rel/include)
include_directories(rel/include/gc)
include_directories(rel/include/mkb)

add_executable(ApeSphere
        rel/assembly/MainLoopAssembly.s
        rel/include/gc/dvd.h
        rel/include/gc/gx.h
        rel/include/gc/os.h
        rel/include/gc/ppc.h
        rel/include/mkb/mkb.h
        rel/include/assembly.h
        rel/include/global.h
        rel/include/heap.h
        rel/include/mod.h
        rel/include/pad.h
        rel/include/patch.h
        rel/include/tetris.h
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
        rel/tetris.cpp)
