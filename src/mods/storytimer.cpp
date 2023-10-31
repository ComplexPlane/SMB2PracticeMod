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
static u32 s_split[10];    // s_split[k] is the loadless time on tape break of the 10th stage of world k
static u32 s_segment_time[10];  // s_segment_time[k] is the loadless time spent on world k, starting from stage select spin in to tape break on the 10th stage
static u32 s_segment_start_time[10];  // the loadless time at the start of world k, used to calculate s_segment_time[k]
static bool s_display_story_timer;
static bool s_display_segment_timer;
static bool s_display_split;
static bool s_display_world_time;
static bool s_display_segment;
static bool s_is_between_worlds;
static bool s_passed_cutscene;
static bool s_is_run_complete;
static u32 s_segment_timer[10];
static bool s_is_on_world[10]; 
static bool s_can_change_segment_start_time[10];
static u32 s_segment_timer_location;

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

   /*
   if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
    s_completed_stages +=1;
   } 
    */

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
        s_spin_in_timer = 0 ;
        s_gameplay_timer = 0;
        s_postgoal_timer = 0;
        // s_postgoal_replay_timer = 0;
        s_stage_select_timer = 0;
        s_exit_game_timer = 0;
        s_fallout_timer = 0;
        s_run_timer = false;
        s_lower_stage_counter = false;
        s_loadless_story_timer = 0;
        s_completed_stages = 0;
        s_prev_completed_stage_count = 0;
        } else{
            s_loadless_story_timer = s_spin_in_timer+s_gameplay_timer+s_postgoal_timer+s_stage_select_timer+s_exit_game_timer+s_fallout_timer;
        }

    if (s_run_timer == true){
        if (s_is_on_spin_in == true) {
            // increment the timer every frame during spin in
            s_spin_in_timer +=1;
        }
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
        s_run_timer = false;
        }

    if (s_completed_stages == 100 && s_is_postgoal == true) {
        s_is_run_complete = true;
    } else {
        s_is_run_complete = false;
    }

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
        } else if (s_completed_stages == 10*(k-1) && mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE){
            s_is_on_world[k-1] = false;
            s_is_on_world[k] = true;
        } else if (s_completed_stages == 10*k && s_is_postgoal == true && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT) {
            s_is_on_world[k] = true;
        }
        

       /* can't use switch case in for loop :(

        switch (s_completed_stages) {
            case ( (10*(k-1) +1<= s_completed_stages) && (s_completed_stages <= (10*k-1)) ):
                s_is_on_world[k] = true; 
                break;
            case (s_completed_stages == 10*(k-1) && mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE):
                s_is_on_world[k-1] = false;
                s_is_on_world[k] = true;
                break;
            case (s_completed_stages == 10*k && s_is_postgoal == true):
                s_is_on_world[k] = true;
                break;
        }

        */
       if (s_is_on_world[1] == true) {
        s_dummy_2 = 1;
       } else {
        s_dummy_2 = 0;
       }
    }



   /*
   if ( ( (10*(k-1)-1 < s_completed_stages) && (s_completed_stages < 10*k-1) ) || (s_completed_stages == 10*k-1 && s_is_postgoal == false) ){
            s_segment_timer[k] = s_loadless_story_timer - s_segment_start_time[k];
        }

        // display split and iw time after breaking the tape on the last stage of a world, but stop displaying when the stage select screen for the next world starts spinning in
        if ((s_completed_stages == 10*k-1 || s_completed_stages == 10*k ) &&  mkb::g_storymode_stageselect_state != mkb::STAGE_SELECT_INTRO_SEQUENCE) {
            s_display_segment = true;
        } else if (s_completed_stages == 10*k && mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) {
            s_display_segment = false;
        }

   */

     if (s_is_between_worlds == true){
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

}

void disp() {
        // timerdisp::draw_timer(static_cast<s32>(s_display_segment), "Splt:", 1, draw::WHITE, true);
        // timerdisp::draw_timer(static_cast<s32>(60*mkb::sub_mode), "dbg:", 4, draw::WHITE, true);
        // timerdisp::draw_timer(static_cast<s32>(s_segment_timer[2]), "IW:", 3, draw::WHITE, true);
/*
        if (FullgameTimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions)) == FullgameTimerOptions::F_AlwaysShow){
            timerdisp::draw_storytimer(static_cast<s32>(s_loadless_story_timer), "Time:", 0, draw::WHITE, false);
        }
*/
    /* 
    if (mkb::main_mode != mkb::MD_GAME || mkb::main_game_mode != mkb::STORY_MODE ||
        freecam::should_hide_hud()) {
        return;
    }
*/
if (s_in_story == false || freecam::should_hide_hud() ){
        return;
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
        timerdisp::draw_storytimer(static_cast<s32>(s_loadless_story_timer), "Time:", 0, draw::WHITE, false, false, 0);
    }
    
    // if the fullgame timer is off but the segment timer is on, move the segment timer up by 1 line
    if(FullgameTimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions)) == FullgameTimerOptions::F_DontShow){
        s_segment_timer_location = 1;
    } else {
            s_segment_timer_location = 0;
        }

    switch(SegmentTimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions))) {
        case SegmentTimerOptions::S_AlwaysShow:
            s_display_segment_timer = true;
            // if the segment timer is always showing, 
            // use the format iw time (split time) after breaking the tape on the last stage of a world
            for (s32 k=1; k<11; k++){
                if (s_display_segment_timer == true && s_is_on_world[k] == true && s_is_between_worlds == false && s_is_run_complete == false) {
                    timerdisp::draw_storytimer(static_cast<s32>(s_segment_timer[k]), "Seg:", 1, draw::WHITE, false, false, 0);
                }
                else if (s_is_between_worlds == true && s_is_on_world[k] == true && k != 10) {
                    timerdisp::draw_storytimer(static_cast<s32>(s_segment_timer[k]), "Time:", 1, draw::WHITE, false, true, s_split[k]); 
                }
            }
            break;
        case SegmentTimerOptions::S_BetweenWorlds:
            s_display_segment_timer = true;
            // otherwise use the format split time (iw time)
            for (s32 k=1; k<11; k++){
                if (s_display_segment_timer == true && s_is_between_worlds == true && s_is_on_world[k] == true && k != 10) {
                    timerdisp::draw_storytimer(static_cast<s32>(s_split[k]), "Time:", 1, draw::WHITE, false, true, s_segment_timer[k]);
                } 
            }
            break;
        case SegmentTimerOptions::S_DontShow:
            s_display_segment_timer = false;
            break;
    }

    // if the segment timer is enabled in any capacity, show all 10 split times + iw times after the tape is broken on the last stage
    if (SegmentTimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) != SegmentTimerOptions::S_DontShow) {
        for (s32 k=1; k<11; k++){
            if (s_is_run_complete == true){
                timerdisp::draw_storytimer(static_cast<s32>(s_split[k]), "Splt:", k+s_segment_timer_location, draw::WHITE, false, true, s_segment_timer[k]);
            }
        }
    }

/*
    for (s32 k=1; k<11; k++){
        if (s_display_story_timer == true && s_is_between_worlds == false){
            timerdisp::draw_storytimer(static_cast<s32>(s_loadless_story_timer), "Time:", 0, draw::WHITE, false, false, 0);
        } else if (s_display_story_timer == true && s_is_between_worlds == true && s_is_on_world[k] == true && k != 10) {
            timerdisp::draw_storytimer(static_cast<s32>(s_loadless_story_timer), "Time:", 0, draw::WHITE, false, true, s_split[k]); 
        }
        if (s_display_segment_timer == true && s_is_on_world[k] == true && s_is_run_complete == false) {
            timerdisp::draw_storytimer(static_cast<s32>(s_segment_timer[k]), "Seg:", 1, draw::WHITE, false, false, 0);
        } else if (s_display_segment_timer == true && s_is_run_complete == true) {
            timerdisp::draw_storytimer(static_cast<s32>(s_split[k]), "Splt:", k, draw::WHITE, false, true, s_segment_timer[k]);
            // to do: modify draw_storytimer so that you can display in the format "World k: split k time (segment k time)"
        }
        }
*/

    // debugging
    timerdisp::draw_timer(static_cast<s32>(60*s_completed_stages), "Splt:", 0, draw::WHITE, true);
    timerdisp::draw_timer(static_cast<s32>(60*s_prev_completed_stage_count), "Splt:", 1, draw::WHITE, true);
// draw_storytimer(s32 frames_1, const char* prefix, u32 row, mkb::GXColor color, bool show_seconds, bool second_argument, s32 frames_2) (reference)
//  if (IlBattleLength(pref::get(pref::U8Pref::IlBattleLength)) == IlBattleLength::Endless); (reference)
} 

} // namespace storytimer
