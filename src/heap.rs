extern crate alloc;

use core::{alloc::GlobalAlloc, cell::UnsafeCell, ptr::null_mut};

use crate::relutil;

// TODO: pull these structs from bindgen'd Ghidra headers

#[repr(C, packed)]
struct HeapInfo {
    capacity: usize,
    first_free: *mut ChunkInfo,
    first_used: *mut ChunkInfo,
}

#[repr(C, packed)]
struct ChunkInfo {
    prev: *mut ChunkInfo,
    next: *mut ChunkInfo,
    size: usize,
    padding: [u8; 20],
}

extern "C" {
    fn DLInsert(free_chunk_list: *mut ChunkInfo, new_free_chunk: *mut ChunkInfo) -> *mut ChunkInfo;
}

fn round_up_pow2(n: usize, align: usize) -> usize {
    (n + (align - 1)) & !(align - 1)
}

fn round_down_pow2(n: usize, align: usize) -> usize {
    n & !(align - 1)
}

unsafe fn extract_chunk(list: *mut ChunkInfo, chunk: *mut ChunkInfo) -> *mut ChunkInfo {
    if !(*chunk).next.is_null() {
        (*(*chunk).next).prev = (*chunk).prev;
    }

    if (*chunk).prev.is_null() {
        (*chunk).next
    } else {
        (*(*chunk).prev).next = (*chunk).next;
        list
    }
}

unsafe fn add_chunk_to_front(list: *mut ChunkInfo, chunk: *mut ChunkInfo) -> *mut ChunkInfo {
    (*chunk).next = list;
    (*chunk).prev = null_mut();

    if !list.is_null() {
        (*list).prev = chunk;
    }

    chunk
}

pub struct Heap {
    owned_heap_info: UnsafeCell<HeapInfo>,
    heap_info: UnsafeCell<*mut HeapInfo>,
}

unsafe fn make_heap_info() -> HeapInfo {
    let start = round_up_pow2(*((0x8000452C) as *const usize), 32);
    let end_ptr = relutil::compute_mainloop_reldata_boundary(start);
    let end = round_down_pow2(end_ptr, 32);
    let size = end - start;

    core::ptr::write_bytes(start as *mut u8, 0, size as usize);

    let heap_info = HeapInfo {
        capacity: size,
        first_free: start as *mut ChunkInfo,
        first_used: null_mut(),
    };
    (*heap_info.first_free).next = null_mut();
    (*heap_info.first_free).prev = null_mut();
    (*heap_info.first_free).size = size;

    heap_info
}

impl Heap {
    pub fn init(&self) {
        // TODO optionally use wsmod's heap
        unsafe {
            *self.owned_heap_info.get() = make_heap_info();
            *self.heap_info.get() = self.owned_heap_info.get();
        }
    }
}

unsafe impl GlobalAlloc for Heap {
    unsafe fn alloc(&self, layout: core::alloc::Layout) -> *mut u8 {
        let heap_info = *self.heap_info.get();

        // Enlarge size to the smallest possible chunk size
        // We currently don't respect starting address alignments > 32
        let new_size = round_up_pow2(layout.size(), layout.align());
        let new_size = new_size + round_up_pow2(core::mem::size_of::<ChunkInfo>(), 32);
        let new_size = round_up_pow2(new_size, 32);

        // Find a memory area large enough
        let mut temp_chunk = (*heap_info).first_free;
        while !temp_chunk.is_null() {
            if new_size <= (*temp_chunk).size {
                break;
            }
            temp_chunk = (*temp_chunk).next;
        }

        // Make sure the found region is valid
        if temp_chunk.is_null() {
            return null_mut();
        }

        let leftover_size = (*temp_chunk).size as isize - new_size as isize;

        let min_size = round_up_pow2(core::mem::size_of::<ChunkInfo>(), 32) + 32;

        // Check if the current chunk can be split into two pieces
        if leftover_size < min_size as isize {
            // Too small to split, so just extract it
            (*heap_info).first_free = extract_chunk((*heap_info).first_free, temp_chunk);
        } else {
            // Large enough to split
            (*temp_chunk).size = new_size;

            // Create a new chunk
            let new_chunk = (temp_chunk as usize + new_size) as *mut ChunkInfo;

            (*new_chunk).size = leftover_size as usize;

            (*new_chunk).prev = (*temp_chunk).prev;
            (*new_chunk).next = (*temp_chunk).next;

            if !(*new_chunk).next.is_null() {
                (*(*new_chunk).next).prev = new_chunk;
            }

            if !(*new_chunk).prev.is_null() {
                (*(*new_chunk).prev).next = new_chunk;
            } else {
                (*heap_info).first_free = new_chunk;
            }
        }

        // Add the chunk to the allocated list
        (*heap_info).first_used = add_chunk_to_front((*heap_info).first_used, temp_chunk);

        // Add the header size to the chunk
        let allocated_memory =
            (temp_chunk as usize + round_up_pow2(core::mem::size_of::<ChunkInfo>(), 32)) as *mut u8;

        core::ptr::write_bytes(allocated_memory, 0, layout.size());
        allocated_memory
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: core::alloc::Layout) {
        let heap_info = *self.heap_info.get();
        let ptr_raw = ptr as usize;

        let header_size = round_up_pow2(core::mem::size_of::<ChunkInfo>(), 32);

        // Remove the header size from ptr, as the value stored in the list does not include it
        let temp_chunk = (ptr_raw - header_size) as *mut ChunkInfo;

        // Extract the chunk from the allocated list
        (*heap_info).first_used = extract_chunk((*heap_info).first_used, temp_chunk);
        // Add in sorted order to the free list
        (*heap_info).first_free = DLInsert((*heap_info).first_free, temp_chunk);
    }
}

#[global_allocator]
pub static HEAP: Heap = Heap {
    owned_heap_info: UnsafeCell::new(HeapInfo {
        capacity: 0,
        first_free: null_mut(),
        first_used: null_mut(),
    }),
    heap_info: UnsafeCell::new(null_mut()),
};

// We only have one thread and will never hook any function called by an interrupt.
// It'd probably be cleaner to signal this with a mutex from e.g. the `critical-section` crate
unsafe impl Sync for Heap {}
