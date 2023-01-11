#include "ilbattle.h"
#include "utils/timerdisp.h"
#include "utils/draw.h"
#include "systems/pad.h"
#include "mkb/mkb.h"
#include "systems/pref.h"

namespace ilbattle {

enum class IlBattleState {
    WaitForFirstRetry, // Mod is enabled but first retry hasnt ocurred
    BattleRunning,     // Battle is running
    BuzzerBeater,      // Time has run out, but an attempt is still going
    BattleDone,        // Final attempt is over, show times in red
};

static IlBattleState s_state = IlBattleState::WaitForFirstRetry;

static constexpr s32 X = 160;
static constexpr s32 Y = 48;
static u32 battle_frames = 0;
static u32 best_frames = 0;
static u32 best_score = 0;
static constexpr u32 SECOND_FRAMES = 60;
static constexpr u32 MINUTE_FRAMES = SECOND_FRAMES * 60;
static constexpr u32 HOUR_FRAMES = MINUTE_FRAMES * 60;
static constexpr u32 CWIDTH = 12;
static constexpr u32 CHEIGHT = 16;
static bool invalid_pause = false;
static bool buzzer_beater_message = false;
static u32 buzzer_message_count = 0;
static u32 battle_length = 0;
// static u32 battle_length = pref::get_ilbattle_length();

static void battle_display(mkb::GXColor color) {
    u32 battle_minutes = battle_frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 battle_seconds = battle_frames % MINUTE_FRAMES / SECOND_FRAMES;

    u32 best_seconds = best_frames / SECOND_FRAMES;
    u32 best_centiseconds = (best_frames % SECOND_FRAMES) * 100 / 60;

    draw::debug_text(X-12*CWIDTH, Y, color, "ELAPSED:");
    draw::debug_text(X, Y, color, "%02d:%02d", battle_minutes, battle_seconds);
    draw::debug_text(X-12*CWIDTH, Y+CHEIGHT, color, "BEST TIME:");
    draw::debug_text(X, Y+CHEIGHT, color, "%d.%02d", best_seconds, best_centiseconds);
    draw::debug_text(X-12*CWIDTH, Y+2*CHEIGHT, color, "BEST SCORE:");
    draw::debug_text(X, Y+2*CHEIGHT, color, "%d", best_score);
}

static u32 score_calc(u32 score) {
    u32 igt_score = mkb::mode_info.stage_time_frames_remaining * 100 / 60; // Score from timer
    u32 goal_bonus = 0; // Blue goal (no bonus)
    if(mkb::mode_info.entered_goal_type == 1) goal_bonus = 10000; // Green goal bonus
    else if(mkb::mode_info.entered_goal_type == 2) goal_bonus = 20000; // Red goal bonus

    if(mkb::mode_info.stage_time_frames_remaining > mkb::mode_info.stage_time_limit / 2){ // With time bonus
        return score + (igt_score + goal_bonus)* 2;
    }
    else {
        return score + (igt_score + goal_bonus); // Without time bonus
    }
}

static u32 convert_battle_length(u8 battle_length_choice){
    if(battle_length_choice == 0) return 5*60*60;
    else if(battle_length_choice == 1) return 7*60*60;
    else if(battle_length_choice == 2) return 10*60*60;
    else return 0;
}

void new_battle(){
    s_state = IlBattleState::WaitForFirstRetry;
    battle_frames = 0;
    best_frames = 0;
    best_score = 0;
    buzzer_beater_message = false;
    buzzer_message_count = 0;
    battle_length = convert_battle_length(pref::get_il_battle_length());
}

static void track_first_retry(){
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    if(!paused_now && (mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)){
        new_battle();
        s_state = IlBattleState::BattleRunning;
    }
}

static void run_battle_timer(){
    if(pref::get_il_battle_length() != 3){ // If the timer isnt endless
        if(battle_frames < battle_length){
            battle_frames++;
        }
        else s_state = IlBattleState::BuzzerBeater;
    }
    else battle_frames++; // If the timer is endless
}

static void track_best() {
    u32 current_frames = mkb::mode_info.stage_time_frames_remaining;
    u32 current_score = mkb::balls[mkb::curr_player_idx].score;
    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN){ // Goal test
        if(current_frames > best_frames && !invalid_pause){ // New best time test, & invalid pause test
            best_frames = current_frames;
        }
        if(score_calc(current_score) > best_score && !invalid_pause){ // New best score test, & invalid pause test
            best_score = score_calc(current_score);
        }
    }
}

static void track_invalid_pauses() {
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    if (mkb::mode_info.stage_time_frames_remaining == mkb::mode_info.stage_time_limit - 1){
        invalid_pause = false;
    }
    if (paused_now) {
        invalid_pause = true;
    }
}

static void track_final_attempt(){
    bool paused_now = *reinterpret_cast<u32*>(0x805BC474) & 8;
    // Save time & end battle if: Goal
    if(mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN){
        u32 pre_buzzer_time = best_frames;
        track_best();
        if(pre_buzzer_time < best_frames) buzzer_beater_message = true;
        s_state = IlBattleState::BattleDone;
    }
    // End battle if: Paused, Fallout, or Time Over,
    else if(paused_now || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN
    || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN){
        s_state = IlBattleState::BattleDone;
    }
}

static void display_buzzer_beater_message(){
    buzzer_message_count++;
    if(buzzer_message_count >= 0) draw::debug_text(X-12*CWIDTH, Y+3*CHEIGHT, draw::RED, "EPIC BUZZER BEATER B)");
    if(buzzer_message_count >= 5) draw::debug_text(X-12*CWIDTH, Y+4*CHEIGHT, draw::ORANGE, "EPIC BUZZER BEATER B)");
    if(buzzer_message_count >= 10) draw::debug_text(X-12*CWIDTH, Y+5*CHEIGHT, draw::GOLD, "EPIC BUZZER BEATER B)");
    if(buzzer_message_count >= 15) draw::debug_text(X-12*CWIDTH, Y+6*CHEIGHT, draw::GREEN, "EPIC BUZZER BEATER B)");
    if(buzzer_message_count >= 20) draw::debug_text(X-12*CWIDTH, Y+7*CHEIGHT, draw::BLUE, "EPIC BUZZER BEATER B)");
    if(buzzer_message_count >= 25) draw::debug_text(X-12*CWIDTH, Y+8*CHEIGHT, draw::BRIGHT_PURPLE, "EPIC BUZZER BEATER B)");
    if(buzzer_message_count > 30) buzzer_message_count = 0;
}

void tick() {
    if(pref::get_il_battle_display()){
        if (s_state == IlBattleState::WaitForFirstRetry) {
            track_first_retry();
        } else if (s_state == IlBattleState::BattleRunning) {
            track_invalid_pauses();
            track_best();
            run_battle_timer(); // To-Do: Implement realtime timer (without loads), if I figure out how
        } else if (s_state == IlBattleState::BuzzerBeater) {
            track_invalid_pauses();
            track_final_attempt();
        } else if (s_state == IlBattleState::BattleDone) { // DELETE THIS IF THERE'S NOTHING HERE LATER
        }

        // THESE ARE FOR TESTING, REMOVE THEM PLEASE NAMBO
        if (pad::button_pressed(mkb::PAD_BUTTON_Y)) {
            new_battle();
        }
    }
}

void disp() {
    if (pref::get_il_battle_display()){
        if(s_state == IlBattleState::BattleDone){
            battle_display(draw::RED);
            if(buzzer_beater_message){
                display_buzzer_beater_message();
            }
        }
        else battle_display(draw::LIGHT_GREEN);
    }
}
} // Namespace ilbattle