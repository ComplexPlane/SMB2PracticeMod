extern crate alloc;

use alloc::vec::Vec;
use core::ffi::c_void;

enum State {
    Prealloc,
    Save,
    Load,
}

pub trait MemStore {
    const STATE: State;

    fn scan_region(&mut self, ptr: *mut c_void, size: usize);

    fn scan_obj<T>(&mut self, obj: *mut T) {
        self.scan_region(obj as *mut c_void, size_of::<T>());
    }
}

pub struct MemStorePrealloc {
    curr_size: usize,
}

impl MemStore for MemStorePrealloc {
    const STATE: State = State::Prealloc;

    fn scan_region(&mut self, _ptr: *mut c_void, size: usize) {
        self.curr_size += size;
    }
}

pub struct MemStoreSave {
    buf: Vec<u8>,
    orig_size: usize,
}

impl MemStore for MemStoreSave {
    const STATE: State = State::Save;

    fn scan_region(&mut self, ptr: *mut c_void, size: usize) {
        unsafe {
            let slice = core::slice::from_raw_parts(ptr as *mut u8, size);
            self.buf.extend_from_slice(slice);
        }
    }
}

impl TryFrom<MemStorePrealloc> for MemStoreSave {
    type Error = (); // TODO use better error type

    fn try_from(mem_store: MemStorePrealloc) -> Result<Self, Self::Error> {
        let mut buf = Vec::new();
        buf.try_reserve_exact(mem_store.curr_size).map_err(|_| ())?;
        Ok(Self {
            orig_size: buf.len(),
            buf,
        })
    }
}

pub struct MemStoreLoad {
    buf: Vec<u8>,
    offset: usize,
}

impl MemStoreLoad {
    fn reset(&mut self) {
        self.offset = 0;
    }
}

impl MemStore for MemStoreLoad {
    const STATE: State = State::Load;

    fn scan_region(&mut self, ptr: *mut c_void, size: usize) {
        unsafe {
            let slice = core::slice::from_raw_parts_mut(ptr as *mut u8, size);
            slice.copy_from_slice(&self.buf[self.offset..self.offset + size]);
        }
    }
}
