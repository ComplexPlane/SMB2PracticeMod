// gci_pack.cpp
// Port of gci_pack.py — packs a raw save file into a GameCube .gci container.
//
// Usage: gci_pack <inputFile> <internalFilename> <comment1> <comment2> <bannerFile> <iconFile> <gameCode+makerCode>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        std::cerr << "Failed to open file: " << path << "\n";
        std::exit(1);
    }
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
}

void writeBE16(std::vector<uint8_t>& buf, size_t offset, uint16_t value) {
    buf[offset]     = static_cast<uint8_t>((value >> 8) & 0xFF);
    buf[offset + 1] = static_cast<uint8_t>(value & 0xFF);
}

void writeBE32(std::vector<uint8_t>& buf, size_t offset, uint32_t value) {
    buf[offset]     = static_cast<uint8_t>((value >> 24) & 0xFF);
    buf[offset + 1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    buf[offset + 2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    buf[offset + 3] = static_cast<uint8_t>(value & 0xFF);
}

// Writes `str` into buf at offset, truncated/zero-padded to `fieldLen` bytes
// (mirrors struct.pack_into("Ns", ...)).
void writeFixedString(std::vector<uint8_t>& buf, size_t offset, size_t fieldLen, const std::string& str) {
    size_t n = std::min(fieldLen, str.size());
    std::memcpy(buf.data() + offset, str.data(), n);
    // remaining bytes in the field are already zero-initialized
}

std::string replaceExtension(const std::string& path, const std::string& newExt) {
    size_t slash = path.find_last_of("/\\");
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) {
        return path + newExt;
    }
    return path.substr(0, dot) + newExt;
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 8) {
        std::cerr << "Usage: " << argv[0]
                  << " <inputFile> <internalFilename> <comment1> <comment2> <bannerFile> <iconFile> <gameCode+makerCode>\n";
        return 1;
    }

    const std::string inputFilename = argv[1];
    const std::string internalName  = argv[2];
    const std::string comment1      = argv[3];
    const std::string comment2      = argv[4];
    const std::string bannerPath    = argv[5];
    const std::string iconPath      = argv[6];
    const std::string gameCode      = argv[7];

    std::vector<uint8_t> inputBuffer = readFile(inputFilename);

    std::vector<uint8_t> bannerBuffer = readFile(bannerPath);
    if (bannerBuffer.size() != 0x1800) {
        std::cout << "Warning: banner size mismatch (should be 96x32 RGB5A3)\n";
    }

    std::vector<uint8_t> iconBuffer = readFile(iconPath);
    if (iconBuffer.size() != 0x800) {
        std::cout << "Warning: icon size mismatch " << iconBuffer.size() << " (should be 32x32 RGB5A3)\n";
    }

    // Comment
    std::vector<uint8_t> commentBuffer(0x40, 0);
    writeFixedString(commentBuffer, 0x00, 32, comment1);
    writeFixedString(commentBuffer, 0x20, 32, comment2);

    // File info
    std::vector<uint8_t> fileInfoBuffer(0x200 - 0x40, 0);
    writeBE32(fileInfoBuffer, 0, static_cast<uint32_t>(inputBuffer.size()));

    // Pad to block boundary
    size_t fileLength = bannerBuffer.size() + iconBuffer.size() + commentBuffer.size()
                       + fileInfoBuffer.size() + inputBuffer.size();
    uint32_t blockCount = static_cast<uint32_t>(std::ceil(static_cast<double>(fileLength) / 0x2000));
    size_t paddingLength = static_cast<size_t>(blockCount) * 0x2000 - fileLength;
    std::vector<uint8_t> paddingBuffer(paddingLength, 0);

    // Seconds since 2000-01-01 UTC
    using namespace std::chrono;
    const system_clock::time_point epoch2000{sys_days{2000y / January / 1}};
    const uint32_t modifiedTime = static_cast<uint32_t>(
        duration_cast<seconds>(system_clock::now() - epoch2000).count());

    // Header
    std::vector<uint8_t> headerBuffer(0x40, 0);
    writeFixedString(headerBuffer, 0x00, 6, gameCode);           // game code + maker code
    headerBuffer[0x06] = 0xFF;                                   // unused
    headerBuffer[0x07] = 2;                                      // banner flags (RGB5A3)
    writeFixedString(headerBuffer, 0x08, 32, internalName);      // filename
    writeBE32(headerBuffer, 0x28, modifiedTime);
    writeBE32(headerBuffer, 0x2C, 0);                            // image offset
    writeBE16(headerBuffer, 0x30, 2);                            // icon format
    writeBE16(headerBuffer, 0x32, 3);                            // animation speed (1 icon for 12 frames)
    headerBuffer[0x34] = 4;                                      // permissions
    headerBuffer[0x35] = 0;                                      // copy counter
    writeBE16(headerBuffer, 0x36, 0);                            // first block number
    writeBE16(headerBuffer, 0x38, static_cast<uint16_t>(blockCount)); // block count
    writeBE16(headerBuffer, 0x3A, 0xFF);                         // unused
    writeBE32(headerBuffer, 0x3C, 0x2000);                       // comment address

    std::string outputFilename = replaceExtension(inputFilename, ".gci");
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Failed to open output file: " << outputFilename << "\n";
        return 1;
    }

    outputFile.write(reinterpret_cast<const char*>(headerBuffer.data()), headerBuffer.size());
    outputFile.write(reinterpret_cast<const char*>(bannerBuffer.data()), bannerBuffer.size());
    outputFile.write(reinterpret_cast<const char*>(iconBuffer.data()), iconBuffer.size());
    outputFile.write(reinterpret_cast<const char*>(commentBuffer.data()), commentBuffer.size());
    outputFile.write(reinterpret_cast<const char*>(fileInfoBuffer.data()), fileInfoBuffer.size());
    outputFile.write(reinterpret_cast<const char*>(inputBuffer.data()), inputBuffer.size());
    outputFile.write(reinterpret_cast<const char*>(paddingBuffer.data()), paddingBuffer.size());

    return 0;
}
