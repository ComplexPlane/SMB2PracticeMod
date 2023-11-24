#include "cardio.h"

#include <optional>
#include "mkb/mkb.h"
#include "systems/heap.h"
#include "systems/log.h"
#include "systems/modlink.h"
#include "utils/draw.h"

namespace cardio {

// Corresponds to CARD call we're waiting for
enum class WriteState {
    Idle,
    Probe,
    Mount,
    Create,  // If memcard file doesn't exist
    Delete,  // Else, it exists but it's too small, delete and create from scratch
    Write,
};

struct WriteParams {
    const char* file_name;
    const void* buf;
    u32 buf_size;
    void (*callback)(mkb::CARDResult);
};

// We need a 40KB(!) buffer just for the privilege of accessing memory cards, this sucks!
// Reminder we only have ~550KB to work with for the entire mod, including savestates
static void* s_card_work_area;
static mkb::CARDFileInfo s_card_file_info;

static WriteState s_state = WriteState::Idle;
static WriteParams s_write_params;                  // Current params
static std::optional<WriteParams> s_write_request;  // Params for use for next write
static u32 s_write_size;  // Sector size of memory card A which we read when probing it

static char s_orig_gamecode[6];

/*
 * Probably not Nintendo-approved hack for letting us read/write to the same savefile even if the
 * gamecode varies. Just modify the gamecode (stored at 0x80000000) to GM2E8P (vanilla SMB2) before
 * doing memcard operations!
 */

static void set_fake_gamecode() {
    mkb::memcpy(mkb::DVD_GAME_NAME, (void*)"GM2E8P", sizeof(s_orig_gamecode));
    mkb::DCFlushRange(mkb::DVD_GAME_NAME, sizeof(s_orig_gamecode));
}

static void restore_original_gamecode() {
    mkb::memcpy(mkb::DVD_GAME_NAME, s_orig_gamecode, sizeof(s_orig_gamecode));
    mkb::DCFlushRange(mkb::DVD_GAME_NAME, sizeof(s_orig_gamecode));
}

static mkb::CARDResult read_file_internal(const char* file_name, void** out_buf) {
    mkb::CARDResult res = mkb::CARD_RESULT_READY;

    // Probe card
    do {
        res = mkb::CARDProbeEx(0, nullptr, nullptr);
    } while (res == mkb::CARD_RESULT_BUSY);
    if (res != mkb::CARD_RESULT_READY) {
        return res;
    }

    // Mount card
    mkb::CARDMountAsync(0, s_card_work_area, nullptr, nullptr);
    do {
        res = mkb::CARDGetResultCode(0);
    } while (res == mkb::CARD_RESULT_BUSY);
    if (res != mkb::CARD_RESULT_READY) {
        return res;
    }

    // Open file
    res = mkb::CARDOpen(0, const_cast<char*>(file_name), &s_card_file_info);
    if (res != mkb::CARD_RESULT_READY) {
        mkb::CARDUnmount(0);
        return res;
    }

    // Get file size
    mkb::CARDStat stat;
    res = mkb::CARDGetStatus(0, s_card_file_info.fileNo, &stat);
    if (res != mkb::CARD_RESULT_READY) {
        mkb::CARDUnmount(0);
        return res;
    }

    u32 buf_size = (stat.length + mkb::CARD_READ_SIZE - 1) & ~(mkb::CARD_READ_SIZE - 1);
    void* buf = heap::alloc(buf_size);
    if (buf == nullptr) {
        // Not quite the right error (we're out of memory, not out of card space)
        mkb::CARDUnmount(0);
        return mkb::CARD_RESULT_INSSPACE;
    }

    mkb::CARDReadAsync(&s_card_file_info, buf, buf_size, 0, nullptr);
    do {
        res = mkb::CARDGetResultCode(0);
    } while (res == mkb::CARD_RESULT_BUSY);
    if (res != mkb::CARD_RESULT_READY) {
        heap::free(buf);
        mkb::CARDUnmount(0);
        return res;
    }

    *out_buf = buf;
    return mkb::CARD_RESULT_READY;
}

mkb::CARDResult read_file(const char* file_name, void** out_buf) {
    set_fake_gamecode();
    mkb::CARDResult res = read_file_internal(file_name, out_buf);
    restore_original_gamecode();
    return res;
}

void write_file(const char* file_name, const void* buf, u32 buf_size,
                void (*callback)(mkb::CARDResult)) {
    s_write_request.emplace(WriteParams{file_name, buf, buf_size, callback});
}

static bool connect_shared_work_area() {
    if (modlink::get() == nullptr) return false;
    if (modlink::get()->modlink_version.minor < 1) return false;
    if (modlink::get()->part2->card_work_area == nullptr) return false;
    s_card_work_area = modlink::get()->part2->card_work_area;
    return true;
}

void init() {
    mkb::memcpy(s_orig_gamecode, mkb::DVD_GAME_NAME, sizeof(s_orig_gamecode));
    if (!connect_shared_work_area()) {
        s_card_work_area = heap::alloc(mkb::CARD_WORKAREA_SIZE);
    }
}

static void finish_write(mkb::CARDResult res) {
    mkb::CARDUnmount(0);  // I'm assuming that trying to unmount when mounting failed is OK
    s_write_params.callback(res);
    s_state = WriteState::Idle;
    restore_original_gamecode();
}

static void tick_state_machine() {
    mkb::CARDResult res;

    switch (s_state) {
        case WriteState::Idle: {
            if (!s_write_request.has_value()) {
                break;
            }

            // Kick off write operation
            s_write_params = s_write_request.value();
            s_write_request.reset();
            set_fake_gamecode();

            // Probe and begin mounting card A
            s_state = WriteState::Probe;
            break;
        }

        case WriteState::Probe: {
            s32 sector_size;
            res = mkb::CARDProbeEx(0, nullptr, &sector_size);
            if (res == mkb::CARD_RESULT_BUSY) {
                break;
            }
            if (res != mkb::CARD_RESULT_READY) {
                finish_write(res);
                break;
            }

            s_write_size = (s_write_params.buf_size + sector_size - 1) & ~(sector_size - 1);
            mkb::CARDMountAsync(0, s_card_work_area, nullptr, nullptr);
            s_state = WriteState::Mount;
            break;
        }

        case WriteState::Mount: {
            res = mkb::CARDGetResultCode(0);
            if (res == mkb::CARD_RESULT_BUSY) {
                break;
            }
            if (res != mkb::CARD_RESULT_READY) {
                finish_write(res);
                break;
            }

            // Try to open the file
            res = mkb::CARDOpen(0, const_cast<char*>(s_write_params.file_name), &s_card_file_info);
            if (res == mkb::CARD_RESULT_READY) {
                // Check if file is too small
                mkb::CARDStat stat;
                res = mkb::CARDGetStatus(0, s_card_file_info.fileNo, &stat);
                if (res != mkb::CARD_RESULT_READY) {
                    finish_write(res);

                } else if (stat.length < s_write_size) {
                    // Recreate file
                    mkb::CARDFastDeleteAsync(0, s_card_file_info.fileNo, nullptr);
                    s_state = WriteState::Delete;

                } else {
                    // Card opened successfully, proceed directly to writing
                    mkb::CARDWriteAsync(&s_card_file_info, const_cast<void*>(s_write_params.buf),
                                        s_write_size, 0, nullptr);
                    s_state = WriteState::Write;
                }

            } else if (res == mkb::CARD_RESULT_NOFILE) {
                // Create new file
                mkb::CARDCreateAsync(0, const_cast<char*>(s_write_params.file_name), s_write_size,
                                     &s_card_file_info, nullptr);
                s_state = WriteState::Create;

            } else {
                // Some other error, fail entire write operation
                finish_write(res);
            }
            break;
        }

        case WriteState::Create: {
            res = mkb::CARDGetResultCode(0);
            if (res == mkb::CARD_RESULT_BUSY) {
                break;
            }
            if (res != mkb::CARD_RESULT_READY) {
                finish_write(res);
                break;
            }

            mkb::CARDWriteAsync(&s_card_file_info, const_cast<void*>(s_write_params.buf),
                                s_write_size, 0, nullptr);
            s_state = WriteState::Write;
            break;
        }

        case WriteState::Delete: {
            res = mkb::CARDGetResultCode(0);
            if (res == mkb::CARD_RESULT_BUSY) {
                break;
            }
            if (res != mkb::CARD_RESULT_READY) {
                finish_write(res);
                break;
            }
            mkb::CARDCreateAsync(0, const_cast<char*>(s_write_params.file_name), s_write_size,
                                 &s_card_file_info, nullptr);
            s_state = WriteState::Create;
            break;
        }

        case WriteState::Write: {
            res = mkb::CARDGetResultCode(0);
            if (res != mkb::CARD_RESULT_BUSY) {
                // Either succeeded or failed, either way we're done
                finish_write(res);
            }
            break;
        }
    }
}

void tick() {
    WriteState prev_state;
    do {
        prev_state = s_state;
        tick_state_machine();
    } while (prev_state != s_state);
}

}  // namespace cardio
