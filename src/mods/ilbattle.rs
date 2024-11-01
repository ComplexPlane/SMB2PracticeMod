use arrayvec::ArrayString;
use num_enum::TryFromPrimitive;

use crate::{
    app_defn::AppContext,
    cstr_buf, fmt, mkb,
    systems::{
        binds::{self, Binds},
        draw,
        pad::{Pad, Prio},
        pref::{BoolPref, Pref, U8Pref},
    },
};

use super::freecam::Freecam;

struct Context<'a> {
    pref: &'a mut Pref,
    freecam: &'a mut Freecam,
    binds: &'a mut Binds,
    pad: &'a mut Pad,
}

#[derive(Clone, Copy, PartialEq, Eq)]
enum IlBattleState {
    NotReady,
    WaitForFirstRetry,    // Mod is enabled but first retry hasn't occurred
    BattleRunning,        // Battle is running
    BuzzerBeater,         // Time has run out, but an attempt is still going
    BuzzerBeaterPostgoal, // Final attempt is over, wait for postgoals for score
    BattleDone,           // Final attempt is over, show times in blue
}

#[derive(Clone, Copy, TryFromPrimitive)]
#[repr(u8)]
enum IlBattleLength {
    FiveMinutes = 0,
    SevenMinutes = 1,
    TenMinutes = 2,
    Endless = 3,
}

// UI constants
const X: u32 = 160;
const Y: u32 = 48;
const CWIDTH: u32 = 12;
const CHEIGHT: u32 = 16;

// Time constants
const SECOND_FRAMES: u32 = 60; // frames per second
const MINUTE_FRAMES: u32 = SECOND_FRAMES * 60; // frames per minute
const HOUR_FRAMES: u32 = MINUTE_FRAMES * 60; // frames per hour

pub struct IlBattle {
    // main state
    state: IlBattleState,

    // battle trackers
    battle_frames: u32,
    battle_length: u32,
    battle_stage_id: u32,
    main_mode_play_timer: u32,

    // best time/score
    best_frames: i16,
    best_score: u32,
    best_score_bananas: u32,
    best_score_frames: u32,

    // ties and retry counts
    best_frames_ties: u32,
    best_score_ties: u32,
    attempts: u32,
    accepted_tie: bool,
    accepted_retry: bool,

    // buzzer beaters
    buzzer_message_count: u32,
    rainbow: u32,
    time_buzzer: bool,
    score_buzzer: bool,
}

impl IlBattle {
    pub fn new() -> Self {
        Self {
            state: IlBattleState::NotReady,
            battle_frames: 0,
            battle_length: 0,
            battle_stage_id: 0,
            main_mode_play_timer: 0,
            best_frames: 0,
            best_score: 0,
            best_score_bananas: 0,
            best_score_frames: 0,
            best_frames_ties: 0,
            best_score_ties: 0,
            attempts: 0,
            accepted_tie: false,
            accepted_retry: false,
            buzzer_message_count: 0,
            rainbow: 0,
            time_buzzer: false,
            score_buzzer: false,
        }
    }

    fn old_buzzer_display(&mut self, start_y: u32) {
        self.buzzer_message_count = (self.buzzer_message_count + 1) % 30;
        if self.buzzer_message_count >= 0 {
            draw::debug_text(
                X - 12 * CWIDTH,
                start_y + 1 * CHEIGHT,
                draw::RED,
                "EPIC BUZZER BEATER B)",
            );
        }
        if self.buzzer_message_count >= 5 {
            draw::debug_text(
                X - 12 * CWIDTH,
                start_y + 2 * CHEIGHT,
                draw::ORANGE,
                "EPIC BUZZER BEATER B)",
            );
        }
        if self.buzzer_message_count >= 10 {
            draw::debug_text(
                X - 12 * CWIDTH,
                start_y + 3 * CHEIGHT,
                draw::GOLD,
                "EPIC BUZZER BEATER B)",
            );
        }
        if self.buzzer_message_count >= 15 {
            draw::debug_text(
                X - 12 * CWIDTH,
                start_y + 4 * CHEIGHT,
                draw::GREEN,
                "EPIC BUZZER BEATER B)",
            );
        }
        if self.buzzer_message_count >= 20 {
            draw::debug_text(
                X - 12 * CWIDTH,
                start_y + 5 * CHEIGHT,
                draw::BLUE,
                "EPIC BUZZER BEATER B)",
            );
        }
        if self.buzzer_message_count >= 25 {
            draw::debug_text(
                X - 12 * CWIDTH,
                start_y + 6 * CHEIGHT,
                draw::BRIGHT_PURPLE,
                "EPIC BUZZER BEATER B)",
            );
        }
    }

    fn battle_display(&mut self, text_color: mkb::GXColor, cx: &mut Context) {
        let battle_hours = self.battle_frames / HOUR_FRAMES;
        let battle_minutes = self.battle_frames % HOUR_FRAMES / MINUTE_FRAMES;
        let battle_seconds = self.battle_frames % MINUTE_FRAMES / SECOND_FRAMES;

        let best_seconds = self.best_frames as u32 / SECOND_FRAMES;
        let best_centiseconds = (self.best_frames as u32 % SECOND_FRAMES) * 100 / 60;

        let best_score_seconds = self.best_score_frames / SECOND_FRAMES;
        let best_score_centiseconds = (self.best_score_frames % SECOND_FRAMES) * 100 / 60;

        let mut current_y = Y;

        let time_color = if self.time_buzzer {
            draw::num_to_rainbow(self.rainbow)
        } else {
            text_color
        };
        let score_color = if self.score_buzzer {
            draw::num_to_rainbow(self.rainbow)
        } else {
            text_color
        };

        draw::debug_text(X - 12 * CWIDTH, Y, text_color, "ELAPSED:");
        if battle_hours > 0 {
            draw::debug_text(
                X - 6,
                Y,
                text_color,
                &fmt!(
                    32,
                    c"%d:%02d:%02d",
                    battle_hours,
                    battle_minutes,
                    battle_seconds
                ),
            );
        } else {
            draw::debug_text(
                X - 6,
                Y,
                text_color,
                &fmt!(32, c"%02d:%02d", battle_minutes, battle_seconds),
            );
        }

        if cx.pref.get_bool(BoolPref::IlBattleShowTime) {
            current_y += CHEIGHT;
            draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "BEST TIME:");
            if cx.pref.get_bool(BoolPref::IlBattleTieCount) && self.best_frames_ties > 0 {
                draw::debug_text(
                    X - 6,
                    current_y,
                    time_color,
                    &fmt!(
                        32,
                        c"%d.%02d (%d)",
                        best_seconds,
                        best_centiseconds,
                        self.best_frames_ties + 1
                    ),
                );
            } else {
                draw::debug_text(
                    X - 6,
                    current_y,
                    time_color,
                    &fmt!(32, c"%d.%02d", best_seconds, best_centiseconds),
                );
            }
        }

        if cx.pref.get_bool(BoolPref::IlBattleShowScore) {
            current_y += CHEIGHT;
            draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "BEST SCORE:");
            if cx.pref.get_bool(BoolPref::IlBattleTieCount) && self.best_score_ties > 0 {
                draw::debug_text(
                    X - 6,
                    current_y,
                    score_color,
                    &fmt!(32, c"%d (%d)", self.best_score, self.best_score_ties + 1),
                );
            } else {
                draw::debug_text(
                    X - 6,
                    current_y,
                    score_color,
                    &fmt!(32, c"%d", self.best_score),
                );
            }

            // breakdown
            let breakdown_value = cx.pref.get_u8(U8Pref::IlBattleBreakdown);
            if breakdown_value == 1 {
                // minimal
                current_y += CHEIGHT;
                draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "BREAKDOWN:");
                draw::debug_text(
                    X - 6,
                    current_y,
                    text_color,
                    &fmt!(
                        32,
                        c"%d.%02d [%d]",
                        best_score_seconds,
                        best_score_centiseconds,
                        self.best_score_bananas
                    ),
                );
            } else if breakdown_value == 2 {
                // full
                current_y += CHEIGHT;
                draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "  BANANAS:");
                draw::debug_text(
                    X - 6,
                    current_y,
                    text_color,
                    &fmt!(32, c"%d", self.best_score_bananas),
                );
                current_y += CHEIGHT;
                draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "  TIMER:");
                draw::debug_text(
                    X - 6,
                    current_y,
                    text_color,
                    &fmt!(32, c"%d.%02d", best_score_seconds, best_score_centiseconds),
                );
            }
        }

        if cx.pref.get_bool(BoolPref::IlBattleAttemptCount) {
            current_y += CHEIGHT;
            draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "ATTEMPTS:");
            draw::debug_text(
                X - 6,
                current_y,
                text_color,
                &fmt!(32, c"%d", self.attempts),
            );
        }

        if cx.pref.get_bool(BoolPref::IlBattleBuzzerOld)
            && ((self.time_buzzer && cx.pref.get_bool(BoolPref::IlBattleShowTime))
                || (self.score_buzzer && cx.pref.get_bool(BoolPref::IlBattleShowScore)))
        {
            self.old_buzzer_display(current_y + CHEIGHT);
        }
    }

    fn score_calc(score: u32) -> u32 {
        unsafe {
            let igt_score = (mkb::mode_info.stage_time_frames_remaining * 100 / 60) as u32; // Score from timer
            let mut goal_bonus = 0; // Blue goal (no bonus)
            if mkb::mode_info.entered_goal_type == 1 {
                goal_bonus = 10000; // Green goal bonus
            } else if mkb::mode_info.entered_goal_type == 2 {
                goal_bonus = 20000; // Red goal bonus
            }
            if mkb::mode_info.stage_time_frames_remaining as u16
                > mkb::mode_info.stage_time_limit / 2
            {
                // With time bonus
                score + (igt_score + goal_bonus) * 2
            } else {
                score + (igt_score + goal_bonus) // Without time bonus
            }
        }
    }

    fn convert_battle_length(battle_length_choice: IlBattleLength) -> u32 {
        match battle_length_choice {
            IlBattleLength::FiveMinutes => 5 * 60 * 60,
            IlBattleLength::SevenMinutes => 7 * 60 * 60,
            IlBattleLength::TenMinutes => 10 * 60 * 60,
            IlBattleLength::Endless => 0, // endless battle
        }
    }

    fn clear_display(&mut self, cx: &mut Context) {
        let battle_len = cx.pref.get_u8(U8Pref::IlBattleLength);
        let battle_len = IlBattleLength::try_from(battle_len).unwrap();

        self.battle_frames = 0;
        self.best_frames = 0;
        self.best_score = 0;
        self.buzzer_message_count = 0;
        self.best_score_bananas = 0;
        self.best_score_frames = 0;
        self.best_frames_ties = 0;
        self.best_score_ties = 0;
        self.attempts = 0;
        self.time_buzzer = false;
        self.score_buzzer = false;
        self.battle_length = Self::convert_battle_length(battle_len);
    }

    fn new_battle(&mut self, cx: &mut Context) {
        self.clear_display(cx);
        self.state = IlBattleState::WaitForFirstRetry;
    }

    fn track_first_retry(&mut self, cx: &mut Context) {
        unsafe {
            let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
            if !paused_now && mkb::sub_mode == mkb::SMD_GAME_READY_INIT {
                self.new_battle(cx);
                self.state = IlBattleState::BattleRunning;
                self.battle_stage_id = mkb::current_stage_id as u32;
            }
        }
    }

    fn run_battle_timer(&mut self, cx: &mut Context) {
        unsafe {
            if mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT && !self.accepted_retry {
                // track attempt counts
                self.attempts += 1;
                self.accepted_retry = true;
            }

            let battle_length: IlBattleLength =
                core::mem::transmute(cx.pref.get_u8(U8Pref::IlBattleLength));
            if matches!(battle_length, IlBattleLength::Endless) {
                // timer is endless
                self.battle_frames += 1;
            } else if self.battle_frames < self.battle_length {
                self.battle_frames += 1;
            } else {
                self.state = IlBattleState::BuzzerBeater;
            }
        }
    }

    fn update_best(&mut self) {
        unsafe {
            let current_frames = mkb::mode_info.stage_time_frames_remaining;
            let current_score = mkb::balls[mkb::curr_player_idx as usize].score as u32;

            let calculated_score = Self::score_calc(current_score);

            // increment ties
            if !self.accepted_tie {
                if current_frames == self.best_frames {
                    self.best_frames_ties += 1;
                }
                if calculated_score == self.best_score {
                    self.best_score_ties += 1;
                }
                self.accepted_tie = true;
            }

            // update times
            if current_frames > self.best_frames {
                self.best_frames_ties = 0;
                self.best_frames = current_frames;
            }
            if calculated_score > self.best_score {
                self.best_score_ties = 0;
                self.best_score = calculated_score;
                self.best_score_bananas =
                    mkb::balls[mkb::curr_player_idx as usize].banana_count as u32;
                self.best_score_frames = current_frames as u32;
            }
        }
    }

    pub fn validate_attempt(&mut self) {
        unsafe {
            // TODO
            // if !validate::was_run_valid(true) {
            //     return;
            // }

            let on_incorrect_stage = self.main_mode_play_timer > 0
                && self.battle_stage_id != mkb::current_stage_id as u32
                && mkb::main_mode == mkb::MD_GAME;

            if on_incorrect_stage {
                return;
            }

            match self.state {
                IlBattleState::BattleRunning => {
                    self.update_best();
                }
                IlBattleState::BuzzerBeater => {
                    let pre_buzzer_time = self.best_frames;
                    let pre_buzzer_score = self.best_score;
                    self.update_best();
                    self.state = IlBattleState::BuzzerBeaterPostgoal;

                    // time buzzer beater
                    if pre_buzzer_time < self.best_frames {
                        self.time_buzzer = true;
                    }
                    // score buzzer beater
                    if pre_buzzer_score < self.best_score {
                        self.score_buzzer = true;
                    }
                }
                _ => {}
            }
        }
    }

    fn final_attempt(&mut self) {
        unsafe {
            let paused_now = *(0x805BC474 as *const u32) & 8 != 0;
            // End battle if: Paused, Fallout, or Time Over
            if paused_now
                || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT
                || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN
                || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT
                || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN
            {
                self.state = IlBattleState::BattleDone;
            }
        }
    }

    fn track_postgoal(&mut self) {
        unsafe {
            if mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT && mkb::sub_mode != mkb::SMD_GAME_GOAL_MAIN
            {
                let pre_buzzer_score = self.best_score;
                self.update_best();
                self.state = IlBattleState::BattleDone;
                // score buzzer beater (track again if there are post goals)
                if pre_buzzer_score < self.best_score {
                    self.score_buzzer = true;
                }
            }
        }
    }

    pub fn tick(&mut self, cx: &AppContext) {
        let cx = &mut Context {
            pref: &mut cx.pref.borrow_mut(),
            freecam: &mut cx.freecam.borrow_mut(),
            binds: &mut cx.binds.borrow_mut(),
            pad: &mut cx.pad.borrow_mut(),
        };

        unsafe {
            if !cx.pref.get_bool(BoolPref::IlBattleDisplay) {
                self.clear_display(cx);
                self.state = IlBattleState::NotReady;
            }

            if mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT {
                self.accepted_retry = false;
            }

            if mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN {
                self.accepted_tie = false;
            }

            if mkb::main_mode == mkb::MD_GAME {
                self.main_mode_play_timer += 1;
            } else {
                self.main_mode_play_timer = 0;
            }

            if mkb::main_mode == mkb::MD_GAME
                && (mkb::main_game_mode == mkb::STORY_MODE
                    || mkb::main_game_mode == mkb::CHALLENGE_MODE)
            {
                self.new_battle(cx);
                return;
            }

            self.rainbow = (self.rainbow + 11) % 1080;

            match self.state {
                IlBattleState::WaitForFirstRetry => {
                    self.track_first_retry(cx);
                }
                IlBattleState::BattleRunning => {
                    self.run_battle_timer(cx);
                }
                IlBattleState::BuzzerBeater => {
                    self.final_attempt();
                }
                IlBattleState::BuzzerBeaterPostgoal => {
                    self.track_postgoal();
                }
                IlBattleState::NotReady | IlBattleState::BattleDone => {}
            }

            // Reset display if menu when battle over
            if mkb::main_mode != mkb::MD_GAME
                && (self.state == IlBattleState::WaitForFirstRetry
                    || self.state == IlBattleState::BattleDone)
            {
                self.state = IlBattleState::NotReady;
                self.clear_display(cx);
            }

            // Resets battles when Dpad Down is pressed
            if mkb::main_mode == mkb::MD_GAME
                && cx.binds.bind_pressed(
                    cx.pref.get_u8(U8Pref::IlBattleReadyBind),
                    Prio::Low,
                    cx.pad,
                )
            {
                self.new_battle(cx);
            }
        }
    }

    pub fn draw(&mut self, cx: &AppContext) {
        let cx = &mut Context {
            pref: &mut cx.pref.borrow_mut(),
            freecam: &mut cx.freecam.borrow_mut(),
            binds: &mut cx.binds.borrow_mut(),
            pad: &mut cx.pad.borrow_mut(),
        };

        unsafe {
            if mkb::main_mode == mkb::MD_GAME
                && (mkb::main_game_mode == mkb::STORY_MODE
                    || mkb::main_game_mode == mkb::CHALLENGE_MODE)
            {
                return;
            }
        }

        if !cx.pref.get_bool(BoolPref::IlBattleDisplay) || cx.freecam.should_hide_hud(cx.pref) {
            return;
        }

        match self.state {
            IlBattleState::NotReady => {
                if unsafe { mkb::main_mode } != mkb::MD_GAME {
                    return;
                }
                let input = cx.pref.get_u8(U8Pref::IlBattleReadyBind);
                let mut buf = ArrayString::<32>::new();
                cx.binds.get_bind_str(input, &mut buf);
                draw::debug_text(X - 12 * CWIDTH, Y, draw::LIGHT_PURPLE, "NOT READY");
                draw::debug_text(
                    X - 12 * CWIDTH,
                    Y + CHEIGHT,
                    draw::LIGHT_PURPLE,
                    &fmt!(32, c"%s to ready", cstr_buf!(buf)),
                );
            }
            IlBattleState::WaitForFirstRetry => {
                if unsafe { mkb::main_mode } == mkb::MD_GAME {
                    draw::debug_text(X - 12 * CWIDTH, Y, draw::GOLD, "READY");
                    draw::debug_text(X - 12 * CWIDTH, Y + CHEIGHT, draw::GOLD, "Retry to begin");
                }
            }
            IlBattleState::BattleRunning | IlBattleState::BuzzerBeater => {
                if self.main_mode_play_timer > 0
                    && self.battle_stage_id != unsafe { mkb::current_stage_id } as u32
                    && unsafe { mkb::main_mode } == mkb::MD_GAME
                {
                    draw::debug_text(X - 12 * CWIDTH, Y, draw::RED, "WRONG STAGE");
                } else {
                    self.battle_display(draw::LIGHT_GREEN, cx);
                }
            }
            IlBattleState::BuzzerBeaterPostgoal => {
                if cx.pref.get_bool(BoolPref::IlBattleShowScore) {
                    self.battle_display(draw::LIGHT_GREEN, cx);
                } else {
                    self.battle_display(draw::LIGHT_PURPLE, cx);
                }
            }
            IlBattleState::BattleDone => {
                self.battle_display(draw::LIGHT_PURPLE, cx);
            }
        }
    }
}
