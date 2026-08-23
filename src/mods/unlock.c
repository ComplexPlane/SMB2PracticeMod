#include "mods/unlock.h"

#include "systems/pref.h"
#include "utils/base.h"

enum Flags {
    Flag_ShouldUnlock = 1 << 0,
};

static u32 s_flags;

static void do_unlock() {
    // Don't yet know how to unlock the staff credits game from a fresh save...
    mkb_unlock_info.master_unlocked = true;
    mkb_unlock_info.monkeys = 99;
    mkb_unlock_info.staff_credits_game_unlocked = true;
    mkb_unlock_info.play_points = 99999;
    mkb_unlock_info.newest_play_point_record = 99999;
    mkb_unlock_info.movies = 0x0fff;
    mkb_unlock_info.party_games = 0x0001b600;
    mkb_unlock_info.g_movies_watched = 0x0fff;
    mkb_memset(mkb_cm_unlock_entries, 0xff, sizeof(mkb_cm_unlock_entries));
    mkb_memset(mkb_storymode_unlock_entries, 0xff, sizeof(mkb_storymode_unlock_entries));
    // Magic flag that allows staff credit game to be selected, ???
    mkb_g_some_gift_menu_flags |= 8;
}

void Unlock_Init() {
    // Unlock progress every frame (so it works even if a saved game is loaded), but only enact this
    // policy if the corresponding setting was enabled on startup.
    char gamecode[7] = {};
    mkb_memcpy(gamecode, mkb_DVD_GAME_NAME, 6);
    if (mkb_strcmp(gamecode, "GM2E8P") == 0) {
        if (Pref_Get(Pref_UnlockVanilla)) {
            s_flags |= Flag_ShouldUnlock;
        }
    } else if (Pref_Get(Pref_UnlockRomhacks)) {
        s_flags |= Flag_ShouldUnlock;
    }
}

void Unlock_Tick() {
    if (s_flags & Flag_ShouldUnlock) {
        do_unlock();
    }
}

void Unlock_Everything() {
    s_flags |= Flag_ShouldUnlock;
}
