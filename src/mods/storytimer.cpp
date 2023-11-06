#include "iw.h"

#include "mkb/mkb.h"

#include "mods/freecam.h"
#include "systems/assembly.h"
#include "systems/pad.h"
#include "systems/pref.h"
#include "utils/draw.h"
#include "utils/patch.h"
#include "utils/timerdisp.h"

namespace storytimer {

    enum class FullgameTimerOptions {
        F_DontShow = 0,
        F_AlwaysShow = 1,
        F_BetweenWorlds = 2,
        F_EndOfRun = 3,
    };
    
    enum class SegmentTimerOptions {
        S_DontShow = 0,
        S_AlwaysShow = 1,
        S_BetweenWorlds = 2,
        S_EndOfRun = 3,
    };

static u32 s_loadless_iw_time;
static u32 s_spin_in_timer;
static u32 s_gameplay_timer;
static u32 s_postgoal_timer;
static u32 s_postgoal_replay_timer;
static u32 s_exit_game_timer;
static u32 s_fallout_timer;
static u32 s_stage_select_timer;
static u32 s_loadless_story_timer;
static bool s_run_timer;
static bool s_is_on_spin_in;
static bool s_is_on_stage_select_screen; 
static bool s_is_on_gameplay;
static bool s_is_on_exit_game_screen;
static bool s_is_on_fallout_screen;
static bool s_is_postgoal;
static bool s_can_increment_stage_counter;
static bool s_lower_stage_counter;
static bool s_in_story;
static u32 s_prev_completed_stage_count;
static s32 s_completed_stages;
static u32 s_dummy;
static u32 s_dummy_2;
static u32 s_dummy_3;
static u32 s_split[11];    // s_split[k] is the loadless time on tape break of the 10th stage of world k
static u32 s_segment_time[11];  // s_segment_time[k] is the loadless time spent on world k, starting from stage select spin in to tape break on the 10th stage
static u32 s_segment_start_time[11];  // the loadless time at the start of world k, used to calculate s_segment_time[k]
static bool s_display_story_timer;
static bool s_display_segment_timer;
static bool s_display_split;
static bool s_display_world_time;
static bool s_display_segment;
static bool s_is_between_worlds;
static bool s_passed_cutscene;
static bool s_is_run_complete;
static u32 s_segment_timer[11];
static bool s_is_on_world[11]; 
static bool s_can_change_segment_start_time[11];
static u32 s_fullgame_timer_location_y;
static u32 s_segment_timer_location_y;
static u32 s_spin_in_init_timer;
static bool s_stage_selected_state;
static bool s_can_increment_spin_in_init_timer;
static bool s_has_incremented_spin_in_init_timer;
static u32 s_previous_spin_in_init_timer;
static u32 s_game_scenario_return_timer;
static u32 s_correction_timer;
static u32 s_corrected_stage_select_timer;
static bool s_is_on_world_first_intro_sequence[11];
static bool s_has_entered_world[11];
static bool s_has_corrected_stage_select_timer[11];
static bool s_is_on_first_frame_of_world[11];
static bool s_has_passed_first_frame_of_world[11];
static bool s_can_detect_first_frame_of_world[11];
static bool s_has_done_start_world_correction[11];
static constexpr s32 fullgame_timer_location_x = 18+24;
static constexpr s32 fullgame_timer_text_offset = 56;
static constexpr s32 segment_timer_location_x = 30+24;
static constexpr s32 segment_timer_text_offset = 44;
static constexpr s32 IW_time_location_x = 42+24; 
static constexpr s32 IW_time_text_offset = 32;
static constexpr s32 Y=24;

void tick() {
    if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE){
        s_run_timer = true;
    }
    
    /*
    if (mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN){
        // can increment counter after going through the goal
        s_can_increment_stage_counter = true;
    } else if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECTED || mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT){
        // but if you go back to the stage select screen without entering the goal or from an exit game, don't increment the counter
        s_can_increment_stage_counter = false;
    } 

    if (s_can_increment_stage_counter == true && mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN){
        s_completed_stages += 1;
    }
    */

   // there is likely a simpler way to increment the stage counter in a way that works when you retry after breaking the tape and also does not break if you pause during 
   // game goal init or game goal main

   if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
        s_prev_completed_stage_count = s_completed_stages; 
    }

   if (mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
        // not allowed to increment the stage counter on goal init because of first framing
        s_can_increment_stage_counter = true;
    }

    if ( (s_completed_stages - s_prev_completed_stage_count) > 0 ) {
        // if we've incremented the stage counter already, we're not allowed to increment again until the next game goal main
        s_can_increment_stage_counter = false;
        // however, if you retry after breaking the tape, lower the counter
        if (mkb::sub_mode==mkb::SMD_GAME_READY_INIT) {
            s_lower_stage_counter = true;
        } else if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN) {
            s_lower_stage_counter = false;
            s_prev_completed_stage_count = s_completed_stages; 
        }
    }

    if (s_can_increment_stage_counter == true) {
        s_completed_stages += 1; 
    }

    if (s_lower_stage_counter == true && ((s_completed_stages - s_prev_completed_stage_count) > 0 )) {
        s_completed_stages += -1;
        s_lower_stage_counter = false;
    }

    for (s32 k=1; k<11; k++) {
        /*
        if ( s_completed_stages == 10*(k-1) && mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN && s_has_done_start_world_correction[k] == false && k != 1) {
            s_corrected_stage_select_timer += 1;
            s_has_done_start_world_correction[k] == true;
        }
        */
       if (s_is_on_world[k] == true) {
        s_corrected_stage_select_timer = 2*k;
       }
    }
   /*
   if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
    s_completed_stages +=1;
   } 
    */

   /*
    // trying to detect the missing frame during spin   
    if (mkb::g_storymode_mode == 5) {
        s_can_increment_spin_in_init_timer = false;
    }

    if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECTED) {
        s_stage_selected_state = true; 
        s_previous_spin_in_init_timer = s_spin_in_init_timer;
    } else {
        false;
    }

    if (s_stage_selected_state == false ) {
        s_can_increment_spin_in_init_timer = true;
    }

    if (s_can_increment_spin_in_init_timer == true) {
        s_spin_in_init_timer +=1; 
    }

    if ( (s_spin_in_init_timer - s_previous_spin_in_init_timer) >0 ) {
        s_can_increment_spin_in_init_timer = false;
    }
    */

   // no idea why the spin_in_init_timer only properly incremenents when it's not in the if (s_run_timer == true){ statement below (I checked and s_run_timer always remains true during spin in)
   // this is very jank I hate it so much
   if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT) {
        s_spin_in_init_timer += 1;
    }

   if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN) {
        s_game_scenario_return_timer += 1;
    }
   
   s_correction_timer = s_spin_in_init_timer + s_game_scenario_return_timer+s_corrected_stage_select_timer;

    // submodes during spin in
    if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT || mkb::sub_mode==mkb::SMD_GAME_READY_INIT || mkb::sub_mode==mkb::SMD_GAME_READY_MAIN) {
        s_is_on_spin_in = true;
    } else {
        s_is_on_spin_in = false;
    }

    // states entered during the story select screen
    if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE || mkb::g_storymode_stageselect_state == 3 || 
        mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_IDLE || mkb::g_storymode_stageselect_state == 5 || mkb::g_storymode_stageselect_state == mkb::STAGE_SELECTED) {
        // 3, 5 unlabelled inits
        s_is_on_stage_select_screen = true;
        } else {
            s_is_on_stage_select_screen = false;
        }

    // submodes during gameplay
    if (mkb::sub_mode==mkb::SMD_GAME_PLAY_INIT || mkb::sub_mode==mkb::SMD_GAME_PLAY_MAIN) {
        s_is_on_gameplay = true;
    } else {
        s_is_on_gameplay = false;
    }

    // submodes entered when exiting game
    if (mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_INIT || mkb::sub_mode == mkb::SMD_GAME_INTR_SEL_MAIN 
    || mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_INIT || mkb::sub_mode == mkb::SMD_GAME_SUGG_SAVE_MAIN) {
        s_is_on_exit_game_screen = true;
    } else{
        s_is_on_exit_game_screen = false;
    }

    // submodes entered when breaking the goal tape
    if (mkb::sub_mode==mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode==mkb::SMD_GAME_GOAL_MAIN || 
    mkb::sub_mode==mkb::SMD_GAME_GOAL_REPLAY_INIT || mkb::sub_mode==mkb::SMD_GAME_GOAL_REPLAY_MAIN || mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN) {
        s_is_postgoal = true;
    } else {
        s_is_postgoal = false;
    }

    // submodes for the fallout and y/n screen
    if (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN 
    || mkb::sub_mode == mkb::SMD_GAME_RETRY_INIT || mkb::sub_mode == mkb::SMD_GAME_RETRY_MAIN) {
        s_is_on_fallout_screen = true;
    } else {
        s_is_on_fallout_screen = false; 
    }

    if (mkb::g_storymode_mode == 5){
        // zero the timer on the file select screen and set the number of completed stages to 0
        s_spin_in_init_timer = 0;
        s_spin_in_timer = 0 ;
        s_gameplay_timer = 0;
        s_postgoal_timer = 0;
        // s_postgoal_replay_timer = 0;
        s_stage_select_timer = 0;
        s_game_scenario_return_timer = 0;
        s_exit_game_timer = 0;
        s_fallout_timer = 0;
        s_run_timer = false;
        s_lower_stage_counter = false;
        s_loadless_story_timer = 0;
        s_completed_stages = 0;
        s_prev_completed_stage_count = 0;
        s_corrected_stage_select_timer = 0;
        for (s32 k=1; k<11; k++) {
            s_has_done_start_world_correction[k] = false;
        }
        } else{
            // trying to figure out missing frames
            // for some reason, separately including s_spin_in_init_timer doesn't cause the visual frame skip in the timer
            // last 3 terms are correction terms to account for missing frames
            s_loadless_story_timer = s_spin_in_timer+s_gameplay_timer+s_postgoal_timer+s_stage_select_timer+s_exit_game_timer+s_fallout_timer
            +s_spin_in_init_timer+s_game_scenario_return_timer+s_corrected_stage_select_timer;
        }
        // on the file select screen, set these to false just in case you reset while on world k but did not complete 10k stages
        // need to stipulate s_run_timer is false otherwise there's weird behavior on the first stage of e/ world
        for (s32 k=1; k<11; k++){
            if (s_run_timer == false){
                s_is_on_world[k] = false;
            }
        }
        
    if (s_run_timer == true){
        if (s_is_on_spin_in == true) {
            // increment the timer every frame during spin in
            s_spin_in_timer +=1;
        }
        /* 
        no idea why putting this here makes this timer not work correctly, but it works correctly above
        if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT) {
            s_spin_in_init_timer += 1;
        }
        */
        if (mkb::sub_mode==mkb::SMD_GAME_PLAY_INIT || mkb::sub_mode==mkb::SMD_GAME_PLAY_MAIN) {
            //increment the timer every frame during gameplay
            s_gameplay_timer +=1;
        }
        if (s_is_postgoal == true) {
            //increment the timer every frame after breaking the tape before returning to story select
            s_postgoal_timer +=1;
        }
        // if (mkb::sub_mode==mkb::SMD_GAME_GOAL_REPLAY_INIT || mkb::sub_mode==mkb::SMD_GAME_GOAL_REPLAY_MAIN) {
            //increment the timer every frame during the postgoal replay
           // s_postgoal_replay_timer= s_postgoal_replay_timer+1;
        //}
        if (s_is_on_stage_select_screen == true) {
            //increment the timer every frame on the story mode select screen
            s_stage_select_timer +=1;
        }
        if (s_is_on_exit_game_screen == true) {
            // increment the timer every frame on the exit game screen
            s_exit_game_timer +=1;
        }
        if (s_is_on_fallout_screen == true) {
            // increment the timer every frame during the fallout sequence and y/n screen
            s_fallout_timer += 1;
        }
    mkb::OSReport("submode: %d ", mkb::sub_mode);
    // mkb::OSReport("mainmode: %d ", mkb::main_mode);
    }

    
    if (s_completed_stages == 99 && mkb::sub_mode ==mkb::SMD_GAME_GOAL_INIT){
        // stop the timer after breaking the tape on stage 100
        // to do: make this work for story mode packs with less than 100 stages like kaizo
        
        // when completing a run, first correct by 2 frames since the timer will run for 2f after breaking the tape, 
        // then set s_run_timer to false so that if we pause on goal init, we don't subtract 2 from the timer every frame while paused
        /*
        if (s_run_timer == true) {
            // s_loadless_story_timer = s_loadless_story_timer - 2; 
            s_loadless_story_timer = s_segment_start_time[10]+s_segment_timer[10];
            s_run_timer = false;
        }
        */
        }
    if (s_completed_stages == 100) {
        s_is_run_complete = true;
        s_loadless_story_timer = s_segment_start_time[10]+s_segment_timer[10];
    } else {
        s_is_run_complete = false;
    }
    /*
    if (s_completed_stages == 100 && s_is_postgoal == true) {
        s_is_run_complete = true;
    } else {
        s_is_run_complete = false;
    }
    */

    if ( (mkb::main_mode == mkb::MD_GAME && mkb::main_game_mode == mkb::STORY_MODE)  || mkb::sub_mode == mkb::SMD_AUTHOR_PLAY_INIT || mkb::sub_mode == mkb::SMD_AUTHOR_PLAY_MAIN) {
     s_in_story = true;
    } else{
        s_in_story = false;
    }

    // code for handling loadless split and segment times
    for (s32 k=1; k<11; k++){
        if (s_completed_stages == 10*k-1 && mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) { // delete later probably
            // on tape break on the last stage in world k
            // need to correct by 3f since goal_init doesn't happen immediately after the tape is broken
            s_split[k] = s_loadless_story_timer-3;
            s_display_segment = true; // testing
        }

        // if you enter the first stage of the next world but then stage select, do not restart the segment timer
        if (s_completed_stages == 10*(k-1) && mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
            s_can_change_segment_start_time[k] = true;
        } else if (s_completed_stages == 10*(k-1) && mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) {
            s_can_change_segment_start_time[k] = false;
        }

        if (s_completed_stages == 10*(k-1) && mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN && s_can_change_segment_start_time[k] == true) {
            s_segment_start_time[k] = s_loadless_story_timer+1;
        }

        // display split and iw time after breaking the tape on the last stage of a world, but stop displaying when the stage select screen for the next world starts spinning in
        if (s_completed_stages == 10*k && mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) { // delete later
            s_display_segment = false; // testing
        }

        // maybe can simplify but I want to avoid things breaking if you first frame (menuing on goal init or the frame before)
        if (s_completed_stages == 10*k-1 && mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
            s_passed_cutscene = false;
        } else if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) {
            s_passed_cutscene = true;
        }

        if ( (s_completed_stages % 10 == 0) && s_completed_stages != 0 && s_passed_cutscene == false){
            s_is_between_worlds = true;
        } else if ((s_completed_stages % 10 == 0) && s_completed_stages != 0 && s_passed_cutscene == true){
            s_is_between_worlds = false;
        }

        // loadless time taken to complete world k
        s_segment_time[k] = s_split[k]-s_segment_start_time[k];

        if ( ( (10*(k-1) <= s_completed_stages) && (s_completed_stages <= (10*k-2) ) ) || (s_completed_stages == (10*k-1) && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT) ){
            s_segment_timer[k] = s_loadless_story_timer - s_segment_start_time[k];
        } else if (s_completed_stages == (10*k-1) && mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
            // I don't know a better way of doing this to make the timer show the time at tape break 
            // if we show the segment timer at all times, since the timer will run for 2 frames after breaking the tape
            s_segment_timer[k] = s_loadless_story_timer - s_segment_start_time[k] -3;
        }

        
        if ( ( (10*(k-1) +1)<= s_completed_stages) && (s_completed_stages <= (10*k-1)) ) {
            s_is_on_world[k] = true; 
        } else if (s_completed_stages == 10*(k-1) && (s_is_on_stage_select_screen == true || s_is_on_spin_in == true || s_is_on_gameplay == true) ){
            s_is_on_world[k-1] = false;
            s_is_on_world[k] = true;
        } else if (s_completed_stages == 10*k && s_is_postgoal == true && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT && mkb::sub_mode != mkb::SMD_GAME_SCENARIO_RETURN) {
            // testing what happens when adding the scenario return part, since if you are on world k+1, have not completed a stage, and stage select back to the 10 ball screen, 
            // the code thinks you are on world k for 1 frame
            s_is_on_world[k] = true;
        }

        
       if (mkb::g_storymode_stageselect_state == 1) {
        s_dummy_2 = 1;
       } else{
        s_dummy_2 = 0;
       }
    }

     if (mkb::g_storymode_mode == 21){
            s_dummy = 1;
        } else {
            s_dummy = 0;
        /* things tested that didn't work for exit game so far
        SMD_GAME_FORCE_EXIT_MAIN=93
        SMD_GAME_FORCE_OVER_MAIN=96
        SMD_GAME_OVER_POINT_MAIN=86

        things that do work
        SMD_GAME_SUGG_SAVE_MAIN, doesn't include playpoint text, only the save data question after
        SMD_GAME_INTR_SEL_MAIN is the playpoint text

        fallout submode testing
        test: 50, 51, 58, 59, 90, 91, 48, 49, 

        SMD_GAME_READY_INIT=48,
        SMD_GAME_READY_MAIN=49,
        SMD_GAME_RINGOUT_INIT=58,
        SMD_GAME_RINGOUT_MAIN=59
        SMD_GAME_RETRY_INIT=90,
        SMD_GAME_RETRY_MAIN=91

        conclusion: ringout = fallout submode, game retry init/main = y/n menu

        FOR LATER, TRY THESE:
        SMD_GAME_SCENSCNPLAY_RETURN=94 (missing frame on world entry?)
        SMD_AUTHOR_PLAY_INIT=247,
        SMD_AUTHOR_PLAY_MAIN=248,
        SMD_AUTHOR_PLAY_STORY_INIT=249,

        */
    } 
    // debugging
    /*
    if (pad::button_pressed(mkb::PAD_BUTTON_DOWN)) {
        s_completed_stages = 91;
    }
    */
}

void disp() {

    if (s_in_story == false || freecam::should_hide_hud() ){
            return;
        }

    if (pref::get(pref::BoolPref::ShowDeathCounter) == true) {
        s_fullgame_timer_location_y = 1;
    } else {
        s_fullgame_timer_location_y = 0;
    }

    // if the fullgame timer and death counter is off but the segment timer is on, move the segment timer to the top line; if either the fullgame timer or death counter are on but not both are on,
    // move it to the 2nd line, if all 3 are enabled, put it on the 3rd line
    if(s_display_story_timer == false && pref::get(pref::BoolPref::ShowDeathCounter) == false ){
        s_segment_timer_location_y = 0;
    } else if (s_display_story_timer == false || pref::get(pref::BoolPref::ShowDeathCounter) == false ){
            s_segment_timer_location_y = 1;
        } else {
            s_segment_timer_location_y = 2;
        }

    switch(FullgameTimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions))) {
        case FullgameTimerOptions::F_AlwaysShow:
            s_display_story_timer = true;
            break;
        case FullgameTimerOptions::F_BetweenWorlds:
            if (s_is_between_worlds == true) {
                s_display_story_timer = true;
            } else {
                s_display_story_timer = false;
            }
            break;
        case FullgameTimerOptions::F_EndOfRun:
            if (s_is_run_complete == true){ 
                s_display_story_timer = true;
            } else {
                s_display_story_timer = false;
            }
            break;
        case FullgameTimerOptions::F_DontShow:
            s_display_story_timer = false;
            break;
    }

    if (s_display_story_timer == true){
         timerdisp::draw_timer_general(fullgame_timer_location_x, s_fullgame_timer_location_y, fullgame_timer_text_offset, "Time:", s_loadless_story_timer, 0, false, false, draw::WHITE);
    }

    switch(SegmentTimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions))) {
        case SegmentTimerOptions::S_AlwaysShow:
            s_display_segment_timer = true;
            // if the segment timer is always showing, 
            // use the format iw time (split time) after breaking the tape on the last stage of a world
            // to do: get the spacing right for showing the on tape break split time; currently I have show second argument set to false between worlds
            for (s32 k=1; k<11; k++){
                if (s_display_segment_timer == true && s_is_on_world[k] == true && s_is_between_worlds == false && s_is_run_complete == false) {
                    timerdisp::draw_timer_general(segment_timer_location_x, s_segment_timer_location_y, segment_timer_text_offset, "Seg:", s_segment_timer[k], 0, false, false, draw::WHITE);
                }
                else if (s_is_between_worlds == true && s_is_on_world[k] == true && k != 10) {
                    timerdisp::draw_timer_general(segment_timer_location_x, s_segment_timer_location_y, segment_timer_text_offset, "Seg:", s_segment_timer[k], s_split[k], false, false, draw::WHITE); 
                }
            }
            break;
        case SegmentTimerOptions::S_BetweenWorlds:
            s_display_segment_timer = true;
            // otherwise use the format split time (iw time) 
            // to do: switch around arguments if I fix the spacing
            for (s32 k=1; k<11; k++){
                if (s_display_segment_timer == true && s_is_between_worlds == true && s_is_on_world[k] == true && k != 10) {
                    timerdisp::draw_timer_general(segment_timer_location_x, s_segment_timer_location_y, segment_timer_text_offset, "Seg:", s_segment_timer[k], s_split[k], false, false, draw::WHITE);
                } 
            }
            break;
        case SegmentTimerOptions::S_DontShow:
            s_display_segment_timer = false;
            break;
    }

    // if the segment timer is enabled in any capacity, show all 10 split times + iw times after the tape is broken on the last stage
    if (SegmentTimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) != SegmentTimerOptions::S_DontShow) {
        /*
        for (s32 k=1; k<11; k++){
            if (s_is_run_complete == true){
                timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+k, IW_time_text_offset, "Wk:", s_split[k], s_segment_timer[k], true, false, draw::WHITE);
            }
        }
        */
       if (s_is_run_complete == true) {
            // I'm so sorry :(
            // I don't know how to get the text to show "Wk" where k ranges in a for loop
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+1, IW_time_text_offset, "W1:", s_split[1], s_segment_timer[1], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+2, IW_time_text_offset, "W2:", s_split[2], s_segment_timer[2], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+3, IW_time_text_offset, "W3:", s_split[3], s_segment_timer[3], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+4, IW_time_text_offset, "W4:", s_split[4], s_segment_timer[4], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+5, IW_time_text_offset, "W5:", s_split[5], s_segment_timer[5], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+6, IW_time_text_offset, "W6:", s_split[6], s_segment_timer[6], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+7, IW_time_text_offset, "W7:", s_split[7], s_segment_timer[7], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+8, IW_time_text_offset, "W8:", s_split[8], s_segment_timer[8], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+9, IW_time_text_offset, "W9:", s_split[9], s_segment_timer[9], true, false, draw::WHITE);
            timerdisp::draw_timer_general(IW_time_location_x, s_segment_timer_location_y+10, IW_time_text_offset, "W10:", s_split[10], s_segment_timer[10], true, false, draw::WHITE);
       }
    }

    if (pref::get(pref::BoolPref::StoryTimerWarning) == true && FullgameTimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions)) == FullgameTimerOptions::F_DontShow 
    && SegmentTimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) == SegmentTimerOptions::S_DontShow && mkb::g_storymode_mode == 21){
        // mkb::g_storymode_mode 21 is the name entry screen, not sure if it has a name in ghidra
        draw::debug_text(460, 425, draw::RED, "Timer Not On!");
    }

    // debugging
    /*
    timerdisp::draw_timer(static_cast<s32>(s_dummy), "dbg1:", 0, draw::WHITE, true);
    timerdisp::draw_timer(static_cast<s32>(60*mkb::g_storymode_mode), "dbg2:", 1, draw::WHITE, true);
    
    timerdisp::draw_timer(static_cast<s32>(60*s_completed_stages), "dbg3:", 2, draw::WHITE, true);
    timerdisp::draw_timer(static_cast<s32>(s_segment_start_time[10]), "dbg4:", 3, draw::WHITE, true);
    timerdisp::draw_timer(static_cast<s32>(s_segment_timer[10]), "dbg5:", 4, draw::WHITE, true);
    */
    

    /*
    timerdisp::draw_timer_general(fullgame_timer_location_x, 3, fullgame_timer_text_offset, "Time:", s_loadless_story_timer, 0, false, false, draw::WHITE);
    timerdisp::draw_timer_general(segment_timer_location_x, 1, segment_timer_text_offset, "Seg:", s_loadless_story_timer, 0, false, false, draw::WHITE);
    timerdisp::draw_timer_general(IW_time_location_x, 2, IW_time_text_offset, "W1:", s_loadless_story_timer, 0, false, false, draw::WHITE);

    timerdisp::draw_timer_general(segment_timer_location_x, s_segment_timer_location_y, segment_timer_text_offset, "Seg:", s_segment_timer[k], 0, false, false, draw::WHITE);

    */
// draw_storytimer(s32 frames_1, const char* prefix, u32 row, mkb::GXColor color, bool show_seconds, bool second_argument, s32 frames_2) (reference)
// draw_timer_general(u32 pos_x, u32 pos_y, u32 text_offset, const char* prefix, s32 frames_1, s32 frames_2, bool show_second_argument, bool show_seconds_only, mkb::GXColor color) (reference)
// timerdisp text offset for 5 characters (includes :) is 54; for 4 characters, try an additional 54 for the x pos, 
//  if (IlBattleLength(pref::get(pref::U8Pref::IlBattleLength)) == IlBattleLength::Endless); (reference)
} 

} // namespace storytimer
