#pragma once

#include "utils/memstore.h"

void timer_init();
void timer_disp();
void timer_save_state(Store *store, StoreFunc func);
