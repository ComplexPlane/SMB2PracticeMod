#include "mods/unlock.h"

#include "systems/pref.h"
#include "utils/base.h"

enum { Flag_ShouldUnlock = 1 << 0 };
static u32 s_flags;

static void do_unlock(void) {
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
    mkb_g_some_gift_menu_flags |= 8;
}

void unlock_init(void) {
    char gamecode[7] = {};
    mkb_memcpy(gamecode, mkb_DVD_GAME_NAME, 6);
    if (mkb_strcmp(gamecode, (char *)"GM2E8P") == 0) {
        if (pref_get(Pref_UnlockVanilla)) s_flags |= Flag_ShouldUnlock;
    } else if (pref_get(Pref_UnlockRomhacks)) {
        s_flags |= Flag_ShouldUnlock;
    }
}

void unlock_tick(void) {
    if (s_flags & Flag_ShouldUnlock) do_unlock();
}

void unlock_unlock_everything(void) { s_flags |= Flag_ShouldUnlock; }
