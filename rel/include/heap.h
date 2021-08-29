#pragma once

#include <mkb.h>
#include "relutil.h"

namespace heap {

struct CustomHeapStruct {
    mkb::HeapInfo* heap_array;
};

struct HeapDataStruct {
    CustomHeapStruct* custom_heap;
    void* relocation_data_arena;
    void* relocation_data_start;  // Also the custom REL module start
    void* relocation_data_end;
    void* custom_rel_bss_area_start;
    void* main_loop_rel_location;
    void* main_loop_bss_location;

    HeapDataStruct() {
        relocation_data_arena = *reinterpret_cast<u32**>(0x8000452C);
        relocation_data_start = *reinterpret_cast<u32**>(0x80004534);
        relocation_data_end =
            relutil::compute_mainloop_reldata_boundary();  // TODO don't do this in header
        custom_rel_bss_area_start = *reinterpret_cast<u32**>(0x80004530);
        main_loop_rel_location = *reinterpret_cast<u32**>(0x80004524);
        main_loop_bss_location = *reinterpret_cast<u32**>(0x80004528);
    }
};

void init();
mkb::ChunkInfo* extract_chunk(mkb::ChunkInfo* list, mkb::ChunkInfo* chunk);
mkb::ChunkInfo* add_chunk_to_front(mkb::ChunkInfo* list, mkb::ChunkInfo* chunk);
mkb::ChunkInfo* find_chunk_in_list(mkb::ChunkInfo* list, mkb::ChunkInfo* chunk);
void* clear_and_flush_memory(void* start, u32 size);
void* init_mem_alloc_services(u32 max_size);
void* init_alloc(void* arena_start, void* arena_end);
void make_heap(u32 size);
void create_heap(void* start, void* end);
void destroy_heap();
void* alloc_from_main_loop_reloc_memory(u32 size);
void* alloc_from_heap(u32 size);
bool free_to_heap(void* ptr);
void check_heap();
u32 get_free_space();
u32 get_total_space();

extern HeapDataStruct heap_data;

}  // namespace heap