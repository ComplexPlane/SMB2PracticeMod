#include "systems/pref.h"

#include "systems/cardio.h"
#include "systems/heap.h"
#include "systems/log.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"

//
// Preferences definition
//

static const Pref PREF_IDS[] = {
    Pref_Savestates,
    Pref_InputDisp,
    Pref_InputDispLocation,
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
    Pref_IlBattleDisplay,
    Pref_IlBattleLength,
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
    Pref_ApeColorType,
    Pref_InputDispColorType,
    Pref_InputDispRed,
    Pref_InputDispGreen,
    Pref_InputDispBlue,
    Pref_TimerType,
    Pref_DisableFalloutVolumes,
    Pref_FalloutPlaneType,
    Pref_IlBattleShowTime,
    Pref_IlBattleShowScore,
    Pref_IlBattleBuzzerOld,
    Pref_IlBattleBreakdown,
    Pref_PhysicsPreset,
    Pref_MenuBind,
    Pref_IlBattleReadyBind,
    Pref_FreecamToggleBind,
    Pref_SavestateClearBind,
    Pref_IlBattleTieCount,
    Pref_IlBattleAttemptCount,
    Pref_TimerShowSubtick,
    Pref_TimerShowFramesave,
    Pref_TimerShowPause,
    Pref_StageEditVariant,
    Pref_JumpChangePhysics,
    Pref_JumpAllowWalljumps,
    Pref_JumpCount,
    Pref_MonkeyType,
    Pref_JumpProfile,
    Pref_CustomPhysicsDisp,
    Pref_SavestateSaveTo,
    Pref_SavestateClearAllBind,
    Pref_InputDispGradientColor2Red,
    Pref_InputDispGradientColor2Green,
    Pref_InputDispGradientColor2Blue,
    Pref_InputDispGradientRotation,
    Pref_InputDispGradientStart,
    Pref_InputDispGradientEnd,
    Pref_RgbFormat,
};

typedef struct {
    Pref pref;
    s16 value;
} DefaultPref;

// Non-zero default preference values
static const DefaultPref DEFAULT_PREFS[] = {
    {Pref_Savestates, 1},
    {Pref_InputDispLocation, 1},
    {Pref_InputDispNotchIndicators, 1},
    {Pref_IwTimer, 1},
    {Pref_CmTimer, 1},
    {Pref_IlMarkPractice, 1},
    {Pref_UnlockVanilla, 1},
    {Pref_FreecamSpeedMult, 3},
    {Pref_FreecamFreezeTimer, 1},
    {Pref_FreecamHideHud, 1},
    {Pref_IlMarkRomhacks, 1},
    {Pref_IlBattleShowTime, 1},
    {Pref_IlBattleShowScore, 1},
    {Pref_MenuBind, 64},
    {Pref_IlBattleReadyBind, 104},
    {Pref_FreecamToggleBind, 255},
    {Pref_SavestateClearBind, 255},
    {Pref_JumpChangePhysics, 1},
    {Pref_JumpAllowWalljumps, 1},
    {Pref_JumpCount, 1},
    {Pref_CustomPhysicsDisp, 1},
    {Pref_SavestateClearAllBind, 255},
    {Pref_InputDispGradientEnd, 100},
};

//
// End preferences definition
//

static constexpr u32 MAX_PREFS = 100;

typedef struct {
    char magic[4];  // "APMP"
    u16 semver_major;
    u16 semver_minor;
    u16 semver_patch;
    u16 num_prefs;
} __attribute__((__packed__)) FileHeader;

typedef struct {
    u16 id;
    u16 data;  // Either the preference value itself (if <= 2 bytes), or offset into buffer
               // prefs, etc
} __attribute__((__packed__)) IdEntry;

// Maps pref ID to value. Values are 16-bit so prefs can hold values outside 0-255
// (e.g. negative numbers), though most prefs only use the low byte
static s16 s_pref_state[MAX_PREFS];
static s16 s_prev_pref_state[MAX_PREFS];

static u8 s_card_buf[sizeof(FileHeader) + LEN(PREF_IDS) * sizeof(IdEntry)]
    __attribute__((__aligned__(32)));  // CARD API requires 32-byte alignment

static void card_buf_to_pref_struct(void *card_buf) {
    FileHeader *header = (FileHeader *)card_buf;
    if (header->semver_major > 1) return;  // Preferences file format too new for this mod

    IdEntry *entry_list = (IdEntry *)((u32)card_buf + sizeof(FileHeader));
    for (u32 i = 0; i < header->num_prefs; i++) {
        Pref id = (Pref)entry_list[i].id;
        u16 pref_data = entry_list[i].data;
        if ((u16)id < LEN(s_pref_state)) {
            s_pref_state[(u16)id] = (s16)pref_data;
        }
    }
}

static void pref_struct_to_card_buf() {
    FileHeader *header = (FileHeader *)(void *)s_card_buf;
    IdEntry *entry_list = (IdEntry *)((u32)s_card_buf + sizeof(FileHeader));

    header->magic[0] = 'A';
    header->magic[1] = 'P';
    header->magic[2] = 'M';
    header->magic[3] = 'P';
    header->semver_major = 1;
    header->semver_minor = 0;
    header->semver_patch = 0;
    header->num_prefs = LEN(PREF_IDS);

    for (u32 i = 0; i < LEN(PREF_IDS); i++) {
        entry_list[i].id = (u16)PREF_IDS[i];
        entry_list[i].data = (u16)s_pref_state[(u16)PREF_IDS[i]];
    }
}

static const char *PREF_FILENAME = "apmp";

void Pref_Init() {
    Pref_ResetAllDefaults();

    FileHeader *header = nullptr;
    s32 result = Card_ReadFile(PREF_FILENAME, (void **)&header);
    if (result == mkb_CARD_RESULT_READY) {
        card_buf_to_pref_struct(header);
        Heap_Free(header);
    } else if (result != mkb_CARD_RESULT_NOFILE) {
        Draw_NotifyWithDuration(COLOR_RED, Draw_NotifyDuration_Long,
                                "Error loading settings from Card A, setting defaults");
    }
}

static void pref_card_write_callback(mkb_CARDResult res) {
    if (res != mkb_CARD_RESULT_READY) {
        if (res == mkb_CARD_RESULT_NOENT || res == mkb_CARD_RESULT_INSSPACE) {
            Draw_NotifyWithDuration(COLOR_RED, Draw_NotifyDuration_Long,
                                    "Cannot Save Settings: Card A Full");
        } else {
            Draw_NotifyWithDuration(COLOR_RED, Draw_NotifyDuration_Long,
                                    "Cannot Save Settings: Card A Unknown Error");
        }
    }
}

void Pref_Save() {
    pref_struct_to_card_buf();
    Card_WriteFile(PREF_FILENAME, s_card_buf, sizeof(s_card_buf), pref_card_write_callback);
}

void Pref_ResetAllDefaults() {
    mkb_memset(&s_pref_state, 0, sizeof(s_pref_state));
    for (u32 i = 0; i < LEN(DEFAULT_PREFS); i++) {
        s_pref_state[(u16)DEFAULT_PREFS[i].pref] = DEFAULT_PREFS[i].value;
    }
}

s16 Pref_Get(Pref pref) {
    ASSERT((u16)pref < LEN(s_pref_state));
    return s_pref_state[(u16)pref];
}

void Pref_Set(Pref pref, s16 value) {
    ASSERT((u16)pref < LEN(s_pref_state));
    s_pref_state[(u16)pref] = value;
}

s16 Pref_GetDefault(Pref pref) {
    for (u32 i = 0; i < LEN(DEFAULT_PREFS); i++) {
        if (DEFAULT_PREFS[i].pref == pref) return DEFAULT_PREFS[i].value;
    }
    return 0;
}

void Pref_Tick() {
    mkb_memcpy(s_prev_pref_state, s_pref_state, sizeof(s_pref_state));
}

bool Pref_DidChange(Pref pref) {
    return s_pref_state[(u32)pref] != s_prev_pref_state[(u32)pref];
}
