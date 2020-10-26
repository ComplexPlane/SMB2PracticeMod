#pragma once

namespace mkb
{

constexpr s32 CM_UNLOCK_ENTRIES_SIZE = 18;
constexpr s32 STORY_MODE_UNLOCK_ENTRIES_SIZE = 13;

struct UnlockInfo
{
    u8 master_unlocked;
    u8 monkeys;
    u8 unk_0x2;
    u8 staff_credits_game_unlocked;
    u32 play_points;
    u32 newest_play_point_record;
    u8 unk_0xc[2];
    u16 movies;
    u32 party_games; // Normally 0x0001b600 when all are unlocked
    u16 movies_watched;
    u8 unk_0x16[110];
} __attribute__((__packed__));

static_assert(sizeof(UnlockInfo) == 0x84);

extern "C"
{
extern UnlockInfo unlock_info;
extern u8 cm_unlock_entries[CM_UNLOCK_ENTRIES_SIZE];
extern u8 storymode_unlock_entries[STORY_MODE_UNLOCK_ENTRIES_SIZE];
}

}