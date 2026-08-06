#pragma once

#include "utils/base.h"

// Mod user preferences, backed by memory card save file

// To add a new preference:
// 1) Append to Pref enum. Don't use an unused "gap" ID, pick a new highest value!
// 2) Add the preference to PREF_IDS in pref.c
// 3) (Optional) Add a default value in DEFAULT_PREFS in pref.c

// Unique ID assigned to each preference. The ID assigned to a preference must never change, nor may
// they be reused, to ensure backwards compatibility!
typedef enum Pref : u16 Pref;
enum Pref : u16 {
    Pref_Savestates = 1,
    Pref_InputDisp = 2,
    Pref_InputDispCenterLocation = 3,
    Pref_TimerShowRTA = 4,
    Pref_CmChara = 5,
    Pref_InputDispColor = 6,
    Pref_InputDispNotchIndicators = 7,
    Pref_IwTimer = 8,
    Pref_CmTimer = 9,
    Pref_JumpMod = 10,
    Pref_BananaCounter9999 = 11,
    Pref_DpadControls = 12,
    Pref_DebugMode = 13,
    // Do not reuse 14, it belonged to old timer freezing BoolPref
    Pref_MuteBgm = 15,
    Pref_MuteTimerDing = 16,
    Pref_InputDispRawStickInputs = 17,
    Pref_Freecam = 18,
    Pref_BallColor = 19,
    Pref_ApeColor = 20,
    Pref_Marathon = 21,
    Pref_Moon = 22,
    Pref_IlBattleDisplay = 23,
    Pref_IlBattleLength = 24,
    // Do not reuse 25, it belonged to old IL Battle Score Breakdown BoolPref
    Pref_IlMarkPractice = 26,
    Pref_IlMarkStory = 27,
    Pref_IlMarkChallenge = 28,
    Pref_HideBg = 29,
    Pref_UnlockVanilla = 30,
    Pref_UnlockRomhacks = 31,
    Pref_FreecamInvertYaw = 32,
    Pref_FreecamInvertPitch = 33,
    Pref_FreecamToggleWithZ = 34,
    Pref_FreecamSpeedMult = 35,
    Pref_FreecamFreezeTimer = 36,
    Pref_FreecamHideHud = 37,
    Pref_HideHud = 38,
    Pref_HideStage = 39,
    Pref_HideBall = 40,
    Pref_HideItems = 41,
    Pref_HideStobjs = 42,
    Pref_HideEffects = 43,
    Pref_IlMarkRomhacks = 44,
    Pref_Camera = 45,
    Pref_BallRed = 46,
    Pref_BallGreen = 47,
    Pref_BallBlue = 48,
    Pref_BallColorType = 49,
    Pref_ApeColorType = 50,
    Pref_InputDispColorType = 51,
    Pref_InputDispRed = 52,
    Pref_InputDispGreen = 53,
    Pref_InputDispBlue = 54,
    Pref_TimerType = 55,
    Pref_DisableFalloutVolumes = 56,
    Pref_FalloutPlaneType = 57,
    Pref_IlBattleShowTime = 58,
    Pref_IlBattleShowScore = 59,
    Pref_IlBattleBuzzerOld = 60,
    Pref_IlBattleBreakdown = 61,
    Pref_UseCustomPhysics = 62,
    Pref_Friction = 63,
    Pref_Restitution = 64,
    Pref_SavestateDisableOverwrite = 65,
    Pref_MenuBind = 66,
    Pref_IlBattleReadyBind = 67,
    Pref_FreecamToggleBind = 68,
    Pref_SavestateClearBind = 69,
    Pref_IlBattleTieCount = 70,
    Pref_IlBattleAttemptCount = 71,
    Pref_TimerShowSubtick = 72,
    Pref_TimerShowFramesave = 73,
    Pref_TimerShowUnrounded = 74,
    Pref_TimerShowPause = 75,
    Pref_StageEditVariant = 76,
    Pref_FullgameTimerOptions = 77,
    Pref_SegmentTimerOptions = 78,
    Pref_StoryTimerWarning = 79,
    Pref_ShowDeathCounter = 80,
};

void pref_init();
void pref_save();
u8 pref_get(Pref pref);
void pref_set(Pref pref, u8 value);
void pref_set_defaults();
