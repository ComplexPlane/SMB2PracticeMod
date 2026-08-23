#pragma once

// Mod user preferences, backed by memory card save file

#include "utils/base.h"

// Unique ID assigned to each preference. The ID assigned to a preference must never change, nor may
// they be reused, to ensure backwards compatibility!
typedef enum : u8 {
    Pref_Savestates = 1,
    Pref_InputDisp = 2,
    Pref_InputDispLocation = 3,
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
    // Do not reuse 22, it belonged to old Moon Gravity BoolPref
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
    Pref_PhysicsPreset = 62,
    // 63 belonged to physics friction which was only used in beta playtesting
    // 64 belonged to physics restitution which was only used in beta playtesting
    // 65 belonged to SavestateDisableOverwrite
    Pref_MenuBind = 66,
    Pref_IlBattleReadyBind = 67,
    Pref_FreecamToggleBind = 68,
    Pref_SavestateClearBind = 69,
    Pref_IlBattleTieCount = 70,
    Pref_IlBattleAttemptCount = 71,
    Pref_TimerShowSubtick = 72,
    Pref_TimerShowFramesave = 73,
    // 74 was used for a boolean timer option that only existed in a beta version.
    // Many people playtested that beta, so it may be best to not reuse until
    // a future update
    Pref_TimerShowPause = 75,
    Pref_StageEditVariant = 76,
    Pref_JumpChangePhysics = 77,
    Pref_JumpAllowWalljumps = 78,
    Pref_JumpCount = 79,
    // 80 belonged to physics weight which was only used in beta playtesting
    Pref_MonkeyType = 81,
    Pref_JumpProfile = 82,
    Pref_CustomPhysicsDisp = 83,
    Pref_SavestateSaveTo = 84,
    Pref_SavestateClearAllBind = 85,
    Pref_InputDispGradientColor2Red = 86,
    Pref_InputDispGradientColor2Green = 87,
    Pref_InputDispGradientColor2Blue = 88,
    Pref_InputDispGradientRotation = 89,
    Pref_InputDispGradientStart = 90,
    Pref_InputDispGradientEnd = 91,
    Pref_RgbFormat = 92,
} Pref;

void Pref_Init();
void Pref_Tick();
void Pref_Save();

s16 Pref_Get(Pref pref);
void Pref_Set(Pref pref, s16 value);
bool Pref_DidChange(Pref pref);
s16 Pref_GetDefault(Pref pref);
void Pref_ResetAllDefaults();
