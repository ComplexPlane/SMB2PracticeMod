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

static const Pref PREF_IDS[] = {
    Pref::Savestates,
    Pref::InputDisp,
    Pref::InputDispLocation,
    Pref::TimerShowRTA,
    Pref::CmChara,
    Pref::InputDispColor,
    Pref::InputDispNotchIndicators,
    Pref::IwTimer,
    Pref::CmTimer,
    Pref::JumpMod,
    Pref::BananaCounter9999,
    Pref::DpadControls,
    Pref::DebugMode,
    Pref::MuteBgm,
    Pref::MuteTimerDing,
    Pref::InputDispRawStickInputs,
    Pref::Freecam,
    Pref::BallColor,
    Pref::ApeColor,
    Pref::Marathon,
    Pref::IlBattleDisplay,
    Pref::IlBattleLength,
    Pref::IlMarkPractice,
    Pref::IlMarkStory,
    Pref::IlMarkChallenge,
    Pref::HideBg,
    Pref::UnlockVanilla,
    Pref::UnlockRomhacks,
    Pref::FreecamInvertYaw,
    Pref::FreecamInvertPitch,
    Pref::FreecamToggleWithZ,
    Pref::FreecamSpeedMult,
    Pref::FreecamFreezeTimer,
    Pref::FreecamHideHud,
    Pref::HideHud,
    Pref::HideStage,
    Pref::HideBall,
    Pref::HideItems,
    Pref::HideStobjs,
    Pref::HideEffects,
    Pref::IlMarkRomhacks,
    Pref::Camera,
    Pref::BallRed,
    Pref::BallGreen,
    Pref::BallBlue,
    Pref::BallColorType,
    Pref::ApeColorType,
    Pref::InputDispColorType,
    Pref::InputDispRed,
    Pref::InputDispGreen,
    Pref::InputDispBlue,
    Pref::TimerType,
    Pref::DisableFalloutVolumes,
    Pref::FalloutPlaneType,
    Pref::IlBattleShowTime,
    Pref::IlBattleShowScore,
    Pref::IlBattleBuzzerOld,
    Pref::IlBattleBreakdown,
    Pref::PhysicsPreset,
    Pref::MenuBind,
    Pref::IlBattleReadyBind,
    Pref::FreecamToggleBind,
    Pref::SavestateClearBind,
    Pref::IlBattleTieCount,
    Pref::IlBattleAttemptCount,
    Pref::TimerShowSubtick,
    Pref::TimerShowFramesave,
    Pref::TimerShowPause,
    Pref::StageEditVariant,
    Pref::JumpChangePhysics,
    Pref::JumpAllowWalljumps,
    Pref::JumpCount,
    Pref::MonkeyType,
    Pref::JumpProfile,
    Pref::CustomPhysicsDisp,
    Pref::SavestateSaveTo,
    Pref::SavestateClearAllBind,
    Pref::InputDispGradientColor2Red,
    Pref::InputDispGradientColor2Green,
    Pref::InputDispGradientColor2Blue,
    Pref::InputDispGradientRotation,
    Pref::InputDispGradientStart,
    Pref::InputDispGradientEnd,
    Pref::RgbFormat,
};

struct DefaultPref {
    Pref pref;
    s16 value;
};

// Non-zero default preference values
static const DefaultPref DEFAULT_PREFS[] = {
    {Pref::Savestates, 1},
    {Pref::InputDispLocation, 1},
    {Pref::InputDispNotchIndicators, 1},
    {Pref::IwTimer, 1},
    {Pref::CmTimer, 1},
    {Pref::IlMarkPractice, 1},
    {Pref::UnlockVanilla, 1},
    {Pref::FreecamSpeedMult, 3},
    {Pref::FreecamFreezeTimer, 1},
    {Pref::FreecamHideHud, 1},
    {Pref::IlMarkRomhacks, 1},
    {Pref::IlBattleShowTime, 1},
    {Pref::IlBattleShowScore, 1},
    {Pref::MenuBind, 64},
    {Pref::IlBattleReadyBind, 104},
    {Pref::FreecamToggleBind, 255},
    {Pref::SavestateClearBind, 255},
    {Pref::JumpChangePhysics, 1},
    {Pref::JumpAllowWalljumps, 1},
    {Pref::JumpCount, 1},
    {Pref::CustomPhysicsDisp, 1},
    {Pref::SavestateClearAllBind, 255},
    {Pref::InputDispGradientEnd, 100},
};

//
// End preferences definition
//

static constexpr u32 MAX_PREFS = 100;

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

// Maps pref ID to value. Values are 16-bit so prefs can hold values outside 0-255
// (e.g. negative numbers), though most prefs only use the low byte
static s16 s_pref_state[MAX_PREFS];
static s16 s_prev_pref_state[MAX_PREFS];

static u8 s_card_buf[sizeof(FileHeader) + LEN(PREF_IDS) * sizeof(IdEntry)]
    __attribute__((__aligned__(32)));  // CARD API requires 32-byte alignment

static void card_buf_to_pref_struct(void *card_buf) {
    FileHeader *header = static_cast<FileHeader *>(card_buf);
    if (header->semver_major > 1) return;  // Preferences file format too new for this mod

    IdEntry *entry_list =
        reinterpret_cast<IdEntry *>(reinterpret_cast<u32>(card_buf) + sizeof(FileHeader));
    for (u32 i = 0; i < header->num_prefs; i++) {
        Pref id = static_cast<Pref>(entry_list[i].id);
        u16 pref_data = entry_list[i].data;
        if (static_cast<u16>(id) < LEN(s_pref_state)) {
            s_pref_state[static_cast<u16>(id)] = static_cast<s16>(pref_data);
        }
    }
}

static void pref_struct_to_card_buf() {
    FileHeader *header = static_cast<FileHeader *>(static_cast<void *>(s_card_buf));
    IdEntry *entry_list =
        reinterpret_cast<IdEntry *>(reinterpret_cast<u32>(s_card_buf) + sizeof(FileHeader));

    header->magic[0] = 'A';
    header->magic[1] = 'P';
    header->magic[2] = 'M';
    header->magic[3] = 'P';
    header->semver_major = 1;
    header->semver_minor = 0;
    header->semver_patch = 0;
    header->num_prefs = LEN(PREF_IDS);

    for (u32 i = 0; i < LEN(PREF_IDS); i++) {
        entry_list[i].id = static_cast<u16>(PREF_IDS[i]);
        entry_list[i].data = static_cast<u16>(s_pref_state[static_cast<u16>(PREF_IDS[i])]);
    }
}

static constexpr const char *PREF_FILENAME = "apmp";

void init() {
    reset_all_defaults();

    FileHeader *header = nullptr;
    s32 result = cardio::read_file(PREF_FILENAME, reinterpret_cast<void **>(&header));
    if (result == mkb::CARD_RESULT_READY) {
        card_buf_to_pref_struct(header);
        heap::free(header);
    } else if (result != mkb::CARD_RESULT_NOFILE) {
        draw::notify(draw::RED, draw::NotifyDuration::Long,
                     "Error loading settings from Card A, setting defaults");
    }
}

void save() {
    pref_struct_to_card_buf();
    cardio::write_file(PREF_FILENAME, s_card_buf, sizeof(s_card_buf), [](mkb::CARDResult res) {
        if (res != mkb::CARD_RESULT_READY) {
            if (res == mkb::CARD_RESULT_NOENT || res == mkb::CARD_RESULT_INSSPACE) {
                draw::notify(draw::RED, draw::NotifyDuration::Long,
                             "Cannot Save Settings: Card A Full");
            } else {
                draw::notify(draw::RED, draw::NotifyDuration::Long,
                             "Cannot Save Settings: Card A Unknown Error");
            }
        }
    });
}

void reset_all_defaults() {
    mkb::memset(&s_pref_state, 0, sizeof(s_pref_state));
    for (u32 i = 0; i < LEN(DEFAULT_PREFS); i++) {
        s_pref_state[static_cast<u16>(DEFAULT_PREFS[i].pref)] = DEFAULT_PREFS[i].value;
    }
}

s16 get(Pref pref) {
    ASSERT(static_cast<u16>(pref) < LEN(s_pref_state));
    return s_pref_state[static_cast<u16>(pref)];
}

void set(Pref pref, s16 value) {
    ASSERT(static_cast<u16>(pref) < LEN(s_pref_state));
    s_pref_state[static_cast<u16>(pref)] = value;
}

s16 get_default(Pref pref) {
    for (u32 i = 0; i < LEN(DEFAULT_PREFS); i++) {
        if (DEFAULT_PREFS[i].pref == pref) return DEFAULT_PREFS[i].value;
    }
    return 0;
}

void tick() {
    mkb::memcpy(s_prev_pref_state, s_pref_state, sizeof(s_pref_state));
}

bool did_change(Pref pref) {
    return s_pref_state[(u32)pref] != s_prev_pref_state[(u32)pref];
}

}  // namespace pref
