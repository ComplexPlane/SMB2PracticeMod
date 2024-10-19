use core::slice::from_raw_parts;

use arrayvec::ArrayVec;

extern crate alloc;

use alloc::vec::Vec;

macro_rules! pref_defn {
    ($( // Line
       $id:literal => $pref_name:ident: $(u8 = $u8_default:literal)? $(bool = $bool_default:literal)?
    ),+ $(,)?) => {
        const ALL_PREF_IDS: &[PrefId] = &[
            $(PrefId::$pref_name),*
        ];

        const BOOL_PREF_COUNT: usize = [$(
            $(swallow!((), $bool_default),)?
        )+].len();

        const U8_PREF_COUNT: usize = [$(
            $(swallow!((), $u8_default),)?
        )+].len();

        #[derive(Clone, Copy)]
        enum PrefId {
            $( // Line
                $pref_name,
            )*
        }

        #[derive(Clone, Copy)]
        #[repr(u16)]
        pub enum U8Pref {
            $(
                $($pref_name = swallow!($id, $u8_default),)?
            )+
        }

        #[derive(Clone, Copy)]
        #[repr(u16)]
        pub enum BoolPref {
            $(
                $($pref_name = swallow!($id, $bool_default),)?
            )+
        }

        impl Pref {
            fn pref_id_to_bool_pref(id: PrefId) -> Option<BoolPref> {
                match id {
                    $( // Line
                        $(PrefId::$pref_name => swallow!((Some(BoolPref::$pref_name)), $bool_default),)?
                    )*
                    _ => None,
                }
            }

            fn pref_id_to_u8_pref(id: PrefId) -> Option<U8Pref> {
                match id {
                    $( // Line
                        $(PrefId::$pref_name => swallow!((Some(U8Pref::$pref_name)), $u8_default),)?
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
    1 => Savestates: bool = false,
    2 => InputDisp: bool = false,
    3 => InputDispCenterLocation: bool = false,
    4 => TimerShowRTA: bool = true,
    6 => InputDispColor: u8 = 0,
    7 => InputDispNotchIndicators: bool = false,
    8 => IwTimer: bool = false,
    9 => CmTimer: bool = false,
    10 => JumpMod: bool = false,
    11 => BananaCounter9999: bool = false,
    12 => DpadControls: bool = false,
    13 => DebugMode: bool = false,
    // Do not reuse 14, it belonged to old timer freezing BoolPref
    15 => MuteBgm: bool = false,
    16 => MuteTimerDing: bool = false,
    17 => InputDispRawStickInputs: bool = false,
    18 => Freecam: bool = false,
    19 => BallColor: u8 = 0,
    20 => ApeColor: u8 = 0,
    21 => Marathon: bool = false,
    // Do not reuse 22, it belonged to old Moon Gravity BoolPref
    23 => IlBattleDisplay: bool = false,
    24 => IlBattleLength: u8 = 0,
    // Do not reuse 25, it belonged to old IL Battle Score Breakdown BoolPref
    26 => IlMarkPractice: bool = false,
    27 => IlMarkStory: bool = false,
    28 => IlMarkChallenge: bool = false,
    29 => HideBg: bool = false,
    30 => UnlockVanilla: bool = false,
    31 => UnlockRomhacks: bool = false,
    32 => FreecamInvertYaw: bool = false,
    33 => FreecamInvertPitch: bool = false,
    34 => FreecamToggleWithZ: bool = false,
    35 => FreecamSpeedMult: u8 = 1,
    36 => FreecamFreezeTimer: bool = false,
    37 => FreecamHideHud: bool = false,
    38 => HideHud: bool = false,
    39 => HideStage: bool = false,
    40 => HideBall: bool = false,
    41 => HideItems: bool = false,
    42 => HideStobjs: bool = false,
    43 => HideEffects: bool = false,
    44 => IlMarkRomhacks: bool = false,
    45 => Camera: u8 = 0,
    46 => BallRed: u8 = 255,
    47 => BallGreen: u8 = 255,
    48 => BallBlue: u8 = 255,
    49 => BallColorType: u8 = 0,
    50 => ApeColorType: u8 = 0,
    51 => InputDispColorType: u8 = 0,
    52 => InputDispRed: u8 = 255,
    53 => InputDispGreen: u8 = 255,
    54 => InputDispBlue: u8 = 255,
    55 => TimerType: u8 = 0,
    56 => DisableFalloutVolumes: bool = false,
    57 => FalloutPlaneType: u8 = 0,
    58 => IlBattleShowTime: bool = true,
    59 => IlBattleShowScore: bool = true,
    60 => IlBattleBuzzerOld: bool = false,
    61 => IlBattleBreakdown: u8 = 0,
    62 => PhysicsPreset: u8 = 0,
    // 63 belonged to physics friction which was only used in beta playtesting
    // 64 belonged to physics restitution which was only used in beta playtesting
    65 => SavestateDisableOverwrite: bool = false,
    66 => MenuBind: u8 = 0,
    67 => IlBattleReadyBind: u8 = 0,
    68 => FreecamToggleBind: u8 = 0,
    69 => SavestateClearBind: u8 = 0,
    70 => IlBattleTieCount: bool = false,
    71 => IlBattleAttemptCount: bool = false,
    72 => TimerShowSubtick: bool = false,
    73 => TimerShowFramesave: bool = false,
    // 74 was used for a boolean timer option that only existed in a beta verison.
    // Many people playtested that beta, so it may be best to not reuse until
    // a future update
    75 => TimerShowPause: bool = false,
    76 => StageEditVariant: u8 = 0,
    77 => JumpChangePhysics: bool = false,
    78 => JumpAllowWalljumps: bool = false,
    79 => JumpCount: u8 = 1,
    // 80 belonged to physics weight which was only used in beta playtesting
    81 => MonkeyType: u8 = 0,
    82 => JumpProfile: u8 = 0,
    83 => CustomPhysicsDisp: bool = false,
);

const PREF_BUF_SIZE: usize = size_of::<FileHeader>() + ALL_PREF_IDS.len() * size_of::<PrefEntry>();

const fn get_bit_array_len(flags: usize) -> usize {
    let mut n = flags / 8;
    if flags % 8 != 0 {
        n += 1;
    }
    n
}

struct DefaultBool {
    id: BoolPref,
    value: bool,
}

struct DefaultU8 {
    id: U8Pref,
    value: u8,
}

#[derive(Clone, Default)]
struct PrefState {
    bools: [u8; get_bit_array_len(BOOL_PREF_COUNT)],
    u8s: [u8; U8_PREF_COUNT],
}

#[repr(C, packed)]
struct FileHeader {
    magic: [u8; 4], // "APMP"
    semver_major: u16,
    semver_minor: u16,
    semver_patch: u16,
    num_prefs: u16,
}

#[repr(C, packed)]
struct PrefEntry {
    id: u16,
    data: u16, // Either the preference value itself (if <= 2 bytes), or offset into buffer
               // prefs, etc
}

fn to_byte_slice<T>(v: &T) -> &[u8] {
    unsafe { from_raw_parts((v as *const T) as *const u8, size_of::<T>()) }
}

struct Pref {
    curr_state: PrefState,
    prev_state: PrefState,
    default_state: PrefState,
    pref_buf: Option<Vec<u8>>,
}

impl Pref {
    pub fn new() -> Pref {
        let mut buf = Vec::<u8>::new();
        buf.reserve_exact(PREF_BUF_SIZE);
        Pref {
            curr_state: PrefState::default(),
            prev_state: PrefState::default(),
            default_state: PrefState::default(),
            pref_buf: Some(buf),
        }
    }

    fn export_to_card_buf<const N: usize>(buf: &mut ArrayVec<u8, N>) {
        let header = FileHeader {
            magic: [b'A', b'P', b'M', b'P'],
            semver_major: 1,
            semver_minor: 0,
            semver_patch: 0,
            num_prefs: ALL_PREF_IDS.len() as u16,
        };
        buf.try_extend_from_slice(to_byte_slice(&header)).unwrap();

        for &pref_id in ALL_PREF_IDS {
            let mut entry = PrefEntry {
                id: pref_id as u16,
                data: 0,
            };

            if let Some(bool_value) = Self::pref_id_to_bool_pref(pref_id) {
                entry.data = bool_value as u16;
            } else if let Some(u8_value) = Self::pref_id_to_u8_pref(pref_id) {
                entry.data = u8_value as u16;
            } else {
                panic!("Failed to determine preference type");
            }

            buf.try_extend_from_slice(to_byte_slice(&entry)).unwrap();
        }
    }

    fn get_bool_pref(bool_pref: BoolPref, state: &PrefState) -> bool {
        let val = state.bools[bool_pref as usize / 8] & (1 << (bool_pref as usize % 8));
        val > 0
    }

    fn get_u8_pref(u8_pref: U8Pref, state: &PrefState) -> u8 {
        state.u8s[u8_pref as usize]
    }

    fn set_bool_pref(bool_pref: BoolPref, state: &mut PrefState, val: bool) {
        state.bools[bool_pref as usize / 8] |= 1 << (bool_pref as usize % 8);
    }

    fn set_u8_pref(u8_pref: U8Pref, state: &mut PrefState, val: u8) {
        state.u8s[u8_pref as usize] = val;
    }

    pub fn get_bool(&self, bool_pref: BoolPref) -> bool {
        Self::get_bool_pref(bool_pref, &self.curr_state)
    }

    pub fn get_u8(&self, u8_pref: U8Pref) -> u8 {
        Self::get_u8_pref(u8_pref, &self.curr_state)
    }

    pub fn set_bool(&mut self, bool_pref: BoolPref, val: bool) {
        Self::set_bool_pref(bool_pref, &mut self.curr_state, val);
    }

    pub fn set_u8(&mut self, u8_pref: U8Pref, val: u8) {
        Self::set_u8_pref(u8_pref, &mut self.curr_state, val);
    }

    pub fn did_change_bool(&self, bool_pref: BoolPref) -> bool {
        let curr = Self::get_bool_pref(bool_pref, &self.curr_state);
        let prev = Self::get_bool_pref(bool_pref, &self.prev_state);
        curr != prev
    }

    pub fn did_change_u8(&self, u8_pref: U8Pref) -> bool {
        let curr = Self::get_u8_pref(u8_pref, &self.curr_state);
        let prev = Self::get_u8_pref(u8_pref, &self.prev_state);
        curr != prev
    }

    pub fn get_default_bool(&self, bool_pref: BoolPref) -> bool {
        Self::get_bool_pref(bool_pref, &self.default_state)
    }

    pub fn get_default_u8(&self, u8_pref: U8Pref) -> u8 {
        Self::get_u8_pref(u8_pref, &self.default_state)
    }

    pub fn reset_all_defaults(&mut self) {
        self.curr_state = self.default_state.clone();
    }

    pub fn tick(&mut self) {
        // Runs after all prefs have been set on a frame
        self.prev_state = self.curr_state.clone();
    }
}
