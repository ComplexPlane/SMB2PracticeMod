use mkb::mkb;

extern crate alloc;

use core::{alloc::GlobalAlloc, cell::UnsafeCell, ptr::null_mut};

use crate::utils::math;
use crate::utils::modlink::ModLink;
use crate::utils::relutil;

unsafe fn extract_chunk(
    list: *mut mkb::ChunkInfo,
    chunk: *mut mkb::ChunkInfo,
) -> *mut mkb::ChunkInfo {
    unsafe {
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
}

unsafe fn add_chunk_to_front(
    list: *mut mkb::ChunkInfo,
    chunk: *mut mkb::ChunkInfo,
) -> *mut mkb::ChunkInfo {
    unsafe {
        (*chunk).next = list;
        (*chunk).prev = null_mut();

        if !list.is_null() {
            (*list).prev = chunk;
        }

        chunk
    }
}

pub struct Heap {
    owned_heap_info: UnsafeCell<mkb::HeapInfo>,
    heap_info: UnsafeCell<*mut mkb::HeapInfo>,
}

unsafe fn make_heap_info() -> mkb::HeapInfo {
    unsafe {
        let start = math::round_up_pow2(*((0x8000452C) as *const usize), 32);
        let end_ptr = relutil::compute_mainloop_reldata_boundary(start);
        let end = math::round_down_pow2(end_ptr, 32);
        let size = end - start;

        core::ptr::write_bytes(start as *mut u8, 0, size);

        let heap_info = mkb::HeapInfo {
            capacity: size as u32,
            first_free: start as *mut mkb::ChunkInfo,
            first_used: null_mut(),
        };
        (*heap_info.first_free).next = null_mut();
        (*heap_info.first_free).prev = null_mut();
        (*heap_info.first_free).size = size as u32;

        heap_info
    }
}

impl Heap {
    pub fn init(&self) {
        let modlink = ModLink::new();
        unsafe {
            match modlink.heap_info {
                Some(heap_info) => {
                    *self.heap_info.get() = heap_info;
                }
                None => {
                    *self.owned_heap_info.get() = make_heap_info();
                    *self.heap_info.get() = self.owned_heap_info.get();
                }
            }
        }
    }

    #[allow(dead_code)]
    pub fn get_free_space(&self) -> usize {
        let mut space = 0usize;
        unsafe {
            let heap_info = &**self.heap_info.get();
            let mut chunk = heap_info.first_free;
            while !chunk.is_null() {
                space += (*chunk).size as usize - 32; // Don't count the ChunkInfo
                chunk = (*chunk).next;
            }
        }
        space
    }
}

unsafe impl GlobalAlloc for Heap {
    unsafe fn alloc(&self, layout: core::alloc::Layout) -> *mut u8 {
        unsafe {
            let heap_info = &mut **self.heap_info.get();

            // Enlarge size to the smallest possible chunk size
            // We currently don't respect starting address alignments > 32
            let new_size = math::round_up_pow2(layout.size(), layout.align());
            let new_size =
                new_size + math::round_up_pow2(core::mem::size_of::<mkb::ChunkInfo>(), 32);
            let new_size = math::round_up_pow2(new_size, 32);

            // Find a memory area large enough
            let mut temp_chunk = heap_info.first_free;
            while !temp_chunk.is_null() {
                if new_size <= (*temp_chunk).size as usize {
                    break;
                }
                temp_chunk = (*temp_chunk).next;
            }

            // Make sure the found region is valid
            if temp_chunk.is_null() {
                return null_mut();
            }

            let leftover_size = (*temp_chunk).size as isize - new_size as isize;

            let min_size = math::round_up_pow2(core::mem::size_of::<mkb::ChunkInfo>(), 32) + 32;

            // Check if the current chunk can be split into two pieces
            if leftover_size < min_size as isize {
                // Too small to split, so just extract it
                heap_info.first_free = extract_chunk(heap_info.first_free, temp_chunk);
            } else {
                // Large enough to split
                (*temp_chunk).size = new_size as u32;

                // Create a new chunk
                let new_chunk = (temp_chunk as usize + new_size) as *mut mkb::ChunkInfo;

                (*new_chunk).size = leftover_size as u32;

                (*new_chunk).prev = (*temp_chunk).prev;
                (*new_chunk).next = (*temp_chunk).next;

                if !(*new_chunk).next.is_null() {
                    (*(*new_chunk).next).prev = new_chunk;
                }

                if !(*new_chunk).prev.is_null() {
                    (*(*new_chunk).prev).next = new_chunk;
                } else {
                    heap_info.first_free = new_chunk;
                }
            }

            // Add the chunk to the allocated list
            heap_info.first_used = add_chunk_to_front(heap_info.first_used, temp_chunk);

            // Add the header size to the chunk
            let allocated_memory = (temp_chunk as usize
                + math::round_up_pow2(core::mem::size_of::<mkb::ChunkInfo>(), 32))
                as *mut u8;

            core::ptr::write_bytes(allocated_memory, 0, layout.size());
            allocated_memory
        }
    }

    unsafe fn dealloc(&self, ptr: *mut u8, _layout: core::alloc::Layout) {
        unsafe {
            let heap_info = &mut **self.heap_info.get();
            let ptr_raw = ptr as usize;

            let header_size = math::round_up_pow2(core::mem::size_of::<mkb::ChunkInfo>(), 32);

            // Remove the header size from ptr, as the value stored in the list does not include it
            let temp_chunk = (ptr_raw - header_size) as *mut mkb::ChunkInfo;

            // Extract the chunk from the allocated list
            heap_info.first_used = extract_chunk(heap_info.first_used, temp_chunk);
            // Add in sorted order to the free list
            heap_info.first_free = mkb::DLInsert(heap_info.first_free, temp_chunk);
        }
    }
}

#[global_allocator]
pub static HEAP: Heap = Heap {
    owned_heap_info: UnsafeCell::new(mkb::HeapInfo {
        capacity: 0,
        first_free: null_mut(),
        first_used: null_mut(),
    }),
    heap_info: UnsafeCell::new(null_mut()),
};

// We only have one thread and will never hook any function called by an interrupt.
// It'd probably be cleaner to signal this with a mutex from e.g. the `critical-section` crate
unsafe impl Sync for Heap {}
