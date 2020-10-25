#include "heap.h"

#include <gc/gc.h>

#include <cstring>
#include <cinttypes>

namespace heap
{

struct HeapDataStruct heap_data;

void init()
{
    make_heap(HEAP_SIZE);
}

gc::ChunkInfo *extract_chunk(
    gc::ChunkInfo *list, gc::ChunkInfo *chunk)
{
    if (chunk->next)
    {
        chunk->next->prev = chunk->prev;
    }

    if (!chunk->prev)
    {
        return chunk->next;
    }
    else
    {
        chunk->prev->next = chunk->next;
        return list;
    }
}

gc::ChunkInfo *add_chunk_to_front(
    gc::ChunkInfo *list, gc::ChunkInfo *chunk)
{
    chunk->next = list;
    chunk->prev = nullptr;

    if (list)
    {
        list->prev = chunk;
    }

    return chunk;
}

gc::ChunkInfo *find_chunk_in_list(
    gc::ChunkInfo *list, gc::ChunkInfo *chunk)
{
    for (; list; list = list->next)
    {
        if (list == chunk)
        {
            return list;
        }
    }
    return nullptr;
}

void *clear_and_flush_memory(void *start, uint32_t size)
{
    // Clear the memory
    memset(start, 0, size);

    // Flush the memory
    gc::DCFlushRange(start, size);

    // Return the address
    return start;
}

void *init_mem_alloc_services(uint32_t max_size)
{
    // Allocate memory for the heap array
    heap_data.custom_heap = reinterpret_cast<CustomHeapStruct *>(
        alloc_from_main_loop_reloc_memory(sizeof(CustomHeapStruct)));

    // Round the size up to the nearest multiple of 0x20 bytes
    const uint32_t alignment = 0x20;
    max_size = (max_size + alignment - 1) & ~(alignment - 1);

    // Allocate the desired memory
    void *arena_start = alloc_from_main_loop_reloc_memory(max_size);

    // Set up the arena end
    void *arena_end = reinterpret_cast<void *>(
        reinterpret_cast<uint32_t>(arena_start) + max_size);

    // Init the memory allocation services
    return init_alloc(arena_start, arena_end);
}

void *init_alloc(void *arena_start, void *arena_end)
{
    uint32_t arena_start_raw = reinterpret_cast<uint32_t>(arena_start);
    uint32_t arena_end_raw = reinterpret_cast<uint32_t>(arena_end);

    // Put the heap array at the start of the arena
    CustomHeapStruct *temp_custom_heap = heap_data.custom_heap;
    gc::HeapInfo *temp_heap_info = reinterpret_cast<gc::HeapInfo *>(arena_start);
    temp_custom_heap->heap_array = temp_heap_info;

    // Initialize the members of the heap array
    temp_heap_info->first_free = nullptr;
    temp_heap_info->first_used = nullptr;

    const uint32_t alignment = 0x20;
    uint32_t array_size = sizeof(gc::HeapInfo);

    // Adjust arenaStart to be at the nearest reasonable location
    // Gets rounded up to the nearest multiple of 0x20 bytes
    arena_start_raw = ((arena_start_raw + array_size) + alignment - 1) & ~(alignment - 1);

    // Round the end down to the nearest multiple of 0x20 bytes
    arena_end_raw &= ~(alignment - 1);

    arena_start = reinterpret_cast<void *>(arena_start_raw);
    arena_end = reinterpret_cast<void *>(arena_end_raw);
    temp_custom_heap->arena_start = arena_start;
    temp_custom_heap->arena_end = arena_end;

    return arena_start;
}

void make_heap(uint32_t size)
{
    // Round the size up to the nearest multiple of 0x20 bytes
    const uint32_t alignment = 0x20;
    size = (size + alignment - 1) & ~(alignment - 1);

    // Init the memory allocation services
    void *heap_array_start = init_mem_alloc_services(size);

    // Remove the total heap info size and then round down to the nearest multiple of 0x20 bytes
    uint32_t array_size = sizeof(gc::HeapInfo);
    size = (size - array_size) & ~(alignment - 1);

    // Set the end address
    void *end = reinterpret_cast<void *>(reinterpret_cast<uint32_t>(heap_array_start) + size);

    // Create the heap
    create_heap(heap_array_start, end);
}

void create_heap(void *start, void *end)
{
    uint32_t start_raw = reinterpret_cast<uint32_t>(start);
    uint32_t end_raw = reinterpret_cast<uint32_t>(end);

    // Round the start up to the nearest multiple of 0x20 bytes,
    // Round the end down to the nearest multiple of 0x20 bytes
    const uint32_t alignment = 0x20;
    start_raw = (start_raw + alignment - 1) & ~(alignment - 1);
    end_raw &= ~(alignment - 1);

    gc::HeapInfo *info = heap_data.custom_heap->heap_array;
    int32_t size = end_raw - start_raw;

    gc::ChunkInfo *temp_chunk = reinterpret_cast<gc::ChunkInfo *>(start_raw);
    temp_chunk->prev = nullptr;
    temp_chunk->next = nullptr;
    temp_chunk->size = size;

    info->first_free = temp_chunk;
    info->first_used = nullptr;
}

void destroy_heap()
{
    gc::HeapInfo *info = heap_data.custom_heap->heap_array;
    info->first_free = nullptr;
    info->first_used = nullptr;
}

void *alloc_from_main_loop_reloc_memory(uint32_t size)
{
    // Round the size up to the nearest multiple of 0x20 bytes
    const uint32_t alignment = 0x20;
    size = (size + alignment - 1) & ~(alignment - 1);

    // Take the memory from the main game loop's relocation data
    uint32_t address_raw = reinterpret_cast<uint32_t>(heap_data.relocation_data_arena);

    // Increment the main game loop's relocation data by the size
    heap_data.relocation_data_arena = reinterpret_cast<void *>(address_raw + size);

    return clear_and_flush_memory(reinterpret_cast<void *>(address_raw), size);
}

void *alloc_from_heap(uint32_t size)
{
    // Enlarge size to the smallest possible chunk size
    const uint32_t alignment = 0x20;
    uint32_t new_size = size + ((sizeof(gc::ChunkInfo) + alignment - 1) & ~(alignment - 1));
    new_size = (new_size + alignment - 1) & ~(alignment - 1);

    gc::HeapInfo *info = heap_data.custom_heap->heap_array;
    gc::ChunkInfo *temp_chunk = nullptr;

    // Find a memory area large enough
    for (temp_chunk = info->first_free; temp_chunk; temp_chunk = temp_chunk->next)
    {
        if (static_cast<int32_t>(new_size) <= temp_chunk->size)
        {
            break;
        }
    }

    // Make sure the found region is valid
    if (!temp_chunk)
    {
        return nullptr;
    }

    int32_t leftover_size = temp_chunk->size - static_cast<int32_t>(new_size);

    int32_t min_size = ((sizeof(gc::ChunkInfo) +
                         alignment - 1) & ~(alignment - 1)) + alignment;

    // Check if the current chunk can be split into two pieces
    if (leftover_size < min_size)
    {
        // Too small to split, so just extract it
        info->first_free = extract_chunk(info->first_free, temp_chunk);
    }
    else
    {
        // Large enough to split
        temp_chunk->size = static_cast<int32_t>(new_size);

        // Create a new chunk
        gc::ChunkInfo *new_chunk = reinterpret_cast<gc::ChunkInfo *>(
            reinterpret_cast<uint32_t>(temp_chunk) + new_size);

        new_chunk->size = leftover_size;

        new_chunk->prev = temp_chunk->prev;
        new_chunk->next = temp_chunk->next;

        if (new_chunk->next)
        {
            new_chunk->next->prev = new_chunk;
        }

        if (new_chunk->prev)
        {
            new_chunk->prev->next = new_chunk;
        }
        else
        {
            info->first_free = new_chunk;
        }
    }

    // Add the chunk to the allocated list
    info->first_used = add_chunk_to_front(info->first_used, temp_chunk);

    // Add the header size to the chunk
    void *allocated_memory = reinterpret_cast<void *>(reinterpret_cast<uint32_t>(temp_chunk) +
                                                      ((sizeof(gc::ChunkInfo) + alignment - 1) & ~(alignment - 1)));

    // Clear and flush the memory and then return it
    return clear_and_flush_memory(allocated_memory, size);
}

bool free_to_heap(void *ptr)
{
    const uint32_t alignment = 0x20;
    uint32_t ptr_raw = reinterpret_cast<uint32_t>(ptr);

    uint32_t header_size = (sizeof(gc::ChunkInfo) +
                            alignment - 1) & ~(alignment - 1);

    // Remove the header size from ptr, as the value stored in the list does not include it
    gc::ChunkInfo *temp_chunk = reinterpret_cast<gc::ChunkInfo *>(ptr_raw - header_size);
    gc::HeapInfo *info = heap_data.custom_heap->heap_array;

    // Make sure ptr is actually allocated
    if (!find_chunk_in_list(info->first_used, temp_chunk))
    {
        return false;
    }

    // Extract the chunk from the allocated list
    info->first_used = extract_chunk(info->first_used, temp_chunk);

    // Add in sorted order to the free list
    info->first_free = gc::DLInsert(info->first_free, temp_chunk);
    return true;
}

size_t get_free_space()
{
    size_t space = 0;
    gc::HeapInfo *temp_heap = heap::heap_data.custom_heap->heap_array;

    for (gc::ChunkInfo *chunk = temp_heap->first_free; chunk; chunk = chunk->next)
    {
        space += chunk->size - 32; // Don't count the ChunkInfo
    }

    return space;
}

void check_heap()
{
    gc::HeapInfo *temp_heap = heap::heap_data.custom_heap->heap_array;
    bool valid = true;

    gc::ChunkInfo *current_chunk = nullptr;
    gc::ChunkInfo *prev_chunk = nullptr;
    for (current_chunk = temp_heap->first_used; current_chunk; current_chunk = current_chunk->next)
    {
        // Check pointer sanity
        auto check_if_pointer_is_valid = [](void *ptr)
        {
            uint32_t ptr_raw = reinterpret_cast<uint32_t>(ptr);
            return (ptr_raw >= 0x80000000) && (ptr_raw < 0x81800000);
        };

        if (!check_if_pointer_is_valid(current_chunk))
        {
            valid = false;
            break;
        }

        // Sanity check size
        if (current_chunk->size >= 0x1800000)
        {
            valid = false;
            break;
        }

        // Check linked list integrity
        if (prev_chunk != current_chunk->prev)
        {
            valid = false;
            break;
        }

        prev_chunk = current_chunk;
    }

    if (!valid)
    {
        // Print the error message to the console
        gc::OSReport(
            "Heap corrupt at 0x%08" PRIx32 "\n",
            reinterpret_cast<uint32_t>(current_chunk));
    }
}

}