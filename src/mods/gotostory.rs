use crate::{app_defn::AppContext, mkb};

#[derive(Default)]
enum State {
    #[default]
    Default,
    LoadMenuReq,
    LoadStoryReq,
}

pub struct GoToStory {
    state: State,
}

impl GoToStory {
    pub fn new() -> Self {
        Self {
            state: State::Default,
        }
    }

    pub fn load_storymode(&mut self) {
        unsafe {
            if mkb::main_mode == mkb::MD_SEL {
                self.state = State::LoadStoryReq;
            } else {
                // Mainloop may not be loaded, so loading storymode may cause main_game.rel to be
                // loaded in a different place
                // So, load the main menu first
                self.state = State::LoadMenuReq;
            }
        }
    }

    fn reset_screenfade_state() {
        unsafe {
            // Reset screenfade parameters to that of "begin fading back from black screen"
            mkb::g_screenfade_flags = 0x00000100;
            mkb::g_screenfade_color = 0x00000000;
            mkb::g_screenfading1 = 0x0000001a;
            mkb::g_screenfading2 = 0x0000001b;
        }
    }

    pub fn tick(&mut self, _cx: &AppContext) {
        unsafe {
            match self.state {
                State::LoadMenuReq => {
                    // Unpause the game to avoid weird darkening issues
                    mkb::g_some_other_flags &= !mkb::OF_GAME_PAUSED;
                    mkb::main_mode_request = mkb::MD_SEL;
                    // Using REINIT instead of INIT seems to prevent weird game state issues, like
                    // the Final Stage sprite being shown when loading a stage in story mode
                    mkb::sub_mode_request = mkb::SMD_SEL_NGC_REINIT;
                    self.state = State::LoadStoryReq;

                    // Set menu state to have chosen Main Game -> Story Mode
                    mkb::sel_menu_info.menu_stack_ptr = 1;
                    mkb::sel_menu_info.menu_stack[0] = 0; // ??
                    mkb::sel_menu_info.menu_stack[1] = 7; // Main game
                    mkb::g_focused_root_menu = 0;
                    mkb::g_focused_maingame_menu = 0;

                    Self::reset_screenfade_state();
                }
                State::LoadStoryReq => {
                    mkb::main_mode_request = mkb::MD_GAME;
                    mkb::sub_mode_request = mkb::SMD_GAME_SCENARIO_INIT;

                    // Reset this timer to prevent dark pause menu when leaving an Exit Game menu. The game
                    // draws the pause menu sprite at a different depth depending on this global, which can
                    // incorrectly place it behind the pause menu dim quad.
                    mkb::g_playpoint_msg_counter = 0;

                    self.state = State::Default;
                    Self::reset_screenfade_state();
                }
                State::Default => {}
            }
        }
    }
}
