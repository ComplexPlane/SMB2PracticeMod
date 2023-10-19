#pragma once

// Mod user preferences, backed by memory card save file

#include "mkb/mkb.h"

namespace pref {

enum class BoolPref : u8 {
    Savestates,
    InputDisp,
    InputDispCenterLocation,
    TimerShowRTA,
    InputDispNotchIndicators,
    IwTimer,
    CmTimer,
    JumpMod,
    BananaCounter9999,
    DpadControls,
    DebugMode,
    MuteBgm,
    MuteTimerDing,
    InputDispRawStickInputs,
    Freecam,
    Marathon,
    Moon,
    IlBattleDisplay,
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
    DisableFalloutVolumes,
    IlBattleShowTime,
    IlBattleShowScore,
    IlBattleBuzzerOld,
    UseCustomPhysics,
    SavestateDisableOverwrite,
    TimerShowSubtick,
    TimerShowFramesave,
    TimerShowUnrounded,
    TimerShowPause,
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
    ApeColorType,
    InputDispColorType,
    InputDispRed,
    InputDispGreen,
    InputDispBlue,
    TimerType,
    Friction,
    Restitution,
    IlBattleBreakdown,
    MenuBind,
    IlBattleReadyBind,
    FreecamToggleBind,
    SavestateClearBind,
    FalloutPlaneType,
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
