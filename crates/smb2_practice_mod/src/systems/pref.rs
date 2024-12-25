extern crate alloc;
use ::mkb::mkb_suppl::CARDResult;

use crate::{cstr, fmt, systems::draw::NotifyDuration};
use alloc::vec;
use alloc::vec::Vec;
use num_enum::TryFromPrimitive;
use zerocopy::{FromBytes, Immutable, IntoBytes, KnownLayout, Unaligned};

use super::{
    cardio::CardIo,
    draw::{self, Draw},
};

macro_rules! pref_defn {
    ($( // Line
        $id:literal => $pref_name:ident:
            $(i16 $(= $i16_default:literal)?)?
            $(bool $(= $bool_default:literal)?)?
    ),+ $(,)?) => {
        const BOOL_PREF_COUNT: usize = [$(
            $(swallow!((), ($($bool_default)?)),)?
        )+].len();

        const I16_PREF_COUNT: usize = [$(
            $(swallow!((), ($($i16_default)?)),)?
        )+].len();

        const ALL_PREF_IDS: &[PrefId] = &[
            $(PrefId::$pref_name),*
        ];

        const DEFAULT_BOOLS: &[DefaultBool] = &[
            $( // Line
                $($(DefaultBool { id: BoolPref::$pref_name, value: $bool_default},)?)?
            )+
        ];

        const DEFAULT_I16S: &[DefaultI16] = &[
            $( // Line
                $($(DefaultI16 { id: I16Pref::$pref_name, value: $i16_default},)?)?
            )+
        ];

        #[derive(Clone, Copy, TryFromPrimitive)]
        #[repr(u16)]
        enum PrefId {
            $( // Line
                $pref_name = $id,
            )*
        }

        #[derive(Clone, Copy)]
        #[repr(u16)]
        pub enum I16Pref {
            $( // Line
                // Making a doc attr is a way to use i16_default to select the right repetition, while
                // producing a valid syntax element
                $(#[doc = core::concat!("default=", core::stringify!($($i16_default)?))] $pref_name,)?
            )+
        }

        #[derive(Clone, Copy)]
        #[repr(u16)]
        pub enum BoolPref {
            $(
                $(#[doc = core::concat!("default=", core::stringify!($($bool_default)?))] $pref_name,)?
            )+
        }

        impl Pref {
            fn pref_id_to_bool_pref(id: PrefId) -> Option<BoolPref> {
                match id {
                    $( // Line
                        $(PrefId::$pref_name => swallow!((Some(BoolPref::$pref_name)), ($($bool_default)?)),)?
                    )*
                    _ => None,
                }
            }

            fn pref_id_to_i16_pref(id: PrefId) -> Option<I16Pref> {
                match id {
                    $( // Line
                        $(PrefId::$pref_name => swallow!((Some(I16Pref::$pref_name)), ($($i16_default)?)),)?
                    )*
                    _ => None,
                }
            }
        }
    };
}

macro_rules! swallow {
    ($good:tt, $garbage:tt) => {
        $good
    };
}

pref_defn!(
    1 => Savestates: bool = true,
    2 => InputDisp: bool,
    3 => InputDispLocation: i16 = 1,
    4 => TimerShowRTA: bool,
    5 => CmChara: i16,
    6 => InputDispColor: i16,
    7 => InputDispNotchIndicators: bool = true,
    8 => IwTimer: bool = true,
    9 => CmTimer: bool = true,
    10 => JumpMod: bool,
    11 => BananaCounter9999: bool,
    12 => DpadControls: bool,
    13 => DebugMode: bool,
    // Do not reuse 14, it belonged to old timer freezing BoolPref
    15 => MuteBgm: bool,
    16 => MuteTimerDing: bool,
    17 => InputDispRawStickInputs: bool,
    18 => Freecam: bool,
    19 => BallColor: i16,
    20 => ApeColor: i16,
    21 => Marathon: bool,
    // Do not reuse 22, it belonged to old Moon Gravity BoolPref
    23 => IlBattleDisplay: bool,
    24 => IlBattleLength: i16,
    // Do not reuse 25, it belonged to old IL Battle Score Breakdown BoolPref
    26 => IlMarkPractice: bool = true,
    27 => IlMarkStory: bool,
    28 => IlMarkChallenge: bool,
    29 => HideBg: bool,
    30 => UnlockVanilla: bool = true,
    31 => UnlockRomhacks: bool,
    32 => FreecamInvertYaw: bool,
    33 => FreecamInvertPitch: bool,
    34 => FreecamToggleWithZ: bool,
    35 => FreecamSpeedMult: i16 = 3,
    36 => FreecamFreezeTimer: bool = true,
    37 => FreecamHideHud: bool = true,
    38 => HideHud: bool,
    39 => HideStage: bool,
    40 => HideBall: bool,
    41 => HideItems: bool,
    42 => HideStobjs: bool,
    43 => HideEffects: bool,
    44 => IlMarkRomhacks: bool = true,
    45 => Camera: i16,
    46 => BallRed: i16,
    47 => BallGreen: i16,
    48 => BallBlue: i16,
    49 => BallColorType: i16,
    50 => ApeColorType: i16,
    51 => InputDispColorType: i16,
    52 => InputDispRed: i16,
    53 => InputDispGreen: i16,
    54 => InputDispBlue: i16,
    55 => TimerType: i16,
    56 => DisableFalloutVolumes: bool,
    57 => FalloutPlaneType: i16,
    58 => IlBattleShowTime: bool = true,
    59 => IlBattleShowScore: bool = true,
    60 => IlBattleBuzzerOld: bool,
    61 => IlBattleBreakdown: i16,
    62 => PhysicsPreset: i16,
    // 63 belonged to physics friction which was only used in beta playtesting
    // 64 belonged to physics restitution which was only used in beta playtesting
    // 65 belonged to SavestateDisableOverwrite
    66 => MenuBind: i16 = 64,
    67 => IlBattleReadyBind: i16 = 104,
    68 => FreecamToggleBind: i16 = 255,
    69 => SavestateClearBind: i16 = 255,
    70 => IlBattleTieCount: bool,
    71 => IlBattleAttemptCount: bool,
    72 => TimerShowSubtick: bool,
    73 => TimerShowFramesave: bool,
    // 74 was used for a boolean timer option that only existed in a beta verison.
    // Many people playtested that beta, so it may be best to not reuse until
    // a future update
    75 => TimerShowPause: bool,
    76 => StageEditVariant: i16,
    77 => JumpChangePhysics: bool = true,
    78 => JumpAllowWalljumps: bool = true,
    79 => JumpCount: i16 = 1,
    // 80 belonged to physics weight which was only used in beta playtesting
    81 => MonkeyType: i16,
    82 => JumpProfile: i16,
    83 => CustomPhysicsDisp: bool = true,
    84 => SavestateSaveTo: i16,
    85 => SavestateClearAllBind: i16 = 255,
    86 => InputDispGradientColor2Red: i16,
    87 => InputDispGradientColor2Green: i16,
    88 => InputDispGradientColor2Blue: i16,
    89 => InputDispGradientRotation: i16,
    90 => InputDispGradientStart: i16,
    91 => InputDispGradientEnd: i16 = 100,
    92 => RgbFormat: i16,
);

const PREF_BUF_SIZE: usize =
    size_of::<FileHeader>() + ALL_PREF_IDS.len() * size_of::<FilePrefEntry>();
const PREF_FILENAME: &str = "apmp";
const PREF_MAGIC: [u8; 4] = [b'A', b'P', b'M', b'P'];

pub trait PrefType<T>: Copy {
    fn get(self, state: &PrefState) -> T;
    fn set(self, v: T, pref: &mut Pref);
}

impl PrefType<bool> for BoolPref {
    fn get(self, state: &PrefState) -> bool {
        Pref::get_bool_pref(self, state)
    }

    fn set(self, v: bool, pref: &mut Pref) {
        Pref::set_bool_pref(self, &mut pref.curr_state, v);
    }
}

impl PrefType<i16> for I16Pref {
    fn get(self, state: &PrefState) -> i16 {
        Pref::get_i16_pref(self, state)
    }

    fn set(self, v: i16, pref: &mut Pref) {
        Pref::set_i16_pref(self, &mut pref.curr_state, v);
    }
}

struct DefaultBool {
    id: BoolPref,
    value: bool,
}

struct DefaultI16 {
    id: I16Pref,
    value: i16,
}

#[derive(Clone)]
pub struct PrefState {
    bools: [u8; get_bit_array_len(BOOL_PREF_COUNT)],
    i16s: [i16; I16_PREF_COUNT],
}

impl Default for PrefState {
    // We need to implement this manually because Default is not implemented for arrays bigger than
    // 32 elements
    fn default() -> Self {
        Self {
            bools: [0; get_bit_array_len(BOOL_PREF_COUNT)],
            i16s: [0; I16_PREF_COUNT],
        }
    }
}

#[repr(C, packed)]
#[derive(FromBytes, IntoBytes, Immutable, KnownLayout, Unaligned)]
struct FileHeader {
    magic: [u8; 4], // "APMP"
    semver_major: u16,
    semver_minor: u16,
    semver_patch: u16,
    num_prefs: u16,
}

#[repr(C, packed)]
#[derive(FromBytes, IntoBytes, Immutable, KnownLayout, Unaligned)]
struct FilePrefEntry {
    id: u16,
    value: u16, // Either the preference value itself (if <= 2 bytes), or offset into buffer
                // prefs, etc
}

const fn get_bit_array_len(flags: usize) -> usize {
    let mut n = flags / 8;
    if flags % 8 != 0 {
        n += 1;
    }
    n
}

fn append_to_byte_slice<'a, T>(dest: &'a mut [u8], source: &T) -> &'a mut [u8]
where
    T: IntoBytes + Immutable,
{
    let source_buf = source.as_bytes();
    dest[0..source_buf.len()].copy_from_slice(source_buf);
    &mut dest[source_buf.len()..]
}

fn read_from_byte_slice<T>(buf: &[u8]) -> (&T, &[u8])
where
    T: FromBytes + Immutable + KnownLayout,
{
    let val = T::ref_from_bytes(&buf[0..size_of::<T>()]).unwrap();
    (val, &buf[size_of::<T>()..])
}

pub struct Pref {
    cardio: CardIo,
    curr_state: PrefState,
    prev_state: PrefState,
    default_state: PrefState,
    pref_buf: Option<Vec<u8>>,
    import_error: Option<&'static str>,
    save_queued: bool,
}

impl Default for Pref {
    fn default() -> Self {
        let mut pref = Self {
            cardio: CardIo::new(),
            curr_state: PrefState::default(),
            prev_state: PrefState::default(),
            default_state: PrefState::default(),
            pref_buf: Some(vec![0; PREF_BUF_SIZE]),
            import_error: None,
            save_queued: false,
        };

        pref.load_default_prefs();
        pref.prev_state = pref.default_state.clone();
        match pref.cardio.read_file(PREF_FILENAME) {
            Ok(buf) => {
                pref.import_from_card_buf(&buf);
            }
            Err(card_result) => {
                if card_result != CARDResult::NoFile {
                    pref.import_error = Some("Failed to load prefs");
                }
            }
        }
        pref
    }
}

impl Pref {
    fn export_to_card_buf(&self, mut buf: &mut [u8]) {
        buf.fill(0);

        let header = FileHeader {
            magic: PREF_MAGIC,
            semver_major: 1,
            semver_minor: 0,
            semver_patch: 0,
            num_prefs: ALL_PREF_IDS.len() as u16,
        };
        buf = append_to_byte_slice(buf, &header);

        for &pref_id in ALL_PREF_IDS {
            let mut entry = FilePrefEntry {
                id: pref_id as u16,
                value: 0,
            };

            if let Some(bool_pref) = Self::pref_id_to_bool_pref(pref_id) {
                entry.value = self.get(bool_pref) as u16;
            } else if let Some(i16_pref) = Self::pref_id_to_i16_pref(pref_id) {
                entry.value = self.get(i16_pref) as u16;
            } else {
                panic!("Failed to determine preference type");
            }

            buf = append_to_byte_slice(buf, &entry);
        }
    }

    fn import_from_card_buf(&mut self, mut buf: &[u8]) {
        let header;
        (header, buf) = read_from_byte_slice::<FileHeader>(buf);
        if header.magic != PREF_MAGIC {
            self.import_error = Some("Failed to load prefs: invalid magic");
            return;
        }
        if header.semver_major != 1 {
            self.import_error = Some("Failed to load prefs: invalid version");
            return; // Preferences file format too new for this mod
        }

        for _ in 0..header.num_prefs {
            let entry;
            (entry, buf) = read_from_byte_slice::<FilePrefEntry>(buf);
            if let Ok(pref_id) = PrefId::try_from(entry.id) {
                if let Some(bool_pref) = Self::pref_id_to_bool_pref(pref_id) {
                    Self::set_bool_pref(bool_pref, &mut self.curr_state, entry.value > 0);
                } else if let Some(i16_pref) = Self::pref_id_to_i16_pref(pref_id) {
                    Self::set_i16_pref(i16_pref, &mut self.curr_state, entry.value as i16);
                } else {
                    // Unknown preference type somehow, ignore
                }
            } else {
                // Unknown pref ID, ignore
            }
        }
    }

    fn load_default_prefs(&mut self) {
        self.default_state = PrefState::default(); // Zero
        for default_bool in DEFAULT_BOOLS {
            Self::set_bool_pref(default_bool.id, &mut self.default_state, default_bool.value);
        }
        for default_i16 in DEFAULT_I16S {
            Self::set_i16_pref(default_i16.id, &mut self.default_state, default_i16.value);
        }
        self.curr_state = self.default_state.clone();
    }

    fn get_bool_pref(bool_pref: BoolPref, state: &PrefState) -> bool {
        let val = state.bools[bool_pref as usize / 8] & (1 << (bool_pref as usize % 8));
        val > 0
    }

    fn get_i16_pref(i16_pref: I16Pref, state: &PrefState) -> i16 {
        state.i16s[i16_pref as usize]
    }

    fn set_bool_pref(bool_pref: BoolPref, state: &mut PrefState, val: bool) {
        if val {
            state.bools[bool_pref as usize / 8] |= 1 << (bool_pref as usize % 8);
        } else {
            state.bools[bool_pref as usize / 8] &= !(1 << (bool_pref as usize % 8));
        }
    }

    fn set_i16_pref(i16_pref: I16Pref, state: &mut PrefState, val: i16) {
        state.i16s[i16_pref as usize] = val;
    }

    pub fn get<T>(&self, pref: impl PrefType<T>) -> T {
        pref.get(&self.curr_state)
    }

    pub fn set<T>(&mut self, pref: impl PrefType<T>, val: T) {
        pref.set(val, self)
    }

    pub fn did_change<T: PartialEq>(&self, pref: impl PrefType<T>) -> bool {
        let curr = pref.get(&self.curr_state);
        let prev = pref.get(&self.prev_state);
        curr != prev
    }

    pub fn get_default<T>(&self, pref: impl PrefType<T>) -> T {
        pref.get(&self.default_state)
    }

    pub fn reset_all_defaults(&mut self) {
        self.curr_state = self.default_state.clone();
    }

    pub fn tick(&mut self, draw: &mut Draw) {
        self.cardio.tick();
        // Runs after all prefs have been set on a frame
        self.prev_state = self.curr_state.clone();

        if let Some(err) = self.import_error {
            draw.notify(draw::RED, NotifyDuration::Long, err);
            self.import_error = None;
        }

        // Check if a pending card write has completed
        if self.pref_buf.is_none() {
            if let Some((buf, card_result)) = self.cardio.get_write_result() {
                self.pref_buf = Some(buf);
                if !matches!(card_result, CARDResult::Ready) {
                    draw.notify(
                        draw::RED,
                        NotifyDuration::Long,
                        &fmt!(
                            32,
                            c"Failed to save settings: %s",
                            cstr!(16, card_result.to_str()),
                        ),
                    );
                }
            }
        }

        // Start a write if a save is pending and a write is not pending
        if self.save_queued {
            if let Some(mut buf) = self.pref_buf.take() {
                self.export_to_card_buf(&mut buf);
                self.cardio.begin_write_file(PREF_FILENAME, buf);
                self.save_queued = false;
            }
        }
    }

    pub fn save(&mut self) {
        self.save_queued = true;
    }
}

pub trait FromPref {
    fn from_pref(pref_id: I16Pref, pref: &Pref) -> Self;
}

impl<T, E> FromPref for T
where
    T: TryFrom<i16, Error = E>,
    E: core::fmt::Debug,
{
    fn from_pref(pref_id: I16Pref, pref: &Pref) -> T {
        let default = T::try_from(pref.get_default(pref_id)).unwrap();
        T::try_from(pref.get(pref_id)).unwrap_or(default)
    }
}
