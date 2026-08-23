#include "heap.h"

#include <inttypes.h>
#include "systems/modlink.h"
#include "utils/base.h"
#include "utils/relutil.h"

static mkb_HeapInfo s_local_heap_info;  // Use our own HeapInfo if Workshop Mod isn't loaded
static mkb_HeapInfo *s_heap_info;       // Pointer to either our HeapInfo or Workshop Mod's

static mkb_ChunkInfo *extract_chunk(mkb_ChunkInfo *list, mkb_ChunkInfo *chunk) {
    if (chunk->next) {
        chunk->next->prev = chunk->prev;
    }

    if (!chunk->prev) {
        return chunk->next;
    } else {
        chunk->prev->next = chunk->next;
        return list;
    }
}

static mkb_ChunkInfo *add_chunk_to_front(mkb_ChunkInfo *list, mkb_ChunkInfo *chunk) {
    chunk->next = list;
    chunk->prev = nullptr;

    if (list) {
        list->prev = chunk;
    }

    return chunk;
}

static mkb_ChunkInfo *find_chunk_in_list(mkb_ChunkInfo *list, mkb_ChunkInfo *chunk) {
    for (; list; list = list->next) {
        if (list == chunk) {
            return list;
        }
    }
    return nullptr;
}

static void make_heap() {
    u32 start = mkb_OSRoundUp32B(*(u32 *)(0x8000452C));
    void *end_ptr = rel_compute_mainloop_reldata_boundary((void *)start);  // TODO precompute?
    u32 end = mkb_OSRoundDown32B((u32)(end_ptr));
    u32 size = end - start;

    mkb_memset((void *)(start), 0, size);

    s_heap_info->capacity = size;
    s_heap_info->first_free = (mkb_ChunkInfo *)(start);
    s_heap_info->first_free->next = nullptr;
    s_heap_info->first_free->prev = nullptr;
    s_heap_info->first_free->size = size;
    s_heap_info->first_used = nullptr;
}

void *Heap_Alloc(u32 size) {
    // Enlarge size to the smallest possible chunk size
    u32 new_size = size + mkb_OSRoundUp32B(sizeof(mkb_ChunkInfo));
    new_size = mkb_OSRoundUp32B(new_size);

    mkb_ChunkInfo *temp_chunk = nullptr;

    // Find a memory area large enough
    for (temp_chunk = s_heap_info->first_free; temp_chunk; temp_chunk = temp_chunk->next) {
        if (new_size <= temp_chunk->size) {
            break;
        }
    }

    // Make sure the found region is valid
    if (!temp_chunk) {
        return nullptr;
    }

    s32 leftover_size = temp_chunk->size - new_size;

    s32 min_size = mkb_OSRoundUp32B(sizeof(mkb_ChunkInfo)) + 32;

    // Check if the current chunk can be split into two pieces
    if (leftover_size < min_size) {
        // Too small to split, so just extract it
        s_heap_info->first_free = extract_chunk(s_heap_info->first_free, temp_chunk);
    } else {
        // Large enough to split
        temp_chunk->size = (s32)(new_size);

        // Create a new chunk
        mkb_ChunkInfo *new_chunk = (mkb_ChunkInfo *)((u32)(temp_chunk) + new_size);

        new_chunk->size = leftover_size;

        new_chunk->prev = temp_chunk->prev;
        new_chunk->next = temp_chunk->next;

        if (new_chunk->next) {
            new_chunk->next->prev = new_chunk;
        }

        if (new_chunk->prev) {
            new_chunk->prev->next = new_chunk;
        } else {
            s_heap_info->first_free = new_chunk;
        }
    }

    // Add the chunk to the allocated list
    s_heap_info->first_used = add_chunk_to_front(s_heap_info->first_used, temp_chunk);

    // Add the header size to the chunk
    void *allocated_memory = (void *)((u32)(temp_chunk) + mkb_OSRoundUp32B(sizeof(mkb_ChunkInfo)));

    mkb_memset(allocated_memory, 0, size);
    return allocated_memory;
}

bool Heap_Free(void *ptr) {
    u32 ptr_raw = (u32)(ptr);

    u32 header_size = mkb_OSRoundUp32B(sizeof(mkb_ChunkInfo));

    // Remove the header size from ptr, as the value stored in the list does not include it
    mkb_ChunkInfo *temp_chunk = (mkb_ChunkInfo *)(ptr_raw - header_size);

    // Make sure ptr is actually allocated
    if (!find_chunk_in_list(s_heap_info->first_used, temp_chunk)) {
        return false;
    }

    // Extract the chunk from the allocated list
    s_heap_info->first_used = extract_chunk(s_heap_info->first_used, temp_chunk);

    // Add in sorted order to the free list
    s_heap_info->first_free = mkb_DLInsert(s_heap_info->first_free, temp_chunk);
    return true;
}

u32 Heap_GetFreeSpace() {
    u32 space = 0;
    for (mkb_ChunkInfo *chunk = s_heap_info->first_free; chunk; chunk = chunk->next) {
        space += chunk->size - 32;  // Don't count the ChunkInfo
    }
    return space;
}

u32 Heap_GetTotalSpace() {
    return s_heap_info->capacity;
}

void Heap_CheckIntegrity() {
    bool valid = true;

    mkb_ChunkInfo *current_chunk = nullptr;
    mkb_ChunkInfo *prev_chunk = nullptr;
    for (current_chunk = s_heap_info->first_used; current_chunk;
         current_chunk = current_chunk->next) {
        // Check pointer sanity
        bool ptr_is_valid = ((u32)current_chunk >= 0x80000000) && ((u32)current_chunk < 0x81800000);
        if (!ptr_is_valid) {
            valid = false;
            break;
        }

        // Sanity check size
        if (current_chunk->size >= 0x1800000) {
            valid = false;
            break;
        }

        // Check linked list integrity
        if (prev_chunk != current_chunk->prev) {
            valid = false;
            break;
        }

        prev_chunk = current_chunk;
    }

    if (!valid) {
        // Print the error message to the console
        mkb_OSReport("Heap corrupt at 0x%08" PRIx32 "\n", (u32)(current_chunk));
    }
}

void Heap_Init() {
    // Use Workshop Mod's heap if it's loaded, otherwise make our own
    if (modlink_get() != nullptr) {
        s_heap_info = modlink_get()->heap_info;
    } else {
        s_heap_info = &s_local_heap_info;
        make_heap();
    }
}
