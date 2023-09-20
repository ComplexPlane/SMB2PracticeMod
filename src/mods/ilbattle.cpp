#include "ilbattle.h"
#include "../mkb/mkb.h"
#include "../systems/pad.h"
#include "../systems/pref.h"
#include "../utils/draw.h"
#include "../utils/patch.h"
#include "../utils/timerdisp.h"

namespace ilbattle {

enum class IlBattleState {
    WaitForFirstRetry,  // Mod is enabled but first retry hasnt ocurred
    BattleRunning,      // Battle is running
    BuzzerBeater,       // Time has run out, but an attempt is still going
    BattleDone,         // Final attempt is over, show times in red
    BattleDoneBuzzer,   // Final attempt is over, shows times in red &
};

static IlBattleState s_state = IlBattleState::BattleDone;

static constexpr s32 X = 160;
static constexpr s32 Y = 48;
static u32 s_battle_frames = 0;
static u32 s_best_frames = 0;
static u32 s_best_score = 0;
static u32 s_best_score_bananas = 0;
static u32 s_best_score_frames = 0;
static constexpr u32 SECOND_FRAMES = 60;                  // frames per second
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;  // frames per minute
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;    // frames per hour
static constexpr u32 CWIDTH = 12;
static constexpr u32 CHEIGHT = 16;
static bool s_invalid_run = false;
static u32 s_buzzer_message_count = 0;
static u32 s_rainbow = 0;
static u32 s_battle_length = 0;
static u32 s_battle_stage_id = 0;
static u32 s_main_mode_play_timer = 0;
// static u32 battle_length = pref::get_ilbattle_length();

static u32 battle_display(mkb::GXColor text, mkb::GXColor times) {
    u32 battle_hours = s_battle_frames / HOUR_FRAMES;
    u32 battle_minutes = s_battle_frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 battle_seconds = s_battle_frames % MINUTE_FRAMES / SECOND_FRAMES;

    u32 best_seconds = s_best_frames / SECOND_FRAMES;
    u32 best_centiseconds = (s_best_frames % SECOND_FRAMES) * 100 / 60;

    u32 best_score_seconds = s_best_score_frames / SECOND_FRAMES;
    u32 best_score_centiseconds = (s_best_score_frames % SECOND_FRAMES) * 100 / 60;

    u32 current_y = Y;

    draw::debug_text(X - 12 * CWIDTH, Y, text, "ELAPSED:");
    if (battle_hours > 0) {
        draw::debug_text(X, Y, times, "%d:%02d:%02d", battle_hours, battle_minutes, battle_seconds);
    } else {
        draw::debug_text(X, Y, times, "%02d:%02d", battle_minutes, battle_seconds);
    }

    if (pref::get(pref::BoolPref::IlBattleShowTime)) {
        current_y += CHEIGHT;
        draw::debug_text(X - 12 * CWIDTH, current_y, text, "BEST TIME:");
        draw::debug_text(X, current_y, times, "%d.%02d", best_seconds, best_centiseconds);
    }
    if (pref::get(pref::BoolPref::IlBattleShowScore)) {
        current_y += CHEIGHT;
        draw::debug_text(X - 12 * CWIDTH, current_y, text, "BEST SCORE:");
        draw::debug_text(X, current_y, times, "%d", s_best_score);

        // breakdown
        if (pref::get(pref::BoolPref::IlBattleShowBreakdown)) {
            current_y += CHEIGHT;
            if (!pref::get(pref::BoolPref::IlBattleMinimalBreakdown)) {
                draw::debug_text(X - 12 * CWIDTH, current_y, times, "BREAKDOWN:");
                draw::debug_text(X, current_y, times, "%d.%02d [%d]", best_score_seconds,
                                 best_score_centiseconds, s_best_score_bananas);
            } else {
                draw::debug_text(X - 12 * CWIDTH, current_y, draw::GOLD, "SCORE BREAKDOWN");
                current_y += CHEIGHT;
                draw::debug_text(X - 12 * CWIDTH, current_y, text, "BANANAS:");
                draw::debug_text(X, current_y, times, "%d", s_best_score_bananas);
                current_y += CHEIGHT;
                draw::debug_text(X - 12 * CWIDTH, current_y, text, "TIMER:");
                draw::debug_text(X, current_y, times, "%d.%02d", best_score_seconds,
                                 best_score_centiseconds);
            }
        }
    }

    return current_y + CHEIGHT;
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

static u32 convert_battle_length(u8 battle_length_choice) {
    if (battle_length_choice == 0) return 5 * 60 * 60;
    if (battle_length_choice == 1) return 7 * 60 * 60;
    if (battle_length_choice == 2) return 10 * 60 * 60;
    return 0;
}

void clear_display() {
    s_battle_frames = 0;
    s_best_frames = 0;
    s_best_score = 0;
    s_buzzer_message_count = 0;
    s_best_score_bananas = 0;
    s_best_score_frames = 0;
    s_battle_length = convert_battle_length(pref::get(pref::U8Pref::IlBattleLength));
}

void new_battle() {
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
    if (pref::get(pref::U8Pref::IlBattleLength) != 3) {  // If the timer isnt endless
        if (s_battle_frames < s_battle_length) {
            s_battle_frames++;
        } else
            s_state = IlBattleState::BuzzerBeater;
    } else
        s_battle_frames++;  // If the timer is endless
}

static void track_best() {
    u32 current_frames = mkb::mode_info.stage_time_frames_remaining;
    u32 current_score = mkb::balls[mkb::curr_player_idx].score;
    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT ||
        mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {              // Goal test
        if (current_frames > s_best_frames && !s_invalid_run) {  // New best time test, & invalid
                                                                 // pause test
            s_best_frames = current_frames;
        }
        if (score_calc(current_score) > s_best_score && !s_invalid_run) {  // New best score test,
                                                                           // &
                                                                           // invalid pause test
            s_best_score = score_calc(current_score);
            s_best_score_bananas = mkb::balls[mkb::curr_player_idx].banana_count;
            s_best_score_frames = current_frames;
        }
    }
}

static void track_invalid_pauses() {
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    if (mkb::mode_info.stage_time_frames_remaining == mkb::mode_info.stage_time_limit - 1) {
        s_invalid_run = false;
    }
    if (paused_now) {
        s_invalid_run = true;
    }
}

static void track_final_attempt() {
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    // Save time & end battle if: Goal
    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
        u32 pre_buzzer_time = s_best_frames;
        track_best();
        if (pre_buzzer_time < s_best_frames)
            s_state = IlBattleState::BattleDoneBuzzer;
        else
            s_state = IlBattleState::BattleDone;
    }
    // End battle if: Paused, Fallout, or Time Over,
    else if (paused_now || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
             mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN ||
             mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
             mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
        s_state = IlBattleState::BattleDone;
    }
}

static constexpr u8 LOW_COLOR = 0x42;   // 0x41
static constexpr u8 HIGH_COLOR = 0xf5;  // 0xf5

static mkb::GXColor get_color(int num) {
    int state = num / 180;
    int loc = num % 180;
    mkb::GXColor color = {LOW_COLOR, LOW_COLOR, LOW_COLOR, 0xff};
    switch (state) {
        case 0: {  // R-G^B
            color.r = HIGH_COLOR;
            color.g += loc;
            break;
        }
        case 1: {  // RvG-B
            color.r = HIGH_COLOR - loc;
            color.g = HIGH_COLOR;
            break;
        }
        case 2: {  // R G-B^
            color.g = HIGH_COLOR;
            color.b += loc;
            break;
        }
        case 3: {  // R GvB-
            color.g = HIGH_COLOR - loc;
            color.b = HIGH_COLOR;
            break;
        }
        case 4: {  // R^G B-
            color.r += loc;
            color.b = HIGH_COLOR;
            break;
        }
        case 5: {  // R-G Bv
            color.r = HIGH_COLOR;
            color.b = HIGH_COLOR - loc;
            break;
        }
    }

    return color;
}

static void display_buzzer_beater_message(u32 start_y) {
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

void tick() {
    s_rainbow = (s_rainbow + 11) % 1080;

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

    // possible incorrect ordering here for pause tracking?
    if (pref::get(pref::BoolPref::IlBattleDisplay)) {
        if (s_state == IlBattleState::WaitForFirstRetry) {
            track_first_retry();
        } else if (s_state == IlBattleState::BattleRunning) {
            track_invalid_pauses();
            track_best();
            run_battle_timer();  // To-Do: Implement realtime timer (without loads), if I figure how
            if (pad::button_down(mkb::PAD_BUTTON_UP)) {  // REMOVE
                s_battle_frames += 60 * 15;
            }
        } else if (s_state == IlBattleState::BuzzerBeater) {
            track_invalid_pauses();
            track_final_attempt();
        }

        // Reset display if menu when battle over
        if (mkb::main_mode != mkb::MD_GAME && (s_state == IlBattleState::BattleDoneBuzzer ||
                                               s_state == IlBattleState::WaitForFirstRetry)) {
            s_state = IlBattleState::BattleDone;
        }
        if (mkb::main_mode != mkb::MD_GAME && s_state == IlBattleState::BattleDone) {
            clear_display();
        }

        // Resets battles when Dpad Down is pressed
        if (pad::button_pressed(mkb::PAD_BUTTON_DOWN)) {
            new_battle();
        }
    }
}

void disp() {
    if (mkb::main_mode == mkb::MD_GAME &&
        (mkb::main_game_mode == mkb::STORY_MODE || mkb::main_game_mode == mkb::CHALLENGE_MODE)) {
        return;
    }

    if (pref::get(pref::BoolPref::IlBattleDisplay)) {
        if (s_state == IlBattleState::WaitForFirstRetry && mkb::main_mode == mkb::MD_GAME) {
            draw::debug_text(X - 12 * CWIDTH, Y, draw::GOLD, "READY");
            draw::debug_text(X - 12 * CWIDTH, Y + CHEIGHT, draw::GOLD, "(RETRY TO BEGIN)");
        } else if (s_state == IlBattleState::BattleRunning ||
                   s_state == IlBattleState::BuzzerBeater) {
            if (s_main_mode_play_timer > 0 && s_battle_stage_id != mkb::current_stage_id &&
                mkb::main_mode == mkb::MD_GAME) {
                s_invalid_run = true;
                draw::debug_text(X - 12 * CWIDTH, Y, draw::RED, "WRONG STAGE");
            } else {
                battle_display(draw::LIGHT_GREEN, draw::LIGHT_GREEN);
            }
        } else if ((s_state == IlBattleState::BattleDone ||
                    s_state == IlBattleState::BattleDoneBuzzer) &&
                   mkb::main_mode == mkb::MD_GAME) {
            if (s_battle_frames == 0) {
                draw::debug_text(X - 12 * CWIDTH, Y, draw::LIGHT_PURPLE, "NOT READY");
                draw::debug_text(X - 12 * CWIDTH, Y + CHEIGHT, draw::LIGHT_PURPLE,
                                 "(DPAD DOWN TO READY)");
            } else if (s_state == IlBattleState::BattleDoneBuzzer) {
                if (!pref::get(pref::BoolPref::IlBattleBuzzerOld)) {
                    battle_display(draw::LIGHT_PURPLE, get_color(s_rainbow));
                } else {
                    u32 start_y = battle_display(draw::LIGHT_PURPLE, draw::LIGHT_PURPLE);
                    display_buzzer_beater_message(start_y);
                }
            } else {
                battle_display(draw::LIGHT_PURPLE, draw::LIGHT_PURPLE);
            }
        }
    }
}
}  // Namespace ilbattle
