#include "unlock.h"

#include "mkb/mkb.h"
#include "mkb/mkb2_ghidra.h"
#include "systems/pref.h"

namespace unlock {

namespace Flags {
enum {
    ShouldUnlock = 1 << 0,
};
}

static u32 s_flags;

static void do_unlock() {
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
    // Magic flag that allows staff credit game to be selected, ???
    mkb::g_some_gift_menu_flags |= 8;
}

void init() {
    // Unlock progress every frame (so it works even if a saved game is loaded), but only enact this
    // policy if the corresponding setting was enabled on startup.
    char gamecode[7] = {};
    mkb::memcpy(gamecode, mkb::DVD_GAME_NAME, 6);
    if (mkb::strcmp(gamecode, "GM2E8P") == 0) {
        if (pref::get(pref::BoolPref::UnlockVanilla)) {
            s_flags |= Flags::ShouldUnlock;
        }
    } else if (pref::get(pref::BoolPref::UnlockRomhacks)) {
        s_flags |= Flags::ShouldUnlock;
    }
}

void tick() {
    if (s_flags & Flags::ShouldUnlock) {
        do_unlock();
    }
}

void unlock_everything() { s_flags |= Flags::ShouldUnlock; }

}  // namespace unlock
