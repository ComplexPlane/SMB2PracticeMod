use core::cmp::Ordering;

use arrayvec::ArrayString;

use crate::fmt_buf;

// Do NOT edit this, it's used directly in the modlink format
#[repr(C)]
#[derive(PartialOrd, Ord, PartialEq, Eq, Clone, Copy)]
pub struct SemVer {
    pub major: u16,
    pub minor: u16,
    pub patch: u16,
}

#[derive(PartialEq, Eq)]
struct PracmodVersion {
    semver: SemVer,
    release_cand: u16,
}

impl Ord for PracmodVersion {
    fn cmp(&self, other: &Self) -> Ordering {
        // RC of 0 means real release, always considered to be newer than a RC version
        let semver_cmp = self.semver.cmp(&other.semver);
        if semver_cmp != Ordering::Equal {
            semver_cmp
        } else if self.release_cand == 0 && other.release_cand == 0 {
            Ordering::Equal
        } else if self.release_cand == 0 && other.release_cand > 0 {
            Ordering::Greater
        } else if self.release_cand > 0 && other.release_cand == 0 {
            Ordering::Less
        } else if self.release_cand > other.release_cand {
            Ordering::Greater
        } else {
            Ordering::Less
        }
    }
}

impl PartialOrd for PracmodVersion {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

static PRACMOD_VERSION: PracmodVersion = PracmodVersion {
    semver: SemVer {
        major: 1,
        minor: 1,
        patch: 0,
    },
    release_cand: 0,
};

pub fn get_version_str<const N: usize>(s: &mut ArrayString<N>) {
    if PRACMOD_VERSION.release_cand > 0 {
        fmt_buf!(
            s,
            c"%d.%d.%d-beta%d",
            PRACMOD_VERSION.semver.major as u32,
            PRACMOD_VERSION.semver.minor as u32,
            PRACMOD_VERSION.semver.patch as u32,
            PRACMOD_VERSION.release_cand as u32
        );
    } else {
        fmt_buf!(
            s,
            c"%d.%d.%d",
            PRACMOD_VERSION.semver.major as u32,
            PRACMOD_VERSION.semver.minor as u32,
            PRACMOD_VERSION.semver.patch as u32
        );
    }
}
