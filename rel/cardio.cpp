#include "cardio.h"
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

void report(const char* format) { draw::notify(draw::ORANGE, format); }

// TODO remember to only read in multiple of CARD_READ_SIZE
Result read_file(const char* file_name, void** out_buf) { return Result::Ok; }

Result write_file(const char* file_name, const void* buf, u32 buf_size) {
    // TODO this is completely happy path, check for errors!

    // Probe and mount card
    s32 sector_size = 0;
    mkb::CARDProbeEx(0, nullptr, &sector_size);
    mkb::CARDMountAsync(0, s_card_work_area, nullptr, nullptr);

    while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
        ;
    report("CARDMountAsync result: %d\n", mkb::CARDGetResultCode(0));

    // File size must be a multiple of the sector size
    // Since our file isn't actually a multiple of the sector size, we'll just write undefined
    // memory to the remainder of the file I guess...
    s32 write_size = (buf_size + sector_size - 1) & ~(sector_size - 1);

    // Open the file, creating it if it doesn't exist or is too small
    s32 res = mkb::CARDOpen(0, const_cast<char*>(file_name), &s_card_file_info);
    //    report("CARDOpen result: %d\n", res);
    if (res == mkb::CARD_RESULT_NOFILE) {
        mkb::CARDCreateAsync(0, const_cast<char*>(file_name), write_size, &s_card_file_info,
                             nullptr);
        while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
            ;
        //        report("CARDCreateAsync result: %d\n", mkb::CARDGetResultCode(0));
    } else if (s_card_file_info.length < write_size) {
        mkb::CARDFastDeleteAsync(0, s_card_file_info.fileNo, nullptr);
        while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
            ;
        //        report("CARDFastDeleteAsync result: %d\n", mkb::CARDGetResultCode(0));
        mkb::CARDCreateAsync(0, const_cast<char*>(file_name), write_size, &s_card_file_info,
                             nullptr);
        while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
            ;
        //        report("CARDCreateAsync result: %d\n", mkb::CARDGetResultCode(0));
        mkb::CARDOpen(0, const_cast<char*>(file_name), &s_card_file_info);
    }

    mkb::CARDWriteAsync(&s_card_file_info, const_cast<void*>(buf), write_size, 0, nullptr);
    while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
        ;
    //    report("CARDWriteAsync result: %d\n", mkb::CARDGetResultCode(0));

    res = mkb::CARDUnmount(0);
    //    report("CARDUnmount result: %d\n", res);

    while (mkb::CARDGetResultCode(0) == mkb::CARD_RESULT_BUSY)
        ;

    return Result::Ok;
}

}  // namespace cardio