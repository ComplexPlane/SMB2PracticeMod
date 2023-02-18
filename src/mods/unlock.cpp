#include "unlock.h"

#include "mkb/mkb.h"

namespace unlock {

static void unlock_everything() {
    // Don't yet know how to unlock the staff credits game from a fresh save...
    mkb::unlock_info.master_unlocked = true;
    mkb::unlock_info.monkeys = 99;
    mkb::unlock_info.staff_credits_game_unlocked = true;
    mkb::unlock_info.play_points = 99999;
    mkb::unlock_info.newest_play_point_record = 99999;
    mkb::unlock_info.movies = 0x0fff;
    mkb::unlock_info.party_games = 0x0001b600;
    mkb::unlock_info.g_movies_watched = 0x0fff;
    mkb::memset(mkb::cm_unlock_entries, 0xff, sizeof(mkb::cm_unlock_entries));
    mkb::memset(mkb::storymode_unlock_entries, 0xff, sizeof(mkb::storymode_unlock_entries));
}

void tick() {
    
}

}
