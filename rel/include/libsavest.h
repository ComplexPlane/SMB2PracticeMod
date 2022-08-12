// Savestate functionality decoupled from UI/controls

#pragma once

#include <mkb.h>
#include "memstore.h"

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
        ErrorPostTimeout,
        ErrorEmpty,
        ErrorWrongStage,
        ErrorWrongMonkey,
        ErrorViewStage,
        ErrorPausedAndNonGameplaySubmode,
    };

    SaveState() noexcept;
    ~SaveState();

    void tick();
    SaveResult save();
    LoadResult load();

 private:
    u32 m_flags;
    s32 m_stage_id;
    u8 m_character;
    memstore::MemStore m_store;
    u8 m_pause_menu_sprite_status;
    mkb::Sprite m_pause_menu_sprite;

    void pass_over_regions();
    void handle_pause_menu_save();
    void handle_pause_menu_load();
    bool handle_load_state_from_nonplay_submode();
};

// Global initialization for savestates
void init();

}  // namespace libsavest
