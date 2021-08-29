#include "cardio.h"
#include <heap.h>
#include <log.h>
#include <cstdarg>
#include "draw.h"

namespace cardio {

// We need a 40KB(!) buffer just for the privilege of accessing memory cards, this sucks!
// Reminder we only have ~550KB to work with for the entire mod, including savestates
static u8 s_card_work_area[mkb::CARD_WORKAREA_SIZE] __attribute__((__aligned__(32)));
static mkb::CARDFileInfo s_card_file_info;

// TODO remove
void report(const char* format, s32 result) {
    if (result != mkb::CARD_RESULT_READY) {
        draw::notify(draw::ORANGE, format, result);
    }
}

mkb::CARDResult read_file(const char* file_name, void** out_buf) {
    mkb::CARDResult res = mkb::CARD_RESULT_READY;

    // Probe and mount card
    mkb::CARDProbeEx(0, nullptr, nullptr);
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
    void *buf = heap::alloc(buf_size);
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

mkb::CARDResult write_file(const char* file_name, const void* buf, u32 buf_size) {
    // TODO this is completely happy path, check for errors!
    // TODO do this asynchronously, it lags

    // Probe and mount card
    s32 sector_size = 0;
    mkb::CARDProbeEx(0, nullptr, &sector_size);
    mkb::CARDMountAsync(0, s_card_work_area, nullptr, nullptr);
    while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
        ;

    // File size must be a multiple of the sector size
    // Since our file isn't actually a multiple of the sector size, we'll just write undefined
    // memory to the remainder of the file I guess...
    s32 write_size = (buf_size + sector_size - 1) & ~(sector_size - 1);

    // Open the file, creating it if it doesn't exist or is too small
    s32 res = mkb::CARDOpen(0, const_cast<char*>(file_name), &s_card_file_info);
    if (res == mkb::CARD_RESULT_NOFILE) {
        mkb::CARDCreateAsync(0, const_cast<char*>(file_name), write_size, &s_card_file_info,
                             nullptr);
        while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
            ;
    } else if (s_card_file_info.length < write_size) {
        mkb::CARDFastDeleteAsync(0, s_card_file_info.fileNo, nullptr);
        while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
            ;
        mkb::CARDCreateAsync(0, const_cast<char*>(file_name), write_size, &s_card_file_info,
                             nullptr);
        while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
            ;
        mkb::CARDOpen(0, const_cast<char*>(file_name), &s_card_file_info);
    }

    mkb::CARDWriteAsync(&s_card_file_info, const_cast<void*>(buf), write_size, 0, nullptr);
    while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
        ;
    mkb::CARDUnmount(0);

    return mkb::CARD_RESULT_READY;
}

}  // namespace cardio