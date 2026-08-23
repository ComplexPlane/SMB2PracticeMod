#pragma once

#include "utils/base.h"

typedef enum {
    Binds_Encoding_SinglePress,
    Binds_Encoding_ChordPress,
    Binds_Encoding_Invalid,
} Binds_Encoding;

bool Binds_Pressed(u8 bind_id, bool priority);
bool Binds_Down(u8 bind_id, bool priority);

Binds_Encoding Binds_GetEncodingType();
u8 Binds_GetCurrentEncoding();
void Binds_ToStr(u8 bind_id, char *buf);

void Binds_Init();
void Binds_Tick();
