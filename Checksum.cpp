#include "headers/Checksum.h"

uint16_t Checksum::crc16(const std::string& data) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < data.size(); ++i) {
        crc ^= static_cast<uint8_t>(data[i]) << 8;
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;  // Polynomial used in CRC16-CCITT
            else
                crc <<= 1;
        }
    }
    return crc;
}

std::string Checksum::crc16_to_string(uint16_t crc) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << crc;
    return ss.str();
}