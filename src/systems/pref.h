#pragma once

// Mod user preferences, backed by memory card save file

#include "mkb/mkb.h"

namespace pref {

// Unique ID assigned to each preference. The ID assigned to a preference must never change, nor may
// they be reused, to ensure backwards compatibility!
enum class Pref : u8 {
    Savestates = 1,
    InputDisp = 2,
    InputDispLocation = 3,
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
    // Do not reuse 22, it belonged to old Moon Gravity BoolPref
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
    PhysicsPreset = 62,
    // 63 belonged to physics friction which was only used in beta playtesting
    // 64 belonged to physics restitution which was only used in beta playtesting
    // 65 belonged to SavestateDisableOverwrite
    MenuBind = 66,
    IlBattleReadyBind = 67,
    FreecamToggleBind = 68,
    SavestateClearBind = 69,
    IlBattleTieCount = 70,
    IlBattleAttemptCount = 71,
    TimerShowSubtick = 72,
    TimerShowFramesave = 73,
    // 74 was used for a boolean timer option that only existed in a beta version.
    // Many people playtested that beta, so it may be best to not reuse until
    // a future update
    TimerShowPause = 75,
    StageEditVariant = 76,
    JumpChangePhysics = 77,
    JumpAllowWalljumps = 78,
    JumpCount = 79,
    // 80 belonged to physics weight which was only used in beta playtesting
    MonkeyType = 81,
    JumpProfile = 82,
    CustomPhysicsDisp = 83,
    SavestateSaveTo = 84,
    SavestateClearAllBind = 85,
    InputDispGradientColor2Red = 86,
    InputDispGradientColor2Green = 87,
    InputDispGradientColor2Blue = 88,
    InputDispGradientRotation = 89,
    InputDispGradientStart = 90,
    InputDispGradientEnd = 91,
    RgbFormat = 92,
    FullgameTimerOptions = 93,
    SegmentTimerOptions = 94,
    ShowRunBreakdown = 95,
    HideRunResetMessage = 96,
    DeathCounterDisplayOptions = 97,
    CountFirstStageDeaths = 98,
    MenuAcceleration = 99,
    RightSideUIHide = 100,
};

void init();
void tick();
void save();

s16 get(Pref pref);
void set(Pref pref, s16 value);
bool did_change(Pref pref);
s16 get_default(Pref pref);
void reset_all_defaults();

}  // namespace pref
