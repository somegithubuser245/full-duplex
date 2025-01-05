#ifndef V7_CHECKSUM_H
#define V7_CHECKSUM_H

#include <iomanip>

class Checksum {
public:
    static uint16_t crc16(const std::string &data);
    static std::string crc16_to_string(uint16_t crc);
};

#endif //V7_CHECKSUM_H