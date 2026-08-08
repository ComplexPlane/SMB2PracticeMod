#include "gamecode.h"

#include "mkb/mkb.h"

namespace gamecode {

static char s_vanilla_gamecode[] = "GM2E8P";
static char s_original_gamecode[6];

void init() {
    mkb::memcpy(s_original_gamecode, mkb::DVD_GAME_NAME, sizeof(s_original_gamecode));
}

bool is_vanilla() {
    return mkb::memcmp(s_original_gamecode, s_vanilla_gamecode, sizeof(s_original_gamecode)) == 0;
}

void override_with_vanilla() {
    mkb::memcpy(mkb::DVD_GAME_NAME, s_vanilla_gamecode, sizeof(s_original_gamecode));
    mkb::DCFlushRange(mkb::DVD_GAME_NAME, sizeof(s_original_gamecode));
}

void restore() {
    mkb::memcpy(mkb::DVD_GAME_NAME, s_original_gamecode, sizeof(s_original_gamecode));
    mkb::DCFlushRange(mkb::DVD_GAME_NAME, sizeof(s_original_gamecode));
}

}  // namespace gamecode
