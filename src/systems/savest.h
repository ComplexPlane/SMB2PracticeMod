#pragma once

#include "mkb/mkb.h"

namespace savest {

constexpr u32 SLOT_COUNT = 8;

enum class SaveResult {
    Ok,
    ErrMainMode,
    ErrPostFallout,
    ErrPostGoal,
    ErrDuringRetry,
    ErrPostTimeout,
    ErrSubMode,
    ErrViewStage,
    ErrInsufficientMemory,
};

enum class LoadResult {
    Ok,
    ErrMainMode,
    ErrSubMode,
    ErrTimeOver,
    ErrEmpty,
    ErrWrongStage,
    ErrWrongMonkey,
    ErrViewStage,
    ErrPausedAndNonGameplaySubmode,
};

enum class Action {
    None,
    Load,
    Save,
};

void init();
void tick();

SaveResult save(u32 slot);
LoadResult load(u32 slot);
void clear(u32 slot);
bool is_empty(u32 slot);
u32 get_timestamp(u32 slot);

bool is_enabled();
Action get_last_action();
bool last_action_was_load();
bool last_action_was_save();

}  // namespace savest
