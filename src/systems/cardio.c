#include "cardio.h"

#include "systems/heap.h"

// Corresponds to CARD call we're waiting for
typedef enum WriteState WriteState;
enum WriteState {
    WriteState_Idle,
    WriteState_Mount,
    WriteState_Create,  // If memcard file doesn't exist
    WriteState_Delete,  // Else, it exists but it's too small, delete and create from scratch
    WriteState_Write,
};

typedef struct WriteRequest WriteRequest;
struct WriteRequest {
    const char *file_name;
    const void *buf;
    u32 buf_size;
    void (*callback)(mkb_CARDResult);
};

// We need a 40KB(!) buffer just for the privilege of accessing memory cards, this sucks!
// Reminder we only have ~550KB to work with for the entire mod, including savestates
static u8 s_card_work_area[mkb_CARD_WORKAREA_SIZE] __attribute__((__aligned__(32)));
static mkb_CARDFileInfo s_card_file_info;

static WriteState s_state = WriteState_Idle;
static WriteRequest s_curr_write;  // Current params
static WriteRequest s_next_write;  // Params for use for next write
static bool s_write_requested;
static u32 s_write_size;  // Sector size of memory card A which we read when probing it

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

    // Probe and mount card
    mkb_CARDProbeEx(0, nullptr, nullptr);
    mkb_CARDMountAsync(0, s_card_work_area, nullptr, nullptr);
    do {
        res = mkb_CARDGetResultCode(0);
    } while (res == mkb_CARD_RESULT_BUSY);
    if (res != mkb_CARD_RESULT_READY) {
        return res;
    }

    // Open file
    res = mkb_CARDOpen(0, (char *)(file_name), &s_card_file_info);
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
    void *buf = heap_alloc(buf_size);
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
        heap_free(buf);
        mkb_CARDUnmount(0);
        return res;
    }

    *out_buf = buf;
    return mkb_CARD_RESULT_READY;
}

mkb_CARDResult read_file(const char *file_name, void **out_buf) {
    set_fake_gamecode();
    mkb_CARDResult res = read_file_internal(file_name, out_buf);
    restore_original_gamecode();
    return res;
}

void write_file(const char *file_name,
                const void *buf,
                u32 buf_size,
                void (*callback)(mkb_CARDResult)) {
    s_next_write = (WriteRequest){
        .file_name = file_name,
        .buf = buf,
        .buf_size = buf_size,
        .callback = callback,
    };
}

void init() {
    mkb_memcpy(s_orig_gamecode, mkb_DVD_GAME_NAME, sizeof(s_orig_gamecode));
}

static void finish_write(mkb_CARDResult res) {
    mkb_CARDUnmount(0);  // I'm assuming that trying to unmount when mounting failed is OK
    s_curr_write.callback(res);
    s_state = WriteState_Idle;
    restore_original_gamecode();
}

void tick() {
    mkb_CARDResult res;

    switch (s_state) {
    case WriteState_Idle: {
        if (s_write_requested) {
            // Kick off write operation
            s_curr_write = s_next_write;
            s_write_requested = false;
            set_fake_gamecode();

            // Probe and begin mounting card A
            s32 sector_size;
            mkb_CARDProbeEx(0, nullptr, &sector_size);
            s_write_size = (s_curr_write.buf_size + sector_size - 1) & ~(sector_size - 1);
            mkb_CARDMountAsync(0, s_card_work_area, nullptr, nullptr);
            s_state = WriteState_Mount;
        }
        break;
    }

    case WriteState_Mount: {
        res = mkb_CARDGetResultCode(0);
        if (res != mkb_CARD_RESULT_BUSY) {
            if (res == mkb_CARD_RESULT_READY) {
                // Try to open the file
                res = mkb_CARDOpen(0, (char *)(s_curr_write.file_name), &s_card_file_info);
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
                        mkb_CARDWriteAsync(&s_card_file_info, (char *)(s_curr_write.buf),
                                           s_write_size, 0, nullptr);
                        s_state = WriteState_Write;
                    }

                } else if (res == mkb_CARD_RESULT_NOFILE) {
                    // Create new file
                    mkb_CARDCreateAsync(0, (char *)(s_curr_write.file_name), s_write_size,
                                        &s_card_file_info, nullptr);
                    s_state = WriteState_Create;

                } else {
                    // Some other error, fail entire write operation
                    finish_write(res);
                }

            } else {
                // Error mounting
                finish_write(res);
            }
        }
        break;
    }

    case WriteState_Create: {
        res = mkb_CARDGetResultCode(0);
        if (res != mkb_CARD_RESULT_BUSY) {
            if (res == mkb_CARD_RESULT_READY) {
                mkb_CARDWriteAsync(&s_card_file_info, (char *)(s_curr_write.buf), s_write_size, 0,
                                   nullptr);
                s_state = WriteState_Write;
            } else {
                finish_write(res);
            }
        }
        break;
    }

    case WriteState_Delete: {
        res = mkb_CARDGetResultCode(0);
        if (res != mkb_CARD_RESULT_BUSY) {
            if (res == mkb_CARD_RESULT_READY) {
                mkb_CARDCreateAsync(0, (char *)(s_curr_write.file_name), s_write_size,
                                    &s_card_file_info, nullptr);
                s_state = WriteState_Create;
            } else {
                finish_write(res);
            }
        }
        break;
    }

    case WriteState_Write: {
        res = mkb_CARDGetResultCode(0);
        if (res != mkb_CARD_RESULT_BUSY) {
            // Either succeeded or failed, either way we're done
            finish_write(res);
        }
        break;
    }
    }
}
