#include "pref.h"

#include <optional>
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
    RtaPauseTimer = 4,
    CmChara = 5,
    InputDispColor = 6,
    InputDispNotchIndicators = 7,
    IwTimer = 8,
    CmTimer = 9,
    JumpMod = 10,
    BananaCounter9999 = 11,
    DpadControls = 12,
    DebugMode = 13,
    FreezeTimer = 14,
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
    IlBattleBreakdown = 25,
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
};

// Verbatim list of preference IDs we iterate over when writing savefile back out
static const PrefId s_pref_ids[] = {
    PrefId::Savestates,
    PrefId::InputDisp,
    PrefId::InputDispCenterLocation,
    PrefId::RtaPauseTimer,
    PrefId::CmChara,
    PrefId::InputDispColor,
    PrefId::InputDispNotchIndicators,
    PrefId::IwTimer,
    PrefId::CmTimer,
    PrefId::JumpMod,
    PrefId::BananaCounter9999,
    PrefId::DpadControls,
    PrefId::DebugMode,
    PrefId::FreezeTimer,
    PrefId::MuteBgm,
    PrefId::MuteTimerDing,
    PrefId::InputDispRawStickInputs,
    PrefId::Freecam,
    PrefId::BallColor,
    PrefId::ApeColor,
    PrefId::Marathon,
    PrefId::Moon,
    PrefId::IlBattleDisplay,
    PrefId::IlBattleBreakdown,
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
};

static std::optional<BoolPref> pref_id_to_bool_pref(PrefId id) {
    switch (id) {
        case PrefId::Savestates:
            return BoolPref::Savestates;
        case PrefId::InputDisp:
            return BoolPref::InputDisp;
        case PrefId::InputDispCenterLocation:
            return BoolPref::InputDispCenterLocation;
        case PrefId::RtaPauseTimer:
            return BoolPref::RtaPauseTimer;
        case PrefId::InputDispNotchIndicators:
            return BoolPref::InputDispNotchIndicators;
        case PrefId::IwTimer:
            return BoolPref::IwTimer;
        case PrefId::CmTimer:
            return BoolPref::CmTimer;
        case PrefId::JumpMod:
            return BoolPref::JumpMod;
        case PrefId::BananaCounter9999:
            return BoolPref::BananaCounter9999;
        case PrefId::DpadControls:
            return BoolPref::DpadControls;
        case PrefId::DebugMode:
            return BoolPref::DebugMode;
        case PrefId::FreezeTimer:
            return BoolPref::FreezeTimer;
        case PrefId::MuteBgm:
            return BoolPref::MuteBgm;
        case PrefId::MuteTimerDing:
            return BoolPref::MuteTimerDing;
        case PrefId::InputDispRawStickInputs:
            return BoolPref::InputDispRawStickInputs;
        case PrefId::Freecam:
            return BoolPref::Freecam;
        case PrefId::Marathon:
            return BoolPref::Marathon;
        case PrefId::Moon:
            return BoolPref::Moon;
        case PrefId::IlBattleDisplay:
            return BoolPref::IlBattleDisplay;
        case PrefId::IlBattleBreakdown:
            return BoolPref::IlBattleBreakdown;
        case PrefId::IlMarkPractice:
            return BoolPref::IlMarkPractice;
        case PrefId::IlMarkStory:
            return BoolPref::IlMarkStory;
        case PrefId::IlMarkChallenge:
            return BoolPref::IlMarkChallenge;
        case PrefId::HideBg:
            return BoolPref::HideBg;
        case PrefId::UnlockVanilla:
            return BoolPref::UnlockVanilla;
        case PrefId::UnlockRomhacks:
            return BoolPref::UnlockRomhacks;
        case PrefId::FreecamInvertYaw:
            return BoolPref::FreecamInvertYaw;
        case PrefId::FreecamInvertPitch:
            return BoolPref::FreecamInvertPitch;
        case PrefId::FreecamToggleWithZ:
            return BoolPref::FreecamToggleWithZ;
        case PrefId::FreecamFreezeTimer:
            return BoolPref::FreecamFreezeTimer;
        case PrefId::FreecamHideHud:
            return BoolPref::FreecamHideHud;
        case PrefId::HideHud:
            return BoolPref::HideHud;
        case PrefId::HideStage:
            return BoolPref::HideStage;
        case PrefId::HideBall:
            return BoolPref::HideBall;
        default:
            return {};
    }
}

static std::optional<U8Pref> pref_id_to_u8_pref(PrefId id) {
    switch (id) {
        case PrefId::CmChara:
            return U8Pref::CmChara;
        case PrefId::InputDispColor:
            return U8Pref::InputDispColor;
        case PrefId::BallColor:
            return U8Pref::BallColor;
        case PrefId::ApeColor:
            return U8Pref::ApeColor;
        case PrefId::IlBattleLength:
            return U8Pref::IlBattleLength;
        case PrefId::FreecamSpeedMult:
            return U8Pref::FreecamSpeedMult;
        default:
            return {};
    }
}

// Boolean preferences that should default to true
static BoolPref s_default_on_bool_prefs[] = {
    BoolPref::Savestates,
    BoolPref::RtaPauseTimer,
    BoolPref::IwTimer,
    BoolPref::CmTimer,
    BoolPref::InputDispNotchIndicators,
    BoolPref::IlMarkPractice,
    BoolPref::UnlockVanilla,
    BoolPref::FreecamFreezeTimer,
    BoolPref::FreecamHideHud,
};

struct DefaultU8Pref {
    U8Pref pref;
    u8 value;
};

// Non-zero default values of u8 preferences
static DefaultU8Pref s_default_u8_prefs[] = {
    {U8Pref::FreecamSpeedMult, 3},
};

//
// End preferences definition
//

struct PrefState {
    u8 bool_prefs[8];
    u8 u8_prefs[8];
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
        std::optional<BoolPref> bool_pref = pref_id_to_bool_pref(id);
        if (bool_pref.has_value()) {
            set_bool_pref(bool_pref.value(), pref_data, s_pref_state);
            continue;
        }

        // For u8 preferences, copy them to struct fields directly
        std::optional<U8Pref> u8_pref = pref_id_to_u8_pref(id);
        if (u8_pref.has_value()) {
            set_u8_pref(u8_pref.value(), static_cast<u8>(pref_data), s_pref_state);
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
        std::optional<BoolPref> bool_pref = pref_id_to_bool_pref(id);
        if (bool_pref.has_value()) {
            entry_list[i].data = get_bool_pref(bool_pref.value(), s_pref_state);
            continue;
        }

        // Write out u8 preference if this is a u8
        std::optional<U8Pref> u8_pref = pref_id_to_u8_pref(id);
        if (u8_pref.has_value()) {
            entry_list[i].data = get_u8_pref(u8_pref.value(), s_pref_state);
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
