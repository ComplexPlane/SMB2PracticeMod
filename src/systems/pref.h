#pragma once

// Mod user preferences, backed by memory card save file

#include "mkb/mkb.h"

namespace pref {

// To add a new preference:
// 1) Append to Pref enum. Don't reuse an unused "gap" ID, pick a new highest value!
// 2) Add the preference to PREF_IDS in pref.cpp
// 3) (Optional) Add a default value in DEFAULT_PREFS in pref.cpp

// Unique ID assigned to each preference. The ID assigned to a preference must never change, nor may
// they be reused, to ensure backwards compatibility!
enum class Pref : u16 {
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
    // Do not reuse 65, it belonged to SavestateDisableOverwrite
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
    // Do not reuse 80-83, they belonged to removed prefs (e.g. ShowDeathCounter)
    SavestateSaveTo = 84,
    SavestateClearAllBind = 85,
    InputDispGradientColor2Red = 86,
    InputDispGradientColor2Green = 87,
    InputDispGradientColor2Blue = 88,
    InputDispGradientRotation = 89,
    InputDispGradientStart = 90,
    InputDispGradientEnd = 91,
    ReverseMode = 92,
    JumpChangePhysics = 93,
    JumpAllowWalljumps = 94,
    InputDispLocation = 95,
    RgbFormat = 96,
    MonkeyType = 97,
    JumpProfile = 98,
    JumpCount = 99,
};

void init();
void save();
void reset_all_defaults();

u8 get(Pref pref);
void set(Pref pref, u8 value);
u8 get_default(Pref pref);

}  // namespace pref
