#include "systems/cardio.h"

#include "systems/heap.h"
#include "systems/log.h"
#include "systems/modlink.h"
#include "utils/draw.h"

// Corresponds to CARD call we're waiting for
typedef enum {
    WriteState_Idle,
    WriteState_Probe,
    WriteState_Mount,
    WriteState_Create,  // If memcard file doesn't exist
    WriteState_Delete,  // Else, it exists but it's too small, delete and create from scratch
    WriteState_Write,
} WriteState;

typedef struct {
    const char *file_name;
    const void *buf;
    u32 buf_size;
    void (*callback)(mkb_CARDResult);
} WriteParams;

// We need a 40KB(!) buffer just for the privilege of accessing memory cards, this sucks!
// Reminder we only have ~550KB to work with for the entire mod, including savestates
static void *s_card_work_area;
static mkb_CARDFileInfo s_card_file_info;

static WriteState s_state = WriteState_Idle;
static WriteParams s_write_params;  // Current params
static WriteParams s_write_request;
static bool s_write_request_valid;  // Params for use for next write
static u32 s_write_size;            // Sector size of memory card A which we read when probing it

static char s_orig_gamecode[6];

/*
 * Probably not Nintendo-approved hack for letting us read/write to the same savefile even if the
 * gamecode varies. Just modify the gamecode (stored at 0x80000000) to GM2E8P (vanilla SMB2) before
 * doing memcard operations!
 */

static void set_fake_gamecode() {
    mkb_memcpy(mkb_DVD_GAME_NAME, (void *)"GM2E8P", sizeof(s_orig_gamecode));
    mkb_DCFlushRange(mkb_DVD_GAME_NAME, sizeof(s_orig_gamecode));
}

static void restore_original_gamecode() {
    mkb_memcpy(mkb_DVD_GAME_NAME, s_orig_gamecode, sizeof(s_orig_gamecode));
    mkb_DCFlushRange(mkb_DVD_GAME_NAME, sizeof(s_orig_gamecode));
}

static mkb_CARDResult read_file_internal(const char *file_name, void **out_buf) {
    mkb_CARDResult res = mkb_CARD_RESULT_READY;

    // Probe card
    do {
        res = mkb_CARDProbeEx(0, nullptr, nullptr);
    } while (res == mkb_CARD_RESULT_BUSY);
    if (res != mkb_CARD_RESULT_READY) {
        return res;
    }

    // Mount card
    mkb_CARDMountAsync(0, s_card_work_area, nullptr, nullptr);
    do {
        res = mkb_CARDGetResultCode(0);
    } while (res == mkb_CARD_RESULT_BUSY);
    if (res != mkb_CARD_RESULT_READY) {
        return res;
    }

    // Open file
    res = mkb_CARDOpen(0, (char *)file_name, &s_card_file_info);
    if (res != mkb_CARD_RESULT_READY) {
        mkb_CARDUnmount(0);
        return res;
    }

    // Get file size
    mkb_CARDStat stat;
    res = mkb_CARDGetStatus(0, s_card_file_info.fileNo, &stat);
    if (res != mkb_CARD_RESULT_READY) {
        mkb_CARDUnmount(0);
        return res;
    }

    u32 buf_size = (stat.length + mkb_CARD_READ_SIZE - 1) & ~(mkb_CARD_READ_SIZE - 1);
    void *buf = Heap_Alloc(buf_size);
    if (buf == nullptr) {
        // Not quite the right error (we're out of memory, not out of card space)
        mkb_CARDUnmount(0);
        return mkb_CARD_RESULT_INSSPACE;
    }

    mkb_CARDReadAsync(&s_card_file_info, buf, buf_size, 0, nullptr);
    do {
        res = mkb_CARDGetResultCode(0);
    } while (res == mkb_CARD_RESULT_BUSY);
    if (res != mkb_CARD_RESULT_READY) {
        Heap_Free(buf);
        mkb_CARDUnmount(0);
        return res;
    }

    *out_buf = buf;
    return mkb_CARD_RESULT_READY;
}

mkb_CARDResult Card_ReadFile(const char *file_name, void **out_buf) {
    set_fake_gamecode();
    mkb_CARDResult res = read_file_internal(file_name, out_buf);
    restore_original_gamecode();
    return res;
}

void Card_WriteFile(const char *file_name,
                    const void *buf,
                    u32 buf_size,
                    void (*callback)(mkb_CARDResult)) {
    s_write_request = (WriteParams){file_name, buf, buf_size, callback};
    s_write_request_valid = true;
}

static bool connect_shared_work_area() {
    if (ModLink_Get() == nullptr) return false;
    if (ModLink_Get()->modlink_version.minor < 1) return false;
    if (ModLink_Get()->part2->card_work_area == nullptr) return false;
    s_card_work_area = ModLink_Get()->part2->card_work_area;
    return true;
}

void Card_Init() {
    mkb_memcpy(s_orig_gamecode, mkb_DVD_GAME_NAME, sizeof(s_orig_gamecode));
    if (!connect_shared_work_area()) {
        s_card_work_area = Heap_Alloc(mkb_CARD_WORKAREA_SIZE);
    }
}

static void finish_write(mkb_CARDResult res) {
    mkb_CARDUnmount(0);  // I'm assuming that trying to unmount when mounting failed is OK
    s_write_params.callback(res);
    s_state = WriteState_Idle;
    restore_original_gamecode();
}

static void tick_state_machine() {
    switch (s_state) {
        case WriteState_Idle: {
            if (!s_write_request_valid) {
                break;
            }

            // Kick off write operation
            s_write_params = s_write_request;
            s_write_request_valid = false;
            set_fake_gamecode();

            // Probe and begin mounting card A
            s_state = WriteState_Probe;
            break;
        }

        case WriteState_Probe: {
            s32 sector_size;
            mkb_CARDResult res = mkb_CARDProbeEx(0, nullptr, &sector_size);
            if (res == mkb_CARD_RESULT_BUSY) {
                break;
            }
            if (res != mkb_CARD_RESULT_READY) {
                finish_write(res);
                break;
            }

            s_write_size = (s_write_params.buf_size + sector_size - 1) & ~(sector_size - 1);
            mkb_CARDMountAsync(0, s_card_work_area, nullptr, nullptr);
            s_state = WriteState_Mount;
            break;
        }

        case WriteState_Mount: {
            mkb_CARDResult res = mkb_CARDGetResultCode(0);
            if (res == mkb_CARD_RESULT_BUSY) {
                break;
            }
            if (res != mkb_CARD_RESULT_READY) {
                finish_write(res);
                break;
            }

            // Try to open the file
            res = mkb_CARDOpen(0, (char *)s_write_params.file_name, &s_card_file_info);
            if (res == mkb_CARD_RESULT_READY) {
                // Check if file is too small
                mkb_CARDStat stat;
                res = mkb_CARDGetStatus(0, s_card_file_info.fileNo, &stat);
                if (res != mkb_CARD_RESULT_READY) {
                    finish_write(res);

                } else if (stat.length < s_write_size) {
                    // Recreate file
                    mkb_CARDFastDeleteAsync(0, s_card_file_info.fileNo, nullptr);
                    s_state = WriteState_Delete;

                } else {
                    // Card opened successfully, proceed directly to writing
                    mkb_CARDWriteAsync(&s_card_file_info, (void *)s_write_params.buf, s_write_size,
                                       0, nullptr);
                    s_state = WriteState_Write;
                }

            } else if (res == mkb_CARD_RESULT_NOFILE) {
                // Create new file
                mkb_CARDCreateAsync(0, (char *)s_write_params.file_name, s_write_size,
                                    &s_card_file_info, nullptr);
                s_state = WriteState_Create;

            } else {
                // Some other error, fail entire write operation
                finish_write(res);
            }
            break;
        }

        case WriteState_Create: {
            mkb_CARDResult res = mkb_CARDGetResultCode(0);
            if (res == mkb_CARD_RESULT_BUSY) {
                break;
            }
            if (res != mkb_CARD_RESULT_READY) {
                finish_write(res);
                break;
            }

            mkb_CARDWriteAsync(&s_card_file_info, (void *)s_write_params.buf, s_write_size, 0,
                               nullptr);
            s_state = WriteState_Write;
            break;
        }

        case WriteState_Delete: {
            mkb_CARDResult res = mkb_CARDGetResultCode(0);
            if (res == mkb_CARD_RESULT_BUSY) {
                break;
            }
            if (res != mkb_CARD_RESULT_READY) {
                finish_write(res);
                break;
            }
            mkb_CARDCreateAsync(0, (char *)s_write_params.file_name, s_write_size,
                                &s_card_file_info, nullptr);
            s_state = WriteState_Create;
            break;
        }

        case WriteState_Write: {
            mkb_CARDResult res = mkb_CARDGetResultCode(0);
            if (res != mkb_CARD_RESULT_BUSY) {
                // Either succeeded or failed, either way we're done
                finish_write(res);
            }
            break;
        }
    }
}

void Card_Tick() {
    WriteState prev_state = WriteState_Idle;
    do {
        prev_state = s_state;
        tick_state_machine();
    } while (prev_state != s_state);
}
