#include "pref.h"

#include "heap.h"
#include "mkb/mkb.h"

#include "cardio.h"
#include "log.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

namespace pref {

//
// Preferences definition
//

// Unique ID assigned to each preference. The ID assigned to a preference must never change, nor may
// they be reused, to ensure backwards compatibility!
enum class PrefId : u16 {
    Savestates = 1,
    InputDisp = 2,
    InputDispCenterLocation = 3,
    TimerShowRTA = 4,
    CmChara = 5,
    InputDispColor = 6,
    InputDispNotchIndicators = 7,
    IwTimer = 8,
    CmTimer = 9,
    JumpMod = 10,
    BananaCounter9999 = 11,
    DpadControls = 12,
    DebugMode = 13,
    // Do not reuse 14, it belonged to old timer freezing BoolPref
    MuteBgm = 15,
    MuteTimerDing = 16,
    InputDispRawStickInputs = 17,
    Freecam = 18,
    BallColor = 19,
    ApeColor = 20,
    Marathon = 21,
    Moon = 22,
    IlBattleDisplay = 23,
    IlBattleLength = 24,
    // Do not reuse 25, it belonged to old IL Battle Score Breakdown BoolPref
    IlMarkPractice = 26,
    IlMarkStory = 27,
    IlMarkChallenge = 28,
    HideBg = 29,
    UnlockVanilla = 30,
    UnlockRomhacks = 31,
    FreecamInvertYaw = 32,
    FreecamInvertPitch = 33,
    FreecamToggleWithZ = 34,
    FreecamSpeedMult = 35,
    FreecamFreezeTimer = 36,
    FreecamHideHud = 37,
    HideHud = 38,
    HideStage = 39,
    HideBall = 40,
    HideItems = 41,
    HideStobjs = 42,
    HideEffects = 43,
    IlMarkRomhacks = 44,
    Camera = 45,
    BallRed = 46,
    BallGreen = 47,
    BallBlue = 48,
    BallColorType = 49,
    ApeColorType = 50,
    InputDispColorType = 51,
    InputDispRed = 52,
    InputDispGreen = 53,
    InputDispBlue = 54,
    TimerType = 55,
    DisableFalloutVolumes = 56,
    FalloutPlaneType = 57,
    IlBattleShowTime = 58,
    IlBattleShowScore = 59,
    IlBattleBuzzerOld = 60,
    IlBattleBreakdown = 61,
    UseCustomPhysics = 62,
    Friction = 63,
    Restitution = 64,
    SavestateDisableOverwrite = 65,
    MenuBind = 66,
    IlBattleReadyBind = 67,
    FreecamToggleBind = 68,
    SavestateClearBind = 69,
    IlBattleTieCount = 70,
    IlBattleAttemptCount = 71,
    TimerShowSubtick = 72,
    TimerShowFramesave = 73,
    TimerShowUnrounded = 74,
    TimerShowPause = 75,
    StageEditVariant = 76,
    FullgameTimerOptions = 77,
    SegmentTimerOptions = 78,
    StoryTimerWarning = 79,
    ShowDeathCounter = 80,
};

// Verbatim list of preference IDs we iterate over when writing savefile back out
static const PrefId s_pref_ids[] = {
    PrefId::Savestates,
    PrefId::InputDisp,
    PrefId::InputDispCenterLocation,
    PrefId::TimerShowRTA,
    PrefId::CmChara,
    PrefId::InputDispColor,
    PrefId::InputDispNotchIndicators,
    PrefId::IwTimer,
    PrefId::CmTimer,
    PrefId::JumpMod,
    PrefId::BananaCounter9999,
    PrefId::DpadControls,
    PrefId::DebugMode,
    PrefId::MuteBgm,
    PrefId::MuteTimerDing,
    PrefId::InputDispRawStickInputs,
    PrefId::Freecam,
    PrefId::BallColor,
    PrefId::ApeColor,
    PrefId::Marathon,
    PrefId::Moon,
    PrefId::IlBattleDisplay,
    PrefId::IlMarkPractice,
    PrefId::IlMarkStory,
    PrefId::IlMarkChallenge,
    PrefId::HideBg,
    PrefId::UnlockVanilla,
    PrefId::UnlockRomhacks,
    PrefId::FreecamInvertYaw,
    PrefId::FreecamInvertPitch,
    PrefId::FreecamToggleWithZ,
    PrefId::FreecamSpeedMult,
    PrefId::FreecamFreezeTimer,
    PrefId::FreecamHideHud,
    PrefId::HideHud,
    PrefId::HideStage,
    PrefId::HideBall,
    PrefId::HideItems,
    PrefId::HideStobjs,
    PrefId::HideEffects,
    PrefId::IlMarkRomhacks,
    PrefId::Camera,
    PrefId::BallRed,
    PrefId::BallGreen,
    PrefId::BallBlue,
    PrefId::BallColorType,
    PrefId::MenuBind,
    PrefId::TimerType,
    PrefId::DisableFalloutVolumes,
    PrefId::IlBattleShowTime,
    PrefId::IlBattleShowScore,
    PrefId::IlBattleBuzzerOld,
    PrefId::Friction,
    PrefId::Restitution,
    PrefId::UseCustomPhysics,
    PrefId::SavestateDisableOverwrite,
    PrefId::ApeColorType,
    PrefId::IlBattleBreakdown,
    PrefId::InputDispColorType,
    PrefId::InputDispRed,
    PrefId::InputDispGreen,
    PrefId::InputDispBlue,
    PrefId::IlBattleReadyBind,
    PrefId::FreecamToggleBind,
    PrefId::SavestateClearBind,
    PrefId::FalloutPlaneType,
    PrefId::IlBattleTieCount,
    PrefId::IlBattleAttemptCount,
    PrefId::TimerShowSubtick,
    PrefId::TimerShowFramesave,
    PrefId::TimerShowUnrounded,
    PrefId::TimerShowPause,
    PrefId::StageEditVariant,
    PrefId::FullgameTimerOptions,
    PrefId::SegmentTimerOptions,
    PrefId::StoryTimerWarning,
    PrefId::ShowDeathCounter,
};

[[nodiscard]]
static bool pref_id_to_bool_pref(PrefId id, BoolPref* out) {
    switch (id) {
        case PrefId::Savestates:
            *out = BoolPref::Savestates;
        case PrefId::InputDisp:
            *out = BoolPref::InputDisp;
        case PrefId::InputDispCenterLocation:
            *out = BoolPref::InputDispCenterLocation;
        case PrefId::TimerShowRTA:
            *out = BoolPref::TimerShowRTA;
        case PrefId::InputDispNotchIndicators:
            *out = BoolPref::InputDispNotchIndicators;
        case PrefId::IwTimer:
            *out = BoolPref::IwTimer;
        case PrefId::CmTimer:
            *out = BoolPref::CmTimer;
        case PrefId::JumpMod:
            *out = BoolPref::JumpMod;
        case PrefId::BananaCounter9999:
            *out = BoolPref::BananaCounter9999;
        case PrefId::DpadControls:
            *out = BoolPref::DpadControls;
        case PrefId::DebugMode:
            *out = BoolPref::DebugMode;
        case PrefId::MuteBgm:
            *out = BoolPref::MuteBgm;
        case PrefId::MuteTimerDing:
            *out = BoolPref::MuteTimerDing;
        case PrefId::InputDispRawStickInputs:
            *out = BoolPref::InputDispRawStickInputs;
        case PrefId::Freecam:
            *out = BoolPref::Freecam;
        case PrefId::Marathon:
            *out = BoolPref::Marathon;
        case PrefId::Moon:
            *out = BoolPref::Moon;
        case PrefId::IlBattleDisplay:
            *out = BoolPref::IlBattleDisplay;
        case PrefId::IlMarkPractice:
            *out = BoolPref::IlMarkPractice;
        case PrefId::IlMarkStory:
            *out = BoolPref::IlMarkStory;
        case PrefId::IlMarkChallenge:
            *out = BoolPref::IlMarkChallenge;
        case PrefId::HideBg:
            *out = BoolPref::HideBg;
        case PrefId::UnlockVanilla:
            *out = BoolPref::UnlockVanilla;
        case PrefId::UnlockRomhacks:
            *out = BoolPref::UnlockRomhacks;
        case PrefId::FreecamInvertYaw:
            *out = BoolPref::FreecamInvertYaw;
        case PrefId::FreecamInvertPitch:
            *out = BoolPref::FreecamInvertPitch;
        case PrefId::FreecamToggleWithZ:
            *out = BoolPref::FreecamToggleWithZ;
        case PrefId::FreecamFreezeTimer:
            *out = BoolPref::FreecamFreezeTimer;
        case PrefId::FreecamHideHud:
            *out = BoolPref::FreecamHideHud;
        case PrefId::HideHud:
            *out = BoolPref::HideHud;
        case PrefId::HideStage:
            *out = BoolPref::HideStage;
        case PrefId::HideBall:
            *out = BoolPref::HideBall;
        case PrefId::HideItems:
            *out = BoolPref::HideItems;
        case PrefId::HideStobjs:
            *out = BoolPref::HideStobjs;
        case PrefId::HideEffects:
            *out = BoolPref::HideEffects;
        case PrefId::IlMarkRomhacks:
            *out = BoolPref::IlMarkRomhacks;
        case PrefId::DisableFalloutVolumes:
            *out = BoolPref::DisableFalloutVolumes;
        case PrefId::IlBattleShowTime:
            *out = BoolPref::IlBattleShowTime;
        case PrefId::IlBattleShowScore:
            *out = BoolPref::IlBattleShowScore;
        case PrefId::IlBattleBuzzerOld:
            *out = BoolPref::IlBattleBuzzerOld;
        case PrefId::UseCustomPhysics:
            *out = BoolPref::UseCustomPhysics;
        case PrefId::SavestateDisableOverwrite:
            *out = BoolPref::SavestateDisableOverwrite;
        case PrefId::IlBattleTieCount:
            *out = BoolPref::IlBattleTieCount;
        case PrefId::IlBattleAttemptCount:
            *out = BoolPref::IlBattleAttemptCount;
        case PrefId::TimerShowSubtick:
            *out = BoolPref::TimerShowSubtick;
        case PrefId::TimerShowFramesave:
            *out = BoolPref::TimerShowFramesave;
        case PrefId::TimerShowUnrounded:
            *out = BoolPref::TimerShowUnrounded;
        case PrefId::TimerShowPause:
            *out = BoolPref::TimerShowPause;
        case PrefId::StoryTimerWarning:
            *out = BoolPref::StoryTimerWarning;
        case PrefId::ShowDeathCounter:
            *out = BoolPref::ShowDeathCounter;
        default:
            return false;
    }
    return true;
}

[[nodiscard]]
static bool pref_id_to_u8_pref(PrefId id, U8Pref* out) {
    switch (id) {
        case PrefId::CmChara:
            *out = U8Pref::CmChara;
        case PrefId::InputDispColor:
            *out = U8Pref::InputDispColor;
        case PrefId::BallColor:
            *out = U8Pref::BallColor;
        case PrefId::ApeColor:
            *out = U8Pref::ApeColor;
        case PrefId::IlBattleLength:
            *out = U8Pref::IlBattleLength;
        case PrefId::FreecamSpeedMult:
            *out = U8Pref::FreecamSpeedMult;
        case PrefId::Camera:
            *out = U8Pref::Camera;
        case PrefId::BallRed:
            *out = U8Pref::BallRed;
        case PrefId::BallGreen:
            *out = U8Pref::BallGreen;
        case PrefId::BallBlue:
            *out = U8Pref::BallBlue;
        case PrefId::BallColorType:
            *out = U8Pref::BallColorType;
        case PrefId::MenuBind:
            *out = U8Pref::MenuBind;
        case PrefId::TimerType:
            *out = U8Pref::TimerType;
        case PrefId::Friction:
            *out = U8Pref::Friction;
        case PrefId::Restitution:
            *out = U8Pref::Restitution;
        case PrefId::ApeColorType:
            *out = U8Pref::ApeColorType;
        case PrefId::IlBattleBreakdown:
            *out = U8Pref::IlBattleBreakdown;
        case PrefId::InputDispColorType:
            *out = U8Pref::InputDispColorType;
        case PrefId::InputDispRed:
            *out = U8Pref::InputDispRed;
        case PrefId::InputDispGreen:
            *out = U8Pref::InputDispGreen;
        case PrefId::InputDispBlue:
            *out = U8Pref::InputDispBlue;
        case PrefId::IlBattleReadyBind:
            *out = U8Pref::IlBattleReadyBind;
        case PrefId::FreecamToggleBind:
            *out = U8Pref::FreecamToggleBind;
        case PrefId::SavestateClearBind:
            *out = U8Pref::SavestateClearBind;
        case PrefId::FalloutPlaneType:
            *out = U8Pref::FalloutPlaneType;
        case PrefId::StageEditVariant:
            *out = U8Pref::StageEditVariant;
        case PrefId::FullgameTimerOptions:
            *out = U8Pref::FullgameTimerOptions;
        case PrefId::SegmentTimerOptions:
            *out = U8Pref::SegmentTimerOptions;
        default:
            return false;
    }
    return true;
}

// Boolean preferences that should default to true
static BoolPref s_default_on_bool_prefs[] = {
    BoolPref::Savestates,
    BoolPref::IwTimer,
    BoolPref::CmTimer,
    BoolPref::InputDispNotchIndicators,
    BoolPref::IlMarkPractice,
    BoolPref::UnlockVanilla,
    BoolPref::FreecamFreezeTimer,
    BoolPref::FreecamHideHud,
    BoolPref::IlBattleShowTime,
    BoolPref::IlBattleShowScore,
};

struct DefaultU8Pref {
    U8Pref pref;
    u8 value;
};

// Non-zero default values of u8 preferences
static DefaultU8Pref s_default_u8_prefs[] = {
    {U8Pref::FreecamSpeedMult, 3},      // 3
    {U8Pref::MenuBind, 64},             // L + R
    {U8Pref::Friction, 110},            // 0.10
    {U8Pref::Restitution, 150},         // 0.5
    {U8Pref::IlBattleReadyBind, 104},   // dpad-down
    {U8Pref::FreecamToggleBind, 255},   // unbound
    {U8Pref::SavestateClearBind, 255},  // unbound
};

//
// End preferences definition
//

struct PrefState {
    u8 bool_prefs[9];
    u8 u8_prefs[28];
};

static PrefState s_pref_state, s_default_pref_state;

struct FileHeader {
    char magic[4];  // "APMP"
    u16 semver_major;
    u16 semver_minor;
    u16 semver_patch;
    u16 num_prefs;
} __attribute__((__packed__));

struct IdEntry {
    u16 id;
    u16 data;  // Either the preference value itself (if <= 2 bytes), or offset into buffer
               // prefs, etc
} __attribute((__packed__));

static u8 s_card_buf[sizeof(FileHeader) + LEN(s_pref_ids) * sizeof(IdEntry)]
    __attribute__((__aligned__(32)));  // CARD API requires 32-byte alignment

static inline u16 validate_bool_pref(BoolPref bp) {
    u16 bpi = static_cast<u16>(bp);
    MOD_ASSERT(static_cast<u16>(bpi / 8) < LEN(PrefState{}.bool_prefs));  // Out of room for bool
                                                                          // prefs
    return bpi;
}

static bool get_bool_pref(BoolPref bp, const PrefState& state) {
    u16 bpi = validate_bool_pref(bp);
    return state.bool_prefs[bpi / 8] & (1 << (bpi % 8));
}

static void set_bool_pref(BoolPref bp, bool value, PrefState& state) {
    u16 bpi = validate_bool_pref(bp);
    if (value) {
        state.bool_prefs[bpi / 8] |= (1 << (bpi % 8));
    } else {
        state.bool_prefs[bpi / 8] &= ~(1 << (bpi % 8));
    }
}

static u32 validate_u8_pref(U8Pref pref) {
    u32 idx = static_cast<u32>(pref);
    MOD_ASSERT(idx < LEN(PrefState{}.u8_prefs));
    return idx;
}

static u8 get_u8_pref(U8Pref pref, const PrefState& state) {
    return state.u8_prefs[validate_u8_pref(pref)];
}

static void set_u8_pref(U8Pref pref, u8 value, PrefState& state) {
    state.u8_prefs[validate_u8_pref(pref)] = value;
}

static void load_default_prefs() {
    mkb::memset(&s_pref_state, 0, sizeof(s_pref_state));

    for (u32 i = 0; i < LEN(s_default_on_bool_prefs); i++) {
        set_bool_pref(s_default_on_bool_prefs[i], true, s_pref_state);
    }
    for (u32 i = 0; i < LEN(s_default_u8_prefs); i++) {
        set_u8_pref(s_default_u8_prefs[i].pref, s_default_u8_prefs[i].value, s_pref_state);
    }

    mkb::memcpy(&s_default_pref_state, &s_pref_state, sizeof(s_default_pref_state));
}

static void card_buf_to_pref_struct(void* card_buf) {
    FileHeader* header = static_cast<FileHeader*>(card_buf);
    if (header->semver_major > 1) return;  // Preferences file format too new for this mod

    IdEntry* entry_list =
        reinterpret_cast<IdEntry*>(reinterpret_cast<u32>(card_buf) + sizeof(FileHeader));

    for (s32 i = 0; i < header->num_prefs; i++) {
        PrefId id = static_cast<PrefId>(entry_list[i].id);
        u16 pref_data = entry_list[i].data;

        // If it's a boolean preference, copy it from the memcard file
        BoolPref bool_pref = {};
        if (pref_id_to_bool_pref(id, &bool_pref)) {
            set_bool_pref(bool_pref, pref_data, s_pref_state);
            continue;
        }

        // For u8 preferences, copy them to struct fields directly
        U8Pref u8_pref = {};
        if (pref_id_to_u8_pref(id, &u8_pref)) {
            set_u8_pref(u8_pref, static_cast<u8>(pref_data), s_pref_state);
            continue;
        }

        // Ignore all other setting IDs we aren't aware of
    }
}

static void pref_struct_to_card_buf() {
    FileHeader* header = static_cast<FileHeader*>(static_cast<void*>(s_card_buf));
    IdEntry* entry_list =
        reinterpret_cast<IdEntry*>(reinterpret_cast<u32>(s_card_buf) + sizeof(FileHeader));

    header->magic[0] = 'A';
    header->magic[1] = 'P';
    header->magic[2] = 'M';
    header->magic[3] = 'P';
    header->semver_major = 1;
    header->semver_minor = 0;
    header->semver_patch = 0;
    header->num_prefs = LEN(s_pref_ids);

    for (u32 i = 0; i < LEN(s_pref_ids); i++) {
        PrefId id = s_pref_ids[i];
        entry_list[i].id = static_cast<u16>(id);

        // Write out boolean preference if this is a boolean
        BoolPref bool_pref = {};
        if (pref_id_to_bool_pref(id, &bool_pref)) {
            entry_list[i].data = get_bool_pref(bool_pref, s_pref_state);
            continue;
        }

        U8Pref u8_pref = {};
        if (pref_id_to_u8_pref(id, &u8_pref)) {
            entry_list[i].data = get_u8_pref(u8_pref, s_pref_state);
            continue;
        }

        // There should be no other types of preferences!
        MOD_ASSERT(false);
    }
}

static constexpr char* PREF_FILENAME = "apmp";

void init() {
    load_default_prefs();

    FileHeader* header = nullptr;
    s32 result = cardio::read_file(PREF_FILENAME, reinterpret_cast<void**>(&header));
    if (result == mkb::CARD_RESULT_READY) {
        card_buf_to_pref_struct(header);
        heap::free(header);
    } else if (result != mkb::CARD_RESULT_NOFILE) {
        draw::notify(draw::RED, "Error loading settings from Card A, setting defaults");
    }
}

void save() {
    pref_struct_to_card_buf();
    cardio::write_file(PREF_FILENAME, s_card_buf, sizeof(s_card_buf), [](mkb::CARDResult res) {
        if (res != mkb::CARD_RESULT_READY) {
            if (res == mkb::CARD_RESULT_NOENT || res == mkb::CARD_RESULT_INSSPACE) {
                draw::notify(draw::RED, "Cannot Save Settings: Card A Full");
            } else {
                draw::notify(draw::RED, "Cannot Save Settings: Card A Unknown Error");
            }
        }
    });
}

void reset_all_defaults() {
    mkb::memcpy(&s_pref_state, &s_default_pref_state, sizeof(s_pref_state));
}

bool get(BoolPref bool_pref) { return get_bool_pref(bool_pref, s_pref_state); }
u8 get(U8Pref u8_pref) { return get_u8_pref(u8_pref, s_pref_state); }
void set(BoolPref bool_pref, bool value) { set_bool_pref(bool_pref, value, s_pref_state); };
void set(U8Pref u8_pref, u8 value) { set_u8_pref(u8_pref, value, s_pref_state); }
bool get_default(BoolPref bool_pref) { return get_bool_pref(bool_pref, s_default_pref_state); }
u8 get_default(U8Pref u8_pref) { return get_u8_pref(u8_pref, s_default_pref_state); }

}  // namespace pref
