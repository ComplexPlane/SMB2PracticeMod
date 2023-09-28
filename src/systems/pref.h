#pragma once

// Mod user preferences, backed by memory card save file

#include "mkb/mkb.h"

namespace pref {

enum class BoolPref : u8 {
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
    FreezeTimer,  // SAFE TO DELETE
    MuteBgm,
    MuteTimerDing,
    InputDispRawStickInputs,
    Freecam,
    Marathon,
    Moon,
    IlBattleDisplay,
    IlBattleShowBreakdown,  // SAFE TO DELETE
    IlMarkPractice,
    IlMarkStory,
    IlMarkChallenge,
    HideBg,
    UnlockVanilla,
    UnlockRomhacks,
    FreecamInvertYaw,
    FreecamInvertPitch,
    FreecamToggleWithZ,
    FreecamFreezeTimer,
    FreecamHideHud,
    HideHud,
    HideStage,
    HideBall,
    HideItems,
    HideStobjs,
    HideEffects,
    IlMarkRomhacks,
    DisableWarps,
    DisableFallouts,
    BouncyFalloutPlane,
    IlBattleShowTime,
    IlBattleShowScore,
    IlBattleBuzzerOld,
    IlBattleMinimalBreakdown,
    UseCustomPhysics,
    SavestateSwitchToUnused,
    Randomizer,  // NEED TO DELETE
};

enum class U8Pref : u8 {
    CmChara,
    InputDispColor,
    BallColor,
    ApeColor,
    IlBattleLength,
    FreecamSpeedMult,
    Camera,
    BallRed,
    BallGreen,
    BallBlue,
    BallColorType,
    MenuBind,
    TimerType,
    Friction,
    Restitution,
    CameraMode,           // NEED TO DELETE
    CameraAngle,          // NEED TO DELETE
    CameraTurnRateScale,  // NEED TO DELETE
    CameraPivotHeight,    // NEED TO DELETE
    CameraHeight,         // NEED TO DELETE
    Gravity,
    BallScale,     // NEED TO DELETE
    Acceleration,  // NEED TO DELETE
    ApeColorType,
    IlBattleBreakdown,
    InputDispColorType,
    InputDispRed,
    InputDispGreen,
    InputDispBlue,
    IlBattleReadyBind,
    FreecamToggleBind,
    SavestateClearBind,
};

void init();
void save();
void reset_all_defaults();

bool get(BoolPref bool_pref);
u8 get(U8Pref u8_pref);
void set(BoolPref bool_pref, bool value);
void set(U8Pref u8_pref, u8 value);
bool get_default(BoolPref bool_pref);
u8 get_default(U8Pref u8_pref);

}  // namespace pref
