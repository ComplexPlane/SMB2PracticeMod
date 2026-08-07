cmake_minimum_required(VERSION 3.15)
project(elf2rel)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
include_directories(elfio)

add_executable(elf2rel elf2rel.cpp)
