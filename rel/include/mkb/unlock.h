#pragma once

#include <cstdint>

namespace mkb
{

constexpr int CM_UNLOCK_ENTRIES_SIZE = 18;
constexpr int STORY_MODE_UNLOCK_ENTRIES_SIZE = 13;

struct UnlockInfo
{
    uint8_t master_unlocked;
    uint8_t monkeys;
    uint8_t unk_0x2;
    uint8_t staff_credits_game_unlocked;
    uint32_t play_points;
    uint32_t newest_play_point_record;
    uint8_t unk_0xc[2];
    uint16_t movies;
    uint32_t party_games; // Normally 0x0001b600 when all are unlocked
    uint16_t movies_watched;
    uint8_t unk_0x16[110];
} __attribute__((__packed__));

static_assert(sizeof(UnlockInfo) == 0x84);

extern "C"
{
extern UnlockInfo unlock_info;
extern uint8_t cm_unlock_entries[CM_UNLOCK_ENTRIES_SIZE];
extern uint8_t storymode_unlock_entries[STORY_MODE_UNLOCK_ENTRIES_SIZE];
}

}