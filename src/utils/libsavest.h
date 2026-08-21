#pragma once

#include "utils/base.h"

enum { SS_SLOT_COUNT = 8 };

typedef enum {
    SS_SaveResult_Ok,
    SS_SaveResult_ErrMainMode,
    SS_SaveResult_ErrPostFallout,
    SS_SaveResult_ErrPostGoal,
    SS_SaveResult_ErrDuringRetry,
    SS_SaveResult_ErrPostTimeout,
    SS_SaveResult_ErrSubMode,
    SS_SaveResult_ErrViewStage,
    SS_SaveResult_ErrInsufficientMemory,
} SS_SaveResult;

typedef enum {
    SS_LoadResult_Ok,
    SS_LoadResult_ErrMainMode,
    SS_LoadResult_ErrSubMode,
    SS_LoadResult_ErrTimeOver,
    SS_LoadResult_ErrEmpty,
    SS_LoadResult_ErrWrongStage,
    SS_LoadResult_ErrWrongMonkey,
    SS_LoadResult_ErrViewStage,
    SS_LoadResult_ErrPausedAndNonGameplaySubmode,
} SS_LoadResult;

void SS_Init();
void SS_Tick();

SS_SaveResult SS_Save(u32 slot);
SS_LoadResult SS_Load(u32 slot);
void SS_Clear(u32 slot);
bool SS_IsEmpty(u32 slot);

bool SS_WasStateLoadedThisFrame();
bool SS_IsEnabled();
