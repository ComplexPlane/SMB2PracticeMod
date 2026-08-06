#pragma once

#include "utils/base.h"

typedef enum binds_EncodingType {
    binds_EncodingType_SinglePress,
    binds_EncodingType_ChordPress,
    binds_EncodingType_Invalid,
} binds_EncodingType;
bool binds_bind_pressed(u8 bind_id, bool priority);
bool binds_bind_down(u8 bind_id, bool priority);
binds_EncodingType binds_get_encoding_type(void);
u8 binds_get_current_encoding(void);
void binds_get_bind_str(u8 bind_id, char* buf);
void binds_init(void);
void binds_tick(void);
