#include "ilbattle.h"
#include "mkb/mkb.h"
#include "mods/validate.h"
#include "systems/binds.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/libsavest.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

namespace ilbattle {

enum class IlBattleState {
    NotReady,
    WaitForFirstRetry,     // Mod is enabled but first retry hasn't occurred
    BattleRunning,         // Battle is running
    BuzzerBeater,          // Time has run out, but an attempt is still going
    BuzzerBeaterPostgoal,  // Final attempt is over, wait for postgoals for score
    BattleDone,            // Final attempt is over, show times in blue
};

enum class IlBattleLength {
    FiveMinutes = 0,
    SevenMinutes = 1,
    TenMinutes = 2,
    Endless = 3,
};

// main state
static IlBattleState s_state = IlBattleState::NotReady;
// ui constants
static constexpr s32 X = 160;
static constexpr s32 Y = 48;
static constexpr u32 CWIDTH = 12;
static constexpr u32 CHEIGHT = 16;
// time constants
static constexpr u32 SECOND_FRAMES = 60;                  // frames per second
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;  // frames per minute
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;    // frames per hour
// battle trackers
static u32 s_battle_frames = 0;
static u32 s_battle_length = 0;
static u32 s_battle_stage_id = 0;
static u32 s_main_mode_play_timer = 0;
// best time/score
static s16 s_best_frames = 0;
static u32 s_best_score = 0;
static u32 s_best_score_bananas = 0;
static u32 s_best_score_frames = 0;
// ties and retry counts
static u32 s_best_frames_ties = 0;
static u32 s_best_score_ties = 0;
static u32 s_attempts = 0;
static bool s_accepted_tie = false;
static bool s_accepted_retry = false;
// buzzer beaters
static u32 s_buzzer_message_count = 0;
static u32 s_rainbow = 0;
static bool s_time_buzzer = false;
static bool s_score_buzzer = false;

static void old_buzzer_display(u32 start_y) {
    s_buzzer_message_count = (s_buzzer_message_count + 1) % 30;
    if (s_buzzer_message_count >= 0)
        draw::debug_text(X - 12 * CWIDTH, start_y + 1 * CHEIGHT, draw::RED,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 5)
        draw::debug_text(X - 12 * CWIDTH, start_y + 2 * CHEIGHT, draw::ORANGE,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 10)
        draw::debug_text(X - 12 * CWIDTH, start_y + 3 * CHEIGHT, draw::GOLD,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 15)
        draw::debug_text(X - 12 * CWIDTH, start_y + 4 * CHEIGHT, draw::GREEN,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 20)
        draw::debug_text(X - 12 * CWIDTH, start_y + 5 * CHEIGHT, draw::BLUE,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 25)
        draw::debug_text(X - 12 * CWIDTH, start_y + 6 * CHEIGHT, draw::BRIGHT_PURPLE,
                         "EPIC BUZZER BEATER B)");
}

static void battle_display(mkb::GXColor text_color) {
    u32 battle_hours = s_battle_frames / HOUR_FRAMES;
    u32 battle_minutes = s_battle_frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 battle_seconds = s_battle_frames % MINUTE_FRAMES / SECOND_FRAMES;

    u32 best_seconds = s_best_frames / SECOND_FRAMES;
    u32 best_centiseconds = (s_best_frames % SECOND_FRAMES) * 100 / 60;

    u32 best_score_seconds = s_best_score_frames / SECOND_FRAMES;
    u32 best_score_centiseconds = (s_best_score_frames % SECOND_FRAMES) * 100 / 60;

    u32 current_y = Y;

    mkb::GXColor time_color = s_time_buzzer ? draw::num_to_rainbow(s_rainbow) : text_color;
    mkb::GXColor score_color = s_score_buzzer ? draw::num_to_rainbow(s_rainbow) : text_color;

    draw::debug_text(X - 12 * CWIDTH, Y, text_color, "ELAPSED:");
    if (battle_hours > 0) {
        draw::debug_text(X - 6, Y, text_color, "%d:%02d:%02d", battle_hours, battle_minutes,
                         battle_seconds);
    } else {
        draw::debug_text(X - 6, Y, text_color, "%02d:%02d", battle_minutes, battle_seconds);
    }

    if (pref::get(pref::BoolPref::IlBattleShowTime)) {
        current_y += CHEIGHT;
        draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "BEST TIME:");
        if (pref::get(pref::BoolPref::IlBattleTieCount) && s_best_frames_ties > 0) {
            draw::debug_text(X - 6, current_y, time_color, "%d.%02d (%d)", best_seconds,
                             best_centiseconds, s_best_frames_ties + 1);
        } else {
            draw::debug_text(X - 6, current_y, time_color, "%d.%02d", best_seconds,
                             best_centiseconds);
        }
    }
    if (pref::get(pref::BoolPref::IlBattleShowScore)) {
        current_y += CHEIGHT;
        draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "BEST SCORE:");
        if (pref::get(pref::BoolPref::IlBattleTieCount) && s_best_score_ties > 0) {
            draw::debug_text(X - 6, current_y, score_color, "%d (%d)", s_best_score,
                             s_best_score_ties + 1);
        } else {
            draw::debug_text(X - 6, current_y, score_color, "%d", s_best_score);
        }

        // breakdown
        u8 breakdown_value = pref::get(pref::U8Pref::IlBattleBreakdown);
        if (breakdown_value == 1) {
            // minimal
            current_y += CHEIGHT;
            draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "BREAKDOWN:");
            draw::debug_text(X - 6, current_y, text_color, "%d.%02d [%d]", best_score_seconds,
                             best_score_centiseconds, s_best_score_bananas);
        } else if (breakdown_value == 2) {
            // full
            current_y += CHEIGHT;
            draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "  BANANAS:");
            draw::debug_text(X - 6, current_y, text_color, "%d", s_best_score_bananas);
            current_y += CHEIGHT;
            draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "  TIMER:");
            draw::debug_text(X - 6, current_y, text_color, "%d.%02d", best_score_seconds,
                             best_score_centiseconds);
        }
    }
    if (pref::get(pref::BoolPref::IlBattleAttemptCount)) {
        current_y += CHEIGHT;
        draw::debug_text(X - 12 * CWIDTH, current_y, text_color, "ATTEMPTS:");
        draw::debug_text(X - 6, current_y, text_color, "%d", s_attempts);
    }

    if (pref::get(pref::BoolPref::IlBattleBuzzerOld) &&
        ((s_time_buzzer && pref::get(pref::BoolPref::IlBattleShowTime)) ||
         (s_score_buzzer && pref::get(pref::BoolPref::IlBattleShowScore)))) {
        old_buzzer_display(current_y + CHEIGHT);
    }
}

static u32 score_calc(u32 score) {
    u32 igt_score = mkb::mode_info.stage_time_frames_remaining * 100 / 60;  // Score from timer
    u32 goal_bonus = 0;                                                     // Blue goal (no bonus)
    if (mkb::mode_info.entered_goal_type == 1) {
        goal_bonus = 10000;  // Green goal bonus
    } else if (mkb::mode_info.entered_goal_type == 2) {
        goal_bonus = 20000;  // Red goal bonus
    }
    if (mkb::mode_info.stage_time_frames_remaining >
        mkb::mode_info.stage_time_limit / 2) {  // With time bonus
        return score + (igt_score + goal_bonus) * 2;
    }
    return score + (igt_score + goal_bonus);  // Without time bonus
}

static u32 convert_battle_length(IlBattleLength battle_length_choice) {
    if (battle_length_choice == IlBattleLength::FiveMinutes) return 5 * 60 * 60;
    if (battle_length_choice == IlBattleLength::SevenMinutes) return 7 * 60 * 60;
    if (battle_length_choice == IlBattleLength::TenMinutes) return 10 * 60 * 60;
    return 0;  // endless battle
}

void clear_display() {
    s_battle_frames = 0;
    s_best_frames = 0;
    s_best_score = 0;
    s_buzzer_message_count = 0;
    s_best_score_bananas = 0;
    s_best_score_frames = 0;
    s_best_frames_ties = 0;
    s_best_score_ties = 0;
    s_attempts = 0;
    s_time_buzzer = false;
    s_score_buzzer = false;
    s_battle_length =
        convert_battle_length(IlBattleLength(pref::get(pref::U8Pref::IlBattleLength)));
}

static void new_battle() {
    clear_display();
    s_state = IlBattleState::WaitForFirstRetry;
}

static void track_first_retry() {
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    if (!paused_now && mkb::sub_mode == mkb::SMD_GAME_READY_INIT) {
        new_battle();
        s_state = IlBattleState::BattleRunning;
        s_battle_stage_id = mkb::current_stage_id;
    }
}

static void run_battle_timer() {
    if (mkb::sub_mode == mkb::SMD_GAME_PLAY_INIT) {
        if (!s_accepted_retry) {  // track attempt counts
            s_attempts++;
            s_accepted_retry = true;
        }
    }
    if (IlBattleLength(pref::get(pref::U8Pref::IlBattleLength)) == IlBattleLength::Endless) {
        // timer is endless
        s_battle_frames++;
    } else if (s_battle_frames < s_battle_length) {
        s_battle_frames++;
    } else {
        s_state = IlBattleState::BuzzerBeater;
    }
}

void update_best() {
    s16 current_frames = mkb::mode_info.stage_time_frames_remaining;
    u32 current_score = mkb::balls[mkb::curr_player_idx].score;

    u32 calculated_score = score_calc(current_score);

    // increment ties
    if (!s_accepted_tie) {
        if (current_frames == s_best_frames) {
            s_best_frames_ties++;
        }
        if (calculated_score == s_best_score) {
            s_best_score_ties++;
        }
        s_accepted_tie = true;
    }

    // update times
    if (current_frames > s_best_frames) {
        s_best_frames_ties = 0;
        s_best_frames = current_frames;
    }
    if (calculated_score > s_best_score) {
        s_best_score_ties = 0;
        s_best_score = calculated_score;
        s_best_score_bananas = mkb::balls[mkb::curr_player_idx].banana_count;
        s_best_score_frames = current_frames;
    }
}

void validate_attempt() {
    if (!validate::was_run_valid(true)) return;

    bool on_incorrect_stage = s_main_mode_play_timer > 0 &&
                              s_battle_stage_id != mkb::current_stage_id &&
                              mkb::main_mode == mkb::MD_GAME;
    if (on_incorrect_stage) return;

    if (s_state == IlBattleState::BattleRunning) {
        update_best();
    } else if (s_state == IlBattleState::BuzzerBeater) {
        s16 pre_buzzer_time = s_best_frames;
        u32 pre_buzzer_score = s_best_score;
        update_best();
        s_state = IlBattleState::BuzzerBeaterPostgoal;
        // time buzzer beater
        if (pre_buzzer_time < s_best_frames) {
            s_time_buzzer = true;
        }
        // score buzzer beater
        if (pre_buzzer_score < s_best_score) {
            s_score_buzzer = true;
        }
    }
}

static void final_attempt() {
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    // End battle if: Paused, Fallout, or Time Over
    if (paused_now || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN ||
        mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
        s_state = IlBattleState::BattleDone;
    }
}

static void track_postgoal() {
    if (mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT && mkb::sub_mode != mkb::SMD_GAME_GOAL_MAIN) {
        u32 pre_buzzer_score = s_best_score;
        update_best();
        s_state = IlBattleState::BattleDone;
        // score buzzer beater (track again if there are post goals)
        if (pre_buzzer_score < s_best_score) {
            s_score_buzzer = true;
        }
    }
}

void tick() {
    if (!pref::get(pref::BoolPref::IlBattleDisplay)) {
        clear_display();
        s_state = IlBattleState::NotReady;
    }

    if (mkb::sub_mode != mkb::SMD_GAME_PLAY_INIT) {
        s_accepted_retry = false;
    }

    if (mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN) {
        s_accepted_tie = false;
    }

    if (mkb::main_mode == mkb::MD_GAME) {
        s_main_mode_play_timer++;
    } else {
        s_main_mode_play_timer = 0;
    }

    if (mkb::main_mode == mkb::MD_GAME &&
        (mkb::main_game_mode == mkb::STORY_MODE || mkb::main_game_mode == mkb::CHALLENGE_MODE)) {
        new_battle();
        return;
    }

    s_rainbow = (s_rainbow + 11) % 1080;

    switch (s_state) {
        case IlBattleState::WaitForFirstRetry: {
            track_first_retry();
            break;
        }
        case IlBattleState::BattleRunning: {
            run_battle_timer();
            break;
        }
        case IlBattleState::BuzzerBeater: {
            final_attempt();
            break;
        }
        case IlBattleState::BuzzerBeaterPostgoal: {
            track_postgoal();
            break;
        }
        default: {
        }
    }

    // Reset display if menu when battle over
    if (mkb::main_mode != mkb::MD_GAME &&
        (s_state == IlBattleState::WaitForFirstRetry || s_state == IlBattleState::BattleDone)) {
        s_state = IlBattleState::NotReady;
        clear_display();
    }

    // Resets battles when Dpad Down is pressed
    if (mkb::main_mode == mkb::MD_GAME &&
        binds::bind_pressed(pref::get(pref::U8Pref::IlBattleReadyBind))) {
        new_battle();
    }
}

void disp() {
    if (mkb::main_mode == mkb::MD_GAME &&
        (mkb::main_game_mode == mkb::STORY_MODE || mkb::main_game_mode == mkb::CHALLENGE_MODE)) {
        return;
    }

    if (!pref::get(pref::BoolPref::IlBattleDisplay)) return;

    switch (s_state) {
        case IlBattleState::NotReady: {
            if (mkb::main_mode != mkb::MD_GAME) return;
            u8 input = pref::get(pref::U8Pref::IlBattleReadyBind);
            char buf[25];
            binds::get_bind_str(input, buf);
            draw::debug_text(X - 12 * CWIDTH, Y, draw::LIGHT_PURPLE, "NOT READY");
            draw::debug_text(X - 12 * CWIDTH, Y + CHEIGHT, draw::LIGHT_PURPLE, "%s to ready", buf);
            break;
        }
        case IlBattleState::WaitForFirstRetry: {
            if (mkb::main_mode == mkb::MD_GAME) {
                draw::debug_text(X - 12 * CWIDTH, Y, draw::GOLD, "READY");
                draw::debug_text(X - 12 * CWIDTH, Y + CHEIGHT, draw::GOLD, "Retry to begin");
            }
            break;
        }
        case IlBattleState::BattleRunning:
        case IlBattleState::BuzzerBeater: {
            if (s_main_mode_play_timer > 0 && s_battle_stage_id != mkb::current_stage_id &&
                mkb::main_mode == mkb::MD_GAME) {
                draw::debug_text(X - 12 * CWIDTH, Y, draw::RED, "WRONG STAGE");
            } else {
                battle_display(draw::LIGHT_GREEN);
            }
            break;
        }
        case IlBattleState::BuzzerBeaterPostgoal: {
            if (pref::get(pref::BoolPref::IlBattleShowScore)) {
                battle_display(draw::LIGHT_GREEN);
            } else {
                battle_display(draw::LIGHT_PURPLE);
            }
            break;
        }
        case IlBattleState::BattleDone: {
            battle_display(draw::LIGHT_PURPLE);
            break;
        }
    }
}
}  // Namespace ilbattle
