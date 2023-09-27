#pragma once

#include "mkb/mkb.h"

namespace binds {

enum class EncodingType {
    SinglePress,
    ChordPress,
    Invalid,
};

bool bind_pressed(u8 bind_id, bool priority = false);
EncodingType get_encoding_type();
u8 get_current_encoding();
u8 get_input1(u8 bind_id);
u8 get_input2(u8 bind_id);
char* get_bind_str(u8 bind_id);

void init();
void tick();

}  // namespace binds
