#include "pref.h"

#include <log.h>
#include <macro_utils.h>
#include <mkb.h>
#include <optional>

namespace pref {

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
};

// Bit index into Pref struct (not ID of preference itself as stored in memcard file
enum class BoolPref {
    Savestates,
    InputDisp,
    InputDispCenterLocation,
    RtaPauseTimer,
    InputDispNotchIndicators,
    IwTimer,
    CmTimer,
    JumpMod,
    BananaCounter9999,
    DpadControls,
    DebugMode,
    FreezeTimer,
};

struct Pref {
    u8 bool_prefs[8];
    u8 cm_chara;
    u8 input_disp_color;
} s_pref;

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

/*
 * Verbatim list of preference IDs we iterate over when writing savefile back out
 */
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
};

static u8 s_card_buf[sizeof(FileHeader) + LEN(s_pref_ids) * sizeof(IdEntry)];

static inline u16 validate_bool_pref(BoolPref bp) {
    u16 bpi = static_cast<u16>(bp);
    MOD_ASSERT(static_cast<u16>(bpi / 8) < LEN(s_pref.bool_prefs));  // Out of room for bool
                                                                     // prefs
    return bpi;
}

static bool get_bool_pref(BoolPref bp) {
    u16 bpi = validate_bool_pref(bp);
    return s_pref.bool_prefs[bpi / 8] & (1 << (bpi % 8));
}

static void set_bool_pref(BoolPref bp, bool value) {
    u16 bpi = validate_bool_pref(bp);
    if (value) {
        s_pref.bool_prefs[bpi / 8] |= (1 << (bpi % 8));
    } else {
        s_pref.bool_prefs[bpi / 8] &= ~(1 << (bpi % 8));
    }
}

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
        default:
            return {};
    }
}

static u8* pref_id_to_u8(PrefId id, Pref& pref) {
    switch (id) {
        case PrefId::CmChara:
            return &pref.cm_chara;
        case PrefId::InputDispColor:
            return &pref.input_disp_color;
        default:
            return nullptr;
    }
}

static const u8* pref_id_to_u8(PrefId id, const Pref& pref) {
    return pref_id_to_u8(id, const_cast<Pref&>(pref));
}

static void card_buf_to_pref_struct(void* card_buf, Pref& pref) {
    FileHeader* header = static_cast<FileHeader*>(card_buf);
    if (header->semver_major > 1) return;  // Preferences file format too new for this mod

    IdEntry* entry_list =
        reinterpret_cast<IdEntry*>(reinterpret_cast<u32>(card_buf) + sizeof(FileHeader));

    for (s32 i = 0; i < header->num_prefs; i++) {
        PrefId id = static_cast<PrefId>(entry_list[i].id);
        u16 pref_data = entry_list[i].data;

        // If it's a boolean preference, copy it from the memcard file
        std::optional<BoolPref> bool_pref_idx = pref_id_to_bool_pref(id);
        if (bool_pref_idx.has_value()) {
            set_bool_pref(bool_pref_idx.value(), pref_data);
            continue;
        }

        // For u8 preferences, copy them to struct fields directly
        u8* u8_pref = pref_id_to_u8(id, pref);
        if (u8_pref != nullptr) {
            // File data offset is in bytes
            *u8_pref = static_cast<u8>(pref_data);
            continue;
        }

        // Ignore all other setting IDs we aren't aware of
    }
}

static void load_default_prefs() {
    mkb::memset(&s_pref, 0, sizeof(s_pref));
    set_bool_pref(BoolPref::Savestates, true);
    set_bool_pref(BoolPref::RtaPauseTimer, true);
    set_bool_pref(BoolPref::IwTimer, true);
    set_bool_pref(BoolPref::CmTimer, true);
    set_bool_pref(BoolPref::InputDispNotchIndicators, true);
}

static void pref_struct_to_card_buf(const Pref& pref, void* card_buf) {
    FileHeader* header = static_cast<FileHeader*>(card_buf);
    IdEntry* entry_list =
        reinterpret_cast<IdEntry*>(reinterpret_cast<u32>(card_buf) + sizeof(FileHeader));

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
        std::optional<BoolPref> bool_pref_idx = pref_id_to_bool_pref(id);
        if (bool_pref_idx.has_value()) {
            entry_list[i].data = get_bool_pref(bool_pref_idx.value());
            continue;
        }

        // Write out u8 preference if this is a u8
        const u8* u8_pref = pref_id_to_u8(id, pref);
        if (u8_pref != nullptr) {
            entry_list[i].data = *u8_pref;
            continue;
        }

        // There should be no other types of preferences!
        MOD_ASSERT(false);
    }
}

void init() { load_default_prefs(); }

void save() {}

u8 get_cm_chara() { return s_pref.cm_chara; }
void set_cm_chara(u8 idx) { s_pref.cm_chara = idx; }

bool get_savestates() { return get_bool_pref(BoolPref::Savestates); }
void set_savestates(bool on) { set_bool_pref(BoolPref::Savestates, on); }

bool get_input_disp() { return get_bool_pref(BoolPref::InputDisp); }
void set_input_disp(bool on) { set_bool_pref(BoolPref::InputDisp, on); }
bool get_input_disp_center_location() { return get_bool_pref(BoolPref::InputDispCenterLocation); }
void set_input_disp_center_location(bool on) {
    set_bool_pref(BoolPref::InputDispCenterLocation, on);
};
u8 get_input_disp_color() { return s_pref.input_disp_color; }
void set_input_disp_color(u8 idx) { s_pref.input_disp_color = idx; }
bool get_input_disp_notch_indicators() { return get_bool_pref(BoolPref::InputDispNotchIndicators); }
void set_input_disp_notch_indicators(bool on) {
    set_bool_pref(BoolPref::InputDispNotchIndicators, on);
}

bool get_rta_pause_timer() { return get_bool_pref(BoolPref::RtaPauseTimer); }
void set_rta_pause_timer(bool on) { set_bool_pref(BoolPref::RtaPauseTimer, on); }
bool get_iw_timer() { return get_bool_pref(BoolPref::IwTimer); }
void set_iw_timer(bool on) { set_bool_pref(BoolPref::IwTimer, on); }
bool get_cm_timer() { return get_bool_pref(BoolPref::CmTimer); }
void set_cm_timer(bool on) { set_bool_pref(BoolPref::CmTimer, on); }

bool get_jump_mod() { return get_bool_pref(BoolPref::JumpMod); }
void set_jump_mod(bool on) { set_bool_pref(BoolPref::JumpMod, on); }
bool get_9999_banana_counter() { return get_bool_pref(BoolPref::BananaCounter9999); }
void set_9999_banana_counter(bool on) { set_bool_pref(BoolPref::BananaCounter9999, on); }
bool get_dpad_controls() { return get_bool_pref(BoolPref::DpadControls); }
void set_dpad_controls(bool on) { set_bool_pref(BoolPref::DpadControls, on); }
bool get_debug_mode() { return get_bool_pref(BoolPref::DebugMode); }
void set_debug_mode(bool on) { set_bool_pref(BoolPref::DebugMode, on); }
bool get_freeze_timer() { return get_bool_pref(BoolPref::FreezeTimer); }
void set_freeze_timer(bool on) { set_bool_pref(BoolPref::FreezeTimer, on); }

}  // namespace pref