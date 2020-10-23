#include "savestate.h"
#include "pad.h"
#include "log.h"
#include "patch.h"
#include "memstore.h"
#include "heap.h"
#include "draw.h"

#include <mkb/mkb.h>

#include <timer.h>

namespace savestate
{

struct SaveState
{
    bool active;
    int stageId;
    memstore::MemStore memStore;
    uint8_t pauseMenuSpriteStatus;
    mkb::Sprite pauseMenuSprite;
};

static SaveState s_states[8];
static int s_activeStateSlot;

static bool s_createdStateLastFrame;
static bool s_frameAdvanceMode;

static void (*s_setMinimapMode_trampoline)(uint32_t mode);

void init()
{
    // Hook set_minimap_mode() to prevent the minimap from being hidden on goal/fallout
    // This way the minimap is unaffected when loading savestates after goal/fallout
    s_setMinimapMode_trampoline = patch::hookFunction(
        mkb::set_minimap_mode, [](uint32_t mode)
        {
            if (!(mkb::main_mode == mkb::MD_GAME
                  && mkb::main_game_mode == mkb::MGM_PRACTICE
                  && mode == mkb::MINIMAP_SHRINK))
            {
                s_setMinimapMode_trampoline(mode);
            }
        });
}

static bool isEitherTriggerHeld()
{
    return pad::analogDown(pad::AR_LTRIG) || pad::analogDown(pad::AR_RTRIG);
}

// For all memory regions that involve just saving/loading to the same region...
// Do a pass over them. This may involve preallocating a buffer to save them in, actually saving them,
// or restoring them, depending on the mode `memStore` is in
static void passOverRegions(memstore::MemStore *memStore)
{
    memStore->doRegion(&mkb::balls[0], sizeof(mkb::balls[0]));
    memStore->doRegion(&mkb::sub_mode, sizeof(mkb::sub_mode));
    memStore->doRegion(&mkb::stage_time_frames_remaining, sizeof(mkb::stage_time_frames_remaining));
    memStore->doRegion(reinterpret_cast<void *>(0x8054E03C), 0xe0); // Camera region
    memStore->doRegion(reinterpret_cast<void *>(0x805BD830), 0x1c); // Some physics region
    memStore->doRegion(&mkb::ball_mode, sizeof(mkb::ball_mode));
    memStore->doRegion(&mkb::g_standstill_camera_frame_counter, sizeof(mkb::g_standstill_camera_frame_counter));
    memStore->doRegion(mkb::balls[0].ape, sizeof(*mkb::balls[0].ape)); // Store entire ape struct for now

    // Itemgroups
    memStore->doRegion(mkb::itemgroups, sizeof(mkb::Itemgroup) * mkb::stagedef->collisionHeaderCount);

    // Bananas
    memStore->doRegion(&mkb::items, sizeof(mkb::Item) * mkb::stagedef->bananaCount);

    // Goal tape, party ball, and button stage objects
    for (uint32_t i = 0; i < mkb::stobj_pool_info.upperBound; i++)
    {
        if (mkb::stobj_pool_info.statusList[i] == 0) continue;

        switch (mkb::stage_objects[i].type)
        {
            case mkb::STOBJ_GOALTAPE:
            case mkb::STOBJ_GOALBAG:
            case mkb::STOBJ_GOALBAG_EXMASTER:
            case mkb::STOBJ_BUTTON:
            {
                memStore->doRegion(&mkb::stage_objects[i], sizeof(mkb::stage_objects[i]));
                break;
            }
        }
    }

    // Seesaws
    for (uint32_t i = 0; i < mkb::stagedef->collisionHeaderCount; i++)
    {
        if (mkb::stagedef->collisionHeaderList[i].animLoopTypeAndSeesaw == mkb::ANIM_SEESAW)
        {
            memStore->doRegion(mkb::itemgroups[i].seesawInfo->state, 12);
        }
    }

    // Goal tape and party ball-specific extra data
    memStore->doRegion(mkb::goaltapes, sizeof(mkb::GoalTape) * mkb::stagedef->goalCount);
    memStore->doRegion(mkb::goalbags, sizeof(mkb::GoalBag) * mkb::stagedef->goalCount);

    // Pause menu
    memStore->doRegion(reinterpret_cast<void *>(0x8054DCA8), 56); // Pause menu state
    memStore->doRegion(reinterpret_cast<void *>(0x805BC474), 4); // Pause menu bitfield

    for (uint32_t i = 0; i < mkb::sprite_pool_info.upperBound; i++)
    {
        if (mkb::sprite_pool_info.statusList[i] == 0) continue;
        mkb::Sprite *sprite = &mkb::sprites[i];

        if (sprite->tickFunc == mkb::sprite_timer_ball_tick)
        {
            // Timer ball sprite (it'll probably always be in the same place in the sprite array)
            memStore->doRegion(sprite, sizeof(*sprite));
        }
        else if (sprite->tickFunc == mkb::sprite_score_tick)
        {
            // Score sprite's lerped score value
            memStore->doRegion(&sprite->lerpValue, sizeof(sprite->lerpValue));
        }
    }

    // RTA timer
    timer::saveState(memStore);
}

static void handlePauseMenuSave(SaveState *state)
{
    state->pauseMenuSpriteStatus = 0;

    // Look for an active sprite that has the same dest func pointer as the pause menu sprite
    for (uint32_t i = 0; i < mkb::sprite_pool_info.upperBound; i++)
    {
        if (mkb::sprite_pool_info.statusList[i] == 0) continue;

        mkb::Sprite &sprite = mkb::sprites[i];
        if (sprite.dispFunc == mkb::sprite_pausemenu_disp)
        {
            state->pauseMenuSpriteStatus = mkb::sprite_pool_info.statusList[i];
            state->pauseMenuSprite = sprite;

            break;
        }
    }
}

static void handlePauseMenuLoad(SaveState *state)
{
    bool pausedNow = *reinterpret_cast<uint32_t *>(0x805BC474) & 8; // TODO actually give this a name
    bool pausedInState = state->pauseMenuSpriteStatus != 0;

    if (pausedNow && !pausedInState)
    {
        // Destroy the pause menu sprite that currently exists
        for (uint32_t i = 0; i < mkb::sprite_pool_info.upperBound; i++)
        {
            if (mkb::sprite_pool_info.statusList[i] == 0) continue;

            if (reinterpret_cast<uint32_t>(mkb::sprites[i].dispFunc) == 0x8032a4bc)
            {
                mkb::sprite_pool_info.statusList[i] = 0;
                break;
            }
        }
    }
    else if (!pausedNow && pausedInState)
    {
        // Allocate a new pause menu sprite
        int i = mkb::pool_alloc(&mkb::sprite_pool_info, state->pauseMenuSpriteStatus);
        mkb::sprites[i] = state->pauseMenuSprite;
    }
}

static void destructPostGoalSprites()
{
    for (uint32_t i = 0; i < mkb::sprite_pool_info.upperBound; i++)
    {
        if (mkb::sprite_pool_info.statusList[i] == 0) continue;

        mkb::Sprite *sprite = &mkb::sprites[i];
        bool postGoalSprite = (
            sprite->dispFunc == mkb::sprite_goal_disp
            || sprite->dispFunc == mkb::sprite_clear_score_disp
            || sprite->dispFunc == mkb::sprite_warp_bonus_disp
            || sprite->dispFunc == mkb::sprite_time_bonus_disp
            || sprite->dispFunc == mkb::sprite_stage_score_disp
            || sprite->tickFunc == mkb::sprite_fallout_tick
            || sprite->tickFunc == mkb::sprite_bonus_finish_or_perfect_tick);
        if (postGoalSprite) mkb::sprite_pool_info.statusList[i] = 0;
    }
}

static void destructDistractingEffects()
{
    // Destruct current spark effects so we don't see big sparks
    // generated when changing position by a large amount.
    // Also destruct banana grabbing effects
    for (uint32_t i = 0; i < mkb::effect_pool_info.upperBound; i++)
    {
        if (mkb::effect_pool_info.statusList[i] == 0) continue;

        switch (mkb::effects[i].type)
        {
            case mkb::EFFECT_COLI_PARTICLE:
            case mkb::EFFECT_HOLDING_BANANA:
            case mkb::EFFECT_GET_BANANA:
            {
                mkb::effect_pool_info.statusList[i] = 0;
            }
        }
    }
}

static void preventReplays()
{
    // Prevent replays from playing in goal and fallout submodes by locking initial submode frame counter
    switch (mkb::sub_mode)
    {
        case mkb::SMD_GAME_GOAL_MAIN:
        {
            // Just prevent the timer from running out completely, so that the GOAL sound plays
            if (mkb::sub_mode_frame_counter == 1) mkb::sub_mode_frame_counter = 2;
            break;
        }
        case mkb::SMD_GAME_RINGOUT_MAIN:
        {
            mkb::sub_mode_frame_counter = 270;
            break;
        }
        case mkb::SMD_GAME_TIMEOVER_MAIN:
        {
            mkb::sub_mode_frame_counter = 120;
            break;
        }
    }
}

void tick()
{
    if (!isEitherTriggerHeld())
    {
        s_frameAdvanceMode = false;
    }

    // Must be in main game
    if (mkb::main_mode != mkb::MD_GAME) return;

    // Allow changing the savestate slot as long as the above conditions are at least met
    int cStickDir = pad::getCStickDir();
    if (cStickDir != pad::DIR_NONE)
    {
        s_activeStateSlot = cStickDir;
        draw::notify(draw::Color::WHITE, "Slot %d Selected", cStickDir + 1);
    }
    auto &state = s_states[s_activeStateSlot];

    preventReplays();

    if (pad::buttonPressed(pad::BUTTON_X))
    {
        if (mkb::sub_mode != mkb::SMD_GAME_PLAY_MAIN || mkb::sub_mode_request != mkb::SMD_INVALID)
        {
            if (mkb::sub_mode == mkb::SMD_GAME_RINGOUT_INIT || mkb::sub_mode == mkb::SMD_GAME_RINGOUT_MAIN)
            {
                draw::notify(draw::Color::RED, "Cannot Create Savestate After Fallout");
            }
            else if (mkb::sub_mode == mkb::SMD_GAME_GOAL_INIT || mkb::sub_mode == mkb::SMD_GAME_GOAL_MAIN)
            {
                draw::notify(draw::Color::RED, "Cannot Create Savestate After Goal");
            }
            else if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)
            {
                draw::notify(draw::Color::RED, "Cannot Create Savestate During Retry");
            }
            else if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN)
            {
                draw::notify(draw::Color::RED, "Cannot Create Savestate After Timeout");
            }
            else
            {
                draw::notify(draw::Color::RED, "Cannot Create Savestate Here");
            }

            return;
        }

        if (mkb::events[mkb::EVENT_VIEW].status != mkb::STAT_NULL)
        {
            draw::notify(draw::Color::RED, "Cannot Create Savestate in View Stage");
            return;
        }

        // Test that there is enough memory to create state
        // TODO use a scratch savestate instead of obliterating whichever slot was currently selected?
        state.memStore.enterPreallocMode();
        passOverRegions(&state.memStore);
        if (!state.memStore.enterSaveMode())
        {
            draw::notify(draw::Color::RED, "Cannot Create Savestate: Out of Memory");
            state.active = false;
            return;
        }

        s_createdStateLastFrame = true;
        state.active = true;
        state.stageId = mkb::current_stage_id;
        passOverRegions(&state.memStore);

        handlePauseMenuSave(&state);

        // TODO allow entering frame advance by pressing L/R while holding X in load-state mode
        s_frameAdvanceMode = isEitherTriggerHeld();

        gc::OSReport("[mod] Saved state:\n");
        state.memStore.printStats();
        size_t freeHeapSpace = heap::getFreeSpace();
        gc::OSReport("[mod] Heap free:        %d bytes\n", freeHeapSpace);
        gc::OSReport("[mod] Heap used:        %d bytes\n", heap::HEAP_SIZE - freeHeapSpace);
        gc::OSReport("[mod] Heap total space: %d bytes\n", heap::HEAP_SIZE);

        if (s_frameAdvanceMode)
        {
            draw::notify(draw::Color::PINK, "Slot %d Frame Advance", s_activeStateSlot + 1);
        }
        else
        {
            draw::notify(draw::Color::PINK, "Slot %d Saved", s_activeStateSlot + 1);
        }
    }
    else if (
        pad::buttonDown(pad::BUTTON_Y)
        || (pad::buttonDown(pad::BUTTON_X)
            && s_createdStateLastFrame)
        || s_frameAdvanceMode
        || (isEitherTriggerHeld() && cStickDir != pad::DIR_NONE))
    {
        if (mkb::sub_mode == mkb::SMD_GAME_READY_INIT || mkb::sub_mode == mkb::SMD_GAME_READY_MAIN)
        {
            draw::notify(draw::Color::RED, "Cannot Load Savestate During Retry");
            return;
        }
        if (mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_INIT || mkb::sub_mode == mkb::SMD_GAME_TIMEOVER_MAIN)
        {
            draw::notify(draw::Color::RED, "Cannot Load Savestate After Timeout");
            return;
        }
        if (!state.active)
        {
            draw::notify(draw::Color::RED, "Slot %d Empty", s_activeStateSlot + 1);
            return;
        }
        if (state.stageId != mkb::current_stage_id)
        {
            draw::notify(draw::Color::RED, "Slot %d Wrong Stage", s_activeStateSlot + 1);
            return;
        }
        if (mkb::events[mkb::EVENT_VIEW].status != mkb::STAT_NULL)
        {
            draw::notify(draw::Color::RED, "Cannot Load Savestate in View Stage");
            return;
        }

        // Need to handle pausemenu-specific loading first so we can detect the game isn't currently paused
        handlePauseMenuLoad(&state);

        state.memStore.enterLoadMode();
        passOverRegions(&state.memStore);

        destructPostGoalSprites();
        destructDistractingEffects();

        if (!s_createdStateLastFrame)
        {
            draw::notify(draw::Color::BLUE, "Slot %d Loaded", s_activeStateSlot + 1);
        }
    }
    else
    {
        s_createdStateLastFrame = false;
    }
}

}