use crate::mkb;
use crate::systems::pref::{BoolPref, Pref};
use crate::utils::misc::for_c_arr;

pub struct Unlock {
    should_unlock: bool,
}

impl Unlock {
    pub fn new(pref: &Pref) -> Self {
        let mut unlock = Self {
            should_unlock: false,
        };
        // Unlock progress every frame (so it works even if a saved game is loaded), but only enact this
        // policy if the corresponding setting was enabled on startup.
        unsafe {
            let game_matches =
                mkb::DVD_GAME_NAME == [b'G' as i8, b'M' as i8, b'2' as i8, b'E' as i8];
            let company_matches = mkb::DVD_COMPANY == [b'8' as i8, b'P' as i8];
            if game_matches && company_matches {
                if pref.get_bool(BoolPref::UnlockVanilla) {
                    unlock.should_unlock = true;
                }
            } else if pref.get_bool(BoolPref::UnlockRomhacks) {
                unlock.should_unlock = true;
            }
        }

        unlock
    }

    fn do_unlock() {
        // Don't yet know how to unlock the staff credits game from a fresh save...
        unsafe {
            mkb::unlock_info.master_unlocked = 1;
            mkb::unlock_info.monkeys = 99;
            mkb::unlock_info.staff_credits_game_unlocked = 1;
            mkb::unlock_info.play_points = 99999;
            mkb::unlock_info.newest_play_point_record = 99999;
            mkb::unlock_info.movies = 0x0fff;
            mkb::unlock_info.party_games = 0x0001b600;
            mkb::unlock_info.g_movies_watched = 0x0fff;

            for_c_arr(&raw mut mkb::cm_unlock_entries, |v| {
                *v = 0xff;
            });
            for_c_arr(&raw mut mkb::storymode_unlock_entries, |v| {
                *v = 0xff;
            });

            // Magic flag that allows staff credit game to be selected, ???
            mkb::g_some_gift_menu_flags |= 8;
        }
    }

    pub fn tick(&self) {
        if self.should_unlock {
            Self::do_unlock();
        }
    }

    pub fn unlock_everything(&mut self) {
        self.should_unlock = true;
    }
}