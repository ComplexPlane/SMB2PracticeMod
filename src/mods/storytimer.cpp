#include "storytimer.h"

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

static u32 s_spin_in_timer;
static u32 s_gameplay_timer;
static u32 s_postgoal_timer;
static u32 s_stage_select_timer;
static u32 s_exit_game_timer;
static u32 s_fallout_timer;
static u32 s_timeover_timer;
static u32 s_spin_in_timer_correction;
static u32 s_game_scenario_return_timer_correction;
static u32 s_world_start_timer_correction;
static u32 s_loadless_story_timer;
static bool s_in_story;
static bool s_is_on_world[11]; 
static bool s_is_between_worlds;
static bool s_is_run_complete;
static bool s_is_on_spin_in;
static bool s_is_on_gameplay;
static bool s_is_postgoal;
static bool s_is_on_stage_select_screen; 
static bool s_is_on_exit_game_screen;
static bool s_is_on_fallout_screen;
static bool s_is_timeover;
static bool s_can_increment_stage_counter;
static bool s_lower_stage_counter;
static bool s_cap_postgoal_timer;
static bool s_start_stage_fade_out_timer;
static u32 s_stage_fade_out_timer;
static u32 s_prev_completed_stage_count;
static s32 s_completed_stages;
static u32 s_segment_timer[11]; // IW timer for world k
static u32 s_split[11];    // s_split[k] is the loadless time on tape break of the 10th stage of world k
static u32 s_segment_start_time[11];  // the loadless time at the start of world k, used to calculate s_segment_timer[k]
static bool s_can_change_segment_start_time[11];
static bool s_display_story_timer;
static bool s_display_segment_timer;
static u32 s_fullgame_timer_location_y;
static u32 s_segment_timer_location_y;
static constexpr s32 fullgame_timer_location_x = 18+24;
static constexpr s32 fullgame_timer_text_offset = 56;
static constexpr s32 segment_timer_location_x = 30+24;
static constexpr s32 segment_timer_text_offset = 44;
static constexpr s32 IW_time_location_x = 42+24; 
static constexpr s32 IW_time_text_offset = 32;
static constexpr s32 stage_fade_out_time = 49;
static u32 s_dummy;
static u32 s_dummy_2;
static u32 s_dummy_3;

void init() {
    static patch::Tramp<decltype(&mkb::handle_pausemenu_selection)> s_handle_pause_menu_selection_tramp;
    patch::hook_function(s_handle_pause_menu_selection_tramp, mkb::handle_pausemenu_selection, [](int param_1) {
        s_handle_pause_menu_selection_tramp.dest(param_1);
        if (mkb::pausemenu_type == mkb::PMT_STORY_PLAY && mkb::g_current_focused_pause_menu_entry == 4){
            // stage select is on line 4 of the pause menu (top line is line 0)
            s_dummy_3 = 1;
        }
    });
}
    
void tick() {
    
    // for later use, it's useful to record how many stages we've completed
    // there is likely a simpler way to increment the stage counter in a way that works when you retry after breaking the tape and also does not break if you pause during 
    // game goal init or game goal main

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

   // for later, it's useful to record what submodes correspond to spin in, gameplay, etc. 
   // splitting up the timer this way also makes it easier in the future if I decide to implement features such as menuing timeloss

    // submodes during spin in
    if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT || mkb::sub_mode==mkb::SMD_GAME_READY_INIT || mkb::sub_mode==mkb::SMD_GAME_READY_MAIN) {
        s_is_on_spin_in = true;
    } else {
        s_is_on_spin_in = false;
    }

    // story mode states entered during the story select screen
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

    // submodes during timeover
    if (mkb:: sub_mode == mkb::SMD_GAME_TIMEOVER_INIT || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN) {
        s_is_timeover = true;
    } else {
        s_is_timeover = false;
    }

    if ( (mkb::main_mode == mkb::MD_GAME && mkb::main_game_mode == mkb::STORY_MODE)  || mkb::sub_mode == mkb::SMD_AUTHOR_PLAY_INIT || mkb::sub_mode == mkb::SMD_AUTHOR_PLAY_MAIN) {
     s_in_story = true;
    } else{
        s_in_story = false;
    }

    // this code is used to halt the timer once the screen becomes completely white when stage selecting out of a level
    if (mkb::pausemenu_type == mkb::PMT_STORY_PLAY && mkb::g_current_focused_pause_menu_entry == 4 && pad::button_pressed(mkb::PAD_BUTTON_A) == true && s_is_postgoal == true){
            // stage select is on line 4 of the pause menu (top line is line 0)
            s_start_stage_fade_out_timer = true;
        } else if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE){
            s_start_stage_fade_out_timer = false;
            s_dummy_3 = 0;
        }
    if (s_start_stage_fade_out_timer == true) {
        s_stage_fade_out_timer += 1;
    } else {
        s_stage_fade_out_timer = 0;
    }


    // before starting the run, there are several values we zero on the file select screen (this serves to reset the timer)
    // to do: in the future, have the timer not reset unless the file's data is reset (either manually or by using the IW move up/down feature)

    if (mkb::g_storymode_mode == 5){
        // zero the timer on the file select screen and set the number of completed stages to 0
        s_spin_in_timer_correction = 0;
        s_spin_in_timer = 0 ;
        s_gameplay_timer = 0;
        s_postgoal_timer = 0;
        s_stage_select_timer = 0;
        s_world_start_timer_correction = 0;
        s_game_scenario_return_timer_correction = 0;
        s_exit_game_timer = 0;
        s_fallout_timer = 0;
        s_timeover_timer = 0;
        s_lower_stage_counter = false;
        s_loadless_story_timer = 0;
        s_completed_stages = 0;
        s_prev_completed_stage_count = 0;
        s_start_stage_fade_out_timer = false;
        s_stage_fade_out_timer = 0;
        s_dummy_3 = 0;
        for (s32 k=1; k<11; k++) {
            // on the file select screen, set these to false just in case you reset while on world k but did not complete 10k stages
            s_is_on_world[k] = false;
        }
        
        }

    if (s_in_story == true) {

        if (mkb::sub_mode == mkb::SMD_GAME_FIRST_INIT) {
            // need to add 1 additional frame to the timer during spin in 
            // putting this code before the code below for s_loadless_story_timer makes the timer tick up more naturally when transitioning from the 10 ball screen to spin in, 
            // more specifically, this prevents the timer from skipping ahead for a few frames, then pausing for a few frames to even itself out (I don't understand why that happens)
            s_spin_in_timer_correction += 1;
        }

        // last 3 terms are correction terms to account for missing frames
        s_loadless_story_timer = s_spin_in_timer+s_gameplay_timer+s_postgoal_timer+s_stage_select_timer+s_exit_game_timer+s_fallout_timer+s_timeover_timer+
            +s_spin_in_timer_correction+s_game_scenario_return_timer_correction+ s_world_start_timer_correction;


        if (s_is_on_spin_in == true) {
            // increment the timer every frame during spin in
            s_spin_in_timer +=1;
        }
        if (s_is_on_gameplay == true) {
            //increment the timer every frame during gameplay
            s_gameplay_timer +=1;
        }
        if (s_is_postgoal == true && s_stage_fade_out_timer < stage_fade_out_time+1) {
            // increment the timer every frame after game goal init happens; once you press stage select, a separate 49 frame timer is started (fade out from stage select
            // to the first completely white frame takes 49 frames). once the timer hits 49 frames, stop incrementing the timer 
            // until the 10 ball screen starts spinning in
            s_postgoal_timer +=1;
        }
        if (mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE || mkb::g_storymode_stageselect_state == 3 || 
        mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_IDLE) {
            //increment the timer every frame on the story mode select screen until the a press input; we do not include the transition time after pressing a afterwards 
            // even ignoring completely white frames, the time spent on mkb::g_storymode_stageselect_state == mkb::STAGE_SELECTED can be
            // highly variable (up to over 40 frames sometimes!), so for the purpose of a loadless timer, it makes sense to cut this out from the timer
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
        if (s_is_timeover == true) {
            // increment the timer every frame during the timeover sequence
            s_timeover_timer += 1;
        }

        for (s32 k=1; k<11; k++) {
            if (s_is_on_world[k] == true) {
                // need to add 2 frames to the timer at the start of each world
            s_world_start_timer_correction = 2*k;
            }
        }
        
        if (mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN && s_completed_stages % 10 != 0) {
            // need to add 2 frames to the timer when stage selecting to the 10 ball screen, but don't correct if on the last stage of a world
            // since the next frame the timer should increment on is covered by s_world_start_timer_correction
                s_game_scenario_return_timer_correction += 2;
            }
            
    }

    if (s_completed_stages == 100) {
        s_is_run_complete = true;
        // s_segment_timer[k] is already corrected for tape break (see code below), so no further corrections needed to get an accurate time
        s_loadless_story_timer = s_segment_start_time[10]+s_segment_timer[10];
    } else {
        s_is_run_complete = false;
    }

    // it is useful to know when we are between worlds (for the "between worlds" option in the menu)
    if ( (s_completed_stages % 10 == 0) && s_completed_stages != 0 && mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN){
            s_is_between_worlds = true;
        } else if ( (s_completed_stages % 10 == 0 && mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) ||  s_completed_stages % 10 !=0 ){
            // no longer "between worlds" if you enter the next worlds 10 ball screen, or if you break the tape on the last stage
            // of the current world, but retry 
            s_is_between_worlds = false;
        }

    // code for handling loadless split and segment times
    for (s32 k=1; k<11; k++){
        
        // if you enter the first stage of the next world but then stage select, do not restart the segment timer
        if (s_completed_stages == 10*(k-1) && mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN) {
            s_can_change_segment_start_time[k] = true;
        } else if (s_completed_stages == 10*(k-1) && mkb::g_storymode_stageselect_state == mkb::STAGE_SELECT_INTRO_SEQUENCE) {
            s_can_change_segment_start_time[k] = false;
        }

        if (s_completed_stages == 10*(k-1) && mkb::sub_mode == mkb::SMD_GAME_SCENARIO_RETURN && s_can_change_segment_start_time[k] == true) {
            s_segment_start_time[k] = s_loadless_story_timer+2;
        }
      
        if ( ( (10*(k-1) <= s_completed_stages) && (s_completed_stages <= (10*k-2) ) ) || (s_completed_stages == (10*k-1) && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT) ){
            s_segment_timer[k] = s_loadless_story_timer - s_segment_start_time[k];
        } else if (s_completed_stages == (10*k-1) && mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT) {
            // I don't know a better way of doing this to make the timer show the time at tape break 
            // if we show the segment timer at all times, since the timer will run for 2 frames after breaking the tape
            s_segment_timer[k] = s_loadless_story_timer - s_segment_start_time[k] -3;
        }

        // fullgame loadless time upon breaking the tape on the last stage of world k 
         s_split[k] = s_segment_start_time[k]+s_segment_timer[k];

        if ( ( (10*(k-1) +1)<= s_completed_stages) && (s_completed_stages <= (10*k-1)) ) {
            s_is_on_world[k] = true; 
        } else if (s_completed_stages == 10*(k-1) && (s_is_on_stage_select_screen == true || s_is_on_spin_in == true || s_is_on_gameplay == true) ){
            // if you've completed 10*(k-1) stages, are in world k, but haven't completed the first stage yet, then you are not in world k-1
            s_is_on_world[k-1] = false;
            s_is_on_world[k] = true;
        } else if (s_completed_stages == 10*k && s_is_postgoal == true && mkb::sub_mode != mkb::SMD_GAME_GOAL_INIT && mkb::sub_mode != mkb::SMD_GAME_SCENARIO_RETURN) {
            s_is_on_world[k] = true;
        }
    }

}

void disp() {

    if (s_in_story == false || freecam::should_hide_hud() ){
            return;
        }

    // move the positions of the fullgame and segment timers if the death counter is on
    if (pref::get(pref::BoolPref::ShowDeathCounter) == true) {
        s_fullgame_timer_location_y = 3;
    } else {
        s_fullgame_timer_location_y = 2;
    }

    // if the fullgame timer and death counter is off but the segment timer is on, move the segment timer to the top line; if either the fullgame timer or death counter are on but not both are on,
    // move it to the 2nd line, if all 3 are enabled, put it on the 3rd line
    if(s_display_story_timer == false && pref::get(pref::BoolPref::ShowDeathCounter) == false ){
        s_segment_timer_location_y = 2;
    } else if (s_display_story_timer == false || pref::get(pref::BoolPref::ShowDeathCounter) == false ){
            s_segment_timer_location_y = 3;
        } else {
            s_segment_timer_location_y = 4;
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
         timerdisp::draw_timer(fullgame_timer_location_x, s_fullgame_timer_location_y, fullgame_timer_text_offset, "Time:", s_loadless_story_timer, 0, false, false, draw::WHITE);
    }

    switch(SegmentTimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions))) {
        case SegmentTimerOptions::S_AlwaysShow:
            for (s32 k=1; k<11; k++){
                if (s_is_on_world[k] == true && s_is_run_complete == false) {
                    timerdisp::draw_timer(segment_timer_location_x, s_segment_timer_location_y, segment_timer_text_offset, "Seg:", s_segment_timer[k], 0, false, false, draw::WHITE);
                }
                
            }
            break;
        case SegmentTimerOptions::S_BetweenWorlds:
            for (s32 k=1; k<11; k++){
                if (s_is_between_worlds == true && s_is_on_world[k] == true && k != 10) {
                    timerdisp::draw_timer(segment_timer_location_x, s_segment_timer_location_y, segment_timer_text_offset, "Seg:", s_segment_timer[k], 0, false, false, draw::WHITE);
                } 
            }
            break;
        case SegmentTimerOptions::S_DontShow:
            s_display_segment_timer = false;
            break;
    }

    // if the segment timer is enabled in any capacity, show all 10 split times + iw times after the tape is broken on the last stage
    if (SegmentTimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) != SegmentTimerOptions::S_DontShow) {
       if (s_is_run_complete == true) {
            // I'm so sorry :(
            // I don't know how to get the text to show "Wk" where k ranges in a for loop
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y, IW_time_text_offset, "W1:", s_split[1], s_segment_timer[1], true, false, draw::WHITE);
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y+1, IW_time_text_offset, "W2:", s_split[2], s_segment_timer[2], true, false, draw::WHITE);
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y+2, IW_time_text_offset, "W3:", s_split[3], s_segment_timer[3], true, false, draw::WHITE);
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y+3, IW_time_text_offset, "W4:", s_split[4], s_segment_timer[4], true, false, draw::WHITE);
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y+4, IW_time_text_offset, "W5:", s_split[5], s_segment_timer[5], true, false, draw::WHITE);
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y+5, IW_time_text_offset, "W6:", s_split[6], s_segment_timer[6], true, false, draw::WHITE);
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y+6, IW_time_text_offset, "W7:", s_split[7], s_segment_timer[7], true, false, draw::WHITE);
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y+7, IW_time_text_offset, "W8:", s_split[8], s_segment_timer[8], true, false, draw::WHITE);
            timerdisp::draw_timer(IW_time_location_x, s_segment_timer_location_y+8, IW_time_text_offset, "W9:", s_split[9], s_segment_timer[9], true, false, draw::WHITE);
            // use segment timer spacing for w10 since "W10" is 3 characters long, not 2
            timerdisp::draw_timer(segment_timer_location_x, s_segment_timer_location_y+9, segment_timer_text_offset, "W10:", s_split[10], s_segment_timer[10], true, false, draw::WHITE);
       }
    }

    // show warning on the name entry screen if no timers are on (if the toggle for the warning is turned on)
    if (pref::get(pref::BoolPref::StoryTimerWarning) == true && FullgameTimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions)) == FullgameTimerOptions::F_DontShow 
    && SegmentTimerOptions(pref::get(pref::U8Pref::SegmentTimerOptions)) == SegmentTimerOptions::S_DontShow && mkb::g_storymode_mode == 21){
        // mkb::g_storymode_mode 21 is the name entry screen, not sure if it has a name in ghidra
        draw::debug_text(460, 425, draw::RED, "Timer Not On!");
    }
    
    // debugging

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
    if (pad::button_pressed(mkb::PAD_BUTTON_A) == true) {
        s_dummy_2 = 1;
       } else{
        s_dummy_2 = 0;
       }
       
    /*
    if (pad::button_pressed(mkb::PAD_BUTTON_DOWN)) {
        s_completed_stages = 91;
    }
    */
    /*
   if (FullgameTimerOptions(pref::get(pref::U8Pref::FullgameTimerOptions)) == FullgameTimerOptions::F_AlwaysShow){
        timerdisp::draw_timer(380, 0, 44, "dbg:", static_cast<s32>(60*s_cap_postgoal_timer), 1, false, true, draw::WHITE);
        timerdisp::draw_timer(380, 1, 44, "dbg:", static_cast<s32>(60*s_start_stage_fade_out_timer), 1, false, true, draw::WHITE);
        timerdisp::draw_timer(380, 2, 44, "dbg:", static_cast<s32>(60*s_stage_fade_out_timer), 1, false, true, draw::WHITE);
    }
*/
    
} 

} // namespace storytimer
