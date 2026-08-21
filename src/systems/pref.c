#include "pref.h"

#include "heap.h"
#include "utils/base.h"

#include "cardio.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

//
// Preferences definition
//

// Verbatim list of preference IDs we iterate over when writing savefile back out
static const Pref PREF_IDS[] = {
    Pref_Savestates,
    Pref_InputDisp,
    Pref_InputDispCenterLocation,
    Pref_TimerShowRTA,
    Pref_CmChara,
    Pref_InputDispColor,
    Pref_InputDispNotchIndicators,
    Pref_IwTimer,
    Pref_CmTimer,
    Pref_JumpMod,
    Pref_BananaCounter9999,
    Pref_DpadControls,
    Pref_DebugMode,
    Pref_MuteBgm,
    Pref_MuteTimerDing,
    Pref_InputDispRawStickInputs,
    Pref_Freecam,
    Pref_BallColor,
    Pref_ApeColor,
    Pref_Marathon,
    Pref_Moon,
    Pref_IlBattleDisplay,
    Pref_IlMarkPractice,
    Pref_IlMarkStory,
    Pref_IlMarkChallenge,
    Pref_HideBg,
    Pref_UnlockVanilla,
    Pref_UnlockRomhacks,
    Pref_FreecamInvertYaw,
    Pref_FreecamInvertPitch,
    Pref_FreecamToggleWithZ,
    Pref_FreecamSpeedMult,
    Pref_FreecamFreezeTimer,
    Pref_FreecamHideHud,
    Pref_HideHud,
    Pref_HideStage,
    Pref_HideBall,
    Pref_HideItems,
    Pref_HideStobjs,
    Pref_HideEffects,
    Pref_IlMarkRomhacks,
    Pref_Camera,
    Pref_BallRed,
    Pref_BallGreen,
    Pref_BallBlue,
    Pref_BallColorType,
    Pref_MenuBind,
    Pref_TimerType,
    Pref_DisableFalloutVolumes,
    Pref_IlBattleShowTime,
    Pref_IlBattleShowScore,
    Pref_IlBattleBuzzerOld,
    Pref_Friction,
    Pref_Restitution,
    Pref_UseCustomPhysics,
    Pref_SavestateDisableOverwrite,
    Pref_ApeColorType,
    Pref_IlBattleBreakdown,
    Pref_InputDispColorType,
    Pref_InputDispRed,
    Pref_InputDispGreen,
    Pref_InputDispBlue,
    Pref_IlBattleReadyBind,
    Pref_FreecamToggleBind,
    Pref_SavestateClearBind,
    Pref_FalloutPlaneType,
    Pref_IlBattleTieCount,
    Pref_IlBattleAttemptCount,
    Pref_TimerShowSubtick,
    Pref_TimerShowFramesave,
    Pref_TimerShowUnrounded,
    Pref_TimerShowPause,
    Pref_StageEditVariant,
    Pref_FullgameTimerOptions,
    Pref_SegmentTimerOptions,
    Pref_StoryTimerWarning,
    Pref_ShowDeathCounter,
};

typedef struct DefaultPref {
    Pref pref;
    u8 value;
} DefaultPref;

// Non-zero default preference values
static const DefaultPref DEFAULT_PREFS[] = {
    {Pref_Savestates, 1},
    {Pref_IwTimer, 1},
    {Pref_CmTimer, 1},
    {Pref_InputDispNotchIndicators, 1},
    {Pref_IlMarkPractice, 1},
    {Pref_UnlockVanilla, 1},
    {Pref_FreecamFreezeTimer, 1},
    {Pref_FreecamHideHud, 1},
    {Pref_IlBattleShowTime, 1},
    {Pref_IlBattleShowScore, 1},
    {Pref_FreecamSpeedMult, 3},      // 3
    {Pref_MenuBind, 64},             // L + R
    {Pref_Friction, 110},            // 0.10
    {Pref_Restitution, 150},         // 0.5
    {Pref_IlBattleReadyBind, 104},   // dpad-down
    {Pref_FreecamToggleBind, 255},   // unbound
    {Pref_SavestateClearBind, 255},  // unbound
};

//
// End preferences definition
//

static constexpr u32 MAX_PREFS = 100;

typedef struct FileHeader {
    char magic[4];  // "APMP"
    u16 semver_major;
    u16 semver_minor;
    u16 semver_patch;
    u16 num_prefs;
} __attribute__((__packed__)) FileHeader;

typedef struct IdEntry {
    u16 id;
    u16 data;  // Either the preference value itself (if <= 2 bytes), or offset into buffer
               // prefs, etc
} __attribute((__packed__)) IdEntry;

// Maps pref ID to value
u8 s_pref_state[MAX_PREFS];

static u8 s_card_buf[sizeof(FileHeader) + LEN(PREF_IDS) * sizeof(IdEntry)]
    __attribute__((__aligned__(32)));  // CARD API requires 32-byte alignment

static void card_buf_to_pref_struct(void *card_buf) {
    FileHeader *header = (FileHeader *)(card_buf);
    if (header->semver_major > 1) return;  // Preferences file format too new for this mod

    IdEntry *entry_list = (IdEntry *)((u32)(card_buf) + sizeof(FileHeader));
    for (u32 i = 0; i < header->num_prefs; i++) {
        Pref id = entry_list[i].id;
        u16 pref_data = entry_list[i].data;
        if (id < LEN(s_pref_state)) {
            s_pref_state[id] = pref_data;
        }
    }
}

static void pref_struct_to_card_buf() {
    FileHeader *header = (FileHeader *)(s_card_buf);
    IdEntry *entry_list = (IdEntry *)((u32)(s_card_buf) + sizeof(FileHeader));

    header->magic[0] = 'A';
    header->magic[1] = 'P';
    header->magic[2] = 'M';
    header->magic[3] = 'P';
    header->semver_major = 1;
    header->semver_minor = 0;
    header->semver_patch = 0;
    header->num_prefs = LEN(PREF_IDS);

    for (u32 i = 0; i < LEN(PREF_IDS); i++) {
        entry_list[i].id = PREF_IDS[i];
        entry_list[i].data = s_pref_state[PREF_IDS[i]];
    }
}

static const char *PREF_FILENAME = "apmp";

void Pref_Init() {
    Pref_SetDefaults();

    FileHeader *header = nullptr;
    mkb_CARDResult result = cardio_read_file(PREF_FILENAME, (void **)(&header));
    if (result == mkb_CARD_RESULT_READY) {
        card_buf_to_pref_struct(header);
        heap_free(header);
    } else if (result != mkb_CARD_RESULT_NOFILE) {
        Draw_Notify(COLOR_RED, "Error loading settings from Card A, setting defaults");
    }
}

static void pref_save_callback(mkb_CARDResult res) {
    if (res != mkb_CARD_RESULT_READY) {
        if (res == mkb_CARD_RESULT_NOENT || res == mkb_CARD_RESULT_INSSPACE) {
            Draw_Notify(COLOR_RED, "Cannot Save Settings: Card A Full");
        } else {
            Draw_Notify(COLOR_RED, "Cannot Save Settings: Card A Unknown Error");
        }
    }
}

void Prev_Save() {
    pref_struct_to_card_buf();
    cardio_write_file(PREF_FILENAME, s_card_buf, sizeof(s_card_buf), pref_save_callback);
}

u8 Pref_Get(Pref pref) {
    ASSERT(pref < LEN(s_pref_state));
    return s_pref_state[pref];
}

void Pref_Set(Pref pref, u8 value) {
    ASSERT(pref < LEN(s_pref_state));
    s_pref_state[pref] = value;
}

void Pref_SetDefaults() {
    mkb_memset(&s_pref_state, 0, sizeof(s_pref_state));
    for (u32 i = 0; i < LEN(DEFAULT_PREFS); i++) {
        Pref_Set(DEFAULT_PREFS[i].pref, DEFAULT_PREFS[i].value);
    }
}

u8 Pref_GetDefault(Pref pref) {
    for (u32 i = 0; i < LEN(DEFAULT_PREFS); i++) {
        if (DEFAULT_PREFS[i].pref == pref) return DEFAULT_PREFS[i].value;
    }
    return 0;
}
