extern crate alloc;

use alloc::vec::Vec;
use core::ffi::c_void;

pub struct Prealloc {
    curr_size: usize,
}

impl Prealloc {
    pub fn new() -> Self {
        Self { curr_size: 0 }
    }
}

pub struct Save {
    buf: Vec<u8>,
    orig_size: usize,
}

impl TryFrom<Prealloc> for Save {
    type Error = ();

    fn try_from(prealloc: Prealloc) -> Result<Self, Self::Error> {
        let mut buf = Vec::new();
        buf.try_reserve_exact(prealloc.curr_size).map_err(|_| ())?;
        Ok(Self {
            orig_size: prealloc.curr_size,
            buf,
        })
    }
}

pub struct Load {
    buf: Vec<u8>,
    offset: usize,
}

impl Load {
    pub fn reset(&mut self) {
        self.offset = 0;
    }
}

impl From<Save> for Load {
    fn from(save: Save) -> Self {
        assert!(save.orig_size == save.buf.len());
        Self {
            buf: save.buf,
            offset: 0,
        }
    }
}

pub enum MemStore<'a> {
    Prealloc(&'a mut Prealloc),
    Save(&'a mut Save),
    Load(&'a mut Load),
}

impl<'a> MemStore<'a> {
    pub fn scan_region(&mut self, ptr: *mut c_void, size: usize) {
        match self {
            MemStore::Prealloc(buf) => {
                buf.curr_size += size;
            }
            MemStore::Save(buf) => unsafe {
                let slice = core::slice::from_raw_parts(ptr as *mut u8, size);
                buf.buf.extend_from_slice(slice);
            },
            MemStore::Load(buf) => unsafe {
                let slice = core::slice::from_raw_parts_mut(ptr as *mut u8, size);
                slice.copy_from_slice(&buf.buf[buf.offset..buf.offset + size]);
                buf.offset += size;
            },
        }
    }

    pub fn scan_obj<T>(&mut self, obj: *mut T) {
        self.scan_region(obj as *mut c_void, size_of::<T>());
    }
}
