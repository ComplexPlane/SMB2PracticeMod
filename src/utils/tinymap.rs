#![cfg(feature = "mkb2")]

// Tinymap is a map with small code size. It simply uses a fixed-size sorted array as the backing
// store which is binary-searched over.

use arrayvec::ArrayVec;

struct Pair<K, V> {
    key: K,
    value: V,
}

pub struct TinyMapBuilder<K, V, const N: usize>
where
    K: Copy + Ord,
    V: Copy,
{
    list: ArrayVec<Pair<K, V>, N>,
}

impl<K, V, const N: usize> TinyMapBuilder<K, V, N>
where
    K: Copy + Ord,
    V: Copy,
{
    pub fn new() -> Self {
        Self {
            list: ArrayVec::<Pair<K, V>, N>::new(),
        }
    }

    pub fn insert(&mut self, key: K, value: V) {
        self.list.push(Pair { key, value });
    }

    pub fn build(mut self) -> TinyMap<K, V, N> {
        let len = self.list.len();
        for i in 0..len {
            let mut min_idx = i;
            for j in (i + 1)..len {
                if self.list[j].key < self.list[min_idx].key {
                    min_idx = j;
                }
            }
            if min_idx != i {
                self.list.swap(i, min_idx);
            }
        }

        TinyMap::new(self)
    }
}

pub struct TinyMap<K, V, const N: usize>
where
    K: Copy + Ord,
    V: Copy,
{
    list: ArrayVec<Pair<K, V>, N>,
}

impl<K, V, const N: usize> TinyMap<K, V, N>
where
    K: Copy + Ord,
    V: Copy,
{
    pub fn new(builder: TinyMapBuilder<K, V, N>) -> Self {
        Self { list: builder.list }
    }

    fn find_idx(&self, key: K) -> usize {
        self.list.binary_search_by(|p| p.key.cmp(&key)).unwrap()
    }

    pub fn get(&self, key: K) -> &V {
        let idx = self.find_idx(key);
        &self.list[idx].value
    }

    pub fn get_mut(&mut self, key: K) -> &mut V {
        let idx = self.find_idx(key);
        &mut self.list[idx].value
    }
}
