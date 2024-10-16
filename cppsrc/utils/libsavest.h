// Savestate functionality decoupled from UI/controls

#pragma once

#include "mkb/mkb.h"
#include "utils/memstore.h"

namespace libsavest {

class SaveState {
 public:
    enum class SaveResult {
        Ok,
        ErrorMainMode,
        ErrorPostFallout,
        ErrorPostGoal,
        ErrorDuringRetry,
        ErrorPostTimeout,
        ErrorSubMode,
        ErrorViewStage,
        ErrorInsufficientMemory,
    };
    enum class LoadResult {
        Ok,
        ErrorMainMode,
        ErrorSubMode,
        ErrorTimeOver,
        ErrorEmpty,
        ErrorWrongStage,
        ErrorWrongMonkey,
        ErrorViewStage,
        ErrorPausedAndNonGameplaySubmode,
    };

    void tick();
    SaveResult save();
    LoadResult load();
    bool isEmpty();
    void clear();

 private:
    u32 m_flags = 0;
    s32 m_stage_id = -1;
    u8 m_character = 0;
    memstore::MemStore m_store;
    u8 m_pause_menu_sprite_status = 0;
    mkb::Sprite m_pause_menu_sprite;

    void pass_over_regions();
    void handle_pause_menu_save();
    void handle_pause_menu_load();
    bool handle_load_state_from_nonplay_submode();
};

void init();  // Global initialization for savestates
bool state_loaded_this_frame();
bool savestates_enabled();

}  // namespace libsavest
