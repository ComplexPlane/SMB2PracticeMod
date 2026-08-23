#include "ilbattle.h"
#include "mkb/mkb.h"
#include "mods/freecam.h"
#include "mods/validate.h"
#include "systems/binds.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "systems/savest.h"
#include "systems/textinfo.h"
#include "utils/draw.h"
#include "utils/macro_utils.h"
#include "utils/patch.h"
#include "utils/relutil.h"
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
static constexpr u32 CWIDTH = draw::DEBUG_CHAR_WIDTH;
static constexpr u32 CHEIGHT = 16;
static constexpr s32 TEXT_POS_X = X - 12 * CWIDTH;
// buzzer beater constants
static constexpr u16 BUZZER_BEATER_FRAMES_PER_COLOR = 5;
static const GXColor s_buzzer_beater_color_list[] = {
    draw::RED, draw::ORANGE, draw::GOLD, draw::GREEN, draw::BLUE, draw::BRIGHT_PURPLE};
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

using Mod = textinfo::Module;
using Slot = textinfo::Slot;
using Format = timerdisp::TimeFormat;

void draw_battle_text_main(s32 pos_x, GXColor color, bool incr_row, char *text) {
    textinfo::draw(Mod::IlBattle, Slot::Left, pos_x, color, incr_row, text);
}

void draw_buzzer_beater_row(GXColor color) {
    draw_battle_text_main(TEXT_POS_X, color, true, "EPIC BUZZER BEATER B)");
}

static void old_buzzer_display(u32 start_y) {
    constexpr u16 COLOR_COUNT = LEN(s_buzzer_beater_color_list);
    constexpr u16 TOTAL_FRAMES = COLOR_COUNT * BUZZER_BEATER_FRAMES_PER_COLOR;

    s_buzzer_message_count = (s_buzzer_message_count + 1) % TOTAL_FRAMES;
    for (u16 col = 0; col < COLOR_COUNT; col++) {
        if (s_buzzer_message_count >= col * BUZZER_BEATER_FRAMES_PER_COLOR) {
            draw_buzzer_beater_row(s_buzzer_beater_color_list[col]);
        }
    }
}

void draw_battle_text(GXColor color, char *text) {
    draw_battle_text_main(TEXT_POS_X, color, false, text);
}

void draw_battle_breakdown_text(GXColor color, char *text) {
    draw_battle_text_main(TEXT_POS_X + 3 * CWIDTH, color, false, text);
}

// When we only want to display a single time or number (like attempt count)
// Special formatting like "time [bananas]"" is handled on a per-case basis, however
void draw_battle_num(GXColor color, u32 num, Format format) {
    // No prefix specified since IL Battle uses custom spacing between its prefixes and times
    textinfo::draw_timer_main(Mod::IlBattle, Slot::Left, color, "", num, format);
}

// pre cleanup
static void battle_display(GXColor text_color) {
    u32 current_y = Y;

    GXColor time_color = s_time_buzzer ? draw::num_to_rainbow(s_rainbow) : text_color;
    GXColor score_color = s_score_buzzer ? draw::num_to_rainbow(s_rainbow) : text_color;

    draw_battle_text(text_color, "ELAPSED:");
    draw_battle_num(text_color, s_battle_frames, Format::MinutesAlwaysLeadingZero);

    if (pref::get(pref::Pref::IlBattleShowTime)) {
        draw_battle_text(text_color, "BEST TIME:");

        if (pref::get(pref::Pref::IlBattleTieCount) && s_best_frames_ties > 0) {
            char time_buf[16] = {};
            timerdisp::format_time(time_buf, s_best_frames, Format::SecondsOnly);
            textinfo::draw_aligned(Mod::IlBattle, Slot::Left, time_color, "%s (%d)", time_buf,
                                   s_best_frames_ties + 1);
        } else {
            draw_battle_num(text_color, s_best_frames, Format::SecondsOnly);
        }
    }
    if (pref::get(pref::Pref::IlBattleShowScore)) {
        draw_battle_text(text_color, "BEST SCORE:");
        if (pref::get(pref::Pref::IlBattleTieCount) && s_best_score_ties > 0) {
            textinfo::draw_aligned(Mod::IlBattle, Slot::Left, score_color, "%d (%d)", s_best_score,
                                   s_best_score_ties + 1);
        } else {
            draw_battle_num(score_color, s_best_score, Format::Unformatted);
        }

        // breakdown
        u8 breakdown_value = pref::get(pref::Pref::IlBattleBreakdown);
        if (breakdown_value == 1) {
            // minimal
            draw_battle_text(text_color, "BREAKDOWN:");
            char time_buf[16] = {};
            timerdisp::format_time(time_buf, s_best_score_frames, Format::SecondsOnly);
            textinfo::draw_aligned(Mod::IlBattle, Slot::Left, text_color, "%s [%d]", time_buf,
                                   s_best_score_bananas);
        } else if (breakdown_value == 2) {
            // full
            draw_battle_breakdown_text(text_color, "BANANAS:");
            textinfo::draw_aligned(Mod::IlBattle, Slot::Left, text_color, "%d",
                                   s_best_score_bananas);
            draw_battle_breakdown_text(text_color, "TIMER:");
            draw_battle_num(text_color, s_best_score_frames, Format::SecondsOnly);
        }
    }
    if (pref::get(pref::Pref::IlBattleAttemptCount)) {
        draw_battle_text(text_color, "ATTEMPTS:");
        draw_battle_num(text_color, s_attempts, Format::Unformatted);
    }

    if (pref::get(pref::Pref::IlBattleBuzzerOld) &&
        ((s_time_buzzer && pref::get(pref::Pref::IlBattleShowTime)) ||
         (s_score_buzzer && pref::get(pref::Pref::IlBattleShowScore)))) {
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
    s_battle_length = convert_battle_length(IlBattleLength(pref::get(pref::Pref::IlBattleLength)));
}

static void new_battle() {
    clear_display();
    s_state = IlBattleState::WaitForFirstRetry;
}

static void track_first_retry() {
    bool paused_now = *reinterpret_cast<u32 *>(relutil::relocate_addr(0x805BC474)) & 8;
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
    if (IlBattleLength(pref::get(pref::Pref::IlBattleLength)) == IlBattleLength::Endless) {
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
    bool paused_now = *reinterpret_cast<u32 *>(relutil::relocate_addr(0x805BC474)) & 8;
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
    if (!pref::get(pref::Pref::IlBattleDisplay)) {
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
        binds::bind_pressed(pref::get(pref::Pref::IlBattleReadyBind))) {
        new_battle();
    }
}

void disp() {
    if (mkb::main_mode == mkb::MD_GAME &&
        (mkb::main_game_mode == mkb::STORY_MODE || mkb::main_game_mode == mkb::CHALLENGE_MODE)) {
        return;
    }

    if (!pref::get(pref::Pref::IlBattleDisplay) || freecam::should_hide_hud()) return;

    switch (s_state) {
        case IlBattleState::NotReady: {
            if (mkb::main_mode != mkb::MD_GAME) return;
            u8 input = pref::get(pref::Pref::IlBattleReadyBind);
            char buf[25];
            binds::get_bind_str(input, buf);
            draw_battle_text_main(TEXT_POS_X, draw::LIGHT_PURPLE, true, "NOT READY");
            textinfo::draw(Mod::IlBattle, Slot::Left, TEXT_POS_X, draw::LIGHT_PURPLE, true,
                           "%s to ready", buf);
            break;
        }
        case IlBattleState::WaitForFirstRetry: {
            if (mkb::main_mode == mkb::MD_GAME) {
                draw_battle_text_main(TEXT_POS_X, draw::GOLD, true, "READY");
                draw_battle_text_main(TEXT_POS_X, draw::GOLD, true, "Retry to begin");
            }
            break;
        }
        case IlBattleState::BattleRunning:
        case IlBattleState::BuzzerBeater: {
            if (s_main_mode_play_timer > 0 && s_battle_stage_id != mkb::current_stage_id &&
                mkb::main_mode == mkb::MD_GAME) {
                draw_battle_text_main(TEXT_POS_X, draw::RED, true, "WRONG STAGE");
            } else {
                battle_display(draw::LIGHT_GREEN);
            }
            break;
        }
        case IlBattleState::BuzzerBeaterPostgoal: {
            if (pref::get(pref::Pref::IlBattleShowScore)) {
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
