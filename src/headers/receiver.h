#ifndef V7_RECEIVER_H
#define V7_RECEIVER_H

#include <bitset>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <array>
#include <cstdint>
#include "Config.h"
#include "Generaldriver.h"

class Receiver {
private:
    GeneralDriver &gdrv;
    bool fileFullyReceived;
    bool lastChunk;
    int frameCounter;

    void readFrame();
    std::string readDataFrame();
    bool checkFileReceived();
    std::array <std::string,15> completed_Frames={};

    uint16_t crc16(const std::string& data);
    std::string crc16_to_string(uint16_t crc);
    std::string checkZeros(std::string data);
    uint16_t receiveChecksum();

    void handleFrame();
    void handleAckFrame();
    void handleNackFrame();
    void sync();
public:
    Receiver(GeneralDriver &gdrv);
    bool checkFlag();
    void monitor();
    void losed_packages_receive();
};

#endif //V7_RECEIVER_H