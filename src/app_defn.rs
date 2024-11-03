use crate::{
    app_modules, mkb,
    mods::{
        ballcolor::BallColor, banans::Banans, cmseg::CmSeg, dpad::Dpad, fallout::Fallout,
        freecam::Freecam, gotostory::GoToStory, ilbattle::IlBattle, inputdisp::InputDisplay,
        jump::Jump, savestates_ui::SaveStatesUi, scratch::Scratch, timer::Timer, unlock::Unlock,
    },
    systems::{
        binds::Binds,
        draw::Draw,
        menu_impl::MenuImpl,
        pad::Pad,
        pref::{BoolPref, Pref},
    },
    utils::libsavestate::LibSaveState,
};

app_modules!(
    Pad { MainLoopLoad, Tick },
    Binds { Tick },
    MenuImpl { Tick },
    Unlock { Tick },
    LibSaveState { MainLoopLoad },
    SaveStatesUi { MainLoopLoad, Tick },
    Fallout { MainLoopLoad, Tick },
    Jump { Tick },
    InputDisplay { Tick, Draw },
    GoToStory { Tick },
    CmSeg {
        MainLoopLoad,
        Tick,
        Draw
    },
    Banans { Tick },
    BallColor {
        MainLoopLoad,
        MainGameLoad,
        Tick
    },
    Freecam { MainLoopLoad, Tick },
    Timer { Draw },
    Dpad { MainLoopLoad },
    IlBattle {
        MainGameLoad,
        Tick,
        Draw,
    },
    Scratch { Tick, Draw },
    Draw { Draw },
    Pref { Tick },
);

pub fn init() {
    critical_section::with(|cs| {
        APP_CONTEXT.borrow(cs).init();
    });
}

pub fn tick() {
    unsafe {
        // Replace overwritten function call
        mkb::perf_init_timer(4);

        critical_section::with(|cs| {
            let cx = APP_CONTEXT.borrow(cs);
            let pref = cx.pref.borrow_mut();
            if pref.get_bool(BoolPref::DebugMode) {
                mkb::dip_switches |= mkb::DIP_DEBUG | mkb::DIP_DISP;
            } else {
                mkb::dip_switches &= !(mkb::DIP_DEBUG | mkb::DIP_DISP);
            }

            cx.pad.borrow_mut().on_frame_start();
        });
    }
}
