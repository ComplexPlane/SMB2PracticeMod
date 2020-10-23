#pragma once

#include <mkb/pool.h>
#include <cstdint>

namespace mkb
{

constexpr int MAX_ITEMS = 256;

enum ItemType
{
    ITEM_COIN = 0,
    ITEM_FGT_BANANA = 1,
    ITEM_MINI_RACE = 2,
    ITEM_FGT_POWER = 3,
    ITEM_FGT_PUNCH = 4,
    ITEM_PILOT = 5,
    ITEM_DOGFIGHT = 6,
    ITEM_TMP001 = 7,
    ITEM_TMP002 = 8,
    ITEM_TMP003 = 9,
    ITEM_TMP004 = 10,
    ITEM_TMP005 = 11,
    ITEM_TMP006 = 12,
    ITEM_TMP007 = 13,
    ITEM_TMP008 = 14,
    ITEM_TMP009 = 15,
    ITEM_TMP010 = 16,
    ITEM_TMP011 = 17,
    ITEM_TMP012 = 18,
    ITEM_TMP013 = 19,
    ITEM_TMP014 = 20,
    ITEM_TMP015 = 21,
    ITEM_TMP016 = 22
};

struct Item
{
    uint16_t idx;
    uint16_t id;
    uint16_t type;
    uint8_t unk_0x8[174];
};

static_assert(sizeof(Item) == 180);

extern "C"
{
extern Item items[MAX_ITEMS];
extern PoolInfo item_pool_info;
}

}