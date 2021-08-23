#include "pref.h"

#include <log.h>
#include <mkb.h>
#include <macro_utils>
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
};

// Bit index into Pref struct (not ID of preference itself as stored in memcard file
enum class BoolPref {
    Savestates,
    InputDisp,
    InputDispCenterLocation,
    RtaPauseTimer,
};

enum class U8Pref {
    CmChara,
    InputDispColor,
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
    PrefId::Savestates,    PrefId::InputDisp, PrefId::InputDispCenterLocation,
    PrefId::RtaPauseTimer, PrefId::CmChara,   PrefId::InputDispColor,
};

static u8 s_file_buf[sizeof(FileHeader) + LEN(s_pref_ids) * sizeof(IdEntry)];

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
        reinterpret_cast<IdEntry*>(reinterpret_cast<uintptr_t>(card_buf) + sizeof(FileHeader));

    for (s32 i = 0; i < header->num_prefs; i++) {
        PrefId id = static_cast<PrefId>(entry_list[i].id);
        u16 pref_data = entry_list[i].data;

        // If it's a boolean preference, copy it from the memcard file
        std::optional<BoolPref> bool_pref_idx = pref_id_to_bool_pref(id);
        if (bool_pref_idx.has_value()) {
            u16 bpi = static_cast<u16>(bool_pref_idx.value());
            MOD_ASSERT_MSG(bpi / 8 < LEN(pref.bool_prefs); // Out of room for boolean preferences
            if (pref_data) {
                pref.bool_prefs[bpi / 8] |= (1 << (bpi % 8));
            } else {
                pref.bool_prefs[bpi / 8] &= ~(1 << (bpi % 8));
            }
            continue;
        }

        // For u8 preferences, copy them to struct fields directly
        u8* u8_pref = pref_id_to_u8(id, pref);
        if (u8_pref != nullptr) {
            // File data offset is in bytes
            *u8_pref = static_cast<u8>(pref_data);
            continue;
        }

        // Ignore all other setting IDs we don't understand
    }
}

static void pref_struct_to_card_buf(const Pref& pref, void* card_buf) {
    FileHeader* header = static_cast<FileHeader*>(card_buf);
    IdEntry* entry_list =
        reinterpret_cast<IdEntry*>(reinterpret_cast<uintptr_t>(card_buf) + sizeof(FileHeader));

    memcpy(header->magic, static_cast<const char*>("APMP"), 4);
    header->semver_major = 1;
    header->semver_minor = 0;
    header->semver_patch = 0;
    header->num_prefs = LEN(s_pref_ids);

    for (u32 i = 0; i < LEN(s_pref_ids); i++) {
        PrefId id = s_pref_ids[i];
        entry_list[i].id = static_cast<u16>(id);

        // Write out boolean property, if this is a boolean
        std::optional<BoolPref> bool_pref_idx = pref_id_to_bool_pref(id);
        if (bool_pref_idx.has_value()) {
            u16 bpi = static_cast<u16>(bool_pref_idx.value());
            MOD_ASSERT(bpi / 8 < LEN(pref.bool_prefs));  // Out of room for bool prefs
            entry_list[i].data = pref.bool_prefs[bpi / 8] & (1 << (bpi % 8));
            continue;
        }

        // Write out u8 property if this is a u8
        const u8* u8_pref = pref_id_to_u8(id, pref);
        if (u8_pref != nullptr) {
            entry_list[i].data = *u8_pref;
            continue;
        }

        // There should be no other types of preferences!
        MOD_ASSERT(false);
    }
}

void init() {}

// u8 get_cm_chara() {}
// void set_cm_chara(s8 idx) {}
//
// bool get_savestates() {}
// void set_savestates(bool on) {}
//
// bool get_input_disp() {}
// void set_input_disp(bool on) {}
// bool get_input_disp_center_location() {}
// void set_input_disp_center_location(bool on) {}
// u8 get_input_disp_color() {}
// void set_input_disp_color(u8 idx) {}
//
// bool get_rta_pause_timer() {}
// void set_rta_pause_timer(bool on) {}
// bool get_story_timer() {}
// void set_story_timer(bool on) {}
// bool get_cm_timer() {}
// void set_cm_timer(bool on) {}
//
// bool get_jump_mod() {}
// void set_jump_mod(bool on) {}
// bool get_9999_banana_counter() {}
// void set_9999_banana_counter(bool on) {}
// bool get_dpad_controls() {}
// void set_dpad_controls(bool on) {}
// bool get_debug_mode() {}
// void set_debug_mode(bool on) {}

}  // namespace pref