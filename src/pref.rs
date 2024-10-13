pub enum BoolPref {
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
    SavestateDisableOverwrite,
    IlBattleTieCount,
    IlBattleAttemptCount,
    TimerShowSubtick,
    TimerShowFramesave,
    TimerShowPause,
    JumpChangePhysics,
    JumpAllowWalljumps,
    CustomPhysicsDisp,
}

pub enum U8Pref {
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
    IlBattleBreakdown,
    MenuBind,
    IlBattleReadyBind,
    FreecamToggleBind,
    SavestateClearBind,
    FalloutPlaneType,
    StageEditVariant,
    JumpCount,
    PhysicsPreset,
    MonkeyType,
    JumpProfile,
}

pub fn get_bool(id: BoolPref) -> bool {
    false
}

pub fn get_u8(id: U8Pref) -> u8 {
    0
}

pub fn did_change_u8(id: U8Pref) -> bool {
    false
}