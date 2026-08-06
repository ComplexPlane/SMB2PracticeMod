#include "mods/ilbattle.h"
#include "utils/base.h"
#include "systems/binds.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/libsavest.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"


typedef enum IlBattleState IlBattleState;
enum IlBattleState {
    IlBattleState_NotReady,
    IlBattleState_WaitForFirstRetry,     // Mod is enabled but first retry hasn't occurred
    IlBattleState_BattleRunning,         // Battle is running
    IlBattleState_BuzzerBeater,          // Time has run out, but an attempt is still going
    IlBattleState_BuzzerBeaterPostgoal,  // Final attempt is over, wait for postgoals for score
    IlBattleState_BattleDone,            // Final attempt is over, show times in blue
};

typedef enum IlBattleLength IlBattleLength;
enum IlBattleLength {
    IlBattleLength_FiveMinutes = 0,
    IlBattleLength_SevenMinutes = 1,
    IlBattleLength_TenMinutes = 2,
    IlBattleLength_Endless = 3,
};

// main state
static IlBattleState s_state = IlBattleState_NotReady;
// ui constants
static const s32 X = 160;
static const s32 Y = 48;
static const u32 CWIDTH = 12;
static const u32 CHEIGHT = 16;
// time constants
static const u32 SECOND_FRAMES = 60;                  // frames per second
static const u32 MINUTE_FRAMES = SECOND_FRAMES * 60;  // frames per minute
static const u32 HOUR_FRAMES = MINUTE_FRAMES * 60;    // frames per hour
// battle trackers
static u32 s_battle_frames = 0;
static bool s_valid_run = false;
static s16 s_paused_frame = 0;
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
        draw_debug_text(X - 12 * CWIDTH, start_y + 1 * CHEIGHT, COLOR_RED,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 5)
        draw_debug_text(X - 12 * CWIDTH, start_y + 2 * CHEIGHT, COLOR_ORANGE,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 10)
        draw_debug_text(X - 12 * CWIDTH, start_y + 3 * CHEIGHT, COLOR_GOLD,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 15)
        draw_debug_text(X - 12 * CWIDTH, start_y + 4 * CHEIGHT, COLOR_GREEN,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 20)
        draw_debug_text(X - 12 * CWIDTH, start_y + 5 * CHEIGHT, COLOR_BLUE,
                         "EPIC BUZZER BEATER B)");
    if (s_buzzer_message_count >= 25)
        draw_debug_text(X - 12 * CWIDTH, start_y + 6 * CHEIGHT, COLOR_BRIGHT_PURPLE,
                         "EPIC BUZZER BEATER B)");
}

static void battle_display(mkb_GXColor text_color) {
    u32 battle_hours = s_battle_frames / HOUR_FRAMES;
    u32 battle_minutes = s_battle_frames % HOUR_FRAMES / MINUTE_FRAMES;
    u32 battle_seconds = s_battle_frames % MINUTE_FRAMES / SECOND_FRAMES;

    u32 best_seconds = s_best_frames / SECOND_FRAMES;
    u32 best_centiseconds = (s_best_frames % SECOND_FRAMES) * 100 / 60;

    u32 best_score_seconds = s_best_score_frames / SECOND_FRAMES;
    u32 best_score_centiseconds = (s_best_score_frames % SECOND_FRAMES) * 100 / 60;

    u32 current_y = Y;

    mkb_GXColor time_color = s_time_buzzer ? draw_num_to_rainbow(s_rainbow) : text_color;
    mkb_GXColor score_color = s_score_buzzer ? draw_num_to_rainbow(s_rainbow) : text_color;

    draw_debug_text(X - 12 * CWIDTH, Y, text_color, "ELAPSED:");
    if (battle_hours > 0) {
        draw_debug_text(X - 6, Y, text_color, "%d:%02d:%02d", battle_hours, battle_minutes,
                         battle_seconds);
    } else {
        draw_debug_text(X - 6, Y, text_color, "%02d:%02d", battle_minutes, battle_seconds);
    }

    if (pref_get(Pref_IlBattleShowTime)) {
        current_y += CHEIGHT;
        draw_debug_text(X - 12 * CWIDTH, current_y, text_color, "BEST TIME:");
        if (pref_get(Pref_IlBattleTieCount) && s_best_frames_ties > 0) {
            draw_debug_text(X - 6, current_y, time_color, "%d.%02d (%d)", best_seconds,
                             best_centiseconds, s_best_frames_ties + 1);
        } else {
            draw_debug_text(X - 6, current_y, time_color, "%d.%02d", best_seconds,
                             best_centiseconds);
        }
    }
    if (pref_get(Pref_IlBattleShowScore)) {
        current_y += CHEIGHT;
        draw_debug_text(X - 12 * CWIDTH, current_y, text_color, "BEST SCORE:");
        if (pref_get(Pref_IlBattleTieCount) && s_best_score_ties > 0) {
            draw_debug_text(X - 6, current_y, score_color, "%d (%d)", s_best_score,
                             s_best_score_ties + 1);
        } else {
            draw_debug_text(X - 6, current_y, score_color, "%d", s_best_score);
        }

        // breakdown
        u8 breakdown_value = pref_get(Pref_IlBattleBreakdown);
        if (breakdown_value == 1) {
            // minimal
            current_y += CHEIGHT;
            draw_debug_text(X - 12 * CWIDTH, current_y, text_color, "BREAKDOWN:");
            draw_debug_text(X - 6, current_y, text_color, "%d.%02d [%d]", best_score_seconds,
                             best_score_centiseconds, s_best_score_bananas);
        } else if (breakdown_value == 2) {
            // full
            current_y += CHEIGHT;
            draw_debug_text(X - 12 * CWIDTH, current_y, text_color, "  BANANAS:");
            draw_debug_text(X - 6, current_y, text_color, "%d", s_best_score_bananas);
            current_y += CHEIGHT;
            draw_debug_text(X - 12 * CWIDTH, current_y, text_color, "  TIMER:");
            draw_debug_text(X - 6, current_y, text_color, "%d.%02d", best_score_seconds,
                             best_score_centiseconds);
        }
    }
    if (pref_get(Pref_IlBattleAttemptCount)) {
        current_y += CHEIGHT;
        draw_debug_text(X - 12 * CWIDTH, current_y, text_color, "ATTEMPTS:");
        draw_debug_text(X - 6, current_y, text_color, "%d", s_attempts);
    }

    if (pref_get(Pref_IlBattleBuzzerOld) &&
        ((s_time_buzzer && pref_get(Pref_IlBattleShowTime)) ||
         (s_score_buzzer && pref_get(Pref_IlBattleShowScore)))) {
        old_buzzer_display(current_y + CHEIGHT);
    }
}

static u32 score_calc(u32 score) {
    u32 igt_score = mkb_mode_info.stage_time_frames_remaining * 100 / 60;  // Score from timer
    u32 goal_bonus = 0;                                                     // Blue goal (no bonus)
    if (mkb_mode_info.entered_goal_type == 1) {
        goal_bonus = 10000;  // Green goal bonus
    } else if (mkb_mode_info.entered_goal_type == 2) {
        goal_bonus = 20000;  // Red goal bonus
    }
    if (mkb_mode_info.stage_time_frames_remaining >
        mkb_mode_info.stage_time_limit / 2) {  // With time bonus
        return score + (igt_score + goal_bonus) * 2;
    }
    return score + (igt_score + goal_bonus);  // Without time bonus
}

static u32 convert_battle_length(IlBattleLength battle_length_choice) {
    if (battle_length_choice == IlBattleLength_FiveMinutes) return 5 * 60 * 60;
    if (battle_length_choice == IlBattleLength_SevenMinutes) return 7 * 60 * 60;
    if (battle_length_choice == IlBattleLength_TenMinutes) return 10 * 60 * 60;
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
        convert_battle_length((IlBattleLength)(pref_get(Pref_IlBattleLength)));
}

void ilbattle_new_battle() {
    clear_display();
    s_state = IlBattleState_WaitForFirstRetry;
}

static void track_first_retry() {
    bool paused_now = *(u32*)(0x805BC474) & 8;
    if (!paused_now && mkb_sub_mode == mkb_SMD_GAME_READY_INIT) {
        ilbattle_new_battle();
        s_state = IlBattleState_BattleRunning;
        s_battle_stage_id = mkb_current_stage_id;
    }
}

static void run_battle_timer() {
    if ((IlBattleLength)(pref_get(Pref_IlBattleLength)) == IlBattleLength_Endless) {
        // timer is endless
        s_battle_frames++;
    } else if (s_battle_frames < s_battle_length) {
        s_battle_frames++;
    } else {
        s_state = IlBattleState_BuzzerBeater;
    }
}

static void track_best() {
    if (mkb_sub_mode != mkb_SMD_GAME_GOAL_INIT) return;

    s16 current_frames = mkb_mode_info.stage_time_frames_remaining;
    u32 current_score = mkb_balls[mkb_curr_player_idx].score;
    bool on_incorrect_stage = s_main_mode_play_timer > 0 &&
                              s_battle_stage_id != mkb_current_stage_id &&
                              mkb_main_mode == mkb_MD_GAME;
    bool valid = (s_valid_run && s_paused_frame <= current_frames) ||
                 s_paused_frame == mkb_mode_info.stage_time_frames_remaining;
    if (!valid || on_incorrect_stage) return;

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
        s_best_score_bananas = mkb_balls[mkb_curr_player_idx].banana_count;
        s_best_score_frames = current_frames;
    }
}

static void track_invalid_pauses() {
    bool paused_now = *(u32*)(0x805BC474) & 8;
    if (mkb_sub_mode == mkb_SMD_GAME_PLAY_INIT) {
        s_valid_run = true;
        s_paused_frame = 0;       // attempt is now valid
        if (!s_accepted_retry) {  // track attempt counts
            s_attempts++;
            s_accepted_retry = true;
        }
    }
    if (mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN && paused_now && s_paused_frame == 0) {
        s_paused_frame = mkb_mode_info.stage_time_frames_remaining;
    } else if ((mkb_sub_mode == mkb_SMD_GAME_PLAY_MAIN && paused_now) ||
               savest_was_state_loaded_this_frame()) {
        s_valid_run = false;
    }
}

static void track_final_attempt() {
    bool paused_now = *(u32*)(0x805BC474) & 8;
    // End battle if: Paused, Fallout, or Time Over
    if (paused_now || mkb_sub_mode == mkb_SMD_GAME_RINGOUT_INIT ||
        mkb_sub_mode == mkb_SMD_GAME_RINGOUT_MAIN ||
        mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_INIT ||
        mkb_sub_mode == mkb_SMD_GAME_TIMEOVER_MAIN) {
        s_state = IlBattleState_BattleDone;
    }
    // Player has entered goal...
    // Save time & enter postgoal phase for score
    else if (mkb_sub_mode == mkb_SMD_GAME_GOAL_INIT || mkb_sub_mode == mkb_SMD_GAME_GOAL_MAIN) {
        s16 pre_buzzer_time = s_best_frames;
        u32 pre_buzzer_score = s_best_score;
        track_best();
        s_state = IlBattleState_BuzzerBeaterPostgoal;
        // time buzzer beater
        if (pre_buzzer_time < s_best_frames) {
            s_time_buzzer = true;
        }
        // score buzzer beater
        if (pre_buzzer_score < s_best_score) {
            s_score_buzzer = true;
        }

    }
    // Game is no longer in goal phase...
    // Save score and end battle
    else if (s_state == IlBattleState_BuzzerBeaterPostgoal &&
             !(mkb_sub_mode == mkb_SMD_GAME_GOAL_INIT ||
               mkb_sub_mode == mkb_SMD_GAME_GOAL_MAIN)) {
        u32 pre_buzzer_score = s_best_score;
        track_best();
        s_state = IlBattleState_BattleDone;
        // score buzzer beater (track again if there are post goals)
        if (pre_buzzer_score < s_best_score) {
            s_score_buzzer = true;
        }
    }
}

void ilbattle_tick() {
    if (!pref_get(Pref_IlBattleDisplay)) {
        clear_display();
        s_state = IlBattleState_NotReady;
    }

    if (mkb_sub_mode != mkb_SMD_GAME_PLAY_INIT) {
        s_accepted_retry = false;
    }

    if (mkb_sub_mode != mkb_SMD_GAME_GOAL_INIT) {
        s_accepted_tie = false;
    }

    if (mkb_main_mode == mkb_MD_GAME) {
        s_main_mode_play_timer++;
    } else {
        s_main_mode_play_timer = 0;
    }

    if (mkb_main_mode == mkb_MD_GAME &&
        (mkb_main_game_mode == mkb_STORY_MODE || mkb_main_game_mode == mkb_CHALLENGE_MODE)) {
        ilbattle_new_battle();
        return;
    }

    s_rainbow = (s_rainbow + 11) % 1080;

    switch (s_state) {
        case IlBattleState_WaitForFirstRetry: {
            track_first_retry();
            break;
        }
        case IlBattleState_BattleRunning: {
            track_invalid_pauses();
            track_best();
            run_battle_timer();  // TODO: Implement realtime timer (without loads), if I figure how
            break;
        }
        case IlBattleState_BuzzerBeater: {
            track_invalid_pauses();
            track_final_attempt();
            break;
        }
        case IlBattleState_BuzzerBeaterPostgoal: {
            track_invalid_pauses();
            track_final_attempt();
            break;
        }
        default: {
        }
    }

    // Reset display if menu when battle over
    if (mkb_main_mode != mkb_MD_GAME &&
        (s_state == IlBattleState_WaitForFirstRetry || s_state == IlBattleState_BattleDone)) {
        s_state = IlBattleState_NotReady;
        clear_display();
    }

    // Resets battles when Dpad Down is pressed
    if (mkb_main_mode == mkb_MD_GAME &&
        binds_bind_pressed(pref_get(Pref_IlBattleReadyBind), false)) {
        ilbattle_new_battle();
    }
}

void ilbattle_disp() {
    if (mkb_main_mode == mkb_MD_GAME &&
        (mkb_main_game_mode == mkb_STORY_MODE || mkb_main_game_mode == mkb_CHALLENGE_MODE)) {
        return;
    }

    if (!pref_get(Pref_IlBattleDisplay)) return;

    switch (s_state) {
        case IlBattleState_NotReady: {
            if (mkb_main_mode != mkb_MD_GAME) return;
            u8 input = pref_get(Pref_IlBattleReadyBind);
            char buf[25];
            binds_get_bind_str(input, buf);
            draw_debug_text(X - 12 * CWIDTH, Y, COLOR_LIGHT_PURPLE, "NOT READY");
            draw_debug_text(X - 12 * CWIDTH, Y + CHEIGHT, COLOR_LIGHT_PURPLE, "%s to ready", buf);
            break;
        }
        case IlBattleState_WaitForFirstRetry: {
            if (mkb_main_mode == mkb_MD_GAME) {
                draw_debug_text(X - 12 * CWIDTH, Y, COLOR_GOLD, "READY");
                draw_debug_text(X - 12 * CWIDTH, Y + CHEIGHT, COLOR_GOLD, "Retry to begin");
            }
            break;
        }
        case IlBattleState_BattleRunning:
        case IlBattleState_BuzzerBeater: {
            if (s_main_mode_play_timer > 0 && s_battle_stage_id != mkb_current_stage_id &&
                mkb_main_mode == mkb_MD_GAME) {
                draw_debug_text(X - 12 * CWIDTH, Y, COLOR_RED, "WRONG STAGE");
            } else {
                battle_display(COLOR_LIGHT_GREEN);
            }
            break;
        }
        case IlBattleState_BuzzerBeaterPostgoal: {
            if (pref_get(Pref_IlBattleShowScore)) {
                battle_display(COLOR_LIGHT_GREEN);
            } else {
                battle_display(COLOR_LIGHT_PURPLE);
            }
            break;
        }
        case IlBattleState_BattleDone: {
            battle_display(COLOR_LIGHT_PURPLE);
            break;
        }
    }
}
