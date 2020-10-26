#pragma once

#include <gc/gc.h>

namespace heap
{

// 0x40000 bytes (256 KiB) for now
constexpr u32 HEAP_SIZE = 0x40000;

struct CustomHeapStruct
{
    gc::HeapInfo *heap_array;
    void *arena_start;
    void *arena_end;
};

struct HeapDataStruct
{
    CustomHeapStruct *custom_heap;
    void *relocation_data_arena;
    void *relocation_data_start; // Also the custom REL module start
    void *custom_rel_bss_area_start;
    void *main_loop_rel_location;
    void *main_loop_bss_location;

    HeapDataStruct()
    {
        relocation_data_arena = *reinterpret_cast<u32 **>(0x8000452C);
        relocation_data_start = *reinterpret_cast<u32 **>(0x80004534);
        custom_rel_bss_area_start = *reinterpret_cast<u32 **>(0x80004530);
        main_loop_rel_location = *reinterpret_cast<u32 **>(0x80004524);
        main_loop_bss_location = *reinterpret_cast<u32 **>(0x80004528);
    }
};

void init();
gc::ChunkInfo *extract_chunk(gc::ChunkInfo *list, gc::ChunkInfo *chunk);
gc::ChunkInfo *add_chunk_to_front(gc::ChunkInfo *list, gc::ChunkInfo *chunk);
gc::ChunkInfo *find_chunk_in_list(gc::ChunkInfo *list, gc::ChunkInfo *chunk);
void *clear_and_flush_memory(void *start, u32 size);
void *init_mem_alloc_services(u32 max_size);
void *init_alloc(void *arena_start, void *arena_end);
void make_heap(u32 size);
void create_heap(void *start, void *end);
void destroy_heap();
void *alloc_from_main_loop_reloc_memory(u32 size);
void *alloc_from_heap(u32 size);
bool free_to_heap(void *ptr);
void check_heap();
u32 get_free_space();

extern HeapDataStruct heap_data;

}