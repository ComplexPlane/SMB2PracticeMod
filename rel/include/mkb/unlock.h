#pragma once

#include <cstdint>

namespace mkb
{

constexpr int CM_UNLOCK_ENTRIES_SIZE = 18;
constexpr int STORY_MODE_UNLOCK_ENTRIES_SIZE = 13;

struct UnlockInfo
{
    uint8_t masterUnlocked;
    uint8_t monkeys;
    uint8_t unk_0x2;
    uint8_t staffCreditsGameUnlocked;
    uint32_t playPoints;
    uint32_t newestPlayPointRecord;
    uint8_t unk_0xc[2];
    uint16_t movies;
    uint32_t partyGames; // Normally 0x0001b600 when all are unlocked
    uint16_t moviesWatched;
    uint8_t unk_0x16[110];
} __attribute__((__packed__));

static_assert(sizeof(UnlockInfo) == 0x84);

extern "C"
{
extern UnlockInfo unlockInfo;
extern uint8_t cmUnlockEntries[CM_UNLOCK_ENTRIES_SIZE];
extern uint8_t storyModeUnlockEntries[STORY_MODE_UNLOCK_ENTRIES_SIZE];
}

}