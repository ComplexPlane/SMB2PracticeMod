#include "ilbattle.h"
#include "mkb/mkb.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/timerdisp.h"

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
static constexpr u32 SECOND_FRAMES = 60;
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;
static constexpr u32 CWIDTH = 12;
static constexpr u32 CHEIGHT = 16;
static bool s_invalid_pause = false;
static u32 s_buzzer_message_count = 0;
static u32 s_battle_length = 0;
// static u32 battle_length = pref::get_ilbattle_length();

static void battle_display(mkb::GXColor color) {
    u32 battle_minutes = s_battle_frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 battle_seconds = s_battle_frames % MINUTE_FRAMES / SECOND_FRAMES;

    u32 best_seconds = s_best_frames / SECOND_FRAMES;
    u32 best_centiseconds = (s_best_frames % SECOND_FRAMES) * 100 / 60;

    u32 best_score_seconds = s_best_score_frames / SECOND_FRAMES;
    u32 best_score_centiseconds = (s_best_score_frames % SECOND_FRAMES) * 100 / 60;

    draw::debug_text(X - 12 * CWIDTH, Y, color, "ELAPSED:");
    draw::debug_text(X, Y, color, "%02d:%02d", battle_minutes, battle_seconds); 
    draw::debug_text(X - 12 * CWIDTH, Y + CHEIGHT, color, "BEST TIME:");
    draw::debug_text(X, Y + CHEIGHT, color, "%d.%02d", best_seconds, best_centiseconds);
    draw::debug_text(X - 12 * CWIDTH, Y + 2 * CHEIGHT, color, "BEST SCORE:");
    draw::debug_text(X, Y + 2 * CHEIGHT, color, "%d", s_best_score);
    if(pref::get_il_battle_breakdown()){
        draw::debug_text(X - 12 * CWIDTH, Y + 3 * CHEIGHT, draw::GOLD, "SCORE BREAKDOWN");
        draw::debug_text(X - 12 * CWIDTH, Y + 4 * CHEIGHT, color, "BANANAS:");
        draw::debug_text(X, Y + 4 * CHEIGHT, color, "%d", s_best_score_bananas);
        draw::debug_text(X - 12 * CWIDTH, Y + 5 * CHEIGHT, color, "TIMER:");
        draw::debug_text(X, Y + 5 * CHEIGHT, color, "%d.%02d", best_score_seconds, best_score_centiseconds);
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

static u32 convert_battle_length(u8 battle_length_choice) {
    if (battle_length_choice == 0)
        return 5 * 60 * 60;
    if (battle_length_choice == 1)
        return 7 * 60 * 60;
    if (battle_length_choice == 2)
        return 10 * 60 * 60;
    return 0;
}

void clear_display() {
    s_battle_frames = 0;
    s_best_frames = 0;
    s_best_score = 0;
    s_buzzer_message_count = 0;
    s_best_score_bananas = 0;
    s_best_score_frames = 0;
    s_battle_length = convert_battle_length(pref::get_il_battle_length());
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
    }
}

static void run_battle_timer() {
    if (pref::get_il_battle_length() != 3) {  // If the timer isnt endless
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
        mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {                // Goal test
        if (current_frames > s_best_frames && !s_invalid_pause) {  // New best time test, & invalid
                                                                   // pause test
            s_best_frames = current_frames;
        }
        if (score_calc(current_score) > s_best_score && !s_invalid_pause) {  // New best score test,
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
        s_invalid_pause = false;
    }
    if (paused_now) {
        s_invalid_pause = true;
    }
}

static void track_final_attempt() {
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    // Save time & end battle if: Goal
    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
        u32 pre_buzzer_time = s_best_frames;
        track_best();
        if (pre_buzzer_time < s_best_frames) s_state = IlBattleState::BattleDoneBuzzer;
        else s_state = IlBattleState::BattleDone;
    }
    // End battle if: Paused, Fallout, or Time Over,
    else if (paused_now || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT ||
             mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN ||
             mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT ||
             mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
        s_state = IlBattleState::BattleDone;
    }
}

static void display_buzzer_beater_message() {
    s_buzzer_message_count++;
    u32 Y2 = Y;
    if(pref::get_il_battle_breakdown()) Y2 = Y + 3 * CHEIGHT;
    if (s_buzzer_message_count >= 0)
        draw::debug_text(X - 12 * CWIDTH, Y2 + 3 * CHEIGHT, draw::RED, "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 5)
        draw::debug_text(X - 12 * CWIDTH, Y2 + 4 * CHEIGHT, draw::ORANGE, "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 10)
        draw::debug_text(X - 12 * CWIDTH, Y2 + 5 * CHEIGHT, draw::GOLD, "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 15)
        draw::debug_text(X - 12 * CWIDTH, Y2 + 6 * CHEIGHT, draw::GREEN, "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 20)
        draw::debug_text(X - 12 * CWIDTH, Y2 + 7 * CHEIGHT, draw::BLUE, "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 25)
        draw::debug_text(X - 12 * CWIDTH, Y2 + 8 * CHEIGHT, draw::BRIGHT_PURPLE,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count > 30) s_buzzer_message_count = 0;
}

void tick() {
    if (pref::get_il_battle_display()) {
        if (s_state == IlBattleState::WaitForFirstRetry) {
            track_first_retry();
        } else if (s_state == IlBattleState::BattleRunning) {
            track_invalid_pauses();
            track_best();
            run_battle_timer();  // To-Do: Implement realtime timer (without loads), if I figure how
        } else if (s_state == IlBattleState::BuzzerBeater) {
            track_invalid_pauses();
            track_final_attempt();
        }

        // Reset display if menu when battle over
        if (mkb::main_mode != mkb::MD_GAME && (s_state == IlBattleState::BattleDoneBuzzer || s_state == IlBattleState::WaitForFirstRetry)){
            s_state = IlBattleState::BattleDone;
        } if (mkb::main_mode != mkb::MD_GAME && s_state == IlBattleState::BattleDone){
            clear_display();
        }

        // Resets battles when Dpad Down is pressed
        if (pad::button_pressed(mkb::PAD_BUTTON_DOWN)) {
            new_battle();
        }
    }
}

void disp() {
    if (pref::get_il_battle_display()) {
        if (s_state == IlBattleState::WaitForFirstRetry && mkb::main_mode == mkb::MD_GAME) {
            draw::debug_text(X - 12 * CWIDTH, Y, draw::GOLD, "READY");
            draw::debug_text(X - 12 * CWIDTH, Y + CHEIGHT, draw::GOLD, "(RETRY TO BEGIN)");
        } else if (s_state == IlBattleState::BattleRunning || s_state == IlBattleState::BuzzerBeater) {
            battle_display(draw::LIGHT_GREEN);
        } else if ((s_state == IlBattleState::BattleDone || s_state == IlBattleState::BattleDoneBuzzer) && mkb::main_mode == mkb::MD_GAME){
            if(s_battle_frames == 0){
                draw::debug_text(X - 12 * CWIDTH, Y, draw::LIGHT_PURPLE, "NOT READY");
                draw::debug_text(X - 12 * CWIDTH, Y + CHEIGHT, draw::LIGHT_PURPLE, "(DPAD DOWN TO READY)");
            } else battle_display(draw::LIGHT_PURPLE);
            if (s_state == IlBattleState::BattleDoneBuzzer) {
                display_buzzer_beater_message();
            }
        }
    }
}
}  // Namespace ilbattle