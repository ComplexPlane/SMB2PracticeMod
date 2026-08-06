#pragma once

#include "utils/base.h"

enum { SS_SLOT_COUNT = 8 };

typedef enum SS_SaveResult SS_SaveResult;
enum SS_SaveResult {
    SS_SaveResult_Ok,
    SS_SaveResult_ErrMainMode,
    SS_SaveResult_ErrPostFallout,
    SS_SaveResult_ErrPostGoal,
    SS_SaveResult_ErrDuringRetry,
    SS_SaveResult_ErrPostTimeout,
    SS_SaveResult_ErrSubMode,
    SS_SaveResult_ErrViewStage,
    SS_SaveResult_ErrInsufficientMemory,
};

typedef enum SS_LoadResult SS_LoadResult;
enum SS_LoadResult {
    SS_LoadResult_Ok,
    SS_LoadResult_ErrMainMode,
    SS_LoadResult_ErrSubMode,
    SS_LoadResult_ErrTimeOver,
    SS_LoadResult_ErrEmpty,
    SS_LoadResult_ErrWrongStage,
    SS_LoadResult_ErrWrongMonkey,
    SS_LoadResult_ErrViewStage,
    SS_LoadResult_ErrPausedAndNonGameplaySubmode,
};

void savest_init();
void savest_tick();

SS_SaveResult savest_save(u32 slot);
SS_LoadResult savest_load(u32 slot);
void savest_clear(u32 slot);
bool savest_is_empty(u32 slot);

bool savest_was_state_loaded_this_frame();
bool savest_is_enabled();
