#[derive(Clone, Copy)]
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

#[derive(Clone, Copy)]
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

pub struct Pref {}

impl Pref {
    pub fn new() -> Self {
        Self {}
    }

    pub fn init(&mut self) {}

    pub fn tick(&mut self) {}

    pub fn get_bool(&self, id: BoolPref) -> bool {
        true
    }

    pub fn get_u8(&self, id: U8Pref) -> u8 {
        0
    }

    pub fn did_change_u8(&self, id: U8Pref) -> bool {
        false
    }

    pub fn set_u8(&mut self, id: U8Pref, value: u8) {}

    pub fn set_bool(&mut self, id: BoolPref, value: bool) {}

    pub fn get_default_bool(&self, id: BoolPref) -> bool {
        true
    }

    pub fn get_default_u8(&self, id: U8Pref) -> u8 {
        0
    }

    pub fn save(&mut self) {}
}
